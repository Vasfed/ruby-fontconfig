#include "rb_fontconfig.h"

VALUE rb_cFcFontSet;

static size_t fc_object_set_memsize(const void *p) { return 0; }
static void fc_object_set_free(void* p){
  if(p){
    FcFontSetDestroy((FcFontSet*)p);
  }
}

static const rb_data_type_t FcFontSet_data_type = {
    "FcFontSet", 0, fc_object_set_free, fc_object_set_memsize,
};

VALUE font_set_wrap(FcFontSet* p){
  return TypedData_Wrap_Struct(rb_cFcFontSet, &FcFontSet_data_type, p);
}

int is_FcFontSet(VALUE val){
  return rb_typeddata_is_kind_of(val, &FcFontSet_data_type);
}

#define OSET_UNWRAP(c) ((FcFontSet*)RTYPEDDATA_DATA(c))

static VALUE rb_oset_add(VALUE self, VALUE val){
  if(!is_fc_pattern(val)){
    rb_raise(rb_eArgError, "argument must be a FcPattern");
  }
  if(!FcFontSetAdd(OSET_UNWRAP(self), pattern_unwrap(val))){
    rb_raise(rb_eRuntimeError, "cannot insert into FcFontSet");
  }
  return val;
}

static VALUE oset_add_i(VALUE str, VALUE self, int argc, VALUE *argv){
  return rb_oset_add(self, str);
}

static VALUE rb_oset_new(int argc, VALUE *argv, VALUE klass){
  VALUE optional = Qnil;
  rb_scan_args(argc, argv, "01", &optional);
  FcFontSet* set = FcFontSetCreate();
  if(!set)
    rb_raise(rb_eRuntimeError, "cannot create FcFontSet");

  VALUE self = object_set_wrap(set);
  if(optional != Qnil){
    ID each = rb_intern("each");
    if(!rb_respond_to(optional, each))
      rb_raise(rb_eArgError, "parameter must be enumerable");
    rb_block_call(optional, each, 0, 0, oset_add_i, self);
  }
  return self;
}


static VALUE rb_oset_each(VALUE self){
  int i;
  FcFontSet* os = OSET_UNWRAP(self);
  for (i = 0; i < os->nfont; i++){
    rb_yield(pattern_wrap(os->fonts[i]));
  }
  return self;
}

static VALUE rb_oset_size(VALUE self){
  return INT2NUM(OSET_UNWRAP(self)->nfont);
}

static VALUE rb_oset_at(VALUE self, VALUE index){

  int i = NUM2INT(index);
  FcFontSet* os = OSET_UNWRAP(self);

  if(i < 0 || i >= os->nfont)
    return Qnil;
  return pattern_wrap(os->fonts[i]);
}

static VALUE rb_oset_print(VALUE self){
  FcFontSetPrint(OSET_UNWRAP(self));
  return self;
}

static VALUE rb_oset_match(VALUE self, VALUE config, VALUE pattern){
  FcFontSet* set = OSET_UNWRAP(self);
  FcConfig* conf = 0;
  if(RTEST(config) && is_fc_config(config))
    conf = CONFIG_UNWRAP(config);
  if(!RTEST(pattern) || !is_fc_pattern(pattern))
    rb_raise(rb_eArgError, "pattern must be Fontconfig::Pattern");
  FcResult res;
  FcPattern* result = FcFontSetMatch(conf, &set, 1, pattern_unwrap(pattern), &res);
  if(!result)
    return Qnil;
  return pattern_wrap(result);
}

void Init_fontconfig_fontset(){
  rb_cFcFontSet = rb_define_class_under(rb_cFontconfig, "FontSet", rb_cObject);
  rb_define_singleton_method(rb_cFcFontSet, "new", rb_oset_new, -1);
  rb_define_method(rb_cFcFontSet, "add", rb_oset_add, 1);
  rb_define_method(rb_cFcFontSet, "<<",  rb_oset_add, 1);
  rb_define_method(rb_cFcFontSet, "each", rb_oset_each, 0);
  rb_define_method(rb_cFcFontSet, "size", rb_oset_size, 0);
  rb_define_method(rb_cFcFontSet, "[]", rb_oset_at, 1);
  rb_define_method(rb_cFcFontSet, "debug_print", rb_oset_print, 0);
  rb_define_method(rb_cFcFontSet, "match_pattern", rb_oset_match, 1);
  rb_include_module(rb_cFcFontSet, rb_mEnumerable);
}
