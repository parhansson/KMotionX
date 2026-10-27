/*
  driver.cc
  This file contains the source code for an emulation of using the
  rs274 interpreter from the EMC system (including the main
  function).

  Modification history:

  9-Jun-1999  FMP fixed fgetws() calls to strip off terminal newline
  16-Feb-1999  FMP replaced gets() calls to fgetws()
  ---  TRK created
*/

/*********************************************************************/

#ifndef _WIN32
#include <cwchar> // Krävs för wcstol och wcstod

#define _wtoi(str) (int)wcstol(str, nullptr, 10)
#define _wtoi(str) static_cast<int>(std::wcstol(str, nullptr, 10))
#define _wtof(str) std::wcstod(str, nullptr)
#endif

#include "stdafx.h"
#include "rs274ngc_return.h"
#include "HiResTimer.h"

//#include <stdlib.h>  /* for _wtof, _wtoi */
//#include <string.h>

//ExcludeTranslate

#define AND              &&
#define IS               ==
#define ISNT             !=
#define MAX(x, y)        ((x) > (y) ? (x) : (y))
#define NOT              !
#define OR               ||
#define SET_TO           =

extern int ConvertToolToIndex(setup_pointer settings,int number,int *index);

/* note that the message must be a string with a %ls in it */
#define DRIVER_ERROR(message,item) if(1)		\
	{											\
		wchar_t s[256];							\
		swprintf(s, 255, message, item);		        \
		ErrorOutput += s;						\
		swprintf(s, 255, L"\n");						\
		ErrorOutput += s;						\
		return RS274NGC_ERROR;					\
	}							                \
	else

//driver error and close file

#define DRIVER_ERROR_CF(message,item) if(1)		\
	{											\
		wchar_t s[256];							\
		fclose(setup_file_port);				\
		swprintf(s, 255, message, item);		        \
		ErrorOutput += s;						\
		swprintf(s, 255, L"\n");						\
		ErrorOutput += s;						\
		return RS274NGC_ERROR;					\
	}							                \
	else

#define DRIVER_ERROR_CF2(message,item) if(1)	\
	{											\
		wchar_t s[256];							\
		fclose(tool_file_port);					\
		swprintf(s, 255, message, item);		        \
		ErrorOutput += s;						\
		swprintf(s, 255, L"\n");						\
		ErrorOutput += s;						\
		return RS274NGC_ERROR;					\
	}							                \
	else

extern setup _setup;
extern wchar_t  _interpreter_linetext[];
extern wchar_t  _interpreter_blocktext[];

/*********************************************************************/

/* strip_terminal_newline

Returned Value: char ptr to string passed

Side effects: destructively replaces terminal newline, if any, with null

Called by: read_keyboard_line(), main()

This takes the terminal newline, if any, off the string passed. It
effectively makes fgetws() behave like gets(), which is what the calls
to fgetws() used to be before we changed them due to problems with gets()
potentially running past the end of the destination string.

*/

static wchar_t * strip_terminal_newline(wchar_t *string)
{
  int index = (int)wcslen(string) - 1;

  while (index >= 0) {
    if (string[index] == '\n' ||
        string[index] == '\r') {
      string[index] = 0;
    }
    index--;
  }

  return string;
}

/*********************************************************************/

/* close_and_down

Returned Value: int (RS274NGC_OK or RS274NGC_ERROR)
   If one of the following errors occurs, this returns RS274NGC_ERROR
   Otherwise, it returns RS274NGC_OK.
   1. A left parenthesis is found inside a comment.
   2. The line ends before an open comment is closed
   3. A newline character is found that is not followed by null
   4. The input line was too long

Side effects:
   see below.

Called by:
   read_keyboard_line

To simplify handling upper case letters, spaces, and tabs, this
function removes spaces and and tabs and downcases everything on a
line which is not part of a comment.

Comments are left unchanged in place. Comments are anything
enclosed in parentheses. Nested comments, indicated by a left
parenthesis inside a comment, are illegal.

The line must have a null character at the end when it comes in.
The line may have one newline character just before the end. If
there is a newline, it will be removed.

Although this software system detects and rejects all illegal characters
and illegal syntax, this particular function does not detect problems
with anything but comments.

We are treating RS274 code here as case-insensitive and spaces and
tabs as if they have no meaning. RS274D, page 6 says spaces and tabs
are to be ignored by control.

The manual [NCMS] says nothing about case or spaces and tabs.

*/

int close_and_down( /* ARGUMENT VALUES             */
 wchar_t * line)       /* string: one line of NC code */
{
  int m;
  int n;
  int comment;
  wchar_t item;
  comment SET_TO 0;
  for (n SET_TO 0, m SET_TO 0; (item SET_TO line[m]) ISNT (wchar_t) NULL; m++)
    {
      if (comment)
        {
          line[n++] SET_TO item;
          if (item IS ')')
            {
              comment SET_TO 0;
            }
          else if (item IS '(')
            DRIVER_ERROR(L"Nested comment found%ls", L"");
        }
      else if ((item IS ' ') OR (item IS '\t') OR (item IS '\r'));
                                      /* don't copy blank or tab  or CR */
      else if (item IS '\n')          /* don't copy newline             */
        {                             /* but check null follows         */
          if (line[m+1] ISNT 0)
            DRIVER_ERROR(L"Null missing after newline%ls", L"");
        }
      else if ((64 < item) AND (item < 91)) /* downcase upper case letters */
        {
          line[n++] SET_TO (32 + item);
        }
      else if (item IS '(')   /* comment is starting */
        {
          comment SET_TO 1;
          line[n++] SET_TO item;
        }
      else
        {
          line[n++] SET_TO item; /* copy anything else */
        }
    }
  if (m IS (INTERP_TEXT_SIZE - 1)) /* line was too long */
    DRIVER_ERROR(L"Command too long%ls", L"");
  else if (comment)
    DRIVER_ERROR(L"Unclosed comment found%ls", L"");
  line[n] SET_TO 0;
  return RS274NGC_OK;
}

/****************************************************************************/

/* read_keyboard_line

Returned Value: int - (RS274NGC_OK or RS274NGC_ERROR)
   If gets or close_and_downcase returns RS274NGC_ERROR,
   this returns RS274NGC_ERROR. Otherwise, it returns RS274NGC_OK.

Side effects:
   The value of the length argument is set to the number of
   characters on the reduced line. The line is written into.

Called by: interpret_from_keyboard

This calls fgetws to read one line of RS274 code and calls
close_and_downcase to downcase and remove spaces from everything that
is not part of a comment. The newline character is removed from the
end of the line by strip_terminal_newline().

*/

int read_keyboard_line( /* ARGUMENT VALUES                 */
 wchar_t * raw_line,       /* array to write into             */
 wchar_t * line,           /* array in which to process text  */
 int * length)          /* pointer to an integer to be set */
{
  wchar_t * returned_value;

  returned_value SET_TO fgetws(raw_line, INTERP_TEXT_SIZE, stdin);
  strip_terminal_newline(raw_line);
  if (returned_value IS NULL)
    DRIVER_ERROR(L"fgetws failed%ls", L"");
  wcscpy(line, raw_line);
  if (close_and_down(line) IS RS274NGC_ERROR)
    return RS274NGC_ERROR;
  *length SET_TO (int)wcslen(line);
  return RS274NGC_OK;
}

/****************************************************************************/

/* read_setup_file

Returned Value: (RS274NGC_OK or RS274NGC_ERROR)
  If any of the following errors occur, this returns RS274NGC_ERROR.
  Otherwise, it returns RS274NGC_OK.
  1. The file named by the user cannot be opened.
  2. No blank line is found.
  3. A line of data cannot be read.
  4. An illegal value is given for any attribute.
  5. An unknown attribute name has been used.

Side Effects:
  Values of machine settings are changed, as specified in the file.

Called By:
  interpret_from_file
  interpret_from_keyboard

Setup File Format
-----------------

Everything above the first blank line in a setup file is read and
ignored, so any sort of header material may be used. The blank
line should have nothing on it (spaces or tabs may not be used).

Everything after the first blank line should be data. Each line of
data should have the name of an attribute followed by its value,
with white space in between. For example:

current_x   3.0

The value may be followed by white space and then comments. For example:

current_x   3.0   X is set to three.

Only certain attribute names are recognized. The use of an unknown
attribute name will cause an error.

*/

int read_setup_file(     /* ARGUMENT VALUES             */
					wchar_t * setup_file,      /* name of setup file          */
					setup_pointer settings) /* pointer to machine settings */
{
	static wchar_t name[] SET_TO L"read_setup_file";
	FILE * setup_file_port;
	wchar_t buffer[1000];
	wchar_t attribute[100];
	wchar_t value[100];

	_tfopen_s(&setup_file_port, setup_file, _T("rt,ccs=UTF-8"));

	if (setup_file_port IS NULL)
		DRIVER_ERROR(L"Cannot open setup file: %ls", setup_file);
	for(;;)    /* read and discard header, checking for blank line */
	{
		if (fgetws(buffer, 1000, setup_file_port) IS NULL)
			DRIVER_ERROR_CF(L"Bad %ls file format", L"setup");
		else if (buffer[0] IS '\n' || buffer[0] IS '\r') //PH test for /r as well, text files are not translated on Linux and macOS
			break;
	}

	for (;;)
	{
		if (fgetws(buffer, 1000, setup_file_port) IS NULL)
			break;
		if (swscanf(buffer, L"%ls%ls", attribute, value) IS 0)
			DRIVER_ERROR_CF(L"Bad input line \"%ls\" in setup file", buffer);
		if (wcscmp(attribute, L"block_delete") IS 0)
		{
			if (wcscmp(value, L"ON") IS 0)
				settings->block_delete SET_TO ON;
			else if (wcscmp(value, L"OFF") IS 0)
				settings->block_delete SET_TO OFF;
			else
				DRIVER_ERROR_CF(L"Bad value %ls for block_delete in setup file", value);
		}
		else if (wcscmp(attribute, L"current_x") IS 0)
			settings->current_x SET_TO _wtof(value);
		else if (wcscmp(attribute, L"current_y") IS 0)
			settings->current_y SET_TO _wtof(value);
		else if (wcscmp(attribute, L"current_z") IS 0)
			settings->current_z SET_TO _wtof(value);
		else if (wcscmp(attribute, L"cutter_radius_comp") IS 0)
		{
			if (wcscmp(value, L"OFF") IS 0)
				settings->cutter_radius_compensation SET_TO OFF;
			else if (wcscmp(value, L"LEFT") IS 0)
				settings->cutter_radius_compensation SET_TO LEFT;
			else if (wcscmp(value, L"RIGHT") IS 0)
				settings->cutter_radius_compensation SET_TO RIGHT;
			else
				DRIVER_ERROR_CF(L"Bad value %ls for cutter_radius_comp in setup file",
				value);
		}
		else if (wcscmp(attribute, L"cycle_i") IS 0)
			settings->cycle_i SET_TO _wtof(value);
		else if (wcscmp(attribute, L"cycle_j") IS 0)
			settings->cycle_j SET_TO _wtof(value);
		else if (wcscmp(attribute, L"cycle_k") IS 0)
			settings->cycle_k SET_TO _wtof(value);
		else if (wcscmp(attribute, L"cycle_l") IS 0)
			settings->cycle_l SET_TO _wtoi(value);
		else if (wcscmp(attribute, L"cycle_p") IS 0)
			settings->cycle_p SET_TO _wtof(value);
		else if (wcscmp(attribute, L"cycle_q") IS 0)
			settings->cycle_q SET_TO _wtof(value);
		else if (wcscmp(attribute, L"cycle_r") IS 0)
			settings->cycle_r SET_TO _wtof(value);
		else if (wcscmp(attribute, L"cycle_z") IS 0)
			settings->cycle_z SET_TO _wtof(value);
		else if (wcscmp(attribute, L"distance_mode") IS 0)
		{
			if (wcscmp(value, L"ABSOLUTE") IS 0)
				settings->distance_mode SET_TO MODE_ABSOLUTE;
			else if (wcscmp(value, L"INCREMENTAL") IS 0)
				settings->distance_mode SET_TO MODE_INCREMENTAL;
			else
				DRIVER_ERROR_CF(L"Bad value %ls for distance_mode in setup file",
				value);
		}
		else if (wcscmp(attribute, L"feed_mode") IS 0)
		{
			if (wcscmp(value, L"PER_MINUTE") IS 0)
				settings->feed_mode SET_TO UNITS_PER_MINUTE;
			else if (wcscmp(value, L"INVERSE_TIME") IS 0)
				settings->feed_mode SET_TO INVERSE_TIME;
			else
				DRIVER_ERROR_CF(L"Bad value %ls for feed_mode in setup file", value);
		}
		else if (wcscmp(attribute, L"feed_rate") IS 0)
			settings->feed_rate SET_TO _wtof(value);
		else if (wcscmp(attribute, L"flood") IS 0)
		{
			if (wcscmp(value, L"OFF") IS 0)
				settings->flood SET_TO OFF;
			else if (wcscmp(value, L"ON") IS 0)
				settings->flood SET_TO ON;
			else
				DRIVER_ERROR_CF(L"Bad value %ls for flood in setup file", value);
		}
		else if (wcscmp(attribute, L"length_units") IS 0)
		{
			if (wcscmp(value, L"MILLIMETERS") IS 0)
				settings->length_units SET_TO CANON_UNITS_MM;
			else if (wcscmp(value, L"INCHES") IS 0)
				settings->length_units SET_TO CANON_UNITS_INCHES;
			else
				DRIVER_ERROR_CF(L"Bad value %ls for length_units in setup file", value);
		}
		else if (wcscmp(attribute, L"comp_entry_style") IS 0)
		{
			if (wcscmp(value, L"EMC_COMP_ENTRY_STYLE") IS 0)
				settings->CompEntryStyle SET_TO EMC_COMP_ENTRY_STYLE;
			else if (wcscmp(value, L"FANUC_COMP_ENTRY_STYLE") IS 0)
				settings->CompEntryStyle SET_TO FANUC_COMP_ENTRY_STYLE;
			else
				DRIVER_ERROR_CF(L"Bad value %ls for comp_entry_style in setup file", value);
		}
		else if (wcscmp(attribute, L"mist") IS 0)
		{
			if (wcscmp(value, L"OFF") IS 0)
				settings->mist SET_TO OFF;
			else if (wcscmp(value, L"ON") IS 0)
				settings->mist SET_TO ON;
			else
				DRIVER_ERROR_CF(L"Bad value %ls for mist in setup file", value);
		}
		else if (wcscmp(attribute, L"motion_mode") IS 0)
			settings->motion_mode SET_TO _wtoi(value);
		else if (wcscmp(attribute, L"plane") IS 0)
		{
			if (wcscmp(value, L"XY") IS 0)
				settings->plane SET_TO CANON_PLANE_XY;
			else if (wcscmp(value, L"YZ") IS 0)
				settings->plane SET_TO CANON_PLANE_YZ;
			else if (wcscmp(value, L"XZ") IS 0)
				settings->plane SET_TO CANON_PLANE_XZ;
			else if (wcscmp(value, L"ZX") IS 0)
				settings->plane SET_TO CANON_PLANE_XZ;
			else
				DRIVER_ERROR_CF(L"Bad value %ls for plane in setup file", value);
		}
		else if (wcscmp(attribute, L"axis_offset_x") IS 0)
			settings->axis_offset_x SET_TO _wtof(value);
		else if (wcscmp(attribute, L"axis_offset_y") IS 0)
			settings->axis_offset_y SET_TO _wtof(value);
		else if (wcscmp(attribute, L"axis_offset_z") IS 0)
			settings->axis_offset_z SET_TO _wtof(value);
		else if (wcscmp(attribute, L"origin_offset_x") IS 0)
			settings->origin_offset_x SET_TO _wtof(value);
		else if (wcscmp(attribute, L"origin_offset_y") IS 0)
			settings->origin_offset_y SET_TO _wtof(value);
		else if (wcscmp(attribute, L"origin_offset_z") IS 0)
			settings->origin_offset_z SET_TO _wtof(value);
		else if (wcscmp(attribute, L"slot_for_length_offset") IS 0)
			settings->length_offset_index SET_TO _wtoi(value);
		else if (wcscmp(attribute, L"slot_for_radius_comp") IS 0)
			settings->tool_table_index SET_TO _wtoi(value);
		else if (wcscmp(attribute, L"slot_in_use") IS 0)
			settings->current_slot SET_TO _wtoi(value);
		else if (wcscmp(attribute, L"slot_selected") IS 0)
		{
			if (ConvertToolToIndex(settings,_wtoi(value),&settings->selected_tool_slot))
				settings->selected_tool_slot SET_TO 0;
		}
		else if (wcscmp(attribute, L"spindle_speed") IS 0)
			settings->speed SET_TO _wtof(value);
		else if (wcscmp(attribute, L"speed_feed_mode") IS 0)
		{
			if (wcscmp(value, L"INDEPENDENT") IS 0)
				settings->speed_feed_mode SET_TO CANON_INDEPENDENT;
			else if (wcscmp(value, L"SYNCHED") IS 0)
				settings->speed_feed_mode SET_TO CANON_SYNCHED;
			else
				DRIVER_ERROR_CF(L"Bad value %ls for speed_feed_mode in setup file",
				value);
		}
		else if (wcscmp(attribute, L"spindle_turning") IS 0)
		{
			if (wcscmp(value, L"STOPPED") IS 0)
				settings->spindle_turning SET_TO CANON_STOPPED;
			else if (wcscmp(value, L"CLOCKWISE") IS 0)
				settings->spindle_turning SET_TO CANON_CLOCKWISE;
			else if (wcscmp(value, L"COUNTERCLOCKWISE") IS 0)
				settings->spindle_turning SET_TO CANON_COUNTERCLOCKWISE;
			else
				DRIVER_ERROR_CF(L"Bad value %ls for spindle_turning in setup file",
				value);
		}
		else if (wcscmp(attribute, L"tool_length_offset") IS 0)
			settings->tool_length_offset SET_TO _wtof(value);
		else if (wcscmp(attribute, L"tool_xoffset") IS 0)
			settings->tool_xoffset SET_TO _wtof(value);
		else if (wcscmp(attribute, L"tool_yoffset") IS 0)
			settings->tool_yoffset SET_TO _wtof(value);
		else if (wcscmp(attribute, L"traverse_rate") IS 0)
			settings->traverse_rate SET_TO _wtof(value);
		else
			DRIVER_ERROR_CF(L"Unknown attribute %ls in setup file", attribute);
	}
	fclose(setup_file_port);

	// make sure Vars are in sync with any modified parameters
	double *Vars = settings->parameters;
	Vars[5211] = settings->axis_offset_x;
	Vars[5212] = settings->axis_offset_y;
	Vars[5213] = settings->axis_offset_z;
	Vars[5214] = settings->AA_axis_offset;
	Vars[5215] = settings->BB_axis_offset;
	Vars[5216] = settings->CC_axis_offset;
	Vars[5217] = settings->UU_axis_offset;
	Vars[5218] = settings->VV_axis_offset;

	int index = settings->origin_index;
	Vars[5201+index*20] = settings->origin_offset_x;
	Vars[5202+index*20] = settings->origin_offset_y;
	Vars[5203+index*20] = settings->origin_offset_z;
	Vars[5204+index*20] = settings->AA_origin_offset;
	Vars[5205+index*20] = settings->BB_origin_offset;
	Vars[5206+index*20] = settings->CC_origin_offset;
	Vars[5207+index*20] = settings->UU_origin_offset;
	Vars[5208+index*20] = settings->VV_origin_offset;

	return RS274NGC_OK;
}

/***********************************************************************/

/* read_tool_file

Returned Value: (RS274NGC_OK or RS274NGC_ERROR)
  If any of the following errors occur, this returns RS274NGC_ERROR.
  Otherwise, it returns RS274NGC_OK.
  1. The file named by the user cannot be opened.
  2. No blank line is found.
  3. A line of data cannot be read.

Side Effects:
  Values in the tool table of the machine setup are changed,
  as specified in the file.

Called By:
  interpret_from_file
  interpret_from_keyboard

Tool File Format
-----------------
Everything above the first blank line is read and ignored, so any sort
of header material may be used.

Everything after the first blank line should be data. Each line of
data should have four or more items separated by white space. The four
required items are slot, tool id, tool length offset, and tool diameter.
Other items might be the holder id and tool description, but these are
optional and will not be read. Here is a sample line:

20  1419  4.299  1.0  0        1 inch carbide end mill

The tool_table is indexed by slot number. Index number 0 is not a
valid slot number.

*/

int read_tool_file(      /* ARGUMENT VALUES             */
				   wchar_t * tool_file,       /* name of tool file           */
				   setup_pointer settings) /* pointer to machine settings */
{
	FILE * tool_file_port;
	int slot,i,n,index;
	int Revision=0;
	int tool_id;
	double offset;
	double diameter;
	double xoffset = 0;
	double yoffset = 0;
	double FeedTime = 0;
	double FeedDist = 0;
	wchar_t buffer[1000];
	CString Comment,Image;

	_tfopen_s(&tool_file_port, tool_file, _T("rt,ccs=UTF-8"));

	if (tool_file_port IS NULL)
		DRIVER_ERROR(L"Cannot open tool file: %ls", tool_file);
	for(;;)    /* read and discard header, checking for blank line */
	{
		if (fgetws(buffer, 1000, tool_file_port) IS NULL)
			DRIVER_ERROR_CF2(L"Bad %ls file format", L"tool");
		else if (buffer[0] IS '\n' || buffer[0] IS '\r') //PH test for /r as well, text files are not translated on Linux and macOS
			break;

		if (wcsstr(buffer, L"IMAGE") != NULL) Revision = 1;  // new format has xy offset 
		if (wcsstr(buffer, L"FEEDTIME") != NULL) Revision = 2;  // newer format also has tool time and distance 
	}

	index=0;
	for (;;)
	{
		if (fgetws(buffer, 1000, tool_file_port) IS NULL)
			break;

		if (Revision==0)
		{
			if (swscanf(buffer, L"%d %d %lf %lf", &slot,
				&tool_id, &offset, &diameter) IS 0)
				DRIVER_ERROR_CF2(L"Bad input line \"%ls\" in tool file", buffer);

		}
		else
		{
			if (Revision == 1)
			{
				if (swscanf(buffer, L"%d %d %lf %lf %lf %lf%n", &slot,
					&tool_id, &offset, &diameter, &xoffset, &yoffset, &n) IS 0)
					DRIVER_ERROR_CF2(L"Bad input line \"%ls\" in tool file", buffer);
			}
			else
			{
				if (swscanf(buffer, L"%d %d %lf %lf %lf %lf %lf %lf%n", &slot,
					&tool_id, &offset, &diameter, &xoffset, &yoffset, &FeedTime, &FeedDist, &n) IS 0)
					DRIVER_ERROR_CF2(L"Bad input line \"%ls\" in tool file", buffer);
			}

			CString s = buffer;
			s.Delete(0,n);

			Comment = s;

			//remove beginning and ending whitespace
			Comment.Trim();
			BOOL bImageSuccess = TRUE;
			BOOL bCommentSuccess = TRUE;
			//first isolate our image name
			if(Comment.Right(1) == '"')
			{
				Comment.Delete(Comment.GetLength()-1,1);
				i=Comment.ReverseFind('"');
				if (i!=-1)
				{
					Image = Comment.Mid(i+1);
					Comment = Comment.Left(i-1);
					Comment.Trim();
				}
				else
				{
					bImageSuccess = FALSE;
				}
			}

			if(bImageSuccess)
			{
				//Isolate our comment 
				if(Comment.GetAt(0) == '"' && Comment.Right(1) == '"')
				{
					Comment.Delete(0,1);
					Comment.Delete(Comment.GetLength()-1,1);
				}
				else
				{
					bCommentSuccess = FALSE;
				}
			}
			if(!bImageSuccess)
			{
				DRIVER_ERROR_CF2(L"Bad input line \"%ls\" in tool file, no quotation marks for comment", buffer);
			}

			if (!bCommentSuccess)
			{
				DRIVER_ERROR_CF2(L"Bad input line \"%ls\" in tool file, no matching quotation marks for tool image filename", buffer);
			}
		}

		settings->tool_table[index].slot SET_TO slot;
		settings->tool_table[index].id SET_TO tool_id;
		settings->tool_table[index].length SET_TO offset;
		settings->tool_table[index].diameter SET_TO diameter;
		settings->tool_table[index].xoffset SET_TO xoffset;
		settings->tool_table[index].yoffset SET_TO yoffset;
		settings->tool_table[index].FeedTime SET_TO FeedTime;
		settings->tool_table[index].FeedDist SET_TO FeedDist;
		settings->tool_table[index].Comment=Comment;
		settings->tool_table[index].ToolImage=Image;
		index++;
	}
	fclose(tool_file_port);

	// clear out the remainder of the tool table
	for (; index < CANON_TOOL_MAX; index++)
	{
		CANON_TOOL_TABLE* T = &_setup.tool_table[index];

		T->slot = T->id = 0;
		T->length = T->diameter = T->xoffset = T->yoffset = T->FeedTime = T->FeedDist = 0.0;
		T->Comment = "";
		T->ToolImage = "";
	}

	return RS274NGC_OK;
}


int save_tool_file(const wchar_t* File)
{
	static CHiResTimer Timer;

	if (File[0] == 0) return 0;  // if no file specified exit

	if (save_tool_file_0(File)) return 1;


	// Save backup each time App launched or after a day
	if (Timer.nSplit == 0)
	{
		Timer.Start();
	}
	else
	{
		if (Timer.Elapsed_Seconds() < 24.0 * 3600.0) return 0;
	}

	Timer.Start();  // restart timer

	CString Backup = File;

	Backup = Backup + ".bak";

	if (save_tool_file_0(Backup)) return 1;
	return 0;
}

int save_tool_file_0(const wchar_t* File)
{
	FILE* f;
	_tfopen_s(&f, File, _T("wt,ccs=UTF-8"));

	if (!f)
	{
		MessageBoxW(NULL, L"Unable to write Tool Table file:\r\r" + (CString)File, L"KMotion", MB_ICONSTOP | MB_OK | MB_TOPMOST | MB_SETFOREGROUND | MB_SYSTEMMODAL);
		return 1;
	}

	fwprintf(f, L"SLOT    ID        LENGTH         DIAMETER        XOFFSET        YOFFSET        FEEDTIME        FEEDDIST   COMMENT     IMAGE\n");
	fwprintf(f, L"\n");

	for (int i = 0; i < CANON_TOOL_MAX; i++)
	{
		CANON_TOOL_TABLE* T = &_setup.tool_table[i];

		if (T->slot || T->id) fwprintf(f, L"%3d %6d %15.6f %15.6f %15.6f %15.6f %15.1f %15.1f \"%ls\" \"%ls\"\n",
			T->slot, T->id, T->length, T->diameter, T->xoffset, T->yoffset, T->FeedTime, T->FeedDist, T->Comment.GetBuffer(), T->ToolImage.GetBuffer());
	}
	fclose(f);
	return 0;
}


/*********************************************************************/

/* interpret_from_file

Returned Value: (RS274NGC_OK or RS274NGC_ERROR)
  If the end of the file has been reached without an m2 or m30
  having been read, this returns ENDFILE.
  Otherwise, if any of the following errors occur, this returns RS274NGC_ERROR.
   Otherwise, it returns RS274NGC_OK.
   1. rs274ngc_init returns RS274NGC_ERROR.
   2. read_tool_file returns RS274NGC_ERROR.
   3. read_setup_file returns RS274NGC_ERROR.
   4. rs274ngc_open returns RS274NGC_ERROR.
   5. rs274ngc_read returns RS274NGC_ERROR and no_stop is off.
   6. rs274ngc_execute returns RS274NGC_ERROR and no_stop is off.
   7. fgetws is called and returns NULL.

Side Effects:
   An NC-program file is opened, interpreted, and closed.

Called By:
   main

This emulates the way the EMC system uses the interpreter.

When this function starts, it prompts the user for the name of a
tool file and then the name of a setup file.  The user may enter a
file name followed by a carriage return or just enter a carriage
return after both prompts. If a file name is entered, the file will be
used. If not, default tool or setup information will be used.

If the no_stop argument is OFF, this returns if an error is found.

If the no_stop argument is ON, an error does not stop interpretation.
However, since the file will have been closed, it has to be reopened
and reread up to the next unread line.

An alternate method of getting back to the right place in a file
would be to use fgetpos and fsetpos.

*/

int interpret_from_file( /* ARGUMENT VALUES                   */
 wchar_t * filename,        /* string: name of the rs274kt file  */
 wchar_t * tool_file,       /* name of tool file                 */
 wchar_t * setup_file,      /* name of setup file                */
 int no_stop)            /* switch which is ON or OFF         */
{
  int status;
  int reads;
  int k;
  wchar_t trash[INTERP_TEXT_SIZE];
  wchar_t * read_ok;
  int program_status;

  program_status SET_TO RS274NGC_OK;
  if (rs274ngc_init() IS RS274NGC_ERROR)
    return RS274NGC_ERROR;
  if (tool_file[0] ISNT 0)
    if (read_tool_file(tool_file, &_setup) IS RS274NGC_ERROR)
      return RS274NGC_ERROR;
  if (setup_file[0] ISNT 0)
    if (read_setup_file(setup_file, &_setup) IS RS274NGC_ERROR)
      return RS274NGC_ERROR;
  if (rs274ngc_open(filename) ISNT RS274NGC_OK)
    return RS274NGC_ERROR;
  for(reads SET_TO 0; ; reads++)
    {
      status SET_TO rs274ngc_read();
      if (status IS RS274NGC_ENDFILE)
        return RS274NGC_ENDFILE;
      if (status ISNT RS274NGC_OK)
        {         /* should not be RS274NGC_EXIT or RS274NGC_ERROR */
		  fwprintf(stderr, L"%ls\n", _interpreter_linetext);
          if (no_stop IS OFF)
            return RS274NGC_ERROR;
          else
            {
              program_status SET_TO RS274NGC_ERROR;
              rs274ngc_open(filename);       /* will have been closed    */
              for(k SET_TO -1; k < reads; k++) /* read up to where we were */
                {
                  read_ok SET_TO
                    fgetws(trash, INTERP_TEXT_SIZE, _setup.file_pointer);
                  if (read_ok IS NULL)
                    return RS274NGC_ERROR;
                }
              continue;
            }
        }
      status SET_TO rs274ngc_execute(NULL);
      if (status IS RS274NGC_ERROR)
        {
		  fwprintf(stderr, L"%ls\n", _interpreter_linetext);
          if (no_stop IS OFF)
            return RS274NGC_ERROR;
          else
            {
              program_status SET_TO RS274NGC_ERROR;
              rs274ngc_open(filename); /* will have been closed */
              for(k SET_TO -1; k < reads; k++) /* read up to where we were */
                {
                  read_ok SET_TO
                    fgetws(trash, INTERP_TEXT_SIZE, _setup.file_pointer);
                  if (read_ok IS NULL)
                    return RS274NGC_ERROR;
                }
              continue;
            }
        }
      else if (status IS RS274NGC_EXIT)
        return program_status;
    }
}

/***********************************************************************/

/* interpret_from_keyboard

Returned Value: int (RS274NGC_OK or RS274NGC_ERROR)
   If any of the following errors occur, this returns RS274NGC_ERROR.
   Otherwise, it returns RS274NGC_OK.
   1. rs274ngc_init returns RS274NGC_ERROR.
   2. read_tool_file returns RS274NGC_ERROR.
   3. read_setup_file returns RS274NGC_ERROR.

Side effects:
  Lines of NC code entered by the user are interpreted.

Called by:
  main

When this function starts, it prompts the user for the name of a
tool file and then the name of a setup file.  The user may enter a
file name followed by a carriage return or just enter a carriage
return after both prompts. If a file name is entered, the file will be
used. If not, default tool or setup information will be used.

This then prompts the user to enter a line of rs274 code, If the
line is blank or the first printing character on the line is a slash,
the user is prompted to enter another line. If the line cannot be
parsed, an error message is sent and the user is prompted to enter
another line.

If the line is parsed without error, the user is prompted to enter
a signal that the user wants the line to be executed (which is a
semicolon followed by a carriage return). If this signal is given,
the line is executed. If anything else is entered (followed by a
carriage return), the line is not executed. Then the user is prompted
to enter another line.

To exit, the user must enter "quit" (followed by a carriage return).

*/

int interpret_from_keyboard( /* ARGUMENT VALUES      */
 wchar_t * tool_file,           /* name of tool file    */
 wchar_t * setup_file)          /* name of setup file   */
{
  wchar_t confirm[INTERP_TEXT_SIZE];
  int length;
  int command_ready;

  if (rs274ngc_init() IS RS274NGC_ERROR)
    return RS274NGC_ERROR;
  if (tool_file[0] ISNT 0)
    if (read_tool_file(tool_file, &_setup) IS RS274NGC_ERROR)
      return RS274NGC_ERROR;
  if (setup_file[0] ISNT 0)
    if (read_setup_file(setup_file, &_setup) IS RS274NGC_ERROR)
      return RS274NGC_ERROR;
  for(command_ready SET_TO FALSE; ; )
    {
      if (command_ready)
        {
          command_ready SET_TO FALSE;
          wprintf(L"EXEC <-");
          fgetws(confirm, INTERP_TEXT_SIZE, stdin);
          if (confirm[0] IS ';')
            {
              rs274ngc_execute(_interpreter_blocktext);
              confirm[0] SET_TO 0;
            }
          else {}
        }
      else
        {
          wprintf(L"READ => ");
          if (read_keyboard_line(_interpreter_linetext,
                                 _interpreter_blocktext, &length)
              IS RS274NGC_ERROR);
          else if (wcscmp (_interpreter_blocktext, L"quit") IS 0)
            return RS274NGC_OK;
          else if (length > 0)
            {
              command_ready SET_TO TRUE;
            }
        }
    }
}

/************************************************************************/

/* main

1. If the rs274ngc stand-alone executable is called with no arguments,
input is taken from the keyboard, and an error in the input does not
cause the rs274ngc executable to exit.

EXAMPLE:

1A. To interpret from the keyboard, enter:

rs274ngc

***********************************************************************

2. If the executable is called with one argument, the argument is
taken to be the name of an NC file and the file is interpreted. An
error in the file will cause the interpreter to exit at the point of
the error.  Interpreted output is printed to the computer screen
unless output is redirected to an output file, in which case primitive
machining function calls generated by the interpreter are printed in
the output file, but error messages still appear on the screen.

EXAMPLES:

2A. To interpret the file "cds.ngc" and read the results on the
screen, enter:

rs274ngc cds.ngc

2B. To interpret the file "cds.ngc" and print the results in the file
"cds.prim", enter:

rs274ngc cds.ngc > cds.prim

***********************************************************************

3. If the executable is called with two arguments and the second
argument is "continue", the first argument is taken to be the name of
an NC file and the file is interpreted. An error in the file will not
cause the interpreter to exit. Rather, it will continue to the end of
the file. Output may be redirected as described above.

EXAMPLES:

3A. To interpret the file "nas.ngc" and read the results on the
screen, and not be stopped by errors, enter:

rs274ngc nas.ngc continue

3B. To interpret the file "ncs.ngc" and print the results in the file
"nas.prim", and not be stopped by errors, enter:

rs274ngc nas.ngc continue > nas.prim

***********************************************************************

4. Any other sort of call to the executable will cause an error message
to be printed and the interpreter will not run.

*/

#ifdef _KMOTIONX
// A main with wide argv is not valid C++, and this is a library. Kept for reference only.
[[maybe_unused]] static int driver_main(int argc, wchar_t ** argv)
#else
int main(int argc, wchar_t ** argv)
#endif
{
  wchar_t tool_file[200];
  wchar_t setup_file[200];


	int result;

	result = interpret_from_file( /* ARGUMENT VALUES                   */
		L"tk.ngc",       /* string: name of the rs274kt file  */
		L"tk.tbl",       /* name of tool file                 */
		L"",				/* name of setup file                */
		OFF);           /* switch which is ON or OFF         */

	exit(result);


  if ((argc > 3) OR ((argc IS 3) AND (wcscmp (argv[2], L"continue") ISNT 0)))
    {
      fwprintf(stderr, L"Usage \"rs274ngc\"\n");
      fwprintf(stderr, L"   or \"rs274ngc filename\"\n");
      fwprintf(stderr, L"   or \"rs274ngc filename continue\"\n");
      exit(1);
    }
  fwprintf(stderr, L"name of tool file => ");
  fflush(stderr);
  fgetws(tool_file, 200, stdin);
  strip_terminal_newline(tool_file);
  if (tool_file[0] IS 0)
    fwprintf(stderr, L"using default tool table\n");

  fwprintf(stderr, L"name of setup file => ");
  fflush(stderr);
  fgetws(setup_file, 200, stdin);
  strip_terminal_newline(setup_file);
  if (setup_file[0] IS 0)
    fwprintf(stderr, L"using default machine setup\n");

  if (argc IS 1)
    exit (interpret_from_keyboard(tool_file, setup_file));
  else if (argc IS 2)
    exit (interpret_from_file( argv[1], tool_file, setup_file, OFF));
  else /* if (argc IS 3) */
    exit (interpret_from_file( argv[1], tool_file, setup_file, ON));

  return 0;
}
//ResumeTranslate
/***********************************************************************/
