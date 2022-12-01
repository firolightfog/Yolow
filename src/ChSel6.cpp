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
		BUTTON_MODE_PARAM,
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
		configParam(SELECT_CH1_PARAM, 1.f, 16.f, 1.f, "First channel");
		configParam(SELECT_CH2_PARAM, 1.f, 16.f, 1.f, "Second channel");
		configParam(SELECT_CH3_PARAM, 1.f, 16.f, 1.f, "Third channel");
		configParam(SELECT_CH4_PARAM, 1.f, 16.f, 1.f, "Fourth channel");
		configParam(SELECT_CH5_PARAM, 1.f, 16.f, 1.f, "Fifth channel");
		configParam(SELECT_CH6_PARAM, 1.f, 16.f, 1.f, "Sixth channel");
		paramQuantities[SELECT_CH1_PARAM]->snapEnabled = true;
		paramQuantities[SELECT_CH2_PARAM]->snapEnabled = true;
		paramQuantities[SELECT_CH3_PARAM]->snapEnabled = true;
		paramQuantities[SELECT_CH4_PARAM]->snapEnabled = true;
		paramQuantities[SELECT_CH5_PARAM]->snapEnabled = true;
		paramQuantities[SELECT_CH6_PARAM]->snapEnabled = true;		
		configInput(POLYIN_INPUT, "");
		configOutput(POLYOUT_OUTPUT, "");
		configParam(BUTTON_MODE_PARAM, 0.f, 1.f, 0.f, "Select output mode");
		paramQuantities[BUTTON_MODE_PARAM]->snapEnabled = true;		
	}

	float paramVal[PARAMS_LEN]={0};
	const std::string valOutMode[7]={"rearranged poly channels",
	"any hits","if no hits",
	"only if one hit","only if not one hit",
	"if odd number of hits","only if not odd number of hits"};

	int indexChan=5; // this means 6 preferred outgoing channels
	int indexOutMode=0;	// defines the output mode (all selected, mono OR, inverted OR, )
	int loop=0;     // save some CPU in process()

	void process(const ProcessArgs& args) override {

		int newVolt=0;

		if (loop--<=0) {
			loop=15421;
			for (int p=0;p<PARAMS_LEN;p++) {paramVal[p]=params[p].getValue();}
			if (paramVal[BUTTON_MODE_PARAM]==1) {
				indexOutMode++;
				if (indexOutMode>=7) {indexOutMode=0;}
			}
			paramQuantities[BUTTON_MODE_PARAM]->description =  valOutMode[indexOutMode]; 
			paramQuantities[SELECT_CH1_PARAM]->description =  valOutMode[indexOutMode]; 
			paramQuantities[SELECT_CH2_PARAM]->description =  valOutMode[indexOutMode]; 
			paramQuantities[SELECT_CH3_PARAM]->description =  valOutMode[indexOutMode]; 
			paramQuantities[SELECT_CH4_PARAM]->description =  valOutMode[indexOutMode]; 
			paramQuantities[SELECT_CH5_PARAM]->description =  valOutMode[indexOutMode]; 
			paramQuantities[SELECT_CH6_PARAM]->description =  valOutMode[indexOutMode]; 
		}
	
		if (indexOutMode==0) {
			outputs[POLYOUT_OUTPUT].channels=indexChan+1;
			for (int c=0;c<=indexChan;c++) {
				// outputs[POLYOUT_OUTPUT].setVoltage(inputs[POLYIN_INPUT].getVoltage(params[SELECT_CH1_PARAM+c].getValue()-1), c);
				outputs[POLYOUT_OUTPUT].setVoltage(inputs[POLYIN_INPUT].getVoltage(paramVal[SELECT_CH1_PARAM+c]-1), c);
			}
		}
		else if (indexOutMode==0) { // return a selection of channels in a polyphonic output
			outputs[POLYOUT_OUTPUT].channels=indexChan+1;
			outputs[POLYOUT_OUTPUT].setVoltage(inputs[POLYIN_INPUT].getVoltage(params[SELECT_CH1_PARAM].getValue()-1), 0);
			outputs[POLYOUT_OUTPUT].setVoltage(inputs[POLYIN_INPUT].getVoltage(params[SELECT_CH2_PARAM].getValue()-1), 1);
			outputs[POLYOUT_OUTPUT].setVoltage(inputs[POLYIN_INPUT].getVoltage(params[SELECT_CH3_PARAM].getValue()-1), 2);
			outputs[POLYOUT_OUTPUT].setVoltage(inputs[POLYIN_INPUT].getVoltage(params[SELECT_CH4_PARAM].getValue()-1), 3);
			outputs[POLYOUT_OUTPUT].setVoltage(inputs[POLYIN_INPUT].getVoltage(params[SELECT_CH5_PARAM].getValue()-1), 4);
			outputs[POLYOUT_OUTPUT].setVoltage(inputs[POLYIN_INPUT].getVoltage(params[SELECT_CH6_PARAM].getValue()-1), 5);
		}

		else {

			// count the number of +2V in the poly input
			outputs[POLYOUT_OUTPUT].channels=1;
			for (int c=0;c<=indexChan;c++) {
				newVolt+=(inputs[POLYIN_INPUT].getVoltage(paramVal[SELECT_CH1_PARAM+c]-1)>2.0f)?1:0;
			}

			// define the monopohnic output based on the newVolt
			if (indexOutMode==1) {	// OR
				outputs[POLYOUT_OUTPUT].setVoltage((newVolt>0)?10:0);
			}
		
			else if (indexOutMode==2) {	// NOR
				outputs[POLYOUT_OUTPUT].setVoltage((newVolt>0)?0:10);
			}

			else if (indexOutMode==3) {	// solo XOR 
				outputs[POLYOUT_OUTPUT].setVoltage((newVolt==1)?10:0);
			}

			else if (indexOutMode==4) {	// solo NXOR
				outputs[POLYOUT_OUTPUT].setVoltage((newVolt==1)?0:10);
			}

			else if (indexOutMode==5) {	// odd XOR     
				outputs[POLYOUT_OUTPUT].setVoltage((newVolt % 2 ==1)?10:0);
			}

			else if (indexOutMode==6) {	// odd NXOR
				outputs[POLYOUT_OUTPUT].setVoltage((newVolt % 2 ==1)?0:10);
			}

			else {	// unexpected indexOutMode
				outputs[POLYOUT_OUTPUT].channels=indexChan+1;			
				for (int c=0;c<=indexChan;c++) {outputs[POLYOUT_OUTPUT].setVoltage(-4.04, c);}
			}
		}
	}

	// this block is to save and reload a variable
	json_t* dataToJson() override {
	json_t* rootJ = json_object();
	json_object_set_new(rootJ, "mode", json_integer(indexChan));
	json_object_set_new(rootJ, "outmode", json_integer(indexOutMode));
	return rootJ;}

	void dataFromJson(json_t* rootJ) override {
	json_t *modeJ = json_object_get(rootJ, "mode");
	if (modeJ) indexChan = json_integer_value(modeJ);
	json_t *outmodeJ = json_object_get(rootJ, "outmode");
	if (outmodeJ) indexOutMode = json_integer_value(outmodeJ);
	}

};


struct ChSel6Widget : ModuleWidget {
	ChSel6Widget(ChSel6* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/ChSel6.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		float HP=5.08;

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(HP*2, HP*8.5)), module, ChSel6::SELECT_CH1_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(HP*2, HP*10.5)), module, ChSel6::SELECT_CH2_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(HP*2, HP*12.5)), module, ChSel6::SELECT_CH3_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(HP*2, HP*14.5)), module, ChSel6::SELECT_CH4_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(HP*2, HP*16.5)), module, ChSel6::SELECT_CH5_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(HP*2, HP*18.5)), module, ChSel6::SELECT_CH6_PARAM));

		addParam(createParamCentered<TL1105>(mm2px(Vec(HP*3.25, HP*19.75)), module, ChSel6::BUTTON_MODE_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(HP*2, HP*6)), module, ChSel6::POLYIN_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(HP*2, HP*22.5)), module, ChSel6::POLYOUT_OUTPUT));
	}
	
	void appendContextMenu(Menu* menu) override {
		ChSel6* module = dynamic_cast<ChSel6*>(this->module);
		assert(module);
		menu->addChild(new MenuSeparator);
		menu->addChild(createIndexPtrSubmenuItem("Output mode", 
			{"rearranged poly channels","any hits","no hits","only one hit","only if not one hit","odd number of hits","only if not odd number of hits"}, 
			&module->indexOutMode));
		menu->addChild(createIndexPtrSubmenuItem("Poly channels", {"1","2","3","4","5","6"}, &module->indexChan));
	}

};


Model* modelChSel6 = createModel<ChSel6, ChSel6Widget>("ChSel6");