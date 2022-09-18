#include "plugin.hpp"


struct PolyExp8 : Module {
	enum ParamId {
		CHAN_PARAM, 
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
		OUTPUTS_LEN
	};
	enum LightId {
		LIGHTS_LEN
	};

	PolyExp8() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(CHAN_PARAM, 0.f, 1.f, 0.f, "Select 1-8 or 8-16 channels");	// new
		configInput(POLYIN_INPUT, "Polyphonic input");
		configOutput(OUT1_OUTPUT, "");
		configOutput(OUT2_OUTPUT, "");
		configOutput(OUT3_OUTPUT, "");
		configOutput(OUT4_OUTPUT, "");
		configOutput(OUT5_OUTPUT, "");
		configOutput(OUT6_OUTPUT, "");
		configOutput(OUT7_OUTPUT, "");
		configOutput(OUT8_OUTPUT, "");
	}

	int indexChan=0;
	int loop=0;
	void process(const ProcessArgs& args) override {
		if (loop--<=0) {
			loop=111222; 
			indexChan=params[CHAN_PARAM].getValue();
		}
		outputs[OUT8_OUTPUT].setVoltage(inputs[POLYIN_INPUT].getVoltage(7+indexChan*8));
		outputs[OUT7_OUTPUT].setVoltage(inputs[POLYIN_INPUT].getVoltage(6+indexChan*8));
		outputs[OUT6_OUTPUT].setVoltage(inputs[POLYIN_INPUT].getVoltage(5+indexChan*8));
		outputs[OUT5_OUTPUT].setVoltage(inputs[POLYIN_INPUT].getVoltage(4+indexChan*8));
		outputs[OUT4_OUTPUT].setVoltage(inputs[POLYIN_INPUT].getVoltage(3+indexChan*8));
		outputs[OUT3_OUTPUT].setVoltage(inputs[POLYIN_INPUT].getVoltage(2+indexChan*8));
		outputs[OUT2_OUTPUT].setVoltage(inputs[POLYIN_INPUT].getVoltage(1+indexChan*8));
		outputs[OUT1_OUTPUT].setVoltage(inputs[POLYIN_INPUT].getVoltage(0+indexChan*8));
	}

};

struct PolyExp8Widget : ModuleWidget {
	PolyExp8Widget(PolyExp8* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/PolyExp8.svg")));

		// addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(5.08, 116.84)), module, PolyExp8::POLYIN_INPUT));

		addParam(createParamCentered<CKSS>(mm2px(Vec(5.08, 15.24)), module, PolyExp8::CHAN_PARAM));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(5.08, 30.48)), module, PolyExp8::OUT1_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(5.08, 40.64)), module, PolyExp8::OUT2_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(5.08, 50.8)), module, PolyExp8::OUT3_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(5.08, 60.96)), module, PolyExp8::OUT4_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(5.08, 71.12)), module, PolyExp8::OUT5_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(5.08, 81.28)), module, PolyExp8::OUT6_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(5.08, 91.44)), module, PolyExp8::OUT7_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(5.08, 101.6)), module, PolyExp8::OUT8_OUTPUT));

	}
		
};

Model* modelPolyExp8 = createModel<PolyExp8, PolyExp8Widget>("PolyExp8");