// Copyright (c) 2022 András Szabó
// Read it: https://en.wikipedia.org/wiki/Category:Equal_temperaments
#include "plugin.hpp"

struct QuantET : Module {

// --------------------------------------------------

	enum ParamId    {
		EQUAL_TEMPERIEMNT_PARAM, NUMBER_OF_NOTES_PARAM, MODE_PARAM, TRIGGER_PROBABILITY_PARAM, NOISE_ATTENUATOR_PARAM, 
		TRANSPOSE_ATTENUATOR_PARAM, PARAMS_LEN};

	enum InputId    {
		TRIGGER_INPUT, NOISE_INPUT, TRANSPOSE_INPUT, INPUTS_LEN};

	enum OutputId    {
		NOTE_OUTPUT, POLY_ALLOWED_OUTPUT, POLY_FULL_OUTPUT, OUTPUTS_LEN};

	enum LightId    {LIGHTS_LEN};

	// small assistance to save older values for reference;
	float paramVal[PARAMS_LEN]={0};
	// float inputVolt[INPUTS_LEN]={0};
	// float lightVal[LIGHTS_LEN]={0};

// --------------------------------------------------

	QuantET() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(EQUAL_TEMPERIEMNT_PARAM, 	1.0f, 53.0f, 12.0f, "Equal temperiemnt");
		configParam(NUMBER_OF_NOTES_PARAM, 		0.0f, 1.0f, 1.0f, "Number of notes","%", 0, 100);
		configParam(MODE_PARAM, 				0.0f, 2.0f, 1.0f, "Mode");
		configParam(TRIGGER_PROBABILITY_PARAM, 	0.0f, 1.0f, 1.0f, "A trigger probability","%", 0, 100);
		configParam(NOISE_ATTENUATOR_PARAM, 	0.0f, 1.0f, 1.0f, "A noise attenuator","%", 0, 100);
		configParam(TRANSPOSE_ATTENUATOR_PARAM,	0.0f, 1.0f, 1.0f, "A transpose attenuator","%", 0, 100);
		paramQuantities[EQUAL_TEMPERIEMNT_PARAM]->randomizeEnabled = false;
		paramQuantities[EQUAL_TEMPERIEMNT_PARAM]->snapEnabled = true;
		paramQuantities[MODE_PARAM]->snapEnabled = true;

		configInput(TRIGGER_INPUT, "Trigger"); 
		configInput(NOISE_INPUT, "Noise"); 
		configInput(TRANSPOSE_INPUT, "Transpose"); 

		configOutput(NOTE_OUTPUT, "Quantized note"); 
		configOutput(POLY_ALLOWED_OUTPUT, "Short listed notes"); 
		configOutput(POLY_FULL_OUTPUT, "All available notes"); 
	}

// --------------------------------------------------

	// for managing the RESET and CLOCK signals
	float newClock=0.0f;
	float oldClock=0.0f;
	bool hitClock=false;
	const int CLOCK_INPUT=TRIGGER_INPUT;
	bool clockIn=false;
	bool noiseIn=false;
	int indexRndScale=0;
	
	// some labels for the parameters
	std::string modeList[3]={
		"Quantize and add the transpose CV",
		"Add the transpose CV and quantize",
		"Ignore transpose CV"};

	// --------------------------------------------------
	
	/* 
		generates euclideian patterns
		https://electro-music.com/forum/topic-62215.html
		c: current step number
		k: hits per bar
		n: bar length
		r: rotation
		returns 1 or 0 according to the euclidean pattern
		int euclidStep( int c, int k, int n, int r ) {return (((c + r) * k) % n) < k;}
	*/

	int euclidArray[53];
	void euclid(int steps, int pulses, int rotation){
		memset(euclidArray, 0, sizeof(euclidArray));
		int currpos=0;
		if (indexRndScale==1) {
			while (pulses > 0) {
				currpos=rand()%steps;
				if (euclidArray[currpos]==0) {euclidArray[currpos]=1;pulses--;}
			}
		}
		else {
			for (currpos=0;currpos<steps;currpos++) {
				euclidArray[currpos]=(((currpos*pulses) % steps) < pulses)?1:0;			
			}	
		}
	}

	// --------------------------------------------------
	
	bool TEST=false;
	
	// this is to calculate and send the output
	float oldET=0.0f;
	float oldNR=0.0f;
	float setOfNotes[53]={0};
	float setOfAllowedNotes[53]={0};
	void calcQuant(float startVolt) {	
	
		// additional tricks: POLY_ALLOWED_OUTPUT sends the scale of one octave for monitoring
		if (oldET!=paramVal[EQUAL_TEMPERIEMNT_PARAM] || oldNR!=paramVal[NUMBER_OF_NOTES_PARAM]) {
			oldET=paramVal[EQUAL_TEMPERIEMNT_PARAM];
			oldNR=paramVal[NUMBER_OF_NOTES_PARAM];						
			// put an Euclidean pattern to identify allowed notes
			int a=(int)(oldET*oldNR);	// number of allowed notes
			euclid(oldET,a,0);	// update euclidArray
		}

		// -------------------------
		// 9 8 7 6 5 4 3 2 1 	setOfNotes
		// 1 0 1 0 1 0 1 1 1 	euclidArray
		// 9 7 5 3 2 1 			ouputs
		// -------------------------

		// OK, setOfAllowedNotes is ready: let's calculate
		float theOct=0.0f;
		float theNote=0.0f;	
		float transpVolt=0.0f;
		if (paramVal[MODE_PARAM]!=2) {transpVolt=inputs[TRANSPOSE_INPUT].getVoltage();}	// transpose voltage
		if (paramVal[MODE_PARAM]==1) {startVolt=startVolt+transpVolt*paramVal[TRANSPOSE_ATTENUATOR_PARAM];}
		theOct=floor(startVolt);
		theNote=startVolt-theOct;

		// select an allowed note for the input
		for (int p=0;p<oldET;p++) {
			if (euclidArray[p]==1) {
				if (setOfNotes[p]>=theNote) {break;}
				startVolt=setOfNotes[p]+theOct;
			}
		}

		if (paramVal[MODE_PARAM]==0) {startVolt=startVolt+transpVolt*paramVal[TRANSPOSE_ATTENUATOR_PARAM];}
		outputs[NOTE_OUTPUT].setVoltage(startVolt);
		
		/*		
		// OK, setOfAllowedNotes is ready: let's calculate
		if (paramVal[MODE_PARAM]==1) {
			startVolt=startVolt+inputs[TRANSPOSE_INPUT].getVoltage()*paramVal[TRANSPOSE_ATTENUATOR_PARAM];}
		theOct=floor(startVolt);
		theNote=startVolt-theOct;
		// startVolt=theOct+round(theNote*paramVal[EQUAL_TEMPERIEMNT_PARAM])/paramVal[EQUAL_TEMPERIEMNT_PARAM];
		startVolt=setOfAllowedNotes[(int)(oldET*oldNR)-1];
		for (int p=0;p<(int)(oldET*oldNR);p++) {
			if (setOfAllowedNotes[p]>theNote) {
				startVolt=theOct
					+(setOfAllowedNotes[p]-theNote>theNote-setOfAllowedNotes[p-1])
					?setOfAllowedNotes[p-1]
					:setOfAllowedNotes[p];
				break;
			};
		}		
		if (paramVal[MODE_PARAM]==0) {
			startVolt=startVolt+inputs[TRANSPOSE_INPUT].getVoltage()*paramVal[TRANSPOSE_ATTENUATOR_PARAM];}
		outputs[NOTE_OUTPUT].setVoltage(startVolt);
		*/
		
	}

	// --------------------------------------------------
	
	// this is the main routine
	int loop=0;     // save some CPU in process()
	void process(const ProcessArgs& args) override {

		if (loop--<=0) {
			loop=9000;
			// save some more CPU
			for (int p=0;p<PARAMS_LEN;p++) {paramVal[p]=params[p].getValue();}
			paramQuantities[MODE_PARAM]->description = modeList[(int)paramVal[MODE_PARAM]];
			clockIn=inputs[CLOCK_INPUT].isConnected();
			noiseIn=inputs[NOISE_INPUT].isConnected();

			// put the equally divided notes into setOfNotes
			outputs[POLY_FULL_OUTPUT].channels=std::min((int)oldET,16);
			for (int n=0;n<oldET;n++) {
				setOfNotes[n]=n/oldET; 
				if (n<16) {outputs[POLY_FULL_OUTPUT].setVoltage(setOfNotes[n],n);}
				}	

			// put the shortlist
			outputs[POLY_ALLOWED_OUTPUT].channels=((int)oldET*oldNR>16)?16:oldET*oldNR;
			int i=0;
			for (int p=0;p<oldET;p++) {
				if (euclidArray[p]==1) {outputs[POLY_ALLOWED_OUTPUT].setVoltage(setOfNotes[p],i);
				i++; if (i>outputs[POLY_ALLOWED_OUTPUT].channels) {break;}
				}
			}

		}

		// let's see the clock signal
		hitClock=false;
		newClock=inputs[CLOCK_INPUT].getVoltage();
		if (newClock>0.2f && oldClock<=0.2f) {
			// hitClock=true;	// replaced by next line to consider TRIGGER_PROB also
			hitClock=(paramVal[TRIGGER_PROBABILITY_PARAM]>rack::random::uniform());
			}
		// else if (newClock>0.2 && oldClock>0.2) {}
		// else if (newClock<=0.2 && oldClock<=0.2) {}
		// else if (newClock<=0.2 && oldClock>0.2) {}
		oldClock=newClock;
		
		// OK, I admit the next section is not the most efficient
		// but at least it's easier to read the code this way
		if (!noiseIn && hitClock && clockIn) {
			calcQuant(rack::random::uniform()*paramVal[NOISE_ATTENUATOR_PARAM]);
		}
		else if (!noiseIn && !clockIn && (1==(rand() % 2501))) {
			calcQuant(rack::random::uniform()*paramVal[NOISE_ATTENUATOR_PARAM]);
		}
		else if (noiseIn && !clockIn) {	// OK
			calcQuant(inputs[NOISE_INPUT].getVoltage()*paramVal[NOISE_ATTENUATOR_PARAM]);
		}
		else if (noiseIn && hitClock && clockIn) {	
			calcQuant(inputs[NOISE_INPUT].getVoltage()*paramVal[NOISE_ATTENUATOR_PARAM]);
		}

	}

	// --------------------------------------------------

};

// --------------------------------------------------

struct QuantETWidget : ModuleWidget {

	QuantET* module;
	#include "share/widgetwizard.hpp"   // addChild abbreviations

	QuantETWidget(QuantET* module) {

		this->module = module;
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/QuantET.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		
		childKnob(QuantET::EQUAL_TEMPERIEMNT_PARAM, 1, HP*2, HP*4.5);
		childKnob(QuantET::NUMBER_OF_NOTES_PARAM, 2, HP*2, HP*7.75);
		childSwitch(QuantET::MODE_PARAM, 2, HP*2, HP*10.75);

		childKnob(QuantET::TRIGGER_PROBABILITY_PARAM, 0, HP*1, HP*13.25);		
		childInput(QuantET::TRIGGER_INPUT, HP*3, HP*13.25);

		childKnob(QuantET::NOISE_ATTENUATOR_PARAM, 0, HP*1, HP*15.75);
		childInput(QuantET::NOISE_INPUT, HP*3, HP*15.75);

		childKnob(QuantET::TRANSPOSE_ATTENUATOR_PARAM, 0, HP*1, HP*18.25);
		childInput(QuantET::TRANSPOSE_INPUT, HP*3, HP*18.25);

		childOutput(QuantET::POLY_FULL_OUTPUT, HP*1, HP*21.5);
		childOutput(QuantET::POLY_ALLOWED_OUTPUT, HP*3, HP*21.5);
		childOutput(QuantET::NOTE_OUTPUT, HP*2, HP*23.0);
		
	}

	void appendContextMenu(Menu* menu) override {
		QuantET* module = dynamic_cast<QuantET*>(this->module);
		assert(module);
		menu->addChild(new MenuSeparator);
		menu->addChild(createIndexPtrSubmenuItem("Scale generator mode", {"Euclidean","Random"}, &module->indexRndScale));
	}

};

Model* modelQuantET = createModel<QuantET, QuantETWidget>("QuantET");
