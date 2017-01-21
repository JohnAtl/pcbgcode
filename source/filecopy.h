// -*- Mode: Eagle -*-
void filecopy(string from_name, string to_name)
{
	string contents[];
	int num_lines;

	fileerror();
	num_lines = fileread(contents, from_name);

	if(fileerror()) {
		dlgMessageBox("Error: Could not open " + from_name 
		+ " for copying to " + to_name);
		exit(1);
	}

	output(to_name) {
		for (int i=0; i < num_lines; i++) {
			printf("%s\n", contents[i]);
		}
	}
	if(fileerror()) {
		dlgMessageBox("Error: Could not write the contents of " 
		+ from_name + " to " + to_name);
		exit(1);
	}
}
