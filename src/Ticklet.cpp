// Copyright (c) 2022 András Szabó

#include "plugin.hpp"

struct Ticklet : Module {

// --------------------------------------------------

	enum ParamId    {ENUMS(DIV_PARAM,8), RESET_PARAM, PARAMS_LEN};
	enum InputId    {CLOCK_INPUT, RESET_INPUT, INPUTS_LEN};
	enum OutputId   {POLY_OUTPUT, ENUMS(DIV_OUTPUT,8), OUTPUTS_LEN};
	enum LightId    {LIGHTS_LEN};

// --------------------------------------------------

	Ticklet() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		
		configParam(RESET_PARAM, 0.0f, 1.0f, 0.0f, "Reset");
		for (int i=0; i<8; i++) {
			configParam(DIV_PARAM+i, 1.0f, 32.0f, 4.0f, "Division");
			paramQuantities[DIV_PARAM+i]->snapEnabled = true;
			paramQuantities[DIV_PARAM+i]->randomizeEnabled = true;
			configOutput(DIV_OUTPUT+i, "Division " + std::to_string(i)); 
		}	

		configInput(CLOCK_INPUT, "Clock"); 
		configInput(RESET_INPUT, "Reset"); 
		configOutput(POLY_OUTPUT, "Polyphonic clock"); 
	}

// --------------------------------------------------

	int loop=0;     // save some CPU in process()

	// for managing the RESET and CLOCK signals
	float newReset=0.0f;
	float oldReset=0.0f;
	float newClock=0.0f;
	float oldClock=0.0f;
	float paramVal[PARAMS_LEN]={0};
	int calcIt[8]={0};
	
	// Ctrl-E bypassing: no sound but the counting stays
	void processBypass(const ProcessArgs& args) override {
	}

	void process(const ProcessArgs& args) override {

		if (loop--<=0) {
			loop=9003;
			// save some more CPU
			for (int p=0; p<PARAMS_LEN; p++) {paramVal[p]=params[p].getValue();}
			outputs[POLY_OUTPUT].channels=8;
			if (paramVal[RESET_PARAM]==1) {
				// paramVal[RESET_PARAM]=0;
				for (int i=0; i<8; i++) {calcIt[i]=0;}
			}
		}

		// let's see the reset signal
		newReset=inputs[RESET_INPUT].getVoltage();
		if (newReset>2.0f && oldReset<=2.0f) {
			for (int i=0; i<8; i++) {calcIt[i]=-1;}
		}
		oldReset=newReset;

		// let's see the clock signal
		newClock=inputs[CLOCK_INPUT].getVoltage();
		if (newClock>2.0f && oldClock<=2.0f) {
			for (int i=0; i<8; i++) {
				calcIt[i]++;
				// outputs[DIV_OUTPUT+i].setVoltage(calcIt[i]+paramVal[DIV_PARAM+i]/10);
				if (calcIt[i]>=paramVal[DIV_PARAM+i]) {
					calcIt[i]=0;
				}
				if (calcIt[i]==0) {
					outputs[POLY_OUTPUT].setVoltage(10,i);
					outputs[DIV_OUTPUT+i].setVoltage(10);
				}
			}
		}
		else if (newClock<=2.0f && oldClock>2.0f) {
			for (int i=0; i<8; i++) {
				outputs[POLY_OUTPUT].setVoltage(0,i);
				outputs[DIV_OUTPUT+i].setVoltage(0);
			}
		}
		oldClock=newClock;

	}

};

// --------------------------------------------------

struct TickletWidget : ModuleWidget {

	Ticklet* module;
	#include "share/widgetwizard.hpp"   // addChild abbreviations

	TickletWidget(Ticklet* module) {

		this->module = module;
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/Ticklet.svg")));

		// addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		
		childInput(Ticklet::CLOCK_INPUT, HP*1, HP*2.5);
		childInput(Ticklet::RESET_INPUT, HP*3, HP*2.5);		
		addParam(createParamCentered<TL1105>(mm2px(Vec(HP*2, HP*4)), module, Ticklet::RESET_PARAM));

		childOutput(Ticklet::POLY_OUTPUT, HP*2, HP*6.5);
		for (int i=0; i<8; i++) {
			if ((i % 2)==0) {
				addOutput(createOutputCentered<PJ301MPort>
					(mm2px(Vec(HP*3, HP*8.5+HP*i*2)), module, Ticklet::DIV_OUTPUT+i));
				addParam(createParamCentered<RoundSmallBlackKnob> 
					(mm2px(Vec(HP*1, HP*8.5+HP*i*2)), module, Ticklet::DIV_PARAM+i));				
			}
			else {
				// childOutput(Ticklet::DIV_OUTPUT+i, HP*1, HP*9+HP*i*2);
				addOutput(createOutputCentered<PJ301MPort>
					(mm2px(Vec(HP*1, HP*8.5+HP*i*2)), module, Ticklet::DIV_OUTPUT+i));
				addParam(createParamCentered<RoundSmallBlackKnob> 
					(mm2px(Vec(HP*3, HP*8.5+HP*i*2)), module, Ticklet::DIV_PARAM+i));								
			}
		}
	
	}
	
};

Model* modelTicklet = createModel<Ticklet, TickletWidget>("Ticklet");