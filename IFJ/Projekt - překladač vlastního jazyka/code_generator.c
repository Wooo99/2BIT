/**
 * @file code_generator.c
 * 
 * @brief implementaiton of functions for code generation
 * 
 * @date 9.12.2020
 * 
 * @copyright Brno University of Technology, Czech Republic
 * 
 * @author Samuel Repka,     xrepka07, +421 907 587 090
 * @author Michal Reznik,    xrezni28, +420 731 748 122
 * @author Jiri Hofirek,     xhofir06, +420 774 883 191
 * @author Timotej Kamensky  xkamen24, +421 944 687 328
 * 
 * */

//##################################################################################################################################
//##########    BUILT-IN FUNCTIONS    ##############################################################################################
//##################################################################################################################################

/*##########    built in funcitons for string operations    ###########################*/
//works
/**
 * @brief built-in strlen function. Page 10
*/
#define BUILT_IN_FUNC_LEN \
    "\n\n                                         \
    \n# built-in function for lenght of a string \
    \nLABEL len                                  \
    \nPUSHFRAME                                  \
    \nDEFVAR LF@__ret_000__                      \
    \nSTRLEN LF@__ret_000__ LF@s                 \
    \nPOPFRAME                                   \
    \nRETURN"

/**
 * @brief built-int function to return ASCII (ordinal)
 * value on a given index in string, else false. Page 10
 * */
#define BUILT_IN_FUNC_ORD \
    "\n\n                                                             \
    \n#built-in function for ASCII value of the given sign in string \
    \nLABEL $ord                                                     \
    \nPUSHFRAME                                                      \
    \nDEFVAR   LF@strlen                                             \
    \nSTRLEN   LF@strlen LF@s                                        \
    \nJUMPIFEQ $ord_end LF@strlen int@0                              \
    \nDEFVAR   LF@bool1                                              \
    \nGT       LF@bool1 LF@i LF@strlen                               \
    \nJUMPIFEQ $ord_end LF@bool1 bool@true                           \
    \nLT       LF@bool1 LF@i int@0                                   \
    \nJUMPIFEQ $ord_end LF@bool1 bool@true                           \
    \nDEFVAR   LF@$__ret_000__                                       \
    \nSTRI2INT LF@__ret_000__ LF@s LF@i                              \
    \nLABEL     $ord_end                                             \
    \nPOPFRAME                                                       \
    \nRETURN"

/**
 * @brief built-in function to return single-char
 * @note TODO v tejto funkcii aj v dalsich mam vratit hodnotu 1 ako chybu. AKO?
 * */
#define BUILT_IN_FUNC_CHR \
    "\n    LABEL   $chr                      \
    \nPUSHFRAME                             \
    \nDEFVAR   LF@returnvalue               \
    \nMOVE     LF@returnvalue string@       \
    \nDEFVAR   LF@bool1                     \
    \nDEFVAR   LF@bool2                     \
    \nLT       LF@bool1 LF@i  int@256       \
    \nGT       LF@bool2 LF@i  int@-1        \
    \nAND      LF@bool1 LF@bool1 LF@bool2   \
    \nJUMPIFNEQ $chr_end LF@bool1 bool@true \
    \nINT2CHAR LF@returnvalue LF@i          \
    \nLABEL    $chr_end                     \
    \nPOPFRAME                              \
    \nRETURN                                \
    \nLABEL _chr_float2int                       \
    \nPUSHFRAME                             \
    \nDEFVAR LF@__ret_000__                 \
    \nPUSHS LF@f                            \
    \nFLOAT2INTS                            \
    \nPOPS LF@$__ret_000__                  \
    \nPOPFRAME                              \
    \nRETURN                                \
    \nLABEL _chr_int2float                       \
    \nPUSHFRAME                             \
    \nDEFVAR LF@__ret_000__                 \
    \nPUSHS LF@i                            \
    \nINT2FLOATS                            \
    \nPOPS LF@$__ret_000__                  \
    \nPOPFRAME                              \
    \nRETURN"

/**
 * @brief built in substr function. Page 10
 * */
#define BUILT_IN_FUNC_SUBSTR \
    "\n    # substr                                      \
    \nLABEL   $substr                                   \
    \nPUSHFRAME                                         \
    \nDEFVAR LF@__ret_000__                             \
    \nDEFVAR LF@__ret_001__                             \
    \nDEFVAR LF@str_len                                 \
    \nDEFVAR LF@str                                     \
    \nMOVE   LF@str         string@                     \
    \nSTRLEN LF@str_len     LF@s                        \
    \nDEFVAR LF@bool1                                   \
    \nJUMPIFEQ $substr_end_bad LF@strlen int@0          \
    \nJUMPIFEQ $substr_end_bad LF@n int@0               \
    \nGT     LF@bool1 LF@i           LF@str_len         \
    \nJUMPIFEQ $substr_end LF@bool1 bool@true           \
    \nDEFVAR LF@total_len                               \
    \nMOVE   LF@total_len   int@0                       \
    \nADD    LF@total_len   LF@s LF@i                   \
    \nGT     LF@bool1       LF@total_len   LF@str_len   \
    \nJUMPIFEQ $substr_long LF@bool1 bool@true          \
    \nJUMP   $substr_short                              \
    \nLABEL   $substr_long                              \
    \nDEFVAR LF@newstr_len                              \
    \nMOVE   LF@newstr_len  LF@s                        \
    \nSUB    LF@total_len   LF@total_len  LF@str_len    \
    \nSUB    LF@newstr_len  LF@newstr_len LF@total_len  \
    \nJUMP   $substr_long_short                         \
    \nLABEL   $substr_short                             \
    \nDEFVAR LF@newstr_len                              \
    \nMOVE   LF@newstr_len  LF@s                        \
    \nLABEL   $substr_long_short                        \
    \nDEFVAR LF@letter                                  \
    \nDEFVAR LF@index                                   \
    \nMOVE   LF@index LF@i                              \
    \nLABEL   $substr_while_start                       \
    \nGETCHAR LF@letter LF@s LF@index                   \
    \nCONCAT LF@__ret_000__ LF@__ret_000__ LF@letter    \
    \nADD    LF@index LF@index int@1                    \
    \nSUB    LF@newstr_len LF@newstr_len int@1          \
    \nJUMPIFNEQ $substr_while_start LF@newstr_len int@1 \
    \nJUMP $substr_end_good                             \
    \nLABEL   $substr_end_bad                           \
    \nMOVE LF@__ret_001__ int@1                         \
    \nJUMP $substr_end                                  \
    \nLABEL $substr_end_good                            \
    \nMOVE LF@__ret_001__ int@0                         \
    \nLABEL $substr_end                                 \
    \nPOPFRAME                                          \
    \nRETURN "

/*####    built in functions for reading of literals and print of terms    ###########################*/
#define BUILT_IN_FUNC_INPUTI \
    "\nLABEL inputi \
   \nPUSHFRAME \
\nDEFVAR LF@__ret_000__ \
\nDEFVAR LF@__ret_001__ \
\nREAD LF@__ret_000__ int \
\nJUMPIFEQ while_inputi_failed LF@__ret_000__ nil@nil  \
\nMOVE LF@__ret_001__ int@0 \
\nPOPFRAME \
\nRETURN \
\nLABEL while_inputi_failed \
\nMOVE LF@__ret_000__ int@0 \
\nMOVE LF@__ret_001__ int@1 \
\nPOPFRAME \
\nRETURN"

#define BUILT_IN_FUNC_INPUTS \
    "\nLABEL inputs \
\nPUSHFRAME \
\nDEFVAR LF@__ret_000__ \
\nDEFVAR LF@__ret_001__ \
\nREAD LF@__ret_000__ string \
\nJUMPIFEQ while_inputs_failed LF@__ret_000__ nil@nil  \
\nPOPFRAME \
\nRETURN \
\nLABEL while_inputs_failed \
\nMOVE LF@__ret_001__ int@1 \
\nPOPFRAME \
\nRETURN"

#define BUILT_IN_FUNC_INPUTF \
    "\nLABEL inputf   \
    \nPUSHFRAME \
\nDEFVAR LF@__ret_000__ \
\nDEFVAR LF@__ret_001__ \
\nREAD LF@__ret_000__ float \
\nJUMPIFEQ while_inputi_failed LF@__ret_000__ nil@nil \
\nPOPFRAME \
\nRETURN \
\nLABEL while_inputf_failed \
\nMOVE LF@__ret_001__ int@1 \
\nPOPFRAME \
\nRETURN"

/*built-in functions for conversion of number types*/
#define BUILT_IN_FUNC_INT_2_FLOAT \
    "\nLABEL int2float           \
    \nPUSHFRAME                 \
    \nDEFVAR LF@__ret_000__     \
    \nPUSHS LF@i                \
    \nINT2FLOATS                \
    \nPOPS LF@$__ret_000__      \
    \nPOPFRAME                  \
    \nRETURN"

#define BUILT_IN_FUNC_FLOAT_2_INT \
    "\nLABEL float2int           \
    \nPUSHFRAME               \
    \nDEFVAR LF@__ret_000__     \
    \nPUSHS LF@f                \
    \nFLOAT2INTS                \
    \nPOPS LF@$__ret_000__      \
    \nPOPFRAME                  \
    \nRETURN"

/*other built-in functions*/
#define BUILT_IN_FUNC_PRINT \
    "\nLABEL print # SUPPORTS UP TO 15 VALUES!!!!!!!!!!!! \
    \nPUSHFRAME                                          \
    \nDEFVAR LF@__counter__                              \
    \nMOVE LF@__counter__ LF@__param_n__                 \
    \nJUMPIFEQ __endprint__ int@0 LF@__counter__         \
    \nWRITE LF@__par000__                                \
    \nSUB LF@__counter__ LF@__counter__ int@1            \
    \nJUMPIFEQ __endprint__ int@0 LF@__counter__         \
    \nWRITE LF@__par001__                                \
    \nSUB LF@__counter__ LF@__counter__ int@1            \
    \nJUMPIFEQ __endprint__ int@0 LF@__counter__         \
    \nWRITE LF@__par002__                                \
    \nSUB LF@__counter__ LF@__counter__ int@1            \
    \nJUMPIFEQ __endprint__ int@0 LF@__counter__         \
    \nWRITE LF@__par003__                                \
    \nSUB LF@__counter__ LF@__counter__ int@1            \
    \nJUMPIFEQ __endprint__ int@0 LF@__counter__         \
    \nWRITE LF@__par004__                                \
    \nSUB LF@__counter__ LF@__counter__ int@1            \
    \nJUMPIFEQ __endprint__ int@0 LF@__counter__         \
    \nWRITE LF@__par005__                                \
    \nSUB LF@__counter__ LF@__counter__ int@1            \
    \nJUMPIFEQ __endprint__ int@0 LF@__counter__         \
    \nWRITE LF@__par006__                                \
    \nSUB LF@__counter__ LF@__counter__ int@1            \
    \nJUMPIFEQ __endprint__ int@0 LF@__counter__         \
    \nWRITE LF@__par007__                                \
    \nSUB LF@__counter__ LF@__counter__ int@1            \
    \nJUMPIFEQ __endprint__ int@0 LF@__counter__         \
    \nWRITE LF@__par008__                                \
    \nSUB LF@__counter__ LF@__counter__ int@1            \
    \nJUMPIFEQ __endprint__ int@0 LF@__counter__         \
    \nWRITE LF@__par009__                                \
    \nSUB LF@__counter__ LF@__counter__ int@1            \
    \nJUMPIFEQ __endprint__ int@0 LF@__counter__         \
    \nWRITE LF@__par010__                                \
    \nSUB LF@__counter__ LF@__counter__ int@1            \
    \nJUMPIFEQ __endprint__ int@0 LF@__counter__         \
    \nWRITE LF@__par011__                                \
    \nSUB LF@__counter__ LF@__counter__ int@1            \
    \nJUMPIFEQ __endprint__ int@0 LF@__counter__         \
    \nWRITE LF@__par012__                                \
    \nSUB LF@__counter__ LF@__counter__ int@1            \
    \nJUMPIFEQ __endprint__ int@0 LF@__counter__         \
    \nWRITE LF@__par013__                                \
    \nSUB LF@__counter__ LF@__counter__ int@1            \
    \nJUMPIFEQ __endprint__ int@0 LF@__counter__         \
    \nWRITE LF@__par014__                                \
    \nSUB LF@__counter__ LF@__counter__ int@1            \
    \nLABEL __endprint__                                 \
    \nPOPFRAME                                           \
    \nRETURN"
