#include "plugin.hpp"


struct PolyExp10 : Module {
	enum ParamId {
		// CONTEXT_INDEXCHAN,	// new
		PARAMS_LEN
	};
	enum InputId {
		POLYIN_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		OUT1_OUTPUT,
		OUT2_OUTPUT,
		OUT3_OUTPUT,
		OUT4_OUTPUT,
		OUT5_OUTPUT,
		OUT6_OUTPUT,
		OUT7_OUTPUT,
		OUT8_OUTPUT,
		OUT9_OUTPUT,
		OUT10_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		LIGHTS_LEN
	};

	PolyExp10() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		// configParam(CONTEXT_INDEXCHAN, 0.f, 1.f, 8.f, "See context menu");	// new
		configInput(POLYIN_INPUT, "");
		configOutput(OUT1_OUTPUT, "");
		configOutput(OUT2_OUTPUT, "");
		configOutput(OUT3_OUTPUT, "");
		configOutput(OUT4_OUTPUT, "");
		configOutput(OUT5_OUTPUT, "");
		configOutput(OUT6_OUTPUT, "");
		configOutput(OUT7_OUTPUT, "");
		configOutput(OUT8_OUTPUT, "");
		configOutput(OUT9_OUTPUT, "");
		configOutput(OUT10_OUTPUT, "");
	}

int indexChan=0;

	void process(const ProcessArgs& args) override {
		
		outputs[OUT10_OUTPUT].setVoltage(inputs[POLYIN_INPUT].getVoltage(9+indexChan*6));
		outputs[OUT9_OUTPUT].setVoltage(inputs[POLYIN_INPUT].getVoltage(8+indexChan*6));
		outputs[OUT8_OUTPUT].setVoltage(inputs[POLYIN_INPUT].getVoltage(7+indexChan*6));
		outputs[OUT7_OUTPUT].setVoltage(inputs[POLYIN_INPUT].getVoltage(6+indexChan*6));
		outputs[OUT6_OUTPUT].setVoltage(inputs[POLYIN_INPUT].getVoltage(5+indexChan*6));
		outputs[OUT5_OUTPUT].setVoltage(inputs[POLYIN_INPUT].getVoltage(4+indexChan*6));
		outputs[OUT4_OUTPUT].setVoltage(inputs[POLYIN_INPUT].getVoltage(3+indexChan*6));
		outputs[OUT3_OUTPUT].setVoltage(inputs[POLYIN_INPUT].getVoltage(2+indexChan*6));
		outputs[OUT2_OUTPUT].setVoltage(inputs[POLYIN_INPUT].getVoltage(1+indexChan*6));
		outputs[OUT1_OUTPUT].setVoltage(inputs[POLYIN_INPUT].getVoltage(0+indexChan*6));
	}

	// this block is to save and reload a variable
	json_t* dataToJson() override {
	json_t* rootJ = json_object();
	json_object_set_new(rootJ, "mode", json_boolean(indexChan));
	return rootJ;}

	void dataFromJson(json_t* rootJ) override {
	json_t *modeJ = json_object_get(rootJ, "mode");
	if (modeJ) indexChan = json_boolean_value(modeJ);}

};

struct PolyExp10Widget : ModuleWidget {
	PolyExp10Widget(PolyExp10* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/PolyExp10.svg")));

		// addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(5.08, 116.84)), module, PolyExp10::POLYIN_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(5.08, 10.16)), module, PolyExp10::OUT1_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(5.08, 20.32)), module, PolyExp10::OUT2_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(5.08, 30.48)), module, PolyExp10::OUT3_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(5.08, 40.64)), module, PolyExp10::OUT4_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(5.08, 50.8)), module, PolyExp10::OUT5_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(5.08, 60.96)), module, PolyExp10::OUT6_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(5.08, 71.12)), module, PolyExp10::OUT7_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(5.08, 81.28)), module, PolyExp10::OUT8_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(5.08, 91.44)), module, PolyExp10::OUT9_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(5.08, 101.6)), module, PolyExp10::OUT10_OUTPUT));
	}
	
	void appendContextMenu(Menu* menu) override {
		PolyExp10* module = dynamic_cast<PolyExp10*>(this->module);
		assert(module);
		menu->addChild(new MenuSeparator);
		menu->addChild(createIndexPtrSubmenuItem("Channels", {"first 10","last 10"}, &module->indexChan));
	}
	
	
};

Model* modelPolyExp10 = createModel<PolyExp10, PolyExp10Widget>("PolyExp10");