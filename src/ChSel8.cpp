/* README.md

### ChSel8

This is a switch module that rearranges channels of a polyphonic module. 

##### INPUTS:
- `POLY IN` accepts a (1-16 channel) polyphonic cable.
##### KNOBS:
- Knobs '1-8' allow to select the channel from the input to be forwarded to the (1-8 channel of) output. 
##### OUTPUTS:
- `OUT` outputs a polyphonic signal with rearranged channels. 
##### RIGHT-CLICK MENU:
- `POLY CHANNELS` allows to limit the output to 1 to 8 channels.
##### ALSO RECOMMENDED:
- Yossarian ChSel16 that is a more comfortable 4 HP modell with similar functions. 

*/

#include "plugin.hpp"


struct ChSel8 : Module {
	enum ParamId {
		SELECT_CH1_PARAM,
		SELECT_CH2_PARAM,
		SELECT_CH3_PARAM,
		SELECT_CH4_PARAM,
		SELECT_CH5_PARAM,
		SELECT_CH6_PARAM,
		SELECT_CH7_PARAM,
		SELECT_CH8_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		POLYIN_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		POLYOUT_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		LIGHTS_LEN
	};

	ChSel8() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(SELECT_CH1_PARAM, 1.f, 16.f, 1.f, "Channel");
		configParam(SELECT_CH2_PARAM, 1.f, 16.f, 2.f, "Channel");
		configParam(SELECT_CH3_PARAM, 1.f, 16.f, 3.f, "Channel");
		configParam(SELECT_CH4_PARAM, 1.f, 16.f, 4.f, "Channel");
		configParam(SELECT_CH5_PARAM, 1.f, 16.f, 5.f, "Channel");
		configParam(SELECT_CH6_PARAM, 1.f, 16.f, 6.f, "Channel");
		configParam(SELECT_CH7_PARAM, 1.f, 16.f, 7.f, "Channel");
		configParam(SELECT_CH8_PARAM, 1.f, 16.f, 8.f, "Channel");
		paramQuantities[SELECT_CH1_PARAM]->snapEnabled = true;
		paramQuantities[SELECT_CH2_PARAM]->snapEnabled = true;
		paramQuantities[SELECT_CH3_PARAM]->snapEnabled = true;
		paramQuantities[SELECT_CH4_PARAM]->snapEnabled = true;
		paramQuantities[SELECT_CH5_PARAM]->snapEnabled = true;
		paramQuantities[SELECT_CH6_PARAM]->snapEnabled = true;		
		paramQuantities[SELECT_CH7_PARAM]->snapEnabled = true;		
		paramQuantities[SELECT_CH8_PARAM]->snapEnabled = true;		
		configInput(POLYIN_INPUT, "");
		configOutput(POLYOUT_OUTPUT, "");
	}

int indexChan=7; 	// this means 8 channels
int loopKnobs=1;
bool isInput=false;
float paramsVal[PARAMS_LEN]={1,1,1,1,1,1,1,1};
bool isPlugged=false;

	void process(const ProcessArgs& args) override {
		
		// occasionly collect the knob values 
		if (loopKnobs-- ==0) {
			loopKnobs=2500;
			for (int c=0;c<PARAMS_LEN;c++) {paramsVal[c]=params[c].getValue()-1;}
			isInput=inputs[POLYIN_INPUT].isConnected();
			if (isInput==false) {indexChan=7;}
			isPlugged=inputs[POLYIN_INPUT].isConnected();
			if (!(paramQuantities[SELECT_CH1_PARAM]->snapEnabled == isPlugged)) {
				paramQuantities[SELECT_CH1_PARAM]->snapEnabled = isPlugged;
				paramQuantities[SELECT_CH2_PARAM]->snapEnabled = isPlugged;
				paramQuantities[SELECT_CH3_PARAM]->snapEnabled = isPlugged;
				paramQuantities[SELECT_CH4_PARAM]->snapEnabled = isPlugged;
				paramQuantities[SELECT_CH5_PARAM]->snapEnabled = isPlugged;
				paramQuantities[SELECT_CH6_PARAM]->snapEnabled = isPlugged;
				paramQuantities[SELECT_CH7_PARAM]->snapEnabled = isPlugged;
				paramQuantities[SELECT_CH8_PARAM]->snapEnabled = isPlugged;
			}
		}
		
		// always collect and forward the input values
		for (int c=0;c<=indexChan;c++) {
			if (isInput==true) {
				outputs[POLYOUT_OUTPUT].setVoltage(inputs[POLYIN_INPUT].getVoltage(paramsVal[c]), c);
			} 
			else {
				outputs[POLYOUT_OUTPUT].setVoltage(paramsVal[c],c);
			}
		}

		outputs[POLYOUT_OUTPUT].channels=indexChan+1;
	}

	// this block is to save and reload a variable
/*
	json_t* dataToJson() override {
	json_t* rootJ = json_object();
	json_object_set_new(rootJ, "mode", json_integer(indexChan));
	return rootJ;}

 	void dataFromJson(json_t* rootJ) override {
	json_t *modeJ = json_object_get(rootJ, "mode");
	if (modeJ) indexChan = json_integer_value(modeJ);}
 */
 
};


struct ChSel8Widget : ModuleWidget {

	#include "share/widgetwizard.hpp"	// addChild abbreviations

    ChSel8* module;

	ChSel8Widget(ChSel8* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/ChSel8.svg")));

		// addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(5.08, 27.94)), module, ChSel8::SELECT_CH1_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(5.08, 38.1)), module, ChSel8::SELECT_CH2_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(5.08, 48.26)), module, ChSel8::SELECT_CH3_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(5.08, 58.42)), module, ChSel8::SELECT_CH4_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(5.08, 68.58)), module, ChSel8::SELECT_CH5_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(5.08, 78.74)), module, ChSel8::SELECT_CH6_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(5.08, 88.9)), module, ChSel8::SELECT_CH7_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(5.08, 99.06)), module, ChSel8::SELECT_CH8_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(5.08, 116.84)), module, ChSel8::POLYIN_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(5.08, 15.24)), module, ChSel8::POLYOUT_OUTPUT));
	}

/* 
	void appendContextMenu(Menu* menu) override {
		ChSel8* module = dynamic_cast<ChSel8*>(this->module);
		assert(module);
		menu->addChild(new MenuSeparator);
		menu->addChild(createIndexPtrSubmenuItem("Poly channels", {"1","2","3","4","5","6","7","8"}, &module->indexChan));
	}
 */
 
};


Model* modelChSel8 = createModel<ChSel8, ChSel8Widget>("ChSel8");