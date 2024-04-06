// in cooperatin with ChatGPT 4 on 2023-12-30

// #include "rack.hpp"
#include "plugin.hpp"

using namespace rack;

struct SimpleLFO : Module {
    enum ParamIds {
		FREQ_A_PARAM,
		FREQ_B_PARAM,
		FREQ_C_PARAM,
        MAX_PARAM,
        MIN_PARAM,
		WAVESHAPE_PARAM,
		PARAMS_LEN
    };
    enum InputId  { RESET_SYNC_INPUT, INPUTS_LEN };
    enum OutputIds { VAR_A_OUTPUT, VAR_B_OUTPUT, VAR_C_OUTPUT, OUTPUTS_LEN };
    enum LightId  { LIGHTS_LEN };
    
    float phaseA = 0.f;
    float phaseB = 0.f;
    float phaseC = 0.f;

    SimpleLFO() {
		
        config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
        configParam(FREQ_A_PARAM, 0.001f, 20.f, 16.25f, "Frequency", " Hz");
        configParam(FREQ_B_PARAM, 0.001f, 20.f, 12.50f, "Frequency", " Hz");
        configParam(FREQ_C_PARAM, 0.001f, 20.f, 02.01f, "Frequency", " Hz");
        configParam(MAX_PARAM, -10.f, 10.f, 5.f, "Max Voltage");
        configParam(MIN_PARAM, -10.f, 10.f, -5.f, "Min Voltage");
        configParam(WAVESHAPE_PARAM, 0.f, 1.f, 0.f, "Bleeding");

		paramQuantities[WAVESHAPE_PARAM]->description = "Wave wiggler";
		getInputInfo(RESET_SYNC_INPUT)->description ="Reset trigger";
		getOutputInfo(VAR_A_OUTPUT)->description="Saw > Reverse saw";
		getOutputInfo(VAR_B_OUTPUT)->description="Reverse saw > Cosinus";
		getOutputInfo(VAR_C_OUTPUT)->description="Cosine > Saw";

    }

	enum state {ON, OFF, NA};

	int oldReset=ON;
	int newReset=OFF;
	float freqA = 25;
	float freqB = 25;
	float freqC = 25;
	float maxVoltage = 5;
	float minVoltage = -5;
	float waveshape = 0;
	bool syncIn=false;

	int indexRange=0; 
	// if 0 then 0.01Hz to 52Hz
	// if 1 then 0.001Hz to 17.5Hz
	// if 2 then 1Hz to 220Hz

void process(const ProcessArgs &args) override {

    static int processDivider = 0;
    if (processDivider++ % 256 == 0) {  // More responsive update rate
        freqA = params[FREQ_A_PARAM].getValue();
        freqB = params[FREQ_B_PARAM].getValue();
        freqC = params[FREQ_C_PARAM].getValue();
		// Adjust frequency based on indexRange
		switch (indexRange) {
			case 1: // 20Hz to 440Hz
				freqA = rescale(freqA, 0.001f, 20.f, 20.f, 440.f);
				freqB = rescale(freqB, 0.001f, 20.f, 20.f, 440.f);
				freqC = rescale(freqC, 0.001f, 20.f, 20.f, 440.f);
				paramQuantities[FREQ_A_PARAM]->displayOffset = 20.f - (paramQuantities[FREQ_A_PARAM]->displayMultiplier * 0.001f);
				paramQuantities[FREQ_B_PARAM]->displayOffset = 20.f - (paramQuantities[FREQ_A_PARAM]->displayMultiplier * 0.001f);
				paramQuantities[FREQ_C_PARAM]->displayOffset = 20.f - (paramQuantities[FREQ_A_PARAM]->displayMultiplier * 0.001f);
				paramQuantities[FREQ_A_PARAM]->displayMultiplier = 20;
				paramQuantities[FREQ_B_PARAM]->displayMultiplier = 20;
				paramQuantities[FREQ_C_PARAM]->displayMultiplier = 20;
				break;
			case 2: // 0.001Hz to 10Hz
				freqA = rescale(freqA, 0.001f, 20.f, 0.001f, 10.f);
				freqB = rescale(freqB, 0.001f, 20.f, 0.001f, 10.f);
				freqC = rescale(freqC, 0.001f, 20.f, 0.001f, 10.f);
				paramQuantities[FREQ_A_PARAM]->displayMultiplier = 0.5f;
				paramQuantities[FREQ_B_PARAM]->displayMultiplier = 0.5f;
				paramQuantities[FREQ_C_PARAM]->displayMultiplier = 0.5f;
				break;
			default: // 0.001Hz to 20Hz
				paramQuantities[FREQ_A_PARAM]->displayMultiplier = 1.f;
				paramQuantities[FREQ_B_PARAM]->displayMultiplier = 1.f;
				paramQuantities[FREQ_C_PARAM]->displayMultiplier = 1.f;
				break;
		}
        maxVoltage = params[MAX_PARAM].getValue();
        minVoltage = params[MIN_PARAM].getValue();
        waveshape = params[WAVESHAPE_PARAM].getValue();
        syncIn = inputs[RESET_SYNC_INPUT].isConnected();
		processDivider = 0;
    }
    
    // Reset/Sync Logic
	newReset=(inputs[RESET_SYNC_INPUT].getVoltage() < 2.f?OFF:ON);
    if (syncIn && oldReset==OFF && newReset==ON) {
        phaseA = phaseB = phaseC = 0.f;
		oldReset=ON;
    } else {
        float deltaTime = args.sampleTime;
        phaseA = fmodf(phaseA + deltaTime * freqA, 1.f);
        phaseB = fmodf(phaseB + deltaTime * freqB, 1.f);
        phaseC = fmodf(phaseC + deltaTime * freqC, 1.f);
	}
	oldReset=newReset;
	
    // Calculate waveforms once
	
    float waveA = 2.f * phaseA - 1.f;	// Saw
	float waveB = -2.f * phaseB + 1.f;	// Inverted Saw
	float waveC = cosf(2 * M_PI * phaseC);

    // Output voltage calculations
    outputs[VAR_A_OUTPUT].setVoltage(rescale(waveB*waveshape+(1-waveshape)*waveA, -1.f, 1.f, minVoltage, maxVoltage));
    outputs[VAR_B_OUTPUT].setVoltage(rescale(waveC*waveshape+(1-waveshape)*waveB, -1.f, 1.f, minVoltage, maxVoltage));
    outputs[VAR_C_OUTPUT].setVoltage(rescale(waveA*waveshape+(1-waveshape)*waveC, -1.f, 1.f, minVoltage, maxVoltage));

}

	// this block is to save and reload a variable
	json_t* dataToJson() override {
	json_t* rootJ = json_object();
	json_object_set_new(rootJ, "range", json_integer(indexRange));
	return rootJ;}

	void dataFromJson(json_t* rootJ) override {
	json_t *rangeJ = json_object_get(rootJ, "range");
	if (rangeJ) indexRange = json_integer_value(rangeJ);
	};

};

struct SimpleLFOWidget : ModuleWidget {
	
	// SimpleLFO* module;
	
	const float HP=5.08;

    SimpleLFOWidget(SimpleLFO *module) {

        setModule(module);
        // setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/SimpleLFO.svg")));
		setPanel(createPanel(asset::plugin(pluginInstance, "res/SimpleLFO.svg")));

        addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(HP*1, HP*2.5)), module, SimpleLFO::WAVESHAPE_PARAM));

        addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(HP*1, HP*5)), module, SimpleLFO::FREQ_A_PARAM));	
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(HP*1, HP*7)), module, SimpleLFO::VAR_A_OUTPUT));

        addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(HP*1, HP*9.5)), module, SimpleLFO::FREQ_B_PARAM));	
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(HP*1, HP*11.5)), module, SimpleLFO::VAR_B_OUTPUT));

        addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(HP*1, HP*14.0)), module, SimpleLFO::FREQ_C_PARAM));	
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(HP*1, HP*16.0)), module, SimpleLFO::VAR_C_OUTPUT));
		
        addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(HP*1, HP*18.5)), module, SimpleLFO::MAX_PARAM));
        addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(HP*1, HP*20.5)), module, SimpleLFO::MIN_PARAM));
		
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(HP*1, HP*23.0)), module, SimpleLFO::RESET_SYNC_INPUT));

    }

	void appendContextMenu(Menu* menu) override {
		SimpleLFO* module = dynamic_cast<SimpleLFO*>(this->module);
		if (!module) return; // Safely exit if the cast failed	
		assert(module);
		menu->addChild(new MenuSeparator);
		menu->addChild(createIndexPtrSubmenuItem("Frequency range", 
			{"0.001Hz to 20Hz","20Hz to 440Hz","0.001Hz to 10Hz"}, 
			&module->indexRange));
	}
	
};


Model *modelSimpleLFO = createModel<SimpleLFO, SimpleLFOWidget>("SimpleLFO");
