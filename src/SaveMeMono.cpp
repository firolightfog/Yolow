// Copyright (c) 2022 András Szabó
// looks OK, can go to Yolow
// to be tested: record another sequencer (CV or gate)
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

	enum LightId    {LED_RECORD_PARAM, LIGHTS_LEN};

	// small assistance to save older values for reference;
	// float paramVal[PARAMS_LEN]={0};
	// float inputVolt[INPUTS_LEN]={0};
	// float lightVal[LIGHTS_LEN]={0};

// --------------------------------------------------

	SaveMeMono() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(STEPS_PARAM, 	1.0f, 256.0f, 16.0f, "Steps");
		configParam(RECORD_PARAM, 	0.0f, 1.0f, 0.0f, "Start/stop recording");
		paramQuantities[STEPS_PARAM]->snapEnabled = true;
		paramQuantities[RECORD_PARAM]->snapEnabled = true;
		paramQuantities[RECORD_PARAM]->randomizeEnabled = false;

		configInput(MONO_LFO_INPUT, "Mono LFO"); 
		configInput(RECORD_INPUT, "Start/stop recording"); 
		configInput(CLOCK_INPUT, "Mono load trigger"); 
		configInput(RESET_INPUT, "Mono load reset"); 

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
		indexShift=-1;
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
		newReset=inputs[RESET_INPUT].getVoltage();
		if (newReset>2.0f && oldReset<=2.0f) {currPos=0;}
		oldReset=newReset;

		// let's see the clock signal
		newClock=inputs[CLOCK_INPUT].getVoltage();
		if (newClock>2.0f && oldClock<=2.0f) {
			if (hitRecord==1) {
				
				// this section grabs the internal/external LFO signal and saves it
				if (lfoIn) {pickVolt=inputs[MONO_LFO_INPUT].getVoltage();}
				else if (indexLFO==1) {pickVolt=(rack::random::uniform()*10)-5;}	// internal LFO for '-5V to 5V'
				else if (indexLFO==2) {pickVolt=(rack::random::uniform()*10);}		// internal LFO for '0V to 10V'
				else if (indexLFO==0) {pickVolt=(rand() % 2) *10;}					// internal LFO for gate sequences
				theSeq[currPos]=pickVolt;	// write it to the sequence step/slot
		
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
		else if (indexPW==1 && newClock<=2.0f && oldClock>2.0f) {
			outputs[MONO_REPLAY_OUTPUT].setVoltage(0);
			outputs[REVERSE_REPLAY_OUTPUT].setVoltage(0);
			outputs[RANDOM_REPLAY_OUTPUT].setVoltage(0);			
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
		
		childKnob(SaveMeMono::STEPS_PARAM, 0, HP*1, HP*2);
		// childPushbutton(SaveMeMono::RECORD_PARAM, SaveMeMono::LED_RECORD_PARAM, HP*1, HP*8);	// this is yellow
		addParam(createLightParamCentered<VCVLightLatch<MediumSimpleLight<RedLight>>>(mm2px(Vec(HP*1, HP*8)), module, SaveMeMono::RECORD_PARAM, SaveMeMono::LED_RECORD_PARAM));	// this is red
		
		childInput(SaveMeMono::MONO_LFO_INPUT, HP*1, HP*5.5);
		childInput(SaveMeMono::RECORD_INPUT, HP*1, HP*9.5);
		childInput(SaveMeMono::CLOCK_INPUT, HP*1, HP*13);
		childInput(SaveMeMono::RESET_INPUT, HP*1, HP*15.5);
		childOutput(SaveMeMono::MONO_REPLAY_OUTPUT, HP*1, HP*18.5);
		childOutput(SaveMeMono::REVERSE_REPLAY_OUTPUT, HP*1, HP*20.5);
		childOutput(SaveMeMono::RANDOM_REPLAY_OUTPUT, HP*1, HP*22.5);
		
		// childLabel(HP*1, HP*12, "TEST", 8);
	}
	
	void appendContextMenu(Menu* menu) override {
		SaveMeMono* module = dynamic_cast<SaveMeMono*>(this->module);
		assert(module);
		menu->addChild(new MenuSeparator);
		menu->addChild(createIndexPtrSubmenuItem("Pulse width", {"Full width","Only clock wide"}, &module->indexPW));
		menu->addChild(createIndexPtrSubmenuItem("Internal LFO", {"Gates","-5V to 5V","0V to 10V"}, &module->indexLFO));
		menu->addChild(createIndexPtrSubmenuItem("Record mode", {"One cycle","Momentary","Always"}, &module->indexRec));
		menu->addChild(createIndexPtrSubmenuItem("Shift sequence", {"Earlier","Later"}, &module->indexShift));	// ugly stuff
	}

};

Model* modelSaveMeMono = createModel<SaveMeMono, SaveMeMonoWidget>("SaveMeMono");
