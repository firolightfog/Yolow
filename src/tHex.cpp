/* README.md

### tHex

This is a 16-step trigger sequencer module. 

##### KNOBS:
- Four knobs are to change the 4-step pattern for the block. 
##### INPUTS:
- `CLK` advances to the next step 
- `RST` resets the sequencer
- 'IN 1-4' changes the pattern for the relevant block
##### BUTTONS:
- Four pushbuttons are to replace the 4-step pattern of the block with a random sequence. 
##### OUTPUTS:
- `OUT` outputs 0V or 10V pulses 
##### RIGHT-CLICK MENU:
- `PRECISION` messes up the pattern upon request (range perfect to drunk)
##### SHORTCUTS:
- `1-4` toggle random push buttons.
- `Shift 1-4` set random knob value.
##### LIGHTS:
- Red lights show the triggers.
- Green lights show the currently active block.
##### ALSO RECOMMENDED:
- Count Modula Nibble Sequencer is based on the same concept. It comes without precision settings but with external expanders.

*/


#include "plugin.hpp"


struct tHex : Module {
	enum ParamId {
		STEP1_PARAM,
		STEP2_PARAM,
		STEP3_PARAM,
		STEP4_PARAM,
		RNDBUTTON1_PARAM,
		RNDBUTTON2_PARAM,
		RNDBUTTON3_PARAM,
		RNDBUTTON4_PARAM,
		// PRECISION_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		MONOIN1_INPUT,
		MONOIN2_INPUT,
		MONOIN3_INPUT,
		MONOIN4_INPUT,
		MONOIN8_INPUT,
		MONOIN7_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		OUTIT_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		BLINK1_LIGHT,
		BLINK2_LIGHT,
		BLINK3_LIGHT,
		BLINK4_LIGHT,
		BLINK5_LIGHT,
		BLINK6_LIGHT,
		BLINK7_LIGHT,
		BLINK8_LIGHT,
		BLINK9_LIGHT,
		BLINK10_LIGHT,
		BLINK11_LIGHT,
		BLINK12_LIGHT,
		BLINK13_LIGHT,
		BLINK14_LIGHT,
		BLINK15_LIGHT,
		BLINK16_LIGHT,
		MUTELED1_LIGHT,
		MUTELED2_LIGHT,
		MUTELED3_LIGHT,
		MUTELED4_LIGHT,
		RNDLED1_LIGHT,
		RNDLED2_LIGHT,
		RNDLED3_LIGHT,
		RNDLED4_LIGHT,
		LIGHTS_LEN
	};

	tHex() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(STEP1_PARAM, 0.f, 15.f, 8.f, "Pattern controller #1");
		configParam(STEP2_PARAM, 0.f, 15.f, 8.f, "Pattern controller #2");
		configParam(STEP3_PARAM, 0.f, 15.f, 8.f, "Pattern controller #3");
		configParam(STEP4_PARAM, 0.f, 15.f, 8.f, "Pattern controller #4");
		paramQuantities[STEP1_PARAM]->snapEnabled = true;
		paramQuantities[STEP2_PARAM]->snapEnabled = true;
		paramQuantities[STEP3_PARAM]->snapEnabled = true;
		paramQuantities[STEP4_PARAM]->snapEnabled = true;
		// configParam(PRECISION_PARAM, 0.f, 1.f, 1.f, "A hidden parameter");
		configSwitch(RNDBUTTON1_PARAM, 0.f, 1.f, 0.f, "Random", {"Off", "On"});
		configSwitch(RNDBUTTON2_PARAM, 0.f, 1.f, 0.f, "Random", {"Off", "On"});
		configSwitch(RNDBUTTON3_PARAM, 0.f, 1.f, 0.f, "Random", {"Off", "On"});
		configSwitch(RNDBUTTON4_PARAM, 0.f, 1.f, 0.f, "Random", {"Off", "On"});
		configInput(MONOIN1_INPUT, "");
		configInput(MONOIN2_INPUT, "");
		configInput(MONOIN3_INPUT, "");
		configInput(MONOIN4_INPUT, "");
		configInput(MONOIN8_INPUT, "");
		configInput(MONOIN7_INPUT, "");
		configOutput(OUTIT_OUTPUT, "");
		for (int c=0;c<RNDLED1_LIGHT;c++) {lights[c].setBrightness(0.1);}
	}

	// --------------------------------------------------
	// shorthand to provide random nr.
	float dice() {return rack::random::uniform();}

	// --------------------------------------------------
	// to operate buttons as push buttons
	float ParamNow=0;
	float ParamLast[PARAMS_LEN];

	// --------------------------------------------------
	// to show current pattern 
	void strToLed(int paramId) {
		ParamNow=(int)(params[paramId].getValue());
		if (ParamLast[paramId]!=ParamNow) {
			// outputs[OUTIT_OUTPUT].setVoltage(ParamNow,1);
			if (paramId==STEP1_PARAM) {
				currPattern[0]=(int)floor(ParamNow/8)%2;
				currPattern[1]=(int)floor(ParamNow/4)%2;
				currPattern[2]=(int)floor(ParamNow/2)%2;
				currPattern[3]=(int)floor(ParamNow/1)%2;
				lights[BLINK1_LIGHT].setBrightness(currPattern[0]);
				lights[BLINK2_LIGHT].setBrightness(currPattern[1]);
				lights[BLINK3_LIGHT].setBrightness(currPattern[2]);
				lights[BLINK4_LIGHT].setBrightness(currPattern[3]);
			}
			else if (paramId==STEP2_PARAM) {
				currPattern[4]=(int)floor(ParamNow/8)%2;
				currPattern[5]=(int)floor(ParamNow/4)%2;
				currPattern[6]=(int)floor(ParamNow/2)%2;
				currPattern[7]=(int)floor(ParamNow/1)%2;
				lights[BLINK5_LIGHT].setBrightness(currPattern[4]);
				lights[BLINK6_LIGHT].setBrightness(currPattern[5]);
				lights[BLINK7_LIGHT].setBrightness(currPattern[6]);
				lights[BLINK8_LIGHT].setBrightness(currPattern[7]);
			}
			else if (paramId==STEP3_PARAM) {
				currPattern[8]=(int)floor(ParamNow/8)%2;
				currPattern[9]=(int)floor(ParamNow/4)%2;
				currPattern[10]=(int)floor(ParamNow/2)%2;
				currPattern[11]=(int)floor(ParamNow/1)%2;
				lights[BLINK9_LIGHT].setBrightness(currPattern[8]);
				lights[BLINK10_LIGHT].setBrightness(currPattern[9]);
				lights[BLINK11_LIGHT].setBrightness(currPattern[10]);
				lights[BLINK12_LIGHT].setBrightness(currPattern[11]);
			}
			else if (paramId==STEP4_PARAM) {
				currPattern[12]=(int)floor(ParamNow/8)%2;
				currPattern[13]=(int)floor(ParamNow/4)%2;
				currPattern[14]=(int)floor(ParamNow/2)%2;
				currPattern[15]=(int)floor(ParamNow/1)%2;
				lights[BLINK13_LIGHT].setBrightness(currPattern[12]);
				lights[BLINK14_LIGHT].setBrightness(currPattern[13]);
				lights[BLINK15_LIGHT].setBrightness(currPattern[14]);
				lights[BLINK16_LIGHT].setBrightness(currPattern[15]);
			}
		ParamLast[paramId]=ParamNow;
		}
	}

	int currStep=0;
	bool newClk=false;
	bool oldClk=false;
	bool newRst=false;
	bool oldRst=false;
	
	int indexPrec=0;	// 0 would mean a perfect drummer
	float arrPrec[7]={1,0.95,0.85,0.75,0.6,0.5,0.35};
	int hitPrec=0;		// current hit modified by the precision settings
	int loopCounterButtons = 1;
	int loopCounterKnobs = 1;
	int currPattern[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

	void contextRndK(int ex) {
		params[STEP1_PARAM+ex].setValue(rand() % 16);
	}

	void contextRnd(int ex) {
		bool rnde = params[RNDBUTTON1_PARAM+ex].getValue() > 0.f;
		params[RNDBUTTON1_PARAM+ex].setValue(!rnde);
	}


	// --------------------------------------------------
	// main routine
	void process(const ProcessArgs& args) override {

		if (loopCounterButtons-- ==0) {
			loopCounterButtons=500;

			// to handle the random buttons on the right
			bool rand1 = params[RNDBUTTON1_PARAM].getValue() > 0.f;
			bool rand2 = params[RNDBUTTON2_PARAM].getValue() > 0.f;
			bool rand3 = params[RNDBUTTON3_PARAM].getValue() > 0.f;
			bool rand4 = params[RNDBUTTON4_PARAM].getValue() > 0.f;
			lights[RNDLED1_LIGHT].setBrightness(rand1);
			lights[RNDLED2_LIGHT].setBrightness(rand2);
			lights[RNDLED3_LIGHT].setBrightness(rand3);
			lights[RNDLED4_LIGHT].setBrightness(rand4);

			if (ParamLast[RNDBUTTON1_PARAM]!=rand1) {ParamLast[RNDBUTTON1_PARAM]=rand1;}
			if (ParamLast[RNDBUTTON2_PARAM]!=rand2) {ParamLast[RNDBUTTON2_PARAM]=rand2;}
			if (ParamLast[RNDBUTTON3_PARAM]!=rand3) {ParamLast[RNDBUTTON3_PARAM]=rand3;}
			if (ParamLast[RNDBUTTON4_PARAM]!=rand4) {ParamLast[RNDBUTTON4_PARAM]=rand4;}

		}

		if (loopCounterKnobs-- ==0) {
			loopCounterKnobs=500;

			// to modify the knobs by the inputs
			if (inputs[MONOIN1_INPUT].isConnected()) {params[STEP1_PARAM].setValue(round(inputs[MONOIN1_INPUT].getVoltage()/10*16));}
			if (inputs[MONOIN2_INPUT].isConnected()) {params[STEP2_PARAM].setValue(round(inputs[MONOIN2_INPUT].getVoltage()/10*16));}
			if (inputs[MONOIN3_INPUT].isConnected()) {params[STEP3_PARAM].setValue(round(inputs[MONOIN3_INPUT].getVoltage()/10*16));}
			if (inputs[MONOIN4_INPUT].isConnected()) {params[STEP4_PARAM].setValue(round(inputs[MONOIN4_INPUT].getVoltage()/10*16));}
			strToLed(STEP1_PARAM);
			strToLed(STEP2_PARAM);
			strToLed(STEP3_PARAM);
			strToLed(STEP4_PARAM);
		}
	
		// to catch the reset signal
		newRst=inputs[MONOIN7_INPUT].getVoltage()>2;
		if (oldRst!=newRst) {currStep=0;oldRst=newRst;}
		
		// to catch the clock signal
		newClk=inputs[MONOIN8_INPUT].getVoltage()>2;
		if (oldClk!=newClk) {
			if (newClk) {
				if (currStep>=15) {currStep=0;} else {currStep++;}
				int currBlock=floor(currStep/4);
				lights[MUTELED1_LIGHT].setBrightness(currBlock==0?0.5:0);
				lights[MUTELED2_LIGHT].setBrightness(currBlock==1?0.5:0);
				lights[MUTELED3_LIGHT].setBrightness(currBlock==2?0.5:0);
				lights[MUTELED4_LIGHT].setBrightness(currBlock==3?0.5:0);				
				if (lights[RNDLED1_LIGHT+currBlock].getBrightness()>0.5) {
					outputs[OUTIT_OUTPUT].setVoltage((0.5>dice())?10:0);
				} else {
					hitPrec=currPattern[currStep];
					if (indexPrec!=0) {	// 0 would mean 100% precision
						// params[PRECISION_PARAM].setValue(indexPrec); 
						if (dice()>arrPrec[indexPrec]) {hitPrec=abs(hitPrec-1);}
					}
					outputs[OUTIT_OUTPUT].setVoltage(hitPrec*10);
				}
			} else {outputs[OUTIT_OUTPUT].setVoltage(0);}	
			oldClk=newClk;
		}		
	};
	
	// this block is to save and reload a variable
	json_t* dataToJson() override {
	json_t* rootJ = json_object();
	json_object_set_new(rootJ, "mode", json_integer(indexPrec));
	return rootJ;}

	void dataFromJson(json_t* rootJ) override {
	json_t *modeJ = json_object_get(rootJ, "mode");
	if (modeJ) indexPrec = json_integer_value(modeJ);}

	
};


struct tHexWidget : ModuleWidget {
	
	tHex* module;
		
	tHexWidget(tHex* module) {
		this->module = module;
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/tHex.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

addParam(createParamCentered<RoundBigBlackKnob>(mm2px(Vec(10.16, 2.54+30.48)), module, tHex::STEP1_PARAM));
addParam(createParamCentered<RoundBigBlackKnob>(mm2px(Vec(10.16, 2.54+50.80)), module, tHex::STEP2_PARAM));
addParam(createParamCentered<RoundBigBlackKnob>(mm2px(Vec(10.16, 2.54+71.12)), module, tHex::STEP3_PARAM));
addParam(createParamCentered<RoundBigBlackKnob>(mm2px(Vec(10.16, 2.54+91.44)), module, tHex::STEP4_PARAM));

addParam(createLightParamCentered<VCVLightLatch<MediumSimpleLight<WhiteLight>>>(mm2px(Vec(33.02, 2.54+25.4)), module, tHex::RNDBUTTON1_PARAM, tHex::RNDLED1_LIGHT));
addParam(createLightParamCentered<VCVLightLatch<MediumSimpleLight<WhiteLight>>>(mm2px(Vec(33.02, 2.54+45.72)), module, tHex::RNDBUTTON2_PARAM, tHex::RNDLED2_LIGHT));
addParam(createLightParamCentered<VCVLightLatch<MediumSimpleLight<WhiteLight>>>(mm2px(Vec(33.02, 2.54+65.828)), module, tHex::RNDBUTTON3_PARAM, tHex::RNDLED3_LIGHT));
addParam(createLightParamCentered<VCVLightLatch<MediumSimpleLight<WhiteLight>>>(mm2px(Vec(33.02, 2.54+86.36)), module, tHex::RNDBUTTON4_PARAM, tHex::RNDLED4_LIGHT));

addInput(createInputCentered<PJ301MPort>(mm2px(Vec(33.02, 2.54+35.56)), module, tHex::MONOIN1_INPUT));
addInput(createInputCentered<PJ301MPort>(mm2px(Vec(33.02, 2.54+55.88)), module, tHex::MONOIN2_INPUT));
addInput(createInputCentered<PJ301MPort>(mm2px(Vec(33.02, 2.54+76.20)), module, tHex::MONOIN3_INPUT));
addInput(createInputCentered<PJ301MPort>(mm2px(Vec(33.02, 2.54+96.52)), module, tHex::MONOIN4_INPUT));
addInput(createInputCentered<PJ301MPort>(mm2px(Vec(10.16+2.54, 2.54+109.22)), module, tHex::MONOIN8_INPUT));
addInput(createInputCentered<PJ301MPort>(mm2px(Vec(30.48, 2.54+109.22)), module, tHex::MONOIN7_INPUT));

addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(30.48, 15.24)), module, tHex::OUTIT_OUTPUT));

addChild(createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(27.94*0+10.16, 3*2.54+25.40)), module, tHex::MUTELED1_LIGHT));
addChild(createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(27.94*0+10.16, 3*2.54+45.72)), module, tHex::MUTELED2_LIGHT));
addChild(createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(27.94*0+10.16, 3*2.54+66.04)), module, tHex::MUTELED3_LIGHT));
addChild(createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(27.94*0+10.16, 3*2.54+86.36)), module, tHex::MUTELED4_LIGHT));

addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(2.54+20.32, 2.54+22.86)), module, tHex::BLINK1_LIGHT));
addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(2.54+20.32, 2.54+27.94)), module, tHex::BLINK2_LIGHT));
addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(2.54+20.32, 2.54+33.02)), module, tHex::BLINK3_LIGHT));
addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(2.54+20.32, 2.54+38.10)), module, tHex::BLINK4_LIGHT));
addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(2.54+20.32, 2.54+43.18)), module, tHex::BLINK5_LIGHT));
addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(2.54+20.32, 2.54+48.26)), module, tHex::BLINK6_LIGHT));
addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(2.54+20.32, 2.54+53.34)), module, tHex::BLINK7_LIGHT));
addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(2.54+20.32, 2.54+58.42)), module, tHex::BLINK8_LIGHT));
addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(2.54+20.32, 2.54+63.50)), module, tHex::BLINK9_LIGHT));
addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(2.54+20.32, 2.54+68.58)), module, tHex::BLINK10_LIGHT));
addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(2.54+20.32, 2.54+73.66)), module, tHex::BLINK11_LIGHT));
addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(2.54+20.32, 2.54+78.74)), module, tHex::BLINK12_LIGHT));
addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(2.54+20.32, 2.54+83.82)), module, tHex::BLINK13_LIGHT));
addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(2.54+20.32, 2.54+88.90)), module, tHex::BLINK14_LIGHT));
addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(2.54+20.32, 2.54+93.98)), module, tHex::BLINK15_LIGHT));
addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(2.54+20.32, 2.54+99.06)), module, tHex::BLINK16_LIGHT));
			
	}

		void appendContextMenu(Menu* menu) override {
		tHex* module = dynamic_cast<tHex*>(this->module);
		assert(module);
		menu->addChild(new MenuSeparator);
		menu->addChild(createIndexPtrSubmenuItem("Precision", {"Perfect (100%)","Very good (95%)","Good (85%)","Not bad (75%)","Wasted (60%)","Crap (50%)","Completely insane (35%)"}, &module->indexPrec));
		menu->addChild(createMenuLabel("1-4: Toggle random button"));
		menu->addChild(createMenuLabel("Shift 1-4: Set random knob value"));
		}

	void onHoverKey(const event::HoverKey &e) override {
		if (e.key >= GLFW_KEY_1 && e.key <= GLFW_KEY_4) {
			if (e.action == GLFW_PRESS) {
				float key_number = e.key - 49; // 49 is the ascii number for key #1
				if ((e.mods & RACK_MOD_MASK) == GLFW_MOD_SHIFT) {module->contextRndK(key_number);}
				else {module->contextRnd(key_number);}
				e.consume(this);
			}
		}
	ModuleWidget::onHoverKey(e);
	}
	
};

Model* modeltHex = createModel<tHex, tHexWidget>("tHex");