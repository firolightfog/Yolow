/* README.md

### RandVolt10

This module is to provide 10 lovely random voltages in the range of 0V-10V

##### INPUTS: a trigger input
##### OUTPUTS:
- `OUT 1-10` outputs 0V-10V volts; if no input is connected then the outputs are sending 1V-10V respectively.
##### RIGHT-CLICK MENU:
- `RANGE` selects between `-5V to +5V` and `0V to 10V`.
- `LENGTH` outputs 0V or 10V pulses. 
- `HIT KNOB MODE` allows selection betwee "solid hits" or "percentages".
##### ALSO RECOMMENDED:
- Tiny Tricks Sample and Hold x16 module for similarly simple random voltages in the -5V to 5V range.
- Submarine SS-221 for similar fix octave voltages in the range of -10V to 10V.

*/

#include "plugin.hpp"


struct RandVolt10 : Module {
	enum ParamId {
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

	RandVolt10() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
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

	int indexRange=1;
	int modRange=0;
	bool newTrig=false;
	bool oldTrig=false;
    // the main routine
	void process(const ProcessArgs& args) override {
		
		if (inputs[POLYIN_INPUT].isConnected()) {
		
			newTrig=inputs[POLYIN_INPUT].getVoltage()>2;
			if (newTrig==true && oldTrig==false) { 
				modRange=(indexRange==0)?-5:0;
				for (int c=0; c<10; c++) {outputs[OUT1_OUTPUT+c].setVoltage(rack::random::uniform()*10+modRange);}
				oldTrig=newTrig;
			}
			else if (newTrig==false && oldTrig==true) {oldTrig=newTrig;}
		} 
		else {
				for (int c=0; c<10; c++) {outputs[OUT1_OUTPUT+c].setVoltage(c+1);}			
		}	
	}
	
	// this block is to save and reload a variable
	json_t* dataToJson() override {
	json_t* rootJ = json_object();
	json_object_set_new(rootJ, "range", json_integer(indexRange));
	// json_object_set_new(rootJ, "quant", json_integer(indexQuant));
	return rootJ;}

	void dataFromJson(json_t* rootJ) override {
	json_t *rangeJ = json_object_get(rootJ, "range");
	if (rangeJ) indexRange = json_integer_value(rangeJ);
	// json_t *quantJ = json_object_get(rootJ, "quant");
	// if (quantJ) indexQuant = json_integer_value(quantJ);
	}

};


struct RandVolt10Widget : ModuleWidget {

    RandVolt10* module;

	RandVolt10Widget(RandVolt10* module) {
        this->module = module;
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/RandVolt10.svg")));

		// addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(5.08, 66.04)), module, RandVolt10::POLYIN_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(5.08, 10.16)), module, RandVolt10::OUT1_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(5.08, 20.32)), module, RandVolt10::OUT2_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(5.08, 30.48)), module, RandVolt10::OUT3_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(5.08, 40.64)), module, RandVolt10::OUT4_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(5.08, 50.8)), module, RandVolt10::OUT5_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(5.08, 76.2)), module, RandVolt10::OUT6_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(5.08, 86.36)), module, RandVolt10::OUT7_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(5.08, 96.52)), module, RandVolt10::OUT8_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(5.08, 106.68)), module, RandVolt10::OUT9_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(5.08, 116.84)), module, RandVolt10::OUT10_OUTPUT));
	}


	void appendContextMenu(Menu* menu) override {
		RandVolt10* module = dynamic_cast<RandVolt10*>(this->module);
		assert(module);
		menu->addChild(new MenuSeparator);
		// menu->addChild(createIndexPtrSubmenuItem("Quantize", {"Nope","Octaves","Notes"}, &module->indexQuant));
		menu->addChild(createIndexPtrSubmenuItem("Range", {"-5V to 5V","0 to 10V"}, &module->indexRange));
	}

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


Model* modelRandVolt10 = createModel<RandVolt10, RandVolt10Widget>("RandVolt10");