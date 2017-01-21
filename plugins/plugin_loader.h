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
/*    dlgVBoxLayout {
      dlgGroup("Square X and Y (doesn't do anything)") {
        #include "square_x_and_y.plugin"
      }
      dlgStretch(1);
    }
    dlgVBoxLayout {
      dlgGroup("Square Z") {
        #include "square_z.plugin"
      }
      dlgStretch(1);
    }
    dlgVBoxLayout {
      dlgGroup("Filenames") {
        #include "filenames.plugin"
      }
      dlgStretch(1);
    }*/
  }
  dlgHBoxLayout {
      dlgStretch(1);
    dlgVBoxLayout {
      #include "calculator.plugin"
    }
    dlgStretch(1);
  }
  dlgStretch(1);
}
// END_PLUGIN_INCLUDES
