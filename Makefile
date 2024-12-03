export CMAKE_OPTIONS+= ${ENV_CMAKE_OPTIONS}

CMAKE=$(shell which cmake)
MKFILE_PATH=$(dir $(abspath $(lastword $(MAKEFILE_LIST))))
BUILD_DIR=${MKFILE_PATH}build
MZN_MODEL_DIR=${MKFILE_PATH}mzn-models
FZN_MODEL_DIR=${MKFILE_PATH}fzn-models
MINIZINC=$(shell which minizinc)
C_COMPILER=$(shell which gcc-13)
CXX_COMPILER=$(shell which g++-13)
GIT=$(shell which git)
MZN_CHALLENGE_REPO="git@github.com:MiniZinc/minizinc-benchmarks.git"
MZN_CHALLENGE_DIR=${MKFILE_PATH}test/mzn-challenge

.PHONY: fzn-sanity
fzn-sanity:
	${MINIZINC} --version | grep -E '(version 2\.[6-9])' || (echo "ERROR: minizinc version too low" && false)

.PHONY: clean
clean:
	rm -rf ${BUILD_DIR}

.PHONY: build
build:
	mkdir -p ${BUILD_DIR}
	${CMAKE} -S ${MKFILE_PATH}test \
	         -B ${BUILD_DIR} \
					 -D CMAKE_C_COMPILER=${C_COMPILER} \
					 -D CMAKE_CXX_COMPILER=${CXX_COMPILER}
	${CMAKE} --build ${BUILD_DIR}

.PHONY: test
test: build
	CTEST_OUTPUT_ON_FAILURE=1;
	exec ${BUILD_DIR}/fznparserTests

.PHONY: mzn-challenge
mzn-challenge:
	mkdir -p ${MZN_CHALLENGE_DIR}
	${GIT} clone ${MZN_CHALLENGE_REPO} ${MZN_CHALLENGE_DIR}

.PHONY: fzn
fzn: fzn-sanity
	${MINIZINC} -c \
					    ${MZN_MODEL_DIR}/car_sequencing.mzn \
					    ${MZN_MODEL_DIR}/car_sequencing.dzn \
					    --fzn ${FZN_MODEL_DIR}/car_sequencing.fzn \
						--no-output-ozn
	${MINIZINC} -c \
					    ${MZN_MODEL_DIR}/magic_square.mzn \
					    -D n=3 \
					    --fzn ${FZN_MODEL_DIR}/magic_square.fzn \
						--no-output-ozn
	${MINIZINC} -c \
					    ${MZN_MODEL_DIR}/tsp_alldiff.mzn \
					    ${MZN_MODEL_DIR}/tsp_17.dzn \
					    --fzn ${FZN_MODEL_DIR}/tsp_alldiff.fzn \
						--no-output-ozn
	${MINIZINC} -c \
					    ${MZN_MODEL_DIR}/n_queens.mzn \
					    -D n=32 \
					    --fzn ${FZN_MODEL_DIR}/n_queens.fzn \
						--no-output-ozn
	

.PHONY: clang-format
clang-format:
	find ${MKFILE_PATH}/include/ ${MKFILE_PATH}/include_private/ ${MKFILE_PATH}/src/ ${MKFILE_PATH}/test/src \
	  -iname *.[ch]pp | xargs clang-format -i -style=Google
