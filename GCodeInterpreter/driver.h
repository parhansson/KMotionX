// driver.h


extern wchar_t  _interpreter_linetext[];
extern wchar_t  _interpreter_blocktext[];


int read_tool_file(      /* ARGUMENT VALUES             */
 wchar_t * tool_file,       /* name of tool file           */
 setup_pointer settings); /* pointer to machine settings */

int save_tool_file(const wchar_t* File);  // save tool file with occasional backup
int save_tool_file_0(const wchar_t* File);  // save tool file


int read_setup_file(     /* ARGUMENT VALUES             */
 wchar_t * setup_file,      /* name of setup file          */
 setup_pointer settings); /* pointer to machine settings */

