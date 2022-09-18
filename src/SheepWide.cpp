// Copyright (c) 2022 András Szabó
#include "plugin.hpp"

struct SheepWide : Module {

// --------------------------------------------------

	enum ParamId    {
		MUTATE_PARAM, RANDOM_PATTERN_PARAM, RED_PARAM, GREEN_PARAM, BLUE_PARAM, 
		GATE_THRESHOLD_PARAM, MIN_V_PARAM, MAX_V_PARAM, START_READING_PARAM, END_READING_PARAM, 
		READING_INCREMENTS_PARAM, CLOCK_DIVISION_PARAM, PARAMS_LEN};

	enum InputId    {
		MUTATE_INPUT, RANDOM_INPUT, CLOCK_INPUT, RESET_INPUT, INPUTS_LEN};

	enum OutputId    {
		GATE_PURE_OUTPUT, GATE_INVERSE_OUTPUT, GATE_MUTANT_OUTPUT, GATE_INVERSED_MUTANT_OUTPUT, CV_PURE_OUTPUT, 
		CV_INVERSE_OUTPUT, CV_MUTANT_OUTPUT, CV_INVERSED_MUTANT_OUTPUT, OUTPUTS_LEN};

	enum LightId    {
		ENUMS(STEPS_A_COLUMN_SEGMENT, 3*8),ENUMS(STEPS_B_COLUMN_SEGMENT, 3*8),ENUMS(STEPS_C_COLUMN_SEGMENT, 3*8),ENUMS(STEPS_D_COLUMN_SEGMENT, 3*8),LIGHTS_LEN};

	// small assistance to save older values for reference;
	float paramVal[PARAMS_LEN]={0};
	// float inputVolt[INPUTS_LEN]={0};
	// float lightVal[LIGHTS_LEN]={0};

// --------------------------------------------------

	SheepWide() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(MUTATE_PARAM, 	0.0f, 16.0f, 1.0f, "Mutate CV");
		configParam(RANDOM_PATTERN_PARAM, 	0.0f, 1.0f, 0.0f, "Random trigger");
		configParam(RED_PARAM, 		0.0f, 1.0f, 0.0f, "Red");
		configParam(GREEN_PARAM, 	0.0f, 1.0f, 1.0f, "Green");
		configParam(BLUE_PARAM, 	0.0f, 1.0f, 0.0f, "Blue");
		configParam(GATE_THRESHOLD_PARAM, 	0.0f, 1.0f, 0.0f, "Gate threshold");
		configParam(MIN_V_PARAM, 	-10.0f, 10.0f, 0.0f, "Min V");
		configParam(MAX_V_PARAM, 	-10.0f, 10.0f, 1.0f, "Max V");
		configParam(START_READING_PARAM, 	0.0f, 32.0f, 17.0f, "Start");
		configParam(END_READING_PARAM, 		0.0f, 32.0f, 32.0f, "End");
		configParam(READING_INCREMENTS_PARAM, 	1.0f, 16.0f, 1.0f, "Reading increments");
		configParam(CLOCK_DIVISION_PARAM, 		1.0f, 32.0f, 1.0f, "Clock division");
		paramQuantities[RANDOM_PATTERN_PARAM]->snapEnabled = true;
		paramQuantities[START_READING_PARAM]->snapEnabled = true;
		paramQuantities[END_READING_PARAM]->snapEnabled = true;
		paramQuantities[RANDOM_PATTERN_PARAM]->randomizeEnabled = false;
		paramQuantities[GATE_THRESHOLD_PARAM]->randomizeEnabled = false;
		paramQuantities[MIN_V_PARAM]->randomizeEnabled = false;
		paramQuantities[MAX_V_PARAM]->randomizeEnabled = false;
		paramQuantities[START_READING_PARAM]->randomizeEnabled = false;
		paramQuantities[END_READING_PARAM]->randomizeEnabled = false;
		paramQuantities[READING_INCREMENTS_PARAM]->snapEnabled = true;
		paramQuantities[READING_INCREMENTS_PARAM]->randomizeEnabled = false;
		paramQuantities[CLOCK_DIVISION_PARAM]->snapEnabled = true;
		paramQuantities[CLOCK_DIVISION_PARAM]->randomizeEnabled = false;

		configInput(MUTATE_INPUT, "Mutate"); 
		configInput(RANDOM_INPUT, "Random"); 
		configInput(CLOCK_INPUT, "Clock"); 
		configInput(RESET_INPUT, "Reset"); 

		configOutput(GATE_PURE_OUTPUT, "Gate pure"); 
		configOutput(GATE_INVERSE_OUTPUT, "Gate inverse"); 
		configOutput(GATE_MUTANT_OUTPUT, "Gate mutated"); 
		configOutput(GATE_INVERSED_MUTANT_OUTPUT, "Gate inversed mutated"); 
		configOutput(CV_PURE_OUTPUT, "CV pure"); 
		configOutput(CV_INVERSE_OUTPUT, "CV inverse"); 
		configOutput(CV_MUTANT_OUTPUT, "CV mutated"); 
		configOutput(CV_INVERSED_MUTANT_OUTPUT, "CV inversed mutated"); 
	}

// --------------------------------------------------

	int loop=0;     // save some CPU in process()

	// for managing the RESET and CLOCK signals
	float newReset=0.0f;
	float oldReset=0.0f;
	float newClock=0.0f;
	float oldClock=0.0f;
	bool hitClock=false;


	// more variables
	float newVolt;	// original idea
	float newVolts[16]={0.0f}; // ammended idea for polyphonic output
	int readingLength=0;
	bool readingForward=true;
	int countDiv=-1;
	
	// even more variables
	bool restartPlease=true;
	float theSeq[32]={0};	// this contains the sequence, 32 slots available
	int currStep=0;
	int indexLinkStrEnd=0;
	int indexRestart=0;
	int indexMutants=1;
	float fieldRed=0.0f;	// weight of Red in the field
	float fieldGreen=1.0f;	// weight of Green in the field
	float fieldBlue=0.0f;	// weight of Blue in the field
	float fieldColors[3]={0};
	int indexMaxChan=1;
	float knobMinV=0.0f;
	float knobMaxV=1.0f;
	float indexGateTrsh=0.0f;
	
// --------------------------------------------------
	
	// it creates a brand new map
	void diceTheSeq() {
		for (int i=0;i<32;i++) {theSeq[i]=rack::random::uniform();}
	}

	// repaint the map with another color
	void diceColors(int newColors) {
		if (newColors==1) {
			fieldRed	= rack::random::uniform();
			fieldGreen	= rack::random::uniform();
			fieldBlue	= rack::random::uniform();
			float sumColors=fieldRed+fieldGreen+fieldBlue;
			if (sumColors>1) {
				fieldRed/=sumColors;
				fieldGreen/=sumColors;
				fieldBlue/=sumColors;				
			}
		}
		else {
			fieldRed	= 0.0f;
			fieldGreen	= 1.0f;
			fieldBlue	= 0.0f;			
		}		
	}
	
	// it changes only a few of the values; see the MUTATE_PARAM knob
	void mutateTheSeq() {
		if (indexMutants==0) {	// only is permanent mutation is set in context menu
			for (int i=0;i<32;i++) {
				if (rack::random::uniform()<paramVal[MUTATE_PARAM]) {
					theSeq[i]=rack::random::uniform();
				}
			}
		}
	}

	// randomize one of the columns
	void keyKnob(int rndSection) {
		for (int i=(rndSection-0)*8;i<(rndSection+1)*8;i++) {theSeq[i]=rack::random::uniform();}	
	}
	
	// clear one of the columns
	void clearKnob(int rndSection) {
		for (int i=(rndSection-0)*8;i<(rndSection+1)*8;i++) {theSeq[i]=0;}
	}

	void drawMap() {
		// redraw the steps & blank the backlight
		fieldColors[0]=fieldRed;
		fieldColors[1]=fieldGreen;
		fieldColors[2]=fieldBlue;
		for (int i=0;i<8*3;i=i+3) {
			if (indexGateTrsh>=0.001) {
				for (int c=0;c<3;c++) {
				lights[STEPS_A_COLUMN_SEGMENT+i+c].setBrightness((theSeq[i/3+ 0]>indexGateTrsh)?fieldColors[c]:0);
				lights[STEPS_B_COLUMN_SEGMENT+i+c].setBrightness((theSeq[i/3+ 8]>indexGateTrsh)?fieldColors[c]:0);
				lights[STEPS_C_COLUMN_SEGMENT+i+c].setBrightness((theSeq[i/3+16]>indexGateTrsh)?fieldColors[c]:0);
				lights[STEPS_D_COLUMN_SEGMENT+i+c].setBrightness((theSeq[i/3+24]>indexGateTrsh)?fieldColors[c]:0);
				}
			}
			else {
				for (int c=0;c<3;c++) {
				lights[STEPS_A_COLUMN_SEGMENT+i+c].setBrightness(theSeq[i/3+ 0]*fieldColors[c]);
				lights[STEPS_B_COLUMN_SEGMENT+i+c].setBrightness(theSeq[i/3+ 8]*fieldColors[c]);
				lights[STEPS_C_COLUMN_SEGMENT+i+c].setBrightness(theSeq[i/3+16]*fieldColors[c]);
				lights[STEPS_D_COLUMN_SEGMENT+i+c].setBrightness(theSeq[i/3+24]*fieldColors[c]);
				}
			}
		}
	}

// --------------------------------------------------

	void process(const ProcessArgs& args) override {

		if (loop--<=0) {
			loop=8967;
			// save some CPU

			// if START_READING_PARAM and END_READING_PARAM knobs are linked 
			if (indexLinkStrEnd==1) {
				if (paramVal[START_READING_PARAM]!=params[START_READING_PARAM].getValue()) {
					params[END_READING_PARAM].setValue(paramVal[END_READING_PARAM]-paramVal[START_READING_PARAM]+params[START_READING_PARAM].getValue());
				}
				if (paramVal[END_READING_PARAM]!=params[END_READING_PARAM].getValue()) {
					params[START_READING_PARAM].setValue(paramVal[START_READING_PARAM]-paramVal[END_READING_PARAM]+params[END_READING_PARAM].getValue());				
				}
			}

			// collect all paramter values for future reference
			for (int p=0;p<PARAMS_LEN;p++) {paramVal[p]=params[p].getValue();}			
			// readingForward=paramVal[START_READING_PARAM]<paramVal[END_READING_PARAM];
			readingLength=paramVal[END_READING_PARAM]-paramVal[START_READING_PARAM]+1;
			readingForward=(readingLength>=1);
			knobMinV=paramVal[MIN_V_PARAM];
			knobMaxV=paramVal[MAX_V_PARAM];
			fieldRed=paramVal[RED_PARAM];
			fieldGreen=paramVal[GREEN_PARAM];
			fieldBlue=paramVal[BLUE_PARAM];
			indexGateTrsh=paramVal[GATE_THRESHOLD_PARAM];
			
			// if random button is pressed: create a new map
			if (paramVal[RANDOM_PATTERN_PARAM]==1) {diceTheSeq();}
			
		}

		// let's see the reset signal
		newReset=inputs[RESET_INPUT].getVoltage();
		if (newReset>2.0f && oldReset<=2.0f) {
			restartPlease=true;
		}
		oldReset=newReset;

		// let's see the clock signal
		newClock=inputs[CLOCK_INPUT].getVoltage();
		if (newClock>2.0f && oldClock<=2.0f) {

			/* newer polyphonic output */
			outputs[CV_PURE_OUTPUT].channels=1;
			outputs[CV_INVERSE_OUTPUT].channels=1;
			outputs[CV_MUTANT_OUTPUT].channels=indexMaxChan;
			outputs[CV_INVERSED_MUTANT_OUTPUT].channels=indexMaxChan;
			outputs[GATE_PURE_OUTPUT].channels=1;
			outputs[GATE_INVERSE_OUTPUT].channels=1;
			outputs[GATE_MUTANT_OUTPUT].channels=indexMaxChan;
			outputs[GATE_INVERSED_MUTANT_OUTPUT].channels=indexMaxChan;

			// new clock signal is in so let's see
			countDiv++;
			if (countDiv>=paramVal[CLOCK_DIVISION_PARAM]) {countDiv=0;}
			
			// if the clock in also fits the CLOCK_DIVISION_PARAM settings
			if (countDiv==0) {

				// mutate if needed
				if (paramVal[MUTATE_PARAM]>0) {mutateTheSeq();}

				// ugh... this way tough
				// anyway I calculate the next step (see currStep) here
				if (restartPlease) {
					restartPlease=false;
					currStep=paramVal[START_READING_PARAM];
				}
				else if (readingForward) {
					currStep=currStep+paramVal[READING_INCREMENTS_PARAM];
					if (currStep>paramVal[END_READING_PARAM]) {currStep-=readingLength;}
					if (currStep<paramVal[START_READING_PARAM]) {currStep=paramVal[START_READING_PARAM];}
				}
				else {
					currStep=currStep-paramVal[READING_INCREMENTS_PARAM];
					if (currStep>paramVal[START_READING_PARAM]) {currStep=paramVal[START_READING_PARAM];}	
					if (currStep<paramVal[END_READING_PARAM]) {
						// currStep+=readingLength;
						currStep=paramVal[START_READING_PARAM]+currStep-paramVal[END_READING_PARAM]+1;
					}
				}

				drawMap();

				// backlight on
				if (currStep>24) {
					lights[STEPS_D_COLUMN_SEGMENT+3*((currStep-24)-1)+0].setBrightness(1);
					lights[STEPS_D_COLUMN_SEGMENT+3*((currStep-24)-1)+2].setBrightness(1);
				}
				else if (currStep>16) {
					lights[STEPS_C_COLUMN_SEGMENT+3*((currStep-16)-1)+0].setBrightness(1);
					lights[STEPS_C_COLUMN_SEGMENT+3*((currStep-16)-1)+2].setBrightness(1);
				}
				else if (currStep>8) {
					lights[STEPS_B_COLUMN_SEGMENT+3*((currStep-8)-1)+0].setBrightness(1);
					lights[STEPS_B_COLUMN_SEGMENT+3*((currStep-8)-1)+2].setBrightness(1);
				}
				else {
					lights[STEPS_A_COLUMN_SEGMENT+3*(currStep-1)+0].setBrightness(1);
					lights[STEPS_A_COLUMN_SEGMENT+3*(currStep-1)+2].setBrightness(1);
				}

				// --------------------------------------------------
			
				/* newer polyphonic output */				
				for (int c=0;c<indexMaxChan;c++) {newVolts[c]=theSeq[currStep-1];}				
				outputs[CV_PURE_OUTPUT].setVoltage(newVolts[0]*abs(knobMaxV-knobMinV)+knobMinV);
				outputs[CV_INVERSE_OUTPUT].setVoltage((1-newVolts[0])*abs(knobMaxV-knobMinV)+knobMinV);
				outputs[GATE_PURE_OUTPUT].setVoltage(newVolts[0]>=indexGateTrsh?10:0);
				outputs[GATE_INVERSE_OUTPUT].setVoltage((1-newVolts[0])>indexGateTrsh?10:0);				
				for (int c=0;c<indexMaxChan;c++) {
					if (rack::random::uniform()<paramVal[MUTATE_PARAM]) {
						// for (int c=0;c<indexMaxChan;c++) {
							newVolts[c]=rack::random::uniform();
							// }				
					}
					newVolt=newVolts[c];
					outputs[CV_MUTANT_OUTPUT].setVoltage(newVolt*abs(knobMaxV-knobMinV)+knobMinV,c);
					outputs[CV_INVERSED_MUTANT_OUTPUT].setVoltage((1-newVolt)*abs(knobMaxV-knobMinV)+knobMinV,c);
					outputs[GATE_MUTANT_OUTPUT].setVoltage(newVolt>indexGateTrsh?10:0,c);
					outputs[GATE_INVERSED_MUTANT_OUTPUT].setVoltage((1-newVolt)>indexGateTrsh?10:0,c);				
				}
			}
		}
		// else if (newClock>2.0f && oldClock>2.0f) {}
		// else if (newClock<=2.0f && oldClock<=2.0f) {}
		else if (newClock<=2.0f && oldClock>2.0f) {
			// if gate signals are requested we also 'switch off' by the 'clock off'
			outputs[GATE_PURE_OUTPUT].setVoltage(0);
			outputs[GATE_INVERSE_OUTPUT].setVoltage(0);
			for (int c=0;c<indexMaxChan;c++) {
				outputs[GATE_MUTANT_OUTPUT].setVoltage(0,c);
				outputs[GATE_INVERSED_MUTANT_OUTPUT].setVoltage(0,c);
			}
		}
		oldClock=newClock;
	}

	// --------------------------------------------------

	// this block is to save and reload a variable
	json_t* dataToJson() override {
	json_t* rootJ = json_object();
	json_object_set_new(rootJ, "restart", json_integer(indexRestart));
	json_object_set_new(rootJ, "mutants", json_integer(indexMutants));
	json_object_set_new(rootJ, "maxout", json_integer(indexMaxChan));
	// json_object_set_new(rootJ, "notes", json_float(theSeq));
	json_t* notes_json_array = json_array();
	for (int notes_index=0; notes_index < 32; notes_index++) {
		json_array_append_new(notes_json_array , json_real(theSeq[notes_index]));
	}	
	json_object_set(rootJ, "notes", notes_json_array);
	return rootJ;}

	void dataFromJson(json_t* rootJ) override {
	json_t *restartM = json_object_get(rootJ, "restart");
	if (restartM) indexRestart = json_integer_value(restartM);
	json_t *mutantM = json_object_get(rootJ, "mutants");
	if (mutantM) indexMutants = json_integer_value(mutantM);
	json_t *maxoutM = json_object_get(rootJ, "maxout");
	if (maxoutM) indexMaxChan = json_integer_value(maxoutM);
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

struct SheepWideWidget : ModuleWidget {

	SheepWide* module;
	#include "share/widgetwizard.hpp"   // addChild abbreviations

	SheepWideWidget(SheepWide* module) {

		this->module = module;
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/SheepWide.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		
		childKnob(SheepWide::MUTATE_PARAM, 1, HP*3, HP*5);
		childButtonmom(SheepWide::RANDOM_PATTERN_PARAM, 1, HP*5, HP*5);

		childInput(SheepWide::MUTATE_INPUT, HP*1.25, HP*4.5);
		childInput(SheepWide::RANDOM_INPUT, HP*6.75, HP*4.5);

		childKnob(SheepWide::RED_PARAM, 0, HP*2, HP*7.5);
		childKnob(SheepWide::GREEN_PARAM, 0, HP*4, HP*7.5);
		childKnob(SheepWide::BLUE_PARAM, 0, HP*6, HP*7.5);

		childKnob(SheepWide::GATE_THRESHOLD_PARAM, 0, HP*1.5, HP*19);
		childKnob(SheepWide::MIN_V_PARAM, 0, HP*4, HP*19);
		childKnob(SheepWide::MAX_V_PARAM, 0, HP*6.5, HP*19);

		childKnob(SheepWide::START_READING_PARAM, 0, HP*2.75, HP*20.75);
		childKnob(SheepWide::END_READING_PARAM, 0, HP*5.25, HP*20.75);
		
		childKnob(SheepWide::CLOCK_DIVISION_PARAM, 0, HP*1, HP*23);
		childKnob(SheepWide::READING_INCREMENTS_PARAM, 0, HP*3, HP*23);

		SegmentDisplay* STEPS_A_COLUMN_SEGMENT_ID = createWidget<SegmentDisplay>(mm2px(Vec(HP*2, HP*8.75)));
		STEPS_A_COLUMN_SEGMENT_ID->vertical = true;
		STEPS_A_COLUMN_SEGMENT_ID->box.size = mm2px(Vec(HP*1, HP*8));
		STEPS_A_COLUMN_SEGMENT_ID->margin = 1.25;
		STEPS_A_COLUMN_SEGMENT_ID->setLights<RedGreenBlueLight>(module, SheepWide::STEPS_A_COLUMN_SEGMENT, 8);
		addChild(STEPS_A_COLUMN_SEGMENT_ID);

		SegmentDisplay* STEPS_B_COLUMN_SEGMENT_ID = createWidget<SegmentDisplay>(mm2px(Vec(HP*3, HP*8.75)));
		STEPS_B_COLUMN_SEGMENT_ID->vertical = true;
		STEPS_B_COLUMN_SEGMENT_ID->box.size = mm2px(Vec(HP*1, HP*8));
		STEPS_B_COLUMN_SEGMENT_ID->margin = 1.25;
		STEPS_B_COLUMN_SEGMENT_ID->setLights<RedGreenBlueLight>(module, SheepWide::STEPS_B_COLUMN_SEGMENT, 8);
		addChild(STEPS_B_COLUMN_SEGMENT_ID);

		SegmentDisplay* STEPS_C_COLUMN_SEGMENT_ID = createWidget<SegmentDisplay>(mm2px(Vec(HP*4, HP*8.75)));
		STEPS_C_COLUMN_SEGMENT_ID->vertical = true;
		STEPS_C_COLUMN_SEGMENT_ID->box.size = mm2px(Vec(HP*1, HP*8));
		STEPS_C_COLUMN_SEGMENT_ID->margin = 1.25;
		STEPS_C_COLUMN_SEGMENT_ID->setLights<RedGreenBlueLight>(module, SheepWide::STEPS_C_COLUMN_SEGMENT, 8);
		addChild(STEPS_C_COLUMN_SEGMENT_ID);

		SegmentDisplay* STEPS_D_COLUMN_SEGMENT_ID = createWidget<SegmentDisplay>(mm2px(Vec(HP*5, HP*8.75)));
		STEPS_D_COLUMN_SEGMENT_ID->vertical = true;
		STEPS_D_COLUMN_SEGMENT_ID->box.size = mm2px(Vec(HP*1, HP*8));
		STEPS_D_COLUMN_SEGMENT_ID->margin = 1.25;
		STEPS_D_COLUMN_SEGMENT_ID->setLights<RedGreenBlueLight>(module, SheepWide::STEPS_D_COLUMN_SEGMENT, 8);
		addChild(STEPS_D_COLUMN_SEGMENT_ID);
		
		childOutput(SheepWide::GATE_PURE_OUTPUT, HP*1, HP*9.75);
		childOutput(SheepWide::GATE_INVERSE_OUTPUT, HP*1, HP*11.75);
		childOutput(SheepWide::GATE_MUTANT_OUTPUT, HP*1, HP*13.75);
		childOutput(SheepWide::GATE_INVERSED_MUTANT_OUTPUT, HP*1, HP*15.75);
		childOutput(SheepWide::CV_PURE_OUTPUT, HP*7, HP*9.75);
		childOutput(SheepWide::CV_INVERSE_OUTPUT, HP*7, HP*11.75);
		childOutput(SheepWide::CV_MUTANT_OUTPUT, HP*7, HP*13.75);
		childOutput(SheepWide::CV_INVERSED_MUTANT_OUTPUT, HP*7, HP*15.75);

		childInput(SheepWide::CLOCK_INPUT, HP*5, HP*23);
		childInput(SheepWide::RESET_INPUT, HP*7, HP*23);

		childLabel(HP*5,HP*1, "DEV", 12);
	}

	// shortkey
	void onHoverKey(const event::HoverKey &e) override {
		if (e.key >= GLFW_KEY_1 && e.key <= GLFW_KEY_4) {
			if (e.action == GLFW_PRESS && ((e.mods & RACK_MOD_MASK) == GLFW_MOD_SHIFT)) {
				float key_number = e.key - 49; // 49 is the ascii number for key #1
				module->clearKnob(key_number);	// this doesn't work
				e.consume(this);
			}
			else if (e.action == GLFW_PRESS) {
				float key_number = e.key - 49; // 49 is the ascii number for key #1
				module->keyKnob(key_number);
				e.consume(this);
			}
		}
		else if (e.key == GLFW_KEY_P && e.action == GLFW_PRESS ) {
			if ((e.mods & RACK_MOD_MASK) == GLFW_MOD_SHIFT) {module->diceColors(0);e.consume(this);} 
			else {module->diceColors(1);e.consume(this);}
		}
		else if (e.key == GLFW_KEY_G && e.action == GLFW_PRESS ) {
			if ((e.mods & RACK_MOD_MASK) == GLFW_MOD_SHIFT) {
				float xf=module->indexGateTrsh;
				if (xf==0.9) {xf=0.75;}
				else if (xf==0.75) {xf=0.5;}
				else if (xf==0.5) {xf=0;}
				else if (xf==0) {xf=0.5;}
				else {xf=0;}
				module->params[module->GATE_THRESHOLD_PARAM].setValue(xf);
				e.consume(this);
			} 
			else {
				float xf=module->indexGateTrsh;
				if (xf == 0) {xf=0.5;}
				else if (xf==0.5) {xf=0.75;}
				else if (xf==0.75) {xf=0.9;}
				else if (xf==0.9) {xf=0;}
				else {xf=0;}
				module->params[module->GATE_THRESHOLD_PARAM].setValue(xf);
				e.consume(this);
			}
			
		}
		else if ((e.key == GLFW_KEY_R) && e.action == GLFW_PRESS && ((e.mods & RACK_MOD_MASK) != GLFW_MOD_CONTROL)) {
			module->diceTheSeq(); 
			e.consume(this);
		}
		ModuleWidget::onHoverKey(e);
	}
	
};

Model* modelSheepWide = createModel<SheepWide, SheepWideWidget>("SheepWide");
