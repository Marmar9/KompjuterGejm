include utils.mk
.DEFAULT_GOAL := all

CXX := clang++
CXXFLAGS := -MMD -pipe -I$(CURDIR) -fPIC -std=c++23
LDFLAGS := -fuse-ld=mold -shared -lvulkan -lwayland-client -lc 

DEBUG := 1

ifeq ($(DEBUG), 1)
CXXFLAGS += -g -O0 -DDEBUG
else
CXXFLAGS += -s -O3
endif

OUTDIR := build
OUTFILE := $(OUTDIR)/libgame-engine.so
SRCDIR := src

OUTFILE_DEPS :=


include $(wildcard $(OUTDIR)/*.d)

VULKAN_SRCDIR := vulkan
$(call add_cpp_srcs,$(VULKAN_SRCDIR))
$(call add_cpp_srcs,$(VULKAN_SRCDIR)/rendering)


# @game-engine/window/*
WINDOW_SRCDIR := window
$(call add_cpp_srcs,$(WINDOW_SRCDIR))

# @game-engine/window/wayland/*
WAYLAND_WINDOW_SRCDIR := $(WINDOW_SRCDIR)/wayland
$(call add_cpp_srcs,$(WAYLAND_WINDOW_SRCDIR))

$(call add_c_srcs,$(WAYLAND_WINDOW_SRCDIR))

UTILS_SRCDIR := utils
$(call add_cpp_srcs,$(UTILS_SRCDIR))


# Runtime targets
RUNTIME_DEPS :=

SHADERS_SRCDIR := $(VULKAN_SRCDIR)/shaders
$(call add_frag_srcs,$(SHADERS_SRCDIR))
$(call add_vert_srcs,$(SHADERS_SRCDIR))


xd = $(OUTDIR)/main.o



$(xd): $(SRCDIR)/main.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@


# Final output 
$(OUTFILE): $(xd) $(OUTFILE_DEPS)
	$(CXX) $(xd) $(OUTFILE_DEPS) -o $@ $(LDFLAGS)
	

# ar rcs $@ $(xd) $(OUTFILE_DEPS)


$(OUTDIR):
	@mkdir $@


.PHONY: clean all

clean: $(OUTDIR)
	@rm -r $<

all: $(OUTDIR) $(RUNTIME_DEPS) $(OUTFILE)

