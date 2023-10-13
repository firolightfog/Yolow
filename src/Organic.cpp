// Copyright (c) 2023 András Szabó
#include "plugin.hpp"

struct Organic : Module {

// --------------------------------------------------

	enum ParamId  {
		AUTORESET_PARAM, CHANNEL_SELECTOR_1_PARAM, CHANNEL_SELECTOR_2_PARAM, CHANNEL_SELECTOR_3_PARAM, CHANNEL_SELECTOR_4_PARAM, 
		CHANNEL_SELECTOR_5_PARAM, CHANNEL_SELECTOR_6_PARAM, CHANNEL_SELECTOR_7_PARAM, CHANNEL_SELECTOR_8_PARAM, JUMP_A_1_PARAM, 
		JUMP_A_2_PARAM, JUMP_A_3_PARAM, JUMP_A_4_PARAM, JUMP_A_5_PARAM, JUMP_A_6_PARAM, 
		JUMP_A_7_PARAM, JUMP_A_8_PARAM, PROB_AB_1_PARAM, PROB_AB_2_PARAM, PROB_AB_3_PARAM, 
		PROB_AB_4_PARAM, PROB_AB_5_PARAM, PROB_AB_6_PARAM, PROB_AB_7_PARAM, PROB_AB_8_PARAM, 
		JUMP_B_1_PARAM, JUMP_B_2_PARAM, JUMP_B_3_PARAM, JUMP_B_4_PARAM, JUMP_B_5_PARAM, 
		JUMP_B_6_PARAM, JUMP_B_7_PARAM, JUMP_B_8_PARAM, DICE_SELECTOR_PARAM, DICE_A_PARAM, 
		DICE_AB_PROB_PARAM, DICE_B_PARAM, PARAMS_LEN};

	enum InputId  {
		POLY_INPUT, CLOCK_INPUT, RESET_INPUT, INPUTS_LEN};

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

	Organic() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(AUTORESET_PARAM, 	0.0f, 128, 16, "Autoreset");
		configParam(CHANNEL_SELECTOR_1_PARAM, 	0.0f, 16.0f, 1.0f, "Channel selector 1");
		configParam(CHANNEL_SELECTOR_2_PARAM, 	0.0f, 16.0f, 2.0f, "Channel selector 2");
		configParam(CHANNEL_SELECTOR_3_PARAM, 	0.0f, 16.0f, 3.0f, "Channel selector 3");
		configParam(CHANNEL_SELECTOR_4_PARAM, 	0.0f, 16.0f, 4.0f, "Channel selector 4");
		configParam(CHANNEL_SELECTOR_5_PARAM, 	0.0f, 16.0f, 5.0f, "Channel selector 5");
		configParam(CHANNEL_SELECTOR_6_PARAM, 	0.0f, 16.0f, 6.0f, "Channel selector 6");
		configParam(CHANNEL_SELECTOR_7_PARAM, 	0.0f, 16.0f, 7.0f, "Channel selector 7");
		configParam(CHANNEL_SELECTOR_8_PARAM, 	0.0f, 16.0f, 8.0f, "Channel selector 8");
		configParam(JUMP_A_1_PARAM, 	0.0f, 8.0f, 2.0f, "Jump A 1");
		configParam(JUMP_A_2_PARAM, 	0.0f, 8.0f, 3.0f, "Jump A 2");
		configParam(JUMP_A_3_PARAM, 	0.0f, 8.0f, 4.0f, "Jump A 3");
		configParam(JUMP_A_4_PARAM, 	0.0f, 8.0f, 5.0f, "Jump A 4");
		configParam(JUMP_A_5_PARAM, 	0.0f, 8.0f, 6.0f, "Jump A 5");
		configParam(JUMP_A_6_PARAM, 	0.0f, 8.0f, 7.0f, "Jump A 6");
		configParam(JUMP_A_7_PARAM, 	0.0f, 8.0f, 8.0f, "Jump A 7");
		configParam(JUMP_A_8_PARAM, 	0.0f, 8.0f, 1.0f, "Jump A 8");
		configParam(PROB_AB_1_PARAM, 	0.0f, 1.0f, 0.0f, "Prob AB 1");
		configParam(PROB_AB_2_PARAM, 	0.0f, 1.0f, 0.0f, "Prob AB 2");
		configParam(PROB_AB_3_PARAM, 	0.0f, 1.0f, 0.0f, "Prob AB 3");
		configParam(PROB_AB_4_PARAM, 	0.0f, 1.0f, 0.0f, "Prob AB 4");
		configParam(PROB_AB_5_PARAM, 	0.0f, 1.0f, 0.0f, "Prob AB 5");
		configParam(PROB_AB_6_PARAM, 	0.0f, 1.0f, 0.0f, "Prob AB 6");
		configParam(PROB_AB_7_PARAM, 	0.0f, 1.0f, 0.0f, "Prob AB 7");
		configParam(PROB_AB_8_PARAM, 	0.0f, 1.0f, 0.0f, "Prob AB 8");
		configParam(JUMP_B_1_PARAM, 	0.0f, 8.0f, 8.0f, "Jump B 1");
		configParam(JUMP_B_2_PARAM, 	0.0f, 8.0f, 8.0f, "Jump B 2");
		configParam(JUMP_B_3_PARAM, 	0.0f, 8.0f, 8.0f, "Jump B 3");
		configParam(JUMP_B_4_PARAM, 	0.0f, 8.0f, 8.0f, "Jump B 4");
		configParam(JUMP_B_5_PARAM, 	0.0f, 8.0f, 8.0f, "Jump B 5");
		configParam(JUMP_B_6_PARAM, 	0.0f, 8.0f, 8.0f, "Jump B 6");
		configParam(JUMP_B_7_PARAM, 	0.0f, 8.0f, 8.0f, "Jump B 7");
		configParam(JUMP_B_8_PARAM, 	0.0f, 8.0f, 8.0f, "Jump B 8");
		configParam(DICE_SELECTOR_PARAM, 	0.0f, 1.0f, 0.0f, "Dice selector");
		configParam(DICE_A_PARAM, 	0.0f, 1.0f, 0.0f, "Dice A");
		configParam(DICE_AB_PROB_PARAM, 	0.0f, 1.0f, 0.0f, "Dice AB prob");
		configParam(DICE_B_PARAM, 	0.0f, 1.0f, 0.0f, "Dice B");
		paramQuantities[AUTORESET_PARAM]->snapEnabled = true;
		paramQuantities[AUTORESET_PARAM]->randomizeEnabled = false;
		paramQuantities[CHANNEL_SELECTOR_1_PARAM]->snapEnabled = true;
		paramQuantities[CHANNEL_SELECTOR_2_PARAM]->snapEnabled = true;
		paramQuantities[CHANNEL_SELECTOR_3_PARAM]->snapEnabled = true;
		paramQuantities[CHANNEL_SELECTOR_4_PARAM]->snapEnabled = true;
		paramQuantities[CHANNEL_SELECTOR_5_PARAM]->snapEnabled = true;
		paramQuantities[CHANNEL_SELECTOR_6_PARAM]->snapEnabled = true;
		paramQuantities[CHANNEL_SELECTOR_7_PARAM]->snapEnabled = true;
		paramQuantities[CHANNEL_SELECTOR_8_PARAM]->snapEnabled = true;
		paramQuantities[JUMP_A_1_PARAM]->snapEnabled = true;
		paramQuantities[JUMP_A_2_PARAM]->snapEnabled = true;
		paramQuantities[JUMP_A_3_PARAM]->snapEnabled = true;
		paramQuantities[JUMP_A_4_PARAM]->snapEnabled = true;
		paramQuantities[JUMP_A_5_PARAM]->snapEnabled = true;
		paramQuantities[JUMP_A_6_PARAM]->snapEnabled = true;
		paramQuantities[JUMP_A_7_PARAM]->snapEnabled = true;
		paramQuantities[JUMP_A_8_PARAM]->snapEnabled = true;
		paramQuantities[JUMP_B_1_PARAM]->snapEnabled = true;
		paramQuantities[JUMP_B_2_PARAM]->snapEnabled = true;
		paramQuantities[JUMP_B_3_PARAM]->snapEnabled = true;
		paramQuantities[JUMP_B_4_PARAM]->snapEnabled = true;
		paramQuantities[JUMP_B_5_PARAM]->snapEnabled = true;
		paramQuantities[JUMP_B_6_PARAM]->snapEnabled = true;
		paramQuantities[JUMP_B_7_PARAM]->snapEnabled = true;
		paramQuantities[JUMP_B_8_PARAM]->snapEnabled = true;
		paramQuantities[DICE_SELECTOR_PARAM]->snapEnabled = true;
		paramQuantities[DICE_A_PARAM]->snapEnabled = true;
		paramQuantities[DICE_AB_PROB_PARAM]->snapEnabled = true;
		paramQuantities[DICE_B_PARAM]->snapEnabled = true;
		paramQuantities[DICE_SELECTOR_PARAM]->randomizeEnabled = false;
		paramQuantities[DICE_A_PARAM]->randomizeEnabled = false;
		paramQuantities[DICE_AB_PROB_PARAM]->randomizeEnabled = false;
		paramQuantities[DICE_B_PARAM]->randomizeEnabled = false;

		configInput(POLY_INPUT, "Poly"); 
		configInput(CLOCK_INPUT, "Clock"); 
		configInput(RESET_INPUT, "Reset"); 

		configOutput(MONO_OUTPUT, "Mono"); 
		configOutput(DEBUG_OUTPUT, "Debug"); 
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
	int currentStep=0;
	int countStep=0;
	float newVolt=0;
	int newChan=0;
	
	// --------------------------------------------------

	// randomize the first column of knobs with a context menu call
	void randomizeCV() {
		for (int p=0;p<8;p++) {params[CHANNEL_SELECTOR_1_PARAM+p].setValue(rand() % 17);}		
	};	
	
	// randomize the second column of knobs with a context menu call
	void randomizeJumpA() {
		for (int p=0;p<8;p++) {params[JUMP_A_1_PARAM+p].setValue(rand() % 8);}		
	};
	
	// randomize the third column of knobs with a context menu call
	void randomizeABProb() {
		for (int p=0;p<8;p++) {params[PROB_AB_1_PARAM+p].setValue(rack::random::uniform());}		
	};	
	
	// randomize the forth column of knobs with a context menu call
	void randomizeJumpB() {
		for (int p=0;p<8;p++) {params[JUMP_B_1_PARAM+p].setValue(rand() % 8);}		
	};	

	// randomize the forth column of knobs with a context menu call
	void randomizeStep(int px) {
		params[CHANNEL_SELECTOR_1_PARAM+px].setValue(rand() % 17);
		params[PROB_AB_1_PARAM+px].setValue(rack::random::uniform());
		params[JUMP_A_1_PARAM+px].setValue(rand() % 8);		
		params[JUMP_B_1_PARAM+px].setValue(rand() % 8);		
	};	
	
	// --------------------------------------------------
	
	void process(const ProcessArgs& args) override {

		if (loop--<=0) {
			loop=9000;
			// save some more CPU
			for (int p=0;p<PARAMS_LEN;p++) {paramVal[p]=params[p].getValue();}
			outputs[DEBUG_OUTPUT].channels=8;
		}

		// let's see the reset signal
		newReset=inputs[RESET_INPUT].getVoltage();
		if (newReset>2.0f && oldReset<=2.0f) {hitReset=true;}
		// else if (newReset>2.0f && oldReset>2.0f) {}
		// else if (newReset<=2.0f && oldReset>2.0f) {}
		// else if (newReset<=2.0f && oldReset<=2.0f) {}
		oldReset=newReset;

		// let's see the clock signal
		newClock=inputs[CLOCK_INPUT].getVoltage();
		if (newClock>2.0f && oldClock<=2.0f) {hitClock=true; countStep++;}
		// else if (newClock>2.0f && oldClock>2.0f) {}
		// else if (newClock<=2.0f && oldClock<=2.0f) {}
		// else if (newClock<=2.0f && oldClock>2.0f) {}
		oldClock=newClock;

		if (hitClock==true && hitReset==false) {
			// first update countStep and currentStep
			if (paramVal[AUTORESET_PARAM]>0 && countStep>=paramVal[AUTORESET_PARAM]) {
				countStep=0; currentStep=0;
			}
			else {
				if (rack::random::uniform() > paramVal[PROB_AB_1_PARAM + currentStep]) {
					currentStep=paramVal[JUMP_A_1_PARAM+currentStep]-1;
				}
				else {
					currentStep=paramVal[JUMP_B_1_PARAM+currentStep]-1;
				}
				if (0>currentStep) {currentStep = (rand() % 8);}
			}
			// switch off the lights
			for (int i=0;i<8;i++) {lights[LIGHT_1_LIGHT+i].setBrightness(0);}
			lights[LIGHT_1_LIGHT+currentStep].setBrightness(1);
			// let's see which channel is selected
			newChan=paramVal[CHANNEL_SELECTOR_1_PARAM+currentStep]-1;
			if (0>newChan) {newChan=rand() % 16;}
			// get the voltage of that channel
			newVolt=inputs[POLY_INPUT].getVoltage(newChan);			
			// and send to the output
			outputs[MONO_OUTPUT].setVoltage(newVolt);
		}
		else if (hitClock==true && hitReset==true) {
			// first update countStep and currentStep
			countStep=0; currentStep=0;
			// switch off the lights
			for (int i=0;i<8;i++) {lights[LIGHT_1_LIGHT+i].setBrightness(0);}
			lights[LIGHT_1_LIGHT+currentStep].setBrightness(1);
			// let's see which channel is selected
			newChan=paramVal[CHANNEL_SELECTOR_1_PARAM+currentStep]-1;
			if (0>newChan) {newChan=rand() % 16;}
			// get the voltage of that channel
			newVolt=inputs[POLY_INPUT].getVoltage(newChan);			
			// and send to the output
			outputs[MONO_OUTPUT].setVoltage(newVolt);
 			}
		else if (hitClock==false && hitReset==true) {currentStep=0;}
		
		hitClock=false;
		hitReset=false;

	}

	// #include "Organic/Organic_json.hpp"

};

// --------------------------------------------------

struct OrganicWidget : ModuleWidget {

	Organic* module;
	#include "share/widgetwizard.hpp"   // addChild abbreviations

	OrganicWidget(Organic* module) {

		this->module = module;
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/Organic.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		
		childKnob(Organic::AUTORESET_PARAM, 1, HP*3, HP*2);
		childKnob(Organic::CHANNEL_SELECTOR_1_PARAM, 1, HP*2, HP*6.5);
		childKnob(Organic::CHANNEL_SELECTOR_2_PARAM, 1, HP*2, HP*8.75);
		childKnob(Organic::CHANNEL_SELECTOR_3_PARAM, 1, HP*2, HP*11);
		childKnob(Organic::CHANNEL_SELECTOR_4_PARAM, 1, HP*2, HP*13.25);
		childKnob(Organic::CHANNEL_SELECTOR_5_PARAM, 1, HP*2, HP*15.5);
		childKnob(Organic::CHANNEL_SELECTOR_6_PARAM, 1, HP*2, HP*17.75);
		childKnob(Organic::CHANNEL_SELECTOR_7_PARAM, 1, HP*2, HP*20);
		childKnob(Organic::CHANNEL_SELECTOR_8_PARAM, 1, HP*2, HP*22.25);
		childKnob(Organic::JUMP_A_1_PARAM, 1, HP*5, HP*6.5);
		childKnob(Organic::JUMP_A_2_PARAM, 1, HP*5, HP*8.75);
		childKnob(Organic::JUMP_A_3_PARAM, 1, HP*5, HP*11);
		childKnob(Organic::JUMP_A_4_PARAM, 1, HP*5, HP*13.25);
		childKnob(Organic::JUMP_A_5_PARAM, 1, HP*5, HP*15.5);
		childKnob(Organic::JUMP_A_6_PARAM, 1, HP*5, HP*17.75);
		childKnob(Organic::JUMP_A_7_PARAM, 1, HP*5, HP*20);
		childKnob(Organic::JUMP_A_8_PARAM, 1, HP*5, HP*22.25);
		childKnob(Organic::PROB_AB_1_PARAM, 0, HP*7, HP*6.5);
		childKnob(Organic::PROB_AB_2_PARAM, 0, HP*7, HP*8.75);
		childKnob(Organic::PROB_AB_3_PARAM, 0, HP*7, HP*11);
		childKnob(Organic::PROB_AB_4_PARAM, 0, HP*7, HP*13.25);
		childKnob(Organic::PROB_AB_5_PARAM, 0, HP*7, HP*15.5);
		childKnob(Organic::PROB_AB_6_PARAM, 0, HP*7, HP*17.75);
		childKnob(Organic::PROB_AB_7_PARAM, 0, HP*7, HP*20);
		childKnob(Organic::PROB_AB_8_PARAM, 0, HP*7, HP*22.25);
		childKnob(Organic::JUMP_B_1_PARAM, 1, HP*9, HP*6.5);
		childKnob(Organic::JUMP_B_2_PARAM, 1, HP*9, HP*8.75);
		childKnob(Organic::JUMP_B_3_PARAM, 1, HP*9, HP*11);
		childKnob(Organic::JUMP_B_4_PARAM, 1, HP*9, HP*13.25);
		childKnob(Organic::JUMP_B_5_PARAM, 1, HP*9, HP*15.5);
		childKnob(Organic::JUMP_B_6_PARAM, 1, HP*9, HP*17.75);
		childKnob(Organic::JUMP_B_7_PARAM, 1, HP*9, HP*20);
		childKnob(Organic::JUMP_B_8_PARAM, 1, HP*9, HP*22.25);
		
		// childButtonmom(Organic::DICE_SELECTOR_PARAM, 0, HP*2, HP*24);
		// childButtonmom(Organic::DICE_A_PARAM, 0, HP*5, HP*24);
		// childButtonmom(Organic::DICE_AB_PROB_PARAM, 0, HP*7, HP*24);
		// childButtonmom(Organic::DICE_B_PARAM, 0, HP*9, HP*24);

		childInput(Organic::POLY_INPUT, HP*6, HP*4);
		childInput(Organic::CLOCK_INPUT, HP*6, HP*2);
		childInput(Organic::RESET_INPUT, HP*8, HP*2);
		childOutput(Organic::MONO_OUTPUT, HP*8, HP*4);
		childLight(Organic::LIGHT_1_LIGHT, 0, HP*2, HP*6.5);
		childLight(Organic::LIGHT_2_LIGHT, 0, HP*2, HP*8.75);
		childLight(Organic::LIGHT_3_LIGHT, 0, HP*2, HP*11);
		childLight(Organic::LIGHT_4_LIGHT, 0, HP*2, HP*13.25);
		childLight(Organic::LIGHT_5_LIGHT, 0, HP*2, HP*15.5);
		childLight(Organic::LIGHT_6_LIGHT, 0, HP*2, HP*17.75);
		childLight(Organic::LIGHT_7_LIGHT, 0, HP*2, HP*20);
		childLight(Organic::LIGHT_8_LIGHT, 0, HP*2, HP*22.25);

		// childOutput(Organic::DEBUG_OUTPUT, HP*3, HP*24.5);

		// childLabel(HP*1,HP*1, "DEV", 12);
	}

	// #include "Organic/Organic_menu.hpp"
	void appendContextMenu(Menu* menu) override {
		Organic* module = dynamic_cast<Organic*>(this->module);
		assert(module);	

		menu->addChild(new MenuSeparator);
		menu->addChild(createMenuItem("Randomize all CVs", "", [=]() {module->randomizeCV();}));	
		menu->addChild(createMenuItem("Randomize A options", "", [=]() {module->randomizeJumpA();}));	
		menu->addChild(createMenuItem("Randomize AB probabilities", "", [=]() {module->randomizeABProb();}));	
		menu->addChild(createMenuItem("Randomize B options", "", [=]() {module->randomizeJumpB();}));	
		menu->addChild(createIndexSubmenuItem("Randomize a step",
 			{"step 1", // 0
			"step 2", // 1
			"step 3", // 2
			"step 4", // 3
			"step 5", // 4
			"step 6", // 5
			"step 7", // 6
			"step 8"},
			[=]() {return -1;},
			[=](int ptx) {module->randomizeStep(ptx);}
		));


	}

	// #include "Organic/Organic_keys.hpp"

};

Model* modelOrganic = createModel<Organic, OrganicWidget>("Organic");
