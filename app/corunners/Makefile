#
# Makefile
#

CIRCLEHOME = ../..

OBJS	= main.o kernel.o synthetic_bench.o temptask.o armv8_pm.o corunners.o malardalen.o randomwrapper.o ldstexcl.o

LIBS	= $(CIRCLEHOME)/lib/sched/libsched.a \
	  $(CIRCLEHOME)/lib/libcircle.a

include $(CIRCLEHOME)/Rules.mk
include Install.mk

-include $(DEPS)
