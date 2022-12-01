/* 
This file is needed by PushmeSeq and its PushmeMore expander. 
I wanted to avoid typing the same thing twice, so here it is.
*/

	// changes the pre-programmed step value based on the indexPrec
	float getVoltNow(int prm) {
		float rndl[7]={1,0.95,0.85,0.75,0.6,0.5,0.35};
		if (indexPrec==0) {return paramVal[prm]*10.0f;}
		else if (indexPrecMode==0 && rndl[indexPrec]<rack::random::uniform()) {
			return abs(paramVal[prm]-1)*10.0f;
		}
		else if ((indexPrecMode==1 && paramVal[prm]==0) && rndl[indexPrec]<rack::random::uniform()) {
			return abs(paramVal[prm]-1)*10.0f;
		}
		else if ((indexPrecMode==2 && paramVal[prm]==1) && rndl[indexPrec]<rack::random::uniform()) {
			return abs(paramVal[prm]-1)*10.0f;
		}
		return paramVal[prm]*10.0f;
		// float rndl[7]={1,0.95,0.85,0.75,0.6,0.5,0.35};
		// if (indexPrec!=0 && rndl[indexPrec]<rack::random::uniform()) {
			// return abs(paramVal[prm]-1)*10.0f;
		// }
		// return paramVal[prm]*10.0f;
	}

	// randomizes a specific zone; called by shortkeys
	void keyKnob(int rndSection) {
		for (int b=8*rndSection;b<8*(rndSection+1);b++) {
			params[SEQ_PARAM+b].setValue(rand() % 2);
			// lights[SEQ_LIGHT+b].setBrightness(params[SEQ_PARAM+b].getValue());
		}	
		updateLights();		
	}

	// clears a specific zone; called by shortkeys
	void clearKnob(int rndSection) {
		for (int b=8*rndSection;b<8*(rndSection+1);b++) {
			params[SEQ_PARAM+b].setValue(0);
			lights[SEQ_LIGHT+b].setBrightness(0);
		}	
	}

	// inverts button values; called by shortkeys
	void invKnob() {
		for (int b=0;b<48;b++) {
			params[SEQ_PARAM+b].setValue(abs(params[SEQ_PARAM+b].getValue()-1));
			lights[SEQ_LIGHT+b].setBrightness(params[SEQ_PARAM+b].getValue());
		}
	}

	// moves the this of a specific zone earlier; called by shortkeys
	void moveupdnKnob(int rndSection) {
		float nextValue=0;
		// float firstValue=params[SEQ_PARAM+8*rndSection+0].getValue();
		float lastValue=params[SEQ_PARAM+8*rndSection+7].getValue();
		for (int b=8*(rndSection+1)-1;b>8*rndSection;b--) {
			nextValue=params[SEQ_PARAM+b-1].getValue();
			// params[SEQ_PARAM+b+1].setValue(0);
			// lights[SEQ_LIGHT+b+1].setBrightness(0);
			params[SEQ_PARAM+b].setValue(nextValue);
			lights[SEQ_LIGHT+b].setBrightness(nextValue);
		}
		nextValue=lastValue;
		lights[SEQ_LIGHT+8*(rndSection)].setBrightness(nextValue);
		params[SEQ_PARAM+8*(rndSection)].setValue(nextValue);	
		updateLights();		
	}

	// unique & special randomization modes for certain modes
	void xrndKnob() {
		if (indexSeqMode==3) {
			int theRandomValueForThisStep=rand() % 3;
			for (int b=0;b<16; b++) {
				theRandomValueForThisStep=rand() % 3;
				params[SEQ_PARAM+00+b].setValue((theRandomValueForThisStep==0)?1:0);
				params[SEQ_PARAM+16+b].setValue((theRandomValueForThisStep==1)?1:0);
				params[SEQ_PARAM+32+b].setValue((theRandomValueForThisStep==2)?1:0);
			}			
		}
		/* Euclidean spaceholder; how to collect expander info?!?
		else if (indexSeqMode==1) {
			for (int i=0;i<48;i++) {
				params[SEQ_PARAM+i].setValue(rand()% 2);	// the trick				
			}
		}
		*/
		else if (indexSeqMode==0) {
			int lastValueHere=-1;
			for (int b=15;b>=0; b--) {
				if (paramVal[SEQ_PARAM+b]==1) {lastValueHere=b; break;}
			}
			if (lastValueHere==-1) {
				params[SEQ_PARAM+00].setValue(1);
				params[SEQ_PARAM+16].setValue(1);
				params[SEQ_PARAM+32].setValue(1);			
				;}
			else if (lastValueHere==15) {
				for (int b=48-1;b>=0; b--) {params[SEQ_PARAM+b].setValue(0);}
			}	
			else {
				params[SEQ_PARAM+00+lastValueHere+1].setValue(1);
				params[SEQ_PARAM+16+lastValueHere+1].setValue(1);
				params[SEQ_PARAM+32+lastValueHere+1].setValue(1);
			}
		}
		else {
			for (int b=0;b<48; b++) {
				params[SEQ_PARAM+b].setValue(rand() % 2);
			}	
		}	
		updateLights();		
	}

	// allows recording a sequence by pressing keys (x,c,v,b,n,m)
	void beatKnob(int hitKey) {

		// outputs[TRIGGER_OUTPUT].setVoltage(hitKey);
		if (indexSeqMode==0) {
			if (hitKey==88 /*x*/) {params[SEQ_PARAM+stepRand].setValue(abs(paramVal[SEQ_PARAM+stepRand]-1));}
		}
		else if (indexSeqMode==1) {
			if (hitKey==88 /*x*/) {params[SEQ_PARAM+stepA].setValue(abs(paramVal[SEQ_PARAM+stepA]-1));}
		}
		else if (indexSeqMode==2) {
			if (hitKey==88 /*x*/) {params[SEQ_PARAM+stepA].setValue(abs(paramVal[SEQ_PARAM+stepA]-1));}
			else if (hitKey==67 /*c*/) {params[SEQ_PARAM+32+stepB].setValue(abs(paramVal[SEQ_PARAM+32+stepB]-1));}
		}
		else if (indexSeqMode==3) {
			if (hitKey==88 /*x*/) {params[SEQ_PARAM+stepB].setValue(abs(paramVal[SEQ_PARAM+stepB]-1));}
			else if (hitKey==67 /*c*/) {params[SEQ_PARAM+16+stepB].setValue(abs(paramVal[SEQ_PARAM+16+stepB]-1));}
			else if (hitKey==86 /*v*/) {params[SEQ_PARAM+32+stepB].setValue(abs(paramVal[SEQ_PARAM+32+stepB]-1));}
		}
		else if (indexSeqMode==4) {
			if (hitKey==88 /*x*/) {params[SEQ_PARAM+stepB].setValue(abs(paramVal[SEQ_PARAM+stepB]-1));}
			else if (hitKey==67 /*c*/) {params[SEQ_PARAM+16+stepB].setValue(abs(paramVal[SEQ_PARAM+16+stepB]-1));}
			else if (hitKey==86 /*v*/) {params[SEQ_PARAM+32+stepC].setValue(abs(paramVal[SEQ_PARAM+32+stepC]-1));}
			else if (hitKey==66 /*b*/) {params[SEQ_PARAM+40+stepC].setValue(abs(paramVal[SEQ_PARAM+40+stepC]-1));}
		}
		else if (indexSeqMode==5) {
			if (hitKey==88 /*x*/) {params[SEQ_PARAM+stepB].setValue(abs(paramVal[SEQ_PARAM+stepB]-1));}
			else if (hitKey==67 /*c*/) {params[SEQ_PARAM+16+stepC].setValue(abs(paramVal[SEQ_PARAM+16+stepC]-1));}
			else if (hitKey==86 /*v*/) {params[SEQ_PARAM+24+stepC].setValue(abs(paramVal[SEQ_PARAM+24+stepC]-1));}
			else if (hitKey==66 /*b*/) {params[SEQ_PARAM+32+stepC].setValue(abs(paramVal[SEQ_PARAM+32+stepC]-1));}
			else if (hitKey==78 /*v*/) {params[SEQ_PARAM+40+stepRand].setValue(abs(paramVal[SEQ_PARAM+40+stepRand]-1));}
		}
		else if (indexSeqMode==6) {
			if (hitKey==88 /*x*/) {params[SEQ_PARAM+stepC].setValue(abs(paramVal[SEQ_PARAM+stepC]-1));}
			else if (hitKey==67 /*c*/) {params[SEQ_PARAM+16+stepC].setValue(abs(paramVal[SEQ_PARAM+16+stepC]-1));}
			else if (hitKey==86 /*v*/) {params[SEQ_PARAM+32+stepC].setValue(abs(paramVal[SEQ_PARAM+32+stepC]-1));}
			else if (hitKey==66 /*b*/) {params[SEQ_PARAM+32+stepC].setValue(abs(paramVal[SEQ_PARAM+32+stepC]-1));}
			else if (hitKey==78 /*n*/) {params[SEQ_PARAM+32+stepC].setValue(abs(paramVal[SEQ_PARAM+32+stepC]-1));}
			else if (hitKey==77 /*m*/) {params[SEQ_PARAM+32+stepRand].setValue(abs(paramVal[SEQ_PARAM+32+stepRand]-1));}
			}

		if (hitKey==88 /*x*/) {outputs[TRIGGER_OUTPUT].setVoltage(10.0f,0);}
		else if (hitKey==67 /*c*/) {outputs[TRIGGER_OUTPUT].setVoltage(10.0f,1);}
		else if (hitKey==86 /*v*/) {outputs[TRIGGER_OUTPUT].setVoltage(10.0f,2);}
		else if (hitKey==66 /*b*/) {outputs[TRIGGER_OUTPUT].setVoltage(10.0f,3);}
		else if (hitKey==78 /*n*/) {outputs[TRIGGER_OUTPUT].setVoltage(10.0f,4);}
		else if (hitKey==77 /*m*/) {outputs[TRIGGER_OUTPUT].setVoltage(10.0f,5);}
		
		if (inputs[CLOCK_INPUT].isConnected()==false) {
			stepA++; stepB++; stepC++;
			if (stepA>=48 || stepA<0) {stepA=0;}
			if (stepB>=16 || stepB<0) {stepB=0;}
			if (stepC>=8 || stepC<0) {stepC=0;}
			updateLights();
		}
		
	}

	void updateLights() {
		for (int k=0;k<48;k++) {
			// lights[SEQ_LIGHT+k].setBrightness(paramVal[SEQ_PARAM+k]*0.452);
			lights[SEQ_LIGHT+k].setBrightness(params[SEQ_PARAM+k].getValue()*0.452);
		}
	}


