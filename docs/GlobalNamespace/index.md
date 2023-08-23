# Global Namespace



## Records

* [](.md)
* [](.md)
* [_internal_arg_dstr](_internal_arg_dstr.md)
* [arg_hdr](arg_hdr.md)
* [arg_rem](arg_rem.md)
* [arg_lit](arg_lit.md)
* [arg_int](arg_int.md)
* [arg_dbl](arg_dbl.md)
* [arg_str](arg_str.md)
* [arg_rex](arg_rex.md)
* [arg_file](arg_file.md)
* [arg_date](arg_date.md)
* [arg_end](arg_end.md)
* [arg_cmd_info](arg_cmd_info.md)
* [](.md)
* [](.md)
* [](.md)


## Functions

### input_new_buffer

*InputBuffer * input_new_buffer()*

*Defined at src/input.c#8*

 Create a new InputBuffer

 Create a new input buffer

### input_prompt

*void input_prompt(char * text)*

*Defined at src/input.c#19*

 Print a prompt

 Print a prompt to the user

### input_read

*int input_read(InputBuffer * input_buffer)*

*Defined at src/input.c#22*

 Read a line of input

 Read input from stdin

### input_close_buffer

*void input_close_buffer(InputBuffer * input_buffer)*

*Defined at src/input.c#39*

 Close an input buffer

 Close and free an InputBuffer

### meta_execute_command

* meta_execute_command(InputBuffer * input_buffer)*

*Defined at src/meta.c#7*

 Execute a meta command (e.g. .exit)

 Execute a meta command (e.g. .exit) and return the result.

### log_level_string

*const char * log_level_string(int level)*

### log_set_lock

*void log_set_lock(log_LockFn fn, void * udata)*

### log_set_level

*void log_set_level(int level)*

### log_set_quiet

*void log_set_quiet(_Bool enable)*

### log_add_callback

*int log_add_callback(log_LogFn fn, void * udata, int level)*

### log_add_fp

*int log_add_fp(FILE * fp, int level)*

### log_log

*void log_log(int level, const char * file, int line, const char * fmt)*

### arg_rem

*struct arg_rem * arg_rem(const char * datatype, const char * glossary)*

** arg_xxx constructor functions ********************************

### arg_lit0

*struct arg_lit * arg_lit0(const char * shortopts, const char * longopts, const char * glossary)*

### arg_lit1

*struct arg_lit * arg_lit1(const char * shortopts, const char * longopts, const char * glossary)*

### arg_litn

*struct arg_lit * arg_litn(const char * shortopts, const char * longopts, int mincount, int maxcount, const char * glossary)*

### arg_int0

*struct arg_int * arg_int0(const char * shortopts, const char * longopts, const char * datatype, const char * glossary)*

### arg_int1

*struct arg_int * arg_int1(const char * shortopts, const char * longopts, const char * datatype, const char * glossary)*

### arg_intn

*struct arg_int * arg_intn(const char * shortopts, const char * longopts, const char * datatype, int mincount, int maxcount, const char * glossary)*

### arg_dbl0

*struct arg_dbl * arg_dbl0(const char * shortopts, const char * longopts, const char * datatype, const char * glossary)*

### arg_dbl1

*struct arg_dbl * arg_dbl1(const char * shortopts, const char * longopts, const char * datatype, const char * glossary)*

### arg_dbln

*struct arg_dbl * arg_dbln(const char * shortopts, const char * longopts, const char * datatype, int mincount, int maxcount, const char * glossary)*

### arg_str0

*struct arg_str * arg_str0(const char * shortopts, const char * longopts, const char * datatype, const char * glossary)*

### arg_str1

*struct arg_str * arg_str1(const char * shortopts, const char * longopts, const char * datatype, const char * glossary)*

### arg_strn

*struct arg_str * arg_strn(const char * shortopts, const char * longopts, const char * datatype, int mincount, int maxcount, const char * glossary)*

### arg_rex0

*struct arg_rex * arg_rex0(const char * shortopts, const char * longopts, const char * pattern, const char * datatype, int flags, const char * glossary)*

### arg_rex1

*struct arg_rex * arg_rex1(const char * shortopts, const char * longopts, const char * pattern, const char * datatype, int flags, const char * glossary)*

### arg_rexn

*struct arg_rex * arg_rexn(const char * shortopts, const char * longopts, const char * pattern, const char * datatype, int mincount, int maxcount, int flags, const char * glossary)*

### arg_file0

*struct arg_file * arg_file0(const char * shortopts, const char * longopts, const char * datatype, const char * glossary)*

### arg_file1

*struct arg_file * arg_file1(const char * shortopts, const char * longopts, const char * datatype, const char * glossary)*

### arg_filen

*struct arg_file * arg_filen(const char * shortopts, const char * longopts, const char * datatype, int mincount, int maxcount, const char * glossary)*

### arg_date0

*struct arg_date * arg_date0(const char * shortopts, const char * longopts, const char * format, const char * datatype, const char * glossary)*

### arg_date1

*struct arg_date * arg_date1(const char * shortopts, const char * longopts, const char * format, const char * datatype, const char * glossary)*

### arg_daten

*struct arg_date * arg_daten(const char * shortopts, const char * longopts, const char * format, const char * datatype, int mincount, int maxcount, const char * glossary)*

### arg_end

*struct arg_end * arg_end(int maxcount)*

### arg_nullcheck

*int arg_nullcheck(void ** argtable)*

** other functions ******************************************

### arg_parse

*int arg_parse(int argc, char ** argv, void ** argtable)*

### arg_print_option

*void arg_print_option(FILE * fp, const char * shortopts, const char * longopts, const char * datatype, const char * suffix)*

### arg_print_syntax

*void arg_print_syntax(FILE * fp, void ** argtable, const char * suffix)*

### arg_print_syntaxv

*void arg_print_syntaxv(FILE * fp, void ** argtable, const char * suffix)*

### arg_print_glossary

*void arg_print_glossary(FILE * fp, void ** argtable, const char * format)*

### arg_print_glossary_gnu

*void arg_print_glossary_gnu(FILE * fp, void ** argtable)*

### arg_print_errors

*void arg_print_errors(FILE * fp, struct arg_end * end, const char * progname)*

### arg_print_option_ds

*void arg_print_option_ds(arg_dstr_t ds, const char * shortopts, const char * longopts, const char * datatype, const char * suffix)*

### arg_print_syntax_ds

*void arg_print_syntax_ds(arg_dstr_t ds, void ** argtable, const char * suffix)*

### arg_print_syntaxv_ds

*void arg_print_syntaxv_ds(arg_dstr_t ds, void ** argtable, const char * suffix)*

### arg_print_glossary_ds

*void arg_print_glossary_ds(arg_dstr_t ds, void ** argtable, const char * format)*

### arg_print_glossary_gnu_ds

*void arg_print_glossary_gnu_ds(arg_dstr_t ds, void ** argtable)*

### arg_print_errors_ds

*void arg_print_errors_ds(arg_dstr_t ds, struct arg_end * end, const char * progname)*

### arg_freetable

*void arg_freetable(void ** argtable, size_t n)*

### arg_dstr_create

*arg_dstr_t arg_dstr_create()*

### arg_dstr_destroy

*void arg_dstr_destroy(arg_dstr_t ds)*

### arg_dstr_reset

*void arg_dstr_reset(arg_dstr_t ds)*

### arg_dstr_free

*void arg_dstr_free(arg_dstr_t ds)*

### arg_dstr_set

*void arg_dstr_set(arg_dstr_t ds, char * str, arg_dstr_freefn * free_proc)*

### arg_dstr_cat

*void arg_dstr_cat(arg_dstr_t ds, const char * str)*

### arg_dstr_catc

*void arg_dstr_catc(arg_dstr_t ds, char c)*

### arg_dstr_catf

*void arg_dstr_catf(arg_dstr_t ds, const char * fmt)*

### arg_dstr_cstr

*char * arg_dstr_cstr(arg_dstr_t ds)*

### arg_cmd_init

*void arg_cmd_init()*

### arg_cmd_uninit

*void arg_cmd_uninit()*

### arg_cmd_register

*void arg_cmd_register(const char * name, arg_cmdfn * proc, const char * description)*

### arg_cmd_unregister

*void arg_cmd_unregister(const char * name)*

### arg_cmd_dispatch

*int arg_cmd_dispatch(const char * name, int argc, char *[] argv, arg_dstr_t res)*

### arg_cmd_count

*unsigned int arg_cmd_count()*

### arg_cmd_info

*arg_cmd_info_t * arg_cmd_info(const char * name)*

### arg_cmd_itr_create

*arg_cmd_itr_t arg_cmd_itr_create()*

### arg_cmd_itr_destroy

*void arg_cmd_itr_destroy(arg_cmd_itr_t itr)*

### arg_cmd_itr_advance

*int arg_cmd_itr_advance(arg_cmd_itr_t itr)*

### arg_cmd_itr_key

*char * arg_cmd_itr_key(arg_cmd_itr_t itr)*

### arg_cmd_itr_value

*arg_cmd_info_t * arg_cmd_itr_value(arg_cmd_itr_t itr)*

### arg_cmd_itr_search

*int arg_cmd_itr_search(arg_cmd_itr_t itr, void * k)*

### arg_mgsort

*void arg_mgsort(void * data, int size, int esize, int i, int k, arg_comparefn * comparefn)*

### arg_make_get_help_msg

*void arg_make_get_help_msg(arg_dstr_t res)*

### arg_make_help_msg

*void arg_make_help_msg(arg_dstr_t ds, char * cmd_name, void ** argtable)*

### arg_make_syntax_err_msg

*void arg_make_syntax_err_msg(arg_dstr_t ds, void ** argtable, struct arg_end * end)*

### arg_make_syntax_err_help_msg

*int arg_make_syntax_err_help_msg(arg_dstr_t ds, char * name, int help, int nerrors, void ** argtable, struct arg_end * end, int * exitcode)*

### arg_set_module_name

*void arg_set_module_name(const char * name)*

### arg_set_module_version

*void arg_set_module_version(int major, int minor, int patch, const char * tag)*

### arg_free

*void arg_free(void ** argtable)*

** deprecated functions, for back-compatibility only *******

### row_serialize

*void row_serialize(Row * source, void * destination)*

*Defined at src/row.c#6*

 Serialize a row for storage in a table.

### row_deserialize

*void row_deserialize(void * source, Row * destination)*

*Defined at src/row.c#13*

 Deserialize a row from storage in a table.

### row_print

*void row_print(Row * row)*

*Defined at src/row.c#20*

 Print a row to stdout.

### table_row_slot

*void * table_row_slot(Table * table, uint32_t row_num)*

*Defined at src/table.c#7*

 Handle memory I/O for a particular row.

### table_new

*Table * table_new()*

*Defined at src/table.c#25*

 Create a new table.

### table_free

*void table_free(Table * table)*

*Defined at src/table.c#38*

 Free a table.

### statement_prepare

* statement_prepare(InputBuffer * input_buffer, Statement * statement)*

*Defined at src/statement.c#12*

 Convert user input into an internal representation

 statement_prepare prepares a statement.

### statement_prepare_insert

* statement_prepare_insert(InputBuffer * input_buffer, Statement * statement)*

*Defined at src/statement.c#32*

 statement_prepare_insert prepares an insert statement.

### statement_execute

* statement_execute(Statement * statement, Table * table)*

*Defined at src/statement.c#67*

 Execute the statement

 statement_execute executes a statement.

### statement_execute_insert

* statement_execute_insert(Statement * statement, Table * table)*

*Defined at src/statement.c#83*

 statement_execute_insert executes an insert statement.

### statement_execute_select

* statement_execute_select(Statement * statement, Table * table)*

*Defined at src/statement.c#100*

 statement_execute_select executes a select statement.

### main

*int main(int argc, char ** argv)*

*Defined at src/gnaro.c#22*



## Enums

| enum  |

--

| META_COMMAND_SUCCESS |
| META_COMMAND_EXIT |
| META_COMMAND_UNRECOGNIZED |


*Defined at ./include/meta.h#8*

 MetaCommandResult is an enum that represents the result of executing a meta command.

| enum  |

--

| LOG_TRACE |
| LOG_DEBUG |
| LOG_INFO |
| LOG_WARN |
| LOG_ERROR |
| LOG_FATAL |


*Defined at ./lib/log/log.h#31*

| enum  |

--

| ARG_TERMINATOR |
| ARG_HASVALUE |
| ARG_HASOPTVALUE |


*Defined at ./lib/argtable/argtable3.h#53*

 bit masks for arg_hdr.flag 

| enum  |

--

| ARG_ELIMIT |
| ARG_EMALLOC |
| ARG_ENOMATCH |
| ARG_ELONGOPT |
| ARG_EMISSARG |


*Defined at ./lib/argtable/argtable3.h#159*

| enum  |

--

| ROW_COLUMN_USERNAME_SIZE |
| ROW_COLUMN_EMAIL_SIZE |


*Defined at ./include/row.h#7*

 Hardcoded Row for now.

| enum  |

--

| TABLE_MAX_PAGES |


*Defined at ./include/table.h#8*

 Pages and rows configuration

| enum  |

--

| STATEMENT_INSERT_COMMAND_SIZE |


*Defined at ./include/statement.h#8*

| enum  |

--

| STATEMENT_PREPARE_SUCCESS |
| STATEMENT_PREPARE_STRING_TOO_LONG |
| STATEMENT_PREPARE_NEGATIVE_ID |
| STATEMENT_PREPARE_SYNTAX_ERROR |
| STATEMENT_PREPARE_UNRECOGNIZED |


*Defined at ./include/statement.h#15*

 StatementPrepareResult is an enum that represents the result of preparing a statement.

| enum  |

--

| STATEMENT_EXECUTE_SUCCESS |
| STATEMENT_EXECUTE_TABLE_FULL |


*Defined at ./include/statement.h#25*

 StatementExecuteResult is an enum that represents the result of executing a statement.

| enum  |

--

| STATEMENT_INSERT |
| STATEMENT_SELECT |


*Defined at ./include/statement.h#31*

 StatementType is an enum that represents the type of a statement.

| enum  |

--

| ARGTABLE_ARG_MAX |


*Defined at src/gnaro.c#12*



