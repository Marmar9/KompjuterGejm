define add_cpp_srcs

$(eval OUTFILE_DEPS += $(patsubst $(SRCDIR)/$1/%.cpp, $(OUTDIR)/%.o, $(wildcard $(SRCDIR)/$1/*.cpp)))

 $(eval \
    $(OUTDIR)/%.o: $(SRCDIR)/$1/%.cpp; \
    $(CXX) $(CXXFLAGS) -c -o $$@ $$< \
  )
endef

define add_frag_srcs

$(eval RUNTIME_DEPS += $$(patsubst $(SRCDIR)/$1/%.frag.glsl, $(OUTDIR)/%.frag.spv, $$(wildcard $(SRCDIR)/$1/*.frag.glsl)))

$(eval \
   $(OUTDIR)/%.frag.spv: $(SRCDIR)/$1/%.frag.glsl; \
   glslc -fshader-stage=fragment -o $$@ $$< \
 )
endef

define add_vert_srcs

$(eval RUNTIME_DEPS += $$(patsubst $(SRCDIR)/$1/%.vert.glsl, $(OUTDIR)/%.vert.spv, $$(wildcard $(SRCDIR)/$1/*.vert.glsl)))

 $(eval \
    $(OUTDIR)/%.vert.spv: $(SRCDIR)/$1/%.vert.glsl; \
    glslc -fshader-stage=vertex -o $$@ $$<  \
  )
endef


define add_c_srcs


$(eval OUTFILE_DEPS += $(patsubst $(SRCDIR)/$1/%.c, $(OUTDIR)/%.o, $(wildcard $(SRCDIR)/$1/*.c)))

$(eval \
   $(OUTDIR)/%.o: $(SRCDIR)/$1/%.c; \
   clang -c -fPIC -o $$@ $$<\
 )
endef
