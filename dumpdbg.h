#ifndef DUMPDBG_H
#define DUMPDBG_H 1

#include <Print.h>

typedef enum {
    D_S8 = 0,
    D_U8,
    D_S16,
    D_U16,
    D_BOOL,
    D_ENUM,
    D_DMP,
    D_STR,
    _D_CTRL_START,
    D_LF,
    D_CLR,
    D_END
} dd_type_e;


typedef struct {
    dd_type_e     type;
    const char*   label;
    const void*   ptr;
    unsigned char base;
    char**        enum_data;
} dd_elem_t;


// I'm flexing c++ here
class Dumpable {
  public:
    virtual void dd_do_dump(Print*) = 0;
};


/// Helpers to create a dd_elem_t array
/// B suffix means base variant
/// L suffix means labeled variant

// Make Int (s8/u8/s16/u16)
#define D_MKINT_LB(type, label, ptr, base) { (type), (label), (ptr), (base), NULL }

#define D_MKINT(type, ptr) D_MKINT_LB((type), NULL, (ptr), 10)

#define D_MKINT_L(type, label, ptr) D_MKINT_LB((type), (label), (ptr), 10)

#define D_MKINT_B(type, ptr, base) D_MKINT_LB((type), NULL, (ptr), (base))


// Make Bool
#define D_MKBOOL_L(label, ptr) { D_BOOL, (label), (ptr), 0, NULL }

#define D_MKBOOL(ptr) D_MKBOOL_L(NULL, (ptr))


// Make Enum
#define D_MKENUM_L(label, ptr, enum_data) { D_ENUM, (label), (ptr), 0, (enum_data) }

#define D_MKENUM(ptr, enum_data) D_MKENUM((label), (ptr), (enum_data))


// Make Dumpable
#define D_MKDMP_L(label, ptr) { D_DMP, (label), (ptr), 0, NULL }

#define D_MKDMP(ptr) D_MKDMP_L(NULL, (ptr))


// Make String
#define D_MKSTR(str) { D_STR, NULL, (str), 0, NULL }


// Make Controls
#define D_MKLF() { D_LF, NULL, NULL, 0, NULL }

#define D_MKCLR() { D_CLR, NULL, NULL, 0, NULL }

#define D_MKEND() { D_END, NULL, NULL, 0, NULL }


void dd_dump(dd_elem_t* data, Print* out);

#endif
