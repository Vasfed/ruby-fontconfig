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

VALUE pattern_wrap(FcPattern* p){
  return TypedData_Wrap_Struct(rb_cFcPattern, &Fc_Pattern_data_type, p);
}

int is_fc_pattern(VALUE val){
  return rb_typeddata_is_kind_of(val, &Fc_Pattern_data_type);
}

#define PATTERN_UNWRAP(c) ((FcPattern*)RTYPEDDATA_DATA(c))


static VALUE rb_pattern_parse(VALUE klass, VALUE string){
  if(TYPE(string) != T_STRING)
    string = rb_any_to_s(string);
  FcPattern* p = FcNameParse(RSTRING_PTR(string));
  return pattern_wrap(p);
}

static VALUE rb_pattern_new(int argc, VALUE *argv, VALUE klass){
  VALUE optional = Qnil;
  rb_scan_args(argc, argv, "01", &optional);
  if(optional != Qnil){
    if(TYPE(optional) == T_STRING)
      return rb_pattern_parse(klass, optional);

    rb_raise(rb_eArgError, "parameter must be a string");
  } else {
    return pattern_wrap(FcPatternCreate());
  }
}

static VALUE rb_pattern_format(VALUE self, VALUE format){
  if(TYPE(format) != T_STRING)
    format = rb_any_to_s(format);
  char* res = FcPatternFormat(PATTERN_UNWRAP(self), RSTRING_PTR(format));
  VALUE str = rb_str_new2((char*)res);
  FcStrFree(res);
  return str;
}

static VALUE rb_pattern_to_s(VALUE self){
  char* res = FcNameUnparse(PATTERN_UNWRAP(self));
  VALUE str = rb_str_new2((char*)res);
  FcStrFree(res);
  return str;
}

static VALUE rb_pattern_duplicate(VALUE self){
  FcPattern* p = FcPatternDuplicate(PATTERN_UNWRAP(self));
  return pattern_wrap(p);
}

static VALUE rb_pattern_debugprint(VALUE self){
  FcPatternPrint(PATTERN_UNWRAP(self));
  return Qnil;
}

static VALUE rb_pattern_equal(VALUE self, VALUE other){
  if(!is_fc_pattern(other)){
    //TODO: cast?
    return Qfalse;
  }
  return FcPatternEqual(PATTERN_UNWRAP(self), PATTERN_UNWRAP(other)) ? Qtrue : Qfalse;
}

static VALUE rb_pattern_hash(VALUE self){
  int res = FcPatternHash(PATTERN_UNWRAP(self));
  return INT2NUM(res);
}

static VALUE rb_pattern_add(int argc, VALUE *argv, VALUE self){
  VALUE object, value, append;
  rb_scan_args(argc, argv, "21", &object, &value, &append);
  FcValue val;
  switch(TYPE(value)){
    case T_NONE: case T_NIL: val.type = FcTypeVoid; break;
    case T_FIXNUM: val.type = FcTypeInteger; val.u.i = NUM2INT(value); break;
    case T_FLOAT: val.type = FcTypeDouble; val.u.d = NUM2DBL(value); break;
    case T_STRING: val.type = FcTypeString; val.u.s = RSTRING_PTR(value); break;
    case T_TRUE: val.type = FcTypeBool; val.u.b = 1; break;
    case T_FALSE: val.type = FcTypeBool; val.u.b = 0; break;
    case T_OBJECT:
    case T_DATA:
      
      //TODO: increment object references?
      // ...
      //  FcTypeMatrix: //    m               FcMatrix *
      //  FcTypeCharSet: //   c               FcCharSet *
      //  FcTypeFTFace: //  f   void * (FT_Face)
      //  FcTypeLangSet
    default:
      rb_raise(rb_eArgError, "unsupported type for value");
      break;
  }
  if(TYPE(object)!=T_STRING)
    object = rb_any_to_s(object);
  int res = FcPatternAdd(PATTERN_UNWRAP(self), RSTRING_PTR(object), val, RTEST(append));
  //TODO: raise on errors?
  return res? Qtrue : Qfalse;
}

static VALUE fc_value_to_value(FcValue* val){
  //FcTypeString, FcTypeMatrix and FcTypeCharSet reference memory, need FcValueDestroy or so
  switch(val->type){
    case FcTypeVoid: return Qnil;
    case FcTypeInteger: return INT2FIX(val->u.i);
    case FcTypeDouble: return rb_float_new(val->u.d);
    case FcTypeString: return rb_str_new2(val->u.s);
    case FcTypeBool: return val->u.b? Qtrue : Qfalse;
    case FcTypeMatrix: //    m               FcMatrix *
    case FcTypeCharSet: //   c               FcCharSet *
    case FcTypeFTFace: //  f   void * (FT_Face)
    case FcTypeLangSet: // l   FcLangSet * l
      //TODO
      // note: data is not copied, we have a pointer to internal structure
      return Qnil;
    default:
      rb_raise(rb_eRuntimeError, "got unknown FcValue type %d", val->type);
      break;
  }
  return Qnil;
}

static VALUE rb_pattern_get(VALUE self, VALUE object, VALUE id){
  Check_Type(id, T_FIXNUM);
  if(TYPE(object)!=T_STRING)
    object = rb_any_to_s(object);
  FcValue val;
  FcResult res = FcPatternGet(PATTERN_UNWRAP(self), RSTRING_PTR(object), FIX2INT(id), &val);
  if(res == FcResultMatch){
    VALUE r_res = fc_value_to_value(&val);
    // no need to free, ptr to internal mem
    return r_res;
  }
  if(res == FcResultNoMatch || res == FcResultNoId){
    //raise?
    return Qnil;
  }
  if(res == FcResultOutOfMemory){
    rb_raise(rb_eRuntimeError, "FcResultOutOfMemory");
  }
  //FcResultTypeMismatch cannot be here
  return Qnil;
}

static VALUE rb_pattern_delete(VALUE self, VALUE object){
  if(TYPE(object)!=T_STRING)
    object = rb_any_to_s(object);
  int res = FcPatternDel(PATTERN_UNWRAP(self), RSTRING_PTR(object));
  return res? Qtrue : Qfalse;
}

static VALUE rb_pattern_remove(VALUE self, VALUE object, VALUE id){
  Check_Type(id, T_FIXNUM);
  if(TYPE(object)!=T_STRING)
    object = rb_any_to_s(object);
  int res = FcPatternRemove(PATTERN_UNWRAP(self), RSTRING_PTR(object), FIX2INT(id));
  return res? Qtrue : Qfalse;
}

static VALUE rb_pattern_default_substitute(VALUE self){
  FcDefaultSubstitute(PATTERN_UNWRAP(self));
  return self;
}


#include "fc_internal.h"

static VALUE rb_pattern_each_key(VALUE self){
  FcPattern* p = PATTERN_UNWRAP(self);
  int i;
  FcPatternElt* e;
  for (i = 0; i < p->num; i++){
    e = &FcPatternElts(p)[i];
    //FIXME: return as symbols?
    rb_yield(rb_str_new2(FcObjectName(e->object)));
  }
  return self;
}

static VALUE rb_pattern_has_key_p(VALUE self, VALUE key){
  FcPattern* p = PATTERN_UNWRAP(self);
  FcPatternElt* e = FcPatternObjectFindElt(p, FcObjectFromName (StringValuePtr(key)));
  if(e){
    return Qtrue;
  }
  return Qfalse;
}

static VALUE rb_pattern_get_keys(VALUE self){
  FcPattern* p = PATTERN_UNWRAP(self);
  VALUE res = rb_ary_new();
  int i;
  FcPatternElt* e;
  for (i = 0; i < p->num; i++){
    e = &FcPatternElts(p)[i];
    rb_ary_push(res, rb_str_new2(FcObjectName(e->object)));
    // FcValueListPrint (FcPatternEltValues(e));
  }
  return res;
}

static VALUE rb_pattern_each_key_value(VALUE self, VALUE key){
  FcPattern* p = PATTERN_UNWRAP(self);
  FcPatternElt* e = FcPatternObjectFindElt(p, FcObjectFromName (StringValuePtr(key)));
  FcValueListPtr l;
  if(e){
    for (l = FcPatternEltValues(e); l; l = FcValueListNext(l)){
      // FcValueCanonicalize(&l->value); - re allocates value pointers, do not need this
      rb_yield(fc_value_to_value(&l->value));
    }
  }
  return self;
}



void Init_fontconfig_pattern(){
  rb_cFcPattern = rb_define_class_under(rb_cFontconfig, "Pattern", rb_cObject);
  rb_define_singleton_method(rb_cFcPattern, "new", rb_pattern_new, -1);
  rb_define_singleton_method(rb_cFcPattern, "parse", rb_pattern_parse, 1);
  rb_define_method(rb_cFcPattern, "dup", rb_pattern_duplicate, 0);

  rb_define_method(rb_cFcPattern, "==", rb_pattern_equal, 1);
  // rb_define_method(rb_cFcPattern, "equal_subset", rb_pattern_equal, 1);
  // filter - need set
  rb_define_method(rb_cFcPattern, "add", rb_pattern_add, -1);
  rb_define_method(rb_cFcPattern, "get", rb_pattern_get, 2);
  rb_define_method(rb_cFcPattern, "delete", rb_pattern_delete, 1);
  rb_define_method(rb_cFcPattern, "remove", rb_pattern_remove, 2);

  rb_define_method(rb_cFcPattern, "hash", rb_pattern_hash, 0);
  rb_define_method(rb_cFcPattern, "to_s", rb_pattern_to_s, 0);
  rb_define_method(rb_cFcPattern, "unparse", rb_pattern_to_s, 0);
  rb_define_method(rb_cFcPattern, "format", rb_pattern_format, 1);
  rb_define_method(rb_cFcPattern, "debug_print", rb_pattern_debugprint, 0);
  rb_define_method(rb_cFcPattern, "default_substitute!", rb_pattern_default_substitute, 0);

  rb_define_method(rb_cFcPattern, "keys", rb_pattern_get_keys, 0);
  rb_define_method(rb_cFcPattern, "each_key", rb_pattern_each_key, 0);
  rb_define_method(rb_cFcPattern, "each_value", rb_pattern_each_key_value, 1);
  rb_define_method(rb_cFcPattern, "has_key?", rb_pattern_has_key_p, 1);

}
