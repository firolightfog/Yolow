// Copyright (c) 2022 András Szabó
#include "plugin.hpp"

struct Celei : Module {

// --------------------------------------------------

	enum ParamId    {
		SEQ_1_VOLTAGE_PARAM, SEQ_2_VOLTAGE_PARAM, SEQ_3_VOLTAGE_PARAM, SEQ_4_VOLTAGE_PARAM, SEQ_5_VOLTAGE_PARAM, 
		SEQ_6_VOLTAGE_PARAM, SEQ_7_VOLTAGE_PARAM, SEQ_8_VOLTAGE_PARAM, STEPS_PARAM, OCTAVE_PARAM, 
		RANGE_PARAM, TRIGGER_STEP_PARAM, PARAMS_LEN};

	enum InputId    {
		TRANSPOSE_INPUT, CLOCK_INPUT, RESET_INPUT, INPUTS_LEN};

	enum OutputId    {
		MONO_OUTPUT, TRIGGER_STEP_OUTPUT, OUTPUTS_LEN};
	
	enum LightId    {
		SEQ_1_LED_LIGHT, SEQ_2_LED_LIGHT, SEQ_3_LED_LIGHT, SEQ_4_LED_LIGHT, SEQ_5_LED_LIGHT, 
		SEQ_6_LED_LIGHT, SEQ_7_LED_LIGHT, SEQ_8_LED_LIGHT, LIGHTS_LEN};

	// small assistance to save older values for reference;
	float paramVal[PARAMS_LEN]={0};
	float inputVolt[INPUTS_LEN]={0};
	float lightVal[LIGHTS_LEN]={0};

// --------------------------------------------------

	Celei() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(SEQ_1_VOLTAGE_PARAM, 	0.0f, 1.0f, 0.0f, "Seq 1 voltage");
		configParam(SEQ_2_VOLTAGE_PARAM, 	0.0f, 1.0f, 0.0f, "Seq 2 voltage");
		configParam(SEQ_3_VOLTAGE_PARAM, 	0.0f, 1.0f, 0.0f, "Seq 3 voltage");
		configParam(SEQ_4_VOLTAGE_PARAM, 	0.0f, 1.0f, 0.0f, "Seq 4 voltage");
		configParam(SEQ_5_VOLTAGE_PARAM, 	0.0f, 1.0f, 0.0f, "Seq 5 voltage");
		configParam(SEQ_6_VOLTAGE_PARAM, 	0.0f, 1.0f, 0.0f, "Seq 6 voltage");
		configParam(SEQ_7_VOLTAGE_PARAM, 	0.0f, 1.0f, 0.0f, "Seq 7 voltage");
		configParam(SEQ_8_VOLTAGE_PARAM, 	0.0f, 1.0f, 0.0f, "Seq 8 voltage");
		configParam(STEPS_PARAM, 			1.0f, 8.0f, 8.0f, "Steps");
		configParam(OCTAVE_PARAM, 			-5.0f, 5.0f, 0.0f, "Octave");
		configParam(RANGE_PARAM, 			1.0f, 7.0f, 1.0f, "Range");
		configParam(TRIGGER_STEP_PARAM, 	1.0f, 8.0f, 1.0f, "Trigger step");
		paramQuantities[STEPS_PARAM]->snapEnabled = true;
		paramQuantities[OCTAVE_PARAM]->snapEnabled = true;
		paramQuantities[RANGE_PARAM]->snapEnabled = true;
		paramQuantities[TRIGGER_STEP_PARAM]->snapEnabled = true;

		configInput(TRANSPOSE_INPUT, "Transpose"); 
		configInput(CLOCK_INPUT, "Clock"); 
		configInput(RESET_INPUT, "Reset"); 

		configOutput(MONO_OUTPUT, "Mono"); 
		configOutput(TRIGGER_STEP_OUTPUT, "Trigger step"); 
	}

// --------------------------------------------------

	int loop=0;     // save some CPU in process()
	int stepA=0;	// position of current step (ie. 1-8)
	float voltA=0.0f;	// calculated voltage output
	
	// for managing the RESET and CLOCK signals
	float newReset=0.0f;
	float oldReset=0.0f;
	float newClock=0.0f;
	float oldClock=0.0f;

	// storing parameter positions occaisonly
	float pSx[8]={0.0f};
	float pSteps=0.0f;
	float pOct=0.0f;
	float pRng=0.0f;
	float pTrg=0.0f;
	bool iTrans=false;	// any transpose cable available?

	void process(const ProcessArgs& args) override {

		if (loop--<=0) {
			loop=9000;
			// save some more CPU
			pSteps=params[STEPS_PARAM].getValue(); 
			pOct=params[OCTAVE_PARAM].getValue();
			pRng=params[RANGE_PARAM].getValue();
			pTrg=params[TRIGGER_STEP_PARAM].getValue();			
			iTrans=inputs[TRANSPOSE_INPUT].isConnected();
			pSx[0]=params[0+SEQ_1_VOLTAGE_PARAM].getValue();
			pSx[1]=params[1+SEQ_1_VOLTAGE_PARAM].getValue();
			pSx[2]=params[2+SEQ_1_VOLTAGE_PARAM].getValue();
			pSx[3]=params[3+SEQ_1_VOLTAGE_PARAM].getValue();
			pSx[4]=params[4+SEQ_1_VOLTAGE_PARAM].getValue();
			pSx[5]=params[5+SEQ_1_VOLTAGE_PARAM].getValue();
			pSx[6]=params[6+SEQ_1_VOLTAGE_PARAM].getValue();
			pSx[7]=params[7+SEQ_1_VOLTAGE_PARAM].getValue();		
		}

		// let's see the reset signal
		newReset=inputs[RESET_INPUT].getVoltage();
		if (newReset>0.2f && oldReset<=0.2f) {
			lights[SEQ_1_LED_LIGHT-1+stepA].setBrightness(0);
			lights[SEQ_1_LED_LIGHT].setBrightness(10);
			stepA=0; 
			}
		// else if (newReset>0.2 && oldReset>0.2) {}
		// else if (newReset<=0.2 && oldReset>0.2) {}
		// else if (newReset<=0.2 && oldReset<=0.2) {}
		oldReset=newReset;

		// let's see the clock signal
		newClock=inputs[CLOCK_INPUT].getVoltage();
		if (newClock>0.2f && oldClock<=0.2f) {			
			// take the next step
			lights[SEQ_1_LED_LIGHT-1+stepA].setBrightness(0);
			stepA++; 
			if (stepA>pSteps || stepA<1) {stepA=1;}
			if (stepA==pTrg) {outputs[TRIGGER_STEP_OUTPUT].setVoltage(10);}
			lights[SEQ_1_LED_LIGHT-1+stepA].setBrightness(10);
		}
		// else if (newClock>0.2 && oldClock>0.2) {}
		// else if (newClock<=0.2 && oldClock<=0.2) {}
		else if (newClock<=0.2 && oldClock>0.2) {outputs[TRIGGER_STEP_OUTPUT].setVoltage(0);}
		oldClock=newClock;

		// voltA=params[SEQ_1_VOLTAGE_PARAM-1+stepA].getValue();
		voltA=pSx[stepA-1];
		voltA=voltA*pRng+pOct;
		if (iTrans) {
			voltA=voltA+remainder(inputs[TRANSPOSE_INPUT].getVoltage(), 10.0f);
		};	
		outputs[MONO_OUTPUT].setVoltage(voltA);

	}

// --------------------------------------------------

	// spaceholder for JSON

// --------------------------------------------------

};

// --------------------------------------------------

struct CeleiWidget : ModuleWidget {

	Celei* module;
	#include "share/widgetwizard.hpp"   // addChild abbreviations

	CeleiWidget(Celei* module) {

		this->module = module;
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/Celei.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		
		childKnob(Celei::SEQ_1_VOLTAGE_PARAM, 1, HP*4.5, HP*5.5);
		childKnob(Celei::SEQ_2_VOLTAGE_PARAM, 1, HP*4.5, HP*8);
		childKnob(Celei::SEQ_3_VOLTAGE_PARAM, 1, HP*4.5, HP*10.5);
		childKnob(Celei::SEQ_4_VOLTAGE_PARAM, 1, HP*4.5, HP*13);
		childKnob(Celei::SEQ_5_VOLTAGE_PARAM, 1, HP*4.5, HP*15.5);
		childKnob(Celei::SEQ_6_VOLTAGE_PARAM, 1, HP*4.5, HP*18);
		childKnob(Celei::SEQ_7_VOLTAGE_PARAM, 1, HP*4.5, HP*20.5);
		childKnob(Celei::SEQ_8_VOLTAGE_PARAM, 1, HP*4.5, HP*23);
		childKnob(Celei::STEPS_PARAM, 0, HP*4.5, HP*2.5);
		childKnob(Celei::OCTAVE_PARAM, 0, HP*1.5, HP*10.5);
		childKnob(Celei::RANGE_PARAM, 0, HP*1.5, HP*13);
		childKnob(Celei::TRIGGER_STEP_PARAM, 0, HP*1.5, HP*15.5);
		
		childLight(Celei::SEQ_1_LED_LIGHT, 0, HP*4.5, HP*5.5);
		childLight(Celei::SEQ_2_LED_LIGHT, 0, HP*4.5, HP*8);
		childLight(Celei::SEQ_3_LED_LIGHT, 0, HP*4.5, HP*10.5);
		childLight(Celei::SEQ_4_LED_LIGHT, 0, HP*4.5, HP*13);
		childLight(Celei::SEQ_5_LED_LIGHT, 0, HP*4.5, HP*15.5);
		childLight(Celei::SEQ_6_LED_LIGHT, 0, HP*4.5, HP*18);
		childLight(Celei::SEQ_7_LED_LIGHT, 0, HP*4.5, HP*20.5);
		childLight(Celei::SEQ_8_LED_LIGHT, 0, HP*4.5, HP*23);
		childInput(Celei::TRANSPOSE_INPUT, HP*1.5, HP*5.5);
		childOutput(Celei::MONO_OUTPUT, HP*1.5, HP*8);
		childOutput(Celei::TRIGGER_STEP_OUTPUT, HP*1.5, HP*18);
		childInput(Celei::CLOCK_INPUT, HP*1.5, HP*20.5);
		childInput(Celei::RESET_INPUT, HP*1.5, HP*23);
	}

};

Model* modelCelei = createModel<Celei, CeleiWidget>("Celei");
