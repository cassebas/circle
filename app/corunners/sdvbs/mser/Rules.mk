SDVBS_MSER = sdvbs/mser
OBJS += $(patsubst $(SDVBS_MSER)/%.c,$(SDVBS_MSER)/%.o,$(wildcard $(SDVBS_MSER)/*.c))
