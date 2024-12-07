// Copyright (c) 2022 András Szabó
#include "plugin.hpp"

#define HP 5.08

const int ROWS=16;
const int COLS=2;
const int CELL_WIDTH=HP*3;
const int CELL_HEIGHT=HP*2.75;
const int CELL_PADDING=HP*0.2;

/* 
#define ROWS 16
#define COLS 2

#define CELL_WIDTH HP*3
#define CELL_HEIGHT HP*2.75
#define CELL_PADDING HP*0.2
 */
struct PokeMe : Module {

    int grid_data[COLS*ROWS];
	// int CURR_COL[2]={0,0};
	// int CURR_DIV[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	int CURR_MODE=1;
	int CURR_STEP=-1;
	const int SEQLEN=ROWS*COLS;
	int CONT_LAPS[3]={0};
	int CONT_SAMP=0;


// --------------------------------------------------

	enum ParamId  {
		DEBUG_PARAM, CHOKE_PARAM, PARAMS_LEN};

	enum InputId  {
		CLOCK_INPUT, RESET_INPUT, DEBUG_INPUT, INPUTS_LEN};

	enum OutputId {
		SEQ_OUTPUT, DEBUG_OUTPUT, OUTPUTS_LEN};

	enum LightId  {CHOKE_LED, MOTHER_LED, LOOP_LED, LIGHTS_LEN};

// --------------------------------------------------

	PokeMe() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(DEBUG_PARAM, 	-10.0f, 10.0f, -5.0f, "Debug");
		configParam(CHOKE_PARAM, 	0.0f, 1.0f, 0.0f, "Choke");
		configInput(DEBUG_INPUT, "Debug"); 
		configInput(CLOCK_INPUT, "Clock"); 
		configInput(RESET_INPUT, "Reset"); 
		configOutput(DEBUG_OUTPUT, "Debug"); 
		configOutput(SEQ_OUTPUT, "Sequence"); 
	}

// --------------------------------------------------

	float oldClock=0;
	float newClock=0;
	float oldReset=0;
	float newReset=0;
	const int ON=1; 
	const int OFF=0;
	const int NA=-1;
	int hitClock=NA;
	int hitReset=NA;
	
	int indexMumSync=1;
	int indexPRG=1;
	int indexPW=0;
	bool indexLoop=true;
	// int indexPrec=0;
	// int indexPrecMode=0;
	bool indexChoke=false;
	bool indexExt=false;
	int indexOut=0;
	float newVolt=0;
	
	float defProb1=1.0f;
	// float defProb2=0.95f;
	// float defProb3=0.80f;
	// float defProb4=0.50f;	
	int defProb2=9;	// 90%
	int defProb3=8;	// 80%
	int defProb4=5;	// 50%
	int defProb7=2;
	int defProb8=3;
	int defProb9=4;

	
	PokeMe* findHostModulePtr(Module* module) {
		if (module) {
			if (module->leftExpander.module) {
				// if it's the mother module, we're done
				if (module->leftExpander.module->model == modelPokeMe) {
					return reinterpret_cast<PokeMe*>(module->leftExpander.module);
				}
			}
		}
		return nullptr;
	}

	void onReset(const ResetEvent& e) override {
		Module::onReset(e);
		
		indexChoke=false;
		indexExt=false;
		indexOut=0;
		
		defProb2=9;	// 90%
		defProb3=8;	// 80%
		defProb4=5;	// 50%
		defProb7=2;
		defProb8=3;
		defProb9=4;
	}

	void process(const ProcessArgs& args) override {
		
		// outputs[DEBUG_OUTPUT].channels=16;
		if (indexOut==1 || indexExt) {outputs[SEQ_OUTPUT].channels=12;}
		else outputs[SEQ_OUTPUT].channels=1;

		#include "PokeMe/PokeMe_programmer.hpp"

		// let's see if we have mother here
		PokeMe const* mother = findHostModulePtr(this);
		
		// if there is a PokeMe on the left then grab the clock and reset info
		if (mother && inputs[CLOCK_INPUT].isConnected()==false) {hitClock=mother->hitClock;}
		else {
			// let's see if the CLOCK has just been ON, OFF or else (NA)
			newClock=inputs[CLOCK_INPUT].getVoltage();
			if (oldClock>2 && newClock<=2) {hitClock=OFF;}
			else if (oldClock<=2 && newClock>2) {hitClock=ON;}
			else {hitClock=NA;}
			oldClock=newClock;
		}

		if (mother && indexMumSync==1 && mother->CURR_STEP==1) {indexMumSync=0;CURR_STEP=0;}
			
		if (mother && inputs[RESET_INPUT].isConnected()==false) {hitReset=mother->hitReset;}
		else {
			// let's see if the RESET has just been ON, OFF or else (NA)
			newReset=inputs[RESET_INPUT].getVoltage();
			if (oldReset>2 && newReset<=2) {hitReset=OFF;}
			else if (oldReset<=2 && newReset>2) {hitReset=ON;}
			else if (hitReset==ON) {hitReset=NA;}
			oldReset=newReset;
		}
	
		// depending the RESET and CLOCK I calculate the step
		// and send some voltage to the output
		if (hitClock==ON && !(hitReset==ON)) {
			// new clock in but no reset
			CURR_STEP++;
			if (CURR_STEP>=SEQLEN) {CURR_STEP=(indexLoop)?0:100;}
			if (grid_data[CURR_STEP]==10) {CURR_STEP=(indexLoop)?0:100;}
			// if (CURR_STEP>99 && mother && mother->indexLoop && mother->CURR_STEP>99) {CURR_STEP=0;}
			if (CURR_STEP==0) {					
				// CONT_LAPS[0]++; if (CONT_LAPS[0]>=2) {CONT_LAPS[0]=0;}
				// CONT_LAPS[1]++; if (CONT_LAPS[1]>=3) {CONT_LAPS[1]=0;}
				// CONT_LAPS[2]++; if (CONT_LAPS[2]>=4) {CONT_LAPS[2]=0;}
				CONT_LAPS[0]++; if (CONT_LAPS[0]>=abs(defProb7)) {CONT_LAPS[0]=0;}
				CONT_LAPS[1]++; if (CONT_LAPS[1]>=abs(defProb8)) {CONT_LAPS[1]=0;}
				CONT_LAPS[2]++; if (CONT_LAPS[2]>=abs(defProb9)) {CONT_LAPS[2]=0;}
				// CONT_LAPS[0]++; if (defProb7>0) {if (CONT_LAPS[0]>=defProb7) {CONT_LAPS[0]=0;}}
				// CONT_LAPS[1]++; if (defProb8>0) {if (CONT_LAPS[1]>=defProb8) {CONT_LAPS[1]=0;}}
				// CONT_LAPS[2]++; if (defProb9>0) {if (CONT_LAPS[2]>=defProb9) {CONT_LAPS[2]=0;}}
			}
			switch (grid_data[CURR_STEP]) {
				case 0: newVolt=0; break;	
				case 1: newVolt=10; break;	
				case 2: newVolt=(rack::random::uniform()*10<defProb2)?10:0.2; break;	
				case 3: newVolt=(rack::random::uniform()*10<defProb3)?10:0.3; break;	
				case 4: newVolt=(rack::random::uniform()*10<defProb4)?10:0.4; break;	
				// case 4: float rndF=rack::random::uniform();newVolt=(rndF<(defProb4/10))?10:rndF*10; break;	
				case 5: newVolt=(0.60>rack::random::uniform())?10:0.5; break;
				case 6: newVolt=3+(rand() % 7); CONT_SAMP=25; break; 	// lame but it provides some random voltage above 2V
				case 7: if (defProb7>0) {newVolt=(CONT_LAPS[0]==0)?10:0.7;}
					else {newVolt=(CONT_LAPS[0]==0)?0.7:10;} break;	
				case 8: if (defProb8>0) {newVolt=(CONT_LAPS[1]==0)?10:0.8;}
					else {newVolt=(CONT_LAPS[0]==0)?0.8:10;} break;	
				case 9: if (defProb9>0) {newVolt=(CONT_LAPS[2]==0)?10:0.9;}
					else {newVolt=(CONT_LAPS[0]==0)?0.9:10;} break;	
				// case 7: newVolt=(CONT_LAPS[0]==0)?10:0.7; break;	
				// case 8: newVolt=(CONT_LAPS[1]==0)?10:0.8; break;	
				// case 9: newVolt=(CONT_LAPS[2]==0)?10:0.9; break;	
				default: newVolt=0.0404; 	// possibly never gets here
			}
			
			if (indexChoke && mother && mother->newVolt>2) {newVolt=0;} // choked
			if (indexOut==0) {
				outputs[SEQ_OUTPUT].setVoltage(newVolt,0);
			}
			else if (indexOut==2) {
				outputs[SEQ_OUTPUT].setVoltage(grid_data[CURR_STEP],0);
			}
			else if (indexOut==1 || indexExt) {
				for (int i=1;i<10;i++) {
					outputs[SEQ_OUTPUT].setVoltage((grid_data[CURR_STEP]==i)?10:0,i);
				}
// itt van egy kis gond
// hogy lehet, hogy grid_data[CURR_STEP]>999999?!?!?!?
if (grid_data[CURR_STEP]>10) {grid_data[CURR_STEP]=0;}
				outputs[SEQ_OUTPUT].setVoltage(grid_data[CURR_STEP],10);
				outputs[SEQ_OUTPUT].setVoltage(CURR_STEP,11);
			} 
			// outputs[DEBUG_OUTPUT].setVoltage(CURR_STEP,1);
			// outputs[DEBUG_OUTPUT].setVoltage(grid_data[CURR_STEP],2);
			// outputs[DEBUG_OUTPUT].setVoltage(newVolt,3);
		}
		else if (hitReset==ON && hitClock==ON) {
			// new reset and new clock
			CURR_STEP=0;
			CONT_LAPS[0]=0;
			CONT_LAPS[1]=0;
			CONT_LAPS[2]=0;
			outputs[SEQ_OUTPUT].setVoltage((grid_data[CURR_STEP]>0)?10:0);
			if (indexOut==1 || indexExt) {
				for (int i=1;i<10;i++) {
					outputs[SEQ_OUTPUT].setVoltage((grid_data[CURR_STEP]==i)?10:0,i);
				}
				outputs[SEQ_OUTPUT].setVoltage(grid_data[CURR_STEP],10);
				outputs[SEQ_OUTPUT].setVoltage(CURR_STEP,11);
			} 
		}
		else if (hitReset==ON && !(hitClock==ON)) {
			// new reset and no new clock
			CURR_STEP=-1;
			CONT_LAPS[0]=0;
			CONT_LAPS[1]=0;
			CONT_LAPS[2]=0;
			outputs[SEQ_OUTPUT].setVoltage(0,0);
			if (indexOut==1 || indexExt) {
				for (int i=1;i<10;i++) {outputs[SEQ_OUTPUT].setVoltage(0,i);}
			}
		}
		else if (!(indexOut==2) && grid_data[CURR_STEP]==5 && hitClock==OFF) {
			if (0.50>rack::random::uniform()) {
				outputs[SEQ_OUTPUT].setVoltage(0,0);
			}			
		}
		else if (indexPW==0 && hitClock==OFF) {
			outputs[SEQ_OUTPUT].setVoltage(0,0);
			if (indexOut==1 || indexExt) {
				for (int i=1;i<10;i++) {outputs[SEQ_OUTPUT].setVoltage(0,i);}
			}
		}
		else {}

		// this is the slicer function for the trigger mode 6
		if (!(indexOut==2) && grid_data[CURR_STEP]==6) { 
			CONT_SAMP--;
			if (hitClock==OFF) {
				CONT_SAMP=25;
				outputs[SEQ_OUTPUT].setVoltage((mother && mother->newVolt>2)?0:10,0);
			}
			else if (hitClock==NA && CONT_SAMP<=0) {
				outputs[SEQ_OUTPUT].setVoltage(0,0);
			}
		}
		
		// outputs[DEBUG_OUTPUT].setVoltage(CURR_MODE,0);
		
		// just to show the BOC for a module operating
		if (hitClock==ON) {
			if (CURR_STEP==0) {lights[MOTHER_LED].setBrightness(1);}
			else {lights[MOTHER_LED].setBrightness(0);}
		}

		// just to show if the module is choked by the left neighbor
		lights[CHOKE_LED].setBrightness((indexChoke)?1:0);	// choked: 1 red, not-chocked: 0 black
		lights[LOOP_LED].setBrightness((indexLoop)?0:1);	// looped: 0 black, not-looped: 1 yellow

	}

	// Ctrl-I
    void onReset() override {
		grid_data[0] = 1;
		for (int i=1;i<SEQLEN;i++) {grid_data[i] = 0;}
		grid_data[16] = 10;
		Module::onReset();		
	}

	// Ctrl-R
	int indexRandom=1;
    void onRandomize() override {		
		for (int i=0;i<SEQLEN;i++) {
			if (grid_data[i]==10) break;
			// alternative Alt-R random
			if (indexRandom>1 && (rand() % 2)) {grid_data[i] = rand() % 10;}
			// regular random Ctrl-R
			else {grid_data[i] = (rand() % 2)?CURR_MODE:0;} 
		}
		indexRandom=1;
		Module::onRandomize();		
	}

	#include "PokeMe/PokeMe_json.hpp"

};

// --------------------------------------------------

#include "PokeMe/PokeMe_gridwidget.hpp"

struct PokeMeWidget : ModuleWidget {

	PokeMe* module;
	// #include "share/widgetwizard.hpp"   // addChild abbreviations

	PokeMeWidget(PokeMe* module) {

		this->module = module;
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/PokeMe.svg")));

		// addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addInput(createInputCentered<PJ301MPort>
			(mm2px(Vec(HP*1.5, HP*2)), module, PokeMe::RESET_INPUT));

		addInput(createInputCentered<PJ301MPort>
			(mm2px(Vec(HP*1.5, HP*4)), module, PokeMe::CLOCK_INPUT));
		
		addOutput(createOutputCentered<PJ301MPort>
			(mm2px(Vec(HP*1.5, HP*6)), module, PokeMe::SEQ_OUTPUT));
		
		GridWidgetPokeMe *grid_widget = new GridWidgetPokeMe();
		grid_widget->box.pos = mm2px(Vec(HP*0.4,HP*7.5));
		grid_widget->module = module;
		addChild(grid_widget);
/* 
		addChild(createLightCentered<SmallSimpleLight<YellowLight>>
			(mm2px(Vec(HP*2.25, HP*1.25)), module, PokeMe::LOOP_LED));

		addChild(createLightCentered<SmallSimpleLight<RedLight>>
			(mm2px(Vec(HP*2.25, HP*3)), module, PokeMe::CHOKE_LED));

		addChild(createLightCentered<SmallSimpleLight<GreenLight>>
			(mm2px(Vec(HP*0.75, HP*3)), module, PokeMe::MOTHER_LED));
 */
		addChild(createLightCentered<SmallSimpleLight<RedLight>>
			(mm2px(Vec(HP*0.75, HP*23.25)), module, PokeMe::CHOKE_LED));

		addChild(createLightCentered<SmallSimpleLight<GreenLight>>
			(mm2px(Vec(HP*1.5, HP*23.25)), module, PokeMe::MOTHER_LED));

		addChild(createLightCentered<SmallSimpleLight<YellowLight>>
			(mm2px(Vec(HP*2.25, HP*23.25)), module, PokeMe::LOOP_LED));

		// addOutput(createOutputCentered<PJ301MPort>
			// (mm2px(Vec(HP*1.5, HP*24)), module, PokeMe::DEBUG_OUTPUT));
		
	};

	void appendContextMenu(Menu* menu) override {
		PokeMe* module = dynamic_cast<PokeMe*>(this->module);
		assert(module);	
		#include "PokeMe/PokeMe_menu.hpp"
	}
	
	#include "PokeMe/PokeMe_keys.hpp"

};

Model* modelPokeMe = createModel<PokeMe, PokeMeWidget>("PokeMe");
