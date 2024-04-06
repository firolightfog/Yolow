// in cooperatin with ChatGPT 4 on 2023-12-30

// #include "rack.hpp"
#include "plugin.hpp"

using namespace rack;

struct Kcik : Module {
    enum ParamIds {
        TUNE_PARAM,
        DECAY_PARAM,
        PITCH_ENV_DEPTH_PARAM,
        CLICK_LEVEL_PARAM,
        DRIVE_PARAM,
        NUM_PARAMS
    };
    enum InputIds {
        GATE_INPUT,
        VELOCITY_CV_INPUT,
        TUNE_CV_INPUT,
        DECAY_CV_INPUT,
        DRIVE_CV_INPUT,
        NUM_INPUTS
    };
    enum OutputIds {
        AUDIO_OUTPUT,
        NOISE_OUTPUT,
        ENVELOPE_OUTPUT,
        PITCH_ENV_OUTPUT,
        NUM_OUTPUTS
    };

    Kcik() {
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);
        configParam(TUNE_PARAM, 10.f, 440.f, 60.f, "Tune", " Hz");
        configParam(DECAY_PARAM, 0.1f, 10.0f, 0.5f, "Decay", " s");
        configParam(PITCH_ENV_DEPTH_PARAM, 0.f, 100.f, 50.f, "Pitch Env Depth");
        configParam(CLICK_LEVEL_PARAM, 0.0f, 1.f, 0.5f, "Click Level");
        configParam(DRIVE_PARAM, 0.f, 1.f, 0.025f, "Drive");

		// paramQuantities[WAVESHAPE_PARAM]->description = "Wave wiggler";
		getInputInfo(GATE_INPUT)->description ="Gate";
		getInputInfo(VELOCITY_CV_INPUT)->description ="Velocity (0V-10V)";
		getInputInfo(TUNE_CV_INPUT)->description ="Tune";
		getInputInfo(DECAY_CV_INPUT)->description ="Decay (0V-10V)";
		getInputInfo(DRIVE_CV_INPUT)->description ="Drive  (0V-10V)";
		getOutputInfo(AUDIO_OUTPUT)->description="Audio";
		getOutputInfo(ENVELOPE_OUTPUT)->description="Envelope";
		getOutputInfo(PITCH_ENV_OUTPUT)->description="Pitch env.";

    }

/*
	// Soft Clipping Distortion Function
	float applyDistortion(float input, float drive) {
		// Apply drive gain
		input *= drive;

		// Soft clipping
		// The magic numbers here define the shape of the soft clipping curve
		float threshold = 1.f;
		if (input > threshold) {
			input = threshold + (input - threshold) / (1.f + powf((input - threshold) / threshold, 2));
		} else if (input < -threshold) {
			input = -threshold + (input + threshold) / (1.f + powf((input + threshold) / threshold, 2));
		}

		return input;
	}
*/

	// Aggressive Hard Clipping Distortion Function
	float applyDistortion(float input, float drive) {
		// Apply drive gain
		float gain = 1.f + drive * 10.f; // Increased gain for more crunch
		input *= gain;

		// Hard clipping with non-linear shaping
		const float threshold = 0.6f; // Lower threshold for more aggressive clipping
		if (input > threshold) {
			input = threshold + (input - threshold) / (1.f + powf((input - threshold) / (1.f - threshold), 2));
		} else if (input < -threshold) {
			input = -threshold + (input + threshold) / (1.f + powf((input + threshold) / (1.f - threshold), 2));
		}

		return input;
	}

/* 
	float getAmpEnvVal(int status) {
		// Basic for testing
		return (status <= 0) ? 0 : 1;	
	}
 */

/* 
	float getAmpEnvVal(int status) {
		// Simple saw for testing
		static float ampEnv=0;
		switch(status) {
			case 1: ampEnv=1; break;
			default: ampEnv-=0.0001; if (0>ampEnv) {ampEnv=0;}; break;
		}
		return ampEnv;
	}
 */
 /* 
	float getAmpEnvVal(float totalDecayTime, float sampleRate, int gateStatus) {
		// Simple saw for testing with adjustable decay times		
		static float envValue = 0.0f; // Holds the current envelope level.

		// Calculate the rate of decay per sample, which is the amount to decrement each sample to reach 0 after `totalDecayTime` seconds.
		float decayRatePerSample = 1.0f / (totalDecayTime * sampleRate);

		// Reset the envelope when a new gate signal starts.
		if (gateStatus == 1) {envValue = 1.0f;}

		// Apply decay if the gate was high and now is not, or if it continues to be high.
		else {
			envValue -= decayRatePerSample;
			envValue = fmaxf(envValue, 0.0f); // Envelope should not go below 0.
		}
		return envValue;
	}
 */

float getExponentialDecayValue(float deltaTime, float decayTime, float sampleRate, int gateStatus, float theClick) {
    static float envValue = 0.0f; // Holds the current envelope level.

    // Calculate the decay coefficient. This will determine the curve's steepness.
    // A larger coefficient results in a faster decay. Typically, the coefficient
    // is a small value like 0.99, 0.9, etc.
    float decayCoefficient = powf(theClick, deltaTime / decayTime);

    // If a new gate signal starts, reset the envelope to its maximum value.
    if (gateStatus == 1) {envValue = 1.0f;}
    // Apply exponential decay when the gate is not high or has just been released.
    else if (gateStatus == 2 || gateStatus == -1) {
        envValue *= decayCoefficient; // Apply exponential decay.
        envValue = fmaxf(envValue, 0.0f); // Ensure the envelope doesn't go below 0.
    }

    return envValue;
}

float getWickedFinValue(float xParam, float sampleRate, int gateStatus) {
    static float envValue = 0.0f; // Holds the current envelope level.
	static bool attackphase=true;
	static int timeCnt=0;
	
	timeCnt++;
	
    // Prevent division by zero and negative values for xParam
	if (xParam == 0) {return 0;}
	if (xParam < 0) {xParam = abs(xParam);}

    // Update envValue based on gateStatus
    switch (gateStatus) {
        case 1: // LH
			attackphase=true;
			envValue =0.0f;
			timeCnt=0;
            break;
        case 2: // HH
			envValue=1-1/timeCnt*xParam;
            break;
        case -1: // HL
			attackphase=false;
			envValue = 1.0f;
			timeCnt=0;
            break;
        case -2: // LL
			envValue=sampleRate*xParam/timeCnt;
            break;
    }
    return clamp(envValue,0.f,1.f);
}

float getWickedEnvValue(float xParam, float sampleRate, int gateStatus) {
    static float envValue = 0.0f; // Holds the current envelope level.

    switch (gateStatus) {
        case -2: // Decay phase
			envValue -= xParam/1000;
            if (0>envValue) {envValue=0;}
            break;
        case 2: // Attack phase
			envValue += (1-xParam)/1000;
            if (envValue>1) {envValue=1;}
			break;
        case -1: // Gate is low, hold the current value
            break;
        case 1: // Gate just went high, reset the envelope
            envValue = 0.0f;
            break;
    }

    return envValue;
}

/*  
float getWicked1XValue(float xParam, float sampleRate, int gateStatus) {
    static float envValue = 0.0f; // Holds the current envelope level.
    static bool attackPhase = true; // State to track attack and decay phase.
	static float calcChg=0.0f; 

	calcChg++;

	if (gateStatus==1) {attackPhase = true; envValue = 0.0f; calcChg=0.0f;} 	// new clock in
	if (envValue>1) {attackPhase = false;}	// end of attack section
	if (calcChg>sampleRate*10) {return 0;}	// no longer envelope than 10 seconds 
	if (attackPhase) {envValue += xParam/1000;}	// attack section
	else {envValue -= abs(1/(calcChg*xParam));}	// decay section

	envValue=clamp(envValue,0.f,1.f);
    return envValue;
}
 */
	float decayParam =0.5f;
	float pitchParam =0.5f;
	float tuneParam =0.5f;
	float click =0.01f;
	float driveParam =0.025f;
	
	float gateOld=0;
	int gateStatus=0;
	
    void process(const ProcessArgs &args) override {
        // Placeholder for kick drum sound generation
        // This will be replaced with actual sound generation logic

		// Save some CPU on reading parameters
		static int processDivider = 0;
		if (processDivider++ % 256 == 0) {  // More responsive update rate
			decayParam = params[DECAY_PARAM].getValue();
			pitchParam = params[PITCH_ENV_DEPTH_PARAM].getValue();
			tuneParam = params[TUNE_PARAM].getValue();
			click = params[CLICK_LEVEL_PARAM].getValue();
			driveParam = params[DRIVE_PARAM].getValue();
			processDivider = 0;
		}

		// Read gate and parameters for the envelope
		float gateValue = inputs[GATE_INPUT].getVoltage();
		float decayCV = inputs[DECAY_CV_INPUT].isConnected() ? inputs[DECAY_CV_INPUT].getVoltage() / 10.f : 0.f;
		float driveCV = inputs[DRIVE_CV_INPUT].isConnected() ? inputs[DRIVE_CV_INPUT].getVoltage() / 10.0f : 0.f;
		float tuneCV = inputs[TUNE_CV_INPUT].isConnected() ? inputs[TUNE_CV_INPUT].getVoltage() : 0.0f;
 
		// Combine parameter and CV for final tuning value
		// Assuming a standard 1V/Oct input, where 1V increase corresponds to a doubling of frequency
		// tuneParam is in Hz, so you need to convert the CV to a multiplier
		// For example, if tuneCV is 1V and tuneParam is 440Hz, the result should be 880Hz
		// In this example, we map a 10V range to a 4 octave range (a factor of 16 in frequency)
		float frequencyMultiplier = powf(2.0f, tuneCV / 5.0f); // 5V per octave if using a 10V range for 4 octaves
		float finalFrequency = tuneParam * frequencyMultiplier;
		// Calculate pitch increment per sample
		float pitchIncrement = finalFrequency / args.sampleRate;

		static float phase = 0.f;  // Phase accumulator

		// gateStatus: 1 just up, -1 just down
		if (gateValue>2) {
			if (gateOld>2) {gateStatus=2;} else {gateStatus=1;}
		}
		else {
			if (gateOld>2) {gateStatus=-1;} else {gateStatus=-2;}	
		}
		gateOld=gateValue;

		// Check gate input and update phase
		if (gateStatus==1) {phase = 0.f;}  // Reset phase on gate trigger

		// Generate sine wave
		float ampEnv = getExponentialDecayValue(args.sampleTime, decayParam + decayCV, args.sampleRate, gateStatus,click);
		// float ampEnv=getAmpEnvVal(decayParam+decayCV, args.sampleRate, gateStatus);
		// float ampEnv=getAmpEnvVal(gateStatus);
		
		if (ampEnv>0) {
			phase += pitchIncrement;
			if (phase >= 1.f) phase -= 1.f;  // Keep phase in [0, 1)
			float sineWave = sinf(2.f * M_PI * phase);  // Generate sine wave
			sineWave = applyDistortion(sineWave, clamp(driveParam + driveCV, 0.f, 1.f));
			sineWave *= ampEnv;
			outputs[AUDIO_OUTPUT].setVoltage(5.f * sineWave);
		} else {
			outputs[AUDIO_OUTPUT].setVoltage(0.f);  // Output zero if no gate signal
		}

		// Envelope outputs (placeholders for now)
		outputs[ENVELOPE_OUTPUT].setVoltage(ampEnv*10);
		
		outputs[PITCH_ENV_OUTPUT].setVoltage(getWickedEnvValue(click, args.sampleRate, gateStatus)*10);

    }
};

struct KcikWidget : ModuleWidget {

	Kcik *module;
	const float HP=5.08f;

    KcikWidget(Kcik *module) {
        setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/Kcik.svg")));

		addParam(createParamCentered<RoundHugeBlackKnob>(mm2px(Vec(HP*2, HP*5)), module, Kcik::TUNE_PARAM));
		addParam(createParamCentered<RoundHugeBlackKnob>(mm2px(Vec(HP*6, HP*5)), module, Kcik::DECAY_PARAM));

        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(HP*2, HP*8)), module, Kcik::TUNE_CV_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(HP*6, HP*8)), module, Kcik::DECAY_CV_INPUT));

		addParam(createParamCentered<RoundLargeBlackKnob>(mm2px(Vec(HP*2, HP*13)), module, Kcik::PITCH_ENV_DEPTH_PARAM));
		addParam(createParamCentered<RoundLargeBlackKnob>(mm2px(Vec(HP*4, HP*11)), module, Kcik::CLICK_LEVEL_PARAM));
		addParam(createParamCentered<RoundLargeBlackKnob>(mm2px(Vec(HP*6, HP*13)), module, Kcik::DRIVE_PARAM));

        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(HP*2, HP*20)), module, Kcik::AUDIO_OUTPUT));
        // addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(HP*4, HP*18)), module, Kcik::NOISE_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(HP*4, HP*20)), module, Kcik::ENVELOPE_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(HP*6, HP*20)), module, Kcik::PITCH_ENV_OUTPUT));

        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(HP*2, HP*23)), module, Kcik::VELOCITY_CV_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(HP*4, HP*23)), module, Kcik::GATE_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(HP*6, HP*23)), module, Kcik::DRIVE_CV_INPUT));



    }
};

Model *modelKcik = createModel<Kcik, KcikWidget>("Kcik");
