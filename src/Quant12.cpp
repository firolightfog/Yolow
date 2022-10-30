// Copyright (c) 2022 András Szabó
// Read it: https://en.wikipedia.org/wiki/Category:Equal_temperaments
#include "plugin.hpp"

struct Quant12 : Module {

// --------------------------------------------------

	enum ParamId    {
		MODE_PARAM, TRIGGER_PROBABILITY_PARAM, NOISE_ATTENUATOR_PARAM, 
		TRANSPOSE_ATTENUATOR_PARAM, ENUMS(NOTE_PARAM,12), PARAMS_LEN};

	enum InputId    {
		TRIGGER_INPUT, NOISE_INPUT, TRANSPOSE_INPUT, INPUTS_LEN};

	enum OutputId    {
		NOTE_OUTPUT, POLY_ALLOWED_OUTPUT, OUTPUTS_LEN};

	enum LightId    {ENUMS(NOTE_LIGHT,12), TRIGGER_LIGHT, LIGHTS_LEN};

	// small assistance to save older values for reference;
	float paramVal[PARAMS_LEN]={0};
	// float inputVolt[INPUTS_LEN]={0};
	// float lightVal[LIGHTS_LEN]={0};

// --------------------------------------------------

	const std::string labelNotes[12]={
		"Note C (0.000V)","Note C# (0.083V)","Note D (0.167V)",
		"Note D# (0.250V)","Note E (0.333V)","Note F (0.417V)",
		"Note F# (0.500V)","Note G (0.583V)","Note G# (0.667V)",
		"Note A (0.750V)","Note A# (0.833V)","Note B (0.917V)"};
	const std::string labelModes[3]={
		"Add the transpose CV and quantize",
		"Ignore transpose CV",
		"Quantize and add the transpose CV"};
		// "Quantize and add the transpose CV",
		// "Add the transpose CV and quantize",
		// "Ignore transpose CV"};

// --------------------------------------------------
	
	Quant12() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(MODE_PARAM, 0.0f, 2.0f, 1.0f, "Mode");
		configParam(TRIGGER_PROBABILITY_PARAM, 0.0f, 1.0f, 1.0f, "Trigger probability","%", 0, 100);
		configParam(NOISE_ATTENUATOR_PARAM, 0.0f, 1.0f, 1.0f, "Noise attenuator","%", 0, 100);
		configParam(TRANSPOSE_ATTENUATOR_PARAM,	0.0f, 1.0f, 0.0f, "Transpose attenuator","%", 0, 100);
		paramQuantities[TRIGGER_PROBABILITY_PARAM]->randomizeEnabled = false;
		paramQuantities[NOISE_ATTENUATOR_PARAM]->randomizeEnabled = false;
		paramQuantities[TRANSPOSE_ATTENUATOR_PARAM]->randomizeEnabled = false;
		paramQuantities[MODE_PARAM]->randomizeEnabled = false;
		paramQuantities[MODE_PARAM]->snapEnabled = true;

		for (int p=0;p<12;p++) {
			configParam(NOTE_PARAM+p, 0.0f, 1.0f, 1.0f, labelNotes[p]);
			paramQuantities[NOTE_PARAM+p]->snapEnabled = true;
		}

		configInput(TRIGGER_INPUT, "Trigger"); 
		configInput(NOISE_INPUT, "Noise"); 
		configInput(TRANSPOSE_INPUT, "Transpose"); 

		configOutput(NOTE_OUTPUT, "Quantized note"); 
		configOutput(POLY_ALLOWED_OUTPUT, "Upto 12 of the allowed notes"); 
	}

// --------------------------------------------------

	// for managing the RESET and CLOCK signals
	bool clockIn=false;	// any clock plugged in?
	bool noiseIn=false;	// any noise plugged in?
	bool transIn=false; // any transpose plugged in?

	float newClock=0.0f;
	float oldClock=0.0f;

	float theOct=0.0f;
	float theNote=0.0f;	
	float aN[12]={1/12,2/12,3/12,4/12,5/12,6/12,7/12,8/12,9/12,10/12,11/12};

	float newNoise;	
	float newTransp;
	
	// --------------------------------------------------

	// this gets a 0v-1v value and returns a quantized one
	float roundTo(float nx) {
		float beforeNote=-1;
		float afterNote=-1;
		nx=nx*12;
		for (int p=0; p<12; p++) {
			if (paramVal[NOTE_PARAM+p]==1) {
				if (nx>=p) {
					if (beforeNote==-1) {afterNote=12+p;}
					beforeNote=p;
				}
				else {
					afterNote=p;
					return ((afterNote-nx>nx-beforeNote)?beforeNote:afterNote)/12;
				}
			}
		}	
		if (beforeNote==-1) {return nx/12;}
		else if (afterNote==-1) {return beforeNote/12;}
		else {return ((afterNote-nx>nx-beforeNote)?beforeNote:afterNote)/12;}
	}

	// this manipulates the incoming voltage based on MODE and sends it to the output
	float oldVoltOut=0;
	float newVoltOut=0;
	void sendToOutput() {
		if (paramVal[MODE_PARAM]==1) {
			theOct=floor(newNoise);
			theNote=newNoise - theOct;
			// newVoltOut=theOct + round(newNoise*12)/12;
			// newVoltOut=theOct + roundTo(newNoise);
			newVoltOut=theOct + roundTo(theNote);
		}
		else if (paramVal[MODE_PARAM]==0) {
			theOct=floor(newNoise+newTransp);
			theNote=(newNoise+newTransp) - theOct;
			// newVoltOut=theOct + round(newNoise*12)/12;
			// newVoltOut=theOct + roundTo(newNoise);
			newVoltOut=theOct + roundTo(theNote);
		}
		else if (paramVal[MODE_PARAM]==2) {
			theOct=floor(newNoise);
			theNote=(newNoise) - theOct;
			// newVoltOut=theOct + round(newNoise*12)/12 + newTransp;
			// newVoltOut=theOct + roundTo(newNoise) + newTransp;
			newVoltOut=theOct + roundTo(theNote) + newTransp;
		}		
		if (oldVoltOut!=newVoltOut) {
			outputs[NOTE_OUTPUT].setVoltage(newVoltOut);
			oldVoltOut=newVoltOut;
			// lights[TRIGGER_LIGHT].setBrightness(0.8f);
		}
	}

	// --------------------------------------------------
	
	// this is the main routine
	int loop=0;
	void process(const ProcessArgs& args) override {

		if (loop--<=0) {
			loop=7755;
			// save some more CPU
			clockIn=inputs[TRIGGER_INPUT].isConnected();
			noiseIn=inputs[NOISE_INPUT].isConnected();
			transIn=inputs[TRANSPOSE_INPUT].isConnected();

			for (int p=0;p<PARAMS_LEN;p++) {paramVal[p]=params[p].getValue();}
			paramQuantities[MODE_PARAM]->description = labelModes[(int)paramVal[MODE_PARAM]];			

			// put the shortlist
			int cc=0;	// channel counter
			outputs[POLY_ALLOWED_OUTPUT].channels=16;
			for (int p=0;p<12;p++) {
				// lights[NOTE_LIGHT+p].setBrightness(paramVal[NOTE_PARAM+p]);
				if (paramVal[NOTE_PARAM+p]==1) {
					lights[NOTE_LIGHT+p].setBrightness(0.8f);
					if (paramVal[MODE_PARAM]==1) {
						outputs[POLY_ALLOWED_OUTPUT].setVoltage(p/12.0f,cc);
					}
					else if (paramVal[MODE_PARAM]==0) {
						theOct=floor(newTransp);
						theNote=newTransp - theOct;						
						outputs[POLY_ALLOWED_OUTPUT].setVoltage(p/12.0f+theOct + roundTo(theNote),cc);						
					}
					else if (paramVal[MODE_PARAM]==2) {
						outputs[POLY_ALLOWED_OUTPUT].setVoltage(p/12.0f+newTransp,cc);
					}
					cc++;
				} 
				else {lights[NOTE_LIGHT+p].setBrightness(0.0f);}
			}
			outputs[POLY_ALLOWED_OUTPUT].channels=cc;

		}


		// --------------------------------------------------

		if (noiseIn) {newNoise=inputs[NOISE_INPUT].getVoltage()*paramVal[NOISE_ATTENUATOR_PARAM];}
		else if (loop % 2222 ==0) {newNoise=rack::random::uniform()*paramVal[NOISE_ATTENUATOR_PARAM];}

		if (transIn) {newTransp=inputs[TRANSPOSE_INPUT].getVoltage()*paramVal[TRANSPOSE_ATTENUATOR_PARAM];}
		else {newTransp=1*paramVal[TRANSPOSE_ATTENUATOR_PARAM];}
		
		if (clockIn) {
			newClock=inputs[TRIGGER_INPUT].getVoltage();
			if (newClock>2.0f && oldClock<=2.0f 
				&& paramVal[TRIGGER_PROBABILITY_PARAM]>=rack::random::uniform()) {
					lights[TRIGGER_LIGHT].setBrightness(0.8f);
					sendToOutput();
			}
			else if (newClock<2.0f && oldClock>=2.0f) {
				lights[TRIGGER_LIGHT].setBrightness(0.0f);
			}
			oldClock=newClock;
		}
		else if (noiseIn) {sendToOutput();}
		else {if (rand() % 44444 == 25) {sendToOutput();};}
		
		// ------------------------------------------
		// H trigIn 	0	1	0	0	1	1	0	1
		// N noiseIn	0	0	1	0	1	0	1	1
		// T transIn	0	0	0	1	0	1	1	1
		//-------------------------------------------		

	}

	// --------------------------------------------------

};

// --------------------------------------------------

struct Quant12Widget : ModuleWidget {

	Quant12* module;
	#include "share/widgetwizard.hpp"   // addChild abbreviations

	Quant12Widget(Quant12* module) {

		this->module = module;
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/Quant12.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		for (int p=0;p<12;p++) {
			childPushbutton(Quant12::NOTE_PARAM+p, Quant12::NOTE_LIGHT+p, HP*0.75+HP*(p%3)*1.25, HP*4.5+HP*floor(p/3)*1.25);
		}

		childSwitch(Quant12::MODE_PARAM, 2, HP*2, HP*10.75);

		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(HP*2, HP*12.50)), module, Quant12::TRIGGER_LIGHT));
		// childLight(Quant12::TRIGGER_LIGHT, 10, HP*2, HP*12.50);		
		childKnob(Quant12::TRIGGER_PROBABILITY_PARAM, 0, HP*1, HP*13.25);		
		childInput(Quant12::TRIGGER_INPUT, HP*3, HP*13.25);

		childKnob(Quant12::NOISE_ATTENUATOR_PARAM, 0, HP*1, HP*15.75);
		childInput(Quant12::NOISE_INPUT, HP*3, HP*15.75);

		childKnob(Quant12::TRANSPOSE_ATTENUATOR_PARAM, 0, HP*1, HP*18.25);
		childInput(Quant12::TRANSPOSE_INPUT, HP*3, HP*18.25);

		childOutput(Quant12::POLY_ALLOWED_OUTPUT, HP*1, HP*22.25);
		childOutput(Quant12::NOTE_OUTPUT, HP*3, HP*22.25);
		
		// childLabel(HP*5,HP*1, "DEV", 12);
	}

	// void appendContextMenu(Menu* menu) override {
		// QuantET* module = dynamic_cast<QuantET*>(this->module);
		// assert(module);
		// menu->addChild(new MenuSeparator);
		// menu->addChild(createIndexPtrSubmenuItem("Mutate scale", {"25%","50%","100%"}, &module->rndScale(this)));
	// }

};

Model* modelQuant12 = createModel<Quant12, Quant12Widget>("Quant12");
