/**
 * @file pars_string.c
 * 
 * @note UNUSED afaik, because was ultimately implemented 
 * directly as a part of scanner and not as a separate function
 * 
 * @brief function for parsing strings
 * 
 * @date 27.11.2020
 * 
 * @copyright Brno University of Technology, Czech Republic
 * 
 * @author Samuel Repka,     xrepka07, +421 907 587 090
 * @author Michal Reznik,    xrezni28, +420 731 748 122
 * @author Jiri Hofirek,     xhofir06, +420 774 883 191
 * @author Timotej Kamensky  xkamen24, +421 944 687 328
 * 
 * */
#include "pars_string.h"

Dynamic_string *pars_string() {
    Dynamic_string *sp = (Dynamic_string *) malloc(sizeof(Dynamic_string));
    if (sp == NULL)
        return NULL;
    dynamic_string_init(sp);

    while (1) {
        int c = getchar();
        switch (c) {
            case EOF:
                sp->error = 1;
                return sp;
            case (int) '"':
                return sp;
            case (int) '\\':
                backslash(sp);
                if (sp->error != 0)
                    return sp;
                break;
            default:
                dynamic_string_add_char(sp, (char) c);
                break;
        }
    }
}

void backslash(Dynamic_string *sp) {
    int c = getchar();
    switch (c) {
        case EOF:
            sp->error = 1;
            break;
        case (int) 'x': // hex ascii code
            break;
        case (int) 'n':
            dynamic_string_add_char(sp, '\n');
            break;
        case (int) '\\':
            dynamic_string_add_char(sp, '\\');
            break;
        case (int) '\"':
            dynamic_string_add_char(sp, '\"');
            break;
        case (int) 't':
            dynamic_string_add_char(sp, '\t');
            break;
        default:
            sp->error = 2;
            break;
    }
}
