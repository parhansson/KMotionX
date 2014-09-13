// driver.h


extern char  _interpreter_linetext[];
extern char  _interpreter_blocktext[];


int read_tool_file(      /* ARGUMENT VALUES             */
 char * tool_file,       /* name of tool file           */
 setup_pointer settings); /* pointer to machine settings */

int read_setup_file(     /* ARGUMENT VALUES             */
 char * setup_file,      /* name of setup file          */
 setup_pointer settings); /* pointer to machine settings */

