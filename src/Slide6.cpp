/* README.md

### Slide6

This is a fix voltage generator utility module. 

##### KNOBS:
- 'MIN' defines the lower threshold for the voltages. 
- 'MAX'  defines the upper threshold for the voltages. 
##### INPUTS:
- `RST` resets the sequencer
- `CLK` advances to the next step 
- 'MIN' modifies the relevant knob. 
- 'MAX' modifies the relevant knob. 
##### SLIDERS:
- Six sliders providing a constant voltage within the allowed range.
##### OUTPUTS:
- `OUT` outputs the result 
##### RIGHT-CLICK MENU:
- `QUANTIZE` allows selection between "Nope", "Octaves", "Notes". 
- `POLY CHANNELS` defines the number of channels in the output (1-6)
##### ALSO RECOMMENDED:
- Slide16 with a poly input and various outputs. 

*/

#include "plugin.hpp"


struct Slide6 : Module {
	enum ParamId {
		KNOBMIN_PARAM,
		KNOBMAX_PARAM,
		RECT1_PARAM,
		RECT3_PARAM,
		RECT4_PARAM,
		RECT5_PARAM,
		RECT6_PARAM,
		RECT2_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		SOCKETMIN_INPUT,
		SOCKETMAX_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		POLYOUT_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		LIGHTS_LEN
	};

	Slide6() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(KNOBMIN_PARAM, -10.f, 0.f, -5.f, "Min");
		configParam(KNOBMAX_PARAM, 0.f, 10.f, 5.f, "Max");
		paramQuantities[KNOBMIN_PARAM]->snapEnabled = true;
		paramQuantities[KNOBMAX_PARAM]->snapEnabled = true;
		paramQuantities[KNOBMIN_PARAM]->randomizeEnabled = false;
		paramQuantities[KNOBMAX_PARAM]->randomizeEnabled = false;
		configParam(RECT1_PARAM, 0.f, 1.f, 0.5f, "Slider 1");
		configParam(RECT2_PARAM, 0.f, 1.f, 0.5f, "Slider 2");
		configParam(RECT3_PARAM, 0.f, 1.f, 0.5f, "Slider 3");
		configParam(RECT4_PARAM, 0.f, 1.f, 0.5f, "Slider 4");
		configParam(RECT5_PARAM, 0.f, 1.f, 0.5f, "Slider 5");
		configParam(RECT6_PARAM, 0.f, 1.f, 0.5f, "Slider 6");
		configInput(SOCKETMIN_INPUT, "");
		configInput(SOCKETMAX_INPUT, "");
		configOutput(POLYOUT_OUTPUT, "");
	}

int minVal=-10;		// min value for all six slider
int maxVal=10;  	// max value for all six slider
float valNew=0;		// value read from a slider
int indexChan=5;	// this means 6 channels :)
int indexQuant=0;	// this means no quantizing :)

float oldVals[6]={0,0,0,0,0,0};	// for moderated CPU usage

	void process(const ProcessArgs& args) override {

		// if there is a min/max cable then move the knobs
		if (inputs[SOCKETMIN_INPUT].isConnected()) {
			params[KNOBMIN_PARAM].setValue(std::min(inputs[SOCKETMIN_INPUT].getVoltage(),0.f));
			}
		if (inputs[SOCKETMAX_INPUT].isConnected()) {
			params[KNOBMAX_PARAM].setValue(std::max(inputs[SOCKETMAX_INPUT].getVoltage(),0.f));
			}
		
		// record the knob positions
		minVal=params[KNOBMIN_PARAM].getValue();
		maxVal=params[KNOBMAX_PARAM].getValue();
		paramQuantities[RECT1_PARAM]->displayMultiplier = (maxVal-minVal);
		paramQuantities[RECT2_PARAM]->displayMultiplier = (maxVal-minVal);
		paramQuantities[RECT3_PARAM]->displayMultiplier = (maxVal-minVal);
		paramQuantities[RECT4_PARAM]->displayMultiplier = (maxVal-minVal);
		paramQuantities[RECT5_PARAM]->displayMultiplier = (maxVal-minVal);
		paramQuantities[RECT6_PARAM]->displayMultiplier = (maxVal-minVal);
		paramQuantities[RECT1_PARAM]->displayOffset = minVal;
		paramQuantities[RECT2_PARAM]->displayOffset = minVal;
		paramQuantities[RECT3_PARAM]->displayOffset = minVal;
		paramQuantities[RECT4_PARAM]->displayOffset = minVal;
		paramQuantities[RECT5_PARAM]->displayOffset = minVal;
		paramQuantities[RECT6_PARAM]->displayOffset = minVal;
		
		// read the sliders one by one and send to the polyout	
		valNew=params[RECT1_PARAM].getValue()*(maxVal-minVal)+minVal;
		if (indexQuant==1) {valNew=round(valNew);}
		else if (indexQuant==2) {valNew=round(valNew*12)/12;}
		outputs[POLYOUT_OUTPUT].setVoltage(valNew, 0);
		oldVals[0]=valNew;
		
		valNew=params[RECT2_PARAM].getValue()*(maxVal-minVal)+minVal;
		if (indexQuant==1) {valNew=round(valNew);}
		else if (indexQuant==2) {valNew=round(valNew*12)/12;}
		outputs[POLYOUT_OUTPUT].setVoltage(valNew, 1);
		oldVals[1]=valNew;
		
		valNew=params[RECT3_PARAM].getValue()*(maxVal-minVal)+minVal;
		if (indexQuant==1) {valNew=round(valNew);}
		else if (indexQuant==2) {valNew=round(valNew*12)/12;}
		outputs[POLYOUT_OUTPUT].setVoltage(valNew, 2);
		oldVals[2]=valNew;
		
		valNew=params[RECT4_PARAM].getValue()*(maxVal-minVal)+minVal;
		if (indexQuant==1) {valNew=round(valNew);}
		else if (indexQuant==2) {valNew=round(valNew*12)/12;}
		outputs[POLYOUT_OUTPUT].setVoltage(valNew, 3);
		oldVals[3]=valNew;
		
		valNew=params[RECT5_PARAM].getValue()*(maxVal-minVal)+minVal;
		if (indexQuant==1) {valNew=round(valNew);}
		else if (indexQuant==2) {valNew=round(valNew*12)/12;}
		outputs[POLYOUT_OUTPUT].setVoltage(valNew, 4);
		oldVals[4]=valNew;
		
		valNew=params[RECT6_PARAM].getValue()*(maxVal-minVal)+minVal;
		if (indexQuant==1) {valNew=round(valNew);}
		else if (indexQuant==2) {valNew=round(valNew*12)/12;}
		outputs[POLYOUT_OUTPUT].setVoltage(valNew, 5);
		oldVals[5]=valNew;

		outputs[POLYOUT_OUTPUT].channels = indexChan+1;	

	}
	
	// this block is to save and reload a variable
	json_t* dataToJson() override {
	json_t* rootJ = json_object();
	json_object_set_new(rootJ, "mode", json_integer(indexChan));
	json_object_set_new(rootJ, "quant", json_integer(indexQuant));
	return rootJ;}

	void dataFromJson(json_t* rootJ) override {
	json_t *modeJ = json_object_get(rootJ, "mode");
	if (modeJ) indexChan = json_integer_value(modeJ);
	json_t *quantJ = json_object_get(rootJ, "quant");
	if (quantJ) indexQuant = json_integer_value(quantJ);}
	
};


struct Slide6Widget : ModuleWidget {
	Slide6Widget(Slide6* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/Slide6.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(5.08, 25.4)), module, Slide6::KNOBMIN_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(15.24, 25.4)), module, Slide6::KNOBMAX_PARAM));

		addParam(createParamCentered<LEDSliderWhite>(mm2px(Vec(5.08, 61.066)), module, Slide6::RECT1_PARAM));
		addParam(createParamCentered<LEDSliderWhite>(mm2px(Vec(10.16, 61.066)), module, Slide6::RECT2_PARAM));
		addParam(createParamCentered<LEDSliderWhite>(mm2px(Vec(15.24, 61.066)), module, Slide6::RECT3_PARAM));
		addParam(createParamCentered<LEDSliderWhite>(mm2px(Vec(5.08, 89.006)), module, Slide6::RECT4_PARAM));
		addParam(createParamCentered<LEDSliderWhite>(mm2px(Vec(10.16, 89.006)), module, Slide6::RECT5_PARAM));
		addParam(createParamCentered<LEDSliderWhite>(mm2px(Vec(15.24, 89.006)), module, Slide6::RECT6_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(5.08, 40.64)), module, Slide6::SOCKETMIN_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(15.24, 40.64)), module, Slide6::SOCKETMAX_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(10.16, 111.76)), module, Slide6::POLYOUT_OUTPUT));
	}

	void appendContextMenu(Menu* menu) override {
		Slide6* module = dynamic_cast<Slide6*>(this->module);
		assert(module);
		menu->addChild(new MenuSeparator);
		menu->addChild(createIndexPtrSubmenuItem("Quantize", {"Nope","Octaves","Notes"}, &module->indexQuant));
		menu->addChild(createIndexPtrSubmenuItem("Poly channels", {"1","2","3","4","5","6"}, &module->indexChan));
	}



};


Model* modelSlide6 = createModel<Slide6, Slide6Widget>("Slide6");