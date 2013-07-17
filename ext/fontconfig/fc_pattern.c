#include "rb_fontconfig.h"
#include <fontconfig/fontconfig.h>

VALUE rb_cFcPattern;

static size_t fc_pattern_memsize(const void *p) { return 0; }
static void fc_pattern_free(void* p){
  if(p){
    FcPatternDestroy((FcPattern*)p);
  }
}

static const rb_data_type_t Fc_Pattern_data_type = {
    "FcPattern", 0, fc_pattern_free, fc_pattern_memsize,
};

static VALUE pattern_wrap(FcPattern* p){
  return TypedData_Wrap_Struct(rb_cFcPattern, &Fc_Pattern_data_type, p);
}

#define PATTERN_UNWRAP(c) ((FcPattern*)RTYPEDDATA_DATA(c))



static VALUE rb_pattern_parse(VALUE klass, VALUE string){
  if(TYPE(string) != T_STRING)
    string = rb_any_to_s(string);
  FcPattern* p = FcNameParse(RSTRING_PTR(string));
  return pattern_wrap(p);
}

static VALUE rb_pattern_new(int argc, VALUE *argv, VALUE klass){
  VALUE optional_string = Qnil;
  rb_scan_args(argc, argv, "01", &optional_string);
  if(optional_string != Qnil){
    return rb_pattern_parse(klass, optional_string);
  } else {
    return pattern_wrap(FcPatternCreate());
  }
}


static VALUE rb_pattern_to_s(VALUE self){
  char* res = FcNameUnparse(PATTERN_UNWRAP(self));
  VALUE str = rb_str_new2((char*)res);
  free(res);
  return str;
}


void Init_fontconfig_pattern(){
  rb_cFcPattern = rb_define_class_under(rb_cFontconfig, "Pattern", rb_cObject);
  rb_define_singleton_method(rb_cFcPattern, "new", rb_pattern_new, -1);
  rb_define_singleton_method(rb_cFcPattern, "parse", rb_pattern_parse, 1);

  rb_define_method(rb_cFcPattern, "to_s", rb_pattern_to_s, 0);

}
