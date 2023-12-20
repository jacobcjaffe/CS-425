#! make

SOURCES = $(wildcard *.cu)
HEADERS = $(wildcard *.h)
KERNELS = $(wildcard Kernels/*.h)
TARGETS = $(SOURCES:.cu=)

NVCC = nvcc

STD = -std=c++17
OPTIMIZER ?= -O3
CXXFLAGS = $(STD) $(OPTIMIZER)

DIRT = $(TARGETS)

default all: $(TARGETS)

%: %.cu $(HEADERS) $(KERNELS)
	$(NVCC) $(CXXFLAGS) $< -o $@

clean clobber:
	$(RM) $(DIRT)

.PHONY: default all clean clobber