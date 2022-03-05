/* README.md

### Xxx

This module is a sequencer and clock modulator.

##### INPUTS:
- `CLK` the clock input 
- `JUMP` the 0v to 10V input controls the 'SELECT' knob
##### KNOBS:
- `SELECT` selects from the nine available trigger sequences (aka block) 	
##### OUTPUTS:
- `THR` forwards the clock input (CLK) received.
- `DIV` outputs a clock input multiplied by the DIV knob.
- `LIFE` outputs the trigger sequence of the selected block.	
- `OUT` outputs TBD 	
- `OUT` outputs TBD 	
- `OUT` outputs TBD 	
- `OUT` outputs TBD 	
##### SHORTCUTS:
- `1-9` randomizes the division, block probabilty, and division probabilty of a block.
- `Shift 1-3` randomizes the division (1), block probabilty (2), or division probabilty (3) of all block.
##### ALSO RECOMMENDED: 
- Sha#Bang! Modules Stochastic Sequencer Grid is clearly a visually wonderful and feature rich version of the same concept. It served as an inspiration and (as I found it too complex) it was a motivation.

*/

#include "plugin.hpp"

struct Xxx : Module {
	enum ParamId {
		STEP1_PARAM,
		STEP2_PARAM,

		SLIDE1A_PARAM,
		SLIDE2A_PARAM,
		SLIDE3A_PARAM,
		SLIDE4A_PARAM,
		SLIDE5A_PARAM,
		SLIDE6A_PARAM,
		SLIDE7A_PARAM,
		SLIDE8A_PARAM,
		SLIDE9A_PARAM,

		SLIDE1B_PARAM,
		SLIDE2B_PARAM,
		SLIDE3B_PARAM,
		SLIDE4B_PARAM,
		SLIDE5B_PARAM,
		SLIDE6B_PARAM,
		SLIDE7B_PARAM,
		SLIDE8B_PARAM,
		SLIDE9B_PARAM,

		SLIDE1C_PARAM,
		SLIDE2C_PARAM,
		SLIDE3C_PARAM,
		SLIDE4C_PARAM,
		SLIDE5C_PARAM,
		SLIDE6C_PARAM,
		SLIDE7C_PARAM,
		SLIDE8C_PARAM,
		SLIDE9C_PARAM,

		PARAMS_LEN
	};
	enum InputId {
		JUMP_INPUT,
		CLK_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		OUT1_OUTPUT,
		OUT2_OUTPUT,
		OUT3_OUTPUT,
		OUT4_OUTPUT,
		OUT5_OUTPUT,
		OUTTHROUGH_OUTPUT,
		DEBUG_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		ENUMS(SLIDE_LIGHT,27),
		LIGHTS_LEN
	};

	Xxx() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(STEP1_PARAM, 1.f, 9.f, 1.f, "Select block");
		configParam(STEP2_PARAM, 1.f, 16.f, 1.f, "Select division");
		paramQuantities[STEP1_PARAM]->snapEnabled = true;
		paramQuantities[STEP2_PARAM]->snapEnabled = true;
		paramQuantities[STEP1_PARAM]->randomizeEnabled = false;
		paramQuantities[STEP2_PARAM]->randomizeEnabled = false;
		
		configParam(SLIDE1A_PARAM, 0.f, 16.f, 1.f, "Divison");
		configParam(SLIDE2A_PARAM, 0.f, 16.f, 1.f, "Divison");
		configParam(SLIDE3A_PARAM, 0.f, 16.f, 1.f, "Divison");
		configParam(SLIDE4A_PARAM, 0.f, 16.f, 1.f, "Divison");
		configParam(SLIDE5A_PARAM, 0.f, 16.f, 1.f, "Divison");
		configParam(SLIDE6A_PARAM, 0.f, 16.f, 1.f, "Divison");
		configParam(SLIDE7A_PARAM, 0.f, 16.f, 1.f, "Divison");
		configParam(SLIDE8A_PARAM, 0.f, 16.f, 1.f, "Divison");
		configParam(SLIDE9A_PARAM, 0.f, 16.f, 1.f, "Divison");

		configParam(SLIDE1B_PARAM, 0.f, 1.f, 1.f, "Block probability");
		configParam(SLIDE2B_PARAM, 0.f, 1.f, 1.f, "Block probability");
		configParam(SLIDE3B_PARAM, 0.f, 1.f, 1.f, "Block probability");
		configParam(SLIDE4B_PARAM, 0.f, 1.f, 1.f, "Block probability");
		configParam(SLIDE5B_PARAM, 0.f, 1.f, 1.f, "Block probability");
		configParam(SLIDE6B_PARAM, 0.f, 1.f, 1.f, "Block probability");
		configParam(SLIDE7B_PARAM, 0.f, 1.f, 1.f, "Block probability");
		configParam(SLIDE8B_PARAM, 0.f, 1.f, 1.f, "Block probability");
		configParam(SLIDE9B_PARAM, 0.f, 1.f, 1.f, "Block probability");

		configParam(SLIDE1C_PARAM, 0.f, 1.f, 1.f, "Div. probability");
		configParam(SLIDE2C_PARAM, 0.f, 1.f, 1.f, "Div. probability");
		configParam(SLIDE3C_PARAM, 0.f, 1.f, 1.f, "Div. probability");
		configParam(SLIDE4C_PARAM, 0.f, 1.f, 1.f, "Div. probability");
		configParam(SLIDE5C_PARAM, 0.f, 1.f, 1.f, "Div. probability");
		configParam(SLIDE6C_PARAM, 0.f, 1.f, 1.f, "Div. probability");
		configParam(SLIDE7C_PARAM, 0.f, 1.f, 1.f, "Div. probability");
		configParam(SLIDE8C_PARAM, 0.f, 1.f, 1.f, "Div. probability");
		configParam(SLIDE9C_PARAM, 0.f, 1.f, 1.f, "Div. probability");

		for (int c=SLIDE1A_PARAM;c<=SLIDE9A_PARAM;c++) {
			paramQuantities[c]->snapEnabled = true;
		}
		
		configInput(JUMP_INPUT, "Reset");
		configInput(CLK_INPUT, "Clock");
		configOutput(OUT1_OUTPUT, "Through");
		configOutput(OUT2_OUTPUT, "Modified");
		configOutput(OUT3_OUTPUT, "Active yellow voltage");
		configOutput(OUT4_OUTPUT, "Active white voltage");
		configOutput(OUT5_OUTPUT, "Active green voltage");
		configOutput(OUTTHROUGH_OUTPUT, "Clock through");
		configOutput(DEBUG_OUTPUT, "DEBUG");

	}


	int loopKnobs=0;	// CPU saviour for knob changes
	int theGap=1;		// to calculate PW
	int calcGap=1;		// to calculate PW

	int theDivX=1;		// multiplication do the quick knob
	int activeBlock=1;
	int theDiv=1;
	float probCell=1;
	float probDiv=0;
	bool isCell=true;
	bool isDiv=true;

	float oldVolt=0.5;	// save clock input voltage
	float newVolt=0.1;	// send the 9lives voltage

	bool newTrig=false;	// is the clock on
	bool oldTrig=false;	// was the clock off
	
	// **************************************************
	// this is a completely separate noise creation section
	int theNoise[25]={0};
	int calcNoise=-1;	
	// **************************************************
	
	void process(const ProcessArgs& args) override {

	// **************************************************
	// this is a completely separate noise creation section
	// **************************************************

		if (calcNoise<=0) {	for (int c=0;c<25;c++) {theNoise[c]=rack::random::uniform()*10;}}

		calcNoise++;
		if (calcNoise>=25) {calcNoise=0;}
		outputs[DEBUG_OUTPUT].setVoltage(theNoise[calcNoise]);
				
		calcGap=calcGap+1;
		// collect incoming clock voltage
		oldVolt=inputs[CLK_INPUT].getVoltage();
		// decide if the clock has changed
		newTrig=oldVolt>2;
		if (newTrig!=oldTrig) {
			// there was some kind of change
			oldTrig=newTrig;	
			// actually the clock is on
			if (newTrig) {

				// let's define the output according to the parameters
				isCell=rack::random::uniform()>probCell;
				isDiv=rack::random::uniform()>probDiv;

				// calculate the PW
				theGap=calcGap;
				calcGap=0;
								
			}
		}
			
		// send the outputs
		outputs[OUTTHROUGH_OUTPUT].setVoltage(oldVolt);
		outputs[OUT1_OUTPUT].setVoltage(10 * floor((1+theDivX*2*calcGap/theGap) % 2));
		outputs[OUT3_OUTPUT].setVoltage(theDiv/2);
		outputs[OUT4_OUTPUT].setVoltage(probCell);
		outputs[OUT5_OUTPUT].setVoltage(probDiv);
		if (isCell) {newVolt=0;}
		else if (isDiv) {
			newVolt=9.8 * floor((1+1*2*calcGap/theGap) % 2);
			// newVolt=(rack::random::uniform()>theDiv)?newVolt:1;
		}
		else {
			newVolt=10 * floor((1+theDiv*2*calcGap/theGap) % 2);
		}
		outputs[OUT2_OUTPUT].setVoltage(newVolt);
		
		if (loopKnobs--==0) {
			loopKnobs=500;
			if (inputs[JUMP_INPUT].isConnected()) {
				params[STEP1_PARAM].setValue(1+floor(abs(inputs[JUMP_INPUT].getVoltage())/10*8));
			}
			
			theDivX=params[STEP2_PARAM].getValue();
			if (theDivX>2) {
				// setPanel(createPanel(asset::plugin(pluginInstance, "res/Xxx.svg")));				
			}	
			else {
				// setPanel(createPanel(asset::plugin(pluginInstance, "res/Xxx-grey.svg")));
			}
			
			activeBlock=params[STEP1_PARAM].getValue();
			for (int c=0;c<27;c++) { 
				lights[SLIDE_LIGHT+c].setBrightness(0.425);
			}
			lights[SLIDE_LIGHT+(activeBlock-1)*3+0].setBrightness(1);
			lights[SLIDE_LIGHT+(activeBlock-1)*3+1].setBrightness(1);
			lights[SLIDE_LIGHT+(activeBlock-1)*3+2].setBrightness(1);
			// for (int c=0;c<27;c++) { 
				// lights[SLIDE_LIGHT+c].setBrightness(((activeBlock-1)*3+1==c)?10:0.25);
			// }
			theDiv=params[SLIDE1A_PARAM+activeBlock-1].getValue();
			probCell=params[SLIDE1B_PARAM+activeBlock-1].getValue();
			probDiv=params[SLIDE1C_PARAM+activeBlock-1].getValue();
			
		}
	}

	// randomize one of the blocks as the keystroke requires
	void contextRnd(int blockId) {
		params[SLIDE1A_PARAM+blockId].setValue(round(rack::random::uniform()*16));
		params[SLIDE1B_PARAM+blockId].setValue(rack::random::uniform());
		params[SLIDE1C_PARAM+blockId].setValue(rack::random::uniform());		
	}

	// randomize colors as the keystroke requires
	void contextRnd2(int colorId) {
		if (colorId==0) {
			for (int c=0; c<9; c++) {params[SLIDE1A_PARAM+c].setValue(round(rack::random::uniform()*16));}	
			// params[SLIDE1A_PARAM+0].setValue(round(rack::random::uniform()*16));
			// params[SLIDE1A_PARAM+1].setValue(round(rack::random::uniform()*16));
			// params[SLIDE1A_PARAM+2].setValue(round(rack::random::uniform()*16));
			// params[SLIDE1A_PARAM+3].setValue(round(rack::random::uniform()*16));
			// params[SLIDE1A_PARAM+4].setValue(round(rack::random::uniform()*16));
			// params[SLIDE1A_PARAM+5].setValue(round(rack::random::uniform()*16));
			// params[SLIDE1A_PARAM+6].setValue(round(rack::random::uniform()*16));
			// params[SLIDE1A_PARAM+7].setValue(round(rack::random::uniform()*16));
			// params[SLIDE1A_PARAM+8].setValue(round(rack::random::uniform()*16));
		}
		else if (colorId==1 || colorId==2) {
			for (int c=0; c<9; c++) {params[SLIDE1C_PARAM+c].setValue(rack::random::uniform());}
			// params[SLIDE1C_PARAM+0].setValue(rack::random::uniform());			
			// params[SLIDE1C_PARAM+1].setValue(rack::random::uniform());			
			// params[SLIDE1C_PARAM+2].setValue(rack::random::uniform());			
			// params[SLIDE1C_PARAM+3].setValue(rack::random::uniform());			
			// params[SLIDE1C_PARAM+4].setValue(rack::random::uniform());			
			// params[SLIDE1C_PARAM+5].setValue(rack::random::uniform());			
			// params[SLIDE1C_PARAM+6].setValue(rack::random::uniform());			
			// params[SLIDE1C_PARAM+7].setValue(rack::random::uniform());			
			// params[SLIDE1C_PARAM+8].setValue(rack::random::uniform());			
		}
		
	}


		int indexSkin=0;
		int getSkin() {return indexSkin;};
		
	// this block is to save and reload a variable
	json_t* dataToJson() override {
	json_t* rootJ = json_object();
	json_object_set_new(rootJ, "skin", json_boolean(indexSkin));
	return rootJ;}

	void dataFromJson(json_t* rootJ) override {
	json_t *skinJ = json_object_get(rootJ, "skin");
	if (skinJ) indexSkin = json_boolean_value(skinJ);}

};

struct XxxWidget : ModuleWidget {

    Xxx* module;

	XxxWidget(Xxx* module) {
        this->module = module;
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/Xxx-grey.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createLightParamCentered<LEDSliderYellow>(mm2px(Vec(7.62, 45.72)), 	module, Xxx::SLIDE1A_PARAM, Xxx::SLIDE_LIGHT+0));
		addParam(createLightParamCentered<LEDSliderWhite>(mm2px(Vec(12.7, 45.72)),  	module, Xxx::SLIDE1B_PARAM, Xxx::SLIDE_LIGHT+1));
		addParam(createLightParamCentered<LEDSliderGreen>(mm2px(Vec(17.78, 45.72)), 	module, Xxx::SLIDE1C_PARAM, Xxx::SLIDE_LIGHT+2));
		addParam(createLightParamCentered<LEDSliderYellow>(mm2px(Vec(25.4, 45.72)), 	module, Xxx::SLIDE2A_PARAM, Xxx::SLIDE_LIGHT+3));
		addParam(createLightParamCentered<LEDSliderWhite>(mm2px(Vec(30.48, 45.72)), 	module, Xxx::SLIDE2B_PARAM, Xxx::SLIDE_LIGHT+4));
		addParam(createLightParamCentered<LEDSliderGreen>(mm2px(Vec(35.56, 45.72)), 	module, Xxx::SLIDE2C_PARAM, Xxx::SLIDE_LIGHT+5));
		addParam(createLightParamCentered<LEDSliderYellow>(mm2px(Vec(43.18, 45.72)), 	module, Xxx::SLIDE3A_PARAM, Xxx::SLIDE_LIGHT+6));
		addParam(createLightParamCentered<LEDSliderWhite>(mm2px(Vec(48.26, 45.72)), 	module, Xxx::SLIDE3B_PARAM, Xxx::SLIDE_LIGHT+7));
		addParam(createLightParamCentered<LEDSliderGreen>(mm2px(Vec(53.34, 45.72)), 	module, Xxx::SLIDE3C_PARAM, Xxx::SLIDE_LIGHT+8));
		addParam(createLightParamCentered<LEDSliderYellow>(mm2px(Vec(7.62, 73.66)), 	module, Xxx::SLIDE4A_PARAM, Xxx::SLIDE_LIGHT+9));
		addParam(createLightParamCentered<LEDSliderWhite>(mm2px(Vec(12.7, 73.66)),  	module, Xxx::SLIDE4B_PARAM, Xxx::SLIDE_LIGHT+10));
		addParam(createLightParamCentered<LEDSliderGreen>(mm2px(Vec(17.78, 73.66)), 	module, Xxx::SLIDE4C_PARAM, Xxx::SLIDE_LIGHT+11));
		addParam(createLightParamCentered<LEDSliderYellow>(mm2px(Vec(25.4, 73.66)), 	module, Xxx::SLIDE5A_PARAM, Xxx::SLIDE_LIGHT+12));
		addParam(createLightParamCentered<LEDSliderWhite>(mm2px(Vec(30.48, 73.66)), 	module, Xxx::SLIDE5B_PARAM, Xxx::SLIDE_LIGHT+13));
		addParam(createLightParamCentered<LEDSliderGreen>(mm2px(Vec(35.56, 73.66)), 	module, Xxx::SLIDE5C_PARAM, Xxx::SLIDE_LIGHT+14));
		addParam(createLightParamCentered<LEDSliderYellow>(mm2px(Vec(43.18, 73.66)), 	module, Xxx::SLIDE6A_PARAM, Xxx::SLIDE_LIGHT+15));
		addParam(createLightParamCentered<LEDSliderWhite>(mm2px(Vec(48.26, 73.66)), 	module, Xxx::SLIDE6B_PARAM, Xxx::SLIDE_LIGHT+16));
		addParam(createLightParamCentered<LEDSliderGreen>(mm2px(Vec(53.34, 73.66)), 	module, Xxx::SLIDE6C_PARAM, Xxx::SLIDE_LIGHT+17));
		addParam(createLightParamCentered<LEDSliderYellow>(mm2px(Vec(7.62, 101.6)), 	module, Xxx::SLIDE7A_PARAM, Xxx::SLIDE_LIGHT+18));
		addParam(createLightParamCentered<LEDSliderWhite>(mm2px(Vec(12.7, 101.6)),  	module, Xxx::SLIDE7B_PARAM, Xxx::SLIDE_LIGHT+19));
		addParam(createLightParamCentered<LEDSliderGreen>(mm2px(Vec(17.78, 101.6)), 	module, Xxx::SLIDE7C_PARAM, Xxx::SLIDE_LIGHT+20));
		addParam(createLightParamCentered<LEDSliderYellow>(mm2px(Vec(25.4, 101.6)), 	module, Xxx::SLIDE8A_PARAM, Xxx::SLIDE_LIGHT+21));
		addParam(createLightParamCentered<LEDSliderWhite>(mm2px(Vec(30.48, 101.6)), 	module, Xxx::SLIDE8B_PARAM, Xxx::SLIDE_LIGHT+22));
		addParam(createLightParamCentered<LEDSliderGreen>(mm2px(Vec(35.56, 101.6)), 	module, Xxx::SLIDE8C_PARAM, Xxx::SLIDE_LIGHT+23));
		addParam(createLightParamCentered<LEDSliderYellow>(mm2px(Vec(43.18, 101.6)), 	module, Xxx::SLIDE9A_PARAM, Xxx::SLIDE_LIGHT+24));
		addParam(createLightParamCentered<LEDSliderWhite>(mm2px(Vec(48.26, 101.6)), 	module, Xxx::SLIDE9B_PARAM, Xxx::SLIDE_LIGHT+25));
		addParam(createLightParamCentered<LEDSliderGreen>(mm2px(Vec(53.34, 101.6)), 	module, Xxx::SLIDE9C_PARAM, Xxx::SLIDE_LIGHT+26));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(66.04, 66.04)), 	 module, Xxx::STEP1_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(66.04, 110.731)), 	 module, Xxx::STEP2_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(66.04, 29.264)), 	 module, Xxx::CLK_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(66.04, 79.865)), 	 module, Xxx::JUMP_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(66.04, 12.7)), 	 module, Xxx::OUTTHROUGH_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(66.04, 93.98)), 	 module, Xxx::OUT1_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(66.04, 50.463)), 	 module, Xxx::OUT2_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(27.94, 12.7)), 	 module, Xxx::OUT3_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(38.312, 12.7)), 	 module, Xxx::OUT4_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(48.472, 12.7)), 	 module, Xxx::OUT5_OUTPUT));

        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(5.08, 128.5-5.08)), 	 module, Xxx::DEBUG_OUTPUT));

	}

    void appendContextMenu(Menu* menu) override {
    Xxx* module = dynamic_cast<Xxx*>(this->module);
    assert(module);
    menu->addChild(new MenuSeparator);
	// menu->addChild(createIndexPtrSubmenuItem("Skin", {"Common","Grey"}, &module->indexSkin));	
	menu->addChild(createIndexPtrSubmenuItem("Skin", {"Common","Grey"}, &module->indexSkin));	
    menu->addChild(createMenuLabel("1-9: Randomize sliders of relevan block"));
    menu->addChild(createMenuLabel("Shift 1-3: Randomize sliders"));
    }

	void onHoverKey(const event::HoverKey &e) override {
		if (e.key >= GLFW_KEY_1 && e.key <= GLFW_KEY_9) {
			if (e.action == GLFW_PRESS) {
				float key_number = e.key - 49; // 49 is the ascii number for key #1
				if ((e.mods & RACK_MOD_MASK) == GLFW_MOD_SHIFT) {
					if (e.key <= GLFW_KEY_3) {module->contextRnd2(key_number);}
					}
				else {module->contextRnd(key_number);}
				e.consume(this);
			}
		}
		ModuleWidget::onHoverKey(e);
	}

};


Model* modelXxx = createModel<Xxx, XxxWidget>("Xxx");