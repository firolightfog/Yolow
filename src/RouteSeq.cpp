// Copyright (c) 2023 András Szabó
#include "plugin.hpp"

struct RouteSeq : Module {

// --------------------------------------------------

	enum ParamId  {
		CHANNEL_SELECTOR_1_PARAM, CHANNEL_SELECTOR_2_PARAM, CHANNEL_SELECTOR_3_PARAM, CHANNEL_SELECTOR_4_PARAM, CHANNEL_SELECTOR_5_PARAM, 
		CHANNEL_SELECTOR_6_PARAM, CHANNEL_SELECTOR_7_PARAM, CHANNEL_SELECTOR_8_PARAM, STEP_FREEZER_1_PARAM, STEP_FREEZER_2_PARAM, 
		STEP_FREEZER_3_PARAM, STEP_FREEZER_4_PARAM, STEP_FREEZER_5_PARAM, STEP_FREEZER_6_PARAM, STEP_FREEZER_7_PARAM, 
		STEP_FREEZER_8_PARAM, PARAMS_LEN};

	enum InputId  {
		CLOCK_INPUT, RESET_INPUT, POLY_INPUT, INPUTS_LEN};

	enum OutputId {
		MONO_OUTPUT, DEBUG_OUTPUT, OUTPUTS_LEN};

	enum LightId  {
		LIGHT_1_LIGHT, LIGHT_2_LIGHT, LIGHT_3_LIGHT, LIGHT_4_LIGHT, LIGHT_5_LIGHT, 
		LIGHT_6_LIGHT, LIGHT_7_LIGHT, LIGHT_8_LIGHT, LIGHTS_LEN};

	// small assistance to save older values for reference;
	float paramVal[PARAMS_LEN]={0};
	// float inputVolt[INPUTS_LEN]={0};
	// float lightVal[LIGHTS_LEN]={0};

// --------------------------------------------------

	RouteSeq() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		
		configParam(CHANNEL_SELECTOR_1_PARAM, 	1.0f, 16.0f, 1.0f, "Channel selector 1");
		configParam(CHANNEL_SELECTOR_2_PARAM, 	1.0f, 16.0f, 1.0f, "Channel selector 2");
		configParam(CHANNEL_SELECTOR_3_PARAM, 	1.0f, 16.0f, 1.0f, "Channel selector 3");
		configParam(CHANNEL_SELECTOR_4_PARAM, 	1.0f, 16.0f, 1.0f, "Channel selector 4");
		configParam(CHANNEL_SELECTOR_5_PARAM, 	1.0f, 16.0f, 1.0f, "Channel selector 5");
		configParam(CHANNEL_SELECTOR_6_PARAM, 	1.0f, 16.0f, 1.0f, "Channel selector 6");
		configParam(CHANNEL_SELECTOR_7_PARAM, 	1.0f, 16.0f, 1.0f, "Channel selector 7");
		configParam(CHANNEL_SELECTOR_8_PARAM, 	1.0f, 16.0f, 1.0f, "Channel selector 8");
		
		configParam(STEP_FREEZER_1_PARAM, 	1.0f, 16.0f, 1.0f, "Step freezer 1");
		configParam(STEP_FREEZER_2_PARAM, 	1.0f, 16.0f, 1.0f, "Step freezer 2");
		configParam(STEP_FREEZER_3_PARAM, 	1.0f, 16.0f, 1.0f, "Step freezer 3");
		configParam(STEP_FREEZER_4_PARAM, 	1.0f, 16.0f, 1.0f, "Step freezer 4");
		configParam(STEP_FREEZER_5_PARAM, 	1.0f, 16.0f, 1.0f, "Step freezer 5");
		configParam(STEP_FREEZER_6_PARAM, 	1.0f, 16.0f, 1.0f, "Step freezer 6");
		configParam(STEP_FREEZER_7_PARAM, 	1.0f, 16.0f, 1.0f, "Step freezer 7");
		configParam(STEP_FREEZER_8_PARAM, 	1.0f, 16.0f, 1.0f, "Step freezer 8");
		
		paramQuantities[CHANNEL_SELECTOR_1_PARAM]->snapEnabled = true;
		paramQuantities[CHANNEL_SELECTOR_2_PARAM]->snapEnabled = true;
		paramQuantities[CHANNEL_SELECTOR_3_PARAM]->snapEnabled = true;
		paramQuantities[CHANNEL_SELECTOR_4_PARAM]->snapEnabled = true;
		paramQuantities[CHANNEL_SELECTOR_5_PARAM]->snapEnabled = true;
		paramQuantities[CHANNEL_SELECTOR_6_PARAM]->snapEnabled = true;
		paramQuantities[CHANNEL_SELECTOR_7_PARAM]->snapEnabled = true;
		paramQuantities[CHANNEL_SELECTOR_8_PARAM]->snapEnabled = true;
		
		paramQuantities[STEP_FREEZER_1_PARAM]->snapEnabled = true;
		paramQuantities[STEP_FREEZER_2_PARAM]->snapEnabled = true;
		paramQuantities[STEP_FREEZER_3_PARAM]->snapEnabled = true;
		paramQuantities[STEP_FREEZER_4_PARAM]->snapEnabled = true;
		paramQuantities[STEP_FREEZER_5_PARAM]->snapEnabled = true;
		paramQuantities[STEP_FREEZER_6_PARAM]->snapEnabled = true;
		paramQuantities[STEP_FREEZER_7_PARAM]->snapEnabled = true;
		paramQuantities[STEP_FREEZER_8_PARAM]->snapEnabled = true;

 		paramQuantities[STEP_FREEZER_1_PARAM]->randomizeEnabled = false;
		paramQuantities[STEP_FREEZER_2_PARAM]->randomizeEnabled = false;
		paramQuantities[STEP_FREEZER_3_PARAM]->randomizeEnabled = false;
		paramQuantities[STEP_FREEZER_4_PARAM]->randomizeEnabled = false;
		paramQuantities[STEP_FREEZER_5_PARAM]->randomizeEnabled = false;
		paramQuantities[STEP_FREEZER_6_PARAM]->randomizeEnabled = false;
		paramQuantities[STEP_FREEZER_7_PARAM]->randomizeEnabled = false;
		paramQuantities[STEP_FREEZER_8_PARAM]->randomizeEnabled = false;

		configInput(CLOCK_INPUT, "Clock"); 
		configInput(RESET_INPUT, "Reset"); 
		configInput(POLY_INPUT, "Poly"); 

		configOutput(MONO_OUTPUT, "Mono"); 
		configOutput(DEBUG_OUTPUT, "DEBUG"); 
	}

// --------------------------------------------------

	int loop=0;     // save some CPU in process()

	// for managing the RESET and CLOCK signals
	float newReset=0.0f;
	float oldReset=0.0f;
	float newClock=0.0f;
	float oldClock=0.0f;
	bool hitClock=false;
	bool hitReset=false;
	
	// for calculating the sequencer steps
	int currentStep=1;
	int totalSteps=0;
	int knobSteps=1;
	int needed=0;
	int randval=1;

	// for the additional labels of channel selector knobs
	const std::string valNotes[17]={"random",
	"C-4","C#4","D-4","D#4","E-4",
	"F-4","F#4","G-4","G#4","A-4",
	"A#4","B-4","C-5","C#5","D-5","D#5"};

	void process(const ProcessArgs& args) override {

		if (loop--<=0) {
			loop=8972;
			// save some more CPU
			for (int p=0;p<PARAMS_LEN;p++) {paramVal[p]=params[p].getValue();}
			
			// calculate the length of the loops based on the freezer/clockdiv knobs
			totalSteps=0;
			for (int p=0;p<8;p++) {totalSteps=totalSteps+paramVal[STEP_FREEZER_1_PARAM+p];}
			
			// add labels to the knobs
			for (int p=0;p<8;p++) {
				paramQuantities[STEP_FREEZER_1_PARAM+p]->description = ("Total steps: " + std::to_string(totalSteps));
				if (inputs[POLY_INPUT].isConnected()) {
					paramQuantities[CHANNEL_SELECTOR_1_PARAM+p]->description = "Poly cable channel selected";
				}
				else {
					paramQuantities[CHANNEL_SELECTOR_1_PARAM+p]->description = ("Without cable: " + valNotes[(int)paramVal[CHANNEL_SELECTOR_1_PARAM+p]]);
				}
			}
			
		// outputs[DEBUG_OUTPUT].channels=4;
		// outputs[DEBUG_OUTPUT].setVoltage(totalSteps,0);
		// outputs[DEBUG_OUTPUT].setVoltage(currentStep,1);
		// outputs[DEBUG_OUTPUT].setVoltage(needed,2);

		}

		// let's see the reset signal
		newReset=inputs[RESET_INPUT].getVoltage();
		if (newReset>2.0f && oldReset<=2.0f) {
			hitReset=true; 
			currentStep=1;
		}
		// else if (newReset>2.0f && oldReset>2.0f) {}
		// else if (newReset<=2.0f && oldReset>2.0f) {}
		// else if (newReset<=2.0f && oldReset<=2.0f) {}

		// let's see the clock signal
		hitClock=false;
		newClock=inputs[CLOCK_INPUT].getVoltage();
		if (newClock>2.0f && oldClock<=2.0f) {
			hitClock=true;
			// let's see the number of step we're at
			if (hitReset==true) {currentStep=1; hitReset=false;}
			else if (currentStep>=totalSteps) {currentStep=1;}
			else {currentStep++;}
			// which of the positions is active?
			int xS=0;
			needed=0;
			for (int p=0;p<8;p++) {
				xS=xS+paramVal[STEP_FREEZER_1_PARAM+p];
				// na és mosta currentStep szerint ezen a gombon kell lenni?
				lights[LIGHT_1_LIGHT+p].setBrightness(0);					
				if (currentStep>=xS) {needed=(p==7)?0:p+1;}
			}
			lights[LIGHT_1_LIGHT+needed].setBrightness(1);					
			randval= 1 + (rand() % 16);
		}
		// else if (newClock>2.0f && oldClock>2.0f) {}
		// else if (newClock<=2.0f && oldClock<=2.0f) {}
		// else if (newClock<=2.0f && oldClock>2.0f) {}

		// I know which position to look at, let's send the voltage
		if (inputs[POLY_INPUT].isConnected()) {
			outputs[MONO_OUTPUT].setVoltage(
				inputs[POLY_INPUT].getVoltage(
					paramVal[CHANNEL_SELECTOR_1_PARAM+needed]-1));
		} 
		else if (loop % 400 == 25) { // if no input is connected then C-4, C#4, D-4, etc.
			outputs[MONO_OUTPUT].setVoltage((paramVal[CHANNEL_SELECTOR_1_PARAM+needed]-1)/12);
		}

		// outputs[DEBUG_OUTPUT].setVoltage(paramVal[CHANNEL_SELECTOR_1_PARAM+needed]-1,3);
		
		oldReset=newReset;
		oldClock=newClock;

	}

	// #include "RouteSeq/RouteSeq_json.hpp"

};

// --------------------------------------------------

struct RouteSeqWidget : ModuleWidget {

	RouteSeq* module;
	#include "share/widgetwizard.hpp"   // addChild abbreviations

	RouteSeqWidget(RouteSeq* module) {

		this->module = module;
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/RouteSeq.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		
		childKnob(RouteSeq::CHANNEL_SELECTOR_1_PARAM, 0, HP*1, HP*7);
		childKnob(RouteSeq::CHANNEL_SELECTOR_2_PARAM, 0, HP*1, HP*9);
		childKnob(RouteSeq::CHANNEL_SELECTOR_3_PARAM, 0, HP*1, HP*11);
		childKnob(RouteSeq::CHANNEL_SELECTOR_4_PARAM, 0, HP*1, HP*13);
		childKnob(RouteSeq::CHANNEL_SELECTOR_5_PARAM, 0, HP*1, HP*15);
		childKnob(RouteSeq::CHANNEL_SELECTOR_6_PARAM, 0, HP*1, HP*17);
		childKnob(RouteSeq::CHANNEL_SELECTOR_7_PARAM, 0, HP*1, HP*19);
		childKnob(RouteSeq::CHANNEL_SELECTOR_8_PARAM, 0, HP*1, HP*21);

		childKnob(RouteSeq::STEP_FREEZER_1_PARAM, 0, HP*3, HP*7);
		childKnob(RouteSeq::STEP_FREEZER_2_PARAM, 0, HP*3, HP*9);
		childKnob(RouteSeq::STEP_FREEZER_3_PARAM, 0, HP*3, HP*11);
		childKnob(RouteSeq::STEP_FREEZER_4_PARAM, 0, HP*3, HP*13);
		childKnob(RouteSeq::STEP_FREEZER_5_PARAM, 0, HP*3, HP*15);
		childKnob(RouteSeq::STEP_FREEZER_6_PARAM, 0, HP*3, HP*17);
		childKnob(RouteSeq::STEP_FREEZER_7_PARAM, 0, HP*3, HP*19);
		childKnob(RouteSeq::STEP_FREEZER_8_PARAM, 0, HP*3, HP*21);
		
		childLight(RouteSeq::LIGHT_1_LIGHT, 0, HP*3, HP*7);
		childLight(RouteSeq::LIGHT_2_LIGHT, 0, HP*3, HP*9);
		childLight(RouteSeq::LIGHT_3_LIGHT, 0, HP*3, HP*11);
		childLight(RouteSeq::LIGHT_4_LIGHT, 0, HP*3, HP*13);
		childLight(RouteSeq::LIGHT_5_LIGHT, 0, HP*3, HP*15);
		childLight(RouteSeq::LIGHT_6_LIGHT, 0, HP*3, HP*17);
		childLight(RouteSeq::LIGHT_7_LIGHT, 0, HP*3, HP*19);
		childLight(RouteSeq::LIGHT_8_LIGHT, 0, HP*3, HP*21);

		childInput(RouteSeq::RESET_INPUT, HP*1, HP*5);
		childInput(RouteSeq::CLOCK_INPUT, HP*3, HP*5);
		childInput(RouteSeq::POLY_INPUT, HP*1, HP*23);
		childOutput(RouteSeq::MONO_OUTPUT, HP*3, HP*23);

		// childOutput(RouteSeq::DEBUG_OUTPUT, HP*2, HP*24.5);
		// childLabel(HP*1,HP*23.5, "DEV", 12);
	
	}

	// #include "RouteSeq/RouteSeq_menu.hpp"
	// #include "RouteSeq/RouteSeq_keys.hpp"

};

Model* modelRouteSeq = createModel<RouteSeq, RouteSeqWidget>("RouteSeq");