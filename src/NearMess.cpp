// Copyright (c) 2022 András Szabó
#include "plugin.hpp"

struct NearMess : Module {

// --------------------------------------------------

	enum ParamId    {
		DEBUG_PARAM, PARAMS_LEN};

	enum InputId    {
		A_INPUT, B_INPUT, INPUTS_LEN};

	enum OutputId    {
		MIX_1TO9_OUTPUT, MIX_2TO8_OUTPUT, MIX_3TO7_OUTPUT, MIX_4TO6_OUTPUT, MIX_5TO5_OUTPUT, 
		MIX_6TO4_OUTPUT, MIX_7TO3_OUTPUT, MIX_8TO2_OUTPUT, MIX_9TO1_OUTPUT, OUTPUTS_LEN};

	enum LightId    {
		LED_DEBUG_PARAM, LIGHTS_LEN};

// --------------------------------------------------

	NearMess() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(DEBUG_PARAM, 	0.0f, 1.0f, 1.0f, "Debug");

		configInput(A_INPUT, "A"); 
		configInput(B_INPUT, "B"); 

		configOutput(MIX_1TO9_OUTPUT, "Mix 1to9"); 
		configOutput(MIX_2TO8_OUTPUT, "Mix 2to8"); 
		configOutput(MIX_3TO7_OUTPUT, "Mix 3to7"); 
		configOutput(MIX_4TO6_OUTPUT, "Mix 4to6"); 
		configOutput(MIX_5TO5_OUTPUT, "Mix 5to5"); 
		configOutput(MIX_6TO4_OUTPUT, "Mix 6to4"); 
		configOutput(MIX_7TO3_OUTPUT, "Mix 7to3"); 
		configOutput(MIX_8TO2_OUTPUT, "Mix 8to2"); 
		configOutput(MIX_9TO1_OUTPUT, "Mix 9to1"); 
	}

// --------------------------------------------------

	int loop=0;     // save some CPU in process()
	float inA=0;
	float inB=0;
	
	void process(const ProcessArgs& args) override {
	
		inA=inputs[A_INPUT].getVoltage();
		inB=inputs[B_INPUT].getVoltage();

		outputs[MIX_1TO9_OUTPUT].setVoltage(inA*0.1f+inB*0.9f); 
		outputs[MIX_2TO8_OUTPUT].setVoltage(inA*0.2f+inB*0.8f); 
		outputs[MIX_3TO7_OUTPUT].setVoltage(inA*0.3f+inB*0.7f); 
		outputs[MIX_4TO6_OUTPUT].setVoltage(inA*0.4f+inB*0.6f); 
		outputs[MIX_5TO5_OUTPUT].setVoltage(inA*0.5f+inB*0.5f); 
		outputs[MIX_6TO4_OUTPUT].setVoltage(inA*0.6f+inB*0.4f); 
		outputs[MIX_7TO3_OUTPUT].setVoltage(inA*0.7f+inB*0.3f); 
		outputs[MIX_8TO2_OUTPUT].setVoltage(inA*0.8f+inB*0.2f); 
		outputs[MIX_9TO1_OUTPUT].setVoltage(inA*0.9f+inB*0.1f); 

	}

};

// --------------------------------------------------

struct NearMessWidget : ModuleWidget {

	NearMess* module;
	#include "share/widgetwizard.hpp"   // addChild abbreviations

	NearMessWidget(NearMess* module) {

		this->module = module;
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/NearMess.svg")));

		// addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		
		childInput(NearMess::A_INPUT, HP*1, HP*2);
		childOutput(NearMess::MIX_1TO9_OUTPUT, HP*1, HP*4.5);
		childOutput(NearMess::MIX_2TO8_OUTPUT, HP*1, HP*6.5);
		childOutput(NearMess::MIX_3TO7_OUTPUT, HP*1, HP*8.5);
		childOutput(NearMess::MIX_4TO6_OUTPUT, HP*1, HP*10.5);
		childOutput(NearMess::MIX_5TO5_OUTPUT, HP*1, HP*12.5);
		childOutput(NearMess::MIX_6TO4_OUTPUT, HP*1, HP*14.5);
		childOutput(NearMess::MIX_7TO3_OUTPUT, HP*1, HP*16.5);
		childOutput(NearMess::MIX_8TO2_OUTPUT, HP*1, HP*18.5);
		childOutput(NearMess::MIX_9TO1_OUTPUT, HP*1, HP*20.5);
		childInput(NearMess::B_INPUT, HP*1, HP*23);
	}

};

Model* modelNearMess = createModel<NearMess, NearMessWidget>("NearMess");
