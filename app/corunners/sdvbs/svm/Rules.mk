SDVBS_SVM = sdvbs/svm
OBJS += $(patsubst $(SDVBS_SVM)/%.c,$(SDVBS_SVM)/%.o,$(wildcard $(SDVBS_SVM)/*.c))
