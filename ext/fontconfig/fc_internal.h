//fontconfig internal



struct _FcPattern {
    int       num;
    int       size;
    intptr_t  elts_offset;
    // FcRef     ref;
    int ref_here_dot_not_use;
};

typedef int FcObject;

typedef enum _FcValueBinding {
    FcValueBindingWeak, FcValueBindingStrong, FcValueBindingSame
} FcValueBinding;

typedef struct _FcValueList {
    struct _FcValueList *next;
    FcValue   value;
    FcValueBinding  binding;
} FcValueList, *FcValueListPtr;

typedef struct _FcPatternElt {
    FcObject    object;
    FcValueList   *values;
} FcPatternElt;

#define FcOffsetToPtr(b,o,t)  ((t *) ((intptr_t) (b) + (o)))
#define FcEncodedOffsetMember(s,m,t) FcOffsetToPtr(s,FcOffsetDecode((s)->m), t)
#define FcIsEncodedOffset(p)  ((((intptr_t) (p)) & 1) != 0)
#define FcPointerMember(s,m,t)  (FcIsEncodedOffset((s)->m) ? \
         FcEncodedOffsetMember (s,m,t) : \
         (s)->m)
#define FcOffsetMember(s,m,t)     FcOffsetToPtr(s,(s)->m,t)
#define FcPatternElts(p)  FcOffsetMember(p,elts_offset,FcPatternElt)
#define FcPatternEltValues(pe)  FcPointerMember(pe,values,FcValueList)

const char *
FcObjectName (FcObject object);
FcObject
FcObjectFromName (const char * name);
FcPatternElt *
FcPatternObjectFindElt (const FcPattern *p, FcObject object);

#define FcValueListNext(vl) FcPointerMember(vl,next,FcValueList)
