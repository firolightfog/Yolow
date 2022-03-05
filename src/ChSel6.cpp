/* README.md

### ChSel6

This is a switch module that rearranges channels of a polyphonic module. 

##### INPUTS:
- `POLY IN` accepts a (1-16 channel) polyphonic cable.
##### KNOBS:
- Knobs '1-6' allow to select the channel from the input to be forwarded to the (1-6 channel of) output. 
##### OUTPUTS:
- `OUT` outputs a polyphonic signal with rearranged channels. 
##### RIGHT-CLICK MENU:
- `POLY CHANNELS` allows to limit the output to 1 to 6 channels.
##### ALSO RECOMMENDED:
- Yossarian ChSel18 that is only 2 HP but allows rearrangement of 8 channels.

*/

#include "plugin.hpp"

struct ChSel6 : Module {
	enum ParamId {
		SELECT_CH1_PARAM,
		SELECT_CH2_PARAM,
		SELECT_CH3_PARAM,
		SELECT_CH4_PARAM,
		SELECT_CH5_PARAM,
		SELECT_CH6_PARAM,
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

	ChSel6() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(SELECT_CH1_PARAM, 1.f, 16.f, 1.f, "Channel");
		configParam(SELECT_CH2_PARAM, 1.f, 16.f, 1.f, "Channel");
		configParam(SELECT_CH3_PARAM, 1.f, 16.f, 1.f, "Channel");
		configParam(SELECT_CH4_PARAM, 1.f, 16.f, 1.f, "Channel");
		configParam(SELECT_CH5_PARAM, 1.f, 16.f, 1.f, "Channel");
		configParam(SELECT_CH6_PARAM, 1.f, 16.f, 1.f, "Channel");
		paramQuantities[SELECT_CH1_PARAM]->snapEnabled = true;
		paramQuantities[SELECT_CH2_PARAM]->snapEnabled = true;
		paramQuantities[SELECT_CH3_PARAM]->snapEnabled = true;
		paramQuantities[SELECT_CH4_PARAM]->snapEnabled = true;
		paramQuantities[SELECT_CH5_PARAM]->snapEnabled = true;
		paramQuantities[SELECT_CH6_PARAM]->snapEnabled = true;		
		configInput(POLYIN_INPUT, "");
		configOutput(POLYOUT_OUTPUT, "");
	}

int indexChan=5; // this means 6 preferred outgoing channels

	void process(const ProcessArgs& args) override {
		
		outputs[POLYOUT_OUTPUT].setVoltage(inputs[POLYIN_INPUT].getVoltage(params[SELECT_CH1_PARAM].getValue()-1), 0);
		outputs[POLYOUT_OUTPUT].setVoltage(inputs[POLYIN_INPUT].getVoltage(params[SELECT_CH2_PARAM].getValue()-1), 1);
		outputs[POLYOUT_OUTPUT].setVoltage(inputs[POLYIN_INPUT].getVoltage(params[SELECT_CH3_PARAM].getValue()-1), 2);
		outputs[POLYOUT_OUTPUT].setVoltage(inputs[POLYIN_INPUT].getVoltage(params[SELECT_CH4_PARAM].getValue()-1), 3);
		outputs[POLYOUT_OUTPUT].setVoltage(inputs[POLYIN_INPUT].getVoltage(params[SELECT_CH5_PARAM].getValue()-1), 4);
		outputs[POLYOUT_OUTPUT].setVoltage(inputs[POLYIN_INPUT].getVoltage(params[SELECT_CH6_PARAM].getValue()-1), 5);

		// if (params[SELECT_CH1_PARAM].getValue()==3) {params[SELECT_CH6_PARAM].hide();} else {params[SELECT_CH6_PARAM].show();}

		outputs[POLYOUT_OUTPUT].channels=indexChan+1;

	}

	// this block is to save and reload a variable
	json_t* dataToJson() override {
	json_t* rootJ = json_object();
	json_object_set_new(rootJ, "mode", json_integer(indexChan));
	return rootJ;}

	void dataFromJson(json_t* rootJ) override {
	json_t *modeJ = json_object_get(rootJ, "mode");
	if (modeJ) indexChan = json_integer_value(modeJ);}

};


struct ChSel6Widget : ModuleWidget {
	ChSel6Widget(ChSel6* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/ChSel6.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(10.16, 45.72-2.54)), module, ChSel6::SELECT_CH1_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(10.16, 55.88-2.54)), module, ChSel6::SELECT_CH2_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(10.16, 66.04-2.54)), module, ChSel6::SELECT_CH3_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(10.16, 76.2-2.54)), module, ChSel6::SELECT_CH4_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(10.16, 86.36-2.54)), module, ChSel6::SELECT_CH5_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(10.16, 96.52-2.54)), module, ChSel6::SELECT_CH6_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(10.16, 30.48)), module, ChSel6::POLYIN_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(10.16, 114.3)), module, ChSel6::POLYOUT_OUTPUT));
	}
	
	void appendContextMenu(Menu* menu) override {
		ChSel6* module = dynamic_cast<ChSel6*>(this->module);
		assert(module);
		menu->addChild(new MenuSeparator);
		menu->addChild(createIndexPtrSubmenuItem("Poly channels", {"1","2","3","4","5","6"}, &module->indexChan));
	}


};


Model* modelChSel6 = createModel<ChSel6, ChSel6Widget>("ChSel6");