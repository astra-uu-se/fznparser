import os
from typing import Union, List, Tuple
import subprocess
import logging


def is_mzn(file: str) -> bool:
    _, ext = os.path.splitext(file)
    return ext == '.mzn'


def is_dzn(file: str) -> bool:
    _, ext = os.path.splitext(file)
    return ext == '.dzn'


def get_mzn_files(files: List[str]) -> list:
    return [f for f in files if is_mzn(f)]


def get_dzn_files(files: List[str]) -> list:
    return [f for f in files if is_dzn(f)]


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

    def __init__(self, input_dir: str, output_dir: str):
        self._input_dir = os.path.abspath(input_dir)
        self._output_dir = os.path.abspath(output_dir)
        self._minizinc_path = find_minizinc()
        self._logger = logging.getLogger(self.__class__.__name__)
        self._logger.debug(f'input dir: {self._input_dir}')
        self._logger.debug(f'output dir: {self._input_dir}')
        self._logger.debug(f'minizinc path: {self._minizinc_path}')

    def find_output_dir(self, input_root: str) -> str:
        assert input_root.startswith(self._input_dir)
        return os.path.join(
            self._output_dir, input_root[len(self._input_dir):].lstrip('/'))

    def flatten_file(self, input_root: str, mzn_file: str,
                     dzn_file: Union[None, str], logging_suffix: str) -> None:
        params = [
            self._minizinc_path, '-c', os.path.join(input_root, mzn_file),
            '--solver', self._solver]
        output_root = self.find_output_dir(input_root)
        self._logger.debug(f'output root: {output_root}')
        make_output_dir(output_root)
        mzn_f, _ = os.path.splitext(mzn_file)
        if dzn_file is not None:
            params.append(os.path.join(input_root, dzn_file))
            dzn_f, _ = os.path.splitext(dzn_file)
            fzn_file_prefix = os.path.join(output_root, mzn_f + '_' + dzn_f)
            fzn_file = fzn_file_prefix + '.fzn'
            self._logger.debug(
                f'flattening {mzn_file} with {dzn_file} into {fzn_file}')
        else:
            fzn_file_prefix = os.path.join(output_root, mzn_f)
            fzn_file = fzn_file_prefix + '.fzn'
            self._logger.debug(f'flattening {mzn_file} into  {fzn_file}')
        params = params + ['--fzn', fzn_file, '--no-output-ozn']
        log_file = fzn_file_prefix + '.log'
        try:
            result = subprocess.run(params, stdout=subprocess.PIPE,
                                    stderr=subprocess.PIPE, text=True)
            if os.path.exists(log_file):
                os.remove(log_file)
            result.check_returncode()
        except subprocess.CalledProcessError as e:
            self._logger.error(f'failed to flatten {mzn_file} with {dzn_file}')
            self._logger.error(f'error: {e}')
            if e.stdout is not None and len(e.stdout) > 0:
                self._logger.error(f'stdout: {e.stdout}')
            if e.stderr is not None and len(e.stderr) > 0:
                with open(log_file, 'w') as f:
                    f.write(e.stderr)
            return
        self._logger.info(f'created {fzn_file} {logging_suffix}')

    def flatten_all(self):
        inputs: List[Tuple[str, str, Union[str, None]]] = []
        for input_root, dirs, files in os.walk(self._input_dir, topdown=False):
            mzn_files = get_mzn_files(files)
            if len(mzn_files) == 0:
                continue
            self._logger.debug(f'in root: {input_root}, found mzn files: '
                               f'{mzn_files}')
            dzn_files = get_dzn_files(files)
            self._logger.debug(f'in root: {input_root}, found fzn files: '
                               f'{dzn_files}')
            for mzn_f in mzn_files:
                if len(dzn_files) == 0:
                    inputs.append((input_root, mzn_f, None))
                else:
                    for dzn_f in dzn_files:
                        inputs.append((input_root, mzn_f, dzn_f))

        inputs.sort()
        for i, (input_root, mzn_f, dzn_f) in enumerate(inputs):
            self.flatten_file(input_root, mzn_f, dzn_f,
                              f' ({i + 1} of {len(inputs)})')


if __name__ == '__main__':
    logging.basicConfig(level=logging.INFO)
    cur_path: str = os.path.dirname(os.path.realpath(__file__))
    flatten = Flatten(os.path.join(cur_path, 'mzn-challenge'),
                      os.path.join(cur_path, 'flattened-mzn-challenge.new'))
    flatten.flatten_all()
