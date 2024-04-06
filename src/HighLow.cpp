// Copyright (c) 2023 András Szabó
#include "plugin.hpp"

struct HighLow : Module {

// --------------------------------------------------

	enum ParamId  {
		HIGH_A_PARAM, LOW_A_PARAM, HIGH_B_PARAM, LOW_B_PARAM, LATCH_PARAM, PARAMS_LEN};

	enum InputId  {
		POLY_HIGH_AND_LOW_INPUT, GATE_INPUT, INPUTS_LEN};

	enum OutputId {
		OUT_A_OUTPUT, OUT_B_OUTPUT, OUT_C_OUTPUT, OUTPUTS_LEN};

	enum LightId  {LIGHTS_LEN};

	// small assistance to save older values for reference;
	float paramVal[PARAMS_LEN]={0};
	// float inputVolt[INPUTS_LEN]={0};
	// float lightVal[LIGHTS_LEN]={0};

// --------------------------------------------------

	HighLow() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(HIGH_A_PARAM, 	-10.0f, 10.0f, 5.0f, "High A");
		configParam(LOW_A_PARAM, 	-10.0f, 10.0f, -5.0f, "Low A");
		configParam(HIGH_B_PARAM, 	-10.0f, 10.0f, 5.0f, "High B");
		configParam(LOW_B_PARAM, 	-10.0f, 10.0f, -5.0f, "Low B");
		configParam(LATCH_PARAM, 	0.0f, 1.0f, 0.0f, "Latch");

		configInput(POLY_HIGH_AND_LOW_INPUT, "Poly High and Low"); 
		configInput(GATE_INPUT, "Gate"); 

		configOutput(OUT_A_OUTPUT, "Out A"); 
		configOutput(OUT_B_OUTPUT, "Out B"); 
		configOutput(OUT_C_OUTPUT, "Out C"); 
	}

// --------------------------------------------------

	float newV=0;

	void process(const ProcessArgs& args) override {

		if (inputs[GATE_INPUT].getVoltage()>2.0f || params[LATCH_PARAM].getValue()>0.5) {
			outputs[OUT_A_OUTPUT].setVoltage(params[HIGH_A_PARAM].getValue());			
			outputs[OUT_B_OUTPUT].setVoltage(params[HIGH_B_PARAM].getValue());
			newV=-10;
			for (int i=0;i<inputs[POLY_HIGH_AND_LOW_INPUT].channels;i++) {
				if (inputs[POLY_HIGH_AND_LOW_INPUT].getVoltage(i)>newV) {
					newV=inputs[POLY_HIGH_AND_LOW_INPUT].getVoltage(i);
				}
			};
			outputs[OUT_C_OUTPUT].setVoltage(newV);			
		}
		else {
			outputs[OUT_A_OUTPUT].setVoltage(params[LOW_A_PARAM].getValue());			
			outputs[OUT_B_OUTPUT].setVoltage(params[LOW_B_PARAM].getValue());			
			newV=10;
			for (int i=0;i<inputs[POLY_HIGH_AND_LOW_INPUT].channels;i++) {
				if (inputs[POLY_HIGH_AND_LOW_INPUT].getVoltage(i)<newV) {
					newV=inputs[POLY_HIGH_AND_LOW_INPUT].getVoltage(i);
				}
			};
			outputs[OUT_C_OUTPUT].setVoltage(newV);
		}

	}

	// #include "HighLow/HighLow_json.hpp"

};

// --------------------------------------------------

struct HighLowWidget : ModuleWidget {

	HighLow* module;
	#include "share/widgetwizard.hpp"   // addChild abbreviations

	HighLowWidget(HighLow* module) {

		this->module = module;
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/HighLow.svg")));

		// addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		
		childKnob(HighLow::HIGH_A_PARAM, 0, HP*1, HP*2.5);
		childKnob(HighLow::LOW_A_PARAM, 0, HP*1, HP*4.5);
		childOutput(HighLow::OUT_A_OUTPUT, HP*1, HP*6.5);

		childKnob(HighLow::HIGH_B_PARAM, 0, HP*1, HP*9);
		childKnob(HighLow::LOW_B_PARAM, 0, HP*1, HP*11);
		childOutput(HighLow::OUT_B_OUTPUT, HP*1, HP*13);

		childInput(HighLow::POLY_HIGH_AND_LOW_INPUT, HP*1, HP*15.5);
		childOutput(HighLow::OUT_C_OUTPUT, HP*1, HP*17.5);

		childSwitch(HighLow::LATCH_PARAM, 0, HP*1, HP*21);
		childInput(HighLow::GATE_INPUT, HP*1, HP*23);

		// childLabel(HP*1,HP*1, "DEV", 12);
	}

	// #include "HighLow/HighLow_menu.hpp"

	// #include "HighLow/HighLow_keys.hpp"

};

Model* modelHighLow = createModel<HighLow, HighLowWidget>("HighLow");
