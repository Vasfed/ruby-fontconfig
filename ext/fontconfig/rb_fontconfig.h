#include <ruby.h>
#include <fontconfig/fontconfig.h>

extern VALUE rb_cFontconfig;


#define BOOL2VAL(p) ((p) ? Qtrue : Qfalse)

int is_fc_pattern(VALUE val);
VALUE pattern_wrap(FcPattern* p);