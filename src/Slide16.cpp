/* README.md

### Slide16

This is a sequencer and fix voltage generator utility module. 

##### KNOBS:
- 'MIN' defines the lower threshold for the voltages. 
- 'MAX'  defines the upper threshold for the voltages. 
##### INPUTS:
- 'MIN' modifies the relevant knob. 
- 'MAX' modifies the relevant knob. 
- `RST` resets the sequencer.
- `STP` advances to the next step. 
- `JMP` overwrites `STP`  order and jumps directly to the dialled CV/slider.  
##### SLIDERS:
- Sixteen sliders providing a constant voltage within the allowed range.
##### OUTPUTS:
- `POLY` outputs the fixed voltages simultaneously on the 16 channels.
- `OUT` outputs the CV selected by the active step. 
##### SHORTCUTS:
- `1-4` randomizes all sliders of a block.
- `Shift 1-4` randomizes all sliders of a column.
##### RIGHT-CLICK MENU:
- `QUANTIZE` allows selection between "Nope", "Octaves", "Notes". 
- `POLY CHANNELS` defines the number of channels in the output (1-6).
##### ALSO RECOMMENDED:
- Slide6 with a mono inputs and a single output. 

*/

#include "plugin.hpp"


struct Slide16 : Module {
	enum ParamId {
		SLIDE1_PARAM,
		SLIDE2_PARAM,
		SLIDE3_PARAM,
		SLIDE4_PARAM,
		SLIDE5_PARAM,
		SLIDE6_PARAM,
		SLIDE7_PARAM,
		SLIDE8_PARAM,
		SLIDE9_PARAM,
		SLIDE10_PARAM,
		SLIDE11_PARAM,
		SLIDE12_PARAM,
		SLIDE13_PARAM,
		SLIDE14_PARAM,
		SLIDE15_PARAM,
		SLIDE16_PARAM,
		KNOBMIN_PARAM,
		KNOBMAX_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		SOCKETMIN_INPUT,
		SOCKETMAX_INPUT,
		SOCKETRESET_INPUT,
		SOCKETSTEP_INPUT,
		SOCKETJUMP_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		POLYOUT_OUTPUT,
		SEQOUT_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		ENUMS(SLIDE_LIGHT,16),
		LIGHTS_LEN
	};

	Slide16() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(KNOBMIN_PARAM, -10.f, 0.f, -5.f, "Min");
		configParam(KNOBMAX_PARAM, 0.f, 10.f, 5.f, "Max");
		paramQuantities[KNOBMIN_PARAM]->snapEnabled = true;
		paramQuantities[KNOBMAX_PARAM]->snapEnabled = true;
		paramQuantities[KNOBMIN_PARAM]->randomizeEnabled = false;
		paramQuantities[KNOBMAX_PARAM]->randomizeEnabled = false;
		configParam(SLIDE1_PARAM, 0.f, 1.f, 0.f, "");
		configParam(SLIDE2_PARAM, 0.f, 1.f, 0.f, "");
		configParam(SLIDE3_PARAM, 0.f, 1.f, 0.f, "");
		configParam(SLIDE4_PARAM, 0.f, 1.f, 0.f, "");
		configParam(SLIDE5_PARAM, 0.f, 1.f, 0.f, "");
		configParam(SLIDE6_PARAM, 0.f, 1.f, 0.f, "");
		configParam(SLIDE7_PARAM, 0.f, 1.f, 0.f, "");
		configParam(SLIDE8_PARAM, 0.f, 1.f, 0.f, "");
		configParam(SLIDE9_PARAM, 0.f, 1.f, 0.f, "");
		configParam(SLIDE10_PARAM, 0.f, 1.f, 0.f, "");
		configParam(SLIDE11_PARAM, 0.f, 1.f, 0.f, "");
		configParam(SLIDE12_PARAM, 0.f, 1.f, 0.f, "");
		configParam(SLIDE13_PARAM, 0.f, 1.f, 0.f, "");
		configParam(SLIDE14_PARAM, 0.f, 1.f, 0.f, "");
		configParam(SLIDE15_PARAM, 0.f, 1.f, 0.f, "");
		configParam(SLIDE16_PARAM, 0.f, 1.f, 0.f, "");
		configInput(SOCKETMIN_INPUT, "");
		configInput(SOCKETMAX_INPUT, "");
		configInput(SOCKETRESET_INPUT, "");
		configInput(SOCKETSTEP_INPUT, "");
		configInput(SOCKETJUMP_INPUT, "");
		configOutput(POLYOUT_OUTPUT, "");
		configOutput(SEQOUT_OUTPUT, "");
		getInputInfo(SOCKETJUMP_INPUT)->description = "Expects 0v-10v";
		highlightSlider(0);
	}

	// to highlight a slider LED and dim the rest
	void highlightSlider(int ledId) {
		for (int c=0;c<LIGHTS_LEN;c++) { 
			lights[SLIDE_LIGHT+c].setBrightness((c==ledId)?1:0.425);
		}
	}

	// to randomize slider values
	void contextRnd(int rndBlock) {		
		params[rndBlock*4+0].setValue(rack::random::uniform());
		params[rndBlock*4+1].setValue(rack::random::uniform());
		params[rndBlock*4+2].setValue(rack::random::uniform());
		params[rndBlock*4+3].setValue(rack::random::uniform());
	}

	// to randomize slider values
	void contextRnd2(int rndBlock) {
		rndBlock-=4;
		params[rndBlock+0].setValue(rack::random::uniform());
		params[rndBlock+4].setValue(rack::random::uniform());
		params[rndBlock+8].setValue(rack::random::uniform());
		params[rndBlock+12].setValue(rack::random::uniform());
	}

	bool oldClk=false;
	bool newClk=false;
	bool oldRst=false;
	bool newRst=false;

	int minVal=-10;		// min value for all six slider
	int maxVal=10;  	// max value for all six slider
	float valNew=0;		// value read from a slider
	int indexChan=15;	// this means 6 channels :)
	int indexQuant=0;	// this means no quantizing :)

	int currStep=0;
	float oldVals[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};	// for moderated CPU usage
	int loopKnobs=1;

    // the main routine
	void process(const ProcessArgs& args) override {
		if (loopKnobs-- ==0) {
			loopKnobs=1000;

			// if there is a min/max cable then move the knobs
			if (inputs[SOCKETMIN_INPUT].isConnected()) {
				params[KNOBMIN_PARAM].setValue(std::min(inputs[SOCKETMIN_INPUT].getVoltage(),0.f));
				}
			if (inputs[SOCKETMAX_INPUT].isConnected()) {
				params[KNOBMAX_PARAM].setValue(std::max(inputs[SOCKETMAX_INPUT].getVoltage(),0.f));
				}

			// record the knob positions
			minVal=params[KNOBMIN_PARAM].getValue();
			maxVal=params[KNOBMAX_PARAM].getValue();
			
			// format display messages
			for (int c=0; c<16; c++) {
				paramQuantities[SLIDE1_PARAM+c]->displayMultiplier = (maxVal-minVal);
				paramQuantities[SLIDE1_PARAM+c]->displayOffset = minVal;
			}
		}

		// collect output for polyphonic output
		if (outputs[POLYOUT_OUTPUT].isConnected()) {
			for (int c=0; c<=indexChan; c++) {
				valNew=params[SLIDE1_PARAM+c].getValue()*(maxVal-minVal)+minVal;
				if (indexQuant==1) {valNew=round(valNew);}
				else if (indexQuant==2) {valNew=round(valNew*12)/12;}
				outputs[POLYOUT_OUTPUT].setVoltage(valNew, c);
				oldVals[c]=valNew;
			}
			outputs[POLYOUT_OUTPUT].channels = indexChan+1;	
		}
		
		// collect output for monophonic/sequencer output
		if (inputs[SOCKETJUMP_INPUT].isConnected()) {
			valNew=floor(inputs[SOCKETJUMP_INPUT].getVoltage()/10*16);
			highlightSlider(valNew);
			valNew=params[SLIDE1_PARAM+valNew].getValue()*(maxVal-minVal)+minVal;
			if (indexQuant==1) {valNew=round(valNew);}
			else if (indexQuant==2) {valNew=round(valNew*12)/12;}			
			outputs[SEQOUT_OUTPUT].setVoltage(valNew);
		}
		
		else if (inputs[SOCKETSTEP_INPUT].isConnected()) {
		
			// to catch the reset signal
			newRst=inputs[SOCKETRESET_INPUT].getVoltage()>2;
			if (oldRst!=newRst) {currStep=0;oldRst=newRst;}
			
			// to catch the clock signal
			newClk=inputs[SOCKETSTEP_INPUT].getVoltage()>2;
			if (oldClk!=newClk) {
				if (newClk) {
					if (currStep>=indexChan) {currStep=0;} else {currStep++;}
					highlightSlider(currStep);
					valNew=params[SLIDE1_PARAM+currStep].getValue()*(maxVal-minVal)+minVal;
					if (indexQuant==1) {valNew=round(valNew);}
					else if (indexQuant==2) {valNew=round(valNew*12)/12;}			
					outputs[SEQOUT_OUTPUT].setVoltage(valNew);
				} 
				oldClk=newClk;
			}
		}		
		
	}

	// this block is to save and reload a variable
	json_t* dataToJson() override {
	json_t* rootJ = json_object();
	json_object_set_new(rootJ, "mode", json_integer(indexChan));
	json_object_set_new(rootJ, "quant", json_integer(indexQuant));
	return rootJ;}

	void dataFromJson(json_t* rootJ) override {
	json_t *modeJ = json_object_get(rootJ, "mode");
	if (modeJ) indexChan = json_integer_value(modeJ);
	json_t *quantJ = json_object_get(rootJ, "quant");
	if (quantJ) indexQuant = json_integer_value(quantJ);}

};


struct Slide16Widget : ModuleWidget {

    Slide16* module;

	Slide16Widget(Slide16* module) {
        this->module = module;
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/Slide16.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(7.62, 25.4)), module, Slide16::KNOBMIN_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(17.78, 25.4)), module, Slide16::KNOBMAX_PARAM));

		addParam(createLightParamCentered<LEDSliderWhite>(mm2px(Vec(30.48, 22.86)), module, Slide16::SLIDE1_PARAM, Slide16::SLIDE_LIGHT+0));
		addParam(createLightParamCentered<LEDSliderWhite>(mm2px(Vec(35.56, 22.86)), module, Slide16::SLIDE2_PARAM, Slide16::SLIDE_LIGHT+1));
		addParam(createLightParamCentered<LEDSliderWhite>(mm2px(Vec(40.64, 22.86)), module, Slide16::SLIDE3_PARAM, Slide16::SLIDE_LIGHT+2));
		addParam(createLightParamCentered<LEDSliderWhite>(mm2px(Vec(45.72, 22.86)), module, Slide16::SLIDE4_PARAM, Slide16::SLIDE_LIGHT+3));
		
		addParam(createLightParamCentered<LEDSliderWhite>(mm2px(Vec(30.48, 50.8)), module, Slide16::SLIDE5_PARAM, Slide16::SLIDE_LIGHT+4));
		addParam(createLightParamCentered<LEDSliderWhite>(mm2px(Vec(35.56, 50.8)), module, Slide16::SLIDE6_PARAM, Slide16::SLIDE_LIGHT+5));
		addParam(createLightParamCentered<LEDSliderWhite>(mm2px(Vec(40.64, 50.8)), module, Slide16::SLIDE7_PARAM, Slide16::SLIDE_LIGHT+6));
		addParam(createLightParamCentered<LEDSliderWhite>(mm2px(Vec(45.72, 50.8)), module, Slide16::SLIDE8_PARAM, Slide16::SLIDE_LIGHT+7));
		
		addParam(createLightParamCentered<LEDSliderWhite>(mm2px(Vec(30.48, 78.74)), module, Slide16::SLIDE9_PARAM, Slide16::SLIDE_LIGHT+8));
		addParam(createLightParamCentered<LEDSliderWhite>(mm2px(Vec(35.56, 78.74)), module, Slide16::SLIDE10_PARAM, Slide16::SLIDE_LIGHT+9));
		addParam(createLightParamCentered<LEDSliderWhite>(mm2px(Vec(40.64, 78.74)), module, Slide16::SLIDE11_PARAM, Slide16::SLIDE_LIGHT+10));
		addParam(createLightParamCentered<LEDSliderWhite>(mm2px(Vec(45.72, 78.74)), module, Slide16::SLIDE12_PARAM, Slide16::SLIDE_LIGHT+11));
		
		addParam(createLightParamCentered<LEDSliderWhite>(mm2px(Vec(30.48, 106.681)), module, Slide16::SLIDE13_PARAM, Slide16::SLIDE_LIGHT+12));
		addParam(createLightParamCentered<LEDSliderWhite>(mm2px(Vec(35.56, 106.681)), module, Slide16::SLIDE14_PARAM, Slide16::SLIDE_LIGHT+13));
		addParam(createLightParamCentered<LEDSliderWhite>(mm2px(Vec(40.64, 106.681)), module, Slide16::SLIDE15_PARAM, Slide16::SLIDE_LIGHT+14));
		addParam(createLightParamCentered<LEDSliderWhite>(mm2px(Vec(45.72, 106.681)), module, Slide16::SLIDE16_PARAM, Slide16::SLIDE_LIGHT+15));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.62, 40.64)), module, Slide16::SOCKETMIN_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(17.78, 40.64)), module, Slide16::SOCKETMAX_INPUT));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(12.7, 68.58)), module, Slide16::SOCKETRESET_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(12.7, 81.28)), module, Slide16::SOCKETSTEP_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(12.7, 93.98)), module, Slide16::SOCKETJUMP_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(12.7, 55.88)), module, Slide16::POLYOUT_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(12.7, 111.76)), module, Slide16::SEQOUT_OUTPUT));
	}

	// context menu
	void appendContextMenu(Menu* menu) override {
		Slide16* module = dynamic_cast<Slide16*>(this->module);
		assert(module);
		menu->addChild(new MenuSeparator);
		menu->addChild(createIndexPtrSubmenuItem("Quantize", {"Nope","Octaves","Notes"}, &module->indexQuant));
		menu->addChild(createIndexPtrSubmenuItem("Poly channels", {"1","2","3","4","5","6","7","8","9","10","11","12","13","14","15","16"}, &module->indexChan));
		menu->addChild(new MenuSeparator);
		menu->addChild(createMenuLabel("1-4: Randomize a block of sliders"));
		menu->addChild(createMenuLabel("Shift 1-4: Randomize a column of sliders"));
	}

	// shortkey
	void onHoverKey(const event::HoverKey &e) override {
		if (e.key >= GLFW_KEY_1 && e.key <= GLFW_KEY_4) {
			if (e.action == GLFW_PRESS) {
				float key_number = e.key - 49; // 49 is the ascii number for key #1
				if ((e.mods & RACK_MOD_MASK) == GLFW_MOD_SHIFT) {module->contextRnd2(key_number+4);}
				else {module->contextRnd(key_number);}
				e.consume(this);
			}
		}
	ModuleWidget::onHoverKey(e);
	}

};


Model* modelSlide16 = createModel<Slide16, Slide16Widget>("Slide16");