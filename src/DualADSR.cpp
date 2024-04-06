// Copyright (c) 2024 András Szabó
#include "plugin.hpp"

struct DualADSR : Module {

// --------------------------------------------------

	enum ParamId  {
		LEFT_ATTACK_PARAM, LEFT_DECAY_PARAM, LEFT_SUSTAIN_PARAM, LEFT_RELEASE_PARAM, 
		RIGHT_ATTACK_PARAM, RIGHT_DECAY_PARAM, RIGHT_SUSTAIN_PARAM, RIGHT_RELEASE_PARAM, 
		LEFT_GAIN_PARAM, RIGHT_GAIN_PARAM, 
		PARAMS_LEN};

	enum InputId  {
		LEFT_AUDIO_IN_INPUT, LEFT_GATE_INPUT, 
		RIGHT_AUDIO_IN_INPUT, RIGHT_GATE_INPUT, 
		INPUTS_LEN};

	enum OutputId {
		LEFT_AUDIO_OUT_OUTPUT, LEFT_ENVELOPE_OUTPUT, 
		RIGHT_AUDIO_OUT_OUTPUT, RIGHT_ENVELOPE_OUTPUT, 
		OUTPUTS_LEN};

	enum LightId  {LIGHTS_LEN};

	// small assistance to save older values for reference;
	float paramVal[PARAMS_LEN]={0};
	// float inputVolt[INPUTS_LEN]={0};
	// float lightVal[LIGHTS_LEN]={0};

// --------------------------------------------------

	DualADSR() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(LEFT_ATTACK_PARAM, 	0.0f, 0.5f, 0.16f, "Left Attack");
		configParam(LEFT_DECAY_PARAM, 	0.0f, 1.0f, 0.5f, "Left Decay");
		configParam(LEFT_SUSTAIN_PARAM, 	0.0f, 1.0f, 0.5f, "Left Sustain");
		configParam(LEFT_RELEASE_PARAM, 	0.0f, 1.0f, 0.5f, "Left Release");
		configParam(RIGHT_ATTACK_PARAM, 	0.0f, 1.0f, 0.5f, "Right Attack");
		configParam(RIGHT_DECAY_PARAM, 	0.0f, 1.0f, 0.5f, "Right Decay");
		configParam(RIGHT_SUSTAIN_PARAM, 	0.0f, 1.0f, 0.5f, "Right Sustain");
		configParam(RIGHT_RELEASE_PARAM, 	0.0f, 1.0f, 0.5f, "Right Release");
		configParam(LEFT_GAIN_PARAM, 	0.0f, 1.0f, 1.0f, "Left Gain");
		configParam(RIGHT_GAIN_PARAM, 	0.0f, 1.0f, 1.0f, "Right Gain");
		paramQuantities[LEFT_GAIN_PARAM]->randomizeEnabled = false;
		paramQuantities[RIGHT_GAIN_PARAM]->randomizeEnabled = false;

		configInput(LEFT_AUDIO_IN_INPUT, "Left Audio In"); 
		configInput(LEFT_GATE_INPUT, "Left Gate"); 
		configInput(RIGHT_AUDIO_IN_INPUT, "Right Audio In"); 
		configInput(RIGHT_GATE_INPUT, "Right Gate"); 

		configOutput(LEFT_AUDIO_OUT_OUTPUT, "Left Audio Out"); 
		configOutput(LEFT_ENVELOPE_OUTPUT, "Left Envelope"); 
		configOutput(RIGHT_AUDIO_OUT_OUTPUT, "Right Audio Out"); 
		configOutput(RIGHT_ENVELOPE_OUTPUT, "Right Envelope"); 
	}

// --------------------------------------------------

	int loop=0;     // save some CPU in process()

	float currentGate[4]={0.f,0.f,0.f,0.f}; // left old, left new, right old, right new
	int cycleCount[2]={0,0};
	int statusLeftGate=0;	// saving the current status of the gate input
	int statusRightGate=0;	// saving the current status of the gate input

	enum gStatus {
		LOW_LOW,    // 0: Gate was low, now low
		LOW_HIGH,   // 1: Gate was low, now high
		HIGH_HIGH,  // 2: Gate was high, now high
		HIGH_LOW    // 3: Gate was high, now low
	};
	
	int statusLeftEnv=0;
	int statusRightEnv=0;
	
	enum eStatus {
		OFF,
		ATTACK,
		DECAY,
		SUSTAIN,
		RELEASE
	};

	float envLeft=0.f;
	float envRight=0.f;

	float audioLeft=0.f;
	float audioRight=0.f;
	
	float attackRate=1;	float decayRate=1;	float releaseRate=1; float sustain=1; 

	void process(const ProcessArgs& args) override {

		// occaisonly read the parameter knobs
		if (loop--<=0) {
			loop=8901;
			for (int p=0;p<PARAMS_LEN;p++) {paramVal[p]=params[p].getValue();}
		}

		// let's see the clock signal
		currentGate[1]=inputs[LEFT_GATE_INPUT].getVoltage();		
		if (currentGate[1]>2.0f && currentGate[0]<=2.0f) {statusLeftGate=LOW_HIGH; statusLeftEnv=ATTACK; cycleCount[0]=1;}
		// else if (currentGate[1]>2.0f && currentGate[0]>2.0f) {statusLeftGate=HIGH_HIGH;}
		// else if (currentGate[1]<=2.0f && currentGate[0]<=2.0f) {statusLeftGate=LOW_LOW;}
		else if (currentGate[1]<=2.0f && currentGate[0]>2.0f) {statusLeftGate=HIGH_LOW; statusLeftEnv=RELEASE; cycleCount[0]=1;}
		currentGate[0]=currentGate[1];	// save the left gate value for later
		cycleCount[0]++;
		
		// step calculations
		sustain = paramVal[LEFT_SUSTAIN_PARAM];
		attackRate = 1.0f - powf(0.01f, 1.0f / (paramVal[LEFT_ATTACK_PARAM] * 5.f * args.sampleRate));
		decayRate = 1.0f - powf(0.01f, 1.0f / (paramVal[LEFT_DECAY_PARAM] * 5.f * args.sampleRate));
		releaseRate = 1.0f - powf(0.01f, 1.0f / (paramVal[LEFT_RELEASE_PARAM] * 5.f * args.sampleRate));
    
// decayRate = 1.0f / cycleCount[0];
// decayRate = 1.0f / powf(cycleCount[0],2.f);
// decayRate = (1.f - paramVal[LEFT_RELEASE_PARAM]) / powf(cycleCount[0],2.f);
// decayRate = (1.f - paramVal[LEFT_RELEASE_PARAM]) / powf(cycleCount[0],sustain);
// decayRate = 1.0f - powf(sustain, 1.0f / cycleCount[0] * (paramVal[LEFT_DECAY_PARAM] * 5.f * args.sampleRate));
// decayRate = 1.f - 1.f / paramVal[LEFT_RELEASE_PARAM] * cycleCount[0];
// decayRate = 1.0f - powf(0.01f, 1.0f / (paramVal[LEFT_DECAY_PARAM] * 5.f * args.sampleRate) * (1.0f - 0.5f) + 0.5f  * cycleCount[0]);
// decayRate = 1.0f / (cycleCount[0] * args.sampleRate * (1.0f - sustain) + sustain);
// decayRate = 1.0f / (1.0f + exp(-0.5f * (cycleCount[0] - 1))) * (1.0f - sustain) + sustain;
// decayRate = 1.0f - 1.0f / cycleCount[0];
// decayRate = 1.0f / (cycleCount[0] / paramVal[LEFT_DECAY_PARAM]);
decayRate = 1.0f / (cycleCount[0] / (1.0f-paramVal[LEFT_DECAY_PARAM]));
 
// ALL cycleCount may need to be REMOVED!!!		
		
		// envelope calculation
		switch (statusLeftEnv) {
			case OFF: break;	// just relax
			case ATTACK: 
				envLeft+=attackRate;
				if (envLeft>=1.f) {envLeft=1.f; statusLeftEnv=DECAY; cycleCount[0]=1;}
				break;
			case DECAY: 
				envLeft-=decayRate;
				if (envLeft<=sustain) {envLeft=sustain; statusLeftEnv=SUSTAIN;}
				else if (envLeft<=0) {envLeft=0.f; statusLeftEnv=OFF;}
				break;
			case SUSTAIN: 
				break; // just wait for HIGH_LOW to set the RELEASE 
			case RELEASE: 
				envLeft-=releaseRate;
				if (envLeft<=0.f) {envLeft=0.f; statusLeftEnv=OFF;}
				break;
		}

		// output calculation
		audioLeft=inputs[LEFT_AUDIO_IN_INPUT].getVoltage()*paramVal[LEFT_GAIN_PARAM]*envLeft;

		outputs[LEFT_ENVELOPE_OUTPUT].setVoltage(envLeft*10*paramVal[LEFT_GAIN_PARAM]);
		outputs[LEFT_AUDIO_OUT_OUTPUT].setVoltage(audioLeft);

		// let's see the clock signal
		if (inputs[RIGHT_GATE_INPUT].isConnected()==false) {currentGate[3]=currentGate[1];} 
		else {currentGate[3]=inputs[RIGHT_GATE_INPUT].getVoltage();}
		if (currentGate[3]>2.0f && currentGate[2]<=2.0f) {statusRightGate=LOW_HIGH; statusRightEnv=ATTACK; cycleCount[1]=1;}
		// else if (currentGate[3]>2.0f && currentGate[2]>2.0f) {statusRightGate=HIGH_HIGH;}
		// else if (currentGate[3]<=2.0f && currentGate[2]<=2.0f) {statusRightGate=LOW_LOW;}
		else if (currentGate[3]<=2.0f && currentGate[2]>2.0f) {statusRightGate=HIGH_LOW; statusRightEnv=RELEASE; cycleCount[1]=1;}
		currentGate[2]=currentGate[3];	// save the right gate value for later
		cycleCount[1]++;

		// step calculations
		sustain = paramVal[RIGHT_SUSTAIN_PARAM];
		attackRate = 1.0f - powf(0.01f, 1.0f / (paramVal[RIGHT_ATTACK_PARAM] * 5.f * args.sampleRate));
		decayRate = 1.0f - powf(0.01f, 1.0f / (paramVal[RIGHT_DECAY_PARAM] * 5.f * args.sampleRate));
		releaseRate = 1.0f - powf(0.01f, 1.0f / (paramVal[RIGHT_RELEASE_PARAM] * 5.f * args.sampleRate));

		// envelope calculation
		switch (statusRightEnv) {
			case OFF: break;	// just relax
			case ATTACK: 
				envRight+=attackRate;
				if (envRight>=1.f) {envRight=1.f; statusRightEnv=DECAY; cycleCount[1]=1;}
				break;
			case DECAY: 
				envRight-=decayRate;
				if (envRight<=sustain) {envRight=sustain; statusRightEnv=SUSTAIN;}
				else if (envRight<=0) {envRight=0.f; statusRightEnv=OFF;}
				break;
			case SUSTAIN: 
				break; // just wait for HIGH_LOW to set the RELEASE 
			case RELEASE: 
				envRight-=releaseRate;
				if (envRight<=0.f) {envRight=0.f; statusRightEnv=OFF;}
				break;
		}


		// output calculation	
		if (inputs[RIGHT_AUDIO_IN_INPUT].isConnected()==false) {audioRight=inputs[LEFT_AUDIO_IN_INPUT].getVoltage()*envRight*paramVal[RIGHT_GAIN_PARAM];}
		else {audioRight=inputs[RIGHT_AUDIO_IN_INPUT].getVoltage()*envRight*paramVal[RIGHT_GAIN_PARAM];}
		
		outputs[RIGHT_ENVELOPE_OUTPUT].setVoltage(envRight*10*paramVal[RIGHT_GAIN_PARAM]);
		outputs[RIGHT_AUDIO_OUT_OUTPUT].setVoltage(audioRight);

	}

	// #include "DualADSR/DualADSR_json.hpp"

};

// --------------------------------------------------

struct DualADSRWidget : ModuleWidget {

	DualADSR* module;
	#include "share/widgetwizard.hpp"   // addChild abbreviations

	DualADSRWidget(DualADSR* module) {

		this->module = module;
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/DualADSR.svg")));

		// addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		
		childKnob(DualADSR::LEFT_ATTACK_PARAM,		0, HP*1, HP*4.5);
		childKnob(DualADSR::LEFT_DECAY_PARAM,		0, HP*1, HP*7.0);
		childKnob(DualADSR::LEFT_SUSTAIN_PARAM,		0, HP*1, HP*9.5);
		childKnob(DualADSR::LEFT_RELEASE_PARAM,		0, HP*1, HP*12.0);
		childKnob(DualADSR::RIGHT_ATTACK_PARAM,		0, HP*3, HP*4.5);
		childKnob(DualADSR::RIGHT_DECAY_PARAM,		0, HP*3, HP*7.0);
		childKnob(DualADSR::RIGHT_SUSTAIN_PARAM,	0, HP*3, HP*9.5);
		childKnob(DualADSR::RIGHT_RELEASE_PARAM,	0, HP*3, HP*12.0);

		childInput(DualADSR::LEFT_GATE_INPUT, HP*1, HP*14);
		childInput(DualADSR::RIGHT_GATE_INPUT, HP*3, HP*14);

		childOutput(DualADSR::LEFT_ENVELOPE_OUTPUT, HP*1, HP*16);
		childOutput(DualADSR::RIGHT_ENVELOPE_OUTPUT, HP*3, HP*16);

		childKnob(DualADSR::LEFT_GAIN_PARAM, 1, HP*1, HP*18.5);
		childKnob(DualADSR::RIGHT_GAIN_PARAM, 1, HP*3, HP*18.5);
		
		childInput(DualADSR::LEFT_AUDIO_IN_INPUT, HP*1, HP*21);
		childInput(DualADSR::RIGHT_AUDIO_IN_INPUT, HP*3, HP*21);
		childOutput(DualADSR::LEFT_AUDIO_OUT_OUTPUT, HP*1, HP*23);
		childOutput(DualADSR::RIGHT_AUDIO_OUT_OUTPUT, HP*3, HP*23);

		childLabel(HP*1,HP*1, "DEV", 12);
	}

	// #include "DualADSR/DualADSR_menu.hpp"

	// #include "DualADSR/DualADSR_keys.hpp"

};

Model* modelDualADSR = createModel<DualADSR, DualADSRWidget>("DualADSR");
