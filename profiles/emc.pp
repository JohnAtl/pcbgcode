//
// Options for pcb-gcode.ulp.
// Often used options are at the top of the file.
// Copied to gcode-defaults.h by the setup program.
//
// author=John Johnson
// description=EMC
//

int FILENAMES_8_CHARACTERS = NO;

//
// Comments.
//
string COMMENT_BEGIN  = "(";
string COMMENT_END    = ")";

// 
// Format strings for coordinates, etc.
//
string EOL        = "\n";				  		/* standard line ending */
string PARAM      = "P";							/* some use P, some # for parameters */
string FORMAT     = "%-6.4f ";      /* coordinate format */
string FR_FORMAT  = "F%-5.2f "; 	/* feedrate format */
string IJ_FORMAT  = "I" + FORMAT + "J" + FORMAT;
string R_FORMAT   = "R" + FORMAT;

//
// Modes
//
string INCH_MODE            = "G20" + EOL;
string INCH_MODE_COMMENT    = COMMENT_BEGIN + "Inch Mode" + COMMENT_END + EOL;
string METRIC_MODE          = "G21" + EOL;
string METRIC_MODE_COMMENT  = COMMENT_BEGIN + "Metric Mode" + COMMENT_END + EOL;
string MIL_MODE             = "M02 (Please setup MIL_MODE in gcode-defaults.h)" + EOL;
string MICRON_MODE          = "M02 (Please setup MICRON_MODE in gcode-defaults.h)" + EOL;
string ABSOLUTE_MODE        = COMMENT_BEGIN + "Absolute Coordinates" + COMMENT_END + EOL + "G90" + EOL;

//
// G codes
//
string RAPID    = "G00 ";
string FEED     = "G01 ";
string ARC_CW   = "G02 ";
string ARC_CCW  = "G03 ";
string DWELL    = "G04 " + PARAM + "%f" + EOL;

//
// M codes
//
string SPINDLE_ON     = "M03" + EOL + DWELL;
string SPINDLE_OFF    = "M05" + EOL;
string END_PROGRAM    = "M02" + EOL;
string OPERATOR_PAUSE = "M06 ";

//
// Spindle speed
//
string SPINDLE_SPEED  = "S%.0f" + EOL;

//
// Coordinates
//
string MOVE_X   = "X" + FORMAT;
string MOVE_Y   = "Y" + FORMAT;
string MOVE_XY  = "X" + FORMAT + "Y" + FORMAT;
string MOVE_Z   = "Z" + FORMAT;
string MOVE_XYZ = MOVE_XY + MOVE_Z;

//
// Rapids
//
string RAPID_MOVE_X       = RAPID + MOVE_X;
string RAPID_MOVE_Y       = RAPID + MOVE_Y;
string RAPID_MOVE_XY      = RAPID + MOVE_XY;
string RAPID_MOVE_XY_HOME = RAPID + "X0 Y0";
string RAPID_MOVE_Z       = RAPID + MOVE_Z;
string RAPID_MOVE_XYZ     = RAPID + MOVE_XYZ;

//
// Feeds
//
string FEED_MOVE_X            = FEED + MOVE_X;
string FEED_MOVE_Y            = FEED + MOVE_Y;
string FEED_MOVE_XY           = FEED + MOVE_XY;
string FEED_MOVE_XY_WITH_RATE = FEED + MOVE_XY + FR_FORMAT;
string FEED_MOVE_Z            = FEED + MOVE_Z;
string FEED_MOVE_Z_WITH_RATE  = FEED + MOVE_Z   + FR_FORMAT;
string FEED_MOVE_XYZ          = FEED + MOVE_XYZ;

//
// Drilling holes
//
// G82 Xx.xxx Yy.yyy Z.zzz Fff.f Rr.rrr #dwell
//
string DRILL_CODE = "G82 ";
string RELEASE_PLANE = "R" + FORMAT;
string DWELL_TIME = PARAM + "%f";
string DRILL_FIRST_HOLE = DRILL_CODE + MOVE_XYZ + FR_FORMAT + RELEASE_PLANE + DWELL_TIME + EOL;
string DRILL_HOLE = DRILL_CODE + MOVE_XY + EOL;

//
// Tool change
//
string TOOL_CODE = "T%02d ";
string TOOL_MM_FORMAT = "%1.3fmm";
string TOOL_INCH_FORMAT = "%1.4fin";
string TOOL_CHANGE = OPERATOR_PAUSE + TOOL_CODE + COMMENT_BEGIN + FORMAT + COMMENT_END + EOL;

string TOOL_CHANGE_TABLE_HEADER = COMMENT_BEGIN + 
  " Tool|       Size       |  Min Sub |  Max Sub |   Count " + COMMENT_END + EOL;

string TOOL_CHANGE_TABLE_FORMAT(int tool_number, real size_mm, real size_inch, real min_drill, real max_drill, int count)
{
  string formatted;

  sprintf(formatted, COMMENT_BEGIN + " " + 
    TOOL_CODE + "| " + TOOL_MM_FORMAT + " " +
    TOOL_INCH_FORMAT + " | " + TOOL_INCH_FORMAT + " | " +
    TOOL_INCH_FORMAT + " | " + 
    "   %4d" + " " + 
    COMMENT_END + EOL,
    tool_number, size_mm, size_inch, min_drill, max_drill, count);
  return(formatted);
}

//
// Circles / Arcs
//
string ARC_CLOCK     = ARC_CW + MOVE_XY + R_FORMAT + FR_FORMAT + EOL;
string ARC_CCLOCK  = ARC_CCW + MOVE_XY + R_FORMAT + FR_FORMAT + EOL;

