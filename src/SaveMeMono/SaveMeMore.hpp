// Copyright (c) 2022 András Szabó
// looks OK, can go to Yolow

// included in SaveMeMono.cpp
// #include "plugin.hpp"

struct SaveMeMore : Module {
	
	enum ParamId    {START_PARAM, LENGT_PARAM, PREC_PARAM, PW_PARAM, SYNC_PARAM, DELAY_PARAM, PARAMS_LEN};
	enum InputId    {START_INPUT, INPUTS_LEN};
	enum OutputId	{MONO_REPLAY_OUTPUT, REVERSE_REPLAY_OUTPUT, RANDOM_REPLAY_OUTPUT, OUTPUTS_LEN};
	enum LightId	{ENUMS(LED_SEGMENT,16), PW_LIGHT, SYNC_LIGHT, LIGHTS_LEN};

	SaveMeMore() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(PREC_PARAM, 	0.5f, 1.0f, 1.0f, "Readhead precision");
		configParam(LENGT_PARAM, 	1.0f, 256.0f, 16.0f, "Length of pattern");
		configParam(START_PARAM, 	1.0f, 256.0f, 1.0f, "Section start");
		configParam(PW_PARAM, 		0.0f, 1.0f, 1.0f, "Clock pulse width or full");
		configParam(SYNC_PARAM, 	0.0f, 1.0f, 0.0f, "Sync to mother");
		configParam(DELAY_PARAM, 	0.0f, 16.0f, 0.0f, "Delay the beat");
		paramQuantities[DELAY_PARAM]->snapEnabled = true;
		paramQuantities[SYNC_PARAM]->snapEnabled = true;
		paramQuantities[PW_PARAM]->snapEnabled = true;
		paramQuantities[START_PARAM]->snapEnabled = true;
		paramQuantities[LENGT_PARAM]->snapEnabled = true;
		paramQuantities[START_PARAM]->description = "Can also be modulated by 0-10V CV";
		configInput(START_INPUT, "Start position CV"); 
		configOutput(MONO_REPLAY_OUTPUT, "Mono replay"); 
		configOutput(REVERSE_REPLAY_OUTPUT, "Reverse replay"); 
		configOutput(RANDOM_REPLAY_OUTPUT, "Random replay"); 
	}

 	SaveMeMono* findHostModulePtr(Module* module) {
		if (module) {
			if (module->leftExpander.module) {
				// if it's the mother module, we're done
				if (module->leftExpander.module->model == modelSaveMeMono) {
					return reinterpret_cast<SaveMeMono*>(module->leftExpander.module);
				}
				// if it's mother module - NOT WORKING
				// else if (module->leftExpander.module->model == modelSaveMeMonoWide) {
					// return reinterpret_cast<SaveMeMonoWide*>(module->leftExpander.module);
				// }				
				// if it's a copy of this expander, keep recursing
				else if (module->leftExpander.module->model == modelSaveMeMore) {
					return findHostModulePtr(module->leftExpander.module);
				}				
			}
		}
		return nullptr;
	}

	// Ctrl-E bypassing: no sound but the counting stays
	void processBypass(const ProcessArgs& args) override {
		outputs[MONO_REPLAY_OUTPUT].setVoltage(1);
		outputs[REVERSE_REPLAY_OUTPUT].setVoltage(2);
		outputs[RANDOM_REPLAY_OUTPUT].setVoltage(3);
	}

	float theSeq[256]={0};	// this contains the sequence, 256 slots available
	int theStep=0;
	int rndStep=0;
	int START_VAL=1;
	int LENGT_VAL=16;
	int DELAY_VAL=0;
	float PREC_VAL=1;
	int loop=1;
	
	void process(const ProcessArgs& args) override {

		if (loop++>8769) {
			loop=1;
			lights[PW_LIGHT].setBrightness(params[PW_PARAM].getValue());
			lights[SYNC_LIGHT].setBrightness(params[SYNC_PARAM].getValue());
			// we may need info if the end position is bigger than the start
			rndStep=(rand() % LENGT_VAL) + START_VAL;
			if (theStep<START_VAL) {theStep=START_VAL;}
		}
		
		SaveMeMono const* mother = findHostModulePtr(this);
		if (mother) {
		
			if (mother->childClockWarning==true) {
				// let's remember the knob settings
				PREC_VAL=params[PREC_PARAM].getValue();
				LENGT_VAL=params[LENGT_PARAM].getValue();
				START_VAL=params[START_PARAM].getValue()-1;
				DELAY_VAL=params[DELAY_PARAM].getValue();
				
// itt
// if reset signal: this SaveMeMore is delayed by 1 step
// ?!?!?!?!?!?

				// let's calculate the current step
				if (mother->childResetWarning==true) {
					lights[SYNC_LIGHT].setBrightness(1);
					params[SYNC_PARAM].setValue(1);
					theStep=START_VAL+LENGT_VAL-1; // oh, no! mother got a RESET signal!
				}
				else if (mother->currPos==0 && params[SYNC_PARAM].getValue()==1) {
					// let's restart if mother gets to her first slot 
					theStep=START_VAL+LENGT_VAL-1;
					lights[SYNC_LIGHT].setBrightness(0);
					params[SYNC_PARAM].setValue(0);
				}
				else {
					// cool, let's take the next step
					theStep++;
					if (theStep>=START_VAL+LENGT_VAL || theStep>=256) {
						if (inputs[START_INPUT].isConnected()) {
							float xStart=inputs[START_INPUT].getVoltage();
							if (xStart>=0) {params[START_PARAM].setValue(1+(int)(xStart/10*256));}	
						}
						theStep=START_VAL;
					}
				}
				
				// let's collect the value for MONO_REPLAY_OUTPUT
				int truStep=theStep-DELAY_VAL;
				if (truStep<START_VAL) {truStep+=LENGT_VAL;}
				if (PREC_VAL==1 || PREC_VAL>rack::random::uniform()) {
					outputs[MONO_REPLAY_OUTPUT].setVoltage(mother->theSeq[truStep]);
				} else {
					outputs[MONO_REPLAY_OUTPUT].setVoltage(mother->theSeq[rndStep]);
				}
				
				// let's collect the value for REVERSE_REPLAY_OUTPUT				
				truStep=(LENGT_VAL+START_VAL)-(theStep-START_VAL)-1;
				if (truStep<START_VAL) {truStep+=LENGT_VAL;}
				if (PREC_VAL==1 || PREC_VAL>rack::random::uniform()) {
					outputs[REVERSE_REPLAY_OUTPUT].setVoltage(mother->theSeq[truStep]);
				} else {
					outputs[REVERSE_REPLAY_OUTPUT].setVoltage(mother->theSeq[rndStep]);
				}
				
				// let's collect the value for RANDOM_REPLAY_OUTPUT				
				// outputs[RANDOM_REPLAY_OUTPUT].setVoltage(mother->theSeq[rndStep]);
			}
			else if (mother->childSleepWarning==true && params[PW_PARAM].getValue()==0) {
				outputs[MONO_REPLAY_OUTPUT].setVoltage(0);
				outputs[REVERSE_REPLAY_OUTPUT].setVoltage(0);
				// outputs[RANDOM_REPLAY_OUTPUT].setVoltage(0);
			}
		}
		else {
			outputs[MONO_REPLAY_OUTPUT].setVoltage(-4.04);
			outputs[REVERSE_REPLAY_OUTPUT].setVoltage(-4.04);
			// outputs[RANDOM_REPLAY_OUTPUT].setVoltage(-4.04);
		}
		
	}
};

struct SaveMeMoreWidget : ModuleWidget {

	SaveMeMore* module;
	#include "../share/widgetwizard.hpp"   // addChild abbreviations

	SaveMeMoreWidget(SaveMeMore* module) {

		this->module = module;
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/SaveMeMore.svg")));

		// addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		childKnob(SaveMeMore::LENGT_PARAM, 1, HP*1.5, HP*2);
		childKnob(SaveMeMore::START_PARAM, 1, HP*1.5, HP*5);
		childInput(SaveMeMore::START_INPUT, HP*1.5, HP*8);
		childKnob(SaveMeMore::DELAY_PARAM, 0, HP*1.5, HP*10.5);

		addParam(createLightParamCentered<VCVLightLatch<MediumSimpleLight<WhiteLight>>>(mm2px(Vec(HP*1.5, HP*13)), module, SaveMeMore::SYNC_PARAM, SaveMeMore::SYNC_LIGHT));

		addParam(createLightParamCentered<VCVLightLatch<MediumSimpleLight<WhiteLight>>>(mm2px(Vec(HP*1.5, HP*15.5)), module, SaveMeMore::PW_PARAM, SaveMeMore::PW_LIGHT));

		childKnob(SaveMeMore::PREC_PARAM, 0, HP*1.5, HP*18);
				
		childOutput(SaveMeMono::MONO_REPLAY_OUTPUT, HP*1.5, HP*21);
		childOutput(SaveMeMono::REVERSE_REPLAY_OUTPUT, HP*1.5, HP*23);
		// childOutput(SaveMeMono::RANDOM_REPLAY_OUTPUT, HP*1.5, HP*23);

		// childLabel(HP*1, HP*3*10, "DEV", 12);
	}
};


Model* modelSaveMeMore = createModel<SaveMeMore, SaveMeMoreWidget>("SaveMeMore");
