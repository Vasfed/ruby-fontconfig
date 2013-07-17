#include "rb_fontconfig.h"
#include <fontconfig/fontconfig.h>

VALUE rb_cFontconfig;

static VALUE rb_fc_version(VALUE self){
  int version = FcGetVersion();
  return INT2FIX(version);
}


void Init_fontconfig_pattern();

void Init_fontconfig(){
  rb_cFontconfig = rb_define_module("Fontconfig");
  rb_define_singleton_method(rb_cFontconfig, "library_version", rb_fc_version, 0);
  Init_fontconfig_pattern();
}
