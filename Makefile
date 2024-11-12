#tool macros
CC := ${CC}

CC ?= clang 
AR ?= llvm-ar

# compiler flags
LDFLAGS := -O3 -Wall -fsanitize=address 
CFLAGS := -O3 -g -fPIC 
ARFLAGS := -X 64 --thin

# path macros
BIN_PATH := bin
OBJ_PATH := obj
SRC_PATH := src
DBG_PATH := debug

ifeq ($(PREFIX),)
    PREFIX := /usr/local
endif

# compile macros
TARGET_NAME_DYNAMIC := liblamada
TARGET_NAME_STATIC  := liblamada

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
HEADERS := $(foreach x, $(SRC_PATH), $(wildcard $(addprefix $(x)/*,.h*)))

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
	@$(CC) -o $@ $(OBJ) $(LDFLAGS) -shared

$(OBJ_PATH)/%.o: $(SRC_PATH)/%.c*
	$(info $(NULL)  CC  $< $@)
	@$(CC) $(CFLAGS) -shared -o $@ $<

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
	$(info LDFlags: $(LDFLAGS))
	$(info CCFlags: $(CFLAGS))
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

.PHONY: installbin
installbin:
	$(info $(NULL)  INSTALL $(TARGET_STATIC))
	@install $(TARGET_STATIC) $(PREFIX)/lib
	
	$(info $(NULL)  INSTALL $(TARGET_DYNAMIC))
	@install $(TARGET_DYNAMIC) $(PREFIX)/lib

.PHONY: insatllheaders $(HEADERS)
installheaders: $(HEADERS)
$(HEADERS):
	$(info $(NULL)  INSTALL $@)
	@cp $@ $(PREFIX)/include/lamada

.PHONY: installclean
installclean:
	@rm -r $(PREFIX)/include/lamada
	@mkdir -p $(PREFIX)/include/lamada

.PHONY: install
install: installclean installbin installheaders

