/*
* Generate g-code for milling PC boards.
*
* Copyright 2004-2009 by John Johnson Software, LLC.
* See readme.html for copyright information.
*
*/

// BEGIN_PLUGIN_INCLUDES
dlgVBoxLayout {
  dlgHBoxLayout {
    dlgStretch(1);
    dlgVBoxLayout {
      #include "settings.plugin"
    }
    dlgVBoxLayout {
      #include "calculator.plugin"
    }
    dlgStretch(1);
  }
  dlgStretch(1);
}
// END_PLUGIN_INCLUDES
