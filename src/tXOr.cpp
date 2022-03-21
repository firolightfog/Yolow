/* README.md

### RandVolt10

This module is a trigger sequencer with three clock dividers and a XOR output. 

##### KNOBS: 
- `DIV A` setting the first clock division 
- `DIV B` setting the second clock division 
- `DIV C` setting the third clock division 
##### INPUTS: a trigger input
- `CLK` default clock input
- `RST` common reset fuction to resart calculation
- `DIV A` overwritting the relevant knob settings and defining the clock division
- `DIV B` overwritting the relevant knob settings and defining the clock division
- `DIV C` overwritting the relevant knob settings and defining the clock division
##### OUTPUTS:
- `OUT` XOR output of the three divisions 

*/


#include "plugin.hpp"

struct tXOr : Module {
	enum ParamId {
		MONOIN1_PARAM,
		MONOIN3_PARAM,
		MONOIN5_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		MONOIN7_INPUT,
		MONOIN8_INPUT,
		MONOIN2_INPUT,
		MONOIN4_INPUT,
		MONOIN6_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		POLYOUT_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		DIVA_LIGHT,
		DIVB_LIGHT,
		DIVC_LIGHT,
		LIGHTS_LEN
	};

	tXOr() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(MONOIN1_PARAM, 0.f, 64.f, 1.f, "DivA knob","");
		configParam(MONOIN3_PARAM, 0.f, 64.f, 0.f, "DivB knob","");
		configParam(MONOIN5_PARAM, 0.f, 64.f, 0.f, "DivC knob","");
		paramQuantities[MONOIN1_PARAM]->snapEnabled = true;
		paramQuantities[MONOIN3_PARAM]->snapEnabled = true;
		paramQuantities[MONOIN5_PARAM]->snapEnabled = true;
		// paramQuantities[MONOIN1_PARAM]->displayMultiplier=6.4;	// 64/10
		// paramQuantities[MONOIN3_PARAM]->displayMultiplier=6.4;	// 64/10
		// paramQuantities[MONOIN5_PARAM]->displayMultiplier=6.4;	// 64/10
		configInput(MONOIN7_INPUT, "Reset");
		configInput(MONOIN8_INPUT, "Clock");
		configInput(MONOIN2_INPUT, "DivA socket");
		configInput(MONOIN4_INPUT, "DivB socket");
		configInput(MONOIN6_INPUT, "DivC socket");
		configOutput(POLYOUT_OUTPUT, "");
	}

int indexPrec=0;	// context menu settings
int indexMaxLen=2;	// context menu settings
int indexHitMode=0;	// context menu settings

bool oldRst=false;
bool newRst=false;
bool oldClk=false;
bool newClk=false;

int loopKnobs=1;	// save some CPU to check the knobs only occaisonly
int currStep=0;

int divA=1;
int divB=1;
int divC=1;
int outV=0;

	void process(const ProcessArgs& args) override {
		
		if (loopKnobs-- ==0) {
			loopKnobs=500;

			// 0-10V controlls the knobs
			if (inputs[MONOIN2_INPUT].isConnected()) {params[MONOIN1_PARAM].setValue(abs(inputs[MONOIN2_INPUT].getVoltage()/10*64));}
			if (inputs[MONOIN4_INPUT].isConnected()) {params[MONOIN3_PARAM].setValue(abs(inputs[MONOIN4_INPUT].getVoltage()/10*64));}
			if (inputs[MONOIN6_INPUT].isConnected()) {params[MONOIN5_PARAM].setValue(abs(inputs[MONOIN6_INPUT].getVoltage()/10*64));}

			// the knobs define the parameters
			divA=round(params[MONOIN1_PARAM].getValue());
			divB=round(params[MONOIN3_PARAM].getValue());
			divC=round(params[MONOIN5_PARAM].getValue());

		}
					
		// to catch the reset signal
		newRst=inputs[MONOIN7_INPUT].getVoltage()>2;
		if (oldRst!=newRst) {currStep=0;oldRst=newRst;}
		
		// to catch the clock signal
		newClk=inputs[MONOIN8_INPUT].getVoltage()>2;
		if (oldClk!=newClk) {
			outV=0;
			if (newClk) {
				if (currStep>=64-1) {currStep=0;} else {currStep++;}
				if (divA>0 && (currStep % divA ==0)) {outV+=10; lights[DIVA_LIGHT].setBrightness(1);} else {lights[DIVA_LIGHT].setBrightness(0);};
				if (divB>0 && (currStep % divB ==0)) {outV+=10; lights[DIVB_LIGHT].setBrightness(1);} else {lights[DIVB_LIGHT].setBrightness(0);};
				if (divC>0 && (currStep % divC ==0)) {outV+=10; lights[DIVC_LIGHT].setBrightness(1);} else {lights[DIVC_LIGHT].setBrightness(0);};
				if (outV!=10) {outV=0;}
			} 
			outputs[POLYOUT_OUTPUT].setVoltage(outV);
			oldClk=newClk;
		}		

	};

};

struct tXOrWidget : ModuleWidget {
	tXOrWidget(tXOr* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/tXOr.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(10.16, 55.88)), module, tXOr::MONOIN1_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(10.16, 76.20)), module, tXOr::MONOIN3_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(10.16, 106.68)), module, tXOr::MONOIN5_PARAM));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(10.16, 66.04)), module, tXOr::MONOIN2_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(10.16, 96.52)), module, tXOr::MONOIN4_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(10.16, 116.84)), module, tXOr::MONOIN6_INPUT));

		// addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(10.16, 55.88)), module, tXOr::MONOIN1_PARAM));
		// addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(10.16, 66.04)), module, tXOr::MONOIN3_PARAM));
		// addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(10.16, 76.20)), module, tXOr::MONOIN5_PARAM));
		// addInput(createInputCentered<PJ301MPort>(mm2px(Vec(10.16, 96.52)), module, tXOr::MONOIN2_INPUT));
		// addInput(createInputCentered<PJ301MPort>(mm2px(Vec(10.16, 106.68)), module, tXOr::MONOIN4_INPUT));
		// addInput(createInputCentered<PJ301MPort>(mm2px(Vec(10.16, 116.84)), module, tXOr::MONOIN6_INPUT));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(5.08, 25.4)), module, tXOr::MONOIN7_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(15.24, 25.4)), module, tXOr::MONOIN8_INPUT));

		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(10.16+2.5*2.54, 55.88-1.5*2.54)), module, tXOr::DIVA_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(10.16+2.5*2.54, 66.04-1.5*2.54)), module, tXOr::DIVB_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(10.16+2.5*2.54, 76.20-1.5*2.54)), module, tXOr::DIVC_LIGHT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(10.16, 40.428)), module, tXOr::POLYOUT_OUTPUT));
	}
		
};


Model* modeltXOr = createModel<tXOr, tXOrWidget>("tXOr");