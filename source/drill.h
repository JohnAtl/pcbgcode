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
real g_mins[];
real g_maxs[];

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
int get_drill_for_and_count(int req_size, int do_count)
{
	int i;
	int tool_num;
	int drill_size;
	int minimum;
	int maximum;
//	string temp_str;
	string fields[];
	string tool_text;
	int FLD_TOOL = 0;
	int FLD_DRILL = 1;
	int FLD_MIN = 2;
	int FLD_MAX = 3;
	int FLD_LEN = 4;
	int FLD_COUNT = 5;

	//
	// No longer complains about subs being available if there isn't a
	// rack file to begin with.
	//
	if (!m_have_rack) {
		return req_size;
	}
	
	m_last_match = -1;
	for(i = 0; i < g_num_drills; i++) {
        int num_fields = strsplit(fields, g_rack[i], DRILL_SEP);
	  if ( num_fields < FLD_COUNT) {
          string spaces_error="";
          if (strstr(g_rack[i], " ")) {
              spaces_error="There are spaces in the rack file. Use only tabs.";
          }
          string tt;
          sprintf(tt, "Improperly formatted rack entry:\nm_rack_file_name=%s\nnum_fields=%d, string=\"%s\"\n%s", 
              m_rack_file_name, num_fields, g_rack[i], spaces_error);
	    dlgMessageBox(tt);
	    exit(0);
	  }
		tool_text = strsub(fields[FLD_TOOL], 1);
		tool_num = strtol(tool_text);
		drill_size  = conv_to_internal_units(fields[FLD_DRILL]);
		minimum     = conv_to_internal_units(fields[FLD_MIN]);
		maximum     = conv_to_internal_units(fields[FLD_MAX]);
//		sprintf(temp_str, "req = %f, tool_num = %d, min = %f, max = %f", 
//		  u2inch(req_size), tool_num, u2inch(minimum), u2inch(maximum));
//		dlgMessageBox(temp_str);

		if(in_range_int(req_size, minimum, maximum)) {
			if (g_drill_sub_cnt[tool_num] == 0) {
//				sprintf(temp_str, "Using drill T%02d\nDrill size: %5.02fmm "
//				"(%5.03fin)\nHole size: %5.02fmm (%5.03fin).\n",
//				tool_num, u2mm(drill_size), u2inch(drill_size), 
//				u2mm(req_size), u2inch(req_size));
//				rack_message(temp_str);
			}
			if (g_mins[tool_num] == 0) {
			  g_mins[tool_num] = u2inch(req_size);
		  }
			g_mins[tool_num] = min(g_mins[tool_num], u2inch(req_size));
			g_maxs[tool_num] = max(g_maxs[tool_num], u2inch(req_size));
			if (do_count) {
			  g_drill_sub_cnt[tool_num] += 1;
		  }
//			sprintf(temp_str, "req = %f, tool_num = %d, min = %f, max = %f", 
//			  u2inch(req_size), tool_num, g_mins[tool_num], g_maxs[tool_num]);
//			dlgMessageBox(temp_str);
			g_did_subs = true;
			m_last_match = tool_num;
			return drill_size;
		}
	}
	rack_message("No drill sub for " + real_to_string(u2inch(req_size)) + "\" " +
		real_to_string(u2mm(req_size)) + "mm");

	return req_size;
}

/*
 * Returns a drill for a requested hole size.
 * Calls get_drill_for_and_count() and retains default side effect of counting hole substitutions.
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
int get_drill_for(int req_size)
{
  return get_drill_for_and_count(req_size, true);
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
	get_drill_for_and_count(req_size, false);
	if (m_last_match == -1) m_last_match = default_tool;
	return m_last_match;
}
