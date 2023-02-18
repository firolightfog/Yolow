// Copyright (c) 2022 Andr?s Szab?
#include "plugin.hpp"

struct Sheep : Module {

// --------------------------------------------------

	enum ParamId    {
		MUTATE_PARAM, FREQUENCY_PARAM, START_READING_PARAM, END_READING_PARAM, READING_INCREMENTS_PARAM, CLOCK_DIVISION_PARAM, 
		RANGE_PARAM, TEMPONLY_PARAM, PARAMS_LEN};

	enum InputId    {
		CLOCK_INPUT, RESET_INPUT, INPUTS_LEN};

	enum OutputId    {
		CV_MUTANT_OUTPUT, DEBUG_OUTPUT, OUTPUTS_LEN};

	enum LightId    {
		ENUMS(STEPS_A_COLUMN_SEGMENT, 32*3),
		ENUMS(STEPS_B_COLUMN_SEGMENT, 32*3),
		ENUMS(STEPS_C_COLUMN_SEGMENT, 32*3),
		ENUMS(STEPS_D_COLUMN_SEGMENT, 32*3),
		TEMPONLY_LIGHT,
		LIGHTS_LEN};

	// small assistance to save older values for reference;
	float paramVal[PARAMS_LEN]={0};
	// float inputVolt[INPUTS_LEN]={0};
	// float lightVal[LIGHTS_LEN]={0};

// --------------------------------------------------

	Sheep() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(MUTATE_PARAM, 	0.0f, 1.0f, 0.0125f, "Mutation frequency","%", 0, 100);
		configParam(START_READING_PARAM, 		1.0f, 32.0f, 17.0f, "Start reading");
		configParam(END_READING_PARAM, 			1.0f, 32.0f, 32.0f, "End reading");
		configParam(READING_INCREMENTS_PARAM, 	1.0f, 16.0f, 1.0f, "Reading increments");
		configParam(CLOCK_DIVISION_PARAM, 		1.0f, 32.0f, 1.0f, "Clock division");
		configParam(RANGE_PARAM, 				0.0f, 10.0f, 1.0f, "Output voltage range");
		configParam(TEMPONLY_PARAM, 			0.0f, 1.0f, 0.0f, "Temporary mutation");
		paramQuantities[START_READING_PARAM]->snapEnabled = true;
		paramQuantities[START_READING_PARAM]->randomizeEnabled = false;
		paramQuantities[END_READING_PARAM]->snapEnabled = true;
		paramQuantities[END_READING_PARAM]->randomizeEnabled = false;
		paramQuantities[READING_INCREMENTS_PARAM]->snapEnabled = true;
		paramQuantities[READING_INCREMENTS_PARAM]->randomizeEnabled = false;
		paramQuantities[CLOCK_DIVISION_PARAM]->snapEnabled = true;
		paramQuantities[CLOCK_DIVISION_PARAM]->randomizeEnabled = false;

		configInput(CLOCK_INPUT, "Clock"); 
		configInput(RESET_INPUT, "Reset"); 

		configOutput(CV_MUTANT_OUTPUT, "Voltage"); 
	}

// --------------------------------------------------

	int loop=0;     // save some CPU in process()

	// for managing the RESET and CLOCK signals
	float newReset=0.0f;
	float oldReset=0.0f;
	float newClock=0.0f;
	float oldClock=0.0f;
	bool hitClock=false;
	bool offClock=false;

	// more variables
	float newVolt;				// original idea
	float newVolts[16]={0.0f}; 	// ammended idea for polyphonic output
	int readingLength=0;
	bool readingForward=true;
	int countDiv=-1;
	
	// even more variables
	bool restartPlease=true;
	float theSeq[32]={0};	// this contains the sequence, 32 slots available
	int currStep=0;
	int indexLinkStrEnd=0;
	int indexGateMode=0;
	float indexGateTrsh=0.0f;
	int indexRestart=0;
	int indexMutants=1;
	float fieldRed=0.0f;	// weight of Red in the field
	float fieldGreen=1.0f;	// weight of Green in the field
	float fieldBlue=0.0f;	// weight of Blue in the field
	float fieldColors[3]={0};
	int indexMaxChan=1;
	float knobMinV=0.0f;
	float knobMaxV=1.0f;
	
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

	// randomize one of the columns; called by shortkeys
	void keyKnob(int rndSection) {
		for (int i=(rndSection-0)*8;i<(rndSection+1)*8;i++) {theSeq[i]=rack::random::uniform();}	
	}
	
	// clear one of the columns; called by shortkeys
	void clearKnob(int rndSection) {
		for (int i=(rndSection-0)*8;i<(rndSection+1)*8;i++) {theSeq[i]=0;}
	}

	void drawMap() {
		fieldColors[0]=fieldRed;
		fieldColors[1]=fieldGreen;
		fieldColors[2]=fieldBlue;
		setGateTrsh();
		for (int i=0;i<8*3;i=i+3) {
			if (indexGateTrsh>=0.01) {
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

	// just set the limit for creating gates (if needed)
	// called by the menu and shotkeys
	void setGateTrsh() {		
		if (indexGateMode<0) {indexGateMode=3;}
		else if (indexGateMode>3) {indexGateMode=0;}
		if (indexGateMode==1){indexGateTrsh=0.5;}
		else if (indexGateMode==2){indexGateTrsh=0.75;}
		else if (indexGateMode==3){indexGateTrsh=0.9;}
		else {indexGateTrsh=0;}
	}

// --------------------------------------------------

	// Ctrl-E bypassing: no sound but the counting stays
	void processBypass(const ProcessArgs& args) override {
		newReset=inputs[RESET_INPUT].getVoltage();
		if (newReset>2.0f && oldReset<=2.0f) {restartPlease=true;}
		oldReset=newReset;
		newClock=inputs[CLOCK_INPUT].getVoltage();
		if (newClock>2.0f && oldClock<=2.0f) {
			hitClock=true; offClock=false; countDiv++;
			if (countDiv>=paramVal[CLOCK_DIVISION_PARAM]) {
				countDiv=0;
				if (restartPlease) {restartPlease=false;currStep=paramVal[START_READING_PARAM];}
				else if (readingForward) {currStep=currStep+paramVal[READING_INCREMENTS_PARAM];
					if (currStep>paramVal[END_READING_PARAM]) {
						if (indexRestart==0) {currStep=paramVal[START_READING_PARAM];}
						else {currStep-=readingLength;}
					}
					if (currStep<paramVal[START_READING_PARAM]) {currStep=paramVal[START_READING_PARAM];}
				}
				else {
					currStep=currStep-paramVal[READING_INCREMENTS_PARAM];
					if (currStep>paramVal[START_READING_PARAM]) {currStep=paramVal[START_READING_PARAM];}	
					if (currStep<paramVal[END_READING_PARAM]) {
						if (indexRestart==0) {currStep=paramVal[START_READING_PARAM];}
						else {currStep=paramVal[START_READING_PARAM]+currStep-paramVal[END_READING_PARAM]+1;}
					}
				}
			}
		}
		else if (newClock<=2.0f && oldClock>2.0f) {hitClock=false; offClock=true;}
		else {hitClock=false; offClock=false;}
		oldClock=newClock;
	}

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
				setGateTrsh();
			}

			// collect all paramter values for future reference
			for (int p=0;p<PARAMS_LEN;p++) {paramVal[p]=params[p].getValue();}			
			// readingForward=paramVal[START_READING_PARAM]<paramVal[END_READING_PARAM];
			readingLength=paramVal[END_READING_PARAM]-paramVal[START_READING_PARAM]+1;
			readingForward=(readingLength>=1);
			knobMinV=0;
			knobMaxV=paramVal[RANGE_PARAM];
			
			// if temporary changes aren't allowed then permament mutation is made
			lights[TEMPONLY_LIGHT].setBrightness(paramVal[TEMPONLY_PARAM]);
			indexMutants=paramVal[TEMPONLY_PARAM];
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

			hitClock=true; offClock=false; // info for the expander
			
			/* newer polyphonic output */
			outputs[CV_MUTANT_OUTPUT].channels=indexMaxChan;

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
					if (currStep>paramVal[END_READING_PARAM]) {
						if (indexRestart==0) {currStep=paramVal[START_READING_PARAM];}
						else {currStep-=readingLength;}
					}
					if (currStep<paramVal[START_READING_PARAM]) {currStep=paramVal[START_READING_PARAM];}
				}
				else {
					currStep=currStep-paramVal[READING_INCREMENTS_PARAM];
					if (currStep>paramVal[START_READING_PARAM]) {currStep=paramVal[START_READING_PARAM];}	
					if (currStep<paramVal[END_READING_PARAM]) {
						if (indexRestart==0) {currStep=paramVal[START_READING_PARAM];}
						else {currStep=paramVal[START_READING_PARAM]+currStep-paramVal[END_READING_PARAM]+1;}
						}
				}

				// redraw the steps & blank the backlight
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
			
				// send the output voltage (or a gate signal if requested so)				
				
				for (int c=0;c<indexMaxChan;c++) {newVolts[c]=theSeq[currStep-1];}				
				if (indexMutants==1 && paramVal[MUTATE_PARAM]<1) {
					if (rack::random::uniform()<paramVal[MUTATE_PARAM]) {
						for (int c=0;c<indexMaxChan;c++) {newVolts[c]=rack::random::uniform();}				
					}
				}
				for (int c=0;c<indexMaxChan;c++) {
					if (indexMaxChan==1 || c<indexMaxChan-1) {newVolt=newVolts[c];}
					// ez itt nem j? valahogy... mintha kimaradna egy ?tem
					else {newVolt=newVolts[c-1]; newVolt=1-newVolts[c-1];}
					if (indexGateTrsh!=0) {
						outputs[CV_MUTANT_OUTPUT].setVoltage((newVolt*paramVal[RANGE_PARAM]>indexGateTrsh)?10:0,c);
					}
					else {
						outputs[CV_MUTANT_OUTPUT].setVoltage(newVolt*paramVal[RANGE_PARAM],c);
					}
				}
			}
		}
		// else if (newClock>2.0f && oldClock>2.0f) {}
		// else if (newClock<=2.0f && oldClock<=2.0f) {}
		else if (newClock<=2.0f && oldClock>2.0f) {
			hitClock=false; offClock=true; 	// info for the expander
			if (indexGateTrsh!=0) {
				for (int c=0;c<indexMaxChan;c++) {outputs[CV_MUTANT_OUTPUT].setVoltage(0,c);}
			}
		}
		else {hitClock=false; offClock=false;}
		oldClock=newClock;

	}

	// --------------------------------------------------

	// this block is to save and reload a variable
	json_t* dataToJson() override {
	json_t* rootJ = json_object();
	json_object_set_new(rootJ, "gatemode", json_real(indexGateMode));
	json_object_set_new(rootJ, "restart", json_integer(indexRestart));
	json_object_set_new(rootJ, "mutants", json_integer(indexMutants));
	json_object_set_new(rootJ, "maxout", json_integer(indexMaxChan));
	// json_object_set_new(rootJ, "notes", json_float(theSeq));
	json_t* notes_json_array = json_array();
	for (int notes_index=0; notes_index < 32; notes_index++) {
		json_array_append_new(notes_json_array , json_real(theSeq[notes_index]));
	}	
	json_object_set_new(rootJ, "notes", notes_json_array);
	json_object_set_new(rootJ, "fieldred", json_real(fieldRed));
	json_object_set_new(rootJ, "fieldgreen", json_real(fieldGreen));
	json_object_set_new(rootJ, "fieldblue", json_real(fieldBlue));
	return rootJ;}

	void dataFromJson(json_t* rootJ) override {
	json_t *gateM = json_object_get(rootJ, "gatemode");
	if (gateM) indexGateMode = json_real_value(gateM);
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
	json_t *fieldRJ = json_object_get(rootJ, "fieldred");
	if (fieldRJ) fieldRed = json_real_value(fieldRJ);
	json_t *fieldGJ = json_object_get(rootJ, "fieldgreen");
	if (fieldGJ) fieldGreen = json_real_value(fieldGJ);
	json_t *fieldBJ = json_object_get(rootJ, "fieldblue");
	if (fieldBJ) fieldBlue = json_real_value(fieldBJ);
	}

};

// --------------------------------------------------

struct SheepWidget : ModuleWidget {

	Sheep* module;
	#include "share/widgetwizard.hpp"   // addChild abbreviations

	SheepWidget(Sheep* module) {

		this->module = module;
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/Sheep.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		childPushbutton(Sheep::TEMPONLY_PARAM, Sheep::TEMPONLY_LIGHT, HP*3, HP*16);		
		childKnob(Sheep::MUTATE_PARAM, 					1, HP*1, HP*17);
		childKnob(Sheep::CLOCK_DIVISION_PARAM, 			1, HP*3, HP*19);
		childKnob(Sheep::READING_INCREMENTS_PARAM, 		1, HP*1, HP*20);
		childKnob(Sheep::END_READING_PARAM, 			1, HP*3, HP*22);
		childKnob(Sheep::START_READING_PARAM, 			1, HP*1, HP*23);
		
		childInput(Sheep::CLOCK_INPUT, HP*1, HP*4.5);
		childInput(Sheep::RESET_INPUT, HP*3, HP*4.5);
		childOutput(Sheep::CV_MUTANT_OUTPUT, HP*1, HP*6.5);
		childKnob(Sheep::RANGE_PARAM, 0, HP*3, HP*6.5);

		// childOutput(Sheep::DEBUG_OUTPUT, HP*2.5, HP*24.5);

		// ----- segment lights are coming here ---------------------------------------------

		SegmentDisplay* STEPS_A_COLUMN_SEGMENT_ID = createWidget<SegmentDisplay>(mm2px(Vec(HP*0.25, HP*7.5)));
		STEPS_A_COLUMN_SEGMENT_ID->vertical = true;
		STEPS_A_COLUMN_SEGMENT_ID->box.size = mm2px(Vec(HP*0.85, HP*8*0.9));
		STEPS_A_COLUMN_SEGMENT_ID->margin = 1.25f;
		STEPS_A_COLUMN_SEGMENT_ID->setLights<RedGreenBlueLight>(module, Sheep::STEPS_A_COLUMN_SEGMENT, 8);
		addChild(STEPS_A_COLUMN_SEGMENT_ID);

		SegmentDisplay* STEPS_B_COLUMN_SEGMENT_ID = createWidget<SegmentDisplay>(mm2px(Vec(HP*(0.25+1*0.9), HP*7.5)));
		STEPS_B_COLUMN_SEGMENT_ID->vertical = true;
		STEPS_B_COLUMN_SEGMENT_ID->box.size = mm2px(Vec(HP*0.85, HP*8*0.9));
		STEPS_B_COLUMN_SEGMENT_ID->margin = 1.25f;
		STEPS_B_COLUMN_SEGMENT_ID->setLights<RedGreenBlueLight>(module, Sheep::STEPS_B_COLUMN_SEGMENT, 8);
		addChild(STEPS_B_COLUMN_SEGMENT_ID);

		SegmentDisplay* STEPS_C_COLUMN_SEGMENT_ID = createWidget<SegmentDisplay>(mm2px(Vec(HP*(0.25+2*0.9), HP*7.5)));
		STEPS_C_COLUMN_SEGMENT_ID->vertical = true;
		STEPS_C_COLUMN_SEGMENT_ID->box.size = mm2px(Vec(HP*0.85, HP*8*0.9));
		STEPS_C_COLUMN_SEGMENT_ID->margin = 1.25f;
		STEPS_C_COLUMN_SEGMENT_ID->setLights<RedGreenBlueLight>(module, Sheep::STEPS_C_COLUMN_SEGMENT, 8);
		addChild(STEPS_C_COLUMN_SEGMENT_ID);

		SegmentDisplay* STEPS_D_COLUMN_SEGMENT_ID = createWidget<SegmentDisplay>(mm2px(Vec(HP*(0.25+3*0.9), HP*7.5)));
		STEPS_D_COLUMN_SEGMENT_ID->vertical = true;
		STEPS_D_COLUMN_SEGMENT_ID->box.size = mm2px(Vec(HP*0.85, HP*8*0.9));
		STEPS_D_COLUMN_SEGMENT_ID->margin = 1.25f;
		STEPS_D_COLUMN_SEGMENT_ID->setLights<RedGreenBlueLight>(module, Sheep::STEPS_D_COLUMN_SEGMENT, 8);
		addChild(STEPS_D_COLUMN_SEGMENT_ID);

		// --------------------------------------------------

		// childLabel(HP*5,HP*1, "DEV", 12);
	}

	void appendContextMenu(Menu* menu) override {
		Sheep* module = dynamic_cast<Sheep*>(this->module);
		assert(module);
		menu->addChild(new MenuSeparator);
		menu->addChild(createIndexPtrSubmenuItem("Restart mode", {"Always","Only on reset"}, &module->indexRestart));
		menu->addChild(createIndexSubmenuItem("Voltage mode (also Shift-G)",
			{"Voltage (0V-1V)","Gates (above 0.5V)","Gates (above 0.75V)","Gates (above 0.9V)"},
			[=]() {return module->indexGateMode;},
			[=](int mode) {module->indexGateMode=mode;module->setGateTrsh();}
		));	
		menu->addChild(createIndexPtrSubmenuItem("Number of output channels", {"0","1","2","3","4","5","6","7","8","9","10","11","12","13","14","15","16"}, &module->indexMaxChan));
		menu->addChild(createIndexPtrSubmenuItem("Link START and END knobs", {"Unlinked","Linked"}, &module->indexLinkStrEnd));
	}

	// shortkey
	void onHoverKey(const event::HoverKey &e) override {
		if (e.key>=GLFW_KEY_1 && e.key <= GLFW_KEY_4 && e.action==GLFW_PRESS) {
			if ((e.mods & RACK_MOD_MASK)==GLFW_MOD_SHIFT) {module->clearKnob(e.key-49); e.consume(this);}
			else {module->keyKnob(e.key-49); e.consume(this);}
		}
		else if (e.key==GLFW_KEY_P && e.action==GLFW_PRESS ) {
			if ((e.mods & RACK_MOD_MASK)==GLFW_MOD_SHIFT) {module->diceColors(0); e.consume(this);} 
			else {module->diceColors(1);e.consume(this);
			}			
		}
		else if (e.key==GLFW_KEY_G && e.action==GLFW_PRESS ) {
			if ((e.mods & RACK_MOD_MASK)==GLFW_MOD_SHIFT) {
				module->indexGateMode--;
				module->setGateTrsh();
				e.consume(this);
			} 
			else {
				module->indexGateMode++;
				module->setGateTrsh();
				e.consume(this);
			}
			// module->drawMap();
		}
		else if ((e.key==GLFW_KEY_R) && e.action==GLFW_PRESS 
		&& ((e.mods & RACK_MOD_MASK) != GLFW_MOD_CONTROL)) {
			module->diceTheSeq(); 
			e.consume(this);
		}
		ModuleWidget::onHoverKey(e);
	}

};

Model* modelSheep = createModel<Sheep, SheepWidget>("Sheep");

// --------------------------------------------------

#include "SheepMore.hpp"	// this is a simple expander