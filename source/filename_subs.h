// -*- Mode: Eagle -*-

string FILENAME_SUBS;

//
// Values to substitute in file names.
//
string setup_subs_side_phase(int l_side, int l_phase)
{
  string side;
  string file;
  string tmp;
  int i;
  
  side = (l_side == TOP) ? TOP_FILE_NAME : BOT_FILE_NAME;
  switch (l_phase) {
    case PH_TOP_OUT_GEN:
    case PH_TOP_OUT_WRITE:
    case PH_BOTTOM_OUT_GEN:
    case PH_BOTTOM_OUT_WRITE:
      file = ETCH_FILE_NAME;
      break;
    case PH_TOP_DRILL:
    case PH_BOTTOM_DRILL:
      file = DRILL_FILE_NAME;
      break;
    case PH_MILL:
      file = MILL_FILE_NAME;
      break;
    case PH_TEXT:
      file = TEXT_FILE_NAME;
      break;
    default:
      file = "invalid";
  }
//  dlgMessageBox("file = " + ETCH_FILE_NAME);
//  file = PHASE_NAME[l_phase];
  FILENAME_SUBS = "";
  i = 0;
  while (path_epf[i] != "") {
    sprintf(tmp, "$PROJECT_PATH[%d]", i);
    FILENAME_SUBS += key_value_record(tmp, remove_last_slash(path_epf[i]));
    sprintf(tmp, "$ULP_PATH[%d]", i);
    FILENAME_SUBS += key_value_record(tmp, remove_last_slash(path_ulp[i]));    
    sprintf(tmp, "$CAM_PATH[%d]", i);
    FILENAME_SUBS += key_value_record(tmp, remove_last_slash(path_cam[i]));   
    i++;    
  }
  FILENAME_SUBS += key_value_record("$PATH", remove_last_slash(g_path)) +
    key_value_record("$BOARD_PATH", remove_last_slash(filedir(BOARD_NAME))) +
    key_value_record("$BOARD_NAME", filesetext(filename(BOARD_NAME), "")) +
    key_value_record("$BOARD_FILE_PATH", filesetext(BOARD_NAME, "")) +
    key_value_record("$SIDE", side) +
    key_value_record("$FILE", file) +
    key_value_record("$EXT", DEFAULT_EXTENSION)
  ;
  return FILENAME_SUBS;
}

string filename_subs_help()
{
  return "<h2>Filename Variables</h2>"
    "<table>"
    "<tr><td>$PROJECT_PATH[n]</td>"
      "<td>Project paths as set in the Control Panel. "
      "n begins at zero for the first entry.</td></tr>"
  "<tr><td>$ULP_PATH[n]</td>"
  "<td>As above, but for User Language Program paths.</td></tr>"
  "<tr><td>$CAM_PATH[n]</td>"
  "<td>As above, but for CAM Jobs paths.</td></tr>"
  "<tr><td>$BOARD_PATH</td><td>Path to the board file.</td></tr>"
  "<tr><td>$BOARD_NAME</td><td>The file name of the board with the extension (.brd) removed.</td></tr>"
  "<tr><td>$BOARD_FILE_PATH</td><td>Path and name of the board with the extension (.brd) removed.</td></tr>"
  "<tr><td>$SIDE</td><td>Side of the board being generated. By default 'bot' or 'top'.</td></tr>"
  "<tr><td>$FILE</td><td>File of the board being generated. By default 'etch', 'drill' and 'mill' or 'text'.</td></tr>"
  "<tr><td>$EXT</td><td>The extension you set under the GCode Options tab.</td></tr>"
  "</table>"
  "<i>Paths do not end with a /.</i><br/>"
  "Example:<br/>"
  "Assume the board we are working on is:<br/>"
  "/home/john/eagle/my_stuff/enabtmr.brd<br/>"
  "and we use these settings:<br/>"
  "Filename Base: <b>$BOARD_PATH/$BOARD_NAME</b><br/>"
  "Top (Component) Side Files"
  "<table>"
  "<tr><td>Etching: </td><td><b>.$SIDE.$FILE.$EXT</b></td></tr>"
  "<tr><td>Drill:   </td><td><b>.$SIDE.$FILE.$EXT</b></td></tr>"
  "<tr><td>Mill:    </td><td><b>/mill_jobs/$BOARD_NAME.$SIDE.$FILE.tap</b></td></tr>"
  "<tr><td>Text:    </td><td><b>$SIDE.$FILE.tap</b></td></tr>"
  "</table>"
  "Would create the following filenames:<br/>"
  "<table>"
  "<tr><td>Etching: </td><td><b>/home/john/eagle/my_stuff/enabtmr.top.etch.nc</td></tr>"
  "<tr><td>Drill:   </td><td><b>/home/john/eagle/my_stuff/enabtmr.top.drill.nc</td></tr>"
  "<tr><td>Mill:    </td><td><b>/home/john/eagle/my_stuff/enabtmr/mill_jobs/enabtmr.top.mill.tap</td></tr>"
  "<tr><td>Drill:   </td><td><b>/home/john/eagle/my_stuff/enabtmr.top.text.nc</td></tr>"
  "</table>"
  ;
}

string setup_subs()
{
  return setup_subs_side_phase(g_side, g_phase);
}

string sub_side_phase(string s, int side, int phase)
{
  if (phase == 0) dlgMessageBox("phase == 0");
  setup_subs_side_phase(side, phase);
  return substitute(s, FILENAME_SUBS);
}

string get_filename()
{
  string name;
  
  setup_subs();

  if (g_side == TOP) {
    switch(g_phase) {
    case PH_TOP_OUT_GEN:
    case PH_TOP_OUT_WRITE:
      name = FILENAME_TOP_ETCH_FILE;
      break;
    case PH_TOP_DRILL:
      name = FILENAME_TOP_DRILL_FILE;
      break;
    case PH_MILL:
      name = FILENAME_TOP_MILL_FILE;
      break;
    case PH_TEXT:
      name = FILENAME_TOP_TEXT_FILE;
      break;
    }
  }
  else {
    switch(g_phase) {
    case PH_BOTTOM_OUT_GEN:
    case PH_BOTTOM_OUT_WRITE:
      name = FILENAME_BOT_ETCH_FILE;
      break;
    case PH_BOTTOM_DRILL:
      name = FILENAME_BOT_DRILL_FILE;
      break;
    case PH_MILL:
      name = FILENAME_BOT_MILL_FILE;
      break;
    case PH_TEXT:
      name = FILENAME_BOT_TEXT_FILE;
      break;
    }
  }
  return substitute(FILENAME_BASE + name, FILENAME_SUBS);
}

setup_subs();

string sub_path(string name)
{
  return substitute(name, FILENAME_SUBS);
}
