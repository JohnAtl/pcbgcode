// -*- Mode: Eagle -*-
/*
 *
 * Math utilities.
 *
 * Copyright 2007 - 2013 by John Johnson Software, LLC.
 * All Rights Reserved.
 *
 */

//#include "source/drill_sizes.h"
#include "drill_sizes.h"

real MM_PER_INCH    = 25.4;
int MICRONS_PER_MM  = 1000;
int MILS_PER_INCH   = 1000;

//
// Thanks EAGLE! No one expects an internal units change. :-)
//
int INTERNAL_SCALAR = 10000;
if (EAGLE_VERSION >= 6) {
  INTERNAL_SCALAR = 320000;
}

int INTERNALS_PER_MICRON = (INTERNAL_SCALAR / 1000);
int INTERNALS_PER_MM = INTERNAL_SCALAR;
int INTERNALS_PER_MIL = MM_PER_INCH * (INTERNAL_SCALAR / 1000);
int INTERNALS_PER_INCH = MM_PER_INCH * INTERNAL_SCALAR;

/*
 * Return the suffix unit of measure from a string.
 */
string get_units(string s)
{
  int len;
  string units;
  
  len = strlen(s);
  units = strsub(s, len - 2, 2);
  // special handling for wire size drill numbers
  if (strsub(units, 1, 1) == "#")
    units = "#";
  return units;
}  

//enum { U_INVALID, U_MICRONS, U_MILLIMETERS, U_MILS, U_INCHES };

real convert(real value, int old_units, int new_units)
{
  real temp;
  real result;

  // Convert current value to 10ths of microns.
  switch(old_units) {
    case U_MICRONS:
    temp = value * INTERNALS_PER_MICRON;
    break;
    case U_MILLIMETERS:
    temp = value * INTERNALS_PER_MM;
    break;
    case U_MILS:
    temp = value * INTERNALS_PER_MIL;
    break;
    case U_INCHES:
    temp = value * INTERNALS_PER_INCH;
    break;
    case U_INTERNALS:
    temp = value;
    break;
    default:
    dlgMessageBox(":Invalid value for old_units in convert()");
    exit(0);
  }

  // Convert temp value to the new unit of measure.
  switch(new_units) {
    case U_MICRONS:
    result = temp / INTERNALS_PER_MICRON;
    break;
    case U_MILLIMETERS:
    result = temp / INTERNALS_PER_MM;
    break;
    case U_MILS:
    result = temp / INTERNALS_PER_MIL;
    break;
    case U_INCHES:
    result = temp / INTERNALS_PER_INCH;
    break;
    case U_INTERNALS:
    result = temp;
    break;
    default:
    dlgMessageBox(":Invalid value for new_units (" + int_to_string(new_units) + ") in convert()");
    exit(0);
  }

  return result;
}


/*
 * Convert a string with a number and a unit-of-measure suffix into 
 * Eagle internal units (microns).
 *
 *    0.032in   0.032 inches
 *    62ml      62 mils, 0.062 inches
 *    0.43mm    0.43 millimeters
 *    1500mc    1500 microns, 1.500 millimeters
 *    60#       60 wire gage drill (0.040" or 1.016mm)
 *    0.12      0.12 inches
 *    0.60      0.60 millimeters
 *    43        43 wire gage drill
 *
 */
int conv_to_units(string s)
{
  int val;
  string units;
  real num;
  real mm;
  string str;
  
  s = strlwr(s);
  
  units = get_units(s);
  num = strtod(s);
  
  if(units == "mm")
    val = convert(num, U_MILLIMETERS, U_INTERNALS);
  else if (units == "in")
    val = convert(num, U_INCHES, U_INTERNALS);
  else if (units == "ml")
    val = convert(num, U_MILS, U_INTERNALS);
  else if (units == "mc")
    val = num;
  else if (units == "#") {
    val = convert(get_drill_size_inches(num), U_INCHES, U_INTERNALS);
  }
  else {
    if (num <= 0.25)
      val = convert(num, U_INCHES, U_INTERNALS);
    else if (num <= 6) 
      val = convert(num, U_MILLIMETERS, U_INTERNALS);
    else
      val = convert(get_drill_size_inches(num), U_INCHES, U_INTERNALS);
  }

  return val;
}

/*
 * Determine if an integer value is within a range (inclusive).
 */
int in_range_int(int val, int min_val, int max_val)
{
  return((val >= min_val) && (val <= max_val));
}

int close(real val_a, real val_b)
{
	return(abs(val_a - val_b) < EPSILON);
}

int close2(real xa, real xb, real ya, real yb)
{
	if (close(xa, xb) && close(ya, yb)) {
		return true;
	}
	return false;
}

int close3(real xa, real xb, real ya, real yb, real za, real zb)
{
	if (close(xa, xb) && close(ya, yb) && close(za, zb)) {
		return true;
	}
	return false;
}
