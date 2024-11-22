import os
from typing import Union, List, Tuple
import subprocess
import logging
from pathlib import Path


def get_mzn_files(input_root: str, files: List[str]) -> list:
    return [os.path.join(input_root, f)
            for f in files if os.path.splitext(f)[1] == '.mzn']


def get_data_files(input_root: str, dirs: List[str],
                   root_files: List[str]) -> list:
    dzn_files = set([os.path.join(input_root, f) for f in root_files
                     if os.path.splitext(f)[1] in {'.dzn', '.json'}])
    if input_root not in dirs:
        dirs = dirs + [input_root]
    for directory in dirs + [input_root]:
        for cur_dir, dirs, files in os.walk(directory, topdown=False):
            for f in files:
                if os.path.splitext(f)[1] in {'.dzn', '.json'}:
                    dzn_files.add(os.path.join(cur_dir, f))
    return dzn_files


def find_minizinc() -> str:
    return subprocess.check_output(
        ['which', 'minizinc']).decode('utf-8').strip()


def make_output_dir(output_root: str) -> None:
    os.makedirs(output_root, exist_ok=True)


class Flatten:
    _input_dir: str = None
    _output_dir: str = None
    _minizinc_path: str = None
    _logger: logging.Logger = None
    _solver: str = 'gecode'
    _timeout = 60  # timeout for the minizinc flattening in seconds

    def __init__(self, input_dir: str, output_dir: str):
        self._input_dir = os.path.abspath(input_dir)
        self._output_dir = os.path.abspath(output_dir)
        self._minizinc_path = find_minizinc()
        self._logger = logging.getLogger(self.__class__.__name__)
        self._logger.debug(f'input dir: {self._input_dir}')
        self._logger.debug(f'output dir: {self._output_dir}')
        self._logger.debug(f'minizinc path: {self._minizinc_path}')
        atlantis_msc = os.path.join(str(Path.home()), 'cbls', 'build', 'atlantis.msc')
        if os.path.isfile(atlantis_msc):
            self._solver = atlantis_msc
        else:
            self._solver = 'gecode'

    def rel_path(self, file_path: str,
                 prefix_path: Union[str, None]=None) -> str:
        if prefix_path is None:
            prefix_path = self._input_dir
        assert file_path.startswith(prefix_path)
        return file_path[len(prefix_path):].lstrip('/')

    def find_output_dir(self, input_root: str) -> str:
        assert input_root.startswith(self._input_dir)
        return os.path.join(
            self._output_dir, input_root[len(self._input_dir):].lstrip('/'))

    def flatten_file(self, input_root: str, mzn_file: str,
                     data_file: Union[None, str], logging_suffix: str) -> None:
        params = [
            self._minizinc_path, '-c', mzn_file,
            '--solver', self._solver, '--use-gecode']
        output_root = self.find_output_dir(input_root)
        self._logger.debug(f'output root: {output_root}')
        make_output_dir(output_root)
        mzn_filename, _ = os.path.splitext(mzn_file)
        _, mzn_filename = os.path.split(mzn_filename)
        if data_file is not None:
            params.append(data_file)
            data_file_name, _ = os.path.splitext(data_file)
            _, data_file_name = os.path.split(data_file_name)
            output_file_name = os.path.join(
                output_root, mzn_filename + '_' + data_file_name)
            fzn_file = output_file_name + '.fzn'
        else:
            output_file_name = os.path.join(output_root, mzn_filename)
            fzn_file = output_file_name + '.fzn'
        if os.path.exists(fzn_file):
            self._logger.debug(f'{self.rel_path(fzn_file, self._output_dir)} '
                               'already exists, skipping')
            return
        self._logger.debug(
            f'flattening {self.rel_path(mzn_file)}' +
            ("" if data_file is None else f' with {self.rel_path(data_file)}') +
            f' into {self.rel_path(fzn_file, self._output_dir)}')
        params.extend(['--fzn', fzn_file, '--no-output-ozn'])
        log_file = output_file_name + '.log'
        try:
            result = subprocess.run(params, stdout=subprocess.PIPE,
                                    stderr=subprocess.PIPE, text=True,
                                    timeout=60)
            if os.path.exists(log_file):
                os.remove(log_file)
            result.check_returncode()
        except subprocess.CalledProcessError as e:
            self._logger.error(
              f'failed to flatten {self.rel_path(mzn_file)} ' +
              ("" if data_file is None else
               f'with {self.rel_path(data_file)}'))
            self._logger.error(f'error: {e}')
            if e.stdout is not None and len(e.stdout) > 0:
                self._logger.error(f'stdout: {e.stdout}')
            if e.stderr is not None and len(e.stderr) > 0:
                with open(log_file, 'w') as f:
                    f.write(e.stderr)
            return
        except subprocess.TimeoutExpired as e:
            self._logger.error(
                f'timeout when flattening {self.rel_path(mzn_file)} with '
                f'{self.rel_path(data_file)}')
            self._logger.error(f'error: {e}')
            if e.stdout is not None and len(e.stdout) > 0:
                self._logger.error(f'stdout: {e.stdout}')
            if e.stderr is not None and len(e.stderr) > 0:
                with open(log_file, 'w') as f:
                    f.write(e.stderr)
            return
        self._logger.info(
          f'created {self.rel_path(fzn_file, self._output_dir)} '
          f'{logging_suffix}')

    def flatten_all(self):
        inputs: List[Tuple[str, str, Union[str, None]]] = []
        for input_root, dirs, files in sorted(os.walk(self._input_dir, topdown=False)):
            dirs.sort()
            files.sort()
            
            mzn_files = get_mzn_files(input_root, files)
            if len(mzn_files) == 0:
                continue
            self._logger.debug(
              f'in root: {self.rel_path(input_root)}, found {len(mzn_files)}' +
              ' mzn file(s): ' + 
              ', '.join([self.rel_path(mf, input_root) for mf in mzn_files]))
            data_files = get_data_files(input_root, dirs, files)
            if len(data_files) == 0 and len(dirs) > 0:
                data_dir = (dirs[0] if len(dirs) == 1 else
                            next((d for d in dirs if d == 'data'), None))
                if data_dir is not None:
                    data_files = get_data_files(
                        os.path.join(input_root, data_dir),
                        os.listdir(os.path.join(input_root, data_dir)))
                
            self._logger.debug(
              f'in root: {self.rel_path(input_root)}, found ' + 
              f'{len(data_files)} data file(s): ' +
              ', '.join([self.rel_path(df, input_root) for df in data_files]))

            for mzn_f in mzn_files:
                if len(data_files) == 0:
                    inputs.append((input_root, mzn_f, None))
                else:
                    for data_f in data_files:
                        inputs.append((input_root, mzn_f, data_f))

        inputs.sort()
        for i, (input_root, mzn_f, data_file) in enumerate(inputs):
            self.flatten_file(input_root, mzn_f, data_file,
                              f' ({i + 1} of {len(inputs)})')


if __name__ == '__main__':
    logging.basicConfig(level=logging.DEBUG)
    cur_path: str = os.path.dirname(os.path.realpath(__file__))
    flatten = Flatten(os.path.join(cur_path, 'mzn-challenge'),
                      os.path.join(cur_path, 'flattened-mzn-challenge'))
    flatten.flatten_all()
