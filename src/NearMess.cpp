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

		configOutput(MIX_1TO9_OUTPUT, "10% A and 90% B output"); 
		configOutput(MIX_2TO8_OUTPUT, "20% A and 80% B output"); 
		configOutput(MIX_3TO7_OUTPUT, "30% A and 70% B output"); 
		configOutput(MIX_4TO6_OUTPUT, "40% A and 60% B output"); 
		configOutput(MIX_5TO5_OUTPUT, "50% A and 50% B output"); 
		configOutput(MIX_6TO4_OUTPUT, "60% A and 40% B output"); 
		configOutput(MIX_7TO3_OUTPUT, "70% A and 30% B output"); 
		configOutput(MIX_8TO2_OUTPUT, "80% A and 20% B output"); 
		configOutput(MIX_9TO1_OUTPUT, "90% A and 10% B output"); 
	}

// --------------------------------------------------

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
	
		childInput(NearMess::A_INPUT, HP*1, HP*2);
		childOutput(NearMess::MIX_9TO1_OUTPUT, HP*1, HP*4.5);
		childOutput(NearMess::MIX_8TO2_OUTPUT, HP*1, HP*6.5);
		childOutput(NearMess::MIX_7TO3_OUTPUT, HP*1, HP*8.5);
		childOutput(NearMess::MIX_6TO4_OUTPUT, HP*1, HP*10.5);
		childOutput(NearMess::MIX_5TO5_OUTPUT, HP*1, HP*12.5);
		childOutput(NearMess::MIX_4TO6_OUTPUT, HP*1, HP*14.5);
		childOutput(NearMess::MIX_3TO7_OUTPUT, HP*1, HP*16.5);
		childOutput(NearMess::MIX_2TO8_OUTPUT, HP*1, HP*18.5);
		childOutput(NearMess::MIX_1TO9_OUTPUT, HP*1, HP*20.5);
		childInput(NearMess::B_INPUT, HP*1, HP*23);

	}

};

Model* modelNearMess = createModel<NearMess, NearMessWidget>("NearMess");

/*--------------------------------------------------*/
// Cousine module
/*--------------------------------------------------*/


struct NearMix : Module {

// --------------------------------------------------

	enum ParamId    {
		DEBUG_PARAM, PARAMS_LEN};

	enum InputId    {
		MIX_1TO9_INPUT, MIX_2TO8_INPUT, MIX_3TO7_INPUT, MIX_4TO6_INPUT, MIX_5TO5_INPUT, 
		MIX_6TO4_INPUT, MIX_7TO3_INPUT, MIX_8TO2_INPUT, MIX_9TO1_INPUT, INPUTS_LEN};

	enum OutputId    {
		A_OUTPUT, B_OUTPUT, OUTPUTS_LEN};

	enum LightId    {
		LED_DEBUG_PARAM, LIGHTS_LEN};

// --------------------------------------------------

	NearMix() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(DEBUG_PARAM, 	0.0f, 1.0f, 1.0f, "Debug");

		configOutput(A_OUTPUT, "A"); 
		configOutput(B_OUTPUT, "B"); 

		configInput(MIX_1TO9_INPUT, "Mix 1to9"); 
		configInput(MIX_2TO8_INPUT, "Mix 2to8"); 
		configInput(MIX_3TO7_INPUT, "Mix 3to7"); 
		configInput(MIX_4TO6_INPUT, "Mix 4to6"); 
		configInput(MIX_5TO5_INPUT, "Mix 5to5"); 
		configInput(MIX_6TO4_INPUT, "Mix 6to4"); 
		configInput(MIX_7TO3_INPUT, "Mix 7to3"); 
		configInput(MIX_8TO2_INPUT, "Mix 8to2"); 
		configInput(MIX_9TO1_INPUT, "Mix 9to1"); 
	}

// --------------------------------------------------

	float inVolts[9]={0};
	float outA=0;
	float outB=0;
	
	void process(const ProcessArgs& args) override {

		for (int i=0; i<9; i++) {
			inVolts[i]=inputs[MIX_1TO9_INPUT+i].getVoltage();			
		}
	
		outB=inVolts[0]*0.9f;
		outB+=inVolts[1]*0.8f;
		outB+=inVolts[2]*0.7f;
		outB+=inVolts[3]*0.6f;
		outB+=inVolts[4]*0.5f;
		outB+=inVolts[5]*0.4f;
		outB+=inVolts[6]*0.3f;
		outB+=inVolts[7]*0.2f;
		outB+=inVolts[8]*0.1f;
		outputs[B_OUTPUT].setVoltage(outB); 

		outA=inVolts[0]*0.1f;
		outA+=inVolts[1]*0.2f;
		outA+=inVolts[2]*0.3f;
		outA+=inVolts[3]*0.4f;
		outA+=inVolts[4]*0.5f;
		outA+=inVolts[5]*0.6f;
		outA+=inVolts[6]*0.7f;
		outA+=inVolts[7]*0.8f;
		outA+=inVolts[8]*0.9f;
		outputs[A_OUTPUT].setVoltage(outA); 

	}

};

// --------------------------------------------------

struct NearMixWidget : ModuleWidget {

	NearMix* module;
	#include "share/widgetwizard.hpp"   // addChild abbreviations

	NearMixWidget(NearMix* module) {

		this->module = module;
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/NearMix.svg")));
	
		childOutput(NearMix::A_OUTPUT, HP*1, HP*2);
		childInput(NearMix::MIX_9TO1_INPUT, HP*1, HP*4.5);
		childInput(NearMix::MIX_8TO2_INPUT, HP*1, HP*6.5);
		childInput(NearMix::MIX_7TO3_INPUT, HP*1, HP*8.5);
		childInput(NearMix::MIX_6TO4_INPUT, HP*1, HP*10.5);
		childInput(NearMix::MIX_5TO5_INPUT, HP*1, HP*12.5);
		childInput(NearMix::MIX_4TO6_INPUT, HP*1, HP*14.5);
		childInput(NearMix::MIX_3TO7_INPUT, HP*1, HP*16.5);
		childInput(NearMix::MIX_2TO8_INPUT, HP*1, HP*18.5);
		childInput(NearMix::MIX_1TO9_INPUT, HP*1, HP*20.5);
		childOutput(NearMix::B_OUTPUT, HP*1, HP*23);

	}

};

Model* modelNearMix = createModel<NearMix, NearMixWidget>("NearMix");
