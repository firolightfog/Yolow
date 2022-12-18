/* README.md

### PolyCopy10

This is a polyphonic utility to merge monophonic signals.  

##### INPUTS:
- `IN 1-10` monophonic input with sockets normalized to the next one. resets the sequencer 
##### OUTPUTS:
- `OUT` outputs a polyphonic signal.
##### RIGHT-CLICK MENU:
- `CHANNELS` according to selection the input signals are merged are located to channel "1 to 10" or "6 to 16".
##### ALSO RECOMMENDED:
- PolyCopy16 with 16 inputs but similar features. 

*/

#include "plugin.hpp"



struct PolyCopy10 : Module {
	enum ParamId {
		SYNC_PARAM,	// new
		PARAMS_LEN
	};
	enum InputId {
		MONOIN1_INPUT,
		MONOIN2_INPUT,
		MONOIN3_INPUT,
		MONOIN4_INPUT,
		MONOIN5_INPUT,
		MONOIN6_INPUT,
		MONOIN7_INPUT,
		MONOIN8_INPUT,
		MONOIN9_INPUT,
		MONOIN10_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		POLYOUT_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		LIGHTS_LEN
	};

	PolyCopy10() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configSwitch(SYNC_PARAM, 0, 1, 0, "Sync mode", {"Soft", "Hard"}); 	// new
		configInput(MONOIN1_INPUT, "Mono chan 1");
		configInput(MONOIN2_INPUT, "Mono chan 2");
		configInput(MONOIN3_INPUT, "Mono chan 3");
		configInput(MONOIN4_INPUT, "Mono chan 4");
		configInput(MONOIN5_INPUT, "Mono chan 5");
		configInput(MONOIN6_INPUT, "Mono chan 6");
		configInput(MONOIN7_INPUT, "Mono chan 7");
		configInput(MONOIN8_INPUT, "Mono chan 8");
		configInput(MONOIN9_INPUT, "Mono chan 9");
		configInput(MONOIN10_INPUT, "Mono chan 10");
		configOutput(POLYOUT_OUTPUT, "Poly out");
	}
	
	// int ALLIN[10]={MONOIN1_INPUT,MONOIN2_INPUT,MONOIN3_INPUT,
		// MONOIN4_INPUT,MONOIN5_INPUT,MONOIN6_INPUT,
		// MONOIN7_INPUT,MONOIN8_INPUT,MONOIN9_INPUT,
		// MONOIN10_INPUT};
	
// these are OK
int indexChan=0;	// this means 0-10 channels :)

	void process(const ProcessArgs& args) override {
		int totalPolyChans=1;
		float valOld=0;
		float valNew=0;
		
		if (outputs[POLYOUT_OUTPUT].isConnected()) {

			for (int c = 9; c >=0; c--) {
				if (!inputs[MONOIN1_INPUT + c].isConnected()) {valNew=valOld;}
				else {valNew=inputs[MONOIN1_INPUT + c].getVoltage();
				// if (!inputs[ALLIN[c]].isConnected()) {valNew=valOld;}
				// else {valNew=inputs[ALLIN[c]].getVoltage();
					if (totalPolyChans==1) {totalPolyChans=c+1;}
				}
				outputs[POLYOUT_OUTPUT].setVoltage(valNew, c+6*indexChan);
				// outputs[POLYOUT_OUTPUT].setVoltage(valNew, c);
				valOld=valNew;
			}
			outputs[POLYOUT_OUTPUT].channels = totalPolyChans;

			outputs[POLYOUT_OUTPUT].channels = totalPolyChans+6*indexChan;
			if (! indexChan==0) {
				outputs[POLYOUT_OUTPUT].setVoltage(valNew, 5);
				outputs[POLYOUT_OUTPUT].setVoltage(valNew, 4);
				outputs[POLYOUT_OUTPUT].setVoltage(valNew, 3);
				outputs[POLYOUT_OUTPUT].setVoltage(valNew, 2);
				outputs[POLYOUT_OUTPUT].setVoltage(valNew, 1);
				outputs[POLYOUT_OUTPUT].setVoltage(valNew, 0);			
			}
		}
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

struct PolyCopy10Widget : ModuleWidget {
	PolyCopy10Widget(PolyCopy10* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/PolyCopy10.svg")));

		// addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(5.08, 10.16)), module, PolyCopy10::MONOIN1_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(5.08, 20.32)), module, PolyCopy10::MONOIN2_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(5.08, 30.48)), module, PolyCopy10::MONOIN3_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(5.08, 40.64)), module, PolyCopy10::MONOIN4_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(5.08, 50.8)), module, PolyCopy10::MONOIN5_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(5.08, 60.96)), module, PolyCopy10::MONOIN6_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(5.08, 71.12)), module, PolyCopy10::MONOIN7_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(5.08, 81.28)), module, PolyCopy10::MONOIN8_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(5.08, 91.44)), module, PolyCopy10::MONOIN9_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(5.08, 101.6)), module, PolyCopy10::MONOIN10_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(5.08, 116.84)), module, PolyCopy10::POLYOUT_OUTPUT));
	}
	
	void appendContextMenu(Menu* menu) override {
		PolyCopy10* module = dynamic_cast<PolyCopy10*>(this->module);
		assert(module);
		menu->addChild(new MenuSeparator);
		menu->addChild(createIndexPtrSubmenuItem("Channels", {"1 to 10","6 to 16"}, &module->indexChan));
	}
	
	
};


Model* modelPolyCopy10 = createModel<PolyCopy10, PolyCopy10Widget>("PolyCopy10");