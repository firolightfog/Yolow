// Copyright (c) 2022 András Szabó
#include "plugin.hpp"

struct Enveloop : Module {

// --------------------------------------------------

	enum ParamId    {
		ATTACK_START_PARAM, ATTACK_SLEW_PARAM, DECAY_SLEW_PARAM, RELEASE_SLEW_PARAM, NOISE_MOD_PARAM, 
		WAVE_SIZE_PARAM, PARAMS_LEN};

	enum InputId    {
		GATE_INPUT, AUDIO_INPUT_INPUT, NOISE_INPUT_INPUT, INPUTS_LEN};

	enum OutputId    {
		AUDIO_OUTPUT_OUTPUT, ENVELOPE_OUTPUT, OUTPUTS_LEN};

	enum LightId    {
		TEST_LIGHT, ATTACK_LED_LIGHT, DECAY_LED_LIGHT, RELEASE_LED_LIGHT, LIGHTS_LEN};

	// small assistance to save older values for reference;
	float paramVal[PARAMS_LEN]={0};
	float inputVolt[INPUTS_LEN]={0};
	float lightVal[LIGHTS_LEN]={0};

// --------------------------------------------------

	Enveloop() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(ATTACK_START_PARAM, 	0.0f, 10.0f, 0.0f, "Attack start");
		configParam(ATTACK_SLEW_PARAM, 		0.0f, 1.0f, 0.5f, "Attack slew");
		configParam(DECAY_SLEW_PARAM, 		0.0f, 1.0f, 0.2f, "Decay slew");
		configParam(RELEASE_SLEW_PARAM, 	0.0f, 1.0f, 0.8f, "Release slew");
		configParam(NOISE_MOD_PARAM, 		0.0f, 1.0f, 0.1f, "Noise mod");
		paramQuantities[ATTACK_START_PARAM]->snapEnabled = true;

		configParam(WAVE_SIZE_PARAM, 	0.0f, 2.0f, 1.0f, "Wave size");
		paramQuantities[WAVE_SIZE_PARAM]->snapEnabled = true;

		configInput(GATE_INPUT, "Gate"); 
		configInput(AUDIO_INPUT_INPUT, "Audio"); 
		configInput(NOISE_INPUT_INPUT, "Noise"); 

		configOutput(AUDIO_OUTPUT_OUTPUT, "Clean env || Audio"); 
		configOutput(ENVELOPE_OUTPUT, "Envelope"); 
	}

// --------------------------------------------------

	int loop=0;     // save some CPU in process()
	bool isGateIn=false;
	bool isNoiseIn=false;
	bool isAudioIn=false;
	int currState=0;
	float envDist=0.0f;
	float envVolt=0.0f;
	float audVolt=0.0f;
	
	const int ON=1;
	const int OFF=-1;
	const int SAME=0;
	int currGate=OFF;
	float oldGate=0.0f;
	float newGate=0.0f;
	float curveType=0.5f;	
	int waveSize=500;
	int indexGateMode=0;
	
	void process(const ProcessArgs& args) override {

		if (loop--<=0) {
			loop=9000;
			// save some more CPU
			isGateIn=inputs[GATE_INPUT].isConnected();
			isNoiseIn=inputs[NOISE_INPUT_INPUT].isConnected();
			isAudioIn=inputs[AUDIO_INPUT_INPUT].isConnected();
			for (int p=0;p<PARAMS_LEN;p++) {
				paramVal[p]=params[p].getValue();
			}
			if (paramVal[WAVE_SIZE_PARAM]==0) {waveSize=250;}
			else if (paramVal[WAVE_SIZE_PARAM]==1) {waveSize=500;}
			else if (paramVal[WAVE_SIZE_PARAM]==2) {waveSize=4000;}
		}

		// check if new GATE is on
		if (isGateIn) {
			newGate=inputs[GATE_INPUT].getVoltage();
			if (newGate>2) {
				if (oldGate>2) {currGate=SAME;}
				else {
					// audVolt=rack::random::uniform()*10*paramVal[NOISE_MOD_PARAM];
					currGate=ON; 
					currState=1; 
					enlightenMe(currState);
					envVolt=paramVal[ATTACK_START_PARAM];
					}
			}
			else {
				if (oldGate>2) {
					currGate=OFF; 
					currState=3;
					enlightenMe(currState);
					}
				else {currGate=SAME;}
			}
			oldGate=newGate;
		}

		// lights[TEST_LIGHT].setBrightness((currState<2)?1:0);	// no need for this one
							
// itt az ATTACK_START_PARAM akár A és D gömbölyítonek is használható lenne 

		// acting based on the phase we're in 
		if (currState==1) {	// attack state
			envVolt+=(1-paramVal[ATTACK_SLEW_PARAM])/waveSize;
			if (envVolt>=10) {envVolt=10; currState=2; enlightenMe(currState);}
			else if (envVolt<0) {envVolt=0;}
		}
		else if (currState==2) {	// decay state
			envVolt-=(1-paramVal[DECAY_SLEW_PARAM])/waveSize;
			if (envVolt<=paramVal[ATTACK_START_PARAM]) {
				envVolt=paramVal[ATTACK_START_PARAM];
				currState=3;
				enlightenMe(currState);
			}
		}
		else if (currState==3) {	// suspension state
			envVolt-=(1-paramVal[RELEASE_SLEW_PARAM])/waveSize;
			if (envVolt<=0) {envVolt=0; currState=(isGateIn && indexGateMode==0)?3:1; enlightenMe(currState);}
		}
		else if (currState==0) { // happens only once: at the beginning
			envVolt=paramVal[ATTACK_START_PARAM]; currState=1; enlightenMe(currState);
		}

		// modifying the envelope with the new noise
		envDist=envVolt;
		if (!isNoiseIn) {/*envDist=10-envVolt;*/
			/*envDist=envVolt-(rack::random::normal()*paramVal[NOISE_MOD_PARAM])*envVolt/10; */}
		else {envDist=envVolt-inputs[NOISE_INPUT_INPUT].getVoltage()*paramVal[NOISE_MOD_PARAM]*envVolt/10;}

		// modifying the audio with the new envelope
		if (!isAudioIn) {audVolt=envVolt;}
		else {audVolt=inputs[AUDIO_INPUT_INPUT].getVoltage()*envDist/10;}
		
		// finally I send it to the output
		outputs[ENVELOPE_OUTPUT].setVoltage(envDist);
		outputs[AUDIO_OUTPUT_OUTPUT].setVoltage(audVolt);

	}

	void enlightenMe(int theState) {
		lights[ATTACK_LED_LIGHT].setBrightness((theState==1)?1:0);
		lights[DECAY_LED_LIGHT].setBrightness((theState==2)?1:0);
		lights[RELEASE_LED_LIGHT].setBrightness((theState==3)?1:0);		
	}

	// this JSON block is to save and reload a variable
	json_t* dataToJson() override {
	json_t* rootJ = json_object();
	json_object_set_new(rootJ, "gatemode", json_integer(indexGateMode));
	return rootJ;}

	void dataFromJson(json_t* rootJ) override {
	json_t *gatemodeJ = json_object_get(rootJ, "gatemode");
	if (gatemodeJ) indexGateMode = json_integer_value(gatemodeJ);}

// --------------------------------------------------

};

// --------------------------------------------------

struct EnveloopWidget : ModuleWidget {

	Enveloop* module;
	#include "share/widgetwizard.hpp"   // addChild abbreviations

	EnveloopWidget(Enveloop* module) {

		this->module = module;
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/Enveloop.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		
		childKnob(Enveloop::ATTACK_START_PARAM, 1, HP*1.5, HP*5);
		childSwitch(Enveloop::WAVE_SIZE_PARAM, 1, HP*3, HP*5);		
		childKnob(Enveloop::ATTACK_SLEW_PARAM, 2, HP*2, HP*8);
		childKnob(Enveloop::DECAY_SLEW_PARAM, 2, HP*2, HP*11.5);
		childKnob(Enveloop::RELEASE_SLEW_PARAM, 2, HP*2, HP*15);
		
		childKnob(Enveloop::NOISE_MOD_PARAM, 1, HP*3, HP*18.5);
		childInput(Enveloop::GATE_INPUT, HP*1, HP*18.5);
		childInput(Enveloop::AUDIO_INPUT_INPUT, HP*1, HP*20.5);
		childOutput(Enveloop::AUDIO_OUTPUT_OUTPUT, HP*1, HP*22.5);
		childInput(Enveloop::NOISE_INPUT_INPUT, HP*3, HP*20.5);
		childOutput(Enveloop::ENVELOPE_OUTPUT, HP*3, HP*22.5);
		// childLight(Enveloop::TEST_LIGHT, 0, HP*2, HP*19.5);	// no need for this one
		
		childLight(Enveloop::ATTACK_LED_LIGHT, 0, HP*2, HP*8);
		childLight(Enveloop::DECAY_LED_LIGHT, 0, HP*2, HP*11.5);
		childLight(Enveloop::RELEASE_LED_LIGHT, 0, HP*2, HP*15);
	}


	// menu for basic quantization
	void appendContextMenu(Menu* menu) override {
		Enveloop* module = dynamic_cast<Enveloop*>(this->module);
		assert(module);
		menu->addChild(new MenuSeparator);
		menu->addChild(createIndexPtrSubmenuItem("Gate mode", {"Single cycle","Sync only"}, &module->indexGateMode));
	}

};

Model* modelEnveloop = createModel<Enveloop, EnveloopWidget>("Enveloop");
