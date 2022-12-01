// Copyright (c) 2022 András Szabó
#include "plugin.hpp"

struct Celei : Module {

// --------------------------------------------------

	enum ParamId    {
		SEQ_1_VOLTAGE_PARAM, SEQ_2_VOLTAGE_PARAM, SEQ_3_VOLTAGE_PARAM, SEQ_4_VOLTAGE_PARAM, SEQ_5_VOLTAGE_PARAM, 
		SEQ_6_VOLTAGE_PARAM, SEQ_7_VOLTAGE_PARAM, SEQ_8_VOLTAGE_PARAM, STEPS_PARAM, OCTAVE_PARAM, 
		RANGE_PARAM, TRIGGER_STEP_PARAM, PARAMS_LEN};

	enum InputId    {
		TRANSPOSE_INPUT, CLOCK_INPUT, RESET_INPUT, INPUTS_LEN};

	enum OutputId    {
		MONO_OUTPUT, TRIGGER_STEP_OUTPUT, OUTPUTS_LEN};
	
	enum LightId    {
		SEQ_1_LED_LIGHT, SEQ_2_LED_LIGHT, SEQ_3_LED_LIGHT, SEQ_4_LED_LIGHT, SEQ_5_LED_LIGHT, 
		SEQ_6_LED_LIGHT, SEQ_7_LED_LIGHT, SEQ_8_LED_LIGHT, 
		// BG_1_LED_LIGHT, BG_2_LED_LIGHT, BG_3_LED_LIGHT, BG_4_LED_LIGHT, BG_5_LED_LIGHT, 
		// BG_6_LED_LIGHT, BG_7_LED_LIGHT, BG_8_LED_LIGHT, 		
		LIGHTS_LEN};

	// small assistance to save older values for reference;
	float paramVal[PARAMS_LEN]={0};
	float inputVolt[INPUTS_LEN]={0};
	float lightVal[LIGHTS_LEN]={0};

// --------------------------------------------------

	Celei() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(SEQ_1_VOLTAGE_PARAM, 	0.0f, 1.0f, 0.0f, "Seq 1 voltage");
		configParam(SEQ_2_VOLTAGE_PARAM, 	0.0f, 1.0f, 0.0f, "Seq 2 voltage");
		configParam(SEQ_3_VOLTAGE_PARAM, 	0.0f, 1.0f, 0.0f, "Seq 3 voltage");
		configParam(SEQ_4_VOLTAGE_PARAM, 	0.0f, 1.0f, 0.0f, "Seq 4 voltage");
		configParam(SEQ_5_VOLTAGE_PARAM, 	0.0f, 1.0f, 0.0f, "Seq 5 voltage");
		configParam(SEQ_6_VOLTAGE_PARAM, 	0.0f, 1.0f, 0.0f, "Seq 6 voltage");
		configParam(SEQ_7_VOLTAGE_PARAM, 	0.0f, 1.0f, 0.0f, "Seq 7 voltage");
		configParam(SEQ_8_VOLTAGE_PARAM, 	0.0f, 1.0f, 0.0f, "Seq 8 voltage");
		configParam(STEPS_PARAM, 			1.0f, 8.0f, 8.0f, "Steps");
		configParam(OCTAVE_PARAM, 			-5.0f, 5.0f, 0.0f, "Octave");
		configParam(RANGE_PARAM, 			1.0f, 7.0f, 1.0f, "Range");
		configParam(TRIGGER_STEP_PARAM, 	1.0f, 8.0f, 1.0f, "Trigger step");
		paramQuantities[STEPS_PARAM]->snapEnabled = true;
		paramQuantities[OCTAVE_PARAM]->snapEnabled = true;
		paramQuantities[RANGE_PARAM]->snapEnabled = true;
		paramQuantities[TRIGGER_STEP_PARAM]->snapEnabled = true;

		configInput(TRANSPOSE_INPUT, "Transpose"); 
		configInput(CLOCK_INPUT, "Clock"); 
		configInput(RESET_INPUT, "Reset"); 

		configOutput(MONO_OUTPUT, "Mono"); 
		configOutput(TRIGGER_STEP_OUTPUT, "Trigger step"); 
	}

// pressing a number provides a variation for that specific knob
void keyKnob(int knobRef) {
	params[SEQ_1_VOLTAGE_PARAM+knobRef].setValue(abs(1-params[SEQ_1_VOLTAGE_PARAM+knobRef].getValue()));
}

// pressing i provides a variation of the same tune
void invKnob() {
	float nSx[8]={0.0f};
	for (int k=0;k<8;k++) {nSx[k]=abs(1-params[SEQ_1_VOLTAGE_PARAM+k].getValue());}
	for (int k=0;k<8;k++) {params[SEQ_1_VOLTAGE_PARAM+k].setValue(nSx[k]);}
}


// pressing r randomizes the note knobs only
void rndKnob() {
	float nSx[8]={0.0f};
	for (int k=0;k<8;k++) {nSx[k]=rack::random::uniform();}
	for (int k=0;k<8;k++) {params[SEQ_1_VOLTAGE_PARAM+k].setValue(nSx[k]);}
	params[STEPS_PARAM].setValue(rand() % 7 + 2);	// random steps between 2-8
/*
	// https://github.com/Ahornberg/Ahornberg-VCV-Modules/blob/master/src/modules/FlyingFader/FlyingFaderWidget.cpp
	// Push ParamChange history action
	ParamQuantity* paramQuantity = getParamQuantity();
	history::ComplexAction* complexAction = new history::ComplexAction;
	complexAction->name = "randomize knobs";
	history::ParamChange* oldKnobValue = new history::ParamChange;
	oldKnobValue->name = "randomize knobs";
	oldKnobValue->moduleId = paramQuantity->module->id;
	oldKnobValue->paramId = Celei::STEPS_PARAM;
	oldKnobValue->oldValue = pSteps;
	oldKnobValue->newValue = params[STEPS_PARAM].getValue();
	complexAction->push(oldKnobValue);
	APP->history->push(complexAction);
*/
}

// pressing u moves knobvalues upwards
void supKnob() {
	float tempVal=params[SEQ_1_VOLTAGE_PARAM].getValue();
	params[SEQ_1_VOLTAGE_PARAM].setValue(params[SEQ_2_VOLTAGE_PARAM].getValue());
	params[SEQ_2_VOLTAGE_PARAM].setValue(params[SEQ_3_VOLTAGE_PARAM].getValue());
	params[SEQ_3_VOLTAGE_PARAM].setValue(params[SEQ_4_VOLTAGE_PARAM].getValue());
	params[SEQ_4_VOLTAGE_PARAM].setValue(params[SEQ_5_VOLTAGE_PARAM].getValue());
	params[SEQ_5_VOLTAGE_PARAM].setValue(params[SEQ_6_VOLTAGE_PARAM].getValue());
	params[SEQ_6_VOLTAGE_PARAM].setValue(params[SEQ_7_VOLTAGE_PARAM].getValue());
	params[SEQ_7_VOLTAGE_PARAM].setValue(params[SEQ_8_VOLTAGE_PARAM].getValue());
	params[SEQ_8_VOLTAGE_PARAM].setValue(tempVal);
}

// pressing d moves knobvalues downwards
void sdnKnob() {
	float tempVal=params[SEQ_8_VOLTAGE_PARAM].getValue();
	params[SEQ_8_VOLTAGE_PARAM].setValue(params[SEQ_7_VOLTAGE_PARAM].getValue());
	params[SEQ_7_VOLTAGE_PARAM].setValue(params[SEQ_6_VOLTAGE_PARAM].getValue());
	params[SEQ_6_VOLTAGE_PARAM].setValue(params[SEQ_5_VOLTAGE_PARAM].getValue());
	params[SEQ_5_VOLTAGE_PARAM].setValue(params[SEQ_4_VOLTAGE_PARAM].getValue());
	params[SEQ_4_VOLTAGE_PARAM].setValue(params[SEQ_3_VOLTAGE_PARAM].getValue());
	params[SEQ_3_VOLTAGE_PARAM].setValue(params[SEQ_2_VOLTAGE_PARAM].getValue());
	params[SEQ_2_VOLTAGE_PARAM].setValue(params[SEQ_1_VOLTAGE_PARAM].getValue());
	params[SEQ_1_VOLTAGE_PARAM].setValue(tempVal);
}

// pressing f allows freezing the nore for a step (not very useful imo)
int freezCv=0;

// --------------------------------------------------

	int loop=0;     // save some CPU in process()
	int stepA=0;	// position of current step (ie. 1-8)
	float voltA=0.0f;	// calculated voltage output
	
	// for managing the RESET and CLOCK signals
	float newReset=0.0f;
	float oldReset=0.0f;
	float newClock=0.0f;
	float oldClock=0.0f;

	// storing parameter positions occaisonly
	float pSx[8]={0.0f};
	float pSteps=0.0f;
	float pOct=0.0f;
	float pRng=0.0f;
	float pTrg=0.0f;
	bool iTrans=false;	// any transpose cable available?
	
	// JSON variables for portable sequence experiment 
//	float lengthInBeats[8]={0};
//	float vOct[8]={1};
	
	// hidden trick: if true it sends a clock pulse instead of a constant CV
	bool modeClock=false;
	int indexQuant=0;	// this means no quantization

	float quantMe(float oldVal) {
		// oldVal=oldVal+5*indexRange;
		if (indexQuant==0) {return oldVal;}
		else if (indexQuant==1) {return round(oldVal);}
		else if (indexQuant==2) {return round(oldVal*12)/12;}
		else if (indexQuant==3) {return (oldVal>2)?10:0;}
		else {return oldVal;}
	}

	// Ctrl-E bypassing: no sound but the counting stays
	void processBypass(const ProcessArgs& args) override {
		newReset=inputs[RESET_INPUT].getVoltage();
		if (newReset>2.0f && oldReset<=2.0f) {stepA=0;}
		oldReset=newReset;
		newClock=inputs[CLOCK_INPUT].getVoltage();
		if (newClock>2.0f && oldClock<=2.0f) {stepA++; if (stepA>pSteps || stepA<1) {stepA=1;}}
		else if (newClock<=2.0f && oldClock>2.0f) {if (freezCv>=0) {freezCv--;}}
		oldClock=newClock;
	}

	void process(const ProcessArgs& args) override {

		if (loop--<=0) {
			loop=9000;
			// save some more CPU
			pSteps=params[STEPS_PARAM].getValue(); 
			pOct=params[OCTAVE_PARAM].getValue();
			pRng=params[RANGE_PARAM].getValue();
			pTrg=params[TRIGGER_STEP_PARAM].getValue();			
			iTrans=inputs[TRANSPOSE_INPUT].isConnected();
			pSx[0]=params[0+SEQ_1_VOLTAGE_PARAM].getValue();
			pSx[1]=params[1+SEQ_1_VOLTAGE_PARAM].getValue();
			pSx[2]=params[2+SEQ_1_VOLTAGE_PARAM].getValue();
			pSx[3]=params[3+SEQ_1_VOLTAGE_PARAM].getValue();
			pSx[4]=params[4+SEQ_1_VOLTAGE_PARAM].getValue();
			pSx[5]=params[5+SEQ_1_VOLTAGE_PARAM].getValue();
			pSx[6]=params[6+SEQ_1_VOLTAGE_PARAM].getValue();
			pSx[7]=params[7+SEQ_1_VOLTAGE_PARAM].getValue();
//			for (int j=0;j<8;j++) {
//				vOct[j]=pOct+pRng*pSx[j];
//				lengthInBeats[j]=0.9;
//			}

			// for (int i=0;i<8;i++) {
				// lights[BG_1_LED_LIGHT+i].setBrightness(pSx[i]);
			// }

		}

		// let's see the reset signal
		newReset=inputs[RESET_INPUT].getVoltage();
		if (newReset>2.0f && oldReset<=2.0f) {
			if (stepA>0) {lights[SEQ_1_LED_LIGHT-1+stepA].setBrightness(0);}
			lights[SEQ_1_LED_LIGHT].setBrightness(10);
			stepA=0; 
			}
		// else if (newReset>0.2 && oldReset>0.2) {}
		// else if (newReset<=0.2 && oldReset>0.2) {}
		// else if (newReset<=0.2 && oldReset<=0.2) {}
		oldReset=newReset;

		// let's see the clock signal
		newClock=inputs[CLOCK_INPUT].getVoltage();
		if (newClock>2.0f && oldClock<=2.0f) {			

			// take the next step
			if (stepA>0) {lights[SEQ_1_LED_LIGHT-1+stepA].setBrightness(0);}
			stepA++; 
			if (stepA>pSteps || stepA<1) {stepA=1;}
			if (stepA==pTrg) {outputs[TRIGGER_STEP_OUTPUT].setVoltage(10);}
			lights[SEQ_1_LED_LIGHT-1+stepA].setBrightness(10);
			
		}
		// else if (newClock>0.2 && oldClock>0.2) {}
		// else if (newClock<=0.2 && oldClock<=0.2) {}
		else if (newClock<=2.0f && oldClock>2.0f) {
			outputs[TRIGGER_STEP_OUTPUT].setVoltage(0);
			if (modeClock==true) {voltA=0;}
			if (freezCv>=0) {freezCv--;}
			}
		oldClock=newClock;

		// let's calculate the output voltage
		if (modeClock==true && oldClock<=2.0) {voltA=0;}
		else if (freezCv>=0) {/* do nothing; keep the last note */}
		else {
			voltA=pSx[stepA-1];
			voltA=voltA*pRng+pOct;
			if (iTrans==true) {
				voltA=voltA+remainder(inputs[TRANSPOSE_INPUT].getVoltage(), 10.0f);
			}
		}
		outputs[MONO_OUTPUT].setVoltage(quantMe(voltA));

	}

	// this JSON block is to save and reload a variable
	json_t* dataToJson() override {
	json_t* rootJ = json_object();
	json_object_set_new(rootJ, "quant", json_integer(indexQuant));
//    json_object_set_new(rootJ, "type", json_string("note"));	// portable seq
//    json_object_set_new(rootJ, "pitch", json_real(vOct);		// portable seq
//    json_object_set_new(rootJ, "length", json_real(lengthInBeats);	// portable seq
	return rootJ;}

	void dataFromJson(json_t* rootJ) override {
	json_t *quantJ = json_object_get(rootJ, "quant");
	if (quantJ) indexQuant = json_integer_value(quantJ);}

};

// --------------------------------------------------

struct CeleiWidget : ModuleWidget {

	Celei* module;
	#include "share/widgetwizard.hpp"   // addChild abbreviations

	CeleiWidget(Celei* module) {

		this->module = module;
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/Celei.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		
		childKnob(Celei::SEQ_1_VOLTAGE_PARAM, 1, HP*4.5, HP*5.5);
		childKnob(Celei::SEQ_2_VOLTAGE_PARAM, 1, HP*4.5, HP*8);
		childKnob(Celei::SEQ_3_VOLTAGE_PARAM, 1, HP*4.5, HP*10.5);
		childKnob(Celei::SEQ_4_VOLTAGE_PARAM, 1, HP*4.5, HP*13);
		childKnob(Celei::SEQ_5_VOLTAGE_PARAM, 1, HP*4.5, HP*15.5);
		childKnob(Celei::SEQ_6_VOLTAGE_PARAM, 1, HP*4.5, HP*18);
		childKnob(Celei::SEQ_7_VOLTAGE_PARAM, 1, HP*4.5, HP*20.5);
		childKnob(Celei::SEQ_8_VOLTAGE_PARAM, 1, HP*4.5, HP*23);
		childKnob(Celei::STEPS_PARAM, 0, HP*4.5, HP*2.5);
		childKnob(Celei::OCTAVE_PARAM, 0, HP*1.5, HP*10.5);
		childKnob(Celei::RANGE_PARAM, 0, HP*1.5, HP*13);
		childKnob(Celei::TRIGGER_STEP_PARAM, 0, HP*1.5, HP*15.5);

		// for (int i=0;i<8;i++) {
			// childLight(Celei::BG_1_LED_LIGHT+i, 2, HP*4.5, HP*5.5+i*2.5);
			// addChild(createLightCentered<LargeLight<WhiteLight>>(mm2px(Vec(5.08*4.5, 5.08*(5.5+i*2.5))), module, Celei::BG_1_LED_LIGHT+i));
		// }
		
		childLight(Celei::SEQ_1_LED_LIGHT, 0, HP*4.5, HP*5.5);
		childLight(Celei::SEQ_2_LED_LIGHT, 0, HP*4.5, HP*8);
		childLight(Celei::SEQ_3_LED_LIGHT, 0, HP*4.5, HP*10.5);
		childLight(Celei::SEQ_4_LED_LIGHT, 0, HP*4.5, HP*13);
		childLight(Celei::SEQ_5_LED_LIGHT, 0, HP*4.5, HP*15.5);
		childLight(Celei::SEQ_6_LED_LIGHT, 0, HP*4.5, HP*18);
		childLight(Celei::SEQ_7_LED_LIGHT, 0, HP*4.5, HP*20.5);
		childLight(Celei::SEQ_8_LED_LIGHT, 0, HP*4.5, HP*23);
		childInput(Celei::TRANSPOSE_INPUT, HP*1.5, HP*5.5);
		childOutput(Celei::MONO_OUTPUT, HP*1.5, HP*8);
		childOutput(Celei::TRIGGER_STEP_OUTPUT, HP*1.5, HP*18);
		childInput(Celei::CLOCK_INPUT, HP*1.5, HP*20.5);
		childInput(Celei::RESET_INPUT, HP*1.5, HP*23);
	}

	// menu for basic quantization
	void appendContextMenu(Menu* menu) override {
		Celei* module = dynamic_cast<Celei*>(this->module);
		assert(module);
		menu->addChild(new MenuSeparator);
		menu->addChild(createIndexPtrSubmenuItem("Quantize", {"Nope","Octaves","Notes" /*,"Gates (>2V)"*/}, &module->indexQuant));
	}

	// shortkey
	void onHoverKey(const event::HoverKey &e) override {
		if (e.key >= GLFW_KEY_1 && e.key <= GLFW_KEY_8) {
			if (e.action == GLFW_PRESS) {
				float key_number = e.key - 49; // 49 is the ascii number for key #1
				module->keyKnob(key_number);
				e.consume(this);
			}
		}
		else if(e.key == GLFW_KEY_C && e.action == GLFW_PRESS) {module->modeClock=true; e.consume(this);}
		else if(e.key == GLFW_KEY_V && e.action == GLFW_PRESS) {module->modeClock=false; e.consume(this);}
		else if(e.key == GLFW_KEY_R && e.action == GLFW_PRESS) {module->rndKnob(); e.consume(this);}
		else if(e.key == GLFW_KEY_I && e.action == GLFW_PRESS) {module->invKnob(); e.consume(this);}
		else if(e.key == GLFW_KEY_U && e.action == GLFW_PRESS) {module->supKnob(); e.consume(this);}
		else if(e.key == GLFW_KEY_D && e.action == GLFW_PRESS) {module->sdnKnob(); e.consume(this);}
		else if(e.key == GLFW_KEY_F && e.action == GLFW_PRESS) {module->freezCv=2; e.consume(this);}
		ModuleWidget::onHoverKey(e);
	}

};

Model* modelCelei = createModel<Celei, CeleiWidget>("Celei");
