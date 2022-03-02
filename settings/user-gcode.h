//
// Define your own gcode sequences in this file.
//
// The settings are listed from most general, to most specific. The settings
// at the top of this file go into every file created, while the settings
// at the end of this file have to do with tool changes only.
//
// Note that each line ends with \n this is Geek Speak for newline.
// In other words, it's like hitting the Enter key at the end of a line.
// Notice also that the lines have a ; at the end.
//
// See the bottom of this file for an example of using multiple lines.
//

// Inserted into bottom, top or all files.
string FILE_BEGIN[] = { "", "", "", "" };
string FILE_END[]   = { "", "", "", "" };

FILE_BEGIN[ALL]    = "(Beginning of every file)\n";
FILE_END[ALL]      = "(End of every file)\n";

FILE_BEGIN[BOTTOM] = "(Beginning of every bottom file)\n";
FILE_END[BOTTOM]   = "(End of every bottom file)\n";

FILE_BEGIN[TOP]    = "(Beginning of every top file)\n";
FILE_END[TOP]      = "(End of every top file)\n";

// Inserted into outline files (track milling).
string OUTLINE_BEGIN[] = { "", "", "", "" };
string OUTLINE_BETWEEN[] = { "", "", "", "" };
string OUTLINE_END[]   = { "", "", "", "" };

OUTLINE_BEGIN[ALL]      = "(Outline Begin)\n";
OUTLINE_BETWEEN[ALL]    = "(Between Passes)\n";
OUTLINE_END[ALL]        = "(Outline End)\n";

OUTLINE_BEGIN[BOTTOM]   = "(Bottom outline Begin)\n";
OUTLINE_BETWEEN[BOTTOM] = "(Bottom between passes)\n";
OUTLINE_END[BOTTOM]     = "(Bottom outline End)\n";

OUTLINE_BEGIN[TOP]      = "(Top outline Begin)\n";
OUTLINE_BETWEEN[TOP]    = "(Top between passes)\n";
OUTLINE_END[TOP]        = "(Top outline End)\n";

// Inserted into drill files.
string DRILL_BEGIN[] = { "", "", "", "" };
string DRILL_END[]   = { "", "", "", "" };

DRILL_BEGIN[ALL]    = "(Beginning of All Drill files)\n";
DRILL_END[ALL]      = "(End of all Drill Files)\n";

DRILL_BEGIN[BOTTOM] = "(Bottom Drill Begin)\n";
DRILL_END[BOTTOM]   = "(Bottom Drill End)\n";

DRILL_BEGIN[TOP]    = "(Top Drill Begin)\n";
DRILL_END[TOP]      = "(Top Drill End)\n";

// Inserted into fill files.
string FILL_BEGIN[] = { "", "", "", "" };
string FILL_END[]   = { "", "", "", "" };

FILL_BEGIN[ALL]    = "(Fill Begin)\n";
FILL_END[ALL]      = "(Fill End)\n";

FILL_BEGIN[BOTTOM] = "(Bottom Fill Begin)\n";
FILL_END[BOTTOM]   = "(Bottom Fill End)\n";

FILL_BEGIN[TOP]    = "(Top Fill Begin)\n";
FILL_END[TOP]      = "(Top Fill End)\n";

// Inserted into stencil files.
string STENCIL_BEGIN[] = { "", "", "", "" };
string STENCIL_END[]   = { "", "", "", "" };

STENCIL_BEGIN[ALL]    = "(Stencil Begin)\n";
STENCIL_END[ALL]      = "(Stencil End)\n";

STENCIL_BEGIN[BOTTOM] = "(Bottom Stencil Begin)\n";
STENCIL_END[BOTTOM]   = "(Bottom Stencil End)\n";

STENCIL_BEGIN[TOP]    = "(Top Stencil Begin)\n";
STENCIL_END[TOP]      = "(Top Stencil End)\n";

// Inserted into milling files.
string MILL_BEGIN[] = { "", "", "", "" };
string MILL_END[]   = { "", "", "", "" };

MILL_BEGIN[ALL]    = "(MILL Begin)\n";
MILL_END[ALL]      = "(MILL End)\n";

MILL_BEGIN[BOTTOM] = "(Bottom MILL Begin)\n";
MILL_END[BOTTOM]   = "(Bottom MILL End)\n";

MILL_BEGIN[TOP]    = "(Top MILL Begin)\n";
MILL_END[TOP]      = "(Top MILL End)\n";

// Inserted into text files.
string TEXT_BEGIN[] = { "", "", "", "" };
string TEXT_END[]   = { "", "", "", "" };

TEXT_BEGIN[ALL]    = "(TEXT Begin)\n";
TEXT_END[ALL]      = "(TEXT End)\n";

TEXT_BEGIN[BOTTOM] = "(Bottom TEXT Begin)\n";
TEXT_END[BOTTOM]   = "(Bottom TEXT End)\n";

TEXT_BEGIN[TOP]    = "(Top TEXT Begin)\n";
TEXT_END[TOP]      = "(Top TEXT End)\n";

// Tool change code.
string TOOL_CHANGE_BEGIN[] = { "", "", "", "" };
string TOOL_CHANGED[]      = { "", "", "", "" };
string TOOL_ZERO_BEGIN[]   = { "", "", "", "" };
string TOOL_ZERO_END[]     = { "", "", "", "" };
string TOOL_CHANGE_END[]   = { "", "", "", "" };

TOOL_CHANGE_BEGIN[ALL]    = "(Tool Change Begin)\n";
TOOL_CHANGED[ALL]         = "(Tool changed)\n";
TOOL_ZERO_BEGIN[ALL]      = "(Tool zero begin)\n";
TOOL_ZERO_END[ALL]        = "(Tool zero end)\n";
TOOL_CHANGE_END[ALL]      = "(Tool Change End)\n";

TOOL_CHANGE_BEGIN[BOTTOM] = "(Bottom Tool Change Begin)\n";
TOOL_CHANGED[BOTTOM]      = "(Bottom Tool changed)\n";
TOOL_ZERO_BEGIN[BOTTOM]   = "(Bottom Tool zero begin)\n";
TOOL_ZERO_END[BOTTOM]     = "(Bottom Tool zero end)\n";
TOOL_CHANGE_END[BOTTOM]   = "(Bottom Tool Change End)\n";

TOOL_CHANGE_BEGIN[TOP]    = "(Top Tool Change Begin)\n";
TOOL_CHANGED[TOP]         = "(Top Tool changed)\n";
TOOL_ZERO_BEGIN[TOP]      = "(Top Tool zero begin)\n";
TOOL_ZERO_END[TOP]        = "(Top Tool zero end)\n";
TOOL_CHANGE_END[TOP]      = "(Top Tool Changed End)\n";

//
// An example of using more than one line of gcode.
// Note that only the last line has a ; at the end.
//
string THIS_ISNT_USED[] = { "", "", "", "" }; // Don't change lines like this
THIS_ISNT_USED[BOTTOM] = "G20\n"
                         "G90\n"
                         "G01 Z0\n"
                         "G00 X0 Y0\n"
                         "M06 T2 ; 0.060\n"
                         "G04 P5\n";

/*
 * The functions below should make it easier for folks outputing their own code, etc.
 *
 */
 
/*
 * Called after the gcode file has been opened.
 * Mode can be 
 * "wt" (write a text file) when the file is first opened.
 * "at" (append a text file) if the file is added to.
 *
 * You might want to check g_side to see which "side" you are working on.
 * g_side can be TOP, BOTTOM or MILL.
 *
 */
void user_file_opened(string name, string mode)
{
	if (USER_GCODE == NO) {
		return;
	}

  // your code here
}

/*
 * Just before the gcode file is closed.
 *
 */
void user_file_closing()
{
	if (USER_GCODE == NO) {
		return;
	}

  // your code here
}

/*
 * Just after the gcode file is closed.
 * See user_file_opened for information on the mode parameter.
 *
 */
void user_file_closed(string name, string mode)
{
	if (USER_GCODE == NO) {
		return;
	}

  // your code here
}

/*
 * The beginning of the outline of a track.
 *
 */
void user_track_begin(real start_x, real start_y, real end_x, real end_y)
{
	if (USER_GCODE == NO) {
		return;
	}

  // your code here
}

/*
 * An intermediate point on the outline of a track.
 *
 */
int user_coord_cnt;
void user_track_continue(real start_x, real start_y, real end_x, real end_y)
{
	if (USER_GCODE == NO) {
		return;
	}

  // your code here
}

/*
 * The end of the outline of a track.
 *
 */
void user_track_end(real start_x, real start_y, real end_x, real end_y)
{
	if (USER_GCODE == NO) {
		return;
	}

  // your code here
}

/*
 * The beginning of an arc.
 *
 */
void user_arc_begin(real start_x, real start_y, real end_x, real end_y)
{
	if (USER_GCODE == NO) {
		return;
	}

  // your code here
}

/*
 * The end of an arc.
 *
 */
void user_arc_end(real start_x, real start_y, real end_x, real end_y)
{
	if (USER_GCODE == NO) {
		return;
	}

  // your code here
}
