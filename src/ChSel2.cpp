/* README.md

### ChSel2

This module is ...

##### BUTTON:
##### INPUTS:
##### KNOBS:
##### OUTPUTS:
- `POLYOUT` outputs ±5 volts.
##### RIGHT-CLICK MENU:
##### SHORTCUTS:
*/

#include "plugin.hpp"


struct ChSel2 : Module {
	enum ParamId {
		SLIDER1_PARAM,
		SLIDER2_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		POLYIN_INPUT,
		CV2_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		OUT1_OUTPUT,
		OUT2_OUTPUT,
        DEBUG_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		LIGHTS_LEN
	};

	ChSel2() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configInput(POLYIN_INPUT, "Polyphonic");
		configParam(SLIDER1_PARAM, 1.f, 16.f, 0.f, "Pick channel A");
		configParam(SLIDER2_PARAM, 1.f, 16.f, 0.f, "Pick channel B");
		paramQuantities[SLIDER1_PARAM]->snapEnabled = true;
		paramQuantities[SLIDER2_PARAM]->snapEnabled = true;
		
		configInput(CV2_INPUT, "Modify B slider");
		configOutput(OUT1_OUTPUT, "A channel");
		configOutput(OUT2_OUTPUT, "B channel");
		configOutput(DEBUG_OUTPUT, "DEBUG");
	}

	float voltA=0;
	float voltB=0;
	int loopKnobs=0;	// CPU saviour for knob changes
	// int polyChannels=1;

    // the main routine
	void process(const ProcessArgs& args) override {
	
		if (loopKnobs--==0) {
			loopKnobs=500;
			if (inputs[CV2_INPUT].isConnected()) {
					// polyChannels=inputs[POLYIN_INPUT].channels;
					// params[SLIDER2_PARAM].setValue(1+abs(floor((inputs[CV2_INPUT].getVoltage())/10*polyChannels)));
					params[SLIDER2_PARAM].setValue(1+abs(floor((inputs[CV2_INPUT].getVoltage())/10*16)));
					}
				voltA=inputs[POLYIN_INPUT].getVoltage((int)params[SLIDER1_PARAM].getValue()-1);
				voltB=inputs[POLYIN_INPUT].getVoltage((int)params[SLIDER2_PARAM].getValue()-1);
		}
			outputs[OUT1_OUTPUT].setVoltage(voltA);
			outputs[OUT2_OUTPUT].setVoltage(voltB);
	}
};


struct ChSel2Widget : ModuleWidget {

	#include "share/widgetwizard.hpp"	// addChild abbreviations

    ChSel2* module;

	ChSel2Widget(ChSel2* module) {
        this->module = module;
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/ChSel2.svg")));

		// addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<LEDSliderWhite>(mm2px(Vec(5.08, 20.32)), 	 module, ChSel2::SLIDER1_PARAM));
		addParam(createParamCentered<LEDSliderWhite>(mm2px(Vec(5.08, 98.847)), 	 module, ChSel2::SLIDER2_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(5.08, 63.5)), 	 module, ChSel2::POLYIN_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(5.08, 116.84)), 	 module, ChSel2::CV2_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(5.08, 48.26)), 	 module, ChSel2::OUT1_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(5.08, 78.74)), 	 module, ChSel2::OUT2_OUTPUT));
        // addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(5.08, 128.5-5.08)), 	 module, TrigPrec2::DEBUG_OUTPUT));
	}

    // void appendContextMenu(Menu* menu) override {
    // ChSel2* module = dynamic_cast<ChSel2*>(this->module);
    // assert(module);
    // menu->addChild(new MenuSeparator);
    // menu->addChild(createMenuLabel("l: Toggle link button"));
    // menu->addChild(createMenuLabel("1-9: Set top precision between 10%-90%"));
    // }

	// void onHoverKey(const event::HoverKey &e) override {
		// if (e.key >= GLFW_KEY_1 && e.key <= GLFW_KEY_4) {
			// if (e.action == GLFW_PRESS) {
				// float key_number = e.key - 49; // 49 is the ascii number for key #1
				// module->contextRnd(key_number);
				// e.consume(this);
			// }
		// }
	// ModuleWidget::onHoverKey(e);
	// }

};


Model* modelChSel2 = createModel<ChSel2, ChSel2Widget>("ChSel2");