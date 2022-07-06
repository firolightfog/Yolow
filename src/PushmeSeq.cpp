// Copyright (c) 2022 András Szabó
#include "plugin.hpp"

struct PushmeSeq : Module {

// --------------------------------------------------

	enum ParamId    {
		SEQA1_PARAM, SEQA2_PARAM, SEQA3_PARAM, SEQA4_PARAM, SEQA5_PARAM, 
		SEQA6_PARAM, SEQA7_PARAM, SEQA8_PARAM, SEQA9_PARAM, SEQA10_PARAM, 
		SEQA11_PARAM, SEQA12_PARAM, SEQA13_PARAM, SEQA14_PARAM, SEQA15_PARAM, 
		SEQA16_PARAM, SEQA17_PARAM, SEQA18_PARAM, SEQA19_PARAM, SEQA20_PARAM, 
		SEQA21_PARAM, SEQA22_PARAM, SEQA23_PARAM, SEQA24_PARAM, SEQA25_PARAM, 
		SEQA26_PARAM, SEQA27_PARAM, SEQA28_PARAM, SEQA29_PARAM, SEQA30_PARAM, 
		SEQA31_PARAM, SEQA32_PARAM, SEQA33_PARAM, SEQA34_PARAM, SEQA35_PARAM, 
		SEQA36_PARAM, SEQA37_PARAM, SEQA38_PARAM, SEQA39_PARAM, SEQA40_PARAM, 
		SEQA41_PARAM, SEQA42_PARAM, SEQA43_PARAM, SEQA44_PARAM, SEQA45_PARAM, 
		SEQA46_PARAM, SEQA47_PARAM, SEQA48_PARAM, INDEXSEQMODE, PARAMS_LEN};

	enum InputId    {
		CLOCK_INPUT, RESET_INPUT, INPUTS_LEN};

	enum OutputId    {
		EOC_OUTPUT, TRIGGER_OUTPUT, OUTPUTS_LEN};

	enum LightId    {
		LED_SEQA1_PARAM, LED_SEQA2_PARAM, LED_SEQA3_PARAM, LED_SEQA4_PARAM, LED_SEQA5_PARAM, 
		LED_SEQA6_PARAM, LED_SEQA7_PARAM, LED_SEQA8_PARAM, LED_SEQA9_PARAM, LED_SEQA10_PARAM, 
		LED_SEQA11_PARAM, LED_SEQA12_PARAM, LED_SEQA13_PARAM, LED_SEQA14_PARAM, LED_SEQA15_PARAM, 
		LED_SEQA16_PARAM, LED_SEQA17_PARAM, LED_SEQA18_PARAM, LED_SEQA19_PARAM, LED_SEQA20_PARAM, 
		LED_SEQA21_PARAM, LED_SEQA22_PARAM, LED_SEQA23_PARAM, LED_SEQA24_PARAM, LED_SEQA25_PARAM, 
		LED_SEQA26_PARAM, LED_SEQA27_PARAM, LED_SEQA28_PARAM, LED_SEQA29_PARAM, LED_SEQA30_PARAM, 
		LED_SEQA31_PARAM, LED_SEQA32_PARAM, LED_SEQA33_PARAM, LED_SEQA34_PARAM, LED_SEQA35_PARAM, 
		LED_SEQA36_PARAM, LED_SEQA37_PARAM, LED_SEQA38_PARAM, LED_SEQA39_PARAM, LED_SEQA40_PARAM, 
		LED_SEQA41_PARAM, LED_SEQA42_PARAM, LED_SEQA43_PARAM, LED_SEQA44_PARAM, LED_SEQA45_PARAM, 
		LED_SEQA46_PARAM, LED_SEQA47_PARAM, LED_SEQA48_PARAM, 
		LIGHTS_LEN};

	// small assistance to save older values for reference;
	float paramVal[PARAMS_LEN]={0};
	float inputVolt[INPUTS_LEN]={0};
	float lightVal[LIGHTS_LEN]={0};

// --------------------------------------------------

	PushmeSeq() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(SEQA1_PARAM, 	0.0f, 1.0f, 1.0f, "SeqA1");
		configParam(SEQA2_PARAM, 	0.0f, 1.0f, 0.0f, "SeqA2");
		configParam(SEQA3_PARAM, 	0.0f, 1.0f, 0.0f, "SeqA3");
		configParam(SEQA4_PARAM, 	0.0f, 1.0f, 0.0f, "SeqA4");
		configParam(SEQA5_PARAM, 	0.0f, 1.0f, 0.0f, "SeqA5");
		configParam(SEQA6_PARAM, 	0.0f, 1.0f, 0.0f, "SeqA6");
		configParam(SEQA7_PARAM, 	0.0f, 1.0f, 0.0f, "SeqA7");
		configParam(SEQA8_PARAM, 	0.0f, 1.0f, 0.0f, "SeqA8");
		configParam(SEQA9_PARAM, 	0.0f, 1.0f, 0.0f, "SeqA9");
		configParam(SEQA10_PARAM, 	0.0f, 1.0f, 0.0f, "SeqA10");
		configParam(SEQA11_PARAM, 	0.0f, 1.0f, 0.0f, "SeqA11");
		configParam(SEQA12_PARAM, 	0.0f, 1.0f, 0.0f, "SeqA12");
		configParam(SEQA13_PARAM, 	0.0f, 1.0f, 0.0f, "SeqA13");
		configParam(SEQA14_PARAM, 	0.0f, 1.0f, 0.0f, "SeqA14");
		configParam(SEQA15_PARAM, 	0.0f, 1.0f, 0.0f, "SeqA15");
		configParam(SEQA16_PARAM, 	0.0f, 1.0f, 0.0f, "SeqA16");
		configParam(SEQA17_PARAM, 	0.0f, 1.0f, 0.0f, "SeqA17");
		configParam(SEQA18_PARAM, 	0.0f, 1.0f, 0.0f, "SeqA18");
		configParam(SEQA19_PARAM, 	0.0f, 1.0f, 0.0f, "SeqA19");
		configParam(SEQA20_PARAM, 	0.0f, 1.0f, 0.0f, "SeqA20");
		configParam(SEQA21_PARAM, 	0.0f, 1.0f, 0.0f, "SeqA21");
		configParam(SEQA22_PARAM, 	0.0f, 1.0f, 0.0f, "SeqA22");
		configParam(SEQA23_PARAM, 	0.0f, 1.0f, 0.0f, "SeqA23");
		configParam(SEQA24_PARAM, 	0.0f, 1.0f, 0.0f, "SeqA24");
		configParam(SEQA25_PARAM, 	0.0f, 1.0f, 0.0f, "SeqA25");
		configParam(SEQA26_PARAM, 	0.0f, 1.0f, 0.0f, "SeqA26");
		configParam(SEQA27_PARAM, 	0.0f, 1.0f, 0.0f, "SeqA27");
		configParam(SEQA28_PARAM, 	0.0f, 1.0f, 0.0f, "SeqA28");
		configParam(SEQA29_PARAM, 	0.0f, 1.0f, 0.0f, "SeqA29");
		configParam(SEQA30_PARAM, 	0.0f, 1.0f, 0.0f, "SeqA30");
		configParam(SEQA31_PARAM, 	0.0f, 1.0f, 0.0f, "SeqA31");
		configParam(SEQA32_PARAM, 	0.0f, 1.0f, 0.0f, "SeqA32");
		configParam(SEQA33_PARAM, 	0.0f, 1.0f, 0.0f, "SeqA33");
		configParam(SEQA34_PARAM, 	0.0f, 1.0f, 0.0f, "SeqA34");
		configParam(SEQA35_PARAM, 	0.0f, 1.0f, 0.0f, "SeqA35");
		configParam(SEQA36_PARAM, 	0.0f, 1.0f, 0.0f, "SeqA36");
		configParam(SEQA37_PARAM, 	0.0f, 1.0f, 0.0f, "SeqA37");
		configParam(SEQA38_PARAM, 	0.0f, 1.0f, 0.0f, "SeqA38");
		configParam(SEQA39_PARAM, 	0.0f, 1.0f, 0.0f, "SeqA39");
		configParam(SEQA40_PARAM, 	0.0f, 1.0f, 0.0f, "SeqA40");
		configParam(SEQA41_PARAM, 	0.0f, 1.0f, 0.0f, "SeqA41");
		configParam(SEQA42_PARAM, 	0.0f, 1.0f, 0.0f, "SeqA42");
		configParam(SEQA43_PARAM, 	0.0f, 1.0f, 0.0f, "SeqA43");
		configParam(SEQA44_PARAM, 	0.0f, 1.0f, 0.0f, "SeqA44");
		configParam(SEQA45_PARAM, 	0.0f, 1.0f, 0.0f, "SeqA45");
		configParam(SEQA46_PARAM, 	0.0f, 1.0f, 0.0f, "SeqA46");
		configParam(SEQA47_PARAM, 	0.0f, 1.0f, 0.0f, "SeqA47");
		configParam(SEQA48_PARAM, 	0.0f, 1.0f, 0.0f, "SeqA48");
			
		configParam(INDEXSEQMODE, 	0.0f, 4.0f, 1.0f, "Sequence mode");
		paramQuantities[INDEXSEQMODE]->randomizeEnabled = false;
		paramQuantities[INDEXSEQMODE]->snapEnabled = true;

		configInput(CLOCK_INPUT, "Clock"); 
		configInput(RESET_INPUT, "Reset"); 

		configOutput(EOC_OUTPUT, "EOC"); 
		configOutput(TRIGGER_OUTPUT, "Trigger"); 
	}

// --------------------------------------------------

void keyKnob(int rndSection) {
	for (int b=8*rndSection;b<8*(rndSection+1);b++) {
		params[SEQA1_PARAM+b].setValue(rand() % 2);
		lights[LED_SEQA1_PARAM+b].setBrightness(params[SEQA1_PARAM+b].getValue());
	}	
}
void invKnob() {
	for (int b=0;b<48;b++) {
		params[SEQA1_PARAM+b].setValue(abs(params[SEQA1_PARAM+b].getValue()-1));
		lights[LED_SEQA1_PARAM+b].setBrightness(params[SEQA1_PARAM+b].getValue());
	}
}

// --------------------------------------------------

	int loop=0;     // save some CPU in process()
	float voltEOC=0.0f;
	float voltTRG=0.0f;
	
	// for managing the RESET and CLOCK signals
	float newReset=0.0f;
	float oldReset=0.0f;
	float newClock=0.0f;
	float oldClock=0.0f;
	bool hitClock=false;
	
	// ideintifying the step we're at
	int stepA=0;	// 1x48
	int stepB=0;	// 3x16
	int stepC=0;	// 6x8
	
	int indexSeqMode=1;
	
	void process(const ProcessArgs& args) override {

		if (loop--<=0) {
			loop=9000;
			// save some more CPU
			indexSeqMode=params[INDEXSEQMODE].getValue();
			if (indexSeqMode==0) {outputs[TRIGGER_OUTPUT].channels=1;}	// 1x48
			else if (indexSeqMode==1) {outputs[TRIGGER_OUTPUT].channels=3;} // 3x16
			else if (indexSeqMode==2) {outputs[TRIGGER_OUTPUT].channels=6;} // 6x8
			else if (indexSeqMode==3) {outputs[TRIGGER_OUTPUT].channels=4;} // 2x16+2x8
			else if (indexSeqMode==4) {outputs[TRIGGER_OUTPUT].channels=1;} // CV
		}
		
		// let's see the reset signal
		newReset=inputs[RESET_INPUT].getVoltage();
		if (newReset>0.2f && oldReset<=0.2f) {
			stepA=-1;stepB=-1;stepC=-1;			
		}
		// else if (newReset>0.2 && oldReset>0.2) {}
		// else if (newReset<=0.2 && oldReset>0.2) {}
		// else if (newReset<=0.2 && oldReset<=0.2) {}
		oldReset=newReset;

		// let's see the clock signal
		hitClock=false;
		newClock=inputs[CLOCK_INPUT].getVoltage();
		if (newClock>0.2f && oldClock<=0.2f) {
			stepA++; stepB++; stepC++;
			if (stepA>=48 || stepA<0) {stepA=0;}
			if (stepB>=16 || stepB<0) {stepB=0;}
			if (stepC>=8 || stepC<0) {stepC=0;}			
			hitClock=true;
		}
		// else if (newClock>0.2 && oldClock>0.2) {}
		// else if (newClock<=0.2 && oldClock<=0.2) {}
		else if (newClock<=0.2 && oldClock>0.2) {
			if (indexSeqMode==0) {outputs[TRIGGER_OUTPUT].setVoltage(0,0);}	// 1x48
			else if (indexSeqMode==1) {	for (int c=0;c<3;c++) {outputs[TRIGGER_OUTPUT].setVoltage(0,c);}} // 3x16
			else if (indexSeqMode==2) {	for (int c=0;c<6;c++) {outputs[TRIGGER_OUTPUT].setVoltage(0,c);}} // 6x8
			else if (indexSeqMode==3) {	for (int c=0;c<4;c++) {outputs[TRIGGER_OUTPUT].setVoltage(0,c);}} // 2x16+2x8
			// outputs[EOC_OUTPUT].setVoltage(0,0);
		}
		oldClock=newClock;
		
		// if there's a "new clock pulse" we have a lot to do
		if (hitClock==true) {

			// fix all lights
			for (int k=0;k<48;k++) {
				lights[LED_SEQA1_PARAM+k].setBrightness(params[SEQA1_PARAM+k].getValue());
			}
			
			// shape the output and 
			if (indexSeqMode==0) {	// 1x48
				lights[LED_SEQA1_PARAM+stepA].setBrightness(0.325);
				outputs[TRIGGER_OUTPUT].setVoltage(params[SEQA1_PARAM+stepA].getValue()*10.0f,0);
				// if (stepA==0) {outputs[EOC_OUTPUT].setVoltage(10,0);}
			}
			else if (indexSeqMode==1) {	// 3x16
				lights[LED_SEQA1_PARAM+stepB].setBrightness(0.325);
				lights[LED_SEQA17_PARAM+stepB].setBrightness(0.325);
				lights[LED_SEQA33_PARAM+stepB].setBrightness(0.325);
				outputs[TRIGGER_OUTPUT].setVoltage(params[SEQA1_PARAM+stepB].getValue()*9.9f,0);
				outputs[TRIGGER_OUTPUT].setVoltage(params[SEQA17_PARAM+stepB].getValue()*9.9f,1);
				outputs[TRIGGER_OUTPUT].setVoltage(params[SEQA33_PARAM+stepB].getValue()*9.9f,2);
				// if (stepB==0) {outputs[EOC_OUTPUT].setVoltage(10,0);}
			}
			else if (indexSeqMode==2) {	// 6x8
				lights[LED_SEQA1_PARAM+stepC].setBrightness(0.325);
				lights[LED_SEQA9_PARAM+stepC].setBrightness(0.325);
				lights[LED_SEQA17_PARAM+stepC].setBrightness(0.325);
				lights[LED_SEQA25_PARAM+stepC].setBrightness(0.325);
				lights[LED_SEQA33_PARAM+stepC].setBrightness(0.325);
				lights[LED_SEQA41_PARAM+stepC].setBrightness(0.325);
				outputs[TRIGGER_OUTPUT].setVoltage(params[SEQA1_PARAM+stepC].getValue()*9.1f,0);
				outputs[TRIGGER_OUTPUT].setVoltage(params[SEQA9_PARAM+stepC].getValue()*9.2f,1);
				outputs[TRIGGER_OUTPUT].setVoltage(params[SEQA17_PARAM+stepC].getValue()*9.3f,2);
				outputs[TRIGGER_OUTPUT].setVoltage(params[SEQA25_PARAM+stepC].getValue()*9.4f,3);
				outputs[TRIGGER_OUTPUT].setVoltage(params[SEQA33_PARAM+stepC].getValue()*9.5f,4);
				outputs[TRIGGER_OUTPUT].setVoltage(params[SEQA41_PARAM+stepC].getValue()*9.6f,5);
				// if (stepC==0) {outputs[EOC_OUTPUT].setVoltage(10,0);}
			}
			else if (indexSeqMode==3) {	// 2x16+2x8
				lights[LED_SEQA1_PARAM+stepB].setBrightness(0.325);
				lights[LED_SEQA17_PARAM+stepB].setBrightness(0.325);
				lights[LED_SEQA33_PARAM+stepC].setBrightness(0.325);
				lights[LED_SEQA41_PARAM+stepC].setBrightness(0.325);
				outputs[TRIGGER_OUTPUT].setVoltage(params[SEQA1_PARAM+stepB].getValue()*9.1f,0);
				outputs[TRIGGER_OUTPUT].setVoltage(params[SEQA17_PARAM+stepB].getValue()*9.2f,1);
				outputs[TRIGGER_OUTPUT].setVoltage(params[SEQA33_PARAM+stepC].getValue()*9.3f,2);
				outputs[TRIGGER_OUTPUT].setVoltage(params[SEQA41_PARAM+stepC].getValue()*9.4f,3);
				// if (stepC==0) {outputs[EOC_OUTPUT].setVoltage(10,0);}		
			}
			else if (indexSeqMode==4) {	// CV
				// this is a special case
				stepA=(rand() % 48);
				lights[LED_SEQA1_PARAM+stepA].setBrightness(0.325);
				outputs[TRIGGER_OUTPUT].setVoltage(params[SEQA1_PARAM+stepA].getValue()*10.0f,0);
			}
		}
		
	}

// --------------------------------------------------

	// spaceholder for JSON

// --------------------------------------------------

};

// --------------------------------------------------

struct PushmeSeqWidget : ModuleWidget {

	PushmeSeq* module;
	#include "share/widgetwizard.hpp"   // addChild abbreviations

	PushmeSeqWidget(PushmeSeq* module) {

		this->module = module;
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/PushmeSeq.svg")));

		// addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		
		childPushbutton(PushmeSeq::SEQA1_PARAM, PushmeSeq::LED_SEQA1_PARAM, HP*0.8, HP*2-HP*0.5);
		childPushbutton(PushmeSeq::SEQA2_PARAM, PushmeSeq::LED_SEQA2_PARAM, HP*0.8, HP*3.25-HP*0.5);
		childPushbutton(PushmeSeq::SEQA3_PARAM, PushmeSeq::LED_SEQA3_PARAM, HP*0.8, HP*4.5-HP*0.5);
		childPushbutton(PushmeSeq::SEQA4_PARAM, PushmeSeq::LED_SEQA4_PARAM, HP*0.8, HP*5.75-HP*0.5);
		childPushbutton(PushmeSeq::SEQA5_PARAM, PushmeSeq::LED_SEQA5_PARAM, HP*0.8, HP*7-HP*0.5);
		childPushbutton(PushmeSeq::SEQA6_PARAM, PushmeSeq::LED_SEQA6_PARAM, HP*0.8, HP*8.25-HP*0.5);
		childPushbutton(PushmeSeq::SEQA7_PARAM, PushmeSeq::LED_SEQA7_PARAM, HP*0.8, HP*9.5-HP*0.5);
		childPushbutton(PushmeSeq::SEQA8_PARAM, PushmeSeq::LED_SEQA8_PARAM, HP*0.8, HP*10.75-HP*0.5);
		childPushbutton(PushmeSeq::SEQA9_PARAM, PushmeSeq::LED_SEQA9_PARAM, HP*0.8, HP*12-HP*0.5);
		childPushbutton(PushmeSeq::SEQA10_PARAM, PushmeSeq::LED_SEQA10_PARAM, HP*0.8, HP*13.25-HP*0.5);
		childPushbutton(PushmeSeq::SEQA11_PARAM, PushmeSeq::LED_SEQA11_PARAM, HP*0.8, HP*14.5-HP*0.5);
		childPushbutton(PushmeSeq::SEQA12_PARAM, PushmeSeq::LED_SEQA12_PARAM, HP*0.8, HP*15.75-HP*0.5);
		childPushbutton(PushmeSeq::SEQA13_PARAM, PushmeSeq::LED_SEQA13_PARAM, HP*0.8, HP*17-HP*0.5);
		childPushbutton(PushmeSeq::SEQA14_PARAM, PushmeSeq::LED_SEQA14_PARAM, HP*0.8, HP*18.25-HP*0.5);
		childPushbutton(PushmeSeq::SEQA15_PARAM, PushmeSeq::LED_SEQA15_PARAM, HP*0.8, HP*19.5-HP*0.5);
		childPushbutton(PushmeSeq::SEQA16_PARAM, PushmeSeq::LED_SEQA16_PARAM, HP*0.8, HP*20.75-HP*0.5);
		childPushbutton(PushmeSeq::SEQA17_PARAM, PushmeSeq::LED_SEQA17_PARAM, HP*2, HP*2-HP*0.5);
		childPushbutton(PushmeSeq::SEQA18_PARAM, PushmeSeq::LED_SEQA18_PARAM, HP*2, HP*3.25-HP*0.5);
		childPushbutton(PushmeSeq::SEQA19_PARAM, PushmeSeq::LED_SEQA19_PARAM, HP*2, HP*4.5-HP*0.5);
		childPushbutton(PushmeSeq::SEQA20_PARAM, PushmeSeq::LED_SEQA20_PARAM, HP*2, HP*5.75-HP*0.5);
		childPushbutton(PushmeSeq::SEQA21_PARAM, PushmeSeq::LED_SEQA21_PARAM, HP*2, HP*7-HP*0.5);
		childPushbutton(PushmeSeq::SEQA22_PARAM, PushmeSeq::LED_SEQA22_PARAM, HP*2, HP*8.25-HP*0.5);
		childPushbutton(PushmeSeq::SEQA23_PARAM, PushmeSeq::LED_SEQA23_PARAM, HP*2, HP*9.5-HP*0.5);
		childPushbutton(PushmeSeq::SEQA24_PARAM, PushmeSeq::LED_SEQA24_PARAM, HP*2, HP*10.75-HP*0.5);
		childPushbutton(PushmeSeq::SEQA25_PARAM, PushmeSeq::LED_SEQA25_PARAM, HP*2, HP*12-HP*0.5);
		childPushbutton(PushmeSeq::SEQA26_PARAM, PushmeSeq::LED_SEQA26_PARAM, HP*2, HP*13.25-HP*0.5);
		childPushbutton(PushmeSeq::SEQA27_PARAM, PushmeSeq::LED_SEQA27_PARAM, HP*2, HP*14.5-HP*0.5);
		childPushbutton(PushmeSeq::SEQA28_PARAM, PushmeSeq::LED_SEQA28_PARAM, HP*2, HP*15.75-HP*0.5);
		childPushbutton(PushmeSeq::SEQA29_PARAM, PushmeSeq::LED_SEQA29_PARAM, HP*2, HP*17-HP*0.5);
		childPushbutton(PushmeSeq::SEQA30_PARAM, PushmeSeq::LED_SEQA30_PARAM, HP*2, HP*18.25-HP*0.5);
		childPushbutton(PushmeSeq::SEQA31_PARAM, PushmeSeq::LED_SEQA31_PARAM, HP*2, HP*19.5-HP*0.5);
		childPushbutton(PushmeSeq::SEQA32_PARAM, PushmeSeq::LED_SEQA32_PARAM, HP*2, HP*20.75-HP*0.5);
		childPushbutton(PushmeSeq::SEQA33_PARAM, PushmeSeq::LED_SEQA33_PARAM, HP*3.2, HP*2-HP*0.5);
		childPushbutton(PushmeSeq::SEQA34_PARAM, PushmeSeq::LED_SEQA34_PARAM, HP*3.2, HP*3.25-HP*0.5);
		childPushbutton(PushmeSeq::SEQA35_PARAM, PushmeSeq::LED_SEQA35_PARAM, HP*3.2, HP*4.5-HP*0.5);
		childPushbutton(PushmeSeq::SEQA36_PARAM, PushmeSeq::LED_SEQA36_PARAM, HP*3.2, HP*5.75-HP*0.5);
		childPushbutton(PushmeSeq::SEQA37_PARAM, PushmeSeq::LED_SEQA37_PARAM, HP*3.2, HP*7-HP*0.5);
		childPushbutton(PushmeSeq::SEQA38_PARAM, PushmeSeq::LED_SEQA38_PARAM, HP*3.2, HP*8.25-HP*0.5);
		childPushbutton(PushmeSeq::SEQA39_PARAM, PushmeSeq::LED_SEQA39_PARAM, HP*3.2, HP*9.5-HP*0.5);
		childPushbutton(PushmeSeq::SEQA40_PARAM, PushmeSeq::LED_SEQA40_PARAM, HP*3.2, HP*10.75-HP*0.5);
		childPushbutton(PushmeSeq::SEQA41_PARAM, PushmeSeq::LED_SEQA41_PARAM, HP*3.2, HP*12-HP*0.5);
		childPushbutton(PushmeSeq::SEQA42_PARAM, PushmeSeq::LED_SEQA42_PARAM, HP*3.2, HP*13.25-HP*0.5);
		childPushbutton(PushmeSeq::SEQA43_PARAM, PushmeSeq::LED_SEQA43_PARAM, HP*3.2, HP*14.5-HP*0.5);
		childPushbutton(PushmeSeq::SEQA44_PARAM, PushmeSeq::LED_SEQA44_PARAM, HP*3.2, HP*15.75-HP*0.5);
		childPushbutton(PushmeSeq::SEQA45_PARAM, PushmeSeq::LED_SEQA45_PARAM, HP*3.2, HP*17-HP*0.5);
		childPushbutton(PushmeSeq::SEQA46_PARAM, PushmeSeq::LED_SEQA46_PARAM, HP*3.2, HP*18.25-HP*0.5);
		childPushbutton(PushmeSeq::SEQA47_PARAM, PushmeSeq::LED_SEQA47_PARAM, HP*3.2, HP*19.5-HP*0.5);
		childPushbutton(PushmeSeq::SEQA48_PARAM, PushmeSeq::LED_SEQA48_PARAM, HP*3.2, HP*20.75-HP*0.5);
		
		childInput(PushmeSeq::CLOCK_INPUT, HP*1, HP*22.25-HP*0.5);
		childKnob(PushmeSeq::INDEXSEQMODE, 0, HP*3, HP*22.25-HP*0.5);
		childInput(PushmeSeq::RESET_INPUT, HP*1, HP*24-HP*0.5);
		childOutput(PushmeSeq::TRIGGER_OUTPUT, HP*3, HP*24-HP*0.5);

	}

	// shortkey
	void onHoverKey(const event::HoverKey &e) override {
		if (e.key >= GLFW_KEY_1 && e.key <= GLFW_KEY_6) {
			if (e.action == GLFW_PRESS) {
				float key_number = e.key - 49; // 49 is the ascii number for key #1
				module->keyKnob(key_number);
				e.consume(this);
			}
		}
		else if(e.key == GLFW_KEY_I && e.action == GLFW_PRESS) {module->invKnob(); e.consume(this);}
		ModuleWidget::onHoverKey(e);
	}

};

Model* modelPushmeSeq = createModel<PushmeSeq, PushmeSeqWidget>("PushmeSeq");
