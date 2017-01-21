/**********************************************************
 *
 * A basic calculator.
 *
 * Plugin Definitions
 * NAME = calculator
 * DESC = Basic calculator
 * AUTHOR = John Johnson
 * TAB_NAME = Plugins
 **********************************************************/

int UNARY_BEGIN = 0x80;
int ANGLE_IN    = 0x40;
int ANGLE_OUT   = 0x20;

enum {
  OP_INVALID,
  OP_CLEAR,
  OP_ENTER,
  OP_ADD,
  OP_SUBTRACT,
  OP_DIVIDE,
  OP_MULTIPLY,
  OP_RADIAN,
  OP_DEGREE
};

enum {  
  OP_MEMORY_PLUS = 0x80 /* UNARY_BEGIN */,
  OP_MEMORY_STORE,
  OP_MEMORY_RECALL,
  OP_MEMORY_CLEAR
};

enum {
  OP_SINE     = 0xC0 /* UNARY_BEGIN + ANGLE_IN */,
  OP_COSINE,
  OP_TANGENT,
  OP_ARC_SINE,
  OP_ARC_COSINE,
  OP_ARC_TANGENT
};

enum {
  OP_INV_SINE   = 0xA0 /* UNARY_BEGIN + ANGLE_OUT */,
  OP_INV_COSINE,
  OP_INV_TANGENT
};

enum {
  ANG_INVALID,
  ANG_RADIAN,
  ANG_DEGREE
  /* ANG_GRADIANS - sorry */
};

int m_angle_mode;
string g_angle_mode_str;
int g_memory;

real accum;
real depth;
int passes;
string nc_file;
int unit_metric;
real length;
string result;

string m_result = "0";
string m_entry = "0";

string format_number(real n)
{
  string tmp;

  sprintf(tmp, "%f", n);
  return tmp;
  
}

real set_accum(string s)
{
  accum = strtod(s);
  return accum;
}

string set_result(real r)
{
  m_result = format_number(r);
//  set_nv_param("calc_result", m_result);
  return m_result;
}

real set_angle_mode(int mode)
{
  real result;
  string num;
  int used_entry;
  
  result = strtod(m_result);
  
  if (m_entry != "") {
    num = m_entry;
    used_entry = 1;
  }
  else if (m_result != "") {
    num = m_result;
  }
  else {
    num = m_result;
  }
  
  if (m_angle_mode == ANG_DEGREE && mode == ANG_RADIAN) {
    result = strtod(num) / 180 * PI;
  }
  else if (m_angle_mode == ANG_RADIAN && mode == ANG_DEGREE) {
    result = strtod(num) / PI * 180;
  }
  m_angle_mode = mode;

  switch (m_angle_mode) {
    case ANG_RADIAN:
      g_angle_mode_str = "rad";
    break;
    case ANG_DEGREE:
      g_angle_mode_str = "deg";
    break;
  }

//  set_nv_param("calc_angle_mode", g_angle_mode_str);
  
//  if (used_entry) {
    m_entry = format_number(result);
//  }
//  else {
    set_result(result);
//  }*/
  return result;
}

string get_mode_str()
{
  return (m_angle_mode == ANG_RADIAN) ? "rad" : "deg";
}

real rad(real n)
{
  return (m_angle_mode == ANG_RADIAN) ? n : n / 180 * PI;
}

real unrad(real n)
{
  return (m_angle_mode == ANG_RADIAN) ? n : n / PI * 180;
}

int is_unary(int op)
{
  return (op & UNARY_BEGIN) ? 1 : 0;
}

void set_memory(real n)
{
  string tmp;
  
  g_memory = n;
//  set_nv_param("calc_memory", format_number(n));
}

real get_memory()
{
  return g_memory;
}

void clear_memory()
{
  g_memory = 0;
}

int m_entering;
void calc_enter(string n)
{
  if (!m_entering) {
    m_entry = n;
    m_entering = 1;
  }
  else {
    m_entry += n;
  }
}

real calc(string a, int OPERATION)
{
  real ra;
  int no_clear;
  
  m_entering = 0;

  ra = strtod(a);
  if (m_entry == "" && m_result != "" && is_unary(OPERATION))
    ra = strtod(m_result);
  
  if (OPERATION & ANGLE_IN) {
    ra = rad(ra);
  }
  
  switch(OPERATION) {
    case OP_ENTER:
      accum = ra;
      set_result(accum);
    break;
    case OP_ADD:
      accum = accum + ra;
    break;
    case OP_SUBTRACT:
      accum = accum - ra;
    break;
    case OP_DIVIDE:
      accum = accum / ra;
    break;
    case OP_MULTIPLY:
      accum = accum * ra;
    break;
    case OP_DEGREE:
      accum = set_angle_mode(ANG_DEGREE);
    break;
    case OP_RADIAN:
      accum = set_angle_mode(ANG_RADIAN);
    break;

    case OP_MEMORY_PLUS:
      set_memory(ra + get_memory());
      accum = ra;
    break;
    case OP_MEMORY_STORE:
      set_memory(ra);
      accum = ra;
    break;
    case OP_MEMORY_RECALL:
      m_entry = format_number(get_memory());
      no_clear = 1;
      accum = ra;
    break;
    case OP_MEMORY_CLEAR:
      clear_memory();
      accum = ra;
    break;

    case OP_COSINE:
      accum = cos(ra);
    break;
    case OP_SINE:
      accum = sin(ra);
    break;
    case OP_TANGENT:
      accum = tan(ra);
    break;

    case OP_ARC_COSINE:
      accum = acos(ra);
    break;
    case OP_ARC_SINE:
      accum = asin(ra);
    break;
    case OP_ARC_TANGENT:
      accum = atan(ra);
    break;

    case OP_CLEAR:
      accum = 0;
    break;
    default:
      dlgMessageBox("Invalid OPERATION");
  }
  
  if (OPERATION & ANGLE_OUT) {
    accum = unrad(accum);
  }
  
  set_result(accum);
  if (!no_clear) m_entry = "";
  return accum;
}

/**********************************
 * Initialization.
 **********************************/

set_angle_mode(ANG_DEGREE);


//~ set_memory(strtod(get_nv_param("calc_memory", "0.0", 0 /* don't abort */)));
//~ if (get_nv_param("calc_angle_mode", "deg", 0 /* don't abort */) == "deg") 
//~ set_angle_mode(ANG_DEGREE);
//~ else
//~ set_angle_mode(ANG_RADIAN);
//~ set_result(strtod(get_nv_param("calc_result", "0.0", 0 /* don't abort */)));

