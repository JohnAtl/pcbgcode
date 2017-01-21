// -*- Mode: Eagle -*-
/* test nonvolatile.h  */

#include "nonvolatile.h"

set_nv_param("first_name", "Jamile");
set_nv_param("last_name", "Johnson");
set_nv_param("email", "jamile@nowhere.com");

m_params[0] = ""
m_params[1] = ""
m_params[2] = ""

if (get_nv_param("first_name", "ERROR", NO) == "ERROR")
  exit("first_name failed");
if (get_nv_param("last_name", "ERROR", NO) == "ERROR")
  exit("last_name failed");
if (get_nv_param("email", "ERROR", NO) == "ERROR")
  exit("email failed");

if (get_nv_param("first_name", "ERROR", NO) != "Jamile")
  exit("first_name failed");
if (get_nv_param("last_name", "ERROR", NO) != "Johnson")
  exit("last_name failed");
if (get_nv_param("email", "ERROR", NO) != "jamile@nowhere.com")
  exit("email failed");


set_nv_param("first_name", "John");
set_nv_param("last_name", "Johnson");
set_nv_param("email", "pcbgcode@pcbgcode.org");

m_params[0] = ""
m_params[1] = ""
m_params[2] = ""

if (get_nv_param("first_name", "ERROR", NO) == "ERROR")
  exit("first_name failed");
if (get_nv_param("last_name", "ERROR", NO) == "ERROR")
  exit("last_name failed");
if (get_nv_param("email", "ERROR", NO) == "ERROR")
  exit("email failed");

if (get_nv_param("first_name", "ERROR", NO) != "John")
  exit("first_name failed");
if (get_nv_param("last_name", "ERROR", NO) != "Johnson")
  exit("last_name failed");
if (get_nv_param("email", "ERROR", NO) != "pcbgcode@pcbgcode.org")
  exit("email failed");


