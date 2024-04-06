// Copyright (c) 2023 András Szabó
#include "plugin.hpp"

struct RouteSeq : Module {

// --------------------------------------------------

	enum ParamId  {
		CHANNEL_SELECTOR_1_PARAM, CHANNEL_SELECTOR_2_PARAM, CHANNEL_SELECTOR_3_PARAM, CHANNEL_SELECTOR_4_PARAM, CHANNEL_SELECTOR_5_PARAM, 
		CHANNEL_SELECTOR_6_PARAM, CHANNEL_SELECTOR_7_PARAM, CHANNEL_SELECTOR_8_PARAM, STEP_FREEZER_1_PARAM, STEP_FREEZER_2_PARAM, 
		STEP_FREEZER_3_PARAM, STEP_FREEZER_4_PARAM, STEP_FREEZER_5_PARAM, STEP_FREEZER_6_PARAM, STEP_FREEZER_7_PARAM, 
		STEP_FREEZER_8_PARAM, PARAMS_LEN};

	enum InputId  {
		CLOCK_INPUT, RESET_INPUT, POLY_INPUT, INPUTS_LEN};

	enum OutputId {
		MONO_OUTPUT, DEBUG_OUTPUT, OUTPUTS_LEN};

	enum LightId  {
		LIGHT_1_LIGHT, LIGHT_2_LIGHT, LIGHT_3_LIGHT, LIGHT_4_LIGHT, LIGHT_5_LIGHT, 
		LIGHT_6_LIGHT, LIGHT_7_LIGHT, LIGHT_8_LIGHT, LIGHTS_LEN};

	// small assistance to save older values for reference;
	float paramVal[PARAMS_LEN]={0};
	// float inputVolt[INPUTS_LEN]={0};
	// float lightVal[LIGHTS_LEN]={0};

// --------------------------------------------------

	RouteSeq() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		
		configParam(CHANNEL_SELECTOR_1_PARAM, 	0.0f, 16.0f, 1.0f, "Channel selector 1");
		configParam(CHANNEL_SELECTOR_2_PARAM, 	0.0f, 16.0f, 1.0f, "Channel selector 2");
		configParam(CHANNEL_SELECTOR_3_PARAM, 	0.0f, 16.0f, 1.0f, "Channel selector 3");
		configParam(CHANNEL_SELECTOR_4_PARAM, 	0.0f, 16.0f, 1.0f, "Channel selector 4");
		configParam(CHANNEL_SELECTOR_5_PARAM, 	0.0f, 16.0f, 1.0f, "Channel selector 5");
		configParam(CHANNEL_SELECTOR_6_PARAM, 	0.0f, 16.0f, 1.0f, "Channel selector 6");
		configParam(CHANNEL_SELECTOR_7_PARAM, 	0.0f, 16.0f, 1.0f, "Channel selector 7");
		configParam(CHANNEL_SELECTOR_8_PARAM, 	0.0f, 16.0f, 1.0f, "Channel selector 8");

		configParam(STEP_FREEZER_1_PARAM, 	0.0f, 16.0f, 1.0f, "Step freezer 1");
		configParam(STEP_FREEZER_2_PARAM, 	0.0f, 16.0f, 1.0f, "Step freezer 2");
		configParam(STEP_FREEZER_3_PARAM, 	0.0f, 16.0f, 1.0f, "Step freezer 3");
		configParam(STEP_FREEZER_4_PARAM, 	0.0f, 16.0f, 1.0f, "Step freezer 4");
		configParam(STEP_FREEZER_5_PARAM, 	0.0f, 16.0f, 1.0f, "Step freezer 5");
		configParam(STEP_FREEZER_6_PARAM, 	0.0f, 16.0f, 1.0f, "Step freezer 6");
		configParam(STEP_FREEZER_7_PARAM, 	0.0f, 16.0f, 1.0f, "Step freezer 7");
		configParam(STEP_FREEZER_8_PARAM, 	0.0f, 16.0f, 1.0f, "Step freezer 8");
		
		paramQuantities[CHANNEL_SELECTOR_1_PARAM]->snapEnabled = true;
		paramQuantities[CHANNEL_SELECTOR_2_PARAM]->snapEnabled = true;
		paramQuantities[CHANNEL_SELECTOR_3_PARAM]->snapEnabled = true;
		paramQuantities[CHANNEL_SELECTOR_4_PARAM]->snapEnabled = true;
		paramQuantities[CHANNEL_SELECTOR_5_PARAM]->snapEnabled = true;
		paramQuantities[CHANNEL_SELECTOR_6_PARAM]->snapEnabled = true;
		paramQuantities[CHANNEL_SELECTOR_7_PARAM]->snapEnabled = true;
		paramQuantities[CHANNEL_SELECTOR_8_PARAM]->snapEnabled = true;
		
		paramQuantities[STEP_FREEZER_1_PARAM]->snapEnabled = true;
		paramQuantities[STEP_FREEZER_2_PARAM]->snapEnabled = true;
		paramQuantities[STEP_FREEZER_3_PARAM]->snapEnabled = true;
		paramQuantities[STEP_FREEZER_4_PARAM]->snapEnabled = true;
		paramQuantities[STEP_FREEZER_5_PARAM]->snapEnabled = true;
		paramQuantities[STEP_FREEZER_6_PARAM]->snapEnabled = true;
		paramQuantities[STEP_FREEZER_7_PARAM]->snapEnabled = true;
		paramQuantities[STEP_FREEZER_8_PARAM]->snapEnabled = true;

 		paramQuantities[STEP_FREEZER_1_PARAM]->randomizeEnabled = false;
		paramQuantities[STEP_FREEZER_2_PARAM]->randomizeEnabled = false;
		paramQuantities[STEP_FREEZER_3_PARAM]->randomizeEnabled = false;
		paramQuantities[STEP_FREEZER_4_PARAM]->randomizeEnabled = false;
		paramQuantities[STEP_FREEZER_5_PARAM]->randomizeEnabled = false;
		paramQuantities[STEP_FREEZER_6_PARAM]->randomizeEnabled = false;
		paramQuantities[STEP_FREEZER_7_PARAM]->randomizeEnabled = false;
		paramQuantities[STEP_FREEZER_8_PARAM]->randomizeEnabled = false;

		configInput(CLOCK_INPUT, "Clock"); 
		configInput(RESET_INPUT, "Reset"); 
		configInput(POLY_INPUT, "Poly"); 

		configOutput(MONO_OUTPUT, "Mono"); 
		configOutput(DEBUG_OUTPUT, "DEBUG"); 
	}

// --------------------------------------------------

	int loop=0;     // save some CPU in process()

	int indexOctave=3; 	// -3,-2,-1,0,1,2,3
	int indexRange=0;	// if no poly input then 0 semitones, 1 octaves

	// for managing the RESET and CLOCK signals
	float newReset=0.0f;
	float oldReset=0.0f;
	float newClock=0.0f;
	float oldClock=0.0f;
	const int ON=1; 
	const int OFF=0;
	const int NA=-1;
	int hitClock=NA;
	int hitReset=NA;
	
	
	// for calculating the sequencer steps
	int currentStep=0;
	int totalSteps=0;
	int knobSteps=1;
	int needed=0;
	int randval=1;

	// for the additional labels of channel selector knobs
	const std::string valNotes[17]={"random",
	"C-4","C#4","D-4","D#4","E-4",
	"F-4","F#4","G-4","G#4","A-4",
	"A#4","B-4","C-5","C#5","D-5","D#5"};

	// OK so in the menu these are the options: 
	/* 
	const int valPatterns[9]={8,12,16,24,32,48,64,96,128};
	const char* txtPatterns[8]={"8 steps", // 0
			"12 steps", // 1
			"16 steps", // 2
			"24 steps", // 3
			"32 steps", // 4
			"64 steps", // 5
			"96 steps", // 6
			"128 steps"};
	 */
	int getPattern() {
		if (totalSteps==8) {return 0;}
		else if (totalSteps==12) {return 1;}
		else if (totalSteps==16) {return 2;}
		else if (totalSteps==24) {return 3;}
		else if (totalSteps==32) {return 4;}
		else if (totalSteps==64) {return 5;}
		else if (totalSteps==96) {return 6;}
		else if (totalSteps==128) {return 7;}
		else {return -1;}
	}
	
	// and this is a routine called from the menu
	void rndPattern(int ptx) {
		int tries=0;
		int valSteps[8]={1,1,1,1,1,1,1,1};
		if (ptx==7) {
			for (int i=0;i<8;i++) {valSteps[i]=16;};
		}
		else {
			int pty=0;
			if (ptx==1) {tries=12;}
			else if (ptx==2) {tries=16;}
			else if (ptx==3) {tries=24;}
			else if (ptx==4) {tries=32;}
			else if (ptx==5) {tries=64;}
			else if (ptx==6) {tries=96;}
			for (int i=0;i<tries-8;i++) {
				for (int d=0;d<9999;d++) {
					pty=rand() % 8;
					if (valSteps[pty]<16) {break;}
				}
				valSteps[pty]+=1;
			};
		}
		for (int p=0;p<8;p++) {params[STEP_FREEZER_1_PARAM+p].setValue(valSteps[p]);};
	}

	// this is called from the menu and it's shuffling the selection knobs
	void shuffleSelectors() {
		int newOrder[8]={-1,-1,-1,-1,-1,-1,-1,-1};
		int theOrder=-1;
			for (int p=0;p<8;p++) {
				// OK this in not nice; it's to avoid unutilized slots
				for (int r=0;r<999;r++) { 
					theOrder=rand() % 8;
					if (newOrder[theOrder]==-1) {
						newOrder[theOrder]=paramVal[CHANNEL_SELECTOR_1_PARAM+p];
						break;
					}
				}
			}
			for (int p=0;p<8;p++) {
				params[CHANNEL_SELECTOR_1_PARAM+p].setValue(newOrder[p]);
			}		
	}

	// OK, so this is going to identify if there's a mother to the left
	RouteSeq* findHostModulePtr(Module* module) {
		if (module) {
			if (module->leftExpander.module) {
				// if it's the mother module, we're done
				if (module->leftExpander.module->model == modelRouteSeq) {
					return reinterpret_cast<RouteSeq*>(module->leftExpander.module);
				}
			}
		}
		return nullptr;
	}

	void process(const ProcessArgs& args) override {

		if (loop--<=0) {
			loop=8972;
			// save some more CPU
			for (int p=0;p<PARAMS_LEN;p++) {paramVal[p]=params[p].getValue();}
			
			// calculate the length of the loops based on the freezer/clockdiv knobs
			totalSteps=0;
			for (int p=0;p<8;p++) {totalSteps=totalSteps+paramVal[STEP_FREEZER_1_PARAM+p];}
			
			// add labels to the knobs
			for (int p=0;p<8;p++) {
				paramQuantities[STEP_FREEZER_1_PARAM+p]->description = ("Total steps: " + std::to_string(totalSteps));
				if (inputs[POLY_INPUT].isConnected()) {
					paramQuantities[CHANNEL_SELECTOR_1_PARAM+p]->description = "Poly cable channel selected";
					paramQuantities[CHANNEL_SELECTOR_1_PARAM+p]->snapEnabled = true;
					paramQuantities[CHANNEL_SELECTOR_1_PARAM+p]->displayMultiplier = 1;
					paramQuantities[CHANNEL_SELECTOR_1_PARAM+p]->displayOffset = 0;					
				}
				else {
					paramQuantities[CHANNEL_SELECTOR_1_PARAM+p]->snapEnabled = (indexRange>0)?false:true;
					// v2.1.23 added next section
					switch (indexRange) {
						case 0: 
							paramQuantities[CHANNEL_SELECTOR_1_PARAM+p]->description = ("Without cable: " + valNotes[(int)paramVal[CHANNEL_SELECTOR_1_PARAM+p]]);
							paramQuantities[CHANNEL_SELECTOR_1_PARAM+p]->displayMultiplier = 1;
							paramQuantities[CHANNEL_SELECTOR_1_PARAM+p]->displayOffset =0; 
							break;
						case 1: 
							paramQuantities[CHANNEL_SELECTOR_1_PARAM+p]->description = ("Without cable: -1V to 1V");
							paramQuantities[CHANNEL_SELECTOR_1_PARAM+p]->displayMultiplier = 0.125f;
							paramQuantities[CHANNEL_SELECTOR_1_PARAM+p]->displayOffset = -1; 
							break;						
						case 2: 
							paramQuantities[CHANNEL_SELECTOR_1_PARAM+p]->description = ("Without cable: -3V to 3V");
							paramQuantities[CHANNEL_SELECTOR_1_PARAM+p]->displayMultiplier = 0.375;
							paramQuantities[CHANNEL_SELECTOR_1_PARAM+p]->displayOffset = -3; 
							break;						
						case 3: 
							paramQuantities[CHANNEL_SELECTOR_1_PARAM+p]->description = ("Without cable: -5V to 5V");
							paramQuantities[CHANNEL_SELECTOR_1_PARAM+p]->displayMultiplier = 0.625;
							paramQuantities[CHANNEL_SELECTOR_1_PARAM+p]->displayOffset = -5; 
							break;						
						case 4: 
							paramQuantities[CHANNEL_SELECTOR_1_PARAM+p]->description = ("Without cable: -10V to 10V");
							paramQuantities[CHANNEL_SELECTOR_1_PARAM+p]->displayMultiplier = 1.25;
							paramQuantities[CHANNEL_SELECTOR_1_PARAM+p]->displayOffset = -10; 
							break;						
						default: 
							paramQuantities[CHANNEL_SELECTOR_1_PARAM+p]->description = ("Without cable: that's for sure");
							paramQuantities[CHANNEL_SELECTOR_1_PARAM+p]->displayMultiplier = 1;
							paramQuantities[CHANNEL_SELECTOR_1_PARAM+p]->displayOffset =0; 
					}

					
				}
			}
			
			outputs[DEBUG_OUTPUT].channels=8;
			
		}

		// let's see if we have mother here
		// RouteSeq const* mother = nullptr; // findHostModulePtr(this);
		RouteSeq const* mother = findHostModulePtr(this);

		// let's see the clock signal
		// if there is a RouteSeq on the left then grab the clock and reset info
		if (mother && inputs[CLOCK_INPUT].isConnected()==false) {hitClock=mother->hitClock;}
		else {
			// let's see if the CLOCK has just been ON, OFF or else (NA)
			newClock=inputs[CLOCK_INPUT].getVoltage();
			if (newClock>2.0f && oldClock<=2.0f) {hitClock=ON;}
			else if (oldClock>2.0f && newClock<=2.0f) {hitClock=OFF;}
			else {hitClock=NA;}
			oldClock=newClock;
		}

		// let's see the reset signal
		if (mother && inputs[RESET_INPUT].isConnected()==false) {hitReset=mother->hitReset;}
		else {
			newReset=inputs[RESET_INPUT].getVoltage();
			if (newReset>2.0f && oldReset<=2.0f) {hitReset=ON;}
			else if (oldReset>2.0f && newReset<=2.0f) {hitReset=OFF;}
			else if (hitReset==ON) {hitReset=NA;}
			oldReset=newReset;
		}

		// let's find the right step	
		if (hitClock==ON && !(hitReset==ON)) {
			currentStep++;	// v2.1.23 added
			if (currentStep>=totalSteps) {currentStep=0; randval= 1 + (rand() % 16);}
			// else {currentStep++;} // v2.1.23 removed
		} 
		else if (hitClock==ON && hitReset==ON) {currentStep=0;}
		else if (!(hitClock==ON) && hitReset==ON) {currentStep=-1;}	// itt ez rossz lehet
		else if (hitClock==OFF && hitReset==OFF) {/* do nothing */}
		
		// which of the positions is active?
		if (hitClock==ON) {
			int xS=0;
			needed=0;
			for (int p=0;p<8;p++) {
				xS=xS+paramVal[STEP_FREEZER_1_PARAM+p];
				// na és most a currentStep szerint ezen a gombon kell lenni?
				lights[LIGHT_1_LIGHT+p].setBrightness(0);					
				if (currentStep>=xS) {needed=(p==7)?0:p+1;}
			}
			lights[LIGHT_1_LIGHT+needed].setBrightness(1);					
			// hitClock=OFF;
		}
		
		// I know which position to look at, let's send the voltage
		if (inputs[POLY_INPUT].isConnected()) {
			if (paramVal[CHANNEL_SELECTOR_1_PARAM+needed]==0) {
				outputs[MONO_OUTPUT].setVoltage(
					indexOctave - 3 + 
					inputs[POLY_INPUT].getVoltage(randval) );
			}
			else {
				outputs[MONO_OUTPUT].setVoltage(
					indexOctave - 3 + 
					inputs[POLY_INPUT].getVoltage(
						paramVal[CHANNEL_SELECTOR_1_PARAM+needed]-1) );
			}
		} 
		else if (loop % 400 == 25) { // if no input is connected then C-4, C#4, D-4, etc.
			if (indexRange==0 && paramVal[CHANNEL_SELECTOR_1_PARAM+needed]==0) {
				outputs[MONO_OUTPUT].setVoltage(
				indexOctave - 3 + 
				(paramVal[CHANNEL_SELECTOR_1_PARAM+(int)(randval/2)])/12);
			}
			else { // frozen, wait longer
				// v2.1.23 added next section
				switch (indexRange) {
					case 0: 
						outputs[MONO_OUTPUT].setVoltage(
						indexOctave - 3 + 
						(paramVal[CHANNEL_SELECTOR_1_PARAM+needed]-1)/12);
						break;
					case 1: 
						outputs[MONO_OUTPUT].setVoltage(
						indexOctave - 3 + 
						(paramVal[CHANNEL_SELECTOR_1_PARAM+needed]/16)*2-1);
						break;
					case 2: 
						outputs[MONO_OUTPUT].setVoltage(
						indexOctave - 3 + 
						(paramVal[CHANNEL_SELECTOR_1_PARAM+needed]/16)*6-3);
						break;
					case 3: 
						outputs[MONO_OUTPUT].setVoltage(
						indexOctave - 3 + 
						(paramVal[CHANNEL_SELECTOR_1_PARAM+needed]/16)*10-5);
						break;
					case 4: 
						outputs[MONO_OUTPUT].setVoltage(
						indexOctave - 3 + 
						(paramVal[CHANNEL_SELECTOR_1_PARAM+needed]/16)*20-10);
						break;
					default: 
						outputs[MONO_OUTPUT].setVoltage(paramVal[CHANNEL_SELECTOR_1_PARAM+needed]-1);
				}
			}
		}
		
	}

	// this block is to save and reload a variable
	json_t* dataToJson() override {
	json_t* rootJ = json_object();
	json_object_set_new(rootJ, "octave", json_integer(indexOctave));
	json_object_set_new(rootJ, "range", json_integer(indexRange));
	return rootJ;}

	void dataFromJson(json_t* rootJ) override {
	json_t *modeJ = json_object_get(rootJ, "mode");
	if (modeJ) indexOctave = json_integer_value(modeJ);
	json_t *rangeJ = json_object_get(rootJ, "range");
	if (rangeJ) indexRange = json_integer_value(rangeJ);
	}

/* 	void dataFromJson(json_t* rootJ) override {
	json_t *modeJ = json_object_get(rootJ, "octave");
	if (modeJ) indexOctave = json_integer_value(modeJ);}
	json_t *rangeJ = json_object_get(rootJ, "range");
	if (rangeJ) indexRange = json_integer_value(rangeJ);}
 */
 	// #include "RouteSeq/RouteSeq_json.hpp"

};

// --------------------------------------------------

struct RouteSeqWidget : ModuleWidget {

	RouteSeq* module;
	#include "share/widgetwizard.hpp"   // addChild abbreviations

	RouteSeqWidget(RouteSeq* module) {

		this->module = module;
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/RouteSeq.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		
		childKnob(RouteSeq::CHANNEL_SELECTOR_1_PARAM, 0, HP*1, HP*7);
		childKnob(RouteSeq::CHANNEL_SELECTOR_2_PARAM, 0, HP*1, HP*9);
		childKnob(RouteSeq::CHANNEL_SELECTOR_3_PARAM, 0, HP*1, HP*11);
		childKnob(RouteSeq::CHANNEL_SELECTOR_4_PARAM, 0, HP*1, HP*13);
		childKnob(RouteSeq::CHANNEL_SELECTOR_5_PARAM, 0, HP*1, HP*15);
		childKnob(RouteSeq::CHANNEL_SELECTOR_6_PARAM, 0, HP*1, HP*17);
		childKnob(RouteSeq::CHANNEL_SELECTOR_7_PARAM, 0, HP*1, HP*19);
		childKnob(RouteSeq::CHANNEL_SELECTOR_8_PARAM, 0, HP*1, HP*21);

		childKnob(RouteSeq::STEP_FREEZER_1_PARAM, 0, HP*3, HP*7);
		childKnob(RouteSeq::STEP_FREEZER_2_PARAM, 0, HP*3, HP*9);
		childKnob(RouteSeq::STEP_FREEZER_3_PARAM, 0, HP*3, HP*11);
		childKnob(RouteSeq::STEP_FREEZER_4_PARAM, 0, HP*3, HP*13);
		childKnob(RouteSeq::STEP_FREEZER_5_PARAM, 0, HP*3, HP*15);
		childKnob(RouteSeq::STEP_FREEZER_6_PARAM, 0, HP*3, HP*17);
		childKnob(RouteSeq::STEP_FREEZER_7_PARAM, 0, HP*3, HP*19);
		childKnob(RouteSeq::STEP_FREEZER_8_PARAM, 0, HP*3, HP*21);
		
		childLight(RouteSeq::LIGHT_1_LIGHT, 0, HP*3, HP*7);
		childLight(RouteSeq::LIGHT_2_LIGHT, 0, HP*3, HP*9);
		childLight(RouteSeq::LIGHT_3_LIGHT, 0, HP*3, HP*11);
		childLight(RouteSeq::LIGHT_4_LIGHT, 0, HP*3, HP*13);
		childLight(RouteSeq::LIGHT_5_LIGHT, 0, HP*3, HP*15);
		childLight(RouteSeq::LIGHT_6_LIGHT, 0, HP*3, HP*17);
		childLight(RouteSeq::LIGHT_7_LIGHT, 0, HP*3, HP*19);
		childLight(RouteSeq::LIGHT_8_LIGHT, 0, HP*3, HP*21);

		childInput(RouteSeq::RESET_INPUT, HP*1, HP*5);
		childInput(RouteSeq::CLOCK_INPUT, HP*3, HP*5);
		childInput(RouteSeq::POLY_INPUT, HP*1, HP*23);
		childOutput(RouteSeq::MONO_OUTPUT, HP*3, HP*23);

		childOutput(RouteSeq::DEBUG_OUTPUT, HP*222, HP*24.5);
		// childLabel(HP*1,HP*23.5, "DEV", 12);
	
	}

	void appendContextMenu(Menu* menu) override {
		RouteSeq* module = dynamic_cast<RouteSeq*>(this->module);
		assert(module);
		menu->addChild(new MenuSeparator);

		// menu->addChild(createIndexPtrSubmenuItem("Quantize", {"Nope","Octaves","Notes"}, &module->indexQuant));

		// menu->addChild(createIndexPtrSubmenuItem("Get a pattern", {"8 steps","16 steps","24 steps","32 steps","64 steps","96 steps","128 steps"}, &module->indexPattern()));

		menu->addChild(createIndexPtrSubmenuItem("Octave transposition", {"-3","-2","-1","0","1","2","3"}, &module->indexOctave));

		menu->addChild(createIndexPtrSubmenuItem("Range", {"Semitones","-1V to 1V","-3V to 3V","-5V to 5V","-10V to 10V"}, &module->indexRange));

		menu->addChild(createIndexSubmenuItem("Set a pattern",
			// module->txtPatterns,
 			{"8 steps", // 0
			"12 steps", // 1
			"16 steps", // 2
			"24 steps", // 3
			"32 steps", // 4
			"64 steps", // 5
			"96 steps", // 6
			"128 steps"},
			[=]() {return module->getPattern();},
			[=](int ptx) {module->rndPattern(ptx);}
		));
		
        menu->addChild(createMenuItem("Shuffle selectors", "", [=]() {module->shuffleSelectors();}));

	}


	// #include "RouteSeq/RouteSeq_menu.hpp"
	// #include "RouteSeq/RouteSeq_keys.hpp"

};

Model* modelRouteSeq = createModel<RouteSeq, RouteSeqWidget>("RouteSeq");
