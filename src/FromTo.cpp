// Copyright (c) 2022 András Szabó
#include "plugin.hpp"

struct FromTo : Module {

// --------------------------------------------------

	enum ParamId    {
		FROM_PARAM, TO_PARAM, DIVISION_PARAM, CLOCK_SOLO_PARAM, PARAMS_LEN};

	enum InputId    {
		CLOCK_INPUT, RESET_INPUT, INPUTS_LEN};

	enum OutputId    {
		DIVISIONS_OUTPUT, STEPPED_DIVISION_OUTPUT, REVERSED_DIVISION_OUTPUT, OUTPUTS_LEN};

	enum LightId    {LIGHTS_LEN};

	// small assistance to save older values for reference;
	float paramVal[PARAMS_LEN]={0};
	// float inputVolt[INPUTS_LEN]={0};
	// float lightVal[LIGHTS_LEN]={0};

// --------------------------------------------------

	FromTo() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(CLOCK_SOLO_PARAM, 	0.0f, 1.0f, 0.0f, "Mono or poly OUT");
		configParam(FROM_PARAM, 	-10.0f, 10.0f, 0.0f, "From");
		configParam(TO_PARAM, 	-10.0f, 10.0f, 1.0f, "To");
		configParam(DIVISION_PARAM, 	0.0f, 64.0f, 12.0f, "Division");
		paramQuantities[DIVISION_PARAM]->snapEnabled = true;

		configInput(CLOCK_INPUT, "Clock"); 
		configInput(RESET_INPUT, "Reset"); 

		configOutput(DIVISIONS_OUTPUT, "Divisions"); 
		configOutput(STEPPED_DIVISION_OUTPUT, "Stepped division"); 
		configOutput(REVERSED_DIVISION_OUTPUT, "Reversed stepped division"); 
	}

// --------------------------------------------------

	int loop=0;     // save some CPU in process()

	// for managing the RESET and CLOCK signals
	float newReset=0.0f;
	float oldReset=0.0f;
	float newClock[16]={0.0f};
	float oldClock[16]={0.0f};
	// int currPos[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	int currPos[16]={0};
	bool hitClock=false;

	void process(const ProcessArgs& args) override {

		if (loop--<=0) {
			loop=9000;
			// save some more CPU
			for (int p=0;p<PARAMS_LEN;p++) {paramVal[p]=params[p].getValue();}
			// the poly output should have 16 (or less) channels
			outputs[DIVISIONS_OUTPUT].channels=(paramVal[DIVISION_PARAM]<16)?paramVal[DIVISION_PARAM]:16;
			// the output voltage for the poly output is done here
			for (int p=0;p<outputs[DIVISIONS_OUTPUT].channels;p++) {
				outputs[DIVISIONS_OUTPUT].setVoltage(paramVal[FROM_PARAM]+p*(paramVal[TO_PARAM]-paramVal[FROM_PARAM])/paramVal[DIVISION_PARAM],p);
			}
			// the mono output can be also multichannel if needed (theMatrix needs it)
			outputs[STEPPED_DIVISION_OUTPUT].channels=(paramVal[CLOCK_SOLO_PARAM]==0)?1:outputs[DIVISIONS_OUTPUT].channels;
			outputs[REVERSED_DIVISION_OUTPUT].channels=outputs[STEPPED_DIVISION_OUTPUT].channels;
		}

		// let's see the clock signal by channels
		for (int c=0;c<16;c++) {
			newClock[c]=inputs[CLOCK_INPUT].getVoltage(c);
			// special situation if more output is needed than input
			if (c>=0 && c>=inputs[CLOCK_INPUT].channels) {newClock[c]=newClock[c-1];}
			// business as usual
			if (newClock[c]>2.0f && oldClock[c]<=2.0f) {
				currPos[c]++;
				if (currPos[c]>=(int)paramVal[DIVISION_PARAM]) {currPos[c]=0;}
				// this could be a single line like in row 64
				float newVal=0;
				newVal=(paramVal[TO_PARAM]-paramVal[FROM_PARAM]);
				newVal=newVal/paramVal[DIVISION_PARAM];
				newVal=newVal*currPos[c];
				newVal=newVal+paramVal[FROM_PARAM];
				outputs[STEPPED_DIVISION_OUTPUT].setVoltage(newVal,c);
				newVal=paramVal[TO_PARAM]-newVal+paramVal[FROM_PARAM];
				outputs[REVERSED_DIVISION_OUTPUT].setVoltage(newVal,c);
			}
			oldClock[c]=newClock[c];
		}
	}

// --------------------------------------------------

};

// --------------------------------------------------

struct FromToWidget : ModuleWidget {

	FromTo* module;
	#include "share/widgetwizard.hpp"   // addChild abbreviations

	FromToWidget(FromTo* module) {

		this->module = module;
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/FromTo.svg")));

		// addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		
		childKnob(FromTo::FROM_PARAM, 1, HP*1.5, HP*3);
		childKnob(FromTo::TO_PARAM, 1, HP*1.5, HP*6);
		childKnob(FromTo::DIVISION_PARAM, 1, HP*1.5, HP*9);
		
		childOutput(FromTo::STEPPED_DIVISION_OUTPUT, HP*1.5, HP*12);
		childOutput(FromTo::REVERSED_DIVISION_OUTPUT, HP*1.5, HP*14);
		childOutput(FromTo::DIVISIONS_OUTPUT, HP*1.5, HP*16.5);

		childSwitch(FromTo::CLOCK_SOLO_PARAM, 0, HP*1.5, HP*18.5);
		childInput(FromTo::CLOCK_INPUT, HP*1.5, HP*20.5);
		childInput(FromTo::RESET_INPUT, HP*1.5, HP*23);
	}

};

Model* modelFromTo = createModel<FromTo, FromToWidget>("FromTo");
