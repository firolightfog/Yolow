// Copyright (c) 2022 András Szabó
#include "plugin.hpp"

struct RotaTrig : Module {

// --------------------------------------------------

	enum ParamId    {
		NUMBER_OF_SLOTS_PARAM, MODE_PARAM, PARAMS_LEN};

	enum InputId    {
		TRIGGER_INPUT, ENABLE_1_INPUT, ENABLE_2_INPUT, ENABLE_3_INPUT, ENABLE_4_INPUT, 
		ENABLE_5_INPUT, ENABLE_6_INPUT, INPUTS_LEN};

	enum OutputId    {
		THE_1_OUTPUT, THE_2_OUTPUT, THE_3_OUTPUT, THE_4_OUTPUT, THE_5_OUTPUT, 
		THE_6_OUTPUT, OUTPUTS_LEN};

	enum LightId    {
		LED_1_LIGHT, LED_2_LIGHT, LED_3_LIGHT, LED_4_LIGHT, LED_5_LIGHT, 
		LED_6_LIGHT, LIGHTS_LEN};

	// small assistance to save older values for reference;
	float paramVal[PARAMS_LEN]={0,0};
	float inputVolt[INPUTS_LEN]={0,0,0,0,0,0,0};
	// float lightVal[LIGHTS_LEN]={0};

// --------------------------------------------------

	RotaTrig() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(NUMBER_OF_SLOTS_PARAM, 	1.0f, 6.0f, 6.0f, "Number of slots");
		configParam(MODE_PARAM, 	0.0f, 2.0f, 1.0f, "Mode");
		paramQuantities[MODE_PARAM]->snapEnabled = true;
		paramQuantities[NUMBER_OF_SLOTS_PARAM]->snapEnabled = true;

		configInput(TRIGGER_INPUT, "Trigger"); 
		configInput(ENABLE_1_INPUT, "Enable 1"); 
		configInput(ENABLE_2_INPUT, "Enable 2"); 
		configInput(ENABLE_3_INPUT, "Enable 3"); 
		configInput(ENABLE_4_INPUT, "Enable 4"); 
		configInput(ENABLE_5_INPUT, "Enable 5"); 
		configInput(ENABLE_6_INPUT, "Enable 6"); 

		configOutput(THE_1_OUTPUT, "The 1"); 
		configOutput(THE_2_OUTPUT, "The 2"); 
		configOutput(THE_3_OUTPUT, "The 3"); 
		configOutput(THE_4_OUTPUT, "The 4"); 
		configOutput(THE_5_OUTPUT, "The 5"); 
		configOutput(THE_6_OUTPUT, "The 6"); 
	}

// --------------------------------------------------

	int loop=0;     // save some CPU in process()

	// for managing the RESET and CLOCK signals
	float newClock=0.0f;
	float oldClock=0.0f;
	int indexMode=0;		// PARAM value; mode of selection
	int currStep=0;			// calculated step (aka output) for the next step
	int maxStep=6;			// PARAM value; max number of outputs available
	int selOut=0;			// selected output
	int CLOCK_INPUT=TRIGGER_INPUT;
	
	std::string labelMode[3]={"Random mode","Step-by-step mode","0v-10V CV driven selection"};
	
	void process(const ProcessArgs& args) override {

		if (loop--<=0) {
			loop=9000;
			// save some more CPU
			indexMode=params[MODE_PARAM].getValue();
			maxStep=(int)params[NUMBER_OF_SLOTS_PARAM].getValue();
			paramQuantities[MODE_PARAM]->description = labelMode[indexMode];
		}
		
		// ENABLE inputs to be checked & next step highlighted
		for (int i=0;i<6;i++) {
			if (inputs[ENABLE_1_INPUT+i].getVoltage()>2) {
				if (inputVolt[ENABLE_1_INPUT+i]<=2) {lights[LED_1_LIGHT+i].setBrightness(1);}
				inputVolt[ENABLE_1_INPUT+i]=10;}
			else {	// .getVoltage()<=2
				inputVolt[ENABLE_1_INPUT+i]=0.1;}
		}

		// let's send the pulses to the outputs
		if (indexMode==0) {	// random
			newClock=inputs[CLOCK_INPUT].getVoltage();
			if (newClock>2.0f && oldClock<=2.0f) {
				selOut=(rand() % maxStep);
				for (int i=6;i>=0;i--) {
					if (lights[LED_1_LIGHT+i].getBrightness()>0){selOut=i;break;}
				}					
				outputs[THE_1_OUTPUT+selOut].setVoltage(10.f);
			}
			else if (newClock<=2.0f && oldClock>2.0f) {
				lights[LED_1_LIGHT+selOut].setBrightness(0);
				outputs[THE_1_OUTPUT+selOut].setVoltage(0);
			}
			oldClock=newClock;
		}
		else if (indexMode==1) {	// step by step (oh baby!)
			newClock=inputs[CLOCK_INPUT].getVoltage();
			if (newClock>2.0f && oldClock<=2.0f) {
				selOut=currStep;
				for (int i=6;i>=0;i--) {
					if (lights[LED_1_LIGHT+i].getBrightness()>0){selOut=i;break;}
				}					
				outputs[THE_1_OUTPUT+selOut].setVoltage(10.f);
				currStep++;
				if (currStep>=maxStep) {currStep=0;}
			}			
			else if (newClock<=2.0f && oldClock>2.0f) {
				lights[LED_1_LIGHT+selOut].setBrightness(0);
				outputs[THE_1_OUTPUT+selOut].setVoltage(0);
			}
			oldClock=newClock;
		}
		else if (indexMode==2) {	// 0-10V CV controlled
			newClock=inputs[CLOCK_INPUT].getVoltage();
			newClock=newClock/6*maxStep;
			for (int o=0;o<6;o++) {
				outputs[THE_1_OUTPUT+o].setVoltage(0);
				lights[LED_1_LIGHT+selOut].setBrightness(0);
			}					
			outputs[THE_1_OUTPUT+floor((newClock/10)*6)].setVoltage(10.f);
		}
	}

// --------------------------------------------------

};

// --------------------------------------------------

struct RotaTrigWidget : ModuleWidget {

	RotaTrig* module;
	#include "share/widgetwizard.hpp"   // addChild abbreviations

	RotaTrigWidget(RotaTrig* module) {

		this->module = module;
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/RotaTrig.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		
		childInput(RotaTrig::TRIGGER_INPUT, HP*1, HP*5);
		childSwitch(RotaTrig::MODE_PARAM, 1, HP*3, HP*5);

		childKnob(RotaTrig::NUMBER_OF_SLOTS_PARAM, 1, HP*2, HP*8);
		
		childInput(RotaTrig::ENABLE_1_INPUT, HP*1, HP*10.75);
		childInput(RotaTrig::ENABLE_2_INPUT, HP*1, HP*13.0);
		childInput(RotaTrig::ENABLE_3_INPUT, HP*1, HP*15.25);
		childInput(RotaTrig::ENABLE_4_INPUT, HP*1, HP*17.5);
		childInput(RotaTrig::ENABLE_5_INPUT, HP*1, HP*19.75);
		childInput(RotaTrig::ENABLE_6_INPUT, HP*1, HP*22.00);
		childOutput(RotaTrig::THE_1_OUTPUT, HP*3, HP*10.75);
		childOutput(RotaTrig::THE_2_OUTPUT, HP*3, HP*13.0);
		childOutput(RotaTrig::THE_3_OUTPUT, HP*3, HP*15.25);
		childOutput(RotaTrig::THE_4_OUTPUT, HP*3, HP*17.50);
		childOutput(RotaTrig::THE_5_OUTPUT, HP*3, HP*19.75);
		childOutput(RotaTrig::THE_6_OUTPUT, HP*3, HP*22.00);
		childLight(RotaTrig::LED_1_LIGHT, 0, HP*2, HP*11.75);
		childLight(RotaTrig::LED_2_LIGHT, 0, HP*2, HP*14.0);
		childLight(RotaTrig::LED_3_LIGHT, 0, HP*2, HP*16.25);
		childLight(RotaTrig::LED_4_LIGHT, 0, HP*2, HP*18.50);
		childLight(RotaTrig::LED_5_LIGHT, 0, HP*2, HP*20.75);
		childLight(RotaTrig::LED_6_LIGHT, 0, HP*2, HP*23.0);
	}

};

Model* modelRotaTrig = createModel<RotaTrig, RotaTrigWidget>("RotaTrig");
