export CMAKE_OPTIONS+= ${ENV_CMAKE_OPTIONS}
export CC=${which gcc-11}
export CXX=${which g++-11}

CC=$(shell which gcc-11)
CXX=$(shell which g++-11)
CMAKE=$(shell which cmake)
MKFILE_PATH=$(dir $(abspath $(lastword $(MAKEFILE_LIST))))
BUILD_DIR=${MKFILE_PATH}build
TEST_DIR=${BUILD_DIR}/test
MZN_MODEL_DIR=${MKFILE_PATH}mzn-models
FZN_MODEL_DIR=${MKFILE_PATH}fzn-models
MINIZINC=$(shell which minizinc)

.PHONY: fzn-sanity
fzn-sanity:
	${MINIZINC} --version | grep -E '(version 2\.[6-9])' || (echo "ERROR: minizinc version too low" && false)

.PHONY: clean
clean:
	rm -rf ${BUILD_DIR}

.PHONY: build
build:
	mkdir -p ${BUILD_DIR}
	cd ${BUILD_DIR}; \
	${CMAKE} ${CMAKE_OPTIONS} -D CMAKE_C_COMPILER=${CC} -D CMAKE_CXX_COMPILER=${CXX} -S ${MKFILE_PATH}test; \
	${MAKE}

.PHONY: test
test: build
	CTEST_OUTPUT_ON_FAILURE=1;
	exec ${BUILD_DIR}/fznparserTests

.PHONY: fzn
fzn: fzn-sanity
	${MINIZINC} -c \
					    ${MZN_MODEL_DIR}/car_sequencing.mzn \
					    ${MZN_MODEL_DIR}/car_sequencing.dzn \
					    --fzn ${FZN_MODEL_DIR}/car_sequencing.fzn
	rm -f ${MZN_MODEL_DIR}/car_sequencing.ozn
	${MINIZINC} -c \
					    ${MZN_MODEL_DIR}/magic_square.mzn \
					    -D n=3 \
					    --fzn ${FZN_MODEL_DIR}/magic_square.fzn
	rm -f ${MZN_MODEL_DIR}/magic_square.ozn
	${MINIZINC} -c \
					    ${MZN_MODEL_DIR}/tsp_alldiff.mzn \
					    ${MZN_MODEL_DIR}/tsp_17.dzn \
					    --fzn ${FZN_MODEL_DIR}/tsp_alldiff.fzn
	rm -f ${MZN_MODEL_DIR}/tsp_alldiff.ozn
	${MINIZINC} -c \
					    ${MZN_MODEL_DIR}/n_queens.mzn \
					    -D n=32 \
					    --fzn ${FZN_MODEL_DIR}/n_queens.fzn
	rm -f ${MZN_MODEL_DIR}/n_queens.ozn
	
	