// -*- Mode: Eagle -*-
/*
* A stack.
*
*/

string m_ary[];
char FIELD_SEP = '\t';
char RECORD_SEP = '\n';

string push(string stack, string entry)
{
  return stack + entry + RECORD_SEP;
}

string top(string stack)
{
  int num = strsplit(m_ary, stack, RECORD_SEP);
  return m_ary[num - 1];
}

string pop(string stack)
{
  string str;
  int num = strsplit(m_ary, stack, RECORD_SEP);
  m_ary[num - 1] = "";
  str = strjoin(m_ary, RECORD_SEP);
  int off = strrchr(str, RECORD_SEP);
  return strsub(str, off - 1);
}

string bottom(string stack)
{
  return "";
}

string shift(string stack)
{
  return "";
}

string unshift(string stack, string entry)
{
  return "";
}

string t;
t = push(t, "aaa");
t = push(t, "bbb");
t = push(t, "ccc");


string msg;
string qqq;
sprintf(msg, "stack:\n%s", t);
qqq = qqq + msg;
string s;
s = top(t);
sprintf(msg, "top:\n%s", s);
qqq = qqq + msg;
t = pop(t);
s = top(t);
sprintf(msg, "top:\n%s", s);
qqq = qqq + msg;

t = push(t, "ddd");

sprintf(msg, "stack:\n%s", t);
qqq = qqq + msg;

dlgMessageBox(qqq);
