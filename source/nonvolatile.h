// -*- Mode: Eagle -*-
/* nonvolatile.h
 * Copyright 2004-2009 by John Johnson Software, LLC.
 * See readme.html for copyright information.
 */

string STORAGE_NAME = g_path + "/storage.nv";
int NAME_FIELD = 0;
int VALUE_FIELD = 1;
char SEPARATOR = '=';
string m_params[];
string empty[];

//
// If the storage file does not exist, create it.
//
if (filetime(STORAGE_NAME) == 0) {
  output(STORAGE_NAME, FILEMODE_WRITE_TEXT) {
    printf("created%c%s\s", SEPARATOR, t2string(time()));
  }
}

//
// Empty the parameters array.
//
// Params:
//  none
// Returns:
//  none
// Changes:
//  none
//
void empty_m_params()
{
  int i;
  
  while (m_params[i] != "")
    m_params[i++] = "";
}

//
// Load parameters from the nv parameters file.
//
// Params:
//  can_abort Pass true if the program can abort on error.
// Returns:
//  int   Number of parameters.
// Changes:
//  m_params
//
int read_nv_file(int can_abort)
{
  int num_params;
  
  empty_m_params();
  fileerror();
  num_params = fileread(m_params, STORAGE_NAME);
  if(fileerror()) {
    if(can_abort) {
      exit(1);
    }
    else {
      return 0;
    }
  }

  return num_params;
}

//
// Returns the value of a non-volatile parameter.
//
// Params:
//  name  Name of the parameter.
//  def   The default value.
//  can_abort Whether the program can abort if an error occurs.
// Returns:
//  string  The value.
// Changes:
//  none
//
string get_nv_param(string name, string def, int can_abort)
{
  string value;

  read_nv_file(can_abort);
  
  value = lookup(m_params, name, VALUE_FIELD, SEPARATOR);
  if (value == "") {
    return def;
  }
  return value;
}

//
// Set a non-volatile parameter.
//
// Params:
//  name  Name of the parameter.
//  value The value to set.
// Returns:
//  none
// Changes:
//  m_params
//
void set_nv_param(string name, string value)
{
  int num_params;
  int i;
  string record[];

  num_params = read_nv_file(0);
  if (lookup(m_params, name, VALUE_FIELD, SEPARATOR) == "") {
   m_params[num_params] = name + SEPARATOR + value;
   num_params++;
  }
  else {
   for (i=0; i < num_params; i++) {
     strsplit(record, m_params[i], SEPARATOR);
     if (record[NAME_FIELD] == name) {
       record[VALUE_FIELD] = value;
       m_params[i] = record[NAME_FIELD] + SEPARATOR + record[VALUE_FIELD];
       break;
     }
   }
  }
  output(STORAGE_NAME, FILEMODE_WRITE_TEXT) {
   for (i = 0; i < num_params; i++) {
     printf("%s\n", m_params[i]);
   }
  }
}

//
// Set a real non-volatile parameter.
//
// Params:
//  name  Name of the parameter.
//  value The value to set.
// Returns:
//  none
// Changes:
//  m_params
//
void set_real_nv_param(string name, real value)
{
  string str;
  sprintf(str, "%f", value);
  set_nv_param(name, str);
}

//
// Returns the value of a real non-volatile parameter.
//
// Params:
//  name  Name of the parameter.
//  def   The default value.
//  can_abort Whether the program can abort if an error occurs.
// Returns:
//  real  The value.
// Changes:
//  none
//
real get_real_nv_param(string name)
{
  string str;
  str = get_nv_param(name, "0.000", YES);
  return strtod(str);
}
