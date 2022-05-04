/**
 * @file pars_string.h
 * 
 * @note UNUSED afaik, because was ultimately implemented 
 * directly as a part of scanner and not as a separate function
 * 
 * @brief header file for parsing strings
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
#include "dynamic_string.h"

Dynamic_string *pars_string();
void backslash(Dynamic_string *sp);