/* README.md

### PolyCopy16

This is a polyphonic utility to merge monophonic signals.  

##### INPUTS:
- `IN 1-16` monophonic input with sockets normalized to the next one. resets the sequencer 
##### OUTPUTS:
- `OUT` outputs a polyphonic signal.
##### ALSO RECOMMENDED:
- PolyCopy10 with 2HP sidth and 10 inputs but similar features. 

*/

#include "plugin.hpp"

struct PolyCopy16 : Module {
	enum ParamId {
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
		MONOIN11_INPUT,
		MONOIN12_INPUT,
		MONOIN13_INPUT,
		MONOIN14_INPUT,
		MONOIN15_INPUT,
		MONOIN16_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		POLYOUT_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		LIGHTS_LEN
	};

	PolyCopy16() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configInput(MONOIN1_INPUT, "");
		configInput(MONOIN9_INPUT, "");
		configInput(MONOIN2_INPUT, "");
		configInput(MONOIN10_INPUT, "");
		configInput(MONOIN3_INPUT, "");
		configInput(MONOIN11_INPUT, "");
		configInput(MONOIN4_INPUT, "");
		configInput(MONOIN12_INPUT, "");
		configInput(MONOIN5_INPUT, "");
		configInput(MONOIN13_INPUT, "");
		configInput(MONOIN6_INPUT, "");
		configInput(MONOIN14_INPUT, "");
		configInput(MONOIN7_INPUT, "");
		configInput(MONOIN15_INPUT, "");
		configInput(MONOIN8_INPUT, "");
		configInput(MONOIN16_INPUT, "");
		configOutput(POLYOUT_OUTPUT, "");
	}

	// added manually for easier loop through
	// actually ALLIN could be replaced with InputId everywhere
	int ALLIN[16]={MONOIN1_INPUT,MONOIN2_INPUT,MONOIN3_INPUT,
		MONOIN4_INPUT,MONOIN5_INPUT,MONOIN6_INPUT,
		MONOIN7_INPUT,MONOIN8_INPUT,MONOIN9_INPUT,
		MONOIN10_INPUT,MONOIN11_INPUT,MONOIN12_INPUT,
		MONOIN13_INPUT,MONOIN14_INPUT,MONOIN15_INPUT,
		MONOIN16_INPUT};

	// almost identical to PolyCopy10
	void process(const ProcessArgs& args) override {
		int totalPolyChans=1;
		float valOld=0;
		float valNew=0;
		
		if (outputs[POLYOUT_OUTPUT].isConnected()) {	// just some CPU saving		
			for (int c = 15; c >=0; c--) {
				if (! inputs[ALLIN[c]].isConnected()) {valNew=valOld;}
				else {valNew=inputs[ALLIN[c]].getVoltage();
					if (totalPolyChans==1) {totalPolyChans=c+1;}
				}
				outputs[POLYOUT_OUTPUT].setVoltage(valNew, c);
				valOld=valNew;
			}	
			outputs[POLYOUT_OUTPUT].channels = totalPolyChans;	
		}
	}
};


struct PolyCopy16Widget : ModuleWidget {
	PolyCopy16Widget(PolyCopy16* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/PolyCopy16.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(5.08, 25.4)), module, PolyCopy16::MONOIN1_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(15.24, 25.4)), module, PolyCopy16::MONOIN9_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(5.08, 35.56)), module, PolyCopy16::MONOIN2_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(15.24, 35.56)), module, PolyCopy16::MONOIN10_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(5.08, 45.72)), module, PolyCopy16::MONOIN3_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(15.24, 45.72)), module, PolyCopy16::MONOIN11_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(5.08, 55.88)), module, PolyCopy16::MONOIN4_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(15.24, 55.88)), module, PolyCopy16::MONOIN12_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(5.08, 66.04)), module, PolyCopy16::MONOIN5_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(15.24, 66.04)), module, PolyCopy16::MONOIN13_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(5.08, 76.2)), module, PolyCopy16::MONOIN6_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(15.24, 76.2)), module, PolyCopy16::MONOIN14_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(5.08, 86.36)), module, PolyCopy16::MONOIN7_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(15.24, 86.36)), module, PolyCopy16::MONOIN15_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(5.08, 96.52)), module, PolyCopy16::MONOIN8_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(15.24, 96.52)), module, PolyCopy16::MONOIN16_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(10.16, 111.76)), module, PolyCopy16::POLYOUT_OUTPUT));
	}
};

Model* modelPolyCopy16 = createModel<PolyCopy16, PolyCopy16Widget>("PolyCopy16");
