#include "rb_fontconfig.h"

VALUE rb_cFcObjectSet;

static size_t fc_object_set_memsize(const void *p) { return 0; }
static void fc_object_set_free(void* p){
  if(p){
    FcObjectSetDestroy((FcObjectSet*)p);
  }
}

static const rb_data_type_t FcObjectSet_data_type = {
    "FcObjectSet", 0, fc_object_set_free, fc_object_set_memsize,
};

static VALUE object_set_wrap(FcObjectSet* p){
  return TypedData_Wrap_Struct(rb_cFcObjectSet, &FcObjectSet_data_type, p);
}

int is_FcObjectSet(VALUE val){
  return rb_typeddata_is_kind_of(val, &FcObjectSet_data_type);
}

#define OSET_UNWRAP(c) ((FcObjectSet*)RTYPEDDATA_DATA(c))

static VALUE rb_oset_add(VALUE self, VALUE str){
  if(!FcObjectSetAdd(OSET_UNWRAP(self), StringValuePtr(str))){
    rb_raise(rb_eRuntimeError, "cannot insert into FcObjectSet");
  }
  return str;
}

static VALUE oset_add_i(VALUE str, VALUE self, int argc, VALUE *argv){
  return rb_oset_add(self, str);
}

static VALUE rb_oset_new(int argc, VALUE *argv, VALUE klass){
  VALUE optional = Qnil;
  rb_scan_args(argc, argv, "01", &optional);
  FcObjectSet* set = FcObjectSetCreate();
  if(!set)
    rb_raise(rb_eRuntimeError, "cannot create FcObjectSet");

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
  FcObjectSet* os = OSET_UNWRAP(self);
  for (i = 0; i < os->nobject; i++){
    rb_yield(rb_str_new2(os->objects[i]));
  }
  return self;
}

static VALUE rb_oset_size(VALUE self){
  return INT2NUM(OSET_UNWRAP(self)->nobject);
}

static VALUE rb_oset_at(VALUE self, VALUE index){

  int i = NUM2INT(index);
  FcObjectSet* os = OSET_UNWRAP(self);

  if(i < 0 || i >= os->nobject)
    return Qnil;
  return rb_str_new2(os->objects[i]);
}

void Init_fontconfig_objectset(){
  rb_cFcObjectSet = rb_define_class_under(rb_cFontconfig, "ObjectSet", rb_cObject);
  rb_define_singleton_method(rb_cFcObjectSet, "new", rb_oset_new, -1);
  rb_define_method(rb_cFcObjectSet, "add", rb_oset_add, 1);
  rb_define_method(rb_cFcObjectSet, "<<",  rb_oset_add, 1);
  rb_define_method(rb_cFcObjectSet, "each", rb_oset_each, 0);
  rb_define_method(rb_cFcObjectSet, "size", rb_oset_size, 0);
  rb_define_method(rb_cFcObjectSet, "[]", rb_oset_at, 1);
  rb_include_module(rb_cFcObjectSet, rb_mEnumerable);
}
