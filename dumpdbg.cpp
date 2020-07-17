#include "dumpdbg.h"

static const char* csi = "\x1b[";

void dd_dump(dd_elem_t* data, Print* out){
    // The union is to save stack space (8 bytes if u8 and s8 are packed, 10 bytes otherwise)
    union {
        unsigned char u8;
        signed char s8;
        unsigned short u16;
        signed short s16;

        char* str;

        Dumpable* dmp;
    } tmp;

    // Current row
    unsigned char c_row = 1;
    
    for(;data->type != D_END;data++){
        if(data->label != NULL){
            out->print(data->label);
            out->print(':');
        }
                
        switch(data->type){
            case D_U8:
                tmp.u8 = *(unsigned char*)data->ptr;
                out->print(tmp.u8, data->base);
                break;

            case D_S8:
                tmp.s8 = *(signed char*)data->ptr;
                out->print(tmp.s8, data->base);
                break;
                
            case D_U16:
                tmp.u16 = *(unsigned short*)data->ptr;
                out->print(tmp.u16, data->base);
                break;

            case D_S16:
                tmp.s16 = *(signed short*)data->ptr;
                out->print(tmp.s16, data->base);
                break;

            case D_BOOL:
                tmp.u8 = *(unsigned char*)data->ptr;
                out->print(tmp.u8 ? 't' : 'f');
                break;

            case D_ENUM:
                tmp.u8 = *(unsigned char*)data->ptr;
                out->print(data->enum_data[tmp.u8]);
                break;

            case D_DMP:
                tmp.dmp = (Dumpable*)data->ptr;
                tmp.dmp->dd_do_dump(out);
                break;
                        
            case D_STR:
                tmp.str = (char*)data->ptr;
                out->print(tmp.str);
                break;
            
            case D_LF:
                out->print(csi);
                out->print('K'); // kill line (delete from cursor to end)
                out->print('\n');

                c_row++;
                break;
                        
            case D_CLR:
                out->print(csi);
                out->print('H');  // move cursor to up-left
                break;

            default:
                // Makes the compiler happy about _D_CTRL_START and D_END not being handled
                break;
        }
    
        if(data->type < _D_CTRL_START)
            out->print(' ');

    }
}
