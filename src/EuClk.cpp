// Copyright (c) 2022 András Szabó
#include "plugin.hpp"

struct Euclk : Module {

	// --------------------------------------------------

	enum ParamId	{LENGTH_PARAM, HITS_PARAM, SHIFTS_PARAM, PREC_PARAM, PW_PARAM, PARAMS_LEN};
	enum InputId	{RESET_INPUT, CLOCK_INPUT, INPUTS_LEN};
	enum OutputId	{OUTPUT_OUTPUT, INVERT_OUTPUT, OUTPUTS_LEN};
	enum LightId 	{LIGHTS_LEN};

	// --------------------------------------------------

	Euclk() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(LENGTH_PARAM, 	1.0f, 64.0f, 16.0f, "Length");
		configParam(HITS_PARAM, 	1.0f, 64.0f, 4.0f, "Hits");
		configParam(SHIFTS_PARAM, 	0.0f, 63.0f, 0.0f, "Shifts");
		configParam(PREC_PARAM, 	0.0f, 1.0f, 0.95f, "Precision");
		paramQuantities[LENGTH_PARAM]->snapEnabled = true;
		paramQuantities[HITS_PARAM]->snapEnabled = true;
		paramQuantities[SHIFTS_PARAM]->snapEnabled = true;

		configInput(RESET_INPUT, "Reset"); 
		configInput(CLOCK_INPUT, "Clock"); 

		configOutput(OUTPUT_OUTPUT, "Output"); 
		configOutput(INVERT_OUTPUT, "Inverted"); 
	}

	// --------------------------------------------------

	int storedRhythm[64]={0};

	// https://www.computermusicdesign.com/simplest-euclidean-rhythm-algorithm-explained/
	void euclid(int steps, int pulses, int rotation){
		// int storedRhythm[64] = {0} ; //empty array which stores the rhythm.
		//the length of the array is equal to the number of steps
		//a value of 1 for each array element indicates a pulse

		int bucket = 0; //out variable to add pulses together for each step

		//fill array with rhythm
		for ( int i=0 ; i < steps ; i++){ 
			bucket += pulses; 
				if(bucket >= steps) {
				bucket -= steps;
				storedRhythm[i]=1; //'1' indicates a pulse on this beat
				// storedRhythm.push(1); 
			} else {
				storedRhythm[i]=0; //'0' indicates no pulse on this beat
				// storedRhythm.push(0); 
			}
		}
	}

	// --------------------------------------------------

	int loop=0;     // save some CPU in process()
	int LEN=16;
	int HIT=4;
	int SHF=0;
	float PRC=0.95f;
	
	int stateClk=0;
	int stateRst=0;
	int newClk=0;
	int oldClk=0;
	int newRst=0;
	int oldRst=0;
	
	int activeStep=0;
	int shiftedStep=0;
	int activePrec=0.5;
	float newVolt=0;
	
	void process(const ProcessArgs& args) override {

		if (loop--<=0) {
			loop=9001;
						
			LEN=params[LENGTH_PARAM].getValue();
			HIT=params[HITS_PARAM].getValue();
			SHF=params[SHIFTS_PARAM].getValue();
			PRC=params[PREC_PARAM].getValue();

			euclid(LEN,HIT,SHF);	// create the pattern
			// storedRhythm[64]=euclid2(LEN,HIT,SHF);
			
			std::string sx="...";
			if (HIT>=LEN) {sx+="Knob sets 100% density";}
			else {sx="Knob sets ca. " + std::to_string((int)(100*HIT/LEN)) + "% density";}
			paramQuantities[HITS_PARAM]->description = (sx);
						
		}

		// check reset signal and jump to the beginning if needed
		stateRst=0;
		newRst=inputs[RESET_INPUT].getVoltage();
		if (newRst>2 && !(oldRst>2)) {stateRst=1; activeStep=0;}
		// else if (!(newRst>2) && (oldRst>2)) {stateRst=-1;}
		
		// check clock signal 
		stateClk=0;
		newClk=inputs[CLOCK_INPUT].getVoltage();
		if (!(newClk>2)) {
			// newVolt=0;
			outputs[OUTPUT_OUTPUT].setVoltage(0);
			outputs[INVERT_OUTPUT].setVoltage(0);						
			if (oldClk>2) {stateClk=-1;}
		}
		else if (newClk>2 && !(oldClk>2)) {	
			stateClk=1;
			// https://electro-music.com/forum/topic-62215.html
			// https://electro-music.com/forum/post-406480.html#406480
			// a great Euclidean step calculator
			newVolt=((((activeStep + SHF) * HIT) % LEN) < HIT)?1.0f:0.0f;					

			activePrec=PRC>rack::random::uniform();
			if (!activePrec) {newVolt=newVolt*-1+1;}
			
			outputs[OUTPUT_OUTPUT].setVoltage(newVolt*10);
			outputs[INVERT_OUTPUT].setVoltage(-1*(newVolt*10-10));

			activeStep++;
			if (activeStep>=LEN) {activeStep=0;};
		}
		oldClk=newClk;
		oldRst=newRst;
		
	}

	// --------------------------------------------------

};

// --------------------------------------------------

struct EuclkWidget : ModuleWidget {

	Euclk* module;
	#include "share/widgetwizard.hpp"   // addChild abbreviations

	EuclkWidget(Euclk* module) {

		this->module = module;
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/Euclk.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		
		childKnob(Euclk::LENGTH_PARAM, 	1, HP*2, HP*12.5);
		childKnob(Euclk::HITS_PARAM, 	1, HP*2, HP*15.5);
		childKnob(Euclk::SHIFTS_PARAM, 	1, HP*2, HP*18.5);
		childKnob(Euclk::PREC_PARAM, 	1, HP*2, HP*21.5);
		
		childOutput(Euclk::OUTPUT_OUTPUT, HP*3, HP*5.5);
		childOutput(Euclk::INVERT_OUTPUT, HP*1, HP*5.5);
		childInput(Euclk::RESET_INPUT, HP*1, HP*8.5);
		childInput(Euclk::CLOCK_INPUT, HP*3, HP*8.5);

	}

};

Model* modelEuclk = createModel<Euclk, EuclkWidget>("Euclk");

/**************************************************/
// EXPANDER CODE - starting here
/**************************************************/

// it is meant to  add 3 more Euclidean rythms (OUT & INV) 
// and random gates (RND, INVRND)
// OR / XOR output of the mother and current
// using the same clock as the mother

// #include "plugin.hpp"
struct EuclkMore : Module {

	enum ParamId	{LENGTH_PARAM, HITS_PARAM, SHIFTS_PARAM, PREC_PARAM, PW_PARAM, MIXED_PARAM, PARAMS_LEN};
	enum InputId	{LENGTH_INPUT, HITS_INPUT, INPUTS_LEN};
	enum OutputId	{OUTPUT_OUTPUT, MIXED_OUTPUT, OUTPUTS_LEN};
	enum LightId 	{LIGHTS_LEN};

	EuclkMore() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configOutput(OUTPUT_OUTPUT, "Output"); 
		configOutput(MIXED_OUTPUT, "Mixed"); 
		configInput(LENGTH_INPUT, "Length CV"); 
		configInput(HITS_INPUT, "Hits CV"); 

		configParam(LENGTH_PARAM, 	1.0f, 64.0f, 16.0f, "Length");
		configParam(HITS_PARAM, 	1.0f, 64.0f, 4.0f, "Hits");
		configParam(SHIFTS_PARAM, 	0.0f, 63.0f, 0.0f, "Shifts");
		configParam(PREC_PARAM, 	0.0f, 1.0f, 0.95f, "Precision");
		paramQuantities[LENGTH_PARAM]->snapEnabled = true;
		paramQuantities[HITS_PARAM]->snapEnabled = true;
		paramQuantities[SHIFTS_PARAM]->snapEnabled = true;

		configParam(MIXED_PARAM, 	0.0f, 7.0f, 1.0f, "Mixed clock modes");
		paramQuantities[MIXED_PARAM]->snapEnabled = true;
		paramQuantities[MIXED_PARAM]->description = ("Mixed output modes inverted, random, XOR and OR with mother, etc...");
	}

 	Euclk* findHostModulePtr(Module* module) {
		if (module) {
			if (module->leftExpander.module) {
				// if it's the mother module, we're done
				if (module->leftExpander.module->model == modelEuclk) {
					return reinterpret_cast<Euclk*>(module->leftExpander.module);
				}
				// if it's EuclkMore, keep recursing
				else if (module->leftExpander.module->model == modelEuclkMore) {
					return findHostModulePtr(module->leftExpander.module);
				}
			}
		}
		return nullptr;
	}
	
	// --------------------------------------------------

	int loop=0;     // save some CPU in process()
	int MXD=1;
	int LEN=16;
	int HIT=4;
	int SHF=0;
	float PRC=0.95f;
	
	bool newClk=false;
	bool oldClk=false;
	bool newRst=false;
	bool oldRst=false;
	
	int activeStep=0;
	int shiftedStep=0;
	int activePrec=0.5;
	float newVolt=0;
	
	// int storedRhythm[64]={0};

	void process(const ProcessArgs& args) override {		
		
		Euclk const* mother = findHostModulePtr(this);
		
		// no mother; out of order 
		if (!mother) {
			outputs[OUTPUT_OUTPUT].setVoltage(-0.404);
			outputs[MIXED_OUTPUT].setVoltage(-1*MXD);
			}
			
		// mother module found
		else {		

			// save some CPU
			if (loop--<=0) {
				loop=9925;
				
				// collect the main params for easier referencing
				MXD=params[MIXED_PARAM].getValue();				
				LEN=params[LENGTH_PARAM].getValue();
				HIT=params[HITS_PARAM].getValue();
				SHF=params[SHIFTS_PARAM].getValue();
				PRC=params[PREC_PARAM].getValue();

				std::string sx="...";	// variable to construct the knob desc.

				// let's check the LEN CV & knob desc.
				if (inputs[LENGTH_INPUT].isConnected()) {
					LEN=std::max(1,(int)(inputs[LENGTH_INPUT].getVoltage()/10*64));
					sx="CV forces " + std::to_string(LEN) + " pattern length";
				}
				paramQuantities[LENGTH_INPUT]->description = (sx);	

				// let's check the HIT CV & knob desc.
				if (inputs[HITS_INPUT].isConnected()) {
					HIT=(int)(inputs[HITS_INPUT].getVoltage()/10*64);
					if (HIT>=LEN) {sx="Knob sets 100% density";}
					else {sx="CV forces " + std::to_string(HIT) + " hits; ca. " + std::to_string((int)(100*HIT/LEN)) + "% density";}
				}
				else {
					if (HIT>=LEN) {sx="Knob sets 100% density";}
					else {sx="Knob sets ca. " + std::to_string((int)(100*HIT/LEN)) + "% density";}
				}
				paramQuantities[HITS_PARAM]->description = (sx);	

			}

			// let's see mother's CLOCK and RESET inputs to calculate our current step
			if (mother->stateRst==1) {activeStep=0;}

			// let's calculate the voltage for the output
			if (mother->stateClk==-1) {
				newVolt=0;	// off clock; means off clock
				outputs[OUTPUT_OUTPUT].setVoltage(0);
				outputs[MIXED_OUTPUT].setVoltage(0);
				} 

			else if (mother->stateClk==1) {

				// https://electro-music.com/forum/topic-62215.html
				// https://electro-music.com/forum/post-406480.html#406480
				// a great Euclidean step calculator
				float newVolt=((((activeStep + SHF) * HIT) % LEN) < HIT)?1.0f:0.0f;			

				activePrec=(PRC<rack::random::uniform());
				if (activePrec) {newVolt=newVolt*-1+1;}

				outputs[OUTPUT_OUTPUT].setVoltage(newVolt*10);
				
				if (MXD==0) {
					outputs[MIXED_OUTPUT].setVoltage(activePrec?10:0);
					paramQuantities[MIXED_PARAM]->description = ("Random clock");	
					}
				else if (MXD==1) {
					outputs[MIXED_OUTPUT].setVoltage(newVolt*-10+10);
					paramQuantities[MIXED_PARAM]->description = ("Inverted output");	
					}
				else if (MXD==2) {
					outputs[MIXED_OUTPUT].setVoltage((mother->newVolt + newVolt==1)?10:0);
					paramQuantities[MIXED_PARAM]->description = ("XOR with mother module");	
					}
				else if (MXD==3) {
					outputs[MIXED_OUTPUT].setVoltage((mother->newVolt==1 || newVolt==1)?10:0);
					paramQuantities[MIXED_PARAM]->description = ("OR with mother module");	
					}
				else if (MXD==4) {
					outputs[MIXED_OUTPUT].setVoltage((mother->newVolt==0 || newVolt==0)?10:0);
					paramQuantities[MIXED_PARAM]->description = ("NOR with mother module");	
					}
				else if (MXD==5) {
					outputs[MIXED_OUTPUT].setVoltage((mother->newVolt == newVolt)?10:0);
					paramQuantities[MIXED_PARAM]->description = ("XNOR with mother module");	
					}
				else if (MXD==6) {
					outputs[MIXED_OUTPUT].setVoltage((mother->newVolt==1 && newVolt==1)?10:0);
					paramQuantities[MIXED_PARAM]->description = ("AND with mother module");	
					}
				else if (MXD==7) {
					outputs[MIXED_OUTPUT].setVoltage(!(mother->newVolt==1 && newVolt==1)?10:0);
					paramQuantities[MIXED_PARAM]->description = ("NAND with mother module");	
					}
				else {
					outputs[MIXED_OUTPUT].setVoltage(10*activeStep/LEN);
					paramQuantities[MIXED_PARAM]->description = ("Not ready yet ...");	
					}

				activeStep++;
				if (activeStep>=LEN) {activeStep=0;};

			}
		}

	}

};
		
struct EuclkMoreWidget : ModuleWidget {

	EuclkMore* module;
	#include "share/widgetwizard.hpp"   // addChild abbreviations

	EuclkMoreWidget(EuclkMore* module) {

		this->module = module;
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/EuclkMore.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		
		childKnob(EuclkMore::MIXED_PARAM, 	2, HP*2, HP*2.5);
		
		childKnob(EuclkMore::LENGTH_PARAM, 	1, HP*2, HP*12.5);
		childKnob(EuclkMore::HITS_PARAM, 	1, HP*2, HP*15.5);
		childKnob(EuclkMore::SHIFTS_PARAM, 	1, HP*2, HP*18.5);
		childKnob(EuclkMore::PREC_PARAM, 	1, HP*2, HP*21.5);
		
		childOutput(EuclkMore::OUTPUT_OUTPUT, 	HP*3, HP*5.5);
		childOutput(EuclkMore::MIXED_OUTPUT, 	HP*1, HP*5.5);
		childInput(EuclkMore::LENGTH_INPUT, 	HP*1, HP*8.5);
		childInput(EuclkMore::HITS_INPUT, 	HP*3, HP*8.5);

	}

};

Model* modelEuclkMore = createModel<EuclkMore, EuclkMoreWidget>("EuclkMore");
