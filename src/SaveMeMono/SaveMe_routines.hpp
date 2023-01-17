// OK, I know ... this is ugly but I had to find a way to shift the (unvisible) sequence 
void shiftSeq() {
	float tempStep=0.0f;
	if (indexShift==0) {
		tempStep=theSeq[0];
		for (int s=0;s<allSteps-1;s++) {theSeq[s]=theSeq[s+1];}
		theSeq[allSteps-1]=tempStep;
	}
	else if (indexShift==1) {
		tempStep=theSeq[allSteps-1];
		for (int s=allSteps-1;s>0;s--) {theSeq[s]=theSeq[s-1];}
		theSeq[0]=tempStep;
	}
	// theShuffle[allSteps]=std::random_shuffle(&theShuffle[0], &theShuffle[allSteps]);
	indexShift=-1;
}

// this will roll the dice for all 256 slots :)
void randomizeFields() {
	for (int i=0; i<256; i++) {
		if (indexLFO==1) {theSeq[i]=(rack::random::uniform()*10)-5;}	// internal LFO for '-5V to 5V'
		else if (indexLFO==2) {theSeq[i]=(rack::random::uniform()*10);}		// internal LFO for '0V to 10V'
		else if (indexLFO==3) {theSeq[i]=(rack::random::uniform()*2-1);}		// internal LFO for '-1V to 1V'
		else if (indexLFO==4) {theSeq[i]=(rack::random::uniform());}		// internal LFO for '0V to 1V'
		else if (indexLFO==0) {theSeq[i]=(rand() % 2) *10;}					// internal LFO for gate sequences
	}
}

// -------------------------------------

// open a dialog and return a filename
std::string selectionDir = asset::user("selections"); //my preferred directory
std::string selectFileVCV(osdialog_file_action OSDIALOG_MODE) {
	std::string filename_string = "";
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
	for (int i=0; i<256; i++) {
		if (method==0) {	// One Zero
			buffer+=(2.0f > theSeq[i])?'0':'1';
			if (i % allSteps == allSteps-1 || i==255) buffer+='\n';
		}
		else {	// One Point
			buffer+=std::to_string(theSeq[i]);
			buffer+=(i % allSteps == allSteps-1 || i==255)?'\n':',';
		}
	}
	std::ofstream newFile(selectFileVCV(OSDIALOG_SAVE));
	newFile << buffer;
	newFile.close();	 
 }

// VoxGlitch One Point (1) & Zero (0) text file export
void importFieldsFromTxt(int method) {
	std::ifstream input_file(selectFileVCV(OSDIALOG_OPEN));
	// test if file open
	if (input_file)	{
		std::string line = "";
		// VoxGlitch One Zero file to be collected
		if (method==0) {
			char ch; int i=0;
			while (ch!=EOF && i<256) {
				input_file.get(ch);
				if (ch=='1') {theSeq[i]=10;}
				else if (ch=='0') {theSeq[i]=0;}
				i++;
				// else if (ch==','){i++;}
				// else if (ch=='\n') {i++;}
				// else skipping spaces, A-Z letters, whatever
			}
		}
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
		input_file.close();
    }
}

// -------------------------------------

