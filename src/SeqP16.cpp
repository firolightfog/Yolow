/* README.md

### SeqP16

This is a sequential switch to choose from channels of a polyphonic input. 

##### INPUTS:
- `RST` resets the sequencer.
- `CLK` advances to the next step. 
- 'POLYIN' receives a polyphonic cable with multiple channels.
##### KNOBS:
- Four knobs represent the four steps of the sequential switch and define the channel to be selected. 
##### OUTPUTS:
- `OUT` outputs the channel selected by the active step.
##### LIGHTS:
- Red lights show the current step.
##### ALSO RECOMMENDED:
- SeqP5 has similar features but with five monophonic inputs to choose from.

*/

#include "plugin.hpp"

struct SeqP16 : Module {

	// let's set the references
	enum ParamId {
		SELECT_CH1_PARAM,
		SELECT_CH2_PARAM,
		SELECT_CH3_PARAM,
		SELECT_CH4_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		RST_INPUT,
		CLK_INPUT,
		POLYIN_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		MONOOUT_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		BLINK1_LIGHT,
		BLINK2_LIGHT,
		BLINK3_LIGHT,
		BLINK4_LIGHT,
		LIGHTS_LEN
	};

	// let's create the elements
	SeqP16() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(SELECT_CH1_PARAM, 0.f, 16.f, 1.f, "Step 1");
		configParam(SELECT_CH2_PARAM, 0.f, 16.f, 0.f, "Step 2");
		configParam(SELECT_CH3_PARAM, 0.f, 16.f, 0.f, "Step 3");
		configParam(SELECT_CH4_PARAM, 0.f, 16.f, 0.f, "Step 4");
		paramQuantities[SELECT_CH1_PARAM]->snapEnabled = true;
		paramQuantities[SELECT_CH2_PARAM]->snapEnabled = true;
		paramQuantities[SELECT_CH3_PARAM]->snapEnabled = true;
		paramQuantities[SELECT_CH4_PARAM]->snapEnabled = true;
		configInput(RST_INPUT, "");
		configInput(CLK_INPUT, "");
		configInput(POLYIN_INPUT, "");
		configOutput(MONOOUT_OUTPUT, "");
	}

	int stepPos=0;  	// to define the active step (1, 2, 3, 4)
	bool clkOld=false;	// to catch the clock pulse
	bool clkNew=false;	// to catch the clock pulse
	bool rstOld=false;	// to catch the reset pulse
	bool rstNew=false;	// to catch the reset pulse

	int nrChan=1;   	// to record number of POLYIN_INPUT channels
	int rdChan=1;   	// to record number of POLYIN_INPUT channels
	bool allowEmpty;	// to handle the case of knob requesting a greater channel than available

	void process(const ProcessArgs& args) override {
		
		// is there a reset? jump to first step!
		rstNew=inputs[RST_INPUT].getVoltage()>2;
		if ((rstNew==true) && (rstOld==false)) {
			stepPos=1;
			lights[BLINK1_LIGHT].setBrightness(10);
			lights[BLINK2_LIGHT].setBrightness(0);
			lights[BLINK3_LIGHT].setBrightness(0);
			lights[BLINK4_LIGHT].setBrightness(0);						
		}

		// is there a clock? jump to the next step!
		if (!inputs[CLK_INPUT].isConnected()) {
			stepPos=1;
			nrChan=(inputs[POLYIN_INPUT].isConnected())?inputs[POLYIN_INPUT].channels:1;
			rdChan=0;
			lights[BLINK1_LIGHT].setBrightness(10);
			lights[BLINK2_LIGHT].setBrightness(0);
			lights[BLINK3_LIGHT].setBrightness(0);
			lights[BLINK4_LIGHT].setBrightness(0);									
		}
		else {
			clkNew=inputs[CLK_INPUT].getVoltage()>2;
			if ((clkNew==true) && (clkOld==false)) {
				// and monitor the number of channels
				nrChan=(inputs[POLYIN_INPUT].isConnected())?inputs[POLYIN_INPUT].channels:1;
				rdChan=(rand() % nrChan);
				if (stepPos>=4) {stepPos=1;} else {stepPos++;}
				lights[BLINK1_LIGHT].setBrightness((stepPos==1)?0.5:0);
				lights[BLINK2_LIGHT].setBrightness((stepPos==2)?0.5:0);
				lights[BLINK3_LIGHT].setBrightness((stepPos==3)?0.5:0);
				lights[BLINK4_LIGHT].setBrightness((stepPos==4)?0.5:0);						
			}
		}

		// let's remember them
		if (rstOld!=rstNew) {rstOld=rstNew;}
		if (clkOld!=clkNew) {clkOld=clkNew;}

		// tell me which one needs to be sent
		int requestedOut=params[stepPos-1].getValue()-1;	// sourece channels (1, 2, 3, ... 16) or 0 for random
		if (requestedOut>=nrChan || requestedOut<0) {requestedOut = rdChan;}
		
		// send the selected input to the output!
		outputs[MONOOUT_OUTPUT].setVoltage(inputs[POLYIN_INPUT].getVoltage(requestedOut));

	}
};

struct SeqP16Widget : ModuleWidget {
	SeqP16Widget(SeqP16* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/SeqP16.svg")));
	
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(10.16, 48.26)), module, SeqP16::SELECT_CH1_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(10.16, 58.42)), module, SeqP16::SELECT_CH2_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(10.16, 68.58)), module, SeqP16::SELECT_CH3_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(10.16, 78.74)), module, SeqP16::SELECT_CH4_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(10.16, 22.86)), module, SeqP16::RST_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(10.16, 35.56)), module, SeqP16::CLK_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(10.16, 114.3)), module, SeqP16::POLYIN_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(10.16, 96.52)), module, SeqP16::MONOOUT_OUTPUT));

		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(10.16, 48.26)), module, SeqP16::BLINK1_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(10.16, 58.42)), module, SeqP16::BLINK2_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(10.16, 68.58)), module, SeqP16::BLINK3_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(10.16, 78.74)), module, SeqP16::BLINK4_LIGHT));

		// addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(15.24, 53.34)), module, SeqP16::BLINK1_LIGHT));
		// addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(15.24, 63.5)), module, SeqP16::BLINK2_LIGHT));
		// addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(15.24, 73.66)), module, SeqP16::BLINK3_LIGHT));
		// addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(15.134, 83.714)), module, SeqP16::BLINK4_LIGHT));
	}

	// void appendContextMenu(Menu* menu) override {
		// SeqP16* module = dynamic_cast<SeqP16*>(this->module);
		// assert(module);
		// menu->addChild(new MenuSeparator);
		// menu->addChild(createBoolPtrMenuItem("Allow empty channels", "", &module->allowEmpty));
	// }

};


Model* modelSeqP16 = createModel<SeqP16, SeqP16Widget>("SeqP16");