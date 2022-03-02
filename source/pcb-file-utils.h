// -*- Mode: Eagle -*-

int m_line_number;
int LINE_NUMBER_INCREMENT = 10;
string g_preview;

void eol()
{
	printf(EOL);
	m_line_number += LINE_NUMBER_INCREMENT;
}

int out(string s) 
{
	int old_line_num;
	string lines[];
	int num;
	int i;
	string line;

	old_line_num = m_line_number;

	num = strsplit(lines, s, '\n');
	for (i = 0; i < num; i++) {
		line = lines[i];
		if (strlen(line) > 0 && line != "\n" && line != "EOL") {
			if (USE_LINE_NUMBERS) {
				printf(LINE_NUMBER_FORMAT, m_line_number);
			}
			printf(line);
			if (i < num - 1 || rightstr(s,1) == "\n") {
				printf(EOL);
			}
			m_line_number = m_line_number + LINE_NUMBER_INCREMENT;
		}
	}
	return old_line_num;
}

//
// Variables used to track the machine's current position and eliminate
// moves to the same coordinates.
//
real cur_x = -999.999;
real cur_y = -999.999;
real cur_z = -999.999;

void reset_current_positions(void)
{
  cur_x = -999.999;
  cur_y = -999.999;
  cur_z = -999.999;
}

void update_cur_z(real z)
{
  cur_z = z;
}

void update_cur_x(real x)
{
  cur_x = x;
}

void update_cur_y(real y)
{
  cur_y = y;
}

void update_cur_xy(real x, real y)
{
  update_cur_x(x);
  update_cur_y(y);
}

void update_cur_xyz(real x, real y, real z)
{
  update_cur_xy(x, y);
  update_cur_z(z);
}

void xy(real x, real y)
{
	if (close(x, cur_x) && close(y, cur_y)) {
		return;
	}
	
	if (COMPACT_GCODE == YES) {
		if (! close(x, cur_x) && ! close(y, cur_y)) {
			out(frr(MOVE_XY, x, y) + EOL);
			update_cur_xy(x, y);
			return;
		}
		if (! close(x, cur_x)) {
			out(fr(MOVE_X, x) + EOL);
			update_cur_xy(x, y);
			return;
		}
		if (! close(y, cur_y)) {
			out(fr(MOVE_Y, y) + EOL);
			update_cur_xy(x, y);
			return;
		}
	}
	if (! close(x, cur_x) || ! close(y, cur_y)) {
		out(frr(MOVE_XY, x, y) + EOL);
		update_cur_xy(x, y);
	}
}

//
// Output Feed commands.
//
void rate(real f)
{
	out(fr(FR_FORMAT, f));
}

void fx(real x) 
{
	if (! close(x, cur_x)) {
		out(fr(FEED_MOVE_X, x) + EOL);
		update_cur_x(x);
	}
}

void fy(real y) 
{
	if (! close(y, cur_y)) {
		out(fr(FEED_MOVE_Y, y) + EOL);
		update_cur_y(y);
	}
}

void fz(real z) 
{
	if (! close(z, cur_z)) {
		out(fr(FEED_MOVE_Z, z) + EOL);
		update_cur_z(z);
	}
}

// Since the next move may depend on the Z rate having been set
// in this routine, it just outputs the move as usual.
void fzr(real z, real f) 
{
	out( frr(FEED_MOVE_Z_WITH_RATE, z, f)      + EOL); 
	update_cur_z(z);
}

void fxy(real x, real y)
{
	if (! close(x, cur_x) || ! close(y, cur_y)) {
		out( frr(FEED_MOVE_XY, x, y)               + EOL);
		update_cur_xy(x, y);
	}
}

// Since the next move may depend on the XY rate having been set
// in this routine, it just outputs the move as usual.
void fxyr(real x, real y, real f) 
{
	out( frrr(FEED_MOVE_XY_WITH_RATE, x, y, f) + EOL);
	update_cur_xy(x, y);
}

void fxyz(real x, real y, real z)
{
	if (! close(x, cur_x) || ! close(y, cur_y) || ! close(z, cur_z)) {
		out(frrr(FEED_MOVE_XYZ, x, y, z)           + EOL);
		update_cur_xy(x, y);
		update_cur_z(z);
	}
}

//
// Output Rapid commands.
//
void rx(real x) 
{
	if (! close(x, cur_x)) {
		out(fr(RAPID_MOVE_X, x) + EOL);
		update_cur_x(x);
	}
}

void ry(real y) 
{
	if (! close(y, cur_y)) {
		out(fr(RAPID_MOVE_Y, y) + EOL);
		update_cur_y(y);
	}
}

void rz(real z) 
{
	if (! close(z, cur_z)) {
		out(fr(RAPID_MOVE_Z, z) + EOL);
		update_cur_z(z);
	}
}

void rxy(real x, real y)
{
	if (! close(x, cur_x) || ! close(y, cur_y)) {
		out( frr(RAPID_MOVE_XY, x, y)               + EOL);
		update_cur_xy(x, y);
	}
}

void rxyz(real x, real y, real z)
{
	if (! close(x, cur_x) || ! close(y, cur_y) || ! close(z, cur_z)) {
		out(frrr(RAPID_MOVE_XYZ, x, y, z)           + EOL);
		update_cur_xy(x, y);
		update_cur_z(z);
	}
}

void comm(string str)
{
	if (strlen(str) > 0) {
		out(COMMENT_BEGIN + str + COMMENT_END + EOL);
	}
}

void output_file_heading()
{
  real fr_xy;
  real fr_z;
  
	if (NC_FILE_COMMENT_FROM_BOARD) {
		comm(elided_path(argv[PROGRAM_NAME_ARG], 30));
		comm("Copyright 2005 - 2012 by John Johnson");
		comm("See readme.txt for licensing terms.");

		board(B) {
			comm("This file generated from the board:");
			comm(elided_path(B.name, 30));
		}
	}

	if (NC_FILE_COMMENT_FROM_BOARD || NC_FILE_COMMENT_DATE ||
			NC_FILE_COMMENT_MACHINE_SETTINGS || NC_FILE_COMMENT_PCB_DEFAULTS_SETTINGS ||
			g_debug_flag) {
		comm("Current profile is " + elided_path(CURRENT_PROFILE[FILE_NAME], 30));
	}
	if (NC_FILE_COMMENT_DATE) {
		comm("This file generated " + t2string(time()));
	}

	if (NC_FILE_COMMENT_MACHINE_SETTINGS) {
		comm("Settings from pcb-machine.h");
		comm(fr("spindle on time = %6.4f", SPINDLE_ON_TIME));
		switch(g_phase) {
		  case PH_TOP_OUT_WRITE:
		  case PH_BOTTOM_OUT_WRITE:
	      comm("  Tool Size");
	      comm(fr(FORMAT, g_tool_size));
		    comm(fr("spindle speed = %6.4f", SPINDLE_ETCH_RPM));
		    fr_xy = FEED_RATE_ETCH_XY;
		    fr_z = FEED_RATE_ETCH_Z;
		    break;
      case PH_MILL:
        comm(fr("spindle speed = %6.4f", SPINDLE_MILL_RPM));
        comm(fr("milling depth = %6.4f", MILLING_DEPTH));
		    fr_xy = FEED_RATE_MILL_XY;
		    fr_z = FEED_RATE_MILL_Z;
        break;
      case PH_TEXT:
        comm(fr("spindle speed = %6.4f", SPINDLE_TEXT_RPM));
        comm(fr("text depth = %6.4f", TEXT_DEPTH));
		    fr_xy = FEED_RATE_TEXT_XY;
		    fr_z = FEED_RATE_TEXT_Z;
        break;
      case PH_TOP_DRILL:
      case PH_BOTTOM_DRILL:
        comm(fr("spindle speed = %6.4f", SPINDLE_DRILL_RPM));
		    fr_xy = 0;
		    fr_z = FEED_RATE_DRILL_Z;
        break;
    }
		comm(frrr("tool change at " + FORMAT +  FORMAT + FORMAT, 
			TOOL_CHANGE_POS_X, TOOL_CHANGE_POS_Y, TOOL_CHANGE_POS_Z));
		comm("feed rate xy = " + fr(FR_FORMAT, fr_xy));
		comm("feed rate z  = " + fr(FR_FORMAT, fr_z));
	}
	comm("Z Axis Settings");
	comm("  High     Up        Down     Drill");
	comm(fr(FORMAT, DEFAULT_Z_HIGH) + "\t" +
		fr(FORMAT, DEFAULT_Z_UP) + "\t" + 
		fr(FORMAT, DEFAULT_Z_DOWN) + "\t" +
		fr(FORMAT, DRILL_DEPTH));

	if (NC_FILE_COMMENT_PCB_DEFAULTS_SETTINGS) {
		comm("Settings from pcb-defaults.h");
		comm(fr("isolate min = %6.4f", ISO_MIN));
		comm(fr("isolate max = %6.4f", ISO_MAX));
		comm(fr("isolate step = %6.4f", ISO_STEP));
		string tt;
		sprintf(tt, "Generated %s%s%s%s%s%s%s%s",
			(GENERATE_TOP_OUTLINES)     ? "top outlines, "    : "",
			(GENERATE_TOP_DRILL)        ? "top drill, "       : "",
			(GENERATE_TOP_FILL)         ? "top fill, "        : "",
			(GENERATE_TOP_STENCIL)      ? "top stencil, "     : "",
			(GENERATE_BOTTOM_OUTLINES)  ? "bottom outlines, " : "",
			(GENERATE_BOTTOM_DRILL)     ? "bottom drill, "    : "",
			(GENERATE_BOTTOM_FILL)      ? "bottom fill"       : "",
			(GENERATE_BOTTOM_STENCIL)   ? "bottom stencil"    : ""
			);
		comm(tt);
		comm("Unit of measure: " + get_unit_of_measure());
	}
	
	if (NC_OPERATOR_MESSAGE) {
	  out(OPERATOR_PAUSE + COMMENT_BEGIN + NC_OPERATOR_MESSAGE + COMMENT_END + EOL);
	}
}

//
// Writes the header for the output file.
//
// Params:
//  spindle_speed
// Returns:
//  none
//
void begin_gcode(real spindle_speed)
{
	out(get_mode());

	out(ABSOLUTE_MODE);

	out(fr(SPINDLE_SPEED, spindle_speed));
	
	reset_current_positions();

	rz(DEFAULT_Z_HIGH);
	rxy(X_HOME, Y_HOME);
	out(fr(SPINDLE_ON, SPINDLE_ON_TIME));
}

//
// Write end of file commands.
//
// Params:
//  none
// Return:
//  none
//
void end_gcode(void)
{
	rz(DEFAULT_Z_HIGH);
	out(SPINDLE_OFF);
	out(END_PROGRAM);
}

// Kinds (outline, drill, etc.)
void output_kind_begin()
{
	if (USER_GCODE == NO)
		return;
		
	switch (g_phase) {
		case PH_TOP_OUT_WRITE:
		case PH_BOTTOM_OUT_WRITE:
			out(OUTLINE_BEGIN[ALL]);
			out(OUTLINE_BEGIN[g_side]);
			break;

		case PH_TOP_DRILL:
		case PH_BOTTOM_DRILL:
			out(DRILL_BEGIN[ALL]);
			out(DRILL_BEGIN[g_side]);
			break;

		case PH_TOP_FILL_WRITE:
		case PH_BOTTOM_FILL_WRITE:
			out(FILL_BEGIN[ALL]);
			out(FILL_BEGIN[g_side]);
			break;

		case PH_TOP_STENCIL:
		case PH_BOTTOM_STENCIL:
			out(STENCIL_BEGIN[ALL]);
			out(STENCIL_BEGIN[g_side]);
			break;

		case PH_MILL:
			out(MILL_BEGIN[ALL]);
			out(MILL_BEGIN[g_side]);
			break;

    case PH_TEXT:
      out(TEXT_BEGIN[ALL]);
      out(TEXT_BEGIN[g_side]);
      break;

		default:
			dlgMessageBox("!Unknown g_phase " + int_to_string(g_phase) 
					+ " in output_kind_begin()");
	}
}

void output_between_outline_passes()
{
	if (USER_GCODE) {
		out(OUTLINE_BETWEEN[ALL]);
		out(OUTLINE_BETWEEN[g_side]);
	}
}

void output_kind_end()
{
	if (USER_GCODE == NO)
		return;

	switch (g_phase) {
		case PH_TOP_OUT_WRITE:
		case PH_BOTTOM_OUT_WRITE:
			out(OUTLINE_END[g_side]);
			out(OUTLINE_END[ALL]);
			break;

		case PH_TOP_DRILL:
		case PH_BOTTOM_DRILL:
			out(DRILL_END[g_side]);
			out(DRILL_END[ALL]);
			break;

		case PH_TOP_FILL_WRITE:
		case PH_BOTTOM_FILL_WRITE:
			out(FILL_END[g_side]);
			out(FILL_END[ALL]);
			break;

		case PH_TOP_STENCIL:
		case PH_BOTTOM_STENCIL:
			out(STENCIL_END[ALL]);
			out(STENCIL_END[g_side]);
			break;

		case PH_MILL:
			out(MILL_END[g_side]);
			out(MILL_END[ALL]);
			break;

    case PH_TEXT:
      out(TEXT_END[ALL]);
      out(TEXT_END[g_side]);
      break;

		default:
			dlgMessageBox("!Unknown g_phase " + int_to_string(g_phase) 
					+ " in output_kind_end()");
	}
}

// Tool changing
void output_tool_change_begin()
{
	if (USER_GCODE) {
		out(TOOL_CHANGE_BEGIN[ALL]);
		out(TOOL_CHANGE_BEGIN[g_side]);
	}
}

void output_tool_changed()
{
	if (USER_GCODE) {
		out(TOOL_CHANGED[g_side]);
		out(TOOL_CHANGED[ALL]);
	}
}

void output_tool_zero_begin()
{
	if (USER_GCODE) {
		out(TOOL_ZERO_BEGIN[ALL]);
		out(TOOL_ZERO_BEGIN[g_side]);
	}
}

void output_tool_zero_end()
{
	if (USER_GCODE) {
		out(TOOL_ZERO_END[g_side]);
		out(TOOL_ZERO_END[ALL]);
	}
}

void output_tool_change_end()
{
	if (USER_GCODE) {
		out(TOOL_CHANGE_END[g_side]);
		out(TOOL_CHANGE_END[ALL]);
	}
}

//
// Print file beginning headers and user-codes.
//
void output_file_preamble()
{
	output_file_heading();

	if (USER_GCODE) {
		out(FILE_BEGIN[ALL]);
		out(FILE_BEGIN[g_side]);
		output_kind_begin();
	}
}

//
// Print file closing text and user-codes.
//
void output_file_postamble()
{
	if (USER_GCODE) {
		output_kind_end();
		out(FILE_END[g_side]);
		out(FILE_END[ALL]);
	}
}

void output_drill_first_hole(real drill_x, real drill_y, real depth)
{
	string tt;

	if (SIMPLE_DRILL_CODE) {
		rxy(drill_x, drill_y);
		fzr(depth, FEED_RATE_DRILL_Z);
		rz(DEFAULT_Z_UP);
	}
	else {
	//"G82 X%fY%f Z%f F10.0 R0.1 #250\n",
		sprintf(tt, DRILL_FIRST_HOLE, drill_x, drill_y,
			depth, FEED_RATE_DRILL_Z, DEFAULT_Z_UP,
			DRILL_DWELL);
		out(tt);
		update_cur_xy(drill_x, drill_y);
	}
}

void output_drill_hole(real drill_x, real drill_y, real depth)
{
	if (SIMPLE_DRILL_CODE) {
		rxy(drill_x, drill_y);
		fzr(depth, FEED_RATE_DRILL_Z);
		rz(DEFAULT_Z_UP);
	}
	else {
		out(frr(DRILL_HOLE, drill_x, drill_y));
		update_cur_xy(drill_x, drill_y);		
	}
}
