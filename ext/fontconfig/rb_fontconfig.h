#include <ruby.h>
#include <fontconfig/fontconfig.h>

extern VALUE rb_cFontconfig;


#define BOOL2VAL(p) ((p) ? Qtrue : Qfalse)

int is_fc_pattern(VALUE val);
VALUE pattern_wrap(FcPattern* p);
#define pattern_unwrap(c) ((FcPattern*)RTYPEDDATA_DATA(c))

int is_FcObjectSet(VALUE val);

VALUE font_set_wrap(FcFontSet* p);
int is_FcFontSet(VALUE val);

int is_fc_config(VALUE val);
#define CONFIG_UNWRAP(c) ((FcConfig*)RTYPEDDATA_DATA(c))