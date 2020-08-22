#
# Makefile
#

CIRCLEHOME = ../..

OBJS	= main.o kernel.o synthetic_bench.o temptask.o armv8_pm.o corunners.o malardalen.o randomwrapper.o ldstexcl.o

LIBS	= $(CIRCLEHOME)/lib/sched/libsched.a \
	  $(CIRCLEHOME)/lib/libcircle.a

# SD-VBS
include sdvbs/common/Rules.mk
include sdvbs/disparity/Rules.mk
include sdvbs/mser/Rules.mk
include sdvbs/svm/Rules.mk
include sdvbs/stitch/Rules.mk

include $(CIRCLEHOME)/Rules.mk
include Install.mk

-include $(DEPS)
