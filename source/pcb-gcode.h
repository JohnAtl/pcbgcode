// -*- Mode: Eagle -*-
//
// Constants and enums for pcb-gcode.ulp.
//
// (Actually, they should be consts and typedefs,
// but Eagle doesn't support that.)
//

//
// No users options here.
//

#include "string.h"

string RELEASE = "NOT SET";
string REVISION = "000";

string g_path = "";

string BOARD_NAME;
board(B) { BOARD_NAME = B.name; }

enum { 
	U_MICRONS      = 0,
	U_MILLIMETERS = 1,
	U_MILS        = 2,
	U_INCHES      = 3,
	U_INTERNALS    = 4
};
 
enum { 
  OUTPUT_MICRONS      = 0,
  OUTPUT_MILLIMETERS  = 1,
  OUTPUT_MILS         = 2,
  OUTPUT_INCHES       = 3,
	OUTPUT_INTERNALS	= 4
};
       
enum { NO = 0, YES = 1 };
enum { false = 0, true = 1 };

enum { TASK_INVALID, TASK_OUTLINES, TASK_FILL };

enum { ST_INVALID,
       ST_START_LINE, ST_CONTINUE_LINE, ST_END_LINE,
       ST_DRILL,
       ST_FILL,
		 ST_ARC_BEGIN,
		 ST_ARC_END,
		 ST_ARC};

enum { TOP = 0, BOTTOM = 1, MILL = 2, TEXT = 3, STENCIL = 4, ALL = 5 };

int PROGRAM_NAME_ARG = 0;
int FILENAME_ARG = 1;
int TOOL_SIZE_ARG = 2;
int ISO_ARG = 3;
int PASS_ARG = 4;
int PHASE_ARG = 5;

real COORD_TOLERANCE = 0.0001;

string OUTLINES_SIGNAL_NAME = "_OUTLINES_";

int TOP_LAYER = 1;
int BOTTOM_LAYER = 16;
int TOP_CREAM_LAYER = 31;
int BOTTOM_CREAM_LAYER = 32;
int MILL_LAYER = 46;
int TEXT_LAYER = 46; // same as MILL_LAYER

int OUTLINES   = 1;
int FILL       = 2;
int MILL_BOARD = 3;
int MILL_TEXT  = 4;

enum {
	PH_INVALID					 =  0,
  PH_TOP_OUT_GEN       =  1,
  PH_TOP_OUT_WRITE     =  2,
  PH_TOP_FILL_GEN      =  3,
  PH_TOP_FILL_WRITE    =  4,
  PH_TOP_STENCIL       =  5,
  PH_BOTTOM_OUT_GEN    =  6,
  PH_BOTTOM_OUT_WRITE  =  7,
  PH_BOTTOM_FILL_GEN   =  8,
  PH_BOTTOM_FILL_WRITE =  9,
  PH_BOTTOM_STENCIL    = 10,
  PH_TOP_DRILL         = 11,
  PH_BOTTOM_DRILL      = 12,
  PH_MILL              = 13,
	PH_TEXT				       = 14,

  PH_LAST_PHASE = 15
};

string FILEMODE_WRITE_TEXT = "wt";
string FILEMODE_APPEND_TEXT = "at";

string PHASE_NAME[] = {
	"invalid",
	"Gen_Top_Outlines", "Write_Top_Outlines", 
	"Gen_Top_Fill", "Write_Top_Fill",
  "Top_Stencil",
	"Gen_Bottom_Outlines", "Write_Bottom_Outlines", 
	"Gen_Bottom_Fill", "Write_Bottom_Fill",
  "Bottom_Stencil",
	"Top_Drills", "Bottom_Drills", 
	"Milling", 
	"Text",
	"Finished!"
};

string get_phase_name(int phase)
{
  return PHASE_NAME[phase];
}
 
// Used to convert a numeric state into a text name for that state.
string state_text[] = {
  "ST_START_LINE",
  "ST_CONTINUE_LINE",
  "ST_END_LINE",
  "ST_DRILL",
  "ST_FILL"
};

real ROUND_FACTOR = 1000;

real BORDER_SIZE = 0.001;

int DRILL_SIZE = 0;
int DRILL_X = 1;
int DRILL_Y = 2;

real g_tool_size = 0.01;
int g_side = TOP;

// Which phase of the process we're working on.
int g_phase;

string IS_SETUP_FILE_NAME = "pcb_gcode_is_setup";

// Current profile array and indices for it.
string CURRENT_PROFILE[];
enum { 
	FILE_NAME = 0,
	AUTHOR = 1,
	DESCRIPTION = 2
	}

string get_current_profile()
{
	string files[];
	int num_files = fileglob(files, g_path + "/" + IS_SETUP_FILE_NAME);
	if (num_files > 0) {
		fileread(CURRENT_PROFILE, g_path + "/" + IS_SETUP_FILE_NAME);
	}
	else {
		CURRENT_PROFILE[FILE_NAME] = "NONE";
		CURRENT_PROFILE[AUTHOR] = "NONE";
		CURRENT_PROFILE[DESCRIPTION] = "NONE";
		return "NONE";
	}
	
	return CURRENT_PROFILE[DESCRIPTION];
}

void set_current_profile(string profile_fields)
{
	strsplit(CURRENT_PROFILE, profile_fields, '\t');
	output(g_path + "/" + IS_SETUP_FILE_NAME) {
		printf("%s\n", CURRENT_PROFILE[FILE_NAME]);
		printf("%s\n", CURRENT_PROFILE[AUTHOR]);
		printf("%s\n", CURRENT_PROFILE[DESCRIPTION]);
	}
}

int program_is_setup()
{
	if (get_current_profile() == "NONE")
		return NO;
		
	return YES;
}

void path_not_set_error()
{
  dlgMessageBox("There is a problem with your installation of pcb-gcode.\n" +
  "You probably need to add the path to pcb-gcode's folder in " +
  "EAGLE's Control Panel | Options | Directories | User Language Programs setting.\n"
  "Please see the Configuration section of the manual");
  exit(-1);
}


// Find the path where all our files are located. It must be one of the directories in the
// Options | Directories | User Language Programs settings.


void get_path()
{
  int index = 0;
  string last_g_path;

  board(B) g_path = filedir(B.name);

  last_g_path = g_path;
  while (g_path > "") {
    g_path = remove_last_dir(g_path);
    if (filetime(g_path + "/source/pcb-gcode.h")) {
      return;
    }
    if (last_g_path == g_path) {
      break;
    }
    last_g_path = g_path;
  }

  while (path_ulp[index] != "" && index < 10) {
    if(filetime(path_ulp[index] + "/source/pcb-gcode.h")) {
      g_path = path_ulp[index];
      return;
    }
    index++;
  }
}

get_path();

if (g_path == "") {
  path_not_set_error();
}
else {
//  dlgMessageBox("g_path = " + g_path);
}

// This reads the current profile into CURRENT_PROFILE, if available.
get_current_profile();
 
// I know, I know. No way around it that I see right now.
string g_real_to_string_string = "";
string real_to_string(real n)
{
	sprintf(g_real_to_string_string, "%f", n);
	return g_real_to_string_string;
}

string g_int_to_string_string = "";
string int_to_string(int n)
{
	sprintf(g_int_to_string_string, "%d", n);
	return g_int_to_string_string;
}


// Used to show debugging information.
string g_debug;

