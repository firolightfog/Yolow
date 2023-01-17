// -------------------------------------

// open a dialog and return a filename
std::string selectionDir = ""; //my preferred directory
std::string selectFileVCV(osdialog_file_action OSDIALOG_MODE) {
	std::string filename_string = "";
	if (selectionDir=="") selectionDir = asset::user("selections");
	osdialog_filters *filters = osdialog_filters_parse("TXT:txt");
	char *filename = osdialog_file(OSDIALOG_MODE, selectionDir.c_str(), NULL, filters);
	// char *filename = osdialog_file(OSDIALOG_SAVE, selectionDir.c_str(), NULL, filters);
	// char *filename = osdialog_file(OSDIALOG_OPEN, "", NULL, filters);
	if (filename != NULL) {
		filename_string.assign(filename);
		osdialog_filters_free(filters);
		std::free(filename);
	}
	return (filename_string);
}
	
// VoxGlitch One Point (1) & Zero (0) text file export
void exportFieldsToTxt(int method) {
	std::string buffer="";	
	for (int xr=0; xr<ROWS; xr++) {
		for (int xc=0; xc<COLS; xc++) {
			if (method==0) {	// One Zero
				switch (grid_data[xc][xr]) {
					case 10: buffer+='\n'; break;
					case 0: buffer+='0'; break;				
					default: buffer+='1'; 				
				}
				// buffer+=(grid_data[xc][xr]==0)?'0':'1';
				if (xc==COLS-1) buffer+='\n';
				else if (grid_data[xc][xr]==10) {break;} // 10 is end of the track
			}
			else {	// One Point
				buffer+=std::to_string(grid_data[xc][xr]);
				buffer+=(xc==COLS-1)?'\n':',';
			}
		}
	}
	std::ofstream newFile(selectFileVCV(OSDIALOG_SAVE));
	newFile << buffer;
	newFile.close();	 
 }

// VoxGlitch One Point (1) & Zero (0) text file export
void importFieldsFromTxt(int method) {
	// let's clear the old patterns
	for(int row=0; row < ROWS; row++) {
		for(int col=0; col < COLS; col++) {
			grid_data[col][row] = 0;
		}
	}
	// let's see the txt file
	std::ifstream input_file(selectFileVCV(OSDIALOG_OPEN));
	// test if file open
	if (input_file)	{
		std::string line = "";
		int xc=0; int xr=0;
		// VoxGlitch One Zero file to be collected
		if (method==0) {
			char ch;
			while (ch!=EOF && xr<ROWS-1) {
				input_file.get(ch);
				if (ch=='1') {grid_data[xc][xr]=1;xc++;}
				else if (ch=='0') {grid_data[xc][xr]=0;xc++;}
				else if (ch=='\n') {
					if (xc<COLS-1 && xc>0) {grid_data[xc][xr]=10;}
					xr++;
					xc=0;
				}
				if (xc>=COLS) xc=0;
				// else if (ch==','){i++;}
				// else skipping spaces, A-Z letters, whatever
			}
		}
	/*
		// VoxGlitch One Point file to be collected
		else if (method==1) {
            std::string line = ""; int i=0;
            while (std::getline(input_file, line) && i<256) {
                // std::vector<float> sequence;
                std::stringstream ss(line);
                while( ss.good() ) {
                    std::string substr;
                    getline(ss, substr,',');
                    // sequence.push_back( std::stod(substr) );
					theSeq[i]=std::stod(substr);
					i++;
                }
                // sequences.push_back(sequence);
            }
		}
	*/
		input_file.close();
    }
}
