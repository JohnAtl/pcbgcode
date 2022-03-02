/**********************************************************
 *
 * Load/Save tool and machine settings.
 *
 * Plugin Definitions
 * NAME = settings
 * DESC = Load and save settings
 * AUTHOR = John Johnson
 * TAB_NAME = Plugins
 **********************************************************/

int FILENAME_DISPLAY_WIDTH = 25;

int set_selection = -1;
string set_files[];
string disp_files[];
int num_set_files;

num_set_files = fileglob(set_files, g_path + "/settings/saved/*.def");

for(int ii=0; ii < num_set_files; ii++) {
 disp_files[ii] = elided_path(set_files[ii], FILENAME_DISPLAY_WIDTH);
}

void save_any_changes()
{
  save_pcb_defaults();
  save_pcb_gcode_options();
  save_pcb_machine();
}

void save_settings_to(string file)
{
  file = filesetext(file, "");
  save_any_changes();
  filecopy(g_path + "/settings/pcb-defaults.h", file + ".def");
  filecopy(g_path + "/settings/pcb-machine.h", file + ".mac");
}

void save_settings(string file)
{
  save_settings_to(file);
}

void save_settings_as()
{
  string new_file = dlgFileSave("Save as", g_path + "/settings/saved/");
  if (new_file != "") {
    save_settings_to(new_file);
  }
}

void load_settings(string file)
{
  file = filesetext(file, "");
  filecopy(file + ".def", g_path + "/settings/pcb-defaults.h");
  filecopy(file + ".mac", g_path + "/settings/pcb-machine.h");
}
