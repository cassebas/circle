SDVBS_DISPARITY = sdvbs/disparity
OBJS += $(patsubst $(SDVBS_DISPARITY)/%.c,$(SDVBS_DISPARITY)/%.o,$(wildcard $(SDVBS_DISPARITY)/*.c))
