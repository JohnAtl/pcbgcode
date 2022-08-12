// -*- Mode: Eagle -*-
//
// Drill routines.
//
// Copyright 2004-2013 by John Johnson Software, LLC.
// See readme.html for copyright information.
//

#include "nonvolatile.h"

char DRILL_SEP  = '\t';
string g_rack[];
int g_num_drills;
int g_drill_sub_cnt[];
real g_min_subbed_for[];
real g_max_subbed_for[];

int m_shut_up;
int m_last_match;
int m_have_rack = false;
string m_rack_file_name = "?";

if (get_nv_param("drill_shut_up", "NO", NO) == "YES") {
	m_shut_up = YES;
}
else {
	m_shut_up = NO;
}

/*
 * Shows a message window with the rack file name and a user supplied message.
 *
 * Param    Function
 * msg      Message to show.
 *
 * Returns
 *
 * Modifies
 * m_shut_up                Set to true if the user requests shut up.
 * nv_param drill_shut_up   Set to YES if the user never wants to see drill messages.
 *
 */
void rack_message(string msg)
{
    if(m_shut_up)
      return;

    msg = "Rack file: " + elided_path(m_rack_file_name, 30) + ":\n" + msg;
    
    switch(dlgMessageBox(msg, "Ok", "Shut up already", "Never ask again")) {
    case 1:
      m_shut_up = true;
      break;
    case 2:
      m_shut_up = true;
      set_nv_param("drill_shut_up", "YES");
      break;
    }
}

/*
 * Reads in a rack file, removes comments, and creates the global rack file.
 *
 * Param        Function
 * drill_file   Name of the drill_file to load.
 *
 * Returns
 *
 * Modifies
 * g_num_drills     Number of drills in the rack file.
 * g_rack[]         Array of strings of rack file text.
 * m_have_rack      Whether a rack file was available.
 * m_rack_file_name The name of the rack file, if available.
 */
void read_rack_file(string drill_file)
{
	string VALID_DRILL_CHARS = "#+-.0123456789imntT" + DRILL_SEP;

	string drill_raw[];
	int num_raw_drills;
	int i;
    string tt;
    
    // rack_message(fs("Reading: %s", drill_file));

	/* 
	* Remove comment lines from the rack file.
	* 
	*/
	g_num_drills = 0;
	num_raw_drills = fileread(drill_raw, drill_file);
	string first_char;
	for (i=0; i<num_raw_drills; i++) {
		first_char = strsub(ltrim(drill_raw[i]), 0, 1);
		if (first_char != "#") {
			g_rack[g_num_drills++] = drill_raw[i];
		}
	}

	if (g_num_drills < 1) {
		dlgMessageBox("There are no drills defined in " + drill_file);
		exit(1);
	}
	
	m_have_rack = true;
	m_rack_file_name = drill_file;
}

/*
 * Intelligently determines the rack file to use, and calls read_rack_file() to load it.
 *
 * Param    Function
 *
 * Returns
 *
 * Modifies
 *
 */
void load_rack()
{
	string board_file;
	string drill_file;

	board(B) {
		board_file = B.name;
	}

	drill_file = filesetext(board_file, ".drl");
	if(filetime(drill_file)) {
		read_rack_file(drill_file);
	}
	else if (filetime(DEFAULT_DRILL_FILE)) {
		read_rack_file(DEFAULT_DRILL_FILE);
	}
	else if (filetime(g_path + "/settings/default.drl")) {
		read_rack_file(g_path + "/settings/default.drl");
	}
	else if (DEFAULT_DRILL_FILE != ""){
		dlgMessageBox("Cannot open the default drill rack file:" + EOL + 
			DEFAULT_DRILL_FILE + EOL +
			"I also tried:" + EOL +
			g_path + "/settings/default.drl" + EOL +
 			"Please use setup to select a file, or clear the setting if you don't want to use one.");
		exit(1);
	}
}

load_rack();

enum { RACK_TOOL_NUM  = 0, 
    RACK_TOOL_SIZE    = 1, 
    RACK_MIN          = 2, 
    RACK_MAX          = 3, 
    RACK_LENGTH       = 4,
    RACK_STEP_XY      = 5,
    RACK_STEP_Z       = 6,

    RACK_TOOL_TYPE    = 20,
    RACK_TOOL_DRILL   = 21,
    RACK_TOOL_ENDMILL = 22
};

/*
 * Returns tool parameters in internal units, except for the tool number, which is just an integer.
 *
 * Parameters
 * tool_num     The tool number to obtain parameters for.
 * param        Which parameter to obtain. Use the RACK_* definitions above.
 *
 * Returns
 * int          An integer with the following characteristics:
 *                  For RACK_TOOL_NUM, returns the tool number as a regular integer, i.e. tool 4 returns 4.
 *                  For tool measurements, such as RACK_TOOL_SIZE, returns a value in Eagle internal units.
 *                  For RACK_TOOL_TYPE, returns either RACK_TOOL_DRILL if a drill, or RACK_TOOL_ENDMILL if an endmill.
 *
 */
int get_tool_param_iu(int tool_num, int param)
{
    string params[];
    
    for (int i=0; i < g_num_drills; i++) {
        strsplit(params, g_rack[i], '\t');
        if (my_strtol(strsub(params[RACK_TOOL_NUM],1)) == tool_num) {
            if (strsub(params[RACK_TOOL_NUM], 0, 1) == "T") {
                if (param > RACK_LENGTH && param < RACK_TOOL_TYPE) {
                    Fatal("get_tool_param_iu()",fi("Requested param=%d for a drill", param));
                }
            }
            if (param == RACK_TOOL_NUM) {
                return my_strtol(strsub(params[param], 1));
            }
            else if (param == RACK_TOOL_TYPE) {
                if (strsub(params[RACK_TOOL_NUM], 0, 1) == "T")
                    return RACK_TOOL_DRILL;
                else if (strsub(params[RACK_TOOL_NUM], 0, 1) == "E")
                    return RACK_TOOL_ENDMILL;
                else
                    Fatal("get_tool_param_iu", "Unknown tool type " + params[RACK_TOOL_NUM]);
            }
            return conv_to_internal_units(params[param]);
        }
    }
    Fatal("get_tool_param_iu", fi("Requested param for unknown tool %d", tool_num));
}

void test_get_tool_param()
{
    dlgMessageBox("test_get_tool_param");
    read_rack_file(g_path + "/source/test.drl");
    
    // test a "normal" drill
    assert(get_tool_param_iu(4, RACK_TOOL_NUM) == 4, "tool is not 4 for tool 4");
    assert(get_tool_param_iu(4, RACK_TOOL_SIZE) == conv_units_from_to(0.050, U_INCHES, U_INTERNALS), "343 size is wrong");
    assert(get_tool_param_iu(4, RACK_MIN) == conv_units_from_to(1.143, U_MILLIMETERS, U_INTERNALS), "344 min is wrong");
    assert(get_tool_param_iu(4, RACK_MAX) == conv_units_from_to(0.055, U_INCHES, U_INTERNALS), "345 max is wrong");
    assert(get_tool_param_iu(4, RACK_LENGTH) == conv_units_from_to(1.5, U_INCHES, U_INTERNALS), "346 length is wrong");
    assert(get_tool_param_iu(4, RACK_TOOL_TYPE) == RACK_TOOL_DRILL, "347 tool type is incorrect");
    
    // test the endmill params
    assert(get_tool_param_iu(2, RACK_TOOL_NUM) == 2, "tool is not 2 for tool 2");
    assertrr(get_tool_param_iu(2, RACK_TOOL_SIZE) == conv_units_from_to(0.032, U_INCHES, U_INTERNALS), 
        "351 size is wrong\nExpected %f, was %f", 0.032, conv_units_from_to(get_tool_param_iu(2, RACK_TOOL_SIZE), U_INTERNALS, U_INCHES));
    assertrr(get_tool_param_iu(2, RACK_MIN) == conv_units_from_to(0.025, U_INCHES, U_INTERNALS), "353 min is wrong\nExpected %f, was %f", 0.025, conv_units_from_to(get_tool_param_iu(2, RACK_MIN), U_INTERNALS, U_INCHES));
    assert(get_tool_param_iu(2, RACK_MAX) == conv_units_from_to(0.035, U_INCHES, U_INTERNALS), "354 max is wrong");
    assert(get_tool_param_iu(2, RACK_LENGTH) == conv_units_from_to(1.5, U_INCHES, U_INTERNALS), "355 length is wrong");
    assert(get_tool_param_iu(2, RACK_STEP_XY) == conv_units_from_to(0.010, U_INCHES, U_INTERNALS), "356 step xy is wrong");
    assert(get_tool_param_iu(2, RACK_STEP_Z) == conv_units_from_to(0.007, U_INCHES, U_INTERNALS), "357 step z is wrong");
    assert(get_tool_param_iu(2, RACK_TOOL_TYPE) == RACK_TOOL_ENDMILL, "358 tool type is incorrect");
    
}

// test_get_tool_param();

int get_tool_num_at_ndx(int ndx)
{
    assert(ndx < g_num_drills, "illegal tool index");
    
    return my_strtol(strsub(g_rack[ndx], 1));
}

/**
 * Returns a drill that the user has in stock,
 * based on the min and max values of the hole.
 * Optionally, counts the number of substitutions made.
 *
 * Param      Function
 * req_size   requested drill size in internal units
 * do_count   whether the substitution counts should be updated
 *
 * Returns
 * Drill size to use, or the requested size if no sub was found.
 *
 * Modifies
 * m_last_match   Set to the tool number of the substitute drill, or -1 if none was available.
 *
**/
int g_did_subs;
int get_tool_for_and_count(int req_size, int do_count)
{
	int i;
	int tool_num;
	int drill_size;
	int minimum;
	int maximum;
	string temp_str;
	string fields[];
	string tool_text;
	int FLD_TOOL = 0;
	int FLD_DRILL = 1;
	int FLD_MIN = 2;
	int FLD_MAX = 3;
	int FLD_LEN = 4;
	int FLD_COUNT = 5;
    int tool_type;

	//
	// No longer complains about subs being available if there isn't a
	// rack file to begin with.
	//
	if (!m_have_rack) {
		return 1;
	}
	
    m_last_match = -1;
    for(i = 0; i < g_num_drills; i++) {
        tool_num = get_tool_num_at_ndx(i);
        tool_type = get_tool_param_iu(tool_num, RACK_TOOL_TYPE);
        drill_size  = get_tool_param_iu(tool_num, RACK_TOOL_SIZE);
        minimum     = get_tool_param_iu(tool_num, RACK_MIN);
        maximum     = get_tool_param_iu(tool_num, RACK_MAX);
        //		sprintf(temp_str, "req = %f, tool_num = %d, min = %f, max = %f", 
        //		  u2inch(req_size), tool_num, u2inch(minimum), u2inch(maximum));
        //		dlgMessageBox(temp_str);

        if(in_range_int(req_size, minimum, maximum)) {
            if (g_drill_sub_cnt[tool_num] == 0) {
                                sprintf(temp_str, "Using tool %s%02d\nDrill size: %5.02fmm "
                                "(%5.03fin)\nHole size: %5.02fmm (%5.03fin).\n",
                                tool_text, tool_num, u2mm(drill_size), u2inch(drill_size),
                                u2mm(req_size), u2inch(req_size));
                                rack_message(temp_str);
            }
            if (g_min_subbed_for[tool_num] == 0) {
                g_min_subbed_for[tool_num] = u2inch(req_size);
            }
            g_min_subbed_for[tool_num] = min(g_min_subbed_for[tool_num], u2inch(req_size));
            g_max_subbed_for[tool_num] = max(g_max_subbed_for[tool_num], u2inch(req_size));
            if (do_count) {
                g_drill_sub_cnt[tool_num] += 1;
            }
            //			sprintf(temp_str, "req = %f, tool_num = %d, min = %f, max = %f", 
            //			  u2inch(req_size), tool_num, g_min_subbed_for[tool_num], g_max_subbed_for[tool_num]);
            //			dlgMessageBox(temp_str);
            g_did_subs = true;
            m_last_match = tool_num;
            return tool_num;
        }
    }
	rack_message("No drill sub for " + real_to_string(u2inch(req_size)) + "\" " +
		real_to_string(u2mm(req_size)) + "mm");

	return 1;
}

/*
 * Returns a drill for a requested hole size.
 * Calls get_tool_for_and_count() and retains default side effect of counting hole substitutions.
 * 
 *
 * Param    Function
 * req_size Size requested in internal units.
 *
 * Returns
 * Substitute size, or size requested if no sub is available.
 *
 * Modifies
 *
 */
int get_tool_for(int req_size)
{
  return get_tool_for_and_count(req_size, true);
}


/**
 * Returns a tool number for a given size, or 
 * a default if the size isn't available, there is no
 * rack file, etc.
 * 
 *
 * Param        Function
 * req_size     Size requested in internal units.
 * default_tool Tool to return if a sub is not available.
 *
 * Returns
 * Tool number for size requested, or default_tool if sub was not available.
 *
 * Modifies
 *
 */
int get_tool_num_for(int req_size, int default_tool)
{
	if (!m_have_rack) {
		return default_tool;
	}
	get_tool_for_and_count(req_size, false);
	if (m_last_match == -1) m_last_match = default_tool;
	return m_last_match;
}

