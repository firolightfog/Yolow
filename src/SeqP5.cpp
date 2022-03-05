/* README.md

### SeqP5

This module is a sequential switch selection one of the 5 inputs

##### INPUTS:
- `IN 1-4` accepts any control voltages.
- `CLK` steps through the four steps represented by the knobs.
- `RST` common reset function restarting the sequence from the beginning.
##### KNOBS:
- All four knobs select one of the input each. Selecting 0 allows automatic (spell: random) selection of input.
##### OUTPUTS:
- `OUT` is a common output with the currently selected input CV.
##### LIGHTS:
- Red lights show the current step.
- Yellow lights show the currently selected input.
##### ALSO RECOMMENDED:
- SeqP16 has similar features but with multiple channels of a polyphonic inputs to choose from.

*/

#include "plugin.hpp"


struct SeqP5 : Module {
	enum ParamId {
		STEP1_PARAM,
		STEP2_PARAM,
		STEP3_PARAM,
		STEP4_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		MONOIN1_INPUT,
		MONOIN2_INPUT,
		MONOIN3_INPUT,
		MONOIN4_INPUT,
		MONOIN5_INPUT,
		MONOIN7_INPUT,
		MONOIN8_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		OUTIT_OUTPUT,
		DEBUG_OUTPUT, 
		OUTPUTS_LEN
	};
	enum LightId {
		BLINK1_LIGHT,
		BLINK2_LIGHT,
		BLINK3_LIGHT,
		BLINK4_LIGHT,
		BLINK5_LIGHT,
		STEP1_LIGHT,
		STEP2_LIGHT,
		STEP3_LIGHT,
		STEP4_LIGHT,
		LIGHTS_LEN
	};

	SeqP5() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(STEP1_PARAM, 0.f, 5.f, 1.f, "Step 1");
		configParam(STEP2_PARAM, 0.f, 5.f, 2.f, "Step 2");
		configParam(STEP3_PARAM, 0.f, 5.f, 3.f, "Step 3");
		configParam(STEP4_PARAM, 0.f, 5.f, 4.f, "Step 4");
		paramQuantities[STEP1_PARAM]->snapEnabled = true;
		paramQuantities[STEP2_PARAM]->snapEnabled = true;
		paramQuantities[STEP3_PARAM]->snapEnabled = true;
		paramQuantities[STEP4_PARAM]->snapEnabled = true;
		configInput(MONOIN1_INPUT, "Source #1");
		configInput(MONOIN2_INPUT, "Source #2");
		configInput(MONOIN3_INPUT, "Source #3");
		configInput(MONOIN4_INPUT, "Source #4");
		configInput(MONOIN5_INPUT, "Source #5");
		configInput(MONOIN7_INPUT, "Reset");
		configInput(MONOIN8_INPUT, "Clock");
		configOutput(OUTIT_OUTPUT, "Mono");
	}

	int stepPos=1;
	bool clkOld=false;
	bool clkNew=false;
	bool rstOld=false;
	bool rstNew=false;
	//  bool holdIt=false;
	int holdThis=0;
	
	int loop=0;
	float knobVal[4]={0};
	int requestedOut=0;

	void process(const ProcessArgs& args) override {
		
		// is there a reset?
		rstNew=inputs[MONOIN7_INPUT].getVoltage()>2;
		if ((rstNew==true) && (rstOld==false)) {stepPos=1;}

		// is there a clock?
		// clkNew=inputs[MONOIN8_INPUT].getVoltage()>2;
		// if ((clkNew==true) && (clkOld==false)) {
			// holdThis=(rand() % 5);
			// if (stepPos>=4) {stepPos=1;} else {stepPos++;}
		// }

		if (inputs[MONOIN8_INPUT].isConnected()) {
			// is there a clock?
			clkNew=inputs[MONOIN8_INPUT].getVoltage()>2;
			if ((clkNew==true) && (clkOld==false)) {
				holdThis=(rand() % 5);
				if (stepPos>=4) {stepPos=1;} else {stepPos++;}
			}
		} 
		else {
			// is there a knob move?
			if (loop--<=0) {
				loop=1000;
				for (int c=0;c<4;c++) {
					if (knobVal[c]!=params[STEP1_PARAM+c].getValue()) {
						knobVal[c]=params[STEP1_PARAM+c].getValue();
						stepPos=c+1;
					}
				}
			}				
		}
			
		// let's remember these values	
		clkOld=clkNew;
		rstOld=rstNew;		

		// tell me which one needs to be sent
		requestedOut=params[stepPos-1].getValue();
		requestedOut=(requestedOut<=0)?holdThis:requestedOut-1;
		lights[BLINK1_LIGHT].setBrightness(0);
		lights[BLINK2_LIGHT].setBrightness(0);
		lights[BLINK3_LIGHT].setBrightness(0);
		lights[BLINK4_LIGHT].setBrightness(0);						
		lights[BLINK5_LIGHT].setBrightness(0);	
		lights[BLINK1_LIGHT+requestedOut].setBrightness(10);	
		lights[STEP1_LIGHT].setBrightness(0);
		lights[STEP2_LIGHT].setBrightness(0);
		lights[STEP3_LIGHT].setBrightness(0);
		lights[STEP4_LIGHT].setBrightness(0);	
		lights[STEP1_LIGHT+stepPos-1].setBrightness(0.5);

		// send the selected input to the output!		
		outputs[OUTIT_OUTPUT].setVoltage(inputs[requestedOut].getVoltage());

	}

/*	DELETE

	// randomize one of the blocks as the keystroke requires
	void contextRnd(int blockId) {
		params[STEP1_PARAM+blockId].setValue(round(rack::random::uniform()*5));
	}

	// returns the selected source if no clock is connected
	void contextSel(int blockId) {
		if (!inputs[MONOIN8_INPUT].isConnected()) {stepPos=blockId+1;}
		};
*/
	
};


struct SeqP5Widget : ModuleWidget {

    SeqP5* module;
	
	SeqP5Widget(SeqP5* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/SeqP5.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(22.86, 25.4)), module, SeqP5::MONOIN1_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(22.86, 40.64)), module, SeqP5::MONOIN2_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(22.86, 55.88)), module, SeqP5::MONOIN3_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(22.86, 71.12)), module, SeqP5::MONOIN4_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(22.86, 86.36)), module, SeqP5::MONOIN5_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.62, 101.6)), module, SeqP5::MONOIN7_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.62, 111.76)), module, SeqP5::MONOIN8_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(22.86, 107.475)), module, SeqP5::OUTIT_OUTPUT));

		addChild(createLightCentered<MediumLight<YellowLight>>(mm2px(Vec(15.24+5.04/2, 5.04+25.4)), module, SeqP5::BLINK1_LIGHT));
		addChild(createLightCentered<MediumLight<YellowLight>>(mm2px(Vec(15.24+5.04/2, 5.04+40.64)), module, SeqP5::BLINK2_LIGHT));
		addChild(createLightCentered<MediumLight<YellowLight>>(mm2px(Vec(15.24+5.04/2, 5.04+55.774)), module, SeqP5::BLINK3_LIGHT));
		addChild(createLightCentered<MediumLight<YellowLight>>(mm2px(Vec(15.24+5.04/2, 5.04+71.014)), module, SeqP5::BLINK4_LIGHT));
		addChild(createLightCentered<MediumLight<YellowLight>>(mm2px(Vec(15.24+5.04/2, 5.04+86.36)), module, SeqP5::BLINK5_LIGHT));

		// addChild(createLightCentered<MediumLight<YellowLight>>(mm2px(Vec(15.24, 25.4)), module, SeqP5::BLINK1_LIGHT));
		// addChild(createLightCentered<MediumLight<YellowLight>>(mm2px(Vec(15.24, 40.64)), module, SeqP5::BLINK2_LIGHT));
		// addChild(createLightCentered<MediumLight<YellowLight>>(mm2px(Vec(15.134, 55.774)), module, SeqP5::BLINK3_LIGHT));
		// addChild(createLightCentered<MediumLight<YellowLight>>(mm2px(Vec(15.134, 71.014)), module, SeqP5::BLINK4_LIGHT));
		// addChild(createLightCentered<MediumLight<YellowLight>>(mm2px(Vec(15.24, 86.36)), module, SeqP5::BLINK5_LIGHT));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(7.62, 33.02)), module, SeqP5::STEP1_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(7.62, 48.26)), module, SeqP5::STEP2_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(7.62, 63.5)), module, SeqP5::STEP3_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(7.62, 78.74)), module, SeqP5::STEP4_PARAM));

		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(7.62, 33.02)), module, SeqP5::STEP1_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(7.62, 48.26)), module, SeqP5::STEP2_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(7.62, 63.5)), module, SeqP5::STEP3_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(7.62, 78.74)), module, SeqP5::STEP4_LIGHT));

		// addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(15.24, 33.02)), module, SeqP5::STEP1_LIGHT));
		// addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(15.24, 48.26)), module, SeqP5::STEP2_LIGHT));
		// addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(15.24, 63.5)), module, SeqP5::STEP3_LIGHT));
		// addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(15.24, 78.74)), module, SeqP5::STEP4_LIGHT));

        // addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(5.08, 128.5-5.08)), 	 module, SeqP5::DEBUG_OUTPUT));
		
	}
	
	
	// void appendContextMenu(Menu* menu) override {
		// SeqP5* module = dynamic_cast<SeqP5*>(this->module);
		// assert(module);
		// menu->addChild(new MenuSeparator);
		// menu->addChild(createMenuLabel("1-5: Return the selected source"));
		// menu->addChild(createMenuLabel("Shift 1-4: Randomizes the knobs"));
	// }

	// void onHoverKey(const event::HoverKey &e) override {
		// if (e.key >= GLFW_KEY_1 && e.key <= GLFW_KEY_5) {
			// if (e.action == GLFW_PRESS) {
				// float key_number = e.key - 49; // 49 is the ascii number for key #1
				// if ((e.mods & RACK_MOD_MASK) == GLFW_MOD_SHIFT) {
					// if (e.key < GLFW_KEY_5) {module->contextRnd(key_number);}
					// }
				// else {module->contextSel(key_number);}
				// e.consume(this);
			// }
		// }
		// ModuleWidget::onHoverKey(e);
	// }


};


Model* modelSeqP5 = createModel<SeqP5, SeqP5Widget>("SeqP5");