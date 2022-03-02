// -*- Mode: Eagle -*-
//
// Convert n, an Eagle internal measure, to the user's selected
// unit of measure.
//
// Params:
//  n   An Eagle internal number.
// Return: 
//  n converted to the user's unit of measure.
//

//real X_OFFSET = 1.0;
//real Y_OFFSET = 1.0;

real internals_to_user(int n)
{
  return (convert(n, U_INTERNALS, OUTPUT_UNITS));
}

//
// Convert n, user's selected unit of measure, to the
// Eagle internal measure.
//
// Params:
//  n   user's unit of measure.
// Return:
//  n converted to an Eagle internal number..
//

int user_to_internals(real n)
{
  return (convert(n, OUTPUT_UNITS, U_INTERNALS));
}

//
// Returns the mode command (i.e. G20) to set the machine to
// the user's selected unit of measure.
//
// Params:
//  none
// Return:
//  A string containing the mode command.
//
string get_mode() {
  string mode;
  
  switch (OUTPUT_UNITS) {
  case U_MICRONS:
    mode= MICRON_MODE;
    break;
  case U_MILLIMETERS:
    if (NC_FILE_COMMENT_PCB_DEFAULTS_SETTINGS) {
      mode = METRIC_MODE_COMMENT;
    }
    mode += METRIC_MODE;
    break;
  case U_MILS:
    mode = MIL_MODE;
    break;
  case U_INCHES:
    if (NC_FILE_COMMENT_PCB_DEFAULTS_SETTINGS) {
      mode = INCH_MODE_COMMENT;
    }
    mode += INCH_MODE;
    break;
  default:
    mode = "M02 (Unknown mode in get_mode)";
  }
  
  return mode;
}

//
// Scale X to positive or negative depending on the side
// of the board. Negative for bottom, positive for top.
//
// Params:
//  x   X coordinate.
// Return:
//  x scaled for top (+) or bottom (-).
//
real scale_x(int x)
{
	real scaled;

	scaled = internals_to_user(x) + X_OFFSET;
	if (g_side == BOTTOM) {
		if (FLIP_BOARD_IN_Y == NO && MIRROR_BOTTOM == NO) {
			scaled = scaled * -1;
		}
	}
	return scaled;
}

//
// Return y converted to user's units.
//
// Params:
//  y   Y coordinate.
// Return:
//  Y converted to user's units.
//
real scale_y(int y)
{
	real scaled;

	scaled = internals_to_user(y) + Y_OFFSET;
	if (g_side == BOTTOM) {
		if (FLIP_BOARD_IN_Y == YES && MIRROR_BOTTOM == NO) {
			scaled = scaled * -1;
		}
	}	
	return scaled;
}

//
// Converts string to long, ignoring leading 0s.
//
// Params:
//  s   A numeric string.
// Returns:
//  s converted to a long.
//
int my_strtol(string s)
{
  string result;
  int i;

  i=0;
  while (s[i] == '0')
    i++;
  result = strsub(s, i);
  return strtol(result);
}



//
// Show a dialog with a message and details.
//
// Params:
//  Message   General message.
//  Details   Details of the message.
// Return: none
//
void Message(string msg)
{
  dlgMessageBox(msg);
}

//
// Show a dialog with an error message and details.
//
// Params:
//  Message   General error message.
//  Details   Details of the error.
// Return: none
//
void Error(string msg, string details)
{
	Message(usage + "<hr><b>Error: " + msg + "</b><p>" + details);
}

//
// Show a dialog with an error message and details,
// then exit the program.
//
// Params:
//  Message   General error message.
//  Details   Details of the error.
// Return: none
//
void Fatal(string msg, string details)
{
  // todo change to call Error
  Error(msg, details);
  exit(1);
}


int file_exists(string file)
{
  string files[];
  int num_files = fileglob(files, file);
  
/*  string s;
  
  for (int i=0; i < num_files; i++)
    s +=files[i] + "\n";
  Message(s);
*/  
  return (num_files > 0);
}

enum {
  OS_INVALID = 0,
  OS_UNKNOWN,
  OS_MACOSX,
  OS_LINUX,
  OS_WINDOWS
}

int get_os()
{
  if (file_exists("/Applications/*"))
    return OS_MACOSX;
  if (file_exists("/home/*"))
    return OS_LINUX;
  return OS_WINDOWS;
}


string get_unit_of_measure()
{
  string unit_of_measure = "not set";
  
  //
  // Set unit of measure.
  //
  switch (OUTPUT_UNITS) {
  	case U_MICRONS:
  	  unit_of_measure = "mic";
  	  break;
  	case U_MILLIMETERS:
  	  unit_of_measure = "mm";
  	  break;
  	case U_MILS:
  	  unit_of_measure = "mil";
  	  break;
  	case U_INCHES:
  	  unit_of_measure = "inch";
  	  break;
  }

  return unit_of_measure;
}


