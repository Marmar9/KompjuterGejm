include game-engine/utils.mk


MAKEFLAGS += -j8
.DEFAULT_GOAL := all

GAME_ENGINE_DIR := $(CURDIR)/game-engine

GAME_ENGINE_LIB := $(GAME_ENGINE_DIR)/build/libgame-engine.so

DEBUG := 1

CXX := clang++
CXXFLAGS := -MMD -fPIE -fstack-protector-all -std=c++23 -I$(CURDIR) -I$(GAME_ENGINE_DIR) 
LDFLAGS := -fuse-ld=mold $(GAME_ENGINE_LIB) -lvulkan 

# -Wl,-rpath=$(GAME_ENGINE_PATH) -L$(GAME_ENGINE_PATH) -lgame-engine
# 


# Debug
ifeq ($(DEBUG), 1)

CXXFLAGS += -g -DDEBUG -O0
else

CXXFLAGS += -s -O3

endif

SRCDIR := src
SRC := $(SRCDIR)/main.cpp
OUTDIR := out
OUTFILE := $(OUTDIR)/out

OUTFILE_DEPS :=


$(GAME_ENGINE_LIB): force
	make -C $(GAME_ENGINE_DIR) DEBUG=$(DEBUG)

force:



$(call add_cpp_srcs)

$(OUTFILE): $(OUTFILE_DEPS) $(GAME_ENGINE_LIB) | $(OUTDIR)
	$(CXX) $(OUTFILE_DEPS) -o $(OUTFILE) $(LDFLAGS) 

$(OUTDIR):
	mkdir $@

run: $(OUTFILE)
	./$(OUTFILE)

clean: 
	make -C $(GAME_ENGINE_DIR) clean
	rm $(OUTFILE) $(OUTFILE).d

dump:
	@coredumpctl dump $(OUTFILE) -o coredump


all: $(OUTFILE)

.PHONY: all clean run
