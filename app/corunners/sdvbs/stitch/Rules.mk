SDVBS_STITCH = sdvbs/stitch
OBJS += $(patsubst $(SDVBS_STITCH)/%.c,$(SDVBS_STITCH)/%.o,$(wildcard $(SDVBS_STITCH)/*.c))
