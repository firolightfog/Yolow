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
- `OUT` XOR output of the three divisions 

*/

#include "plugin.hpp"

struct tXOr : Module {
	enum ParamId {DIVA_PARAM,DIVB_PARAM,DIVC_PARAM,PARAMS_LEN};
	enum InputId {DIVA_INPUT,DIVB_INPUT,DIVC_INPUT,RESET_INPUT,CLOCK_INPUT,INPUTS_LEN};
	enum OutputId {OR_OUTPUT,XOR_OUTPUT,OUTPUTS_LEN};
	enum LightId {DIVA_LIGHT,DIVB_LIGHT,DIVC_LIGHT,LIGHTS_LEN};

	tXOr() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(DIVA_PARAM, 0.f, 64.f, 0.f, "DivA knob");
		configParam(DIVB_PARAM, 0.f, 64.f, 0.f, "DivB knob");
		configParam(DIVC_PARAM, 0.f, 64.f, 0.f, "DivC knob");
		paramQuantities[DIVA_PARAM]->snapEnabled = true;
		paramQuantities[DIVB_PARAM]->snapEnabled = true;
		paramQuantities[DIVC_PARAM]->snapEnabled = true;
		// paramQuantities[DIVA_PARAM]->displayMultiplier=6.4;	// 64/10
		// paramQuantities[DIVB_PARAM]->displayMultiplier=6.4;	// 64/10
		// paramQuantities[DIVC_PARAM]->displayMultiplier=6.4;	// 64/10
		configInput(RESET_INPUT, "Reset");
		configInput(CLOCK_INPUT, "Clock");
		configInput(DIVA_INPUT, "DivA CV");
		configInput(DIVB_INPUT, "DivB CV");
		configInput(DIVC_INPUT, "DivC CV");
		configOutput(XOR_OUTPUT, "XOR Sequence");
		configOutput(OR_OUTPUT, "OR Sequence");
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
	int outXorV=0;
	int outOrV=0;

	void process(const ProcessArgs& args) override {
		
		if (loopKnobs-- ==0) {
			loopKnobs=500;

			// 0-10V controlls the knobs
			if (inputs[DIVA_INPUT].isConnected()) {params[DIVA_PARAM].setValue(abs(inputs[DIVA_INPUT].getVoltage()/10*64));}
			if (inputs[DIVB_INPUT].isConnected()) {params[DIVB_PARAM].setValue(abs(inputs[DIVB_INPUT].getVoltage()/10*64));}
			if (inputs[DIVC_INPUT].isConnected()) {params[DIVC_PARAM].setValue(abs(inputs[DIVC_INPUT].getVoltage()/10*64));}

			// the knobs define the parameters
			divA=round(params[DIVA_PARAM].getValue());
			divB=round(params[DIVB_PARAM].getValue());
			divC=round(params[DIVC_PARAM].getValue());

		}
					
		// to catch the reset signal
		newRst=inputs[RESET_INPUT].getVoltage()>2;
		if (oldRst!=newRst) {currStep=0;oldRst=newRst;}
		
		// to catch the clock signal
		newClk=inputs[CLOCK_INPUT].getVoltage()>2;
		if (oldClk!=newClk) {
			outOrV=0;
			outXorV=0;
			if (newClk) {
				if (currStep>=64-1) {currStep=0;} else {currStep++;}
				if (divA>0 && (currStep % divA ==0)) {outXorV+=10; lights[DIVA_LIGHT].setBrightness(1);} 
				else {lights[DIVA_LIGHT].setBrightness(0);};
				if (divB>0 && (currStep % divB ==0)) {outXorV+=10; lights[DIVB_LIGHT].setBrightness(1);} 
				else {lights[DIVB_LIGHT].setBrightness(0);};
				if (divC>0 && (currStep % divC ==0)) {outXorV+=10; lights[DIVC_LIGHT].setBrightness(1);} 
				else {lights[DIVC_LIGHT].setBrightness(0);};
				if (outXorV>=10) {outOrV=10;}
				if (outXorV!=10) {outXorV=0;}
			} 
			outputs[OR_OUTPUT].setVoltage(outOrV);
			outputs[XOR_OUTPUT].setVoltage(outXorV);
			oldClk=newClk;
		}		

	};

};

struct tXOrWidget : ModuleWidget {

	tXOr* module;
	// #include "share/widgetwizard.hpp"   // addChild abbreviations
	float HP=5.08f;
	
	tXOrWidget(tXOr* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/tXOr.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		// childKnob(tXOr::DIVA_PARAM,1,HP*2,HP*11);
		// childKnob(tXOr::DIVB_PARAM,1,HP*2,HP*16);
		// childKnob(tXOr::DIVC_PARAM,1,HP*2,HP*21);
		// childInput(tXOr::DIVA_INPUT,HP*2,HP*13);
		// childInput(tXOr::DIVB_INPUT,HP*2,HP*18);
		// childInput(tXOr::DIVC_INPUT,HP*2,HP*23);
		// childLight(tXOr::DIVA_LIGHT,1,HP*3.25,HP*(13-1));
		// childLight(tXOr::DIVB_LIGHT,1,HP*3.25,HP*(18-1));
		// childLight(tXOr::DIVC_LIGHT,1,HP*3.25,HP*(23-1));
	addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(HP*2,HP*11)), module, tXOr::DIVA_PARAM));
	addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(HP*2,HP*16)), module, tXOr::DIVB_PARAM));
	addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(HP*2,HP*21)), module, tXOr::DIVC_PARAM));
	addInput(createInputCentered<PJ301MPort>(mm2px(Vec(HP*2,HP*13)), module, tXOr::DIVA_INPUT));
	addInput(createInputCentered<PJ301MPort>(mm2px(Vec(HP*2,HP*18)), module, tXOr::DIVB_INPUT));
	addInput(createInputCentered<PJ301MPort>(mm2px(Vec(HP*2,HP*23)), module, tXOr::DIVC_INPUT));
	addChild(createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(HP*3.25,HP*(13-1))), module, tXOr::DIVA_LIGHT));
	addChild(createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(HP*3.25,HP*(18-1))), module, tXOr::DIVB_LIGHT));
	addChild(createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(HP*3.25,HP*(23-1))), module, tXOr::DIVC_LIGHT));

		// childInput(tXOr::RESET_INPUT,HP*1,HP*5);
		// childInput(tXOr::CLOCK_INPUT,HP*3,HP*5);
		// childOutput(tXOr::OR_OUTPUT,HP*1,HP*8);
		// childOutput(tXOr::XOR_OUTPUT,HP*3,HP*8);

	addInput(createInputCentered<PJ301MPort>(mm2px(Vec(HP*1,HP*5.5)), module, tXOr::RESET_INPUT));
	addInput(createInputCentered<PJ301MPort>(mm2px(Vec(HP*3,HP*5.5)), module, tXOr::CLOCK_INPUT));
	addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(HP*1,HP*8.5)), module, tXOr::OR_OUTPUT));
	addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(HP*3,HP*8.5)), module, tXOr::XOR_OUTPUT));

	}
		
};


Model* modeltXOr = createModel<tXOr, tXOrWidget>("tXOr");