// Copyright (c) 2022 Andr�s Szab�
// looks OK, can go to Yolow
// to be tested: record another sequencer (CV or gate)
// too many menu items -> panel to be redesigned

// included in SaveMeMono.cpp
// #include "plugin.hpp"

struct SaveMeMonoWide : Module {

// --------------------------------------------------

	enum ParamId    {
		STEPS_PARAM, RECORD_PARAM, LEFT_PARAM, RIGHT_PARAM, PW_PARAM, LFOMODE_PARAM, RECMODE_PARAM, PARAMS_LEN};

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

	SaveMeMonoWide() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(LEFT_PARAM, 	0.0f, 1.0f, 0.0f, "Shift left");
		configParam(RIGHT_PARAM, 	0.0f, 1.0f, 0.0f, "Shift right");
		configParam(PW_PARAM,   	0.0f, 1.0f, 0.0f, "Pulse width");
		configParam(LFOMODE_PARAM, 	0.0f, 2.0f, 1.0f, "Internal LFO modes");
		configParam(RECMODE_PARAM,  	0.0f, 2.0f, 0.0f, "Record mode");
		paramQuantities[LEFT_PARAM]->snapEnabled = true;
		paramQuantities[RIGHT_PARAM]->snapEnabled = true;
		paramQuantities[PW_PARAM]->snapEnabled = true;
		paramQuantities[LFOMODE_PARAM]->snapEnabled = true;
		paramQuantities[RECMODE_PARAM]->snapEnabled = true;
		paramQuantities[LEFT_PARAM]->randomizeEnabled = false;
		paramQuantities[RIGHT_PARAM]->randomizeEnabled = false;
		paramQuantities[PW_PARAM]->randomizeEnabled = false;
		paramQuantities[LFOMODE_PARAM]->randomizeEnabled = false;
		paramQuantities[RECMODE_PARAM]->randomizeEnabled = false;
		
		configParam(STEPS_PARAM, 	1.0f, 256.0f, 16.0f, "Number of steps");
		configParam(RECORD_PARAM, 	0.0f, 1.0f, 0.0f, "Start/stop recording");
		paramQuantities[STEPS_PARAM]->snapEnabled = true;
		paramQuantities[RECORD_PARAM]->snapEnabled = true;
		paramQuantities[STEPS_PARAM]->randomizeEnabled = false;
		paramQuantities[RECORD_PARAM]->randomizeEnabled = false;

		configInput(MONO_LFO_INPUT, "External LFO"); 
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
	
	// more variables
	float theSeq[256]={0};	// this contains the sequence, 256 slots available
	int allSteps=16;		// set the number of active steps
	int currPos=0;
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

	#include "../SaveMeMono/SaveMe_routines.hpp"

	// Ctrl-E bypassing: no sound but the counting stays
	void processBypass(const ProcessArgs& args) override {
	}
		
	void process(const ProcessArgs& args) override {

		if (loop--<=0) {
			// save some more CPU
			loop=8125;

			// collect some important variables
			allSteps=params[STEPS_PARAM].getValue();
			hitRecord=params[RECORD_PARAM].getValue();
			lights[LED_RECORD_PARAM].setBrightness(hitRecord);
			indexPW=params[PW_PARAM].getValue();
			indexLFO=params[LFOMODE_PARAM].getValue();
			indexRec=params[RECMODE_PARAM].getValue();
			if (params[LEFT_PARAM].getValue()==1) {indexShift=0;}
			if (params[RIGHT_PARAM].getValue()==1) {indexShift=1;}
			
			// collect some more variables
			clockIn=inputs[CLOCK_INPUT].isConnected();
			lfoIn=inputs[MONO_LFO_INPUT].isConnected();
			if (indexShift!=-1) {shiftSeq();}	// ugly stuff
			lights[FIRST_HIT_LIGHT].setBrightness(theSeq[0]);
			
			// this section is only about updating the descriptions
			std::string sx="";
			if (indexPW==0) {
				paramQuantities[PW_PARAM]->description = ("Pulse width is set to full");}
			else if (indexPW==1) {
				paramQuantities[PW_PARAM]->description = ("Pulse width is set according to the clock input");}
			if (indexRec==0) {sx+="Single cycle";
				paramQuantities[RECMODE_PARAM]->description = ("Single cycle is recorded");}
			else if (indexRec==1) {sx+="Single step";
				paramQuantities[RECMODE_PARAM]->description = ("Single step is recorded");}
			else if (indexRec==2) {sx+="Non-stop";
				paramQuantities[RECMODE_PARAM]->description = ("Non-stop recording");}
			if (indexLFO==0) {sx+=(lfoIn)?" from LFO":" & Gates";
				paramQuantities[LFOMODE_PARAM]->description = ("Gates are provided if no LFO is connected");}
			else if (indexLFO==1) {sx+=(lfoIn)?" from LFO":" & -5V to +5V";
				paramQuantities[LFOMODE_PARAM]->description = ("-5V to 5V range if no LFO is nonnected");}
			else if (indexLFO==2) {sx+=(lfoIn)?" from LFO":" & 0V to 10V";
				paramQuantities[LFOMODE_PARAM]->description = ("0V to 10V range if no LFO is connected");}
			paramQuantities[RECORD_PARAM]->description = sx;

			// this is just a small treat, feel free to delete it
			// sx="";
			// if (indexLFO==0) {
				// for (int i=0;i<std::min(16,allSteps);i++) {
					// sx=sx + (theSeq[i]==1)?"X":".";
					// }
			// }
			// paramQuantities[STEPS_PARAM]->description = sx;
			
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
				if (lfoIn) {pickVolt=inputs[MONO_LFO_INPUT].getVoltage();}			// external LFO
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

    void onRandomize() override {		
		randomizeFields();
		Module::onRandomize();		
	}

	#include "../SaveMeMono/SaveMe_json.hpp"

};

// --------------------------------------------------

// as seen in VCV Fundamental Logic

struct VCVBezelBig : app::SvgSwitch {
	VCVBezelBig() {momentary = false;
	addFrame(Svg::load(asset::plugin(pluginInstance, "res/components/VCVBezelBig.svg")));
	}
};

template <typename TBase>
struct VCVBezelLightBig : TBase {
	VCVBezelLightBig() {
		this->borderColor = color::BLACK_TRANSPARENT;
		this->bgColor = color::BLACK_TRANSPARENT;
		this->box.size = mm2px(math::Vec(11.1936, 11.1936));
	}
};

/*

template <typename TBase, typename TLight = WhiteLight>
struct LightButton : TBase {
	app::ModuleLightWidget* light;
	LightButton() {light = new TLight;
		light->box.pos = this->box.size.div(2).minus(light->box.size.div(2));
		this->addChild(light);}
	app::ModuleLightWidget* getLight() {return light;}
};

*/

using VCVBezelLightBigRed = LightButton<VCVBezelBig, VCVBezelLightBig<RedLight>>;


// --------------------------------------------------

struct SaveMeMonoWideWidget : ModuleWidget {

	SaveMeMonoWide* module;
	#include "../share/widgetwizard.hpp"   // addChild abbreviations

	SaveMeMonoWideWidget(SaveMeMonoWide* module) {

		this->module = module;
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/SaveMeMonoWide.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		childSwitch(SaveMeMonoWide::PW_PARAM, 0, 			HP*4.5, HP*2.5);
		childInput(SaveMeMonoWide::CLOCK_INPUT, 			HP*1.5, HP*5.25);
		childInput(SaveMeMonoWide::RESET_INPUT, 			HP*4.5, HP*5.25);
		childKnob(SaveMeMonoWide::STEPS_PARAM, 1, 			HP*3, HP*8);
		childButtonmom(SaveMeMonoWide::LEFT_PARAM, 2, 		HP*1.5, HP*10.5);
		childButtonmom(SaveMeMonoWide::RIGHT_PARAM, 2,  	HP*4.5, HP*10.5);

		childLight(SaveMeMonoWide::FIRST_HIT_LIGHT, 14, HP*3, HP*10.5);

		childOutput(SaveMeMonoWide::MONO_REPLAY_OUTPUT, 	HP*1, HP*13);
		childOutput(SaveMeMonoWide::REVERSE_REPLAY_OUTPUT, 	HP*3, HP*13);
		childOutput(SaveMeMonoWide::RANDOM_REPLAY_OUTPUT, 	HP*5, HP*13);

// as seen in VCV Fundamental Logic
addParam(createLightParamCentered<VCVBezelLightBigRed>(mm2px(Vec(HP*3, HP*17)), module, SaveMeMonoWide::RECORD_PARAM, SaveMeMonoWide::LED_RECORD_PARAM));
// addParam(createLightParamCentered<VCVButtonSquareRed>(mm2px(Vec(HP*3, HP*17)), module, SaveMeMonoWide::RECORD_PARAM, SaveMeMonoWide::LED_RECORD_PARAM));

		// addParam(createLightParamCentered<VCVLightLatch<LargeSimpleLight<RedLight>>>(mm2px(Vec(HP*3, HP*17)), module, SaveMeMonoWide::RECORD_PARAM, SaveMeMonoWide::LED_RECORD_PARAM));

		childInput(SaveMeMonoWide::RECORD_INPUT, 			HP*4.5, HP*19.5);
		childInput(SaveMeMonoWide::MONO_LFO_INPUT, 			HP*1.5, HP*19.5);
		childSwitch(SaveMeMonoWide::RECMODE_PARAM, 1, 		HP*4.5, HP*23);	
		childSwitch(SaveMeMonoWide::LFOMODE_PARAM, 1, 		HP*1.5, HP*23);
		
		// childLabel(HP*1, HP*12, "TEST", 8);
	}

	void appendContextMenu(Menu* menu) override {
		SaveMeMonoWide* module = dynamic_cast<SaveMeMonoWide*>(this->module);
		assert(module);
		menu->addChild(new MenuSeparator);
		menu->addChild(createMenuItem("Randomize all CVs", "", [=]() {module->randomizeFields();}));	

		menu->addChild(createSubmenuItem("Text export/import", "",
			[=](Menu* menu) {
				menu->addChild(createMenuLabel("(OneZero format: 10101101)"));
				menu->addChild(createMenuItem("Export to OneZero text file", "", [=]() {module->exportFieldsToTxt(0);}));	
				menu->addChild(createMenuItem("Import from OneZero text file", "", [=]() {module->importFieldsFromTxt(0);}));
				menu->addChild(createMenuLabel("(OnePoint format: 1.16, 2.25, 4.22)"));
				menu->addChild(createMenuItem("Export to OnePoint text file", "", [=]() {module->exportFieldsToTxt(1);}));	
				menu->addChild(createMenuItem("Import from OnePoint text file", "", [=]() {module->importFieldsFromTxt(1);}));
			}
		));

		// menu->addChild(createMenuLabel("(OneZero format: 10101101)"));
		// menu->addChild(createMenuItem("Export to OneZero text file", "", [=]() {module->exportFieldsToTxt(0);}));	
		// menu->addChild(createMenuItem("Import from OneZero text file", "", [=]() {module->importFieldsFromTxt(0);}));

		// menu->addChild(createMenuLabel("(OnePoint format: 1.16, 2.25, 4.22)"));
		// menu->addChild(createMenuItem("Export to OnePoint text file", "", [=]() {module->exportFieldsToTxt(1);}));	
		// menu->addChild(createMenuItem("Import from OnePoint text file", "", [=]() {module->importFieldsFromTxt(1);}));

		// menu->addChild(new MenuSeparator);
		// menu->addChild(createIndexPtrSubmenuItem("Pulse width", {"Full width","Only clock wide"}, &module->indexPW));
		// menu->addChild(createIndexPtrSubmenuItem("Internal LFO", {"Gates","-5V to 5V","0V to 10V","-1V to 1V","0V to 1V"}, &module->indexLFO));
		// menu->addChild(createIndexPtrSubmenuItem("Record mode", {"One cycle","Momentary","Always"}, &module->indexRec));
		// menu->addChild(createIndexPtrSubmenuItem("Shift sequence", {"Earlier","Later"}, &module->indexShift));	// ugly stuff
		// menu->addChild(new MenuSeparator);
	}
	
};

Model* modelSaveMeMonoWide = createModel<SaveMeMonoWide, SaveMeMonoWideWidget>("SaveMeMonoWide");
