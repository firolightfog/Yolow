// Copyright (c) 2022 András Szabó
#include "plugin.hpp"

struct Celesta : Module {

// --------------------------------------------------

	enum ParamId    {
		PRECISION_PARAM, OCTAVE_PARAM, RANGE_PARAM, SEQ_A_RUN_PARAM, SEQ_B_RUN_PARAM, 
		SEQ_C_RUN_PARAM, SEQ_A_STEPS_PARAM, SEQ_B_STEPS_PARAM, SEQ_C_STEPS_PARAM, SEQ_A_1_VOLTAGE_PARAM, 
		SEQ_A_2_VOLTAGE_PARAM, SEQ_A_3_VOLTAGE_PARAM, SEQ_A_4_VOLTAGE_PARAM, SEQ_A_5_VOLTAGE_PARAM, SEQ_A_6_VOLTAGE_PARAM, 
		SEQ_A_7_VOLTAGE_PARAM, SEQ_A_8_VOLTAGE_PARAM, SEQ_B_1_VOLTAGE_PARAM, SEQ_B_2_VOLTAGE_PARAM, SEQ_B_3_VOLTAGE_PARAM, 
		SEQ_B_4_VOLTAGE_PARAM, SEQ_B_5_VOLTAGE_PARAM, SEQ_B_6_VOLTAGE_PARAM, SEQ_B_7_VOLTAGE_PARAM, SEQ_B_8_VOLTAGE_PARAM, 
		SEQ_C_1_VOLTAGE_PARAM, SEQ_C_2_VOLTAGE_PARAM, SEQ_C_3_VOLTAGE_PARAM, SEQ_C_4_VOLTAGE_PARAM, SEQ_C_5_VOLTAGE_PARAM, 
		SEQ_C_6_VOLTAGE_PARAM, SEQ_C_7_VOLTAGE_PARAM, SEQ_C_8_VOLTAGE_PARAM, SEQ_A_1_PRECISION_PARAM, SEQ_A_2_PRECISION_PARAM, 
		SEQ_A_3_PRECISION_PARAM, SEQ_A_4_PRECISION_PARAM, SEQ_A_5_PRECISION_PARAM, SEQ_A_6_PRECISION_PARAM, SEQ_A_7_PRECISION_PARAM, 
		SEQ_A_8_PRECISION_PARAM, SEQ_B_1_PRECISION_PARAM, SEQ_B_2_PRECISION_PARAM, SEQ_B_3_PRECISION_PARAM, SEQ_B_4_PRECISION_PARAM, 
		SEQ_B_5_PRECISION_PARAM, SEQ_B_6_PRECISION_PARAM, SEQ_B_7_PRECISION_PARAM, SEQ_B_8_PRECISION_PARAM, SEQ_C_1_PRECISION_PARAM, 
		SEQ_C_2_PRECISION_PARAM, SEQ_C_3_PRECISION_PARAM, SEQ_C_4_PRECISION_PARAM, SEQ_C_5_PRECISION_PARAM, SEQ_C_6_PRECISION_PARAM, 
		SEQ_C_7_PRECISION_PARAM, SEQ_C_8_PRECISION_PARAM, PARAMS_LEN};

	enum InputId    {
		CLOCK_INPUT, RESET_INPUT, PRECISION_CV_INPUT, INPUTS_LEN};

	enum OutputId    {
		MERGED_MONO_OUTPUT, SEQ_A_OUTPUT, SEQ_B_OUTPUT, SEQ_C_OUTPUT, SEQ_A_EOC_OUTPUT, SEQ_B_EOC_OUTPUT, SEQ_C_EOC_OUTPUT, OUTPUTS_LEN};

	enum LightId    {
		SEQ_A_1_LED_LIGHT, SEQ_A_2_LED_LIGHT, SEQ_A_3_LED_LIGHT, SEQ_A_4_LED_LIGHT, SEQ_A_5_LED_LIGHT, 
		SEQ_A_6_LED_LIGHT, SEQ_A_7_LED_LIGHT, SEQ_A_8_LED_LIGHT, SEQ_B_1_LED_LIGHT, SEQ_B_2_LED_LIGHT, 
		SEQ_B_3_LED_LIGHT, SEQ_B_4_LED_LIGHT, SEQ_B_5_LED_LIGHT, SEQ_B_6_LED_LIGHT, SEQ_B_7_LED_LIGHT, 
		SEQ_B_8_LED_LIGHT, SEQ_C_1_LED_LIGHT, SEQ_C_2_LED_LIGHT, SEQ_C_3_LED_LIGHT, SEQ_C_4_LED_LIGHT, 
		SEQ_C_5_LED_LIGHT, SEQ_C_6_LED_LIGHT, SEQ_C_7_LED_LIGHT, SEQ_C_8_LED_LIGHT, 
		LED_SEQ_A_RUN_PARAM, LED_SEQ_B_RUN_PARAM, LED_SEQ_C_RUN_PARAM, LIGHTS_LEN};

	// small assistance to save older values for reference;
	float paramVal[PARAMS_LEN]={0};
	float inputVolt[INPUTS_LEN]={0};
	float lightVal[LIGHTS_LEN]={0};

// --------------------------------------------------

	Celesta() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(PRECISION_PARAM, 	0.0f, 1.0f, 0.95f, "Precision");

		configParam(OCTAVE_PARAM, 	-5, 5, 0.0f, "Octave");
		configParam(RANGE_PARAM, 	1, 7, 1.0f, "Range");
		paramQuantities[OCTAVE_PARAM]->snapEnabled = true;
		paramQuantities[RANGE_PARAM]->snapEnabled = true;
		
		configParam(SEQ_A_RUN_PARAM, 	0.0f, 1.0f, 0.0f, "Seq A dice");
		configParam(SEQ_B_RUN_PARAM, 	0.0f, 1.0f, 0.0f, "Seq B dice");
		configParam(SEQ_C_RUN_PARAM, 	0.0f, 1.0f, 0.0f, "Seq C dice");
		paramQuantities[SEQ_A_RUN_PARAM]->snapEnabled = true;
		paramQuantities[SEQ_B_RUN_PARAM]->snapEnabled = true;
		paramQuantities[SEQ_C_RUN_PARAM]->snapEnabled = true;

		configParam(SEQ_A_STEPS_PARAM, 	1.0f, 8.0f, 6.0f, "Seq A steps");
		configParam(SEQ_B_STEPS_PARAM, 	1.0f, 8.0f, 4.0f, "Seq B steps");
		configParam(SEQ_C_STEPS_PARAM, 	1.0f, 8.0f, 2.0f, "Seq C steps");
		paramQuantities[SEQ_A_STEPS_PARAM]->snapEnabled = true;
		paramQuantities[SEQ_B_STEPS_PARAM]->snapEnabled = true;
		paramQuantities[SEQ_C_STEPS_PARAM]->snapEnabled = true;

		configParam(SEQ_A_1_VOLTAGE_PARAM, 	0.0f, 1.0f, 0.0f, "Seq A 1 voltage");
		configParam(SEQ_A_2_VOLTAGE_PARAM, 	0.0f, 1.0f, 0.0f, "Seq A 2 voltage");
		configParam(SEQ_A_3_VOLTAGE_PARAM, 	0.0f, 1.0f, 0.0f, "Seq A 3 voltage");
		configParam(SEQ_A_4_VOLTAGE_PARAM, 	0.0f, 1.0f, 0.0f, "Seq A 4 voltage");
		configParam(SEQ_A_5_VOLTAGE_PARAM, 	0.0f, 1.0f, 0.0f, "Seq A 5 voltage");
		configParam(SEQ_A_6_VOLTAGE_PARAM, 	0.0f, 1.0f, 0.0f, "Seq A 6 voltage");
		configParam(SEQ_A_7_VOLTAGE_PARAM, 	0.0f, 1.0f, 0.0f, "Seq A 7 voltage");
		configParam(SEQ_A_8_VOLTAGE_PARAM, 	0.0f, 1.0f, 0.0f, "Seq A 8 voltage");
		configParam(SEQ_B_1_VOLTAGE_PARAM, 	0.0f, 1.0f, 0.0f, "Seq B 1 voltage");
		configParam(SEQ_B_2_VOLTAGE_PARAM, 	0.0f, 1.0f, 0.0f, "Seq B 2 voltage");
		configParam(SEQ_B_3_VOLTAGE_PARAM, 	0.0f, 1.0f, 0.0f, "Seq B 3 voltage");
		configParam(SEQ_B_4_VOLTAGE_PARAM, 	0.0f, 1.0f, 0.0f, "Seq B 4 voltage");
		configParam(SEQ_B_5_VOLTAGE_PARAM, 	0.0f, 1.0f, 0.0f, "Seq B 5 voltage");
		configParam(SEQ_B_6_VOLTAGE_PARAM, 	0.0f, 1.0f, 0.0f, "Seq B 6 voltage");
		configParam(SEQ_B_7_VOLTAGE_PARAM, 	0.0f, 1.0f, 0.0f, "Seq B 7 voltage");
		configParam(SEQ_B_8_VOLTAGE_PARAM, 	0.0f, 1.0f, 0.0f, "Seq B 8 voltage");
		configParam(SEQ_C_1_VOLTAGE_PARAM, 	0.0f, 1.0f, 0.0f, "Seq C 1 voltage");
		configParam(SEQ_C_2_VOLTAGE_PARAM, 	0.0f, 1.0f, 0.0f, "Seq C 2 voltage");
		configParam(SEQ_C_3_VOLTAGE_PARAM, 	0.0f, 1.0f, 0.0f, "Seq C 3 voltage");
		configParam(SEQ_C_4_VOLTAGE_PARAM, 	0.0f, 1.0f, 0.0f, "Seq C 4 voltage");
		configParam(SEQ_C_5_VOLTAGE_PARAM, 	0.0f, 1.0f, 0.0f, "Seq C 5 voltage");
		configParam(SEQ_C_6_VOLTAGE_PARAM, 	0.0f, 1.0f, 0.0f, "Seq C 6 voltage");
		configParam(SEQ_C_7_VOLTAGE_PARAM, 	0.0f, 1.0f, 0.0f, "Seq C 7 voltage");
		configParam(SEQ_C_8_VOLTAGE_PARAM, 	0.0f, 1.0f, 0.0f, "Seq C 8 voltage");
		configParam(SEQ_A_1_PRECISION_PARAM, 	0.0f, 2.0f, 0.0f, "Seq A 1 precision");
		configParam(SEQ_A_2_PRECISION_PARAM, 	0.0f, 2.0f, 0.0f, "Seq A 2 precision");
		configParam(SEQ_A_3_PRECISION_PARAM, 	0.0f, 2.0f, 0.0f, "Seq A 3 precision");
		configParam(SEQ_A_4_PRECISION_PARAM, 	0.0f, 2.0f, 0.0f, "Seq A 4 precision");
		configParam(SEQ_A_5_PRECISION_PARAM, 	0.0f, 2.0f, 0.0f, "Seq A 5 precision");
		configParam(SEQ_A_6_PRECISION_PARAM, 	0.0f, 2.0f, 0.0f, "Seq A 6 precision");
		configParam(SEQ_A_7_PRECISION_PARAM, 	0.0f, 2.0f, 0.0f, "Seq A 7 precision");
		configParam(SEQ_A_8_PRECISION_PARAM, 	0.0f, 2.0f, 0.0f, "Seq A 8 precision");
		configParam(SEQ_B_1_PRECISION_PARAM, 	0.0f, 2.0f, 0.0f, "Seq B 1 precision");
		configParam(SEQ_B_2_PRECISION_PARAM, 	0.0f, 2.0f, 0.0f, "Seq B 2 precision");
		configParam(SEQ_B_3_PRECISION_PARAM, 	0.0f, 2.0f, 0.0f, "Seq B 3 precision");
		configParam(SEQ_B_4_PRECISION_PARAM, 	0.0f, 2.0f, 0.0f, "Seq B 4 precision");
		configParam(SEQ_B_5_PRECISION_PARAM, 	0.0f, 2.0f, 0.0f, "Seq B 5 precision");
		configParam(SEQ_B_6_PRECISION_PARAM, 	0.0f, 2.0f, 0.0f, "Seq B 6 precision");
		configParam(SEQ_B_7_PRECISION_PARAM, 	0.0f, 2.0f, 0.0f, "Seq B 7 precision");
		configParam(SEQ_B_8_PRECISION_PARAM, 	0.0f, 2.0f, 0.0f, "Seq B 8 precision");
		configParam(SEQ_C_1_PRECISION_PARAM, 	0.0f, 2.0f, 0.0f, "Seq C 1 precision");
		configParam(SEQ_C_2_PRECISION_PARAM, 	0.0f, 2.0f, 0.0f, "Seq C 2 precision");
		configParam(SEQ_C_3_PRECISION_PARAM, 	0.0f, 2.0f, 0.0f, "Seq C 3 precision");
		configParam(SEQ_C_4_PRECISION_PARAM, 	0.0f, 2.0f, 0.0f, "Seq C 4 precision");
		configParam(SEQ_C_5_PRECISION_PARAM, 	0.0f, 2.0f, 0.0f, "Seq C 5 precision");
		configParam(SEQ_C_6_PRECISION_PARAM, 	0.0f, 2.0f, 0.0f, "Seq C 6 precision");
		configParam(SEQ_C_7_PRECISION_PARAM, 	0.0f, 2.0f, 0.0f, "Seq C 7 precision");
		configParam(SEQ_C_8_PRECISION_PARAM, 	0.0f, 2.0f, 0.0f, "Seq C 8 precision");
		paramQuantities[SEQ_A_1_PRECISION_PARAM]->snapEnabled = true;
		paramQuantities[SEQ_A_2_PRECISION_PARAM]->snapEnabled = true;
		paramQuantities[SEQ_A_3_PRECISION_PARAM]->snapEnabled = true;
		paramQuantities[SEQ_A_4_PRECISION_PARAM]->snapEnabled = true;
		paramQuantities[SEQ_A_5_PRECISION_PARAM]->snapEnabled = true;
		paramQuantities[SEQ_A_6_PRECISION_PARAM]->snapEnabled = true;
		paramQuantities[SEQ_A_7_PRECISION_PARAM]->snapEnabled = true;
		paramQuantities[SEQ_A_8_PRECISION_PARAM]->snapEnabled = true;
		paramQuantities[SEQ_B_1_PRECISION_PARAM]->snapEnabled = true;
		paramQuantities[SEQ_B_2_PRECISION_PARAM]->snapEnabled = true;
		paramQuantities[SEQ_B_3_PRECISION_PARAM]->snapEnabled = true;
		paramQuantities[SEQ_B_4_PRECISION_PARAM]->snapEnabled = true;
		paramQuantities[SEQ_B_5_PRECISION_PARAM]->snapEnabled = true;
		paramQuantities[SEQ_B_6_PRECISION_PARAM]->snapEnabled = true;
		paramQuantities[SEQ_B_7_PRECISION_PARAM]->snapEnabled = true;
		paramQuantities[SEQ_B_8_PRECISION_PARAM]->snapEnabled = true;
		paramQuantities[SEQ_C_1_PRECISION_PARAM]->snapEnabled = true;
		paramQuantities[SEQ_C_2_PRECISION_PARAM]->snapEnabled = true;
		paramQuantities[SEQ_C_3_PRECISION_PARAM]->snapEnabled = true;
		paramQuantities[SEQ_C_4_PRECISION_PARAM]->snapEnabled = true;
		paramQuantities[SEQ_C_5_PRECISION_PARAM]->snapEnabled = true;
		paramQuantities[SEQ_C_6_PRECISION_PARAM]->snapEnabled = true;
		paramQuantities[SEQ_C_7_PRECISION_PARAM]->snapEnabled = true;
		paramQuantities[SEQ_C_8_PRECISION_PARAM]->snapEnabled = true;

		configInput(CLOCK_INPUT, "Clock"); 
		configInput(RESET_INPUT, "Reset"); 
		configInput(PRECISION_CV_INPUT, "Precision CV"); 

		configOutput(MERGED_MONO_OUTPUT, "Merged mono"); 
		configOutput(SEQ_A_OUTPUT, "Seq A"); 
		configOutput(SEQ_B_OUTPUT, "Seq B"); 
		configOutput(SEQ_C_OUTPUT, "Seq C"); 
		configOutput(SEQ_A_EOC_OUTPUT, "Seq A EOC"); 
		configOutput(SEQ_B_EOC_OUTPUT, "Seq B EOC"); 
		configOutput(SEQ_C_EOC_OUTPUT, "Seq C EOC"); 
	}


// pressing i provides a variation of the same tune
void invKnob() {
	for (int k=0;k<8*3;k++) {
		params[SEQ_A_1_VOLTAGE_PARAM+k].setValue(abs(1-params[SEQ_A_1_VOLTAGE_PARAM+k].getValue()));
	}
}

// pressing r randomizes the note knobs only
void rndKnob() {
	params[SEQ_A_STEPS_PARAM].setValue(rand() % 7 + 2);	// random steps between 2-8
	params[SEQ_B_STEPS_PARAM].setValue(rand() % 7 + 2);	// random steps between 2-8
	params[SEQ_C_STEPS_PARAM].setValue(rand() % 7 + 2);	// random steps between 2-8
	for (int k=0;k<8*3;k++) {
		params[SEQ_A_1_VOLTAGE_PARAM+k].setValue(rack::random::uniform());
	}
}

// --------------------------------------------------

	int loop=0;     // save some CPU in process()
	float precVal=1.0f;

	// it saves RST and CLK signal
	float oldReset=0;
	float newReset=0;
	float oldClock=0;
	float newClock=0;
	
	// this stores the step reference we're at
	int stepA=1;
	int stepB=1;
	int stepC=1;
	
	// it temporarily saves the precision setting of a step
	int precA=0;
	int precB=0;
	int precC=0;
	
	// it temporarily saves the output voltage
	float voltA=0.0f;
	float voltB=0.0f;
	float voltC=0.0f;

	int indexQuant=0;	// this means no quantization

	float quantMe(float oldVal) {
		// oldVal=oldVal+5*indexRange;
		if (indexQuant==0) {return oldVal;}
		else if (indexQuant==1) {return round(oldVal);}
		else {return round(oldVal*12)/12;}
	}

	void process(const ProcessArgs& args) override {

		if (loop--<=0) {
			loop=9000;
			if (inputs[PRECISION_CV_INPUT].isConnected()) {
				params[PRECISION_PARAM].setValue(abs(inputs[PRECISION_CV_INPUT].getVoltage()));
				}
			precVal=params[PRECISION_PARAM].getValue();
			lights[LED_SEQ_A_RUN_PARAM].setBrightness(params[SEQ_A_RUN_PARAM].getValue());
			lights[LED_SEQ_B_RUN_PARAM].setBrightness(params[SEQ_B_RUN_PARAM].getValue());
			lights[LED_SEQ_C_RUN_PARAM].setBrightness(params[SEQ_C_RUN_PARAM].getValue());
			// save some more CPU
			
			if (lights[LED_SEQ_A_RUN_PARAM].getBrightness()==1) {
				for (int s=0;s<8;s++) {params[SEQ_A_1_VOLTAGE_PARAM+s].setValue(rack::random::uniform());}
				lights[LED_SEQ_A_RUN_PARAM].setBrightness(0);
				params[SEQ_A_RUN_PARAM].setValue(0);
			}
			if (lights[LED_SEQ_B_RUN_PARAM].getBrightness()==1) {
				for (int s=0;s<8;s++) {params[SEQ_B_1_VOLTAGE_PARAM+s].setValue(rack::random::uniform());}				
				lights[LED_SEQ_B_RUN_PARAM].setBrightness(0);
				params[SEQ_B_RUN_PARAM].setValue(0);
			}
			if (lights[LED_SEQ_C_RUN_PARAM].getBrightness()==1) {
				for (int s=0;s<8;s++) {params[SEQ_C_1_VOLTAGE_PARAM+s].setValue(rack::random::uniform());}				
				lights[LED_SEQ_C_RUN_PARAM].setBrightness(0);
				params[SEQ_C_RUN_PARAM].setValue(0);
			}
		}

		// let's see the reset signal
		newReset=inputs[RESET_INPUT].getVoltage();
		if (newReset>0.2 && oldReset<=0.2) {
			stepA=-1; stepB=1; stepC=1;
			outputs[SEQ_A_EOC_OUTPUT].setVoltage(10);
			outputs[SEQ_B_EOC_OUTPUT].setVoltage(10);
			outputs[SEQ_C_EOC_OUTPUT].setVoltage(10);			
			}
		// else if (newReset>0.2 && oldReset>0.2) {}
		// else if (newReset<=0.2 && oldReset>0.2) {}
		// else if (newReset<=0.2 && oldReset<=0.2) {}
		oldReset=newReset;

		// let's see the clock signal
		newClock=inputs[CLOCK_INPUT].getVoltage();
		if (newClock>0.2 && oldClock<=0.2) {
			
			// take the next step
			stepA++; 
			if (stepA==0) {stepA++;}	// bug fix; added to allow reset to properly jump to the first step
			if (stepA>params[SEQ_A_STEPS_PARAM].getValue()) {stepA=1; stepB++; outputs[SEQ_A_EOC_OUTPUT].setVoltage(10);}
			if (stepB>params[SEQ_B_STEPS_PARAM].getValue()) {stepB=1; stepC++; outputs[SEQ_B_EOC_OUTPUT].setVoltage(10);}
			if (stepC>params[SEQ_C_STEPS_PARAM].getValue()) {stepC=1; outputs[SEQ_C_EOC_OUTPUT].setVoltage(10);}

			// let's memorize the precision settings
			precA=params[SEQ_A_1_PRECISION_PARAM+stepA-1].getValue();
			precB=params[SEQ_B_1_PRECISION_PARAM+stepB-1].getValue();
			precC=params[SEQ_C_1_PRECISION_PARAM+stepC-1].getValue();

			// let's get the next voltage (if needed)
			// 0 = always perfect; 1 = grab a random note instead; 2 = play the previous by mistake
			if (precA==0 || (precA==2 && rack::random::uniform()>precVal)) {
				voltA=params[SEQ_A_1_VOLTAGE_PARAM+stepA-1].getValue();
			}
			else if (precA==1 && rack::random::uniform()>precVal) {
				voltA=params[SEQ_A_1_VOLTAGE_PARAM+(rand() % 8)].getValue();
			}
			else {
				voltA=params[SEQ_A_1_VOLTAGE_PARAM+stepA-1].getValue();
			}
			if (precB==0 || (precB==2 && rack::random::uniform()>precVal)) {
				voltB=params[SEQ_B_1_VOLTAGE_PARAM+stepB-1].getValue();
			}
			else if (precB==1 && rack::random::uniform()>precVal) {
				voltB=params[SEQ_B_1_VOLTAGE_PARAM+(rand() % 8)].getValue();
			}
			else {
				voltB=params[SEQ_B_1_VOLTAGE_PARAM+stepB-1].getValue();
			}
			if (precC==0 || (precC==2 && rack::random::uniform()>precVal)) {
				voltC=params[SEQ_C_1_VOLTAGE_PARAM+stepC-1].getValue();
			}
			else if (precC==1 && rack::random::uniform()>precVal) {
				voltC=params[SEQ_C_1_VOLTAGE_PARAM+(rand() % 8)].getValue();
			}
			else {
				voltC=params[SEQ_C_1_VOLTAGE_PARAM+stepC-1].getValue();
			}
			// voltB=params[SEQ_B_1_VOLTAGE_PARAM+stepB-1].getValue();
			// voltA=params[SEQ_A_1_VOLTAGE_PARAM+stepA-1].getValue();
			// voltC=params[SEQ_C_1_VOLTAGE_PARAM+stepC-1].getValue();
			
			// remember to us the range
			voltA=voltA*params[RANGE_PARAM].getValue();
			voltB=voltB*params[RANGE_PARAM].getValue();
			voltC=voltC*params[RANGE_PARAM].getValue();

			// remember to use the pre-defined octave settings
			// voltA+=params[OCTAVE_PARAM].getValue();
			// voltB+=params[OCTAVE_PARAM].getValue();
			// voltC+=params[OCTAVE_PARAM].getValue();
			
			// dump it :)
			outputs[SEQ_A_OUTPUT].setVoltage(quantMe(voltA + params[OCTAVE_PARAM].getValue()));
			outputs[SEQ_B_OUTPUT].setVoltage(quantMe(voltB + params[OCTAVE_PARAM].getValue()));
			outputs[SEQ_C_OUTPUT].setVoltage(quantMe(voltC + params[OCTAVE_PARAM].getValue()));
			outputs[MERGED_MONO_OUTPUT].setVoltage(quantMe(voltA + voltB + voltC + params[OCTAVE_PARAM].getValue()));
			
			// and now highlight the LEDs that steps can be followed
			for (int l=0;l<24;l++) {
				lights[SEQ_A_1_LED_LIGHT+l].setBrightness(0);
			}
			lights[SEQ_A_1_LED_LIGHT+stepA-1].setBrightness(1);
			lights[SEQ_B_1_LED_LIGHT+stepB-1].setBrightness(1);
			lights[SEQ_C_1_LED_LIGHT+stepC-1].setBrightness(1);
		
		}
		// else if (newClock>0.2 && oldClock>0.2) {}
		// else if (newClock<=0.2 && oldClock>0.2) {}
		// else if (newClock<=0.2 && oldClock<=0.2) {}
		oldClock=newClock;
		
		if (newClock<=0.2) {
			outputs[SEQ_A_EOC_OUTPUT].setVoltage(0);
			outputs[SEQ_B_EOC_OUTPUT].setVoltage(0);
			outputs[SEQ_C_EOC_OUTPUT].setVoltage(0);		
		}

	}

	// this JSON block is to save and reload a variable
	json_t* dataToJson() override {
	json_t* rootJ = json_object();
	json_object_set_new(rootJ, "quant", json_integer(indexQuant));
	return rootJ;}

	void dataFromJson(json_t* rootJ) override {
	json_t *quantJ = json_object_get(rootJ, "quant");
	if (quantJ) indexQuant = json_integer_value(quantJ);}

};

// --------------------------------------------------

struct CelestaWidget : ModuleWidget {

	Celesta* module;
	#include "share/widgetwizard.hpp"   // addChild abbreviations

	CelestaWidget(Celesta* module) {

		this->module = module;
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/Celesta.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		
		childKnob(Celesta::PRECISION_PARAM, 1, HP*2, HP*13);
		childKnob(Celesta::OCTAVE_PARAM, 1, HP*2, HP*20.5);
		childKnob(Celesta::RANGE_PARAM, 1, HP*2, HP*23);
		childPushbutton(Celesta::SEQ_A_RUN_PARAM, Celesta::LED_SEQ_A_RUN_PARAM, HP*5, HP*2.5);
		childPushbutton(Celesta::SEQ_B_RUN_PARAM, Celesta::LED_SEQ_B_RUN_PARAM, HP*9.5, HP*2.5);
		childPushbutton(Celesta::SEQ_C_RUN_PARAM, Celesta::LED_SEQ_C_RUN_PARAM, HP*14, HP*2.5);
		childKnob(Celesta::SEQ_A_STEPS_PARAM, 1, HP*7, HP*2.5);
		childKnob(Celesta::SEQ_B_STEPS_PARAM, 1, HP*11.5, HP*2.5);
		childKnob(Celesta::SEQ_C_STEPS_PARAM, 1, HP*16, HP*2.5);
		childKnob(Celesta::SEQ_A_1_VOLTAGE_PARAM, 1, HP*5, HP*5.5);
		childKnob(Celesta::SEQ_A_2_VOLTAGE_PARAM, 1, HP*5, HP*8);
		childKnob(Celesta::SEQ_A_3_VOLTAGE_PARAM, 1, HP*5, HP*10.5);
		childKnob(Celesta::SEQ_A_4_VOLTAGE_PARAM, 1, HP*5, HP*13);
		childKnob(Celesta::SEQ_A_5_VOLTAGE_PARAM, 1, HP*5, HP*15.5);
		childKnob(Celesta::SEQ_A_6_VOLTAGE_PARAM, 1, HP*5, HP*18);
		childKnob(Celesta::SEQ_A_7_VOLTAGE_PARAM, 1, HP*5, HP*20.5);
		childKnob(Celesta::SEQ_A_8_VOLTAGE_PARAM, 1, HP*5, HP*23);
		childKnob(Celesta::SEQ_B_1_VOLTAGE_PARAM, 1, HP*9.5, HP*5.5);
		childKnob(Celesta::SEQ_B_2_VOLTAGE_PARAM, 1, HP*9.5, HP*8);
		childKnob(Celesta::SEQ_B_3_VOLTAGE_PARAM, 1, HP*9.5, HP*10.5);
		childKnob(Celesta::SEQ_B_4_VOLTAGE_PARAM, 1, HP*9.5, HP*13);
		childKnob(Celesta::SEQ_B_5_VOLTAGE_PARAM, 1, HP*9.5, HP*15.5);
		childKnob(Celesta::SEQ_B_6_VOLTAGE_PARAM, 1, HP*9.5, HP*18);
		childKnob(Celesta::SEQ_B_7_VOLTAGE_PARAM, 1, HP*9.5, HP*20.5);
		childKnob(Celesta::SEQ_B_8_VOLTAGE_PARAM, 1, HP*9.5, HP*23);
		childKnob(Celesta::SEQ_C_1_VOLTAGE_PARAM, 1, HP*14, HP*5.5);
		childKnob(Celesta::SEQ_C_2_VOLTAGE_PARAM, 1, HP*14, HP*8);
		childKnob(Celesta::SEQ_C_3_VOLTAGE_PARAM, 1, HP*14, HP*10.5);
		childKnob(Celesta::SEQ_C_4_VOLTAGE_PARAM, 1, HP*14, HP*13);
		childKnob(Celesta::SEQ_C_5_VOLTAGE_PARAM, 1, HP*14, HP*15.5);
		childKnob(Celesta::SEQ_C_6_VOLTAGE_PARAM, 1, HP*14, HP*18);
		childKnob(Celesta::SEQ_C_7_VOLTAGE_PARAM, 1, HP*14, HP*20.5);
		childKnob(Celesta::SEQ_C_8_VOLTAGE_PARAM, 1, HP*14, HP*23);
		childSwitch(Celesta::SEQ_A_1_PRECISION_PARAM, 1, HP*7, HP*5.5);
		childSwitch(Celesta::SEQ_A_2_PRECISION_PARAM, 1, HP*7, HP*8);
		childSwitch(Celesta::SEQ_A_3_PRECISION_PARAM, 1, HP*7, HP*10.5);
		childSwitch(Celesta::SEQ_A_4_PRECISION_PARAM, 1, HP*7, HP*13);
		childSwitch(Celesta::SEQ_A_5_PRECISION_PARAM, 1, HP*7, HP*15.5);
		childSwitch(Celesta::SEQ_A_6_PRECISION_PARAM, 1, HP*7, HP*18);
		childSwitch(Celesta::SEQ_A_7_PRECISION_PARAM, 1, HP*7, HP*20.5);
		childSwitch(Celesta::SEQ_A_8_PRECISION_PARAM, 1, HP*7, HP*23);
		childSwitch(Celesta::SEQ_B_1_PRECISION_PARAM, 1, HP*11.5, HP*5.5);
		childSwitch(Celesta::SEQ_B_2_PRECISION_PARAM, 1, HP*11.5, HP*8);
		childSwitch(Celesta::SEQ_B_3_PRECISION_PARAM, 1, HP*11.5, HP*10.5);
		childSwitch(Celesta::SEQ_B_4_PRECISION_PARAM, 1, HP*11.5, HP*13);
		childSwitch(Celesta::SEQ_B_5_PRECISION_PARAM, 1, HP*11.5, HP*15.5);
		childSwitch(Celesta::SEQ_B_6_PRECISION_PARAM, 1, HP*11.5, HP*18);
		childSwitch(Celesta::SEQ_B_7_PRECISION_PARAM, 1, HP*11.5, HP*20.5);
		childSwitch(Celesta::SEQ_B_8_PRECISION_PARAM, 1, HP*11.5, HP*23);
		childSwitch(Celesta::SEQ_C_1_PRECISION_PARAM, 1, HP*16, HP*5.5);
		childSwitch(Celesta::SEQ_C_2_PRECISION_PARAM, 1, HP*16, HP*8);
		childSwitch(Celesta::SEQ_C_3_PRECISION_PARAM, 1, HP*16, HP*10.5);
		childSwitch(Celesta::SEQ_C_4_PRECISION_PARAM, 1, HP*16, HP*13);
		childSwitch(Celesta::SEQ_C_5_PRECISION_PARAM, 1, HP*16, HP*15.5);
		childSwitch(Celesta::SEQ_C_6_PRECISION_PARAM, 1, HP*16, HP*18);
		childSwitch(Celesta::SEQ_C_7_PRECISION_PARAM, 1, HP*16, HP*20.5);
		childSwitch(Celesta::SEQ_C_8_PRECISION_PARAM, 1, HP*16, HP*23);
		
		childInput(Celesta::CLOCK_INPUT, HP*2, HP*5.5);
		childInput(Celesta::RESET_INPUT, HP*2, HP*8);
		childInput(Celesta::PRECISION_CV_INPUT, HP*2, HP*15.5);
		childOutput(Celesta::MERGED_MONO_OUTPUT, HP*18, HP*21.75);
		childOutput(Celesta::SEQ_A_OUTPUT, HP*18, HP*6.75);
		childOutput(Celesta::SEQ_B_OUTPUT, HP*18, HP*9.25);
		childOutput(Celesta::SEQ_C_OUTPUT, HP*18, HP*11.75);
		childOutput(Celesta::SEQ_A_EOC_OUTPUT, HP*18, HP*14.25);
		childOutput(Celesta::SEQ_B_EOC_OUTPUT, HP*18, HP*16.75);
		childOutput(Celesta::SEQ_C_EOC_OUTPUT, HP*18, HP*19.25);
		childLight(Celesta::SEQ_A_1_LED_LIGHT, 0, HP*5, HP*5.5);
		childLight(Celesta::SEQ_A_2_LED_LIGHT, 0, HP*5, HP*8);
		childLight(Celesta::SEQ_A_3_LED_LIGHT, 0, HP*5, HP*10.5);
		childLight(Celesta::SEQ_A_4_LED_LIGHT, 0, HP*5, HP*13);
		childLight(Celesta::SEQ_A_5_LED_LIGHT, 0, HP*5, HP*15.5);
		childLight(Celesta::SEQ_A_6_LED_LIGHT, 0, HP*5, HP*18);
		childLight(Celesta::SEQ_A_7_LED_LIGHT, 0, HP*5, HP*20.5);
		childLight(Celesta::SEQ_A_8_LED_LIGHT, 0, HP*5, HP*23);
		childLight(Celesta::SEQ_B_1_LED_LIGHT, 0, HP*9.5, HP*5.5);
		childLight(Celesta::SEQ_B_2_LED_LIGHT, 0, HP*9.5, HP*8);
		childLight(Celesta::SEQ_B_3_LED_LIGHT, 0, HP*9.5, HP*10.5);
		childLight(Celesta::SEQ_B_4_LED_LIGHT, 0, HP*9.5, HP*13);
		childLight(Celesta::SEQ_B_5_LED_LIGHT, 0, HP*9.5, HP*15.5);
		childLight(Celesta::SEQ_B_6_LED_LIGHT, 0, HP*9.5, HP*18);
		childLight(Celesta::SEQ_B_7_LED_LIGHT, 0, HP*9.5, HP*20.5);
		childLight(Celesta::SEQ_B_8_LED_LIGHT, 0, HP*9.5, HP*23);
		childLight(Celesta::SEQ_C_1_LED_LIGHT, 0, HP*14, HP*5.5);
		childLight(Celesta::SEQ_C_2_LED_LIGHT, 0, HP*14, HP*8);
		childLight(Celesta::SEQ_C_3_LED_LIGHT, 0, HP*14, HP*10.5);
		childLight(Celesta::SEQ_C_4_LED_LIGHT, 0, HP*14, HP*13);
		childLight(Celesta::SEQ_C_5_LED_LIGHT, 0, HP*14, HP*15.5);
		childLight(Celesta::SEQ_C_6_LED_LIGHT, 0, HP*14, HP*18);
		childLight(Celesta::SEQ_C_7_LED_LIGHT, 0, HP*14, HP*20.5);
		childLight(Celesta::SEQ_C_8_LED_LIGHT, 0, HP*14, HP*23);
	}

	// menu for basic quantization
	void appendContextMenu(Menu* menu) override {
		Celesta* module = dynamic_cast<Celesta*>(this->module);
		assert(module);
		menu->addChild(new MenuSeparator);
		menu->addChild(createIndexPtrSubmenuItem("Quantize", {"Nope","Octaves","Notes"}, &module->indexQuant));
	}

	// shortkey
	void onHoverKey(const event::HoverKey &e) override {
		if (e.key >= GLFW_KEY_1 && e.key <= GLFW_KEY_8) {
			/* if (e.action == GLFW_PRESS) {
				float key_number = e.key - 49; // 49 is the ascii number for key #1
				module->keyKnob(key_number);
				e.consume(this);
			}*/
		}
		// else if(e.key == GLFW_KEY_G && e.action == GLFW_PRESS) {module->modeGate=true; e.consume(this);}
		// else if(e.key == GLFW_KEY_V && e.action == GLFW_PRESS) {module->modeGate=false; e.consume(this);}
		else if(e.key == GLFW_KEY_R && e.action == GLFW_PRESS) {module->rndKnob(); e.consume(this);}
		else if(e.key == GLFW_KEY_I && e.action == GLFW_PRESS) {module->invKnob(); e.consume(this);}
		// else if(e.key == GLFW_KEY_U && e.action == GLFW_PRESS) {module->supKnob(); e.consume(this);}
		// else if(e.key == GLFW_KEY_D && e.action == GLFW_PRESS) {module->sdnKnob(); e.consume(this);}
		// else if(e.key == GLFW_KEY_F && e.action == GLFW_PRESS) {module->freezCv=2; e.consume(this);}
		ModuleWidget::onHoverKey(e);
	}

};

Model* modelCelesta = createModel<Celesta, CelestaWidget>("Celesta");
