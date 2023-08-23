# struct arg_hdr

*Defined at ./lib/argtable/argtable3.h#95*

 The arg_hdr struct defines properties that are common to all arg_xxx structs. The argtable library requires each arg_xxx struct to have an arg_hdr struct as its first data member. The argtable library functions then use this data to identify the properties of the command line option, such as its option tags, datatype string, and glossary strings, and so on. Moreover, the arg_hdr struct contains pointers to custom functions that are provided by each arg_xxx struct which perform the tasks of parsing that particular arg_xxx arguments, performing post-parse checks, and reporting errors. These functions are private to the individual arg_xxx source code and are the pointer to them are initiliased by that arg_xxx struct's constructor function. The user could alter them after construction if desired, but the original intention is for them to be set by the constructor and left unaltered.



## Members

public char flag

public const char * shortopts

public const char * longopts

public const char * datatype

public const char * glossary

public int mincount

public int maxcount

public void * parent

public arg_resetfn * resetfn

public arg_scanfn * scanfn

public arg_checkfn * checkfn

public arg_errorfn * errorfn

public void * priv



