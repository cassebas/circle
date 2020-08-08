SDVBS_COMMON = sdvbs/common
OBJS += $(patsubst $(SDVBS_COMMON)/%.c,$(SDVBS_COMMON)/%.o,$(wildcard $(SDVBS_COMMON)/*.c))
