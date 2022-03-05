/* README.md

### MKnob

This is a module to provide four fix control voltages (CV). 

##### KNOBS:
- Knobs '1-4' allow to select a CV level each. 
##### OUTPUTS:
- Four monophonic `OUT` provide the four defined CVs. 
- `OUT` providing the four defined CVs. 
##### RIGHT-CLICK MENU:
- `QUANTIZE` selects between quantizition modes: `Nope`, `Octaves`, `Notes`.
- `RANGE` selects between `-5V to +5V` and `0V to +10V`.
- `POLY CHANNELS` allows to limit the output to 1 to 8 channels.
##### ALSO RECOMMENDED:
- Yossarian Slide6 with similar functions but with six voltages. 

*/

#include "plugin.hpp"

struct MKnob : Module {
	enum ParamId {
		KNOBA_PARAM,
		KNOBB_PARAM,
		KNOBC_PARAM,
		KNOBD_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		INPUTS_LEN
	};
	enum OutputId {
		SOCKETA_OUTPUT,
		SOCKETB_OUTPUT,
		SOCKETC_OUTPUT,
		SOCKETD_OUTPUT,
		POLYOUT_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		LIGHTS_LEN
	};

	MKnob() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(KNOBA_PARAM, -5, 5, 0, "Knob A", "");
		configParam(KNOBB_PARAM, -5, 5, 0, "Knob B", "");
		configParam(KNOBC_PARAM, -5, 5, 0, "Knob C", "");
		configParam(KNOBD_PARAM, -5, 5, 0, "Knob D", "V");
		paramQuantities[KNOBA_PARAM]->snapEnabled = false;
		paramQuantities[KNOBB_PARAM]->snapEnabled = false;
		paramQuantities[KNOBC_PARAM]->snapEnabled = false;
		paramQuantities[KNOBD_PARAM]->snapEnabled = false;
		configOutput(SOCKETA_OUTPUT, "A");
		configOutput(SOCKETB_OUTPUT, "B");
		configOutput(SOCKETC_OUTPUT, "C");
		configOutput(SOCKETD_OUTPUT, "D");
		configOutput(POLYOUT_OUTPUT, "Polyphonic");
	}

// these are OK
float newVal=0;	// reading the knob value
int loopKnobs=1;
int indexChan=3;	// this means 4 channels :)
int indexQuant=0;	// this means no quantization
int indexRange=0;

	float quantMe(float oldVal) {
		oldVal=oldVal+5*indexRange;
		if (indexQuant==0) {return oldVal;}
		else if (indexQuant==1) {return round(oldVal);}
		else {return round(oldVal*12)/12;}
	}

	void process(const ProcessArgs& args) override {
		
		if (loopKnobs-- ==0) {
			loopKnobs=500;

			// change the display only
			paramQuantities[KNOBA_PARAM]->displayOffset=5*indexRange;
			paramQuantities[KNOBB_PARAM]->displayOffset=5*indexRange;
			paramQuantities[KNOBC_PARAM]->displayOffset=5*indexRange;
			paramQuantities[KNOBD_PARAM]->displayOffset=5*indexRange;			
			
			// knob A
			newVal=params[KNOBA_PARAM].getValue();
			outputs[SOCKETA_OUTPUT].setVoltage(quantMe(newVal));
			outputs[POLYOUT_OUTPUT].setVoltage(quantMe(newVal), 0);

			// knob B
			newVal=params[KNOBB_PARAM].getValue();
			outputs[SOCKETB_OUTPUT].setVoltage(quantMe(newVal));
			outputs[POLYOUT_OUTPUT].setVoltage(quantMe(newVal), 1);

			// knob C
			newVal=params[KNOBC_PARAM].getValue();
			outputs[SOCKETC_OUTPUT].setVoltage(quantMe(newVal));
			outputs[POLYOUT_OUTPUT].setVoltage(quantMe(newVal), 2);

			// knob D
			newVal=params[KNOBD_PARAM].getValue();
			outputs[SOCKETD_OUTPUT].setVoltage(quantMe(newVal));
			outputs[POLYOUT_OUTPUT].setVoltage(quantMe(newVal), 3);
			
			outputs[POLYOUT_OUTPUT].channels=indexChan+1;
		
				}
	
	}

	// this block is to save and reload a variable
	json_t* dataToJson() override {
	json_t* rootJ = json_object();
	json_object_set_new(rootJ, "mode", json_integer(indexChan));
	json_object_set_new(rootJ, "range", json_integer(indexRange));
	json_object_set_new(rootJ, "quant", json_integer(indexQuant));
	return rootJ;}

	void dataFromJson(json_t* rootJ) override {
	json_t *modeJ = json_object_get(rootJ, "mode");
	if (modeJ) indexChan = json_integer_value(modeJ);
	json_t *rangeJ = json_object_get(rootJ, "range");
	if (rangeJ) indexRange = json_integer_value(rangeJ);
	json_t *quantJ = json_object_get(rootJ, "quant");
	if (quantJ) indexQuant = json_integer_value(quantJ);}

};

struct MKnobWidget : ModuleWidget {

	MKnobWidget(MKnob* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/MKnob.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(10.16, 25.4-2.54)), module, MKnob::KNOBA_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(10.16, 35.56-2.54)), module, MKnob::KNOBB_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(10.16, 45.72-2.54)), module, MKnob::KNOBC_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(10.16, 55.88-2.54)), module, MKnob::KNOBD_PARAM));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(10.16, 66.04)), module, MKnob::SOCKETA_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(10.16, 76.2)), module, MKnob::SOCKETB_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(10.16, 86.36)), module, MKnob::SOCKETC_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(10.16, 96.52)), module, MKnob::SOCKETD_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(10.16, 111.76)), module, MKnob::POLYOUT_OUTPUT));
	}
	
	void appendContextMenu(Menu* menu) override {
		MKnob* module = dynamic_cast<MKnob*>(this->module);
		assert(module);
		menu->addChild(new MenuSeparator);
		menu->addChild(createIndexPtrSubmenuItem("Quantize", {"Nope","Octaves","Notes"}, &module->indexQuant));
		menu->addChild(createIndexPtrSubmenuItem("Range", {"-5V to 5V","0 to 10V"}, &module->indexRange));
		menu->addChild(createIndexPtrSubmenuItem("Poly channels", {"1","2","3","4"}, &module->indexChan));
	}

};


Model* modelMKnob = createModel<MKnob, MKnobWidget>("MKnob");