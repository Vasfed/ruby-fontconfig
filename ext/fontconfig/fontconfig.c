#include <ruby.h>

static VALUE rb_cFontconfig;



void Init_fontconfig(){
  rb_cFontconfig = rb_define_module("Fontconfig");
  // rb_define_singleton_method(rb_mIsA, "class_of", rb_get_class_of, 1);
}
