// Copyright (c) 2022 András Szabó
#include "plugin.hpp"

struct Sequin : Module {

// --------------------------------------------------

	enum ParamId    {
		CLOCK_MULTIPLACATION_PARAM, CLOCK_DIVISION_PARAM, SH_PARAM, PROBABILITY_PARAM, PARAMS_LEN};

	enum InputId    {
		CLOCK_INPUT, TRIGGER_INPUT, PULSE_WIDTH_INPUT, NOTE_INPUT, PROBABILITY_INPUT, INPUTS_LEN};

	enum OutputId    {
		CLOCK_OUTPUT, MAIN_OUTPUT, OUTPUTS_LEN};

	enum LightId    {
		CLOCK_LIGHT, CLOCK_MOD_LIGHT, LIGHTS_LEN};

	// small assistance to save older values for reference;
	float inputVolt[INPUTS_LEN]={0};
	// float paramVal[PARAMS_LEN]={0};
	// float lightVal[LIGHTS_LEN]={0};

// --------------------------------------------------

	Sequin() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		// configParam(CLOCK_MULTIPLACATION_PARAM, 	1.0f, 8.0f, 1.0f, "Clock multiplacation");
		// configParam(CLOCK_DIVISION_PARAM, 	1.0f, 5.0f, 1.0f, "Clock division");
		// paramQuantities[CLOCK_MULTIPLACATION_PARAM]->snapEnabled = true;
		// paramQuantities[CLOCK_DIVISION_PARAM]->snapEnabled = true;

		configInput(CLOCK_INPUT, "Clock"); 
		configInput(TRIGGER_INPUT, "Trigger"); 
		configInput(PULSE_WIDTH_INPUT, "Pulse width"); 
		configInput(NOTE_INPUT, "Note"); 
		configInput(PROBABILITY_INPUT, "Probability"); 

		configParam(PROBABILITY_PARAM, 	0.0f, 1.0f, 1.0f, "Probabilty");
		configParam(SH_PARAM, 	0.0f, 1.0f, 1.0f, "0: T&H or 1: S&H");
		paramQuantities[SH_PARAM]->snapEnabled = true;

		configOutput(CLOCK_OUTPUT, "Clock"); 
		configOutput(MAIN_OUTPUT, "Main"); 
	}

// --------------------------------------------------

// for calculation the gap, and keeping the current value
int calcGap=0;	
int theGap=0;	// distance between clocks (if 0 then clock in is the order)

// for catching the clock signal
float oldClock=0.0f;
float newClock=0.0f;
bool hitClock=false; 	// a new clock hit is in... let's work

// identify if thos cables are connected
bool conCLK=false;
bool conTRG=false;
bool conPWI=false;
bool conNTE=false;
bool conPRB=false;
float valTRG=0;
float valPWI=0;
float valNTE=0;
float valPRB=1;

// clock modifier
int mulCLK=1;
int divCLK=1;

float voltOut=0;
int indexQuant=0;		// this means no quantization

	float quantMe(float oldVal) {
		if (indexQuant==0) {return oldVal;}
		else if (indexQuant==1) {return round(oldVal);}
		else {return round(oldVal*12)/12;}
	}


// --------------------------------------------------

	int loop=0;     // save some CPU in process()

	void process(const ProcessArgs& args) override {

		if (loop--<=0) {
			loop=9000;
			// save some more CPU
			conCLK=inputs[CLOCK_INPUT].isConnected();
			conTRG=inputs[TRIGGER_INPUT].isConnected();
			conPWI=inputs[PULSE_WIDTH_INPUT].isConnected();
			conNTE=inputs[NOTE_INPUT].isConnected();
			// conPRB=inputs[PROBABILITY_INPUT].isConnected();
			conPRB=valPRB<1;
			valPRB=params[PROBABILITY_PARAM].getValue();
			// mulCLK=params[CLOCK_MULTIPLACATION_PARAM].getValue();
			// divCLK=params[CLOCK_DIVISION_PARAM].getValue();
		}
		
		if (conCLK) {
			// let's see the clock signal
			calcGap++;
			hitClock=false;
			newClock=inputs[CLOCK_INPUT].getVoltage();
			if (newClock>2.0f && oldClock<=2.0f) {
				theGap=(theGap+calcGap)/2;
				calcGap=0;
				hitClock=true;
				valPWI=inputs[PULSE_WIDTH_INPUT].getVoltage()/10;
			}
			// else if (newClock>2.0f && oldClock>2.0f) {}
			// else if (newClock<=2.0f && oldClock<=2.0f) {}
			else if (newClock<=2.0f && oldClock>2.0f) {
				outputs[CLOCK_OUTPUT].setVoltage(0);
				if (!conPWI) {
					// voltOut=0; 
					// outputs[MAIN_OUTPUT].setVoltage(0);
					}
				}
			oldClock=newClock;
		}

		if (!hitClock) {
			if (conPWI && valPWI*theGap<calcGap) {voltOut=0;}
			if (conNTE && voltOut>0 && params[SH_PARAM].getValue()==0) {
				voltOut=inputs[NOTE_INPUT].getVoltage();
			}
			outputs[MAIN_OUTPUT].setVoltage(quantMe(voltOut));
		}
		else if (hitClock) {

			// none is true
			if (!conTRG && !conNTE && !conPRB) {voltOut=10;}

			// only 1 true
			else if (!conTRG && conNTE && !conPRB) {
				voltOut=inputs[NOTE_INPUT].getVoltage();
				}
			else if (!conTRG && !conNTE && conPRB) {
				// valPRB=inputs[PROBABILITY_INPUT].getVoltage();
				// valPRB=abs(valPRB)/10;
				voltOut=(valPRB>rack::random::uniform())?10:0;
				}
			else if (conTRG && !conNTE && !conPRB) {
				valTRG=inputs[TRIGGER_INPUT].getVoltage();
				voltOut=(valTRG>2)?10:0;
				}
				
			// only 2 true
			else if (!conTRG && conNTE && conPRB) {
				valNTE=inputs[NOTE_INPUT].getVoltage();
				// valPRB=inputs[PROBABILITY_INPUT].getVoltage();
				// valPRB=abs(valPRB)/10;
				voltOut=(valPRB>rack::random::uniform())?valNTE:0;
				}
			else if (conTRG && !conNTE && conPRB) {
				valTRG=inputs[TRIGGER_INPUT].getVoltage();
				// valPRB=inputs[PROBABILITY_INPUT].getVoltage();
				// valPRB=abs(valPRB)/10;
				voltOut=(valTRG>2 && valPRB>rack::random::uniform())?10:0;
				}
			else if (conTRG && conNTE && !conPRB) {
				valTRG=inputs[TRIGGER_INPUT].getVoltage();
				valNTE=inputs[NOTE_INPUT].getVoltage();
				voltOut=(valTRG>2)?valNTE:0;
				}
			
			// all 3 true
			else if (conTRG && conNTE && conPRB) {
				valTRG=inputs[TRIGGER_INPUT].getVoltage();
				valNTE=inputs[NOTE_INPUT].getVoltage();
				// valPRB=inputs[PROBABILITY_INPUT].getVoltage();
				// valPRB=abs(valPRB)/10;
				voltOut=(valTRG>2 && valPRB>rack::random::uniform())?valNTE:0;
				}

			outputs[CLOCK_OUTPUT].setVoltage((voltOut>0)?10:0);
			outputs[MAIN_OUTPUT].setVoltage(quantMe(voltOut));
					
		}
		
	}

// --------------------------------------------------

	// this block is to save and reload a variable
	json_t* dataToJson() override {
	json_t* rootJ = json_object();
	json_object_set_new(rootJ, "quant", json_integer(indexQuant));
	return rootJ;}

	void dataFromJson(json_t* rootJ) override {
	json_t *quantJ = json_object_get(rootJ, "quant");
	if (quantJ) indexQuant = json_integer_value(quantJ);
	}
// --------------------------------------------------

};

// --------------------------------------------------

struct SequinWidget : ModuleWidget {

	Sequin* module;
	#include "share/widgetwizard.hpp"   // addChild abbreviations

	SequinWidget(Sequin* module) {

		this->module = module;
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/Sequin.svg")));

		// addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		
		// childKnob(Sequin::CLOCK_MULTIPLACATION_PARAM, 1, HP*1, HP*3);
		// childKnob(Sequin::CLOCK_DIVISION_PARAM, 1, HP*1, HP*6);
		
		childOutput(Sequin::CLOCK_OUTPUT, 		HP*1, HP*3);
		childOutput(Sequin::MAIN_OUTPUT, 		HP*1, HP*6);

		childInput(Sequin::CLOCK_INPUT, 		HP*1, HP*9);
		childInput(Sequin::TRIGGER_INPUT, 		HP*1, HP*12);
		childInput(Sequin::PULSE_WIDTH_INPUT, 	HP*1, HP*15);
		childInput(Sequin::NOTE_INPUT, 			HP*1, HP*18);
		childSwitch(Sequin::	SH_PARAM, 0, HP*1, HP*19.75);
		
		// childInput(Sequin::PROBABILITY_INPUT, 	HP*1, HP*22);
		childKnob(Sequin::PROBABILITY_PARAM, 0, 	HP*1, HP*22);
		
		// childLight(Sequin::CLOCK_LIGHT, 1, HP*0.5, HP*8);
		// childLight(Sequin::CLOCK_MOD_LIGHT, 0, HP*1.5, HP*8);
	}
	
	void appendContextMenu(Menu* menu) override {
		Sequin* module = dynamic_cast<Sequin*>(this->module);
		assert(module);
		menu->addChild(new MenuSeparator);
		menu->addChild(createIndexPtrSubmenuItem("Quantize", {"Nope","Octaves","Notes"}, &module->indexQuant));
		// menu->addChild(createIndexPtrSubmenuItem("Pulse width multiplication", {"x1","x2","x3","x4","x5","x6","x7","x8"}, &module->indexPWcalc));
	}

};

Model* modelSequin = createModel<Sequin, SequinWidget>("Sequin");
