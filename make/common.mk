BUILD ?= release

COMMON_CPP_STD = -std=c++23

ifeq ($(BUILD),release)
COMMON_OPT_CFLAGS = -O3 -DNDEBUG
else
COMMON_OPT_CFLAGS = -O0 -g3 -fno-omit-frame-pointer -fno-inline -DDEBUG
endif
