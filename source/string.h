// -*- Mode: Eagle -*-
/*
 *
 * String utilities.
 *
 * Copyright 2007 - 2009 by John Johnson Software, LLC.
 * All Rights Reserved.
 *
 */



int STR_NOT_FOUND = -1;
char REC_SEP = '\v';
char FIELD_SEP = '\t';
 
string leftstr(string s, int n)
{
  return strsub(s, 0, n);
}

string rightstr(string s, int n)
{
  return strsub(s, strlen(s) - n);
}

string ltrim(string s)
{
 while (leftstr(s, 1) == " ") {
   s = strsub(s, 1);
 }
 return s;
}
 
string rtrim(string s)
{
  while (strsub(s, strlen(s) - 1, 1) == " ") {
    s = strsub(s, 0, strlen(s) - 1);
  }
  return s;
}

string trim(string s) 
{
 return ltrim(rtrim(s));
}

string remove_last_slash(string s)
{
  if (rightstr(s, 1) == "/") {
    s = leftstr(s, strlen(s) - 1);
  }
  return s;
}
    
string remove_last_dir(string s)
{
  int pos;
  
  pos = strrstr(s, "/");
  return leftstr(s, pos);
}

string key_value_record(string a, string b)
{
  return a + FIELD_SEP + b + REC_SEP;
}

/*
 * Replace text in a string with other text.
 *
 */
string substitute(string s, string replacements)
{
  int pos;
  string keys[];
  string values[];
  string records[];
  string fields[];
  int num_keys;
  int num_fields;
  int i;
  string temp;
  int last_match;
  
  num_keys = strsplit(records, replacements, REC_SEP);
  num_keys--;
  for (i=0; i<num_keys; i++) {
    num_fields = strsplit(fields, records[i], FIELD_SEP);
    if (num_fields != 2) {
      sprintf(temp, "Software error\n"
      "substitute replacements has the wrong\n"
      "number of fields in record %d of %d.\n"
      "\nThe record is %s\n", i + 1, num_fields, records[i]);
      dlgMessageBox(temp);
      exit(1);
    }
    keys[i] = fields[0];
    values[i] = fields[1];
  }
  
  do {
    last_match = STR_NOT_FOUND;
    for (i=0; i<num_keys; i++) {
      pos = strstr(s, keys[i]);
      if (pos != STR_NOT_FOUND) {
        temp = leftstr(s, pos);
        temp += values[i];
        temp += strsub(s, pos + strlen(keys[i]));
        s = temp;
        last_match = pos;
        break;
      }
    }
  } while (last_match != STR_NOT_FOUND);
  return s;
}

/*
 * Find the first character in chars in string s.
 *
 */
int strhas(string s, string chars)
{
  int i;
  int pos;
  
  for (i=0; i<strlen(chars); i++) {
    pos = strchr(s, chars[i]);
    if (pos != -1) {
      return pos;
    }
  }
  return STR_NOT_FOUND;
}

int stronlyhas(string s, string chars)
{
  int i;
  int pos;
  
  for (i=0; i<strlen(s); i++) {
    pos = strchr(chars, s[i]);
    if (pos == -1) {
      return 0;
    }
  }
  return 1;
}

/*
 * Return how many c's are in s.
 *
 */
int strcnt(string s, char c)
{
  int cnt;
  int pos;

  for (pos = 0; pos < strlen(s); pos++) {
    if (s[pos] == c) {
      cnt++;
    }
  }
      
  return cnt;
}

/*
 * Find a key = value pair in a string.
 * Value goes to the end of the line it is on.
 *
 */
string get_value(string s, string key)
{
  int pos;
  int nl_pos;
  int space_pos;
  string found_key;
  string found_value;
  string assignment;
  string tokens[];
  
  pos = strstr(s, key);
  if (pos == STR_NOT_FOUND) {
    return "";
  }
  nl_pos = strstr(s, "\n");
  assignment = strsub(s, pos, nl_pos - pos);
  strsplit(tokens, assignment, '=');
  found_key = trim(tokens[0]);
  found_value = trim(tokens[1]);
  if (key == found_key) {
    return found_value;
  }
  return "";
}

string get_value_from_file(string path, string key)
{
  string text;
  
  fileread(text, path);
  return get_value(text, key);
}



string elided_path(string path, int width)
{
  int i;
  string ep;
  int pos;
  int keep_pos;
  
  ep = "";
  pos = -1;
  keep_pos = -1;
  while (1) {
    pos = strrchr(path, '/', pos);
    if (pos == -1) { break; }
    keep_pos = pos;
    if ((strlen(path) - keep_pos) > width) { break; }
    pos = pos - strlen(path) - 1;
  }
  if (keep_pos <= 0) { return path; }
  return "..." + strsub(path, keep_pos);
}


string fi(string f, int i) { 
  string str;
  if (strcnt(f, '%') < 1) {
    return "";
  }
  else {
    sprintf(str, f, i);
  }
  return str;
}

string fir(string f, int i, real r) { 
  string str;
  if (strcnt(f, '%') < 1) {
    return fi(f, i);
  }
  else {
    sprintf(str, f, i, r);
  }
  return str;
}

string fr(string f, real r) { 
  string str;
  if (strcnt(f, '%') < 1) {
    return "";
  }
  else {
    sprintf(str, f, r);
  }
  return str;
}

string frr(string f, real r1, real r2) { 
  string str;
  if (strcnt(f, '%') < 2) {
    return fr(f, r1);
  }
  else {
    sprintf(str, f, r1, r2);
  }
  return str;
}

string frrr(string f, real r1, real r2, real r3) { 
  string str;
  if (strcnt(f, '%') < 3) {
    return frr(f, r1, r2);
  }
  else {
    sprintf(str, f, r1, r2, r3);
  }
  return str;
}

string frrrr(string f, real r1, real r2, real r3, real r4) { 
  string str;
  if (strcnt(f, '%') < 4) {
    return frrr(f, r1, r2, r3);
  }
  else {
    sprintf(str, f, r1, r2, r3, r4);
  }
  return str;
}

string frrrrr(string f, real r1, real r2, real r3, real r4, real r5) { 
  string str;
  if (strcnt(f, '%') < 5) {
    return frrrr(f, r1, r2, r3, r4);
  }
  else {
    sprintf(str, f, r1, r2, r3, r4, r5);
  }
  return str;
}

string frrrrrr(string f, real r1, real r2, real r3, real r4, real r5, real r6) { 
  string str;
  if (strcnt(f, '%') < 6) {
    return frrrrr(f, r1, r2, r3, r4, r5);
  }
  else {
    sprintf(str, f, r1, r2, r3, r4, r5, r6);
  }
  return str;
}

string frrrrrrr(string f, real r1, real r2, real r3, real r4, real r5, real r6, real r7) { 
  string str;
  if (strcnt(f, '%') < 7) {
    return frrrrrr(f, r1, r2, r3, r4, r5, r6);
  }
  else {
    sprintf(str, f, r1, r2, r3, r4, r5, r6, r7);
  }
  return str;
}

string fs(string f, string s) { 
  string str;
  if (strcnt(f, '%') < 1) {
    return "";
  }
  else {
    sprintf(str, f, s);
  }
  return str;
}
