// Copyright (c) 2022 András Szabó
#include "plugin.hpp"

const std::string labelSeqMode[7]={"1x48 random","1x48","1x32 + 1x16","3x16","2x16 + 2x8","1x16 + 4x8 (last random)","6x8 (last random)"};

struct PushmeSeq : Module {

	enum ParamId 	{ENUMS(SEQ_PARAM,48), SEQMODE_PARAM, DIV_PARAM, FETCH_SWITCH, PARAMS_LEN};
	enum InputId 	{CLOCK_INPUT, RESET_INPUT, INPUTS_LEN};
	enum OutputId 	{EOC_OUTPUT, TRIGGER_OUTPUT, OUTPUTS_LEN};
	enum LightId 	{ENUMS(SEQ_LIGHT, 48),LIGHTS_LEN};

	// small assistance to save older values for reference;
	float paramVal[PARAMS_LEN]={0};

// --------------------------------------------------

	PushmeSeq() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configInput(CLOCK_INPUT, "Clock"); 
		configInput(RESET_INPUT, "Reset"); 
		configOutput(EOC_OUTPUT, "EOC"); 
		configOutput(TRIGGER_OUTPUT, "Trigger"); 
		configParam(SEQMODE_PARAM, 	0.0f, 6.0f, 3.0f, "Sequence mode");
		paramQuantities[SEQMODE_PARAM]->randomizeEnabled = false;
		paramQuantities[SEQMODE_PARAM]->snapEnabled = true;
		configParam(DIV_PARAM, 	1.0f, 8.0f, 1.0f, "Clock division");
		paramQuantities[DIV_PARAM]->snapEnabled = true;

		// the 48 buttons are defined with a loop
		configParam(SEQ_PARAM,0.0f, 1.0f, 1.0f, "Zone 1 - SeqA1");
		char lx[25];
		for (int b=1;b<48;b++) {
			sprintf(lx,"Zone %d - SeqA%d", (int(b/8)+1), (b+1));
			configParam(SEQ_PARAM+b,0.0f, 1.0f, 0.0f, lx);
		}
	}

	// --------------------------------------------------

	#include "PushmeSeq/PushmeSeq_routines.hpp"

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
	bool offClock=false;
	
	// ideintifying the step we're at
	int stepA=0;	// current step of 48-step-sequencers
	int stepB=0;	// current step of 16-step-sequencers
	int stepC=0;	// current step of 8-step-sequencers
	int stepRand=0;
	bool restartPlease=true;
	
	int indexSeqMode=1;
	int indexPrec=0;
	
	// const std::string labelSeqMode[7]={"1x48 random","1x48","1x32 + 1x16","3x16","2x16 + 2x8","1x16 + 4x8 (last random)","6x8 (last random)"};

	// Ctrl-E bypassing: no sound but the counting stays
	void processBypass(const ProcessArgs& args) override {
		newReset=inputs[RESET_INPUT].getVoltage();
		if (newReset>2.0f && oldReset<=2.0f) {restartPlease=true;}
		oldReset=newReset;
		hitClock=false;
		newClock=inputs[CLOCK_INPUT].getVoltage();
		if (newClock>2.0f && oldClock<=2.0f) {
			hitClock=true; offClock=false;
			if (restartPlease) {stepA=0; stepB=0; stepC=0; restartPlease=false;}
			else {stepA++; stepB++; stepC++;}
			if (stepA>=48 || stepA<0) {stepA=0;}
			if (stepB>=16 || stepB<0) {stepB=0;}
			if (stepC>=8 || stepC<0) {stepC=0;}
		}
		else if (newClock<=2.0f && oldClock>2.0f) {offClock=true;}
		oldClock=newClock;
	}

	void process(const ProcessArgs& args) override {

		if (loop--<=0) {
			loop=9000;
			// save some more CPU
			indexSeqMode=params[SEQMODE_PARAM].getValue();
			// indexSeqMode: 0 (random) 1 (1x48) 2 (1x32+1x16) 3 (3x16) 4 (2x16+2x8) 5 (1x16+4x8) 6 (6x8)
			paramQuantities[SEQMODE_PARAM]->description = labelSeqMode[indexSeqMode];
			if (indexSeqMode==0) {outputs[TRIGGER_OUTPUT].channels=1;}
			else if (indexSeqMode==1) {outputs[TRIGGER_OUTPUT].channels=1;}
			else if (indexSeqMode==2) {outputs[TRIGGER_OUTPUT].channels=2;}
			else if (indexSeqMode==3) {outputs[TRIGGER_OUTPUT].channels=3;}
			else if (indexSeqMode==4) {outputs[TRIGGER_OUTPUT].channels=4;}
			else if (indexSeqMode==5) {outputs[TRIGGER_OUTPUT].channels=5;}
			else if (indexSeqMode==6) {outputs[TRIGGER_OUTPUT].channels=6;}
			for (int i=0;i<PARAMS_LEN;i++) {paramVal[i]=params[i].getValue();}
		}
		
		// let's see the reset signal
		newReset=inputs[RESET_INPUT].getVoltage();
		if (newReset>2.0f && oldReset<=2.0f) {restartPlease=true;}	// after reset, we better start from the beginning
		// else if (newReset>2.0f && oldReset>2.0f) {}
		// else if (newReset<=2.0f && oldReset>2.0f) {}
		// else if (newReset<=2.0f && oldReset<=2.0f) {}
		oldReset=newReset;

		// let's see the clock signal
		hitClock=false;
		newClock=inputs[CLOCK_INPUT].getVoltage();
		if (newClock>2.0f && oldClock<=2.0f) {
			// was there a reset?
			if (restartPlease) {
				stepA=0; stepB=0; stepC=0; 
				restartPlease=false;
			}
			// if no reset then next step
			else {stepA++; stepB++; stepC++;}
			if (stepA>=48 || stepA<0) {stepA=0;}
			if (stepB>=16 || stepB<0) {stepB=0;}
			if (stepC>=8 || stepC<0) {stepC=0;}
			hitClock=true;
			offClock=false;
		}
		// else if (newClock>0.2 && oldClock>0.2) {}
		// else if (newClock<=0.2 && oldClock<=0.2) {}
		else if (newClock<=2.0f && oldClock>2.0f) {
			// if the clock pulse is off then the output is also off
			offClock=true;
			for (int c=0;c<outputs[TRIGGER_OUTPUT].channels;c++) {outputs[TRIGGER_OUTPUT].setVoltage(0,c);}
		}
		oldClock=newClock;
		
		// if there's a "new clock pulse" we have a lot to do
		if (hitClock==true) {

			// fix all lights
			for (int k=0;k<48;k++) {
				lights[SEQ_LIGHT+k].setBrightness(paramVal[SEQ_PARAM+k]*0.452);
			}
			
			// indexSeqMode: 0 (random) 1 (1x48) 2 (1x32+1x16) 3 (3x16) 4 (2x16+2x8) 5 (1x16+4x8) 6 (6x8)		
			if (indexSeqMode==0) {
				stepRand=(rand() % 48);
				lights[SEQ_LIGHT+stepRand].setBrightness(0.999);
				outputs[TRIGGER_OUTPUT].setVoltage(paramVal[SEQ_PARAM+stepRand]*10.0f,0);
			}
			else if (indexSeqMode==1) {	
				lights[SEQ_LIGHT+stepA].setBrightness(0.999);
				outputs[TRIGGER_OUTPUT].setVoltage(getVoltNow(SEQ_PARAM+stepA),0);
				// outputs[TRIGGER_OUTPUT].setVoltage(paramVal[SEQ_PARAM+stepA]*10.0f,0);
			}
			else if (indexSeqMode==2) {
				// stepB=(stepA % 16);
				lights[SEQ_LIGHT+stepA].setBrightness(0.999);
				lights[SEQ_LIGHT+32+stepB].setBrightness(0.999);
				outputs[TRIGGER_OUTPUT].setVoltage(getVoltNow(SEQ_PARAM+stepA),0);
				outputs[TRIGGER_OUTPUT].setVoltage(getVoltNow(SEQ_PARAM+32+stepB),1);
				// outputs[TRIGGER_OUTPUT].setVoltage(paramVal[SEQ_PARAM+stepA]*10.0f,0);
				// outputs[TRIGGER_OUTPUT].setVoltage(paramVal[SEQ_PARAM+32+stepB]*10.0f,1);
				if (stepA>=32) {stepA=0;stepB=0;}
			}
			else if (indexSeqMode==3) {	
				// stepB=(stepA % 16);			
				lights[SEQ_LIGHT+stepB].setBrightness(0.999);
				lights[SEQ_LIGHT+16+stepB].setBrightness(0.999);
				lights[SEQ_LIGHT+32+stepB].setBrightness(0.999);
				outputs[TRIGGER_OUTPUT].setVoltage(getVoltNow(SEQ_PARAM+stepB),0);
				outputs[TRIGGER_OUTPUT].setVoltage(getVoltNow(SEQ_PARAM+16+stepB),1);
				outputs[TRIGGER_OUTPUT].setVoltage(getVoltNow(SEQ_PARAM+32+stepB),2);
				// outputs[TRIGGER_OUTPUT].setVoltage(paramVal[SEQ_PARAM+stepB]*10.0f,0);
				// outputs[TRIGGER_OUTPUT].setVoltage(paramVal[SEQ_PARAM+16+stepB]*10.0f,1);
				// outputs[TRIGGER_OUTPUT].setVoltage(paramVal[SEQ_PARAM+32+stepB]*10.0f,2);
			}
			else if (indexSeqMode==4) {	
				// stepB=(stepA % 16);
				// stepC=(stepA % 8);
				lights[SEQ_LIGHT+stepB].setBrightness(0.999);
				lights[SEQ_LIGHT+16+stepB].setBrightness(0.999);
				lights[SEQ_LIGHT+32+stepC].setBrightness(0.999);
				lights[SEQ_LIGHT+40+stepC].setBrightness(0.999);
				outputs[TRIGGER_OUTPUT].setVoltage(getVoltNow(SEQ_PARAM+stepB),0);
				outputs[TRIGGER_OUTPUT].setVoltage(getVoltNow(SEQ_PARAM+16+stepB),1);
				outputs[TRIGGER_OUTPUT].setVoltage(getVoltNow(SEQ_PARAM+32+stepC),2);
				outputs[TRIGGER_OUTPUT].setVoltage(getVoltNow(SEQ_PARAM+40+stepC),3);
			}
			else if (indexSeqMode==5) {
				// stepB=(stepA % 16);
				// stepC=(stepA % 8);
				stepRand=(rand() % 8);
				lights[SEQ_LIGHT+stepB].setBrightness(0.999);
				lights[SEQ_LIGHT+16+stepC].setBrightness(0.999);
				lights[SEQ_LIGHT+24+stepC].setBrightness(0.999);
				lights[SEQ_LIGHT+32+stepC].setBrightness(0.999);
				lights[SEQ_LIGHT+40+stepRand].setBrightness(0.999);
				outputs[TRIGGER_OUTPUT].setVoltage(getVoltNow(SEQ_PARAM+stepB),0);
				outputs[TRIGGER_OUTPUT].setVoltage(getVoltNow(SEQ_PARAM+16+stepC),1);
				outputs[TRIGGER_OUTPUT].setVoltage(getVoltNow(SEQ_PARAM+24+stepC),2);
				outputs[TRIGGER_OUTPUT].setVoltage(getVoltNow(SEQ_PARAM+32+stepC),3);
				outputs[TRIGGER_OUTPUT].setVoltage(paramVal[SEQ_PARAM+40+stepRand]*10.0f,4);
			}
			else if (indexSeqMode==6) {	
				// stepC=(stepA % 8);
				stepRand=(rand() % 8);
				lights[SEQ_LIGHT+stepC].setBrightness(0.999);
				lights[SEQ_LIGHT+8+stepC].setBrightness(0.999);
				lights[SEQ_LIGHT+16+stepC].setBrightness(0.999);
				lights[SEQ_LIGHT+24+stepC].setBrightness(0.999);
				lights[SEQ_LIGHT+32+stepC].setBrightness(0.999);
				lights[SEQ_LIGHT+40+stepRand].setBrightness(0.999);
				outputs[TRIGGER_OUTPUT].setVoltage(getVoltNow(SEQ_PARAM+stepC),0);
				outputs[TRIGGER_OUTPUT].setVoltage(getVoltNow(SEQ_PARAM+8+stepC),1);
				outputs[TRIGGER_OUTPUT].setVoltage(getVoltNow(SEQ_PARAM+16+stepC),2);
				outputs[TRIGGER_OUTPUT].setVoltage(getVoltNow(SEQ_PARAM+24+stepC),3);
				outputs[TRIGGER_OUTPUT].setVoltage(getVoltNow(SEQ_PARAM+32+stepC),4);
				outputs[TRIGGER_OUTPUT].setVoltage(paramVal[SEQ_PARAM+40+stepRand]*10.0f,5);
			}
		}
		
	}

	#include "PushmeSeq/PushmeSeq_json.hpp"

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
		
		// childLabel(HP*1,HP*1, "DEV", 12);

		for (int b=0;b<48;b++) {
			addParam(createLightParamCentered<VCVLightLatch<MediumSimpleLight<YellowLight>>>
				(mm2px(Vec(HP*(floor(b/16)*1.2+0.8), HP*(1.5+1.25*(b % 16)))), 
				module, PushmeSeq::SEQ_PARAM+b, PushmeSeq::SEQ_LIGHT+b));
		}
			
		childInput(PushmeSeq::CLOCK_INPUT,  	HP*1, HP*21.75);
		childKnob(PushmeSeq::SEQMODE_PARAM, 0, 	HP*3, HP*21.75);
		childInput(PushmeSeq::RESET_INPUT,   	HP*1, HP*23.5);
		childOutput(PushmeSeq::TRIGGER_OUTPUT, 	HP*3, HP*23.5);

	}

	// #include "PushmeSeq/PushmeSeq_menu.hpp"
	void appendContextMenu(Menu* menu) override {
		PushmeSeq* module = dynamic_cast<PushmeSeq*>(this->module);
		assert(module);
		menu->addChild(new MenuSeparator);
		menu->addChild(createIndexPtrSubmenuItem("Precision", {"Perfect (100%)","Very good (95%)","Good (85%)","Not bad (75%)","Wasted (60%)","Crap (50%)","Completely insane (35%)"}, &module->indexPrec));
	}

	#include "PushmeSeq/PushmeSeq_keys.hpp"

};

Model* modelPushmeSeq = createModel<PushmeSeq, PushmeSeqWidget>("PushmeSeq");

// --------------------------------------------------

#include "PushmeMore.hpp"	// this is a simple expander