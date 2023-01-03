// Copyright (c) 2022 András Szabó
// too many menu items -> panel to be redesigned

#include "plugin.hpp"

struct SaveMeMono : Module {

// --------------------------------------------------

	enum ParamId    {
		STEPS_PARAM, RECORD_PARAM, PARAMS_LEN};

	enum InputId    {
		MONO_LFO_INPUT, RECORD_INPUT, CLOCK_INPUT, RESET_INPUT, INPUTS_LEN};

	enum OutputId    {
		MONO_REPLAY_OUTPUT, REVERSE_REPLAY_OUTPUT, RANDOM_REPLAY_OUTPUT, OUTPUTS_LEN};

	enum LightId    {LED_RECORD_PARAM, FIRST_HIT_LIGHT, LIGHTS_LEN};

	// small assistance to save older values for reference;
	// float paramVal[PARAMS_LEN]={0};
	// float inputVolt[INPUTS_LEN]={0};
	// float lightVal[LIGHTS_LEN]={0};

// --------------------------------------------------

	SaveMeMono() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(STEPS_PARAM, 	1.0f, 256.0f, 16.0f, "Number of steps");
		configParam(RECORD_PARAM, 	0.0f, 1.0f, 0.0f, "Start/stop recording");
		paramQuantities[STEPS_PARAM]->snapEnabled = true;
		paramQuantities[RECORD_PARAM]->snapEnabled = true;
		paramQuantities[RECORD_PARAM]->randomizeEnabled = false;

		configInput(MONO_LFO_INPUT, "Mono LFO"); 
		configInput(RECORD_INPUT, "Start/stop recording"); 
		configInput(CLOCK_INPUT, "Clock"); 
		configInput(RESET_INPUT, "Reset"); 

		configOutput(MONO_REPLAY_OUTPUT, "Mono replay"); 
		configOutput(REVERSE_REPLAY_OUTPUT, "Reverse replay"); 
		configOutput(RANDOM_REPLAY_OUTPUT, "Random replay"); 
	}

// --------------------------------------------------

	int loop=0;     // save some CPU in process()

	// for managing the RESET and CLOCK signals
	float newRec=0.0f;
	float oldRec=0.0f;
	float newReset=0.0f;
	float oldReset=0.0f;
	float newClock=0.0f;
	float oldClock=0.0f;
	int currPos=0;
	
	// more variables
	float theSeq[256]={0};	// this contains the sequence, 256 slots available
	// int theShuffle[256]={0};
	int allSteps=16;		// set the number of active steps
	int hitRecord=0;		// on the air?

	// even more variables
	float pickVolt=0.0f;	// the currently sampled input
	bool clockIn=false;		// is the clock input connected?
	bool lfoIn=false;		// is the LFO input connected?
	int indexPW=0;			// pulse width can be 0 (full) or 1 (clock wide)
	int indexLFO=1;			// if LFO is unplugged the internal voltage can be set
	int indexRec=0;			// record mode (toggle / momentary) can be set
	int indexShift=-1;		// sooner or later shift the sequence
	int countRec=0;
	bool childClockWarning=false;
	bool childSleepWarning=false;
	bool childResetWarning=false;

	struct childWarning {
		bool signalClock=false;
		bool signalReset=false;
	};

	/*
	// Voxglitch Zero Point text file export - doesn't work
	void exportFields() {
		std::string buffer="";
		for (int i=0; i<256; i++) {buffer=buffer + std::to_string(theSeq[i]) +",";}
		std::ofstream newFile("SaveMe_CVsequence.txt");
		newFile << buffer;
		newFile.close();	 
	}
	*/
	
	// OK, I know ... this is ugly but I had to find a way to shift the (unvisible) sequence 
	void shiftSeq() {
		float tempStep=0.0f;
		if (indexShift==0) {
			tempStep=theSeq[0];
			for (int s=0;s<allSteps-1;s++) {theSeq[s]=theSeq[s+1];}
			theSeq[allSteps-1]=tempStep;
		}
		else if (indexShift==1) {
			tempStep=theSeq[allSteps-1];
			for (int s=allSteps-1;s>0;s--) {theSeq[s]=theSeq[s-1];}
			theSeq[0]=tempStep;
		}
		// theShuffle[allSteps]=std::random_shuffle(&theShuffle[0], &theShuffle[allSteps]);
		indexShift=-1;
	}
	
	void randomizeFields() {
		for (int i=0; i<256; i++) {
			if (indexLFO==1) {theSeq[i]=(rack::random::uniform()*10)-5;}	// internal LFO for '-5V to 5V'
			else if (indexLFO==2) {theSeq[i]=(rack::random::uniform()*10);}		// internal LFO for '0V to 10V'
			else if (indexLFO==3) {theSeq[i]=(rack::random::uniform()*2-1);}		// internal LFO for '-1V to 1V'
			else if (indexLFO==4) {theSeq[i]=(rack::random::uniform());}		// internal LFO for '0V to 1V'
			else if (indexLFO==0) {theSeq[i]=(rand() % 2) *10;}					// internal LFO for gate sequences
		}
	}
	
	// Ctrl-E bypassing: no sound but the counting stays
	void processBypass(const ProcessArgs& args) override {
	}

	void process(const ProcessArgs& args) override {

		if (loop--<=0) {
			loop=9000;
			// save some more CPU
			allSteps=params[STEPS_PARAM].getValue();
			hitRecord=params[RECORD_PARAM].getValue();
			lights[LED_RECORD_PARAM].setBrightness(hitRecord);
			clockIn=inputs[CLOCK_INPUT].isConnected();
			lfoIn=inputs[MONO_LFO_INPUT].isConnected();
			// paramQuantities[RECORD_PARAM]->description = ("Recording mode: " + indexRec);
			if (indexShift!=-1) {shiftSeq();}	// ugly stuff
			lights[FIRST_HIT_LIGHT].setBrightness(theSeq[0]);

			// this section is only about updating the descriptions
			std::string sx="";
			if (indexRec==0) {sx+="Single cycle";}
			else if (indexRec==1) {sx+="Single step";}
			else if (indexRec==2) {sx+="Non-stop";}
			if (indexLFO==0) {sx+=(lfoIn)?" from LFO":" & Gates";}
			else if (indexLFO==1) {sx+=(lfoIn)?" from LFO":" & -5V to +5V";}
			else if (indexLFO==2) {sx+=(lfoIn)?" from LFO":" & 0V to 10V";}
			paramQuantities[RECORD_PARAM]->description = sx;

		}

		// set recording start if needed
		newRec=inputs[RECORD_INPUT].getVoltage();
		if (newRec>2.0f && oldRec<=2.0f) {
			hitRecord=(hitRecord==0)?1:0;
			params[RECORD_PARAM].setValue(hitRecord);
			lights[LED_RECORD_PARAM].setBrightness(hitRecord);
		}
		oldRec=newRec;

		// let's see the reset signal
		childResetWarning=false;
		newReset=inputs[RESET_INPUT].getVoltage();
		if (newReset>2.0f && oldReset<=2.0f) {currPos=0; childResetWarning=true;}
		oldReset=newReset;

		// let's see the clock signal
		childClockWarning=false;
		childSleepWarning=false;
		newClock=inputs[CLOCK_INPUT].getVoltage();
		if (newClock>2.0f && oldClock<=2.0f) {
			childClockWarning=true;
			if (hitRecord==1) {
				
				// this section grabs the internal/external LFO signal and saves it
				if (lfoIn) {pickVolt=inputs[MONO_LFO_INPUT].getVoltage();}
				else if (indexLFO==1) {pickVolt=(rack::random::uniform()*10)-5;}	// internal LFO for '-5V to 5V'
				else if (indexLFO==2) {pickVolt=(rack::random::uniform()*10);}		// internal LFO for '0V to 10V'
				else if (indexLFO==3) {pickVolt=(rack::random::uniform()*2-1);}		// internal LFO for '-1V to 1V'
				else if (indexLFO==4) {pickVolt=(rack::random::uniform());}		// internal LFO for '0V to 1V'
				else if (indexLFO==0) {pickVolt=(rand() % 2) *10;}					// internal LFO for gate sequences
				theSeq[currPos]=pickVolt;	// write it to the sequence step/slot
				// theShuffle[currPos]=currPos;
		
				// this sction allows switching off recording (see toggle/momentary)
				if (indexRec==1) {params[RECORD_PARAM].setValue(0);}	// momentary
				else if (indexRec==0) {									// single cycle
					if (countRec<=0) {params[RECORD_PARAM].setValue(0);}
					else {countRec--;}
					}													
				else if (indexRec==2) {/* keep on doing */}				// always
				}
			else {countRec=allSteps;} 	// (hitRecord==1)
			
			outputs[MONO_REPLAY_OUTPUT].setVoltage(theSeq[currPos]);
			outputs[REVERSE_REPLAY_OUTPUT].setVoltage(theSeq[allSteps-currPos-1]);
			outputs[RANDOM_REPLAY_OUTPUT].setVoltage(theSeq[rand() % allSteps]);
			currPos++;	if (currPos>=allSteps) {currPos=0;}
		}
		else if (newClock>2.0f && oldClock>2.0f) {
			if (lfoIn && hitRecord==1 && indexLFO==0) {
				pickVolt=inputs[MONO_LFO_INPUT].getVoltage();
				if (theSeq[currPos]<pickVolt) {theSeq[currPos]=pickVolt;}
			}
		}
		else if (newClock<=2.0f && oldClock>2.0f) {
			childSleepWarning=true;
			if (indexPW==1) {
				outputs[MONO_REPLAY_OUTPUT].setVoltage(0);
				outputs[REVERSE_REPLAY_OUTPUT].setVoltage(0);
				outputs[RANDOM_REPLAY_OUTPUT].setVoltage(0);			
			}
		}
		oldClock=newClock;

	}

	// --------------------------------------------------

	// this block is to save and reload a variable
	json_t* dataToJson() override {
	json_t* rootJ = json_object();
	json_object_set_new(rootJ, "pw", json_integer(indexPW));
	json_object_set_new(rootJ, "lfo", json_integer(indexLFO));
	json_object_set_new(rootJ, "rec", json_integer(indexRec));
	// json_object_set_new(rootJ, "notes", json_float(theSeq));
	json_t* notes_json_array = json_array();
	for (int notes_index=0; notes_index < 256; notes_index++) {
		json_array_append_new(notes_json_array , json_real(theSeq[notes_index]));
	}	
	json_object_set(rootJ, "notes", notes_json_array);
	return rootJ;}

	void dataFromJson(json_t* rootJ) override {
	json_t *pwJ = json_object_get(rootJ, "pw");
	if (pwJ) indexPW = json_integer_value(pwJ);
	json_t *lfoJ = json_object_get(rootJ, "lfo");
	if (lfoJ) indexLFO = json_integer_value(lfoJ);
	json_t *recJ = json_object_get(rootJ, "rec");
	if (recJ) indexRec = json_integer_value(recJ);
	// json_t *seqJ = json_object_get(rootJ, "notes");
	// if (seqJ) theSeq = json_float_value(seqJ);
	json_t *notes_json_array = json_object_get(rootJ, "notes");
	size_t note_index;
	json_t *json_value;
	if (notes_json_array) {
		json_array_foreach(notes_json_array, note_index, json_value)
		{theSeq[note_index] = json_real_value(json_value);}
	}
	}

};

// --------------------------------------------------

struct SaveMeMonoWidget : ModuleWidget {

	SaveMeMono* module;
	#include "share/widgetwizard.hpp"   // addChild abbreviations

	SaveMeMonoWidget(SaveMeMono* module) {

		this->module = module;
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/SaveMeMono.svg")));

		// addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		
		childKnob(SaveMeMono::STEPS_PARAM, 1, HP*1.5, HP*2);
		// childPushbutton(SaveMeMono::RECORD_PARAM, SaveMeMono::LED_RECORD_PARAM, HP*1, HP*8);	// this is yellow
		addParam(createLightParamCentered<VCVLightLatch<MediumSimpleLight<RedLight>>>(mm2px(Vec(HP*1.5, HP*8)), module, SaveMeMono::RECORD_PARAM, SaveMeMono::LED_RECORD_PARAM));	// this is red
		
		childInput(SaveMeMono::MONO_LFO_INPUT, HP*1.5, HP*5.5);
		childInput(SaveMeMono::RECORD_INPUT, HP*1.5, HP*9.5);
		childLight(SaveMeMono::FIRST_HIT_LIGHT, 14, HP*2.5, HP*8.75);
		childInput(SaveMeMono::CLOCK_INPUT, HP*1.5, HP*13);
		childInput(SaveMeMono::RESET_INPUT, HP*1.5, HP*15.5);		
		childOutput(SaveMeMono::MONO_REPLAY_OUTPUT, HP*1.5, HP*19);
		childOutput(SaveMeMono::REVERSE_REPLAY_OUTPUT, HP*1.5, HP*21);
		childOutput(SaveMeMono::RANDOM_REPLAY_OUTPUT, HP*1.5, HP*23);
		
		// childLabel(HP*1, HP*12, "TEST", 8);
	}
	
	void appendContextMenu(Menu* menu) override {
		SaveMeMono* module = dynamic_cast<SaveMeMono*>(this->module);
		assert(module);
		menu->addChild(new MenuSeparator);
		menu->addChild(createMenuItem("Randomize all CVs", "", [=]() {module->randomizeFields();}));	
		menu->addChild(new MenuSeparator);
		menu->addChild(createIndexPtrSubmenuItem("Pulse width", {"Full width","Only clock wide"}, &module->indexPW));
		menu->addChild(createIndexPtrSubmenuItem("Internal LFO", {"Gates","-5V to 5V","0V to 10V","-1V to 1V","0V to 1V"}, &module->indexLFO));
		menu->addChild(createIndexPtrSubmenuItem("Record mode", {"One cycle","Momentary","Always"}, &module->indexRec));
		menu->addChild(createIndexPtrSubmenuItem("Shift sequence", {"Earlier","Later"}, &module->indexShift));	// ugly stuff
	}

};

Model* modelSaveMeMono = createModel<SaveMeMono, SaveMeMonoWidget>("SaveMeMono");


/**************************************************/
// EXPANDER CODE - starting here
/**************************************************/

struct SaveMeMore : Module {
	
	enum ParamId    {START_PARAM, LENGT_PARAM, PREC_PARAM, PW_PARAM, SYNC_PARAM, DELAY_PARAM, PARAMS_LEN};
	enum InputId    {INPUTS_LEN};
	enum OutputId	{MONO_REPLAY_OUTPUT, REVERSE_REPLAY_OUTPUT, RANDOM_REPLAY_OUTPUT, OUTPUTS_LEN};
	enum LightId	{ENUMS(LED_SEGMENT,16), PW_LIGHT, SYNC_LIGHT, LIGHTS_LEN};

	SaveMeMore() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(PREC_PARAM, 	0.5f, 1.0f, 1.0f, "Readhead precision");
		configParam(LENGT_PARAM, 	1.0f, 256.0f, 16.0f, "Length of pattern");
		configParam(START_PARAM, 	1.0f, 256.0f, 1.0f, "Section start");
		configParam(PW_PARAM, 	0.0f, 1.0f, 0.0f, "Clock pulse width or full");
		configParam(SYNC_PARAM, 	0.0f, 1.0f, 0.0f, "Sync to mother");
		configParam(DELAY_PARAM, 	0.0f, 16.0f, 0.0f, "Delay the beat");
		paramQuantities[DELAY_PARAM]->snapEnabled = true;
		paramQuantities[SYNC_PARAM]->snapEnabled = true;
		paramQuantities[PW_PARAM]->snapEnabled = true;
		paramQuantities[START_PARAM]->snapEnabled = true;
		paramQuantities[LENGT_PARAM]->snapEnabled = true;
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
				
				// let's calculate the current step
				if (mother->childResetWarning==true) {
					// oh, no! mother got a RESET signal!
					theStep=START_VAL;
					}
				else if (params[SYNC_PARAM].getValue()==1 && mother->currPos==0) {
					// let's restart if mother gets to her first slot 
					theStep=START_VAL;
					lights[SYNC_LIGHT].setBrightness(0);
					params[SYNC_PARAM].setValue(0);
				}
				else {
					// cool, let's take the next step
					theStep++;
					if (theStep>=START_VAL+LENGT_VAL || theStep>=256) {theStep=START_VAL;}
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
				truStep=theStep-DELAY_VAL;
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
	#include "share/widgetwizard.hpp"   // addChild abbreviations

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
		childKnob(SaveMeMore::DELAY_PARAM, 0, HP*1.5, HP*8);

		addParam(createLightParamCentered<VCVLightLatch<MediumSimpleLight<WhiteLight>>>(mm2px(Vec(HP*1.5, HP*11)), module, SaveMeMore::SYNC_PARAM, SaveMeMore::SYNC_LIGHT));

		addParam(createLightParamCentered<VCVLightLatch<MediumSimpleLight<WhiteLight>>>(mm2px(Vec(HP*1.5, HP*15.5)), module, SaveMeMore::PW_PARAM, SaveMeMore::PW_LIGHT));

		childKnob(SaveMeMore::PREC_PARAM, 0, HP*1.5, HP*18);
				
		childOutput(SaveMeMono::MONO_REPLAY_OUTPUT, HP*1.5, HP*21);
		childOutput(SaveMeMono::REVERSE_REPLAY_OUTPUT, HP*1.5, HP*23);
		// childOutput(SaveMeMono::RANDOM_REPLAY_OUTPUT, HP*1.5, HP*23);

		// childLabel(HP*1, HP*3*10, "DEV", 12);
	}
};


Model* modelSaveMeMore = createModel<SaveMeMore, SaveMeMoreWidget>("SaveMeMore");
