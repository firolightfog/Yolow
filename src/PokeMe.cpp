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
	
	int indexPW=0;
	bool indexLoop=true;
	// int indexPrec=0;
	// int indexPrecMode=0;
	bool indexChoke=false;
	bool indexExt=false;
	float newVolt=0;
	
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

	void process(const ProcessArgs& args) override {
		
		// outputs[DEBUG_OUTPUT].channels=16;
		if (indexExt) {outputs[SEQ_OUTPUT].channels=12;}
		else outputs[SEQ_OUTPUT].channels=1;

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
				CONT_LAPS[0]++; if (CONT_LAPS[0]>=2) {CONT_LAPS[0]=0;}
				CONT_LAPS[1]++; if (CONT_LAPS[1]>=3) {CONT_LAPS[1]=0;}
				CONT_LAPS[2]++; if (CONT_LAPS[2]>=4) {CONT_LAPS[2]=0;}
			}
			switch (grid_data[CURR_STEP]) {
				case 0: newVolt=0; break;	
				case 1: newVolt=10; break;	
				case 2: newVolt=(0.95>rack::random::uniform())?10:0.2; break;	
				case 3: newVolt=(0.80>rack::random::uniform())?10:0.3; break;	
				case 4: newVolt=(0.50>rack::random::uniform())?10:0.4; break;	
				case 5: newVolt=(0.60>rack::random::uniform())?10:0.5; break;
				case 6: newVolt=3+(rand() % 7); CONT_SAMP=25; break; 			// lame but it provides some random voltage above 2V
				case 7: newVolt=(CONT_LAPS[0]==0)?10:0.7; break;	
				case 8: newVolt=(CONT_LAPS[1]==0)?10:0.8; break;	
				case 9: newVolt=(CONT_LAPS[2]==0)?10:0.9; break;	
				default: newVolt=0.25; 	// possibly never gets here
			}
			
			if (indexChoke && mother && mother->newVolt>2) {newVolt=0;} // choked
			outputs[SEQ_OUTPUT].setVoltage(newVolt,0);
			if (indexExt) {
				for (int i=1;i<10;i++) {
					outputs[SEQ_OUTPUT].setVoltage((grid_data[CURR_STEP]==i)?10:0,i);
				}
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
			if (indexExt) {
				for (int i=1;i<10;i++) {
					outputs[SEQ_OUTPUT].setVoltage((grid_data[CURR_STEP]==i)?10:0,i);
				}
				outputs[SEQ_OUTPUT].setVoltage(grid_data[CURR_STEP],10);
				outputs[SEQ_OUTPUT].setVoltage(CURR_STEP,11);
			} 
		}
		else if (hitReset==ON && !(hitClock==ON)) {
			// new reset and no new clock
			CURR_STEP=0;
			CONT_LAPS[0]=0;
			CONT_LAPS[1]=0;
			CONT_LAPS[2]=0;
			outputs[SEQ_OUTPUT].setVoltage(0,0);
			if (indexExt) {
				for (int i=1;i<10;i++) {outputs[SEQ_OUTPUT].setVoltage(0,i);}
			}
		}
		else if (grid_data[CURR_STEP]==5 && hitClock==OFF) {
			if (0.50>rack::random::uniform()) {
				outputs[SEQ_OUTPUT].setVoltage(0,0);
			}			
		}
		else if (indexPW==0 && hitClock==OFF) {
			outputs[SEQ_OUTPUT].setVoltage(0,0);
			if (indexExt) {
				for (int i=1;i<10;i++) {outputs[SEQ_OUTPUT].setVoltage(0,i);}
			}
		}
		else {}

		// this is the slicer function for the trigger mode 6
		if (grid_data[CURR_STEP]==6) { 
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

    void onReset() override {
		grid_data[0] = 1;
		for (int i=1;i<SEQLEN;i++) {grid_data[i] = 0;}
		Module::onReset();		
	}

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
