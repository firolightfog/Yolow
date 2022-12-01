// Copyright (c) 2022 András Szabó
#include "plugin.hpp"

// -----------------------------------------------------------------------
// EXPANDER STARTS HERE --------------------------------------------------
// -----------------------------------------------------------------------

struct PushmeMore : Module {

	enum ParamId 	{ENUMS(SEQ_PARAM,48), SEQMODE_PARAM, DIV_PARAM, FETCH_SWITCH, PARAMS_LEN};
	enum InputId 	{CLOCK_INPUT, RESET_INPUT, INPUTS_LEN};
	enum OutputId 	{EOC_OUTPUT, TRIGGER_OUTPUT, OUTPUTS_LEN};
	enum LightId 	{ENUMS(SEQ_LIGHT, 48*3),LIGHTS_LEN};

	// small assistance to save older values for reference;
	float paramVal[PARAMS_LEN]={0};

// --------------------------------------------------

	PushmeMore() {
		
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		// configInput(CLOCK_INPUT, "Clock"); 
		// configInput(RESET_INPUT, "Reset"); 
		configOutput(EOC_OUTPUT, "EOC"); 
		configParam(FETCH_SWITCH, 0.0f, 1.0f, 0.0f, "Fetch mother's sequence");
		configOutput(TRIGGER_OUTPUT, "Trigger"); 
		configParam(SEQMODE_PARAM, 	0.0f, 6.0f, 3.0f, "Sequence mode");
		paramQuantities[SEQMODE_PARAM]->randomizeEnabled = false;
		paramQuantities[SEQMODE_PARAM]->snapEnabled = true;
		configParam(DIV_PARAM, 	1.0f, 8.0f, 1.0f, "Clock division");
		paramQuantities[DIV_PARAM]->snapEnabled = true;

		// the 48 buttons are defined with a loop
		configParam(SEQ_PARAM,0.0f, 1.0f, 1.0f, "Zone 1 - SeqA1");
		char lx[25];
		for (int b=1;b<48;b++) {
			sprintf(lx,"Zone %d - SeqA%d", (int(b/8)+1), (b+1));
			configParam(SEQ_PARAM+b,0.0f, 1.0f, 0.0f, lx);
		}
	}

	PushmeSeq* findHostModulePtr(Module* module) {
		if (module) {
			if (module->leftExpander.module) {
				// if it's PushmeSeq, we're done
				if (module->leftExpander.module->model == modelPushmeSeq) {
					return reinterpret_cast<PushmeSeq*>(module->leftExpander.module);
				}
				// if it's PushmeMore, keep recursing
				else if (module->leftExpander.module->model == modelPushmeMore) {
					return findHostModulePtr(module->leftExpander.module);
				}
			}
		}
		return nullptr;
	}

	// --------------------------------------------------

	#include "PushmeSeq/PushmeSeq_routines.hpp"

	// --------------------------------------------------
	
	// ideintifying the step we're at
	int loop=0;
	int stepA=0;	// current step of 48-step-sequencers
	int stepB=0;	// current step of 16-step-sequencers
	int stepC=0;	// current step of 8-step-sequencers
	int stepRand=0;
	bool hitClock=false;
	bool restartPlease=true;
	
	int indexSeqMode=1;
	int indexPrecMode=0;
	int indexPrec=0;
	int indexPW=0;

	void process(const ProcessArgs& args) override {

		// outputs[EOC_OUTPUT].setVoltage(lights[SEQ_LIGHT].getBrightness());

		PushmeSeq const* mother = findHostModulePtr(this);
	
		if (mother) {

			if (loop--<=0) {
				loop=9001;
				// save some more CPU
				indexSeqMode=params[SEQMODE_PARAM].getValue();
				// indexSeqMode: 0 (random) 1 (1x48) 2 (1x32+1x16) 3 (3x16) 4 (2x16+2x8) 5 (1x16+4x8) 6 (6x8)
				paramQuantities[SEQMODE_PARAM]->description = labelSeqMode[indexSeqMode];
				if (indexSeqMode==0) {outputs[TRIGGER_OUTPUT].channels=1;}
				else if (indexSeqMode==1) {outputs[TRIGGER_OUTPUT].channels=1;}
				else if (indexSeqMode==2) {outputs[TRIGGER_OUTPUT].channels=2;}
				else if (indexSeqMode==3) {outputs[TRIGGER_OUTPUT].channels=3;}
				else if (indexSeqMode==4) {outputs[TRIGGER_OUTPUT].channels=4;}
				else if (indexSeqMode==5) {outputs[TRIGGER_OUTPUT].channels=5;}
				else if (indexSeqMode==6) {outputs[TRIGGER_OUTPUT].channels=6;}
				for (int i=0;i<PARAMS_LEN;i++) {paramVal[i]=params[i].getValue();}

				// so if this button is pressed then grab mother's sequence 
				if (paramVal[FETCH_SWITCH]==1) {
					for (int i=0;i<SEQ_PARAM+48;i++) {
						paramVal[SEQ_PARAM+i]=mother->paramVal[SEQ_PARAM+i];
						params[SEQ_PARAM+i].setValue(paramVal[SEQ_PARAM+i]);
						lights[SEQ_LIGHT+i].setBrightness(paramVal[SEQ_PARAM+i]);
					}				
				}

			}
			
			hitClock=mother->hitClock;
			
			// if there's a "new clock pulse" we have a lot to do
			if (hitClock==true) {
				
				if (stepA!=int(mother->stepA/paramVal[DIV_PARAM])) {
				
					stepA=int(mother->stepA/paramVal[DIV_PARAM]);
					hitClock=false;
					
					// fix all lights
					for (int k=0;k<48;k++) {
						lights[SEQ_LIGHT+k].setBrightness(paramVal[SEQ_PARAM+k]*0.452);
						// lights[SEQ_LIGHT+k*3+1].setBrightness(paramVal[SEQ_PARAM+k]*0.452);
					}
					
					// indexSeqMode: 0 (random) 1 (1x48) 2 (1x32+1x16) 3 (3x16) 4 (2x16+2x8) 5 (1x16+4x8) 6 (6x8)		
					if (indexSeqMode==0) {
						if (mother->stepA % (int)paramVal[DIV_PARAM] == 0) {stepRand=(rand() % 48);};
						lights[SEQ_LIGHT+stepRand].setBrightness(0.999);
						outputs[TRIGGER_OUTPUT].setVoltage(paramVal[SEQ_PARAM+stepRand]*10.0f,0);
					}
					else if (indexSeqMode==1) {	
						lights[SEQ_LIGHT+stepA].setBrightness(0.999);
						outputs[TRIGGER_OUTPUT].setVoltage(getVoltNow(SEQ_PARAM+stepA),0);
					}
					else if (indexSeqMode==2) {
						stepB=(stepA % 16);
						lights[SEQ_LIGHT+stepA].setBrightness(0.999);
						lights[SEQ_LIGHT+32+stepB].setBrightness(0.999);
						outputs[TRIGGER_OUTPUT].setVoltage(getVoltNow(SEQ_PARAM+stepA),0);
						outputs[TRIGGER_OUTPUT].setVoltage(getVoltNow(SEQ_PARAM+32+stepB),1);
						if (stepA>=32) {stepA=0;stepB=0;}
					}
					else if (indexSeqMode==3) {	
						stepB=(stepA % 16);			
						lights[SEQ_LIGHT+stepB].setBrightness(0.999);
						lights[SEQ_LIGHT+16+stepB].setBrightness(0.999);
						lights[SEQ_LIGHT+32+stepB].setBrightness(0.999);
						outputs[TRIGGER_OUTPUT].setVoltage(getVoltNow(SEQ_PARAM+stepB),0);
						outputs[TRIGGER_OUTPUT].setVoltage(getVoltNow(SEQ_PARAM+16+stepB),1);
						outputs[TRIGGER_OUTPUT].setVoltage(getVoltNow(SEQ_PARAM+32+stepB),2);
					}
					else if (indexSeqMode==4) {	
						stepB=(stepA % 16);
						stepC=(stepA % 8);
						lights[SEQ_LIGHT+stepB].setBrightness(0.999);
						lights[SEQ_LIGHT+16+stepB].setBrightness(0.999);
						lights[SEQ_LIGHT+32+stepC].setBrightness(0.999);
						lights[SEQ_LIGHT+40+stepC].setBrightness(0.999);
						outputs[TRIGGER_OUTPUT].setVoltage(getVoltNow(SEQ_PARAM+stepB),0);
						outputs[TRIGGER_OUTPUT].setVoltage(getVoltNow(SEQ_PARAM+16+stepB),1);
						outputs[TRIGGER_OUTPUT].setVoltage(getVoltNow(SEQ_PARAM+32+stepC),2);
						outputs[TRIGGER_OUTPUT].setVoltage(getVoltNow(SEQ_PARAM+40+stepC),3);
					}
					else if (indexSeqMode==5) {
						stepB=(stepA % 16);
						stepC=(stepA % 8);
						if (mother->stepC % (int)paramVal[DIV_PARAM] == 0) {stepRand=(rand() % 8);};
						lights[SEQ_LIGHT+stepB].setBrightness(0.999);
						lights[SEQ_LIGHT+16+stepC].setBrightness(0.999);
						lights[SEQ_LIGHT+24+stepC].setBrightness(0.999);
						lights[SEQ_LIGHT+32+stepC].setBrightness(0.999);
						lights[SEQ_LIGHT+40+stepRand].setBrightness(0.999);
						outputs[TRIGGER_OUTPUT].setVoltage(getVoltNow(SEQ_PARAM+stepB),0);
						outputs[TRIGGER_OUTPUT].setVoltage(getVoltNow(SEQ_PARAM+16+stepC),1);
						outputs[TRIGGER_OUTPUT].setVoltage(getVoltNow(SEQ_PARAM+24+stepC),2);
						outputs[TRIGGER_OUTPUT].setVoltage(getVoltNow(SEQ_PARAM+32+stepC),3);
						outputs[TRIGGER_OUTPUT].setVoltage(paramVal[SEQ_PARAM+40+stepRand]*10.0f,4);
					}
					else if (indexSeqMode==6) {	
						stepC=(stepA % 8);
						if (mother->stepC % (int)paramVal[DIV_PARAM] == 0) {stepRand=(rand() % 8);};
						lights[SEQ_LIGHT+stepC].setBrightness(0.999);
						lights[SEQ_LIGHT+8+stepC].setBrightness(0.999);
						lights[SEQ_LIGHT+16+stepC].setBrightness(0.999);
						lights[SEQ_LIGHT+24+stepC].setBrightness(0.999);
						lights[SEQ_LIGHT+32+stepC].setBrightness(0.999);
						lights[SEQ_LIGHT+40+stepRand].setBrightness(0.999);
						outputs[TRIGGER_OUTPUT].setVoltage(getVoltNow(SEQ_PARAM+stepC),0);
						outputs[TRIGGER_OUTPUT].setVoltage(getVoltNow(SEQ_PARAM+8+stepC),1);
						outputs[TRIGGER_OUTPUT].setVoltage(getVoltNow(SEQ_PARAM+16+stepC),2);
						outputs[TRIGGER_OUTPUT].setVoltage(getVoltNow(SEQ_PARAM+24+stepC),3);
						outputs[TRIGGER_OUTPUT].setVoltage(getVoltNow(SEQ_PARAM+32+stepC),4);
						outputs[TRIGGER_OUTPUT].setVoltage(paramVal[SEQ_PARAM+40+stepRand]*10.0f,5);
					}
				}
			}
			else if (mother->offClock==true) {
				if (indexPW==0) {
					for (int c=0;c<outputs[TRIGGER_OUTPUT].channels;c++) {
						outputs[TRIGGER_OUTPUT].setVoltage(0.0f,c);
					}
				}
			}
		}		
		else {	// no mother
			for (int c=0;c<outputs[TRIGGER_OUTPUT].channels;c++) {
				outputs[TRIGGER_OUTPUT].setVoltage(-4.04f,c);
			}			
		}
	}

	#include "PushmeSeq/PushmeSeq_json.hpp"

};

// --------------------------------------------------

struct PushmeMoreWidget : ModuleWidget {

	#include "share/widgetwizard.hpp"   // addChild abbreviations

	PushmeMore* module;
	PushmeMoreWidget(PushmeMore* module) {
		this->module = module;
		setModule(module);
 
		setPanel(createPanel(asset::plugin(pluginInstance, "res/PushmeSeq.svg")));

		for (int b=0;b<48;b++) {
			// RedGreenBlueLight?
			addParam(createLightParamCentered<VCVLightLatch<MediumSimpleLight<GreenLight>>>
				(mm2px(Vec(HP*(floor(b/16)*1.2+0.8), HP*(1.5+1.25*(b % 16)))), 
				module, PushmeMore::SEQ_PARAM+b, PushmeMore::SEQ_LIGHT+b));
			// addParam(createLightParamCentered<VCVButtonSquareGreen>
				// (mm2px(Vec(HP*(floor(b/16)*1.2+0.8), HP*(1.5+1.2*(b % 16)))), 
				// module, PushmeMore::SEQ_PARAM+b, PushmeMore::SEQ_LIGHT+b));
		}
			
		childKnob(PushmeMore::SEQMODE_PARAM, 0, HP*3, HP*21.75);
		childOutput(PushmeMore::TRIGGER_OUTPUT, HP*3, HP*23.5);
		// childInput(PushmeMore::CLOCK_INPUT, HP*1, HP*21.75);
		// childInput(PushmeMore::RESET_INPUT, HP*1, HP*23.5);
		// childOutput(PushmeMore::EOC_OUTPUT, HP*2, HP*24);

		childSwitch(PushmeMore::FETCH_SWITCH, 5, HP*1, HP*21.75);
		childKnob(PushmeMore::DIV_PARAM, 0, HP*1, HP*23.5);

	}

	void appendContextMenu(Menu* menu) override {
		PushmeMore* module = dynamic_cast<PushmeMore*>(this->module);
		assert(module);
		#include "PushmeSeq/PushmeSeq_menu.hpp"
	}

	#include "PushmeSeq/PushmeSeq_keys.hpp"

};

Model* modelPushmeMore = createModel<PushmeMore, PushmeMoreWidget>("PushmeMore");