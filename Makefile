UNAME_S := $(shell uname -s)
find-first = $(firstword $(foreach candidate,$1,$(shell which $(candidate) 2>/dev/null)))

ifeq ($(UNAME_S),Darwin)
GCC=$(call find-first,clang)
GPP=$(call find-first,clang++)
else
GCC=$(call find-first,gcc-14 gcc-13 gcc)
GPP=$(call find-first,g++-14 g++-13 g++)
endif

CMAKE_C_COMPILER=$(if ${GCC}, -DCMAKE_C_COMPILER=${GCC},)
CMAKE_CXX_COMPILER=$(if ${GPP}, -DCMAKE_CXX_COMPILER=${GPP},)
export CMAKE_OPTIONS+= ${ENV_CMAKE_OPTIONS}${CMAKE_C_COMPILER}${CMAKE_CXX_COMPILER}

CMAKE=$(shell which cmake)
MKFILE_PATH=$(dir $(abspath $(lastword $(MAKEFILE_LIST))))
BUILD_DIR=${MKFILE_PATH}build
MZN_MODEL_DIR=${MKFILE_PATH}mzn-models
FZN_MODEL_DIR=${MKFILE_PATH}fzn-models
MINIZINC=$(shell which minizinc)
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
	         -B ${BUILD_DIR}
	${CMAKE} --build ${BUILD_DIR} -j 8

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
