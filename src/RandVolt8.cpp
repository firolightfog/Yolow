// Copyright (c) 2022 András Szabó
#include "plugin.hpp"

struct RandVolt8 : Module {

// --------------------------------------------------

	enum ParamId    {
		RANGE_PARAM, BOTTOM_PARAM, PARAMS_LEN};

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
		configParam(RANGE_PARAM, 	-10.0f, 10.0f, 10.f, "Range V (upper)");
		configParam(BOTTOM_PARAM, 	-10.0f, 10.0f, 0.f, "Range V (lower)");
		paramQuantities[RANGE_PARAM]->snapEnabled = true;
		paramQuantities[BOTTOM_PARAM]->snapEnabled = true;

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

	bool clockIn=false;
	bool doItRarely=false;	// if no input is available then it's enough to send the voltage only rarely (not all cycle)
	
	float newVolt=0.0f;
	float allCv[8]={0.0f};
	
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
			if (newClock>2.0f && oldClock<=2.0f) {	// it is plugged in and there's a clock
				for (int o=0; o<8; o++) {
					allCv[o]=rack::random::uniform();
					allCv[o]=allCv[o]*abs(paramVal[RANGE_PARAM]-paramVal[BOTTOM_PARAM])+paramVal[BOTTOM_PARAM];
					outputs[OUT_1_OUTPUT+o].setVoltage(allCv[o]);
				}
			}
			else if (doItRarely==true) {	// it is plugged in but there's no clock
				doItRarely=false;
				for (int o=0; o<8; o++) {outputs[OUT_1_OUTPUT+o].setVoltage(allCv[o]);}
			}
			oldClock=newClock;
		}	
		else if (doItRarely==true) {	// it is not plugged in at all
			doItRarely=false;
			for (int o=0; o<8; o++) {
				allCv[o]=paramVal[BOTTOM_PARAM]+o*abs(paramVal[RANGE_PARAM]-paramVal[BOTTOM_PARAM])/8;
				outputs[OUT_1_OUTPUT+o].setVoltage(allCv[o]);
			}
		}
	}

	// this block is to save and reload a variable
	json_t* dataToJson() override {
	json_t* rootJ = json_object();
	json_t* cv_json_array = json_array();
	for (int cv_index=0; cv_index < 8; cv_index++) {
		json_array_append_new(cv_json_array , json_real(allCv[cv_index]));
	}	
	json_object_set_new(rootJ, "allCv", cv_json_array);
	return rootJ;}

	void dataFromJson(json_t* rootJ) override {
	json_t *cv_json_array = json_object_get(rootJ, "allCv");
	size_t cv_index;
	json_t *json_value;
	if (cv_json_array) {
		json_array_foreach(cv_json_array, cv_index, json_value)
		{allCv[cv_index] = json_number_value(json_value);}
	}
	}
	
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
		
	
		childOutput(RandVolt8::OUT_1_OUTPUT, HP*1, HP*2);
		childOutput(RandVolt8::OUT_2_OUTPUT, HP*1, HP*4);
		childOutput(RandVolt8::OUT_3_OUTPUT, HP*1, HP*6);
		childOutput(RandVolt8::OUT_4_OUTPUT, HP*1, HP*8);

		childKnob(RandVolt8::RANGE_PARAM, 0, HP*1, HP*10);
		childInput(RandVolt8::CLOCK_INPUT, HP*1, HP*12.5);
		childKnob(RandVolt8::BOTTOM_PARAM, 0, HP*1, HP*15);

		childOutput(RandVolt8::OUT_5_OUTPUT, HP*1, HP*17);
		childOutput(RandVolt8::OUT_6_OUTPUT, HP*1, HP*19);
		childOutput(RandVolt8::OUT_7_OUTPUT, HP*1, HP*21);
		childOutput(RandVolt8::OUT_8_OUTPUT, HP*1, HP*23);
	}

};

Model* modelRandVolt8 = createModel<RandVolt8, RandVolt8Widget>("RandVolt8");
