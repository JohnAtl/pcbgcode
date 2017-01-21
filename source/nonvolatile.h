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

if (filetime(STORAGE_NAME) == 0) {
  output(STORAGE_NAME, "wt") {
    printf("created%c%s\s", SEPARATOR, t2string(time()));
  }
}

void empty_m_params()
{
  int i;
  
  while (m_params[i] != "")
    m_params[i++] = "";
}

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
/*   string t;
  sprintf(t, "there are %d params", num_params);
  dlgMessageBox(t);
 */  return num_params;
}

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
  output(STORAGE_NAME, "wt") {
   for (i = 0; i < num_params; i++) {
     printf("%s\n", m_params[i]);
   }
  }
}

void set_real_nv_param(string name, real value)
{
  string str;
  sprintf(str, "%f", value);
  set_nv_param(name, str);
}

real get_real_nv_param(string name)
{
  string str;
  str = get_nv_param(name, "0.000", YES);
  return strtod(str);
}
