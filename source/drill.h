// -*- Mode: Eagle -*-
//
// Drill routines.
//

#include "nonvolatile.h"

string TOOL_FLD   = "tool";
string DRILL_FLD  = "drill_size";
string MIN_FLD    = "minimum";
string MAX_FLD    = "maximum";
string LENGTH_FLD = "length";
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

void message(string msg)
{
    if(m_shut_up)
      return;

    msg = "Rack file: " + elided_path(m_rack_file_name, 30) + ":\n" + msg;
    
    switch(dlgMessageBox(msg, "Ok", "Shut up already", "Never ask again")) {
    case 1:
      m_shut_up = 1;
      break;
    case 2:
      m_shut_up = 1;
      set_nv_param("drill_shut_up", "YES");
      break;
    }
}

void read_rack_file(string drill_file)
{
	string VALID_DRILL_CHARS = "#+-.0123456789imntT\t";

	string drill_raw[];
	int num_raw_drills;
	string key;
	string temp;
	string drill_text;
	int i;

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
			drill_text += drill_raw[i] + "\n";
		}
	}

	// g_num_drills--;
	// dlgMessageBox(drill_text);
	if (g_num_drills < 1) {
		dlgMessageBox("There are no drills defined in " + drill_file);
		exit(1);
	}
	
	m_have_rack = true;
	m_rack_file_name = drill_file;
}

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
 *
**/
int g_did_subs;
int get_drill_for(int req_size)
{
	int i;
	string key;
	int drill_size;
	int minimum;
	int maximum;
	string str;
	real drill_inch;
	string temp_str;

	//
	// No longer complains about subs being available if there isn't a
	// rack file to begin with.
	//
	if (!m_have_rack) {
		return req_size;
	}
	
	m_last_match = -1;
	for(i = 0; i < g_num_drills; i++) {
		sprintf(key, "T%02d", i);
		drill_size  = conv_to_units(lookup(g_rack, key, DRILL_FLD, DRILL_SEP));
		minimum     = conv_to_units(lookup(g_rack, key, MIN_FLD, DRILL_SEP));
		maximum     = conv_to_units(lookup(g_rack, key, MAX_FLD, DRILL_SEP));
		sprintf(temp_str, "req = %f, %d min = %f, max = %f", 
		u2inch(req_size), i, u2inch(minimum), u2inch(maximum));
		//dlgMessageBox(temp_str);

		if(in_range_int(req_size, minimum, maximum)) {
			if (g_drill_sub_cnt[i] == 0) {
				sprintf(str, "Using drill T%02d\nDrill size: %5.02fmm "
				"(%5.03fin)\nHole size: %5.02fmm (%5.03fin).\n",
				i, u2mm(drill_size), u2inch(drill_size), 
				u2mm(req_size), u2inch(req_size));
				//        message(str);
			}
			drill_inch = u2inch(drill_size);
			if (g_mins[i] == 0) g_mins[i] = u2inch(req_size);
			g_mins[i] = min(g_mins[i], u2inch(req_size));
			g_maxs[i] = max(g_maxs[i], u2inch(req_size));
			g_drill_sub_cnt[i] += 1;
			//      sprintf(temp_str, "req = %f, %d min = %f, max = %f", u2inch(req_size), i, g_mins[i], g_maxs[i]);
			//      dlgMessageBox(temp_str);
			g_did_subs = 1;
			m_last_match = i;
			return drill_size;
		}
	}
	message("No drill sub for " + real_to_string(u2inch(req_size)) + "\" " +
		real_to_string(u2mm(req_size)) + "mm");

	return req_size;
}

/**
 * Returns a tool number for a given size, or 
 * a default if the size isn't available, there is no
 * rack file, etc.
 *
**/
int get_tool_num_for(int req_size, int default_tool)
{
	if (!m_have_rack) {
		return default_tool;
	}
	get_drill_for(req_size);
	if (m_last_match == -1) m_last_match = default_tool;
	return m_last_match;
}
