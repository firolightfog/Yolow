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
		REPEAT_CH1_PARAM,
		REPEAT_CH2_PARAM,
		REPEAT_CH3_PARAM,
		REPEAT_CH4_PARAM,
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
		configParam(SELECT_CH1_PARAM, 0.f, 16.f, 1.f, "Select a channel for step 1");
		configParam(SELECT_CH2_PARAM, 0.f, 16.f, 0.f, "Select a channel for step 2");
		configParam(SELECT_CH3_PARAM, 0.f, 16.f, 0.f, "Select a channel for step 3");
		configParam(SELECT_CH4_PARAM, 0.f, 16.f, 0.f, "Select a channel for step 4");
		paramQuantities[SELECT_CH1_PARAM]->snapEnabled = true;
		paramQuantities[SELECT_CH2_PARAM]->snapEnabled = true;
		paramQuantities[SELECT_CH3_PARAM]->snapEnabled = true;
		paramQuantities[SELECT_CH4_PARAM]->snapEnabled = true;
		configParam(REPEAT_CH1_PARAM, 1.f, 16.f, 1.f, "Repeat 1");
		configParam(REPEAT_CH2_PARAM, 1.f, 16.f, 1.f, "Repeat 2");
		configParam(REPEAT_CH3_PARAM, 1.f, 16.f, 1.f, "Repeat 3");
		configParam(REPEAT_CH4_PARAM, 1.f, 16.f, 1.f, "Repeat 4");
		paramQuantities[REPEAT_CH1_PARAM]->snapEnabled = true;
		paramQuantities[REPEAT_CH2_PARAM]->snapEnabled = true;
		paramQuantities[REPEAT_CH3_PARAM]->snapEnabled = true;
		paramQuantities[REPEAT_CH4_PARAM]->snapEnabled = true;
		configInput(RST_INPUT, "Reset");
		configInput(CLK_INPUT, "Clock");
		configInput(POLYIN_INPUT, "Polyphonic");
		configOutput(MONOOUT_OUTPUT, "Monophonic");
	}

	int stepPos=1;  	// to define the active step (1, 2, 3, 4)
	bool clkOld=false;	// to catch the clock pulse
	bool clkNew=false;	// to catch the clock pulse
	bool rstOld=false;	// to catch the reset pulse
	bool rstNew=false;	// to catch the reset pulse

	int countRepeat=0;	// this on will count the number of repeats
	int nrChan=1;   	// to record number of POLYIN_INPUT channels
	int rdChan=1;   	// to record number of POLYIN_INPUT channels
	bool allowEmpty;	// to handle the case of knob requesting a greater channel than available
	bool clockIn=false;


	float paramVal[PARAMS_LEN];
	int loop=0;		// for skipping rounds in process()

	void process(const ProcessArgs& args) override {

		if (loop-- ==0) {
			loop=7525;
			for (int p=0;p<PARAMS_LEN;p++) {paramVal[p]=params[p].getValue();}
			nrChan=(inputs[POLYIN_INPUT].isConnected())?inputs[POLYIN_INPUT].channels:1;
			clockIn=inputs[CLK_INPUT].isConnected();
			// place to do somethings
		}
		
		// is there a reset? jump to first step!
		rstNew=inputs[RST_INPUT].getVoltage()>2;
		if ((rstNew==true) && (rstOld==false)) {
			stepPos=1;
			lights[BLINK1_LIGHT].setBrightness(1);
			lights[BLINK2_LIGHT].setBrightness(0);
			lights[BLINK3_LIGHT].setBrightness(0);
			lights[BLINK4_LIGHT].setBrightness(0);						
		}

		// is there a clock? jump to the next step!
		if (clockIn) {	// there is a clock available
			clkNew=inputs[CLK_INPUT].getVoltage()>2;
			if ((clkNew==true) && (clkOld==false)) {
				// and monitor the number of channels
				rdChan=(rand() % nrChan);
				countRepeat++;
				// stepPos++; if (stepPos>4) {stepPos=1;}
				// if (paramVal[REPEAT_CH1_PARAM+stepPos-1]<=countRepeat) {
				if (params[REPEAT_CH1_PARAM+stepPos-1].getValue()>countRepeat) {
					// stepPos shall stay the same
				}
				else {
					// stepPos sould be increased
					stepPos++;
					if (stepPos>4) {stepPos=1;}
					countRepeat=0;
				}
				lights[BLINK1_LIGHT].setBrightness(0);
				lights[BLINK2_LIGHT].setBrightness(0);
				lights[BLINK3_LIGHT].setBrightness(0);
				lights[BLINK4_LIGHT].setBrightness(0);						
				lights[BLINK1_LIGHT+stepPos-1].setBrightness(0.75);
			}
		}
		else /* if (!clockIn) */ {	// if no clock, stand still
			stepPos=1;
			rdChan=0;
			lights[BLINK1_LIGHT].setBrightness(0.25);
			lights[BLINK2_LIGHT].setBrightness(0);
			lights[BLINK3_LIGHT].setBrightness(0);
			lights[BLINK4_LIGHT].setBrightness(0);									
		}

		// let's remember the clock and the reset value
		if (rstOld!=rstNew) {rstOld=rstNew;}
		if (clkOld!=clkNew) {clkOld=clkNew;}

		// tell me which one needs to be sent
		int requestedOut=paramVal[SELECT_CH1_PARAM+stepPos-1]-1;	// sourece channels (1, 2, 3, ... 16) or 0 for random
		if (requestedOut>=nrChan || requestedOut<0) {requestedOut = rdChan;}
		
		// send the selected input to the output!
		outputs[MONOOUT_OUTPUT].setVoltage(inputs[POLYIN_INPUT].getVoltage(requestedOut));

	}
};

struct SeqP16Widget : ModuleWidget {
	
	float HP=5.08f;
	
	SeqP16Widget(SeqP16* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/SeqP16.svg")));
	
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(1*HP, 5*HP)), module, SeqP16::RST_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(3*HP, 5*HP)), module, SeqP16::CLK_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(2*HP, 23*HP)), module, SeqP16::POLYIN_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(2*HP, 20*HP)), module, SeqP16::MONOOUT_OUTPUT));

		// repeater
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(3*HP, 8*HP)), module, SeqP16::REPEAT_CH1_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(3*HP, 11*HP)), module, SeqP16::REPEAT_CH2_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(3*HP, 14*HP)), module, SeqP16::REPEAT_CH3_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(3*HP, 17*HP)), module, SeqP16::REPEAT_CH4_PARAM));

		// selection knobs
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(1.5*HP, 7*HP)), module, SeqP16::SELECT_CH1_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(1.5*HP, 10*HP)), module, SeqP16::SELECT_CH2_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(1.5*HP, 13*HP)), module, SeqP16::SELECT_CH3_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(1.5*HP, 16*HP)), module, SeqP16::SELECT_CH4_PARAM));

		// selection knob lights
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(1.5*HP, 7*HP)), module, SeqP16::BLINK1_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(1.5*HP, 10*HP)), module, SeqP16::BLINK2_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(1.5*HP, 13*HP)), module, SeqP16::BLINK3_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(1.5*HP, 16*HP)), module, SeqP16::BLINK4_LIGHT));

	}

};


Model* modelSeqP16 = createModel<SeqP16, SeqP16Widget>("SeqP16");