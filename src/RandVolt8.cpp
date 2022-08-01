// Copyright (c) 2022 András Szabó
#include "plugin.hpp"

struct RandVolt8 : Module {

// --------------------------------------------------

	enum ParamId    {
		UNI_PARAM, RANGE_PARAM, PARAMS_LEN};

	enum InputId    {
		CLOCK_INPUT, INPUTS_LEN};

	enum OutputId    {
		OUT_1_OUTPUT, OUT_2_OUTPUT, OUT_3_OUTPUT, OUT_4_OUTPUT, OUT_5_OUTPUT, 
		OUT_6_OUTPUT, OUT_7_OUTPUT, OUT_8_OUTPUT, OUTPUTS_LEN};

	enum LightId    {LIGHTS_LEN};

	// small assistance to save older values for reference;
	float paramVal[PARAMS_LEN]={0};
	// float inputVolt[INPUTS_LEN]={0};
	// float lightVal[LIGHTS_LEN]={0};

// --------------------------------------------------

	RandVolt8() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(UNI_PARAM, 	0.0f, 1.0f, 1.0f, "0: Bi, 1:Uni");
		configParam(RANGE_PARAM, 	1.0f, 10.0f, 10.f, "Range V (upper)");
		paramQuantities[UNI_PARAM]->snapEnabled = true;
		paramQuantities[RANGE_PARAM]->snapEnabled = true;

		configInput(CLOCK_INPUT, "Clock"); 

		configOutput(OUT_1_OUTPUT, "Out 1"); 
		configOutput(OUT_2_OUTPUT, "Out 2"); 
		configOutput(OUT_3_OUTPUT, "Out 3"); 
		configOutput(OUT_4_OUTPUT, "Out 4"); 
		configOutput(OUT_5_OUTPUT, "Out 5"); 
		configOutput(OUT_6_OUTPUT, "Out 6"); 
		configOutput(OUT_7_OUTPUT, "Out 7"); 
		configOutput(OUT_8_OUTPUT, "Out 8"); 
	}

// --------------------------------------------------

	int loop=0;     // save some CPU in process()

	// for managing the CLOCK signals
	float newClock=0.0f;
	float oldClock=0.0f;
	bool doItRarely=false;
	float newVolt=0.0f;
	bool clockIn=false;

	void process(const ProcessArgs& args) override {

		if (loop--<=0) {
			loop=9000;
			for (int p=0;p<PARAMS_LEN;p++) {paramVal[p]=params[p].getValue();}
			clockIn=inputs[CLOCK_INPUT].isConnected();
			doItRarely=true;
			// save some more CPU
		}

		// let's see the clock signal
		if (clockIn) {		
		
			newClock=inputs[CLOCK_INPUT].getVoltage();
			if (newClock>0.2f && oldClock<=0.2f) {
				for (int o=OUT_1_OUTPUT; o<=OUT_8_OUTPUT; o++) {
					newVolt=rack::random::uniform();
					newVolt=newVolt*paramVal[RANGE_PARAM];
					if (paramVal[UNI_PARAM]==0) {
						newVolt=newVolt*2;
						newVolt=newVolt-paramVal[RANGE_PARAM];
					}
					outputs[o].setVoltage(newVolt);
				}
			}
			oldClock=newClock;
		}
		
		else if (doItRarely==true) {
			doItRarely=false;
			for (int o=0; o<8; o++) {
				outputs[OUT_1_OUTPUT+o].setVoltage(10-o);
			}
		}

	}

// --------------------------------------------------

};

// --------------------------------------------------

struct RandVolt8Widget : ModuleWidget {

	RandVolt8* module;
	#include "share/widgetwizard.hpp"   // addChild abbreviations

	RandVolt8Widget(RandVolt8* module) {

		this->module = module;
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/RandVolt8.svg")));

		// addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		
		childSwitch(RandVolt8::UNI_PARAM, 0, HP*1, HP*15);
		childKnob(RandVolt8::RANGE_PARAM, 0, HP*1, HP*10);
		
		childOutput(RandVolt8::OUT_1_OUTPUT, HP*1, HP*2);
		childOutput(RandVolt8::OUT_2_OUTPUT, HP*1, HP*4);
		childOutput(RandVolt8::OUT_3_OUTPUT, HP*1, HP*6);
		childOutput(RandVolt8::OUT_4_OUTPUT, HP*1, HP*8);
		childInput(RandVolt8::CLOCK_INPUT, HP*1, HP*13);
		childOutput(RandVolt8::OUT_5_OUTPUT, HP*1, HP*17);
		childOutput(RandVolt8::OUT_6_OUTPUT, HP*1, HP*19);
		childOutput(RandVolt8::OUT_7_OUTPUT, HP*1, HP*21);
		childOutput(RandVolt8::OUT_8_OUTPUT, HP*1, HP*23);
	}

};

Model* modelRandVolt8 = createModel<RandVolt8, RandVolt8Widget>("RandVolt8");
