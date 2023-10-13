// Copyright (c) 2023 András Szabó
#include "plugin.hpp"

struct Rubber : Module {

// --------------------------------------------------

	enum ParamId  {
		SELECT_PARAM, BUTTON_PARAM, PARAMS_LEN};

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
		configParam(BUTTON_PARAM, 	0.0f, 1.0f, 0.0f, "Button");
		paramQuantities[SELECT_PARAM]->snapEnabled = true;

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
	const std::string labels[17][10]={
		{"SUM AVG MIN MAX","Ignored",
		"Mono -10v to +10v","Mono -10v to +10v","Mono -10v to +10v","Mono -10v to +10v",
		"SUM","AVG","MIN","MAX"},
		{"S&H random CVs","Reset >2v",
		">2V triggers a new CV",">2V triggers a new CV (normalized to A)",">2V triggers a new CV (normalized to A)",">2V triggers a new CV (normalized to A)",
		"Random 1v to 2v","Random 0v to +1v","Random -1v to 0v","Random -2v to -1v"},
		{"Quantized random CVs","Reset >2v",
		">2V triggers a new CV",">2V triggers a new CV (normalized to A)",">2V triggers a new CV (normalized to A)",">2V triggers a new CV (normalized to A)",
		"Quantized random 1v to 2v","Quantized random 0v to +1v","Quantized random -1v to 0v","Random -2v to -1v"},
		{"Basic quantizer for 4 inputs","Mute",
		"Raw CV A","Raw CV B","Raw CV C","Raw CV D",
		"Quantized A","Quantized B","Quantized C","Quantized D"},
		{"Advanced quantizer for 1 input","Trigger",
		"Trigger","Raw CV","Transpose","Optional polyphonic scale",
		"Quantized +1 octave","Quantized CV","Quantized -1 octave","Quantized -2 octave"},
		{"Merge","Ignored",	"","","","",	"ABCD","BCDA","CDAB","DABC"},
		{"Split","Ignored",	"","Ignored","Ignored","Ignored",	"channel #1","channel #2","channel #3","channel #4"},
		{"Drum pattern","Create new pattern map",
		"Reset >2v","Clock >2v","Nr. of steps (default 1.6v=16steps)","<0 redialable random, >=5v random, >=4v D, >=3v C, >=2v B, >=1v A",
		"Kick","Snare","Closed hat","Open hat"},
		{"Looped random CV","Create new pattern map",
		"Reset >2v","Clock >2v","Nr. of steps (default 1.6v=16steps)","<0 redialable random, >=5v random, >=1v random",
		"0v to 1v","-1v to 1v","0v to 10v","-5v to 5v"},
		{"Phasor drums","Mute",
		"LFO A","LFO B","LFO C","LFO D",
		"Kick","Snare","Closed hat","Open hat"},
		{"Disabled","ERR",	"","","","",	"","","",""},
		{"Disabled","ERR",	"","","","",	"","","",""},
		{"Disabled","ERR",	"","","","",	"","","",""},
		{"Disabled","ERR",	"","","","",	"","","",""},
		{"Disabled","ERR",	"","","","",	"","","",""},
		{"Disabled","ERR",	"","","","",	"","","",""},
		{"ABS +5 inverted SIN","Ignored",	"","","","",	"ABS","+5V added","Inverted","SIN"}
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
	int defVolt=0;
	int defVolts[4]={0,0,0,0};	// placeholder for future development: store the default voltage of inputs set in the context menu
	
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
	int alternate[4]={3,2,1,0};

	void process(const ProcessArgs& args) override {

		if (loop--<=0) {
			loop=9017;
			// save some more CPU

			selectorVal=params[SELECT_PARAM].getValue();
		
			paramQuantities[SELECT_PARAM]->description = labels[selectorVal][0];
			paramQuantities[BUTTON_PARAM]->description = labels[selectorVal][1];
			getInputInfo(A_INPUT)->description = labels[selectorVal][2];
			getInputInfo(B_INPUT)->description = labels[selectorVal][3];
			getInputInfo(C_INPUT)->description = labels[selectorVal][4];
			getInputInfo(D_INPUT)->description = labels[selectorVal][5];
			getOutputInfo(A_OUTPUT)->description = labels[selectorVal][6];
			getOutputInfo(B_OUTPUT)->description = labels[selectorVal][7];
			getOutputInfo(C_OUTPUT)->description = labels[selectorVal][8];
			getOutputInfo(D_OUTPUT)->description = labels[selectorVal][9];
			
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
			lastOut[C] = +999;
			lastOut[D] = -999;		
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
		case 1: 
		case 2: {
			// 1 random value
			// 2 random quantized value
			for (int s=0;s<4;s++) {
				outputs[A_OUTPUT+s].channels=1;
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
				// in case of 2 quantize
				if (selectorVal==2) {lastOut[A+s]=floor(lastOut[A+s]*12+0.5)/12;}
				// else if (selectorVal==1) {lastOut[A+s]=7*lastOut[A+s];}
				outputs[A_OUTPUT+s].setVoltage(lastOut[A+s] -s +1);
			}
			break;
		}
		case 3: {
			// 3 superbasic 12 semitone quantizer
			if (buttonToggle==true) {break;}
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
				lastIn[C]=inputs[C_INPUT].getVoltage();
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
			// 5 merge
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
			// 6 split
			for (int s=0;s<4;s++) {
				outputs[A_OUTPUT+s].channels=1;
				outputs[A_OUTPUT+s].setVoltage(inputs[A_INPUT].getVoltage(s));
			}
			break;
		}
		case 7: {
			// 7 drum pattern
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
				steps=clamp(floor(inputs[C_INPUT].getVoltage()*10+0.5),0,99);
			} else {steps=16;}
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
					lastIn[D]=inputs[D_INPUT].getVoltage();
					if (lastIn[D]==0.0f)  {lastOut[A-1+(int)pattern[active]]=10;}
					else if (lastIn[D]>=9.0f) {lastOut[A+(rand() % 4)]=10;} // trigger to random output
					else if (lastIn[D]>=8.0f) {lastOut[A+(rand() % 4)]=10;} // trigger to random output
					else if (lastIn[D]>=7.0f) {lastOut[A+(rand() % 4)]=10;
/* 						int xCent=(int)pattern[active+clamp(floor(-10*(lastIn[D])),0,256-1-steps)];
						int xLeft=(int)pattern[active+clamp(floor(-10*(lastIn[D]))-1,0,256-1-steps)];
						int xRight=(int)pattern[active+clamp(floor(-10*(lastIn[D]))+1,0,256-1-steps)];
						for (int i=1;i<4;i++) {
							if (i!=xCent && i!=xLeft && i!=xRight) {lastOut[A+i]=10;break;}
						}
 */					}	
					else if (lastIn[D]>=6.0f) {lastOut[alternate[A-1+(int)pattern[active]]]=10;}
					else if (lastIn[D]>=5.0f) {/* mute all */}
					else if (lastIn[D]>=4.0f) {lastOut[D]=10;}
					else if (lastIn[D]>=3.0f) {lastOut[C]=10;}
					else if (lastIn[D]>=2.0f) {lastOut[B]=10;}
					else if (lastIn[D]>=1.0f) {lastOut[A]=10;}
					else if (lastIn[D]<0.0f) {
						lastOut[A-1+(int)pattern[active+clamp(floor(-10*(lastIn[D])),0,256-1-steps)]]=10;
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
			// 8 drum pattern
			// new pattern needed?
			if (buttonON==true) {
				for (int i=0; i<256; i++) {pattern[i]=rack::random::uniform();}
			}
			// step length received?
			if (inputs[C_INPUT].isConnected()==true) {
				steps=clamp(floor(inputs[C_INPUT].getVoltage()*10+0.5),0,99);
			} else {steps=16;}
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
					lastIn[D]=inputs[D_INPUT].getVoltage();
					if (lastIn[D]==0.0f)  {lastOut[A]=pattern[active];}
					else if (lastIn[D]>=7.0f) {lastOut[A]=pattern[rand() % (256-1-steps)];}
					else if (lastIn[D]>=6.0f) {lastOut[A]=pattern[active + (rand() % steps)];}
					else if (lastIn[D]>=5.0f) {/* mute all */}
					else if (lastIn[D]>=4.0f) {lastOut[A]=pattern[active+4];}
					else if (lastIn[D]>=3.0f) {lastOut[A]=pattern[active+3];}
					else if (lastIn[D]>=2.0f) {lastOut[A]=pattern[active+2];}
					else if (lastIn[D]>=1.0f) {lastOut[A]=pattern[active+1];}
					else if (lastIn[D]<0.0f) {
						// lastOut[A-1+(int)pattern[active+clamp(floor(-10*(lastIn[D])),0,256-1-steps)]]=10;
						lastOut[A]=pattern[active+clamp(floor(-10*(lastIn[D])),0,256-1-steps)];
						}
					else {lastOut[A]=pattern[active];}
				}
			}
			// else if (lastIn[B]>0 && inputs[B_INPUT].getVoltage()<=2.0f) {
			// }
			lastIn[B]=inputs[B_INPUT].getVoltage();
			// send the CVs
			lastOut[B]=lastOut[A]*2-1; 
			lastOut[C]=lastOut[A]*10; 
			lastOut[D]=lastOut[A]*10-5;
			for (int s=0;s<4;s++) {
				outputs[A_OUTPUT+s].channels=1;
				outputs[A_OUTPUT+s].setVoltage(lastOut[A+s]);
			}	
			break;
		}
		case 9: {
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
		case 16: {
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
		default: {
			for (int s=0;s<4;s++) {
				outputs[A_OUTPUT+s].channels=1;
				outputs[A_OUTPUT+s].setVoltage(s);
			}				
		}
		}

	}

	#include "Rubber/Rubber_json.hpp"

};

// --------------------------------------------------

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
		
		childKnob(Rubber::SELECT_PARAM, 0, HP*1, HP*2.5);
		childPushbutton(Rubber::BUTTON_PARAM, Rubber::LED_BUTTON_PARAM, HP*1, HP*5);
		
		childInput(Rubber::A_INPUT, HP*1, HP*7.5);
		childInput(Rubber::B_INPUT, HP*1, HP*9.5);
		childInput(Rubber::C_INPUT, HP*1, HP*11.5);
		childInput(Rubber::D_INPUT, HP*1, HP*13.5);
		
		childOutput(Rubber::A_OUTPUT, HP*1, HP*16.0);
		childOutput(Rubber::B_OUTPUT, HP*1, HP*18.0);
		childOutput(Rubber::C_OUTPUT, HP*1, HP*20.0);
		childOutput(Rubber::D_OUTPUT, HP*1, HP*22.0);
		childOutput(Rubber::DEBUG_OUTPUT, HP*22, HP*23.23);

		// childLabel(HP*1,HP*1, "DEV", 12);

	}

	// #include "Rubber/Rubber_menu.hpp"

	// menu for basic quantization
	void appendContextMenu(Menu* menu) override {
		Rubber* module = dynamic_cast<Rubber*>(this->module);
		assert(module);
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
		
/* 	
		menu->addChild(createIndexPtrSubmenuItem("Default A voltage", {"0","1","1.6","3.2","6.4","10"}, &module->defVolts[0]));
		menu->addChild(createIndexPtrSubmenuItem("Default B voltage", {"0","1","1.6","3.2","6.4","10"}, &module->defVolts[1]));
		menu->addChild(createIndexPtrSubmenuItem("Default C voltage", {"0","1","1.6","3.2","6.4","10"}, &module->defVolts[2]));
		menu->addChild(createIndexPtrSubmenuItem("Default D voltage", {"0","1","1.6","3.2","6.4","10"}, &module->defVolts[3]));
 */
		// menu->addChild(createIndexPtrSubmenuItem("Default C voltage", {"0.4","0.8","1.2","1.6","3.2","6.4","9.6","12.8"}, &module->defVolts[2]));

		// menu->addChild(createIndexPtrSubmenuItem("Default A voltage", {"0v","1v","2v","3v","4v","5v"}, &module->defVolt));


// menu->addChild(new LLFOSliderMenuItem<SampleQuantity>(m, "Output sampling"));

 	}

	// #include "Rubber/Rubber_keys.hpp"

};

Model* modelRubber = createModel<Rubber, RubberWidget>("Rubber");
