// Copyright (c) 2023 András Szabó
#include "plugin.hpp"

struct Rubber : Module {

// --------------------------------------------------

	enum ParamId  {
		SELECT_PARAM, DEF_A_PARAM, DEF_B_PARAM, DEF_C_PARAM, DEF_D_PARAM, BUTTON_PARAM, GENERAL_PARAM, PARAMS_LEN};

	enum InputId  {
		A_INPUT, B_INPUT, C_INPUT, D_INPUT, INPUTS_LEN};

	enum OutputId {
		A_OUTPUT, B_OUTPUT, C_OUTPUT, D_OUTPUT, DEBUG_OUTPUT, OUTPUTS_LEN};

	enum LightId  {
		LED_BUTTON_PARAM, LIGHTS_LEN};

	// small assistance to save older values for reference;
	float paramVal[PARAMS_LEN]={0};
	// float inputVolt[INPUTS_LEN]={0};
	// float lightVal[LIGHTS_LEN]={0};

// --------------------------------------------------

	Rubber() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(SELECT_PARAM, 	0.0f, 16.0f, 1.0f, "Select");
		configParam(DEF_A_PARAM, 	-10.0f, 10.0f, 0.0f, "Normalized A input");
		configParam(DEF_B_PARAM, 	-10.0f, 10.0f, 0.0f, "Normalized B input");
		configParam(DEF_C_PARAM, 	-10.0f, 10.0f, 0.0f, "Normalized C input");
		configParam(DEF_D_PARAM, 	-10.0f, 10.0f, 0.0f, "Normalized D input");
		configParam(GENERAL_PARAM, 	0.0f, 1.0f, 0.5f, "Fine tuning");
		configParam(BUTTON_PARAM, 	0.0f, 1.0f, 0.0f, "Button");
		paramQuantities[SELECT_PARAM]->snapEnabled = true;
		paramQuantities[GENERAL_PARAM]->randomizeEnabled = false;
		paramQuantities[GENERAL_PARAM]->description = "This does literally nothing";

		configInput(A_INPUT, "A"); 
		configInput(B_INPUT, "B"); 
		configInput(C_INPUT, "C"); 
		configInput(D_INPUT, "D"); 

		configOutput(A_OUTPUT, "A"); 
		configOutput(B_OUTPUT, "B"); 
		configOutput(C_OUTPUT, "C"); 
		configOutput(D_OUTPUT, "C"); 
		configOutput(DEBUG_OUTPUT, "Debug"); 
	}

// --------------------------------------------------

	int loop=0;     // save some CPU in process()
	const std::string labels[17][11]={
		/* 0 */ {"SUM AVG MIN MAX","Ignored","This does literally nothing",
		"Mono -10v to +10v","Mono -10v to +10v","Mono -10v to +10v","Mono -10v to +10v",
		"SUM","AVG","MIN","MAX"},
		/* 1 */ {"ABS +5 inverted SIN","Ignored","This does literally nothing",	
		"CV to ABS","CV to add +5V","CV to be inv.","CV to SIN",	
		"ABS","+5V added","Inverted","SIN"},
		/* 2 */ {"Quantized random CVs","Reset >2v","This does literally nothing",
		">2V triggers a new CV",">2V triggers a new CV (normalized to A)",">2V triggers a new CV (normalized to A)",">2V triggers a new CV (normalized to A)",
		"0v to 1v","-1v to 1v","0v to 10v","-5v to 5v"},
		/* 3 */ {"Basic quantizer for 4 inputs","Mute","This does literally nothing",
		"Raw CV A","Raw CV B","Raw CV C","Raw CV D",
		"Quantized A","Quantized B","Quantized C","Quantized D"},
		/* 4 */ {"Advanced quantizer for 1 input","Trigger","This does literally nothing",
		"Trigger","Raw CV","Transpose","Optional polyphonic scale",
		"Quantized +1 octave","Quantized CV","Quantized -1 octave","Quantized -2 octave"},
		/* 5 */ {"Merge","Ignored","This does literally nothing", "","","","", "ABCD","BCDA","CDAB","DABC"},		
		/* 6 */ {"Split","Ignored","This does literally nothing",
		"","Ignored","Ignored","Ignored",
		"channel #1","channel #2","channel #3","channel #4"},
		/* 7 */ {"Linear drum pattern","Create new pattern map","This does literally nothing",
		"Reset >2v","Clock >2v","Nr. of steps (default 1.6v=16steps)","<0 redialable pattern, >5v random, 5v= mute, >=4v D, >=3v C, >=2v B, >=1v A",
		"Kick","Snare","Closed hat","Open hat"},
		/* 8 */ {"Layered drum pattern","Create new pattern map","This does literally nothing",
		"Reset >2v","Clock >2v","Nr. of steps (default 1.6v=16steps)","<0 redialable pattern, >5v random, 5v= mute, >=4v D, >=3v C, >=2v B, >=1v A",
		"Kick","Snare","Closed hat","Open hat"},
		/* 9 */ {"Phasor drums (+-5V to hits)","Mute / Unmute","This does literally nothing",
		"Add LFO A","Add LFO B","Add LFO C","Add LFO D",
		"Kick","Snare","Closed hat","Open hat"},
		/* 10 */ {"Looped random CV","Create new pattern map","Raw or quantized (>0.5V)",
		"Reset >2v","Clock >2v","Nr. of steps (default 1.6v=16steps)","<0 redialable random, >=5v random, >=1v random",
		"0v to 1v","-1v to 1v","0v to 10v","-5v to 5v"}, 
		/* 11 */ {"Bernoulli random triggers","Ignored" /* "Manual trigger" */,"This does literally nothing",
		"Define A weight (0-10v)","Trigger >2v","Additional trigger >2v","Additional trigger >2v",
		"As A input requests","Busy","Not so busy","Rarest"},
		/* 12 */ {"Mono to poly","Ignored","This does literally nothing",
		"Note CV","Gate >2v","Note CV","Gate >2v",
		"Polyphonic notes","Polyphonic gates","Polyphonic notes","Polyphonic gates"},
/* 		{"Poly stepper","Sequencer","This does literally nothing",	
		"Reset >2v","Clock >2v","Polyphonic input","Ignored",
		"Currently selected channel","Previous #1","Previous #2","Previous #3"},
 */		
		{"Disabled","ERR","This does literally nothing",	"","","","",	"","","",""},
		{"Disabled","ERR","This does literally nothing",	"","","","",	"","","",""},
		{"Disabled","ERR","This does literally nothing",	"","","","",	"","","",""},
		{"Disabled","ERR","This does literally nothing",	"","","","",	"","","",""}
		};
	float lastIn[4]={0,0,0,0};
	float lastOut[4]={0,0,0,0};

	double sampleCnt=0;
	int selectorVal=0;
	float buttonVal=0;
	bool buttonON=false;
	bool buttonToggle=false;
	const int A=0;
	const int B=1;
	const int C=2;
	const int D=3;
	
	// it's needed for mode #7 and some repeating patterns in general
	int steps=16;
	int active=0;
	float pattern[256]={
		0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 
		0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 
		0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 
		0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 
		0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 
		0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 
		0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 
		0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 
		0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 
		0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 
		0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 
		0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 
		0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 
		0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 
		0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 
		0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0}; 

/* Opera Aria :)
Pop stílusú dobritmus:
1,3,3,2,1,3,3,2,1,3,3,2,1,3,3,2.0,
Rock stílusú dobritmus:
2,2,1,2,2,1,2,2,1,2,2,1,2,2,1,2.0,
Techno stílusú dobritmus:
3,4,3,4,3,4,3,4,3,4,3,4,3,4,3,4.0,
Drum and Bass (dnb) stílusú dobritmus:
4,2,3,2,4,2,3,2,4,2,3,2,4,2,3,2.0,
Jungle stílusú dobritmus:
1,3,1,1,2,3,2,2,3,4,3,3,2,4,2,2.0,
Afrikai ritmusok inspirálta dobritmus:
3,4,1,3,4,1,3,2,4,1,3,4,1,3,2,4.0,
Funk stílusú dobritmus:
2,1,2,1,2,1,2,1,3,2,3,2,3,2,3,2.0,
Reggae stílusú dobritmus:
1,2,3,2,1,2,3,2,1,2,3,2,1,2,3,2.0,
Hip-hop stílusú dobritmus:
4,4,3,3,2,2,1,1,4,4,3,3,2,2,1,1.0,
House stílusú dobritmus:
3,3,4,4,3,3,4,4,3,3,4,4,3,3,4,4.0,
Jazz stílusú dobritmus:
2,2,1,2,2,1,2,2,3,3,2,3,3,2,3,3.0,
R&B stílusú dobritmus:
1,4,2,4,1,4,2,4,1,4,2,4,1,4,2,4.0,
Rock dobritmus 1:
2,2,1,2,2,1,2,2,1,2,2,1,2,2,1,2.0,
Rock dobritmus 2:
3,3,2,3,3,2,3,3,2,3,3,2,3,3,2,3.0,
Rock dobritmus 3:
4,4,3,4,4,3,4,4,3,4,4,3,4,4,3,4.0,
Rock dobritmus 4:
1,2,3,4,1,2,3,4,1,2,3,4,1,2,3,4.0,
 */

 	int  getMode() {return params[SELECT_PARAM].getValue();}
	void  setMode(int xm) {params[SELECT_PARAM].setValue(xm);}
	int alternate[6]={3,2,1,0,5,4};

	void process(const ProcessArgs& args) override {

		if (loop--<=0) {
			loop=9017;
			// save some more CPU

			if (selectorVal!=params[SELECT_PARAM].getValue()) {
				selectorVal=params[SELECT_PARAM].getValue();			
				paramQuantities[SELECT_PARAM]->description = labels[selectorVal][0];
				paramQuantities[BUTTON_PARAM]->description = labels[selectorVal][1];
				paramQuantities[GENERAL_PARAM]->description = labels[selectorVal][2];
				getInputInfo(A_INPUT)->description = labels[selectorVal][3];
				getInputInfo(B_INPUT)->description = labels[selectorVal][4];
				getInputInfo(C_INPUT)->description = labels[selectorVal][5];
				getInputInfo(D_INPUT)->description = labels[selectorVal][6];
				getOutputInfo(A_OUTPUT)->description = labels[selectorVal][7];
				getOutputInfo(B_OUTPUT)->description = labels[selectorVal][8];
				getOutputInfo(C_OUTPUT)->description = labels[selectorVal][9];
				getOutputInfo(D_OUTPUT)->description = labels[selectorVal][10];
				lastOut[A]=0;
				lastOut[B]=0;
				lastOut[C]=0;
				lastOut[D]=0;
				outputs[A_OUTPUT].channels=1;
				outputs[B_OUTPUT].channels=1;
				outputs[C_OUTPUT].channels=1;
				outputs[D_OUTPUT].channels=1;
			}
		}

		buttonON=false;
		if (buttonVal==0 && params[BUTTON_PARAM].getValue()>0) {
			buttonON=true;
			buttonToggle=!buttonToggle;
		};
		buttonVal=params[BUTTON_PARAM].getValue();
		params[BUTTON_PARAM].setValue(0);

		switch (selectorVal) {
		case 0: {
			// 0 SUM		AVG		MIN		MAX
			int allIns=0;
			lastOut[C] = +99;
			lastOut[D] = -99;			
			for (int s=0;s<4;s++) { 
				if (inputs[A_INPUT+s].isConnected()==true) {
					allIns++;
					lastIn[A+s]=inputs[A_INPUT+s].getVoltage();
					if (lastOut[C]>lastIn[A+s]) {lastOut[C]=lastIn[A+s];}
					if (lastOut[D]<lastIn[A+s]) {lastOut[D]=lastIn[A+s];}
				}
				else {lastIn[A+s]=0;}	// itt ez nem tuzdom jó-e
			}
			lastOut[A] = lastIn[A] + lastIn[B] + lastIn[C] + lastIn[D]; 
			lastOut[B] = lastOut[A]/allIns;
			for (int s=0;s<4;s++) {
				outputs[A_OUTPUT+s].channels=1;
				outputs[A_OUTPUT+s].setVoltage(lastOut[A+s]);
			}
			break;
		}
		case 1: {
			// ABS +5 inverted SIN
			lastIn[A]=inputs[A_INPUT].getVoltage();
			if (inputs[B_INPUT].isConnected()==false) {lastIn[B]=lastIn[A];}
			else {lastIn[B]=inputs[B_INPUT].getVoltage();}
			if (inputs[C_INPUT].isConnected()==false) {lastIn[C]=lastIn[A];}
			else {lastIn[C]=inputs[C_INPUT].getVoltage();}
			if (inputs[D_INPUT].isConnected()==false) {lastIn[D]=lastIn[A];}
			else {lastIn[D]=inputs[D_INPUT].getVoltage();}
			// if (outputs[A_OUTPUT].isConnected()) {outputs[A_OUTPUT].setVoltage(abs(lastIn[A]));}
			// if (outputs[B_OUTPUT].isConnected()) {outputs[B_OUTPUT].setVoltage(lastIn[B]+5);}
			// if (outputs[C_OUTPUT].isConnected()) {outputs[C_OUTPUT].setVoltage(lastIn[C]*-1);}
			// if (outputs[D_OUTPUT].isConnected()) {outputs[D_OUTPUT].setVoltage(sin(lastIn[D]));}
			outputs[A_OUTPUT].setVoltage(abs(lastIn[A]));
			outputs[B_OUTPUT].setVoltage(lastIn[B]+5);
			outputs[C_OUTPUT].setVoltage(lastIn[C]*-1);
			if (outputs[D_OUTPUT].isConnected()) {outputs[D_OUTPUT].setVoltage(sin(lastIn[D]));}
			break;
		}
		case 2: {
			// 2 random quantized value
			for (int s=0;s<4;s++) {
				// lastOut[A+s]=0;
				if (buttonON==true) {lastOut[A+s]=rack::random::uniform();}
				else if (inputs[A_INPUT+s].isConnected()==true) {
					if (lastIn[A+s]<=2 && inputs[A_INPUT+s].getVoltage()>2) {
						lastIn[A+s]=10;
						lastOut[A+s]=rack::random::uniform();
					}
					else if (inputs[A_INPUT+s].getVoltage()<=2) {lastIn[A+s]=0;}					
				}
				else if (inputs[A_INPUT].isConnected()==true) {
					if (lastIn[A+s]<=2 && inputs[A_INPUT].getVoltage()>2) {
						lastIn[A+s]=10;
						lastOut[A+s]=rack::random::uniform();
					}
					else if (inputs[A_INPUT].getVoltage()<=2) {lastIn[A+s]=0;}
				}
				outputs[A_OUTPUT+s].channels=1;
			}
			if (params[GENERAL_PARAM].getValue()>0.5) {
				paramQuantities[GENERAL_PARAM]->description = "Quantized";
				lastOut[A]=floor(lastOut[A]*12+0.5)/12;
				lastOut[B]=floor(lastOut[B]*12+0.5)/12;
				lastOut[C]=floor(lastOut[C]*12+0.5)/12;
				lastOut[D]=floor(lastOut[D]*12+0.5)/12;
			}
			else {paramQuantities[GENERAL_PARAM]->description = "Unquantized";}
			outputs[A].setVoltage(lastOut[A]);
			outputs[B].setVoltage(lastOut[B]*2-1);
			outputs[C].setVoltage(lastOut[C]*10);
			outputs[D].setVoltage(lastOut[D]*10-5);
			break;
		}
		case 3: {
			// 3 superbasic 12 semitone quantizer
			if (buttonToggle==true) {paramQuantities[BUTTON_PARAM]->description="Muted"; break;}
			else {paramQuantities[BUTTON_PARAM]->description="Unmuted";}
			for (int s=0;s<4;s++) {
				outputs[A_OUTPUT+s].channels=1;
				lastIn[A+s]=inputs[A_INPUT+s].getVoltage();
				lastOut[A+s]=floor(lastIn[A+s]*12)/12;
				outputs[A_OUTPUT+s].setVoltage(lastOut[A+s]);
				}
			break;
		}
		case 4: {
			// 4 a feature rich 12 semitone quantizer
			if (buttonON==true || inputs[A_INPUT].isConnected()==false || (inputs[A_INPUT].getVoltage()>2.0f && lastIn[A]<=2.0f)) {
				lastIn[B]=inputs[B_INPUT].getVoltage();
				/* lastIn[C]=inputs[C_INPUT].getVoltage(); */
				if (inputs[C_INPUT].isConnected()==true) {lastIn[C]=inputs[C_INPUT].getVoltage();}
				else {lastIn[C]=params[DEF_C_PARAM].getValue();}

				if (inputs[D_INPUT].isConnected()==true) {
					float closestVal=999;
					int closestPos=0;
					for (int c=0;c<inputs[D_INPUT].channels;c++) {
						lastIn[D]=inputs[D_INPUT].getVoltage(c);
						if (closestVal>abs(lastIn[B]-lastIn[D])) {
							closestPos=c;
							closestVal=abs(lastIn[B]-lastIn[D]);
						}
					}
					lastOut[B]=inputs[D_INPUT].getVoltage(closestPos);	
				}
				else {lastOut[B]=floor(lastIn[B]*12)/12+1;}
				lastOut[B]+=floor(lastIn[C]*12)/12;
				lastOut[A]=lastOut[B]+1;
				lastOut[C]=lastOut[B]-1;
				lastOut[D]=lastOut[B]-2;
				for (int s=0;s<4;s++) {
					outputs[A_OUTPUT+s].channels=1;
					outputs[A_OUTPUT+s].setVoltage(lastOut[A+s]);
				}
			}
			lastIn[A]=inputs[A_INPUT].getVoltage();
			break;
		}
		case 5: {
			// Merge
			steps=4;	// OK this variable was created for something else but it's good enough here :)
			if (inputs[B].isConnected()==false) {steps=1;}
			else if (inputs[C].isConnected()==false) {steps=2;}
			else if (inputs[D].isConnected()==false) {steps=3;}
			for (int s=0;s<4;s++) {
				lastIn[A+s]=inputs[A_INPUT+s].getVoltage();
				outputs[A_OUTPUT+s].channels=steps;
			}
			outputs[A_OUTPUT].setVoltage(lastIn[A],0);
			outputs[A_OUTPUT].setVoltage(lastIn[B],1);
			outputs[A_OUTPUT].setVoltage(lastIn[C],2);
			outputs[A_OUTPUT].setVoltage(lastIn[D],3);
			outputs[B_OUTPUT].setVoltage(lastIn[B],0);
			outputs[B_OUTPUT].setVoltage(lastIn[C],1);
			outputs[B_OUTPUT].setVoltage(lastIn[D],2);
			outputs[B_OUTPUT].setVoltage(lastIn[A],3);
			outputs[C_OUTPUT].setVoltage(lastIn[C],0);
			outputs[C_OUTPUT].setVoltage(lastIn[D],1);
			outputs[C_OUTPUT].setVoltage(lastIn[A],2);
			outputs[C_OUTPUT].setVoltage(lastIn[B],3);
			outputs[D_OUTPUT].setVoltage(lastIn[D],0);
			outputs[D_OUTPUT].setVoltage(lastIn[A],1);
			outputs[D_OUTPUT].setVoltage(lastIn[B],2);
			outputs[D_OUTPUT].setVoltage(lastIn[C],3);
			break;
		}
		case 6: {
			// Split
			for (int s=0;s<4;s++) {
				outputs[A_OUTPUT+s].channels=1;
				outputs[A_OUTPUT+s].setVoltage(inputs[A_INPUT].getVoltage(s));
			}
			break;
		}
		case 7: {
			// Linear drum pattern
			// new pattern needed?
			if (buttonON==true) {
				for (int i=0; i<256; i++) {pattern[i]= rand() % 4 + 1;}
				// for (int i=0; i<256; i++) {
					// if (rand() % 16 == 0 ) {pattern[i]=1;}
					// if (rand() % 8 == 4 ) {pattern[i]=2;}
				// }
			}
			// step length received?
			if (inputs[C_INPUT].isConnected()==true) {
				// steps=clamp(floor(inputs[C_INPUT].getVoltage()*10+0.5),0,99);}
				steps=rack::math::clamp(floor(inputs[C_INPUT].getVoltage()*10+0.5),0,99);}
			else { /* steps=16; */
				steps=rack::math::clamp(floor(params[DEF_C_PARAM].getValue()*10+0.5),0,99);
				if (steps<1) {steps=16;}
			}
			// reset received?
			if (lastIn[A]<=0 && inputs[A_INPUT].getVoltage()>2.0f) {
				active=-1;
			}
			lastIn[A]=inputs[A_INPUT].getVoltage();
			// new clock needed?
			if (lastIn[B]<=0 && inputs[B_INPUT].getVoltage()>2.0f) {
				if (steps<=0) {	// at 0 pattern length do randomly
					lastOut[A]=0; lastOut[B]=0; lastOut[C]=0; lastOut[D]=0;
					lastOut[A+(rand() % 4)]=10;
				}
				else {
					active++;
					if (active>=steps) {active=0;}
					lastOut[A]=0; lastOut[B]=0; lastOut[C]=0; lastOut[D]=0;

					// this is just some fancy solution to avoid feeding constant values :)
					// feel free to delete it along with the "if selval==8" in the menu
					if (inputs[D_INPUT].isConnected()==false) {lastIn[D]=params[DEF_D_PARAM].getValue();} 
					else {lastIn[D]=inputs[D_INPUT].getVoltage();}

					/* lastIn[D]=inputs[D_INPUT].getVoltage(); */
					if (lastIn[D]==0.0f)  {lastOut[A-1+(int)pattern[active]]=10;}
					else if (lastIn[D]>=9.0f) {lastOut[A+(rand() % 4)]=10;} // trigger to random output
					else if (lastIn[D]>=8.0f) {lastOut[A+(rand() % 4)]=10;} // trigger to random output
					else if (lastIn[D]>=7.0f) {lastOut[alternate[A-1+(int)pattern[active]]]=10;}
					else if (lastIn[D]>=6.0f) {
						if (lastIn[D]>=6.0f + rack::random::uniform()) {lastOut[A+(rand() % 4)]=10;}
						else {lastOut[A-1+(int)pattern[active]]=10;}
					}	
					else if (lastIn[D]>=5.0f) {/* mute all */}
					else if (lastIn[D]>=4.0f) {lastOut[D]=10;}
					else if (lastIn[D]>=3.0f) {lastOut[C]=10;}
					else if (lastIn[D]>=2.0f) {lastOut[B]=10;}
					else if (lastIn[D]>=1.0f) {lastOut[A]=10;}
					else if (lastIn[D]<0.0f) {
						lastOut[A-1+(int)pattern[active+rack::math::clamp(floor(-10*(lastIn[D])),0,256-1-steps)]]=10;
					}
					else {lastOut[A-1+(int)pattern[active]]=10;}
				}
			}
			else if (lastIn[B]>0 && inputs[B_INPUT].getVoltage()<=2.0f) {
				lastOut[A]=0; lastOut[B]=0; lastOut[C]=0; lastOut[D]=0;
				// if (inputs[D_INPUT].getVoltage()>2.0f) {lastOut[A+(rand() % 4)]=10;}
			}
			lastIn[B]=inputs[B_INPUT].getVoltage();
			// send the triggers
			for (int s=0;s<4;s++) {
				outputs[A_OUTPUT+s].channels=1;
				outputs[A_OUTPUT+s].setVoltage(lastOut[A+s]);
			}	
			break;
		}
		
		case 8: {
			// Layered drum pattern
			// new pattern needed?
			if (buttonON==true) {for (int i=0;i<256;i++) {pattern[i]=rand()%6+1;}}
			// step length received?
			if (inputs[C_INPUT].isConnected()==true) {steps=rack::math::clamp(floor(inputs[C_INPUT].getVoltage()*10+0.5),0,99);}
			else { /* steps=16; */
				steps=rack::math::clamp(floor(params[DEF_C_PARAM].getValue()*10+0.5),0,99);
				if (steps<1) {steps=16;}
			}
							
			/* else {steps=rack::math::clamp(floor(inputs[DEF_C_PARAM].getVoltage()*10+0.5),0,99);} */
			// reset received?
			if (lastIn[A]<=0 && inputs[A_INPUT].getVoltage()>2.0f) {active=-1;}
			lastIn[A]=inputs[A_INPUT].getVoltage();
			// new clock needed?
			if (lastIn[B]<=0 && inputs[B_INPUT].getVoltage()>2.0f) {
				if (steps<=0) {	// at 0 pattern length do randomly
					std::fill(lastOut, lastOut+4, 0);
					lastOut[A+(rand() % 4)]=10;}
				else {
					active++;
					if (active>=steps) {active=0;}
					std::fill(lastOut, lastOut+4, 0);
					// std::fill(lastOut, lastOut+4, {0,0,0,0});

					// this is just some fancy solution to avoid feeding constant values :)
					// feel free to delete it along with the "if selval==8" in the menu
					if (inputs[D_INPUT].isConnected()==false) {lastIn[D]=params[DEF_D_PARAM].getValue();} 
					else {lastIn[D]=inputs[D_INPUT].getVoltage();}
					/* lastIn[D]=inputs[D_INPUT].getVoltage(); */
					
					float layerVar[7][4]={{0,0,0,0},{10,10,0,0},{0,10,10,0},{0,0,10,10},{10,0,0,10},{10,0,10,0},{0,10,0,10}};
					// float layerVar[5][4]={{0,0,0,0},{10,0,0,0},{0,10,0,0},{0,0,10,0},{0,0,0,10}};
					
					// let's get to work
					if (lastIn[D]==0.0f)  {
						std::copy(
							layerVar[(int)pattern[active]], 
							layerVar[(int)pattern[active]] + 4, 
							lastOut);
					}
					else if (lastIn[D]>=9.0f) {
						lastOut[A+(rand() % 4)]=10;
						lastOut[A+(rand() % 4)]=10;} // trigger to random output
					else if (lastIn[D]>=8.0f) {
						lastOut[A+(rand() % 4)]=10;
						lastOut[A+(rand() % 4)]=10;} // trigger to random output
					else if (lastIn[D]>=7.0f) {lastOut[alternate[A-1+(int)pattern[active]]]=10;}
					else if (lastIn[D]>=6.0f) {
						if (lastIn[D]>=6.0f + rack::random::uniform()) {
							lastOut[A+(rand() % 4)]=10;
							lastOut[A+(rand() % 4)]=10;
							}
						else {
							// lastOut[A-1+(int)pattern[active]]=10;
							std::copy(
								layerVar[(int)pattern[active]], 
								layerVar[(int)pattern[active]] + 4, 
								lastOut);
							}
						}	
					else if (lastIn[D]>=5.0f) {/* mute all */}
					else if (lastIn[D]>=4.0f) {lastOut[D]=10; lastOut[A+(rand() % 4)]=10;}
					else if (lastIn[D]>=3.0f) {lastOut[C]=10; lastOut[A+(rand() % 4)]=10;}
					else if (lastIn[D]>=2.0f) {lastOut[B]=10; lastOut[A+(rand() % 4)]=10;}
					else if (lastIn[D]>=1.0f) {lastOut[A]=10; lastOut[A+(rand() % 4)]=10;}
					else if (lastIn[D]<0.0f) {
						// lastOut[A-1+(int)pattern[active+rack::math::clamp(floor(-10*(lastIn[D])),0,256-1-steps)]]=10;
						std::copy(
							layerVar[(int)pattern[active+rack::math::clamp(floor(-10*(lastIn[D])),0,255-steps)]], 
							layerVar[(int)pattern[active+rack::math::clamp(floor(-10*(lastIn[D])),0,255-steps)]]+4, 
							lastOut);
					}
					else {lastOut[A-1+(int)pattern[active]]=10;}
				}
			}
			else if (lastIn[B]>0 && inputs[B_INPUT].getVoltage()<=2.0f) {
					std::fill(lastOut, lastOut+4, 0);
			}
			lastIn[B]=inputs[B_INPUT].getVoltage();
			// send the triggers
			for (int s=0;s<4;s++) {
				outputs[A_OUTPUT+s].channels=1;
				outputs[A_OUTPUT+s].setVoltage(lastOut[A+s]);
			}	
			break;
		}
		
		case 9: {
			// Phasor drums
			if (buttonToggle==true) {break;}
			int allIn=0;
			for (int s=0;s<4;s++) {
				lastIn[A+s]=inputs[A_INPUT+s].getVoltage();
				allIn=allIn+lastIn[A+s];
				}
			if (allIn>7.5) {active=3;}
			else if (7.5>=allIn and allIn>5.0) {active=2;}
			else if (5.0>=allIn and allIn>2.5) {active=1;}
			else if (2.5>=allIn and allIn>0.0) {active=0;}
			else if (0.0>=allIn and allIn>-2.5) {active=3;}
			else if (-2.5>=allIn and allIn>-5.0) {active=2;}
			else if (-5.0>=allIn and allIn>-7.5) {active=1;}
			else {active=0;}
			for (int s=0;s<4;s++) {
				outputs[A_OUTPUT+s].channels=1;
				outputs[A_OUTPUT+s].setVoltage((active==s)?10:0);
				}
			break;
		}

		case 10: {
			// Looped random CV
			// new pattern needed?
			if (buttonON==true) {
				for (int i=0; i<256; i++) {pattern[i]=rack::random::uniform();}
			}
			// step length received?
			if (inputs[C_INPUT].isConnected()==true) {steps=rack::math::clamp(floor(inputs[C_INPUT].getVoltage()*10+0.5),0,99);}
			else { /* steps=16; */
				steps=rack::math::clamp(floor(params[DEF_C_PARAM].getValue()*10+0.5),0,99);
				if (steps<1) {steps=16;}
			}
			// reset received?
			if (lastIn[A]<=0 && inputs[A_INPUT].getVoltage()>2.0f) {
				active=-1;
			}
			lastIn[A]=inputs[A_INPUT].getVoltage();
			// new clock needed?
			if (lastIn[B]<=0 && inputs[B_INPUT].getVoltage()>2.0f) {
				if (steps<=0) {	// at 0 pattern length do randomly
					lastOut[A]=pattern[rand() % (256-steps)]; 
					// lastOut[B]=lastOut[A]*2-1; 
					// lastOut[C]=lastOut[A]*10; 
					// lastOut[D]=lastOut[A]*10-5;
				}
				else {
					active++;
					if (active>=steps) {active=0;}
					lastOut[A]=0; 
					// this is just some fancy solution to avoid feeding constant values :)
					// feel free to delete it along with the "if selval==8" in the menu
					if (inputs[D_INPUT].isConnected()==false) {lastIn[D]=params[DEF_D_PARAM].getValue();} 
					else {lastIn[D]=inputs[D_INPUT].getVoltage();}
					/* lastIn[D]=inputs[D_INPUT].getVoltage(); */
					if (lastIn[D]==0.0f)  {lastOut[A]=pattern[active];}
					else if (lastIn[D]>=8.0f) {lastOut[A]=pattern[rand() % (255-steps)];}
					else if (lastIn[D]>=7.0f) {lastOut[A]=pattern[active + (rand() % steps)];}
					else if (lastIn[D]>=6.0f) {
						if (lastIn[D]>=6.0f + rack::random::uniform()) {lastOut[A]=pattern[rand() % (255-steps)];}
						else {lastOut[A]=pattern[active];}
						}	
					else if (lastIn[D]>=5.0f) {/* mute all */}
					else if (lastIn[D]>=4.0f) {lastOut[A]=pattern[active+4];}
					else if (lastIn[D]>=3.0f) {lastOut[A]=pattern[active+3];}
					else if (lastIn[D]>=2.0f) {lastOut[A]=pattern[active+2];}
					else if (lastIn[D]>=1.0f) {lastOut[A]=pattern[active+1];}
					else if (lastIn[D]<0.0f) {
						// lastOut[A-1+(int)pattern[active+rack::math::clamp(floor(-10*(lastIn[D])),0,256-1-steps)]]=10;
						lastOut[A]=pattern[active+rack::math::clamp(floor(-10*(lastIn[D])),0,256-1-steps)];
						}
					else {lastOut[A]=pattern[active];}
				}
			}
			// else if (lastIn[B]>0 && inputs[B_INPUT].getVoltage()<=2.0f) {
			// }
			lastIn[B]=inputs[B_INPUT].getVoltage();
			// send the CVs
			lastOut[B]=lastOut[A]*2-1; 
			lastOut[C]=lastOut[A]*10; 			// originally to have 0V to 10V
			lastOut[D]=lastOut[A]*10-5;
			paramQuantities[GENERAL_PARAM]->description = "Unquantized";
			if (params[GENERAL_PARAM].getValue()>0.5) {
				paramQuantities[GENERAL_PARAM]->description = "Quantized";
				lastOut[A]=floor(lastOut[A]*12)/12;
				lastOut[B]=floor(lastOut[B]*12)/12;
				lastOut[C]=floor(lastOut[C]*12)/12;
				lastOut[D]=floor(lastOut[D]*12)/12;
			}
			for (int s=0;s<4;s++) {
				outputs[A_OUTPUT+s].channels=1;
				outputs[A_OUTPUT+s].setVoltage(lastOut[A+s]);
			}	
			break;
		}

		case 11: {
			// Bernoulli random triggers
		
			// A weight received?
			if (inputs[A_INPUT].isConnected()==true) {lastIn[A]=abs(inputs[A_INPUT].getVoltage()/10);} 
			else {
				lastIn[A]=abs(params[DEF_A_PARAM].getValue()/10);
				if (lastIn[A]==0) {lastIn[A]=params[GENERAL_PARAM].getValue();} // this is the screw :) delete me
			}
			if ((lastIn[B]<=0 && inputs[B_INPUT].getVoltage()>2.0f)
				|| (lastIn[C]<=0 && inputs[C_INPUT].getVoltage()>2.0f) 
				|| (lastIn[D]<=0 && inputs[D_INPUT].getVoltage()>2.0f)) {
				float rndcalc = rack::random::uniform();  // Egy 0 és 1 közötti véletlen érték
				if (rndcalc <= lastIn[A]) {outputs[A_OUTPUT].setVoltage(10);}
				else if (rndcalc <= lastIn[A] + 0.5f * (1 - lastIn[A])) {outputs[B_OUTPUT].setVoltage(10);}
				else if (rndcalc <= lastIn[A] + 0.5f * (1 - lastIn[A]) + 0.3f * (1 - lastIn[A])) {outputs[C_OUTPUT].setVoltage(10);}
				else {outputs[D_OUTPUT].setVoltage(10);}			
			}
			else if ((lastIn[B]>0 && inputs[B_INPUT].getVoltage()<=2.0f)
				|| (lastIn[C]>0 && inputs[C_INPUT].getVoltage()<=2.0f) 
				|| (lastIn[D]>0 && inputs[D_INPUT].getVoltage()<=2.0f) 
				/* || (buttonON==false) */) {
				outputs[A_OUTPUT].setVoltage(0);
				outputs[B_OUTPUT].setVoltage(0);
				outputs[C_OUTPUT].setVoltage(0);
				outputs[D_OUTPUT].setVoltage(0);
			}
			lastIn[B]=inputs[B_INPUT].getVoltage();
			lastIn[C]=inputs[C_INPUT].getVoltage();
			lastIn[D]=inputs[D_INPUT].getVoltage();
			break;
		}

		case 12: {
			// mono to poly
			if (lastIn[A]!=inputs[A_INPUT].getVoltage()) {
				lastOut[A] = int(lastOut[A] + 1) % 4; // this is to identify the output channel
				lastIn[A]=inputs[A_INPUT].getVoltage();
			}
			if (lastIn[C]!=inputs[C_INPUT].getVoltage()) {
				lastOut[C] = int(lastOut[C] + 1) % 4; // this is to identify the output channel
				lastIn[C]=inputs[C_INPUT].getVoltage();
			}
			if (lastIn[B]<2 && inputs[B_INPUT].getVoltage()>=2) {
				lastOut[B] = int(lastOut[B] + 1) % 4; // this is to identify the output channel
				lastIn[B]=10;
			}
			else if (lastIn[B]>=2 && inputs[B_INPUT].getVoltage()<2) {
				lastIn[B]=0;		
			}
			if (lastIn[D]<2 && inputs[D_INPUT].getVoltage()>=2) {
				lastOut[D] = int(lastOut[D] + 1) % 4; // this is to identify the output channel
				lastIn[D]=10;
			}
			else if (lastIn[D]>=2 && inputs[D_INPUT].getVoltage()<2) {
				lastIn[D]=0;		
			}
			for (int s=0;s<4;s++) {
				outputs[A_OUTPUT+s].channels=4;
				outputs[A_OUTPUT+s].setVoltage(lastIn[s],lastOut[s]);
			}				
			break;
		}
		
		default: {
			for (int s=0;s<4;s++) {
				outputs[A_OUTPUT+s].channels=1;
				lastIn[s]=inputs[A_INPUT+s].getVoltage();
				lastOut[s]=lastIn[s]*params[GENERAL_PARAM].getValue();
				outputs[A_OUTPUT+s].setVoltage(lastOut[s]);
			}				
		}
		}

	}

	#include "Rubber/Rubber_json.hpp"

};

// --------------------------------------------------

/* struct CustomPortWidget : app::SvgPort {
	CustomPortWidget() {
		setSvg(Svg::load(asset::system("res/ComponentLibrary/PJ301M.svg")));
	}
	Rubber* module;
    void appendContextMenu(ui::Menu* menu) override {
        // Call the base class method
        PortWidget::appendContextMenu(menu);
        // Add your custom menu item
		menu->addChild(new MenuSeparator);
        menu->addChild(createMenuLabel("Port widget test"));
		menu->addChild(createMenuLabel("Experimental default input values"));
		auto defSliderC = new rack::ui::Slider;
			defSliderC->quantity = module->paramQuantities[module->DEF_C_PARAM];
			defSliderC->box.size.x = 125;
			menu->addChild(defSliderC);

    }
}; */

struct TinykKnob : RoundKnob {
	TinykKnob () {
		setSvg(Svg::load(asset::system("res/ComponentLibrary/ScrewSilver.svg")));
		bg->setSvg(Svg::load(asset::system("res/ComponentLibrary/BlackSilver.svg")));
	}
};


struct RubberWidget : ModuleWidget {

	Rubber* module;
	#include "share/widgetwizard.hpp"   // addChild abbreviations

	RubberWidget(Rubber* module) {

		this->module = module;
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/Rubber.svg")));

		// addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		
		childKnob(Rubber::SELECT_PARAM, 0, HP*1.f, HP*2.0f);
		childPushbutton(Rubber::BUTTON_PARAM, Rubber::LED_BUTTON_PARAM, HP*1.f, HP*4.0f);
		addParam(createParamCentered<TinykKnob>(mm2px(Vec(HP*1.0f, HP*6.0f)), module, Rubber::GENERAL_PARAM));

		addParam(createParamCentered<TinykKnob>(mm2px(Vec(HP*31.0f, HP*8.5f)), module, Rubber::DEF_A_PARAM));
		addParam(createParamCentered<TinykKnob>(mm2px(Vec(HP*31.0f, HP*10.5f)), module, Rubber::DEF_B_PARAM));
		addParam(createParamCentered<TinykKnob>(mm2px(Vec(HP*31.0f, HP*12.5f)), module, Rubber::DEF_C_PARAM));
		addParam(createParamCentered<TinykKnob>(mm2px(Vec(HP*31.0f, HP*14.5f)), module, Rubber::DEF_D_PARAM));
		
		// addInput(createInputCentered<CustomPortWidget>(mm2px(Vec(HP*1.f, HP*7.5f)), module, Rubber::A_INPUT));
		
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(HP*1.f, HP*8.0f)), module, Rubber::A_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(HP*1.f, HP*10.0f)), module, Rubber::B_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(HP*1.f, HP*12.0f)), module, Rubber::C_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(HP*1.f, HP*14.0f)), module, Rubber::D_INPUT));
		
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(HP*1.f, HP*17.f)), module, Rubber::A_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(HP*1.f, HP*19.f)), module, Rubber::B_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(HP*1.f, HP*21.f)), module, Rubber::C_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(HP*1.f, HP*23.f)), module, Rubber::D_OUTPUT));
		
		// childInput(Rubber::A_INPUT, HP*1.f, HP*7.5f);
		// childInput(Rubber::B_INPUT, HP*1.f, HP*9.5f);
		// childInput(Rubber::C_INPUT, HP*1.f, HP*11.5f);
		// childInput(Rubber::D_INPUT, HP*1.f, HP*13.5f);

		// childOutput(Rubber::A_OUTPUT, HP*1.f, HP*16.f);
		// childOutput(Rubber::B_OUTPUT, HP*1.f, HP*18.f);
		// childOutput(Rubber::C_OUTPUT, HP*1.f, HP*20.f);
		// childOutput(Rubber::D_OUTPUT, HP*1.f, HP*22.f);
		// childOutput(Rubber::DEBUG_OUTPUT, HP*32.f, HP*23.23f);

		// childLabel(HP*1,HP*1, "DEV", 12);

	}


	// menu for basic quantization
	void appendContextMenu(Menu* menu) override {
		Rubber* module = dynamic_cast<Rubber*>(this->module);
		assert(module);

		// menu to verview and select modes
		menu->addChild(new MenuSeparator);
		menu->addChild(createIndexSubmenuItem("Mode knob", {
			module->labels[0][0], module->labels[1][0],
			module->labels[2][0], module->labels[3][0],
			module->labels[4][0], module->labels[5][0],
			module->labels[6][0], module->labels[7][0],
			module->labels[8][0], module->labels[9][0],
			module->labels[10][0], module->labels[11][0],
			module->labels[12][0], module->labels[13][0],
			module->labels[14][0], module->labels[15][0],
			module->labels[16][0]}, 
			[=]() {return module->getMode();},
			[=](int mode) {module->setMode(mode);}			
		));

		// special menu item is defined here
		auto addSlider = [&](int paramIndex) {
			auto slider = new rack::ui::Slider;
			slider->quantity = module->paramQuantities[paramIndex];
			slider->box.size.x = 125;
			menu->addChild(slider);
		};
				
		// conditional elements in the menu
		switch (module->selectorVal) {
			case 4: {
				menu->addChild(new MenuSeparator);
				menu->addChild(createMenuLabel("Experimental default input values"));
				addSlider(module->DEF_C_PARAM);
				break;
			}
			case 7: 
			case 8: 
			case 10: {
				menu->addChild(new MenuSeparator);
				menu->addChild(createMenuLabel("Experimental default input values"));
				addSlider(module->DEF_C_PARAM);
				addSlider(module->DEF_D_PARAM);
				break;
			}
			// case 0: 
			case 11: {
				menu->addChild(new MenuSeparator);
				menu->addChild(createMenuLabel("Experimental default input values"));
				addSlider(module->DEF_A_PARAM);
				break;
			}
		}


 	}

	// #include "Rubber/Rubber_keys.hpp"

};

Model* modelRubber = createModel<Rubber, RubberWidget>("Rubber");
