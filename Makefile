# tool macros
CC := gcc
AR := llvm-ar
CFLAGS := -O0 -Wall 
COBJFLAGS := -O0 -c 
ARFLAGS := -X 64 --thin

# path macros
BIN_PATH := bin
OBJ_PATH := obj
SRC_PATH := src
DBG_PATH := debug

# compile macros
TARGET_NAME_DYNAMIC := liblamadamem
TARGET_NAME_STATIC  := liblamadamem

ifeq ($(OS),Windows_NT)
	$(error This library was not intended to be working on Windows Platform.)
	TARGET_NAME_DYNAMIC := $(addsuffix .dll,$(TARGET_NAME_DYNAMIC))
	TARGET_NAME_STATIC  := $(addsuffix .lib,$(TARGET_NAME_STATIC)) 

else
	TARGET_NAME_DYNAMIC := $(addsuffix .so,$(TARGET_NAME_DYNAMIC))
	TARGET_NAME_STATIC  := $(addsuffix .a,$(TARGET_NAME_STATIC))
endif
TARGET_DYNAMIC := $(BIN_PATH)/$(TARGET_NAME_DYNAMIC)
TARGET_STATIC  := $(BIN_PATH)/$(TARGET_NAME_STATIC)

# src files & obj files
SRC := $(foreach x, $(SRC_PATH), $(wildcard $(addprefix $(x)/*,.c*)))
OBJ := $(addprefix $(OBJ_PATH)/, $(addsuffix .o, $(notdir $(basename $(SRC)))))

# clean files list
DISTCLEAN_LIST := $(OBJ)
CLEAN_LIST := $(TARGET_DYNAMIC) \
			  $(TARGET_STATIC)  \
			  $(DISTCLEAN_LIST)

# default rule
default: makedir all

# non-phony targets
$(TARGET_STATIC): $(OBJ)
	$(info $(NULL)  AR  $(TARGET_STATIC))
	@$(AR) r $(TARGET_STATIC) $(OBJ) > /dev/null 2>&1  

$(TARGET_DYNAMIC): $(OBJ)
	$(info $(NULL)  ELF $(TARGET_DYNAMIC))
	@$(CC) -o $@ $(OBJ) $(CFLAGS)

$(OBJ_PATH)/%.o: $(SRC_PATH)/%.c*
	$(info $(NULL)  CC  $< $@)
	@$(CC) $(COBJFLAGS) -o $@ $<

# phony rules
.PHONY: envinfo
envinfo:

ifeq ($(OS),Windows_NT)
	$(info Platform: Windows $())
else
	$(info Platform: $(shell uname -a))
endif

	$(info CC: $(CC))
	$(info AR: $(AR))
	$(info CFlags: $(CFLAGS))
	$(info CObjFlags: $(COBJFLAGS))
	$(info ARFlags: $(ARFLAGS))
	$(info Targets: $(TARGET_DYNAMIC) $(TARGET_STATIC))

.PHONY: makedir
makedir:
	@mkdir -p $(BIN_PATH) $(OBJ_PATH)

.PHONY: all
all: envinfo $(TARGET_DYNAMIC) $(TARGET_STATIC)

.PHONY: clean
clean:
	@echo "  CLEAN $(CLEAN_LIST)"
	@rm -rf $(CLEAN_LIST)


