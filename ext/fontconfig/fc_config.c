#include "rb_fontconfig.h"

VALUE rb_cFcConfig;

static size_t fc_config_memsize(const void *p) { return 0; }
static void fc_config_free(void* p){
  if(p){
// FcConfigDestroy -- Destroy a configuration
    FcConfigDestroy((FcConfig*)p);
  }
}

static const rb_data_type_t Fc_Config_data_type = {
    "FcConfig", 0, fc_config_free, fc_config_memsize,
};

static VALUE config_wrap(FcConfig* p){
  return TypedData_Wrap_Struct(rb_cFcConfig, &Fc_Config_data_type, p);
}

int is_fc_config(VALUE val){
  return rb_typeddata_is_kind_of(val, &Fc_Config_data_type);
}

#define CONFIG_UNWRAP(c) ((FcConfig*)RTYPEDDATA_DATA(c))

// FcConfigParseAndLoad -- load a configuration file
static VALUE rb_config_parse_and_load(int argc, VALUE *argv, VALUE self){
  VALUE path, complain = Qnil;
  rb_scan_args(argc, argv, "11", &path, &complain);
  if(TYPE(path)!=T_STRING)
    path = rb_any_to_s(path);

  int res = FcConfigParseAndLoad(CONFIG_UNWRAP(self), RSTRING_PTR(path), RTEST(complain));
  return BOOL2VAL(res);
}

// FcConfigCreate -- Create a configuration
static VALUE rb_config_new(int argc, VALUE *argv, VALUE klass){
  FcConfig * conf = FcConfigCreate();
  VALUE file;
  rb_scan_args(argc, argv, "01", &file);

  if(!conf){
    rb_raise(rb_eRuntimeError, "Cannot create FcConfig");
  }
  VALUE self = config_wrap(conf);
  if(RTEST(file)){
    if(!rb_config_parse_and_load(1, &file, self) == Qfalse){
      FcConfigDestroy(conf);
      RTYPEDDATA_DATA(self) = 0;
      rb_raise(rb_eRuntimeError, "Cannot load FcConfig");
    }
  }
  return self;
}

// FcConfigFilename -- Find a config file
static VALUE rb_config_filename(VALUE klass, VALUE filename){
  // in FONTCONFIG_PATH
  char* fname = 0;
  if(RTEST(filename)){
    if(TYPE(filename) != T_STRING)
      filename = rb_any_to_s(filename);
    fname = RSTRING_PTR(filename);
  }
  char* res = FcConfigFilename(fname);
  if(!res){
    rb_raise(rb_eRuntimeError, "cannot get config filename");
  }
  VALUE rb_res = rb_str_new2(res);
  FcStrFree(res); //?
  return rb_res;
}

// FcConfigGetCurrent -- Return current configuration
//// FcConfigReference -- Increment config reference count
static VALUE rb_config_get_current(VALUE klass){
  FcConfig* conf = FcConfigGetCurrent();
  if(!conf)
    rb_raise(rb_eRuntimeError, "no current in FcConfigGetCurrent");
  FcConfigReference(conf);
  return config_wrap(conf);
}

// FcConfigSetCurrent -- Set configuration as default
static VALUE rb_config_set_current(VALUE self){
  return BOOL2VAL(FcConfigSetCurrent(CONFIG_UNWRAP(self)));
}

// FcConfigUptoDate -- Check timestamps on config files
static VALUE rb_config_up_to_date_p(VALUE self){
  return BOOL2VAL(FcConfigUptoDate(CONFIG_UNWRAP(self)));
}

// FcConfigHome -- return the current home directory.
static VALUE rb_config_home(VALUE klass){
  char* home = FcConfigHome();
  if(!home)
    return Qnil;
  return rb_str_new2(home);
}

// FcConfigEnableHome -- controls use of the home directory.
static VALUE rb_config_enable_home(int argc, VALUE *argv, VALUE klass){
  VALUE enable;
  if(rb_scan_args(argc, argv, "01", &enable) == 0){
    enable = Qtrue;
  }
  return BOOL2VAL(FcConfigEnableHome(RTEST(enable)));
}

// FcConfigBuildFonts -- Build font database
static VALUE rb_config_build_fonts(VALUE self){
  //TODO: set some flag?
  //changes after this have undetermined effects
  return BOOL2VAL(FcConfigBuildFonts(CONFIG_UNWRAP(self)));
}

VALUE FcStrList2Array(FcStrList* list){
  VALUE res = rb_ary_new();
  char* str;
  while(str = FcStrListNext(list)){
    rb_ary_push(res, rb_str_new2(str));
  }
  FcStrListDone(list);
  return res;
}

// FcConfigGetConfigDirs -- Get config directories
static VALUE rb_config_get_config_dirs(VALUE self){
  FcStrList * list = FcConfigGetConfigDirs(CONFIG_UNWRAP(self));
  return FcStrList2Array(list);
}

// FcConfigGetFontDirs -- Get font directories
static VALUE rb_config_get_font_dirs(VALUE self){
  FcStrList * list = FcConfigGetFontDirs(CONFIG_UNWRAP(self));
  return FcStrList2Array(list);
}

// FcConfigGetConfigFiles -- Get config files
static VALUE rb_config_get_config_files(VALUE self){
  FcStrList * list = FcConfigGetConfigFiles(CONFIG_UNWRAP(self));
  return FcStrList2Array(list);
}

// FcConfigGetCacheDirs -- return the list of directories searched for cache files
static VALUE rb_config_get_cache_dirs(VALUE self){
  FcStrList * list = FcConfigGetCacheDirs(CONFIG_UNWRAP(self));
  return FcStrList2Array(list);
}

// FcConfigGetRescanInterval -- Get config rescan interval
static VALUE rb_config_get_rescan_interval(VALUE self){
  return INT2FIX(FcConfigGetRescanInterval(CONFIG_UNWRAP(self)));
}

// FcConfigSetRescanInterval -- Set config rescan interval
static VALUE rb_config_set_rescan_interval(VALUE self, VALUE interval){
  interval = rb_to_int(interval);
  return INT2FIX(FcConfigSetRescanInterval(CONFIG_UNWRAP(self), NUM2INT(interval)));
}

// FcConfigGetSysRoot -- Obtain the system root directory
static VALUE rb_config_get_sys_root(VALUE self){
  const char* res = FcConfigGetSysRoot(CONFIG_UNWRAP(self));
  if(!res)
    return Qnil;
  return rb_str_new2(res);
}

// FcConfigSetSysRoot -- Set the system root directory
static VALUE rb_config_set_sys_root(VALUE self, VALUE root){
  FcConfigSetSysRoot(CONFIG_UNWRAP(self), StringValuePtr(root));
  return self;
}

// FcFontRenderPrepare -- Prepare pattern for loading font file
static VALUE rb_config_font_render_prepare(VALUE self, VALUE pat, VALUE font){
  if(!is_fc_pattern(pat) || !is_fc_pattern(font)){
    rb_raise(rb_eArgError, "arguments must be Fontconfig::Pattern");
  }
  FcPattern * res = FcFontRenderPrepare(CONFIG_UNWRAP(self), (FcPattern*)RTYPEDDATA_DATA(pat), (FcPattern*)RTYPEDDATA_DATA(font));
  if(!res){
    rb_raise(rb_eRuntimeError, "FcFontRenderPrepare returned NULL");
  }
  return pattern_wrap(res);
}

// FcConfigAppFontClear -- Remove all app fonts from font database
static VALUE rb_config_app_font_clear(VALUE self){
  FcConfigAppFontClear(CONFIG_UNWRAP(self));
  return self;
}

// FcConfigAppFontAddFile -- Add font file to font database
static VALUE rb_config_app_font_add_file(VALUE self, VALUE path){
  return BOOL2VAL(FcConfigAppFontAddFile(CONFIG_UNWRAP(self), StringValuePtr(path)));
}

// FcConfigAppFontAddDir -- Add fonts from directory to font database
static VALUE rb_config_app_font_add_dir(VALUE self, VALUE path){
  return BOOL2VAL(FcConfigAppFontAddDir(CONFIG_UNWRAP(self), StringValuePtr(path)));
}


// FcFontMatch -- Return best font
// This function should be called only after FcConfigSubstitute and FcDefaultSubstitute called on pattern
// TODO: check for this?
static VALUE rb_config_font_match(VALUE self, VALUE pattern){
  if(!is_fc_pattern(pattern)){
    rb_raise(rb_eArgError, "argument must be Fontconfig::Pattern");
  }
  FcResult res; // result of FcFontRenderPrepare for font and the provided pattern //???
  FcPattern* match = FcFontMatch(CONFIG_UNWRAP(self), RTYPEDDATA_DATA(pattern), &res);
  if(match){
    return pattern_wrap(match);
  }
  return Qnil;
}

VALUE symPattern, symFont, symScan;

FcMatchKind sym2match_kind(VALUE match_kind){
  if(match_kind == symPattern)
    return FcMatchPattern;
  else if(match_kind == symFont)
    return FcMatchFont;
  else if(match_kind == symScan)
    return FcMatchScan;
  else
    rb_raise(rb_eArgError, "match_kind argument must be one of [:pattern, :font, :scan]");
  return FcMatchPattern;
}

// FcConfigSubstitute -- Execute substitutions
static VALUE rb_config_substitute(int argc, VALUE *argv, VALUE self){
  VALUE pattern, match_kind;
  rb_scan_args(argc, argv, "11", &pattern, &match_kind);

  if(!is_fc_pattern(pattern)){
    rb_raise(rb_eArgError, "argument must be Fontconfig::Pattern");
  }
  FcMatchKind kind = FcMatchPattern; //TODO: default?
  if(RTEST(match_kind))
    kind = sym2match_kind(match_kind);

  return BOOL2VAL(FcConfigSubstitute(CONFIG_UNWRAP(self), RTYPEDDATA_DATA(pattern), kind));
}

// FcConfigSubstituteWithPat -- Execute substitutions
static VALUE rb_config_substitute_with_pat(int argc, VALUE *argv, VALUE self){
  VALUE pattern, pat, match_kind;
  int n = rb_scan_args(argc, argv, "12", &pattern, &pat, &match_kind);

  if(n == 2){
    match_kind = pat;
    pat = Qnil;
  }

  if(!is_fc_pattern(pattern)){
    rb_raise(rb_eArgError, "argument must be Fontconfig::Pattern");
  }
  FcPattern* pat_pat = 0;
  if(RTEST(pat)){
    if(!is_fc_pattern(pat)){
      rb_raise(rb_eArgError, "argument must be Fontconfig::Pattern");
    }
    pat_pat = RTYPEDDATA_DATA(pat);
  }
  FcMatchKind kind = FcMatchPattern; //TODO: default?
  if(RTEST(match_kind))
    kind = sym2match_kind(match_kind);

  return BOOL2VAL(FcConfigSubstituteWithPat(CONFIG_UNWRAP(self), RTYPEDDATA_DATA(pattern), pat_pat, kind));
}

// FcFontList -- List fonts
static VALUE rb_config_font_list(int argc, VALUE *argv, VALUE self){
  // FcPattern *p, FcObjectSet *os(optional)
  VALUE pattern, object_set;
  FcObjectSet* os = 0;
  rb_scan_args(argc, argv, "11", &pattern, &object_set);
  if(!is_fc_pattern(pattern)){
    rb_raise(rb_eArgError, "argument must be Fontconfig::Pattern");
  }
  if(object_set){
    if(is_FcObjectSet(object_set)){
      os = RTYPEDDATA_DATA(object_set);
    } else {
      //TODO: convert array to os?
      rb_raise(rb_eArgError, "os must be Fontconfig::ObjectSet");
    }
  }
  FcFontSet* set = FcFontList(CONFIG_UNWRAP(self), RTYPEDDATA_DATA(pattern), os);

  return self;
}

// FcConfigGetFonts -- Get config font set
static VALUE config_get_fonts(VALUE self, FcSetName set_name){
  FcFontSet* set = FcConfigGetFonts(CONFIG_UNWRAP(self), set_name);

}

static VALUE rb_config_get_system_fonts(VALUE self){
  return config_get_fonts(self, FcSetSystem);
}

static VALUE rb_config_get_application_fonts(VALUE self){
  return config_get_fonts(self, FcSetApplication);
}

// FcFontSort -- Return list of matching fonts
static VALUE rb_config_font_sort(int argc, VALUE *argv, VALUE self){
  VALUE pattern, trim;
  rb_scan_args(argc, argv, "11", &pattern, &trim);
  //charset - ignored.
  if(!RTEST(pattern) || !is_fc_pattern(pattern)){
    rb_raise(rb_eArgError, "pattern must be Fonconfig::Pattern");
  }
  FcResult res;
  FcFontSet* set = FcFontSort(CONFIG_UNWRAP(self), pattern_unwrap(pattern), RTEST(trim), 0, &res);
  return font_set_wrap(set);
}

void Init_fontconfig_config(){
  rb_cFcConfig = rb_define_class_under(rb_cFontconfig, "Config", rb_cObject);
  rb_define_singleton_method(rb_cFcConfig, "new", rb_config_new, -1);
  rb_define_method(rb_cFcConfig, "parse_and_load", rb_config_parse_and_load, -1);
  rb_define_singleton_method(rb_cFcConfig, "filename", rb_config_filename, 1);
  rb_define_singleton_method(rb_cFontconfig, "current_config", rb_config_get_current, 0);
  rb_define_singleton_method(rb_cFcConfig, "get_current", rb_config_get_current, 0);
  rb_define_method(rb_cFcConfig, "set_current!", rb_config_set_current, 0);
  rb_define_method(rb_cFcConfig, "up_to_date?", rb_config_up_to_date_p, 0);
  rb_define_singleton_method(rb_cFcConfig, "home", rb_config_home, 0);
  rb_define_singleton_method(rb_cFcConfig, "enable_home!", rb_config_enable_home, -1);
  rb_define_method(rb_cFcConfig, "build_fonts!", rb_config_build_fonts, 0);

  rb_define_method(rb_cFcConfig, "config_dirs", rb_config_get_config_dirs, 0);
  rb_define_method(rb_cFcConfig, "font_dirs", rb_config_get_font_dirs, 0);
  rb_define_method(rb_cFcConfig, "config_files", rb_config_get_config_files, 0);
  rb_define_method(rb_cFcConfig, "cache_dirs", rb_config_get_cache_dirs, 0);

  rb_define_method(rb_cFcConfig, "rescan_interval",  rb_config_get_rescan_interval, 0);
  rb_define_method(rb_cFcConfig, "rescan_interval=", rb_config_set_rescan_interval, 1);
  rb_define_method(rb_cFcConfig, "sys_root", rb_config_get_sys_root, 0);
  rb_define_method(rb_cFcConfig, "set_sys_root!", rb_config_set_sys_root, 1);

  rb_define_method(rb_cFcConfig, "font_render_prepare", rb_config_font_render_prepare, 2);

  rb_define_method(rb_cFcConfig, "app_font_clear!", rb_config_app_font_clear, 0);
  rb_define_method(rb_cFcConfig, "app_font_add_file", rb_config_app_font_add_file, 1);
  rb_define_method(rb_cFcConfig, "app_font_add_dir", rb_config_app_font_add_dir, 1);

  rb_define_method(rb_cFcConfig, "font_match", rb_config_font_match, 1);

  symPattern = ID2SYM(rb_intern("pattern"));
  symFont = ID2SYM(rb_intern("font"));
  symScan = ID2SYM(rb_intern("scan"));
  rb_define_method(rb_cFcConfig, "substitute", rb_config_substitute, -1);
  rb_define_method(rb_cFcConfig, "substitute_with_pat", rb_config_substitute_with_pat, -1);
  rb_define_method(rb_cFcConfig, "font_sort", rb_config_font_sort, -1);

// FcConfigGetBlanks -- Get config blanks
}