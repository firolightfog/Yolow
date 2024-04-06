// Copyright (c) 2023 András Szabó
#include "plugin.hpp"

struct MidTest : Module {

// --------------------------------------------------

	enum ParamId  {
		TRIGGER_PARAM, START_CV_A_PARAM, END_CV_A_PARAM, DURATION_A_PARAM, SHAPE_A_PARAM, 
		START_CV_B_PARAM, END_CV_B_PARAM, DURATION_B_PARAM, SHAPE_B_PARAM, START_CV_C_PARAM, 
		END_CV_C_PARAM, DURATION_C_PARAM, SHAPE_C_PARAM, START_CV_D_PARAM, END_CV_D_PARAM, 
		DURATION_D_PARAM, SHAPE_D_PARAM, PARAMS_LEN};

	enum InputId  {
		TRIG_INPUT, START_TRIGGER_B_INPUT, END_TRIGGER_B_INPUT, DURATION_GATE_B_INPUT, START_TRIGGER_C_INPUT, 
		END_TRIGGER_C_INPUT, DURATION_GATE_C_INPUT, START_TRIGGER_D_INPUT, END_TRIGGER_D_INPUT, DURATION_GATE_D_INPUT, 
		AUDIO_INPUT, INPUTS_LEN};

	enum OutputId {
		START_TRIGGER_A_OUTPUT, END_TRIGGER_A_OUTPUT, DURATION_GATE_A_OUTPUT, ENVELOPE_OUTPUT, AUDIO_OUTPUT, OUTPUTS_LEN};

	enum LightId  {
		LED_TRIGGER_PARAM, LIGHTS_LEN};

	// small assistance to save older values for reference;
	float paramVal[PARAMS_LEN]={0};
	// float inputVolt[INPUTS_LEN]={0};
	// float lightVal[LIGHTS_LEN]={0};

// --------------------------------------------------

	MidTest() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(TRIGGER_PARAM, 	0.0f, 1.0f, 0.0f, "Trigger");
		configParam(START_CV_A_PARAM, 	-10.0f, 10.0f, 0.0f, "Start CV A");
		configParam(END_CV_A_PARAM, 	-10.0f, 10.0f, 0.0f, "End CV A");
		configParam(DURATION_A_PARAM, 	-10.0f, 10.0f, 0.0f, "Duration A");
		configParam(SHAPE_A_PARAM, 	-1.0f, 1.0f, 0.0f, "Shape A");
		configParam(START_CV_B_PARAM, 	-10.0f, 10.0f, 0.0f, "Start CV B");
		configParam(END_CV_B_PARAM, 	-10.0f, 10.0f, 0.0f, "End CV B");
		configParam(DURATION_B_PARAM, 	-10.0f, 10.0f, 0.0f, "Duration B");
		configParam(SHAPE_B_PARAM, 	-1.0f, 1.0f, 0.0f, "Shape B");
		configParam(START_CV_C_PARAM, 	-10.0f, 10.0f, 0.0f, "Start CV C");
		configParam(END_CV_C_PARAM, 	-10.0f, 10.0f, 0.0f, "End CV C");
		configParam(DURATION_C_PARAM, 	-10.0f, 10.0f, 0.0f, "Duration C");
		configParam(SHAPE_C_PARAM, 	-1.0f, 1.0f, 0.0f, "Shape C");
		configParam(START_CV_D_PARAM, 	-10.0f, 10.0f, 0.0f, "Start CV D");
		configParam(END_CV_D_PARAM, 	-10.0f, 10.0f, 0.0f, "End CV D");
		configParam(DURATION_D_PARAM, 	-10.0f, 10.0f, 0.0f, "Duration D");
		configParam(SHAPE_D_PARAM, 	-1.0f, 1.0f, 0.0f, "Shape D");

		configInput(TRIG_INPUT, "Trig"); 
		configInput(START_TRIGGER_B_INPUT, "Start trigger B"); 
		configInput(END_TRIGGER_B_INPUT, "End trigger B"); 
		configInput(DURATION_GATE_B_INPUT, "Duration gate B"); 
		configInput(START_TRIGGER_C_INPUT, "Start trigger C"); 
		configInput(END_TRIGGER_C_INPUT, "End trigger C"); 
		configInput(DURATION_GATE_C_INPUT, "Duration gate C"); 
		configInput(START_TRIGGER_D_INPUT, "Start trigger D"); 
		configInput(END_TRIGGER_D_INPUT, "End trigger D"); 
		configInput(DURATION_GATE_D_INPUT, "Duration gate D"); 
		configInput(AUDIO_INPUT, "Audio"); 

		configOutput(START_TRIGGER_A_OUTPUT, "Start trigger A"); 
		configOutput(END_TRIGGER_A_OUTPUT, "End trigger A"); 
		configOutput(DURATION_GATE_A_OUTPUT, "Duration gate A"); 
		configOutput(ENVELOPE_OUTPUT, "Envelope"); 
		configOutput(AUDIO_OUTPUT, "Audio"); 
	}

// --------------------------------------------------

	int loop=0;     // save some CPU in process()

	// for managing the RESET and CLOCK signals
	float newReset=0.0f;
	float oldReset=0.0f;
	float newClock=0.0f;
	float oldClock=0.0f;
	bool hitClock=false;

	void process(const ProcessArgs& args) override {

		if (loop--<=0) {
			loop=9000;
			// save some more CPU
			for (int p=0;p<PARAMS_LEN;p++) {paramVal[p]=params[p].getValue();}
		}

		// let's see the reset signal
		// newReset=inputs[RESET_INPUT].getVoltage();
		if (newReset>2.0f && oldReset<=2.0f) {}
		// else if (newReset>2.0f && oldReset>2.0f) {}
		// else if (newReset<=2.0f && oldReset>2.0f) {}
		// else if (newReset<=2.0f && oldReset<=2.0f) {}
		oldReset=newReset;

		// let's see the clock signal
		hitClock=false;
		// newClock=inputs[CLOCK_INPUT].getVoltage();
		if (newClock>2.0f && oldClock<=2.0f) {hitClock=true;}
		// else if (newClock>2.0f && oldClock>2.0f) {}
		// else if (newClock<=2.0f && oldClock<=2.0f) {}
		// else if (newClock<=2.0f && oldClock>2.0f) {}
		oldClock=newClock;

	}

	// #include "MidTest/MidTest_json.hpp"

};

// --------------------------------------------------

struct MidTestWidget : ModuleWidget {

	MidTest* module;
	#include "share/widgetwizard.hpp"   // addChild abbreviations

	MidTestWidget(MidTest* module) {

		this->module = module;
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/MidTest.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		
		childKnob(MidTest::START_CV_A_PARAM, 1, HP*3, HP*13);
		childKnob(MidTest::END_CV_A_PARAM, 1, HP*3, HP*15.5);
		childKnob(MidTest::DURATION_A_PARAM, 1, HP*3, HP*18);
		childKnob(MidTest::SHAPE_A_PARAM, 1, HP*3, HP*20.5);
		childKnob(MidTest::START_CV_B_PARAM, 1, HP*5.5, HP*13);
		childKnob(MidTest::END_CV_B_PARAM, 1, HP*5.5, HP*15.5);
		childKnob(MidTest::DURATION_B_PARAM, 1, HP*5.5, HP*18);
		childKnob(MidTest::SHAPE_B_PARAM, 1, HP*5.5, HP*20.5);
		childKnob(MidTest::START_CV_C_PARAM, 1, HP*8, HP*13);
		childKnob(MidTest::END_CV_C_PARAM, 1, HP*8, HP*15.5);
		childKnob(MidTest::DURATION_C_PARAM, 1, HP*8, HP*18);
		childKnob(MidTest::SHAPE_C_PARAM, 1, HP*8, HP*20.5);
		childKnob(MidTest::START_CV_D_PARAM, 1, HP*10.5, HP*13);
		childKnob(MidTest::END_CV_D_PARAM, 1, HP*10.5, HP*15.5);
		childKnob(MidTest::DURATION_D_PARAM, 1, HP*10.5, HP*18);
		childKnob(MidTest::SHAPE_D_PARAM, 1, HP*10.5, HP*20.5);
		
		
		childPushbutton(MidTest::TRIGGER_PARAM, MidTest::LED_TRIGGER_PARAM, HP*2, HP*3);
		childInput(MidTest::TRIG_INPUT, HP*4.5, HP*3);
		childOutput(MidTest::ENVELOPE_OUTPUT, HP*7, HP*3);
		childInput(MidTest::AUDIO_INPUT, HP*9.5, HP*3);
		childOutput(MidTest::AUDIO_OUTPUT, HP*12, HP*3);

		childOutput(MidTest::START_TRIGGER_A_OUTPUT, HP*3, HP*5.5);
		childOutput(MidTest::END_TRIGGER_A_OUTPUT, HP*3, HP*8);
		childOutput(MidTest::DURATION_GATE_A_OUTPUT, HP*3, HP*10.5);
		childInput(MidTest::START_TRIGGER_B_INPUT, HP*5.5, HP*5.5);
		childInput(MidTest::END_TRIGGER_B_INPUT, HP*5.5, HP*8);
		childInput(MidTest::DURATION_GATE_B_INPUT, HP*5.5, HP*10.5);
		childInput(MidTest::START_TRIGGER_C_INPUT, HP*8, HP*5.5);
		childInput(MidTest::END_TRIGGER_C_INPUT, HP*8, HP*8);
		childInput(MidTest::DURATION_GATE_C_INPUT, HP*8, HP*10.5);
		childInput(MidTest::START_TRIGGER_D_INPUT, HP*10.5, HP*5.5);
		childInput(MidTest::END_TRIGGER_D_INPUT, HP*10.5, HP*8);
		childInput(MidTest::DURATION_GATE_D_INPUT, HP*10.5, HP*10.5);

		childLabel(HP*1,HP*1, "DEV", 12);
	}

	// #include "MidTest/MidTest_menu.hpp"

	// #include "MidTest/MidTest_keys.hpp"

};

Model* modelMidTest = createModel<MidTest, MidTestWidget>("MidTest");
