/* 
This file is needed by PokeMe. 
I'm very proud of this one. It allows entering Euclidean hits be enabling 
programming on the RESET input.
Programming feature is DISABLED at startup by default.
*/

// programmer experience
if (indexPRG==1 && inputs[RESET_INPUT].isConnected()==true 
&& inputs[RESET_INPUT].channels==4) {

	// this is working!! :)
	if (inputs[RESET_INPUT].getVoltage(0)==0) {
		int xLEN=clamp((int)floor(inputs[RESET_INPUT].getVoltage(1)*100),1,33);
		int xHIT=clamp((int)floor(inputs[RESET_INPUT].getVoltage(2)*100),0,xLEN);
		int xSHF=clamp((int)floor(inputs[RESET_INPUT].getVoltage(3)*100),0,xLEN);		
		for (int i=0;i<SEQLEN;i++) {
			if (grid_data[i]==10 || grid_data[i]==CURR_MODE) {grid_data[i]=0;};
		}
		if (xLEN<=31 && xLEN>=1) {grid_data[xLEN] = 10;}
		// euclidean magic here
		int bucket = 0; //out variable to add pulses together for each step
		for ( int i=0; i<xLEN; i++){ 
			bucket += xHIT; 
				if(bucket >= xLEN) {
				bucket -= xLEN;
				grid_data[(xLEN - 1 - (i - xSHF % xLEN)) % xLEN] = CURR_MODE;
			}
		}
	}	

	// doesn't work yet
	else if (inputs[RESET_INPUT].getVoltage(0)==1) {
		int xLEN=clamp((int)floor(inputs[RESET_INPUT].getVoltage(1)*10),1,33);
		int xHIT=clamp((int)floor(inputs[RESET_INPUT].getVoltage(2)*10),0,xLEN);
		int xSHF=clamp((int)floor(inputs[RESET_INPUT].getVoltage(3)*10),0,xLEN);
		for (int i=0;i<SEQLEN;i++) {
			if (grid_data[i]==10 || grid_data[i]==CURR_MODE) {grid_data[i]=0;};
		}
		if (xLEN<=31 && xLEN>=1) {grid_data[xLEN]=10;}
		// golomb ruler 
		int position=0;
		for (int i=0; i<xHIT; i++) {
			grid_data[position] = CURR_MODE;
			position = (position + xSHF) % xLEN;
		}
	}	

	// doesn't work yet
	else if (inputs[RESET_INPUT].getVoltage(0)==2) {
		int xLEN=clamp((int)floor(inputs[RESET_INPUT].getVoltage(1)*10),1,33);
		int xHIT=clamp((int)floor(inputs[RESET_INPUT].getVoltage(2)*10),0,xLEN);
		// int xSHF=clamp((int)floor(inputs[RESET_INPUT].getVoltage(3)*10),0,xLEN);
		for (int i=0;i<SEQLEN;i++) {
			if (grid_data[i]==10 || grid_data[i]==CURR_MODE) {grid_data[i]=0;};
		}
		if (xLEN<=31 && xLEN>=1) {grid_data[xLEN] = 10;}
		// sparse ruler 
		int rp=0;
		for (int i=0; i<xHIT; i++) {
			for (int r=0; r<999; r++) {
				rp=rand() % xLEN;
				if (grid_data[rp]!=CURR_MODE) {grid_data[rp]=CURR_MODE; break;}   
			}
		}
	}	
	
}