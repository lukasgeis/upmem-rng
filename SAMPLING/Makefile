DPU_DIR := dpu
HOST_DIR := host
CPU_DIR := baselines
BUILDDIR ?= bin
NR_TASKLETS ?= 16
NR_DPUS ?= 1
N ?= 1e7

define conf_filename
	${BUILDDIR}/.NR_DPUS_$(1)_NR_TASKLETS_$(2).conf
endef
CONF := $(call conf_filename,${NR_DPUS},${NR_TASKLETS})

HOST_TARGET := ${BUILDDIR}/host
DPU_TARGET := ${BUILDDIR}/dpu
CPU_TARGET := ${BUILDDIR}/cpu

COMMON_INCLUDES := ../includes
HOST_SOURCES := $(wildcard ${HOST_DIR}/*.c)
DPU_SOURCES := $(wildcard ${DPU_DIR}/*.c)
CPU_SOURCES := $(wildcard ${CPU_DIR}/*.c)

.PHONY: all clean test 

__dirs := $(shell mkdir -p ${BUILDDIR})

COMMON_FLAGS := -Wall -Wextra -Werror -g -I${COMMON_INCLUDES}

HOST_FLAGS := ${COMMON_FLAGS} -std=c11 -O3 `dpu-pkg-config --cflags --libs dpu` -DNR_TASKLETS=${NR_TASKLETS} -DNR_DPUS=${NR_DPUS} 
DPU_FLAGS := ${COMMON_FLAGS} -O3 -DNR_TASKLETS=${NR_TASKLETS} -DNR_DPUS=${NR_DPUS}  -DN=${N}
CPU_FLAGS := -O3 -fopenmp -DNR_TASKLETS=${NR_TASKLETS} -DN=${N}

all: ${HOST_TARGET} ${DPU_TARGET} ${CPU_TARGET}

dpu: ${HOST_TARGET} ${DPU_TARGET}

cpu: ${CPU_TARGET}

${CONF}:
	$(RM) $(call conf_filename,*,*)
	touch ${CONF}

${HOST_TARGET}: ${HOST_SOURCES} ${COMMON_INCLUDES} ${CONF}
	$(CC) -o $@ ${HOST_SOURCES} ${HOST_FLAGS}

${DPU_TARGET}: ${DPU_SOURCES} ${COMMON_INCLUDES} ${CONF}
	dpu-upmem-dpurte-clang ${DPU_FLAGS} -o $@ ${DPU_SOURCES}

${CPU_TARGET}: ${CPU_SOURCES}
	$(CC) -o $@ ${CPU_SOURCES} ${CPU_FLAGS}

clean:
	${RM} -r ${BUILDDIR}

test: all
	./${HOST_TARGET}