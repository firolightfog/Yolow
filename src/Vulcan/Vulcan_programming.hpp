// the 1st channel of the RESET input identifies the kind of parameters transmitted
int PRG_MODE=(int)inputs[PRG_INPUT].getVoltage(0);

// #1 script: replace certain values with something 
if (PRG_MODE==1) {
	int PRG_TRACK=(int)inputs[PRG_INPUT].getVoltage(1);
	int PRG_CHANGE_FROM=(int)inputs[PRG_INPUT].getVoltage(2);
	int PRG_CHANGE_TO=(int)inputs[PRG_INPUT].getVoltage(3);
	for (int c=0;c<8;c++) {
		if (PRG_TRACK==c+1 || PRG_TRACK==0) {
			for (int step=0; step<16; step++) {
				if (grid_data[step][c]==PRG_CHANGE_FROM) {grid_data[step][c]=PRG_CHANGE_TO;}
			}
		}
	}
}

// #2 script: enter Euclidean rhythm with current selection 
else if (PRG_MODE==2) {
	int PRG_TRACK=(int)inputs[PRG_INPUT].getVoltage(1);
	int PRG_EUC_HITS=(int)inputs[PRG_INPUT].getVoltage(2);
	int PRG_EUC_SHIFTS=(int)inputs[PRG_INPUT].getVoltage(3);
	for (int c=0;c<8;c++) {
		if (PRG_TRACK==c+1 || PRG_TRACK==0) {
			for (int step=0; step<16; step++) {
				grid_data[step][c]=((((step + (16-PRG_EUC_SHIFTS)) * PRG_EUC_HITS) % 16) < PRG_EUC_HITS)?CURR_MODE:0;
			}
		}
	}
}
