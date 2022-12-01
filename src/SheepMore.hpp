// Copyright (c) 2022 András Szabó
#include "plugin.hpp"

struct SheepMore : Module {

// --------------------------------------------------

	enum ParamId    {
		GATE_TRIGGER_PARAM, START_READING_PARAM, END_READING_PARAM, PARAMS_LEN};

	enum InputId    {INPUTS_LEN};

	enum OutputId    {
		CV_PURE_OUTPUT, CV_INVERT_OUTPUT, CV_PURE_AMPLIFIED_OUTPUT, CV_INVERT_AMPLIFIED_OUTPUT, CV_PURE_MODIFIED_OUTPUT, 
		CV_INVERT_MODIFIED_OUTPUT, GATE_PURE_OUTPUT, GATE_INVERT_OUTPUT, GATE_MODIFIED_OUTPUT, GATE_INVERT_MODIFIED_OUTPUT, 
		OUTPUTS_LEN};

	enum LightId    {TEST_LIGHT, LIGHTS_LEN};

// --------------------------------------------------

	SheepMore() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(GATE_TRIGGER_PARAM, 	0.0f, 1.0f, 0.5f, "Gate trigger");
		// configParam(START_READING_PARAM, 		1.0f, 32.0f, 17.0f, "Start reading");
		// configParam(END_READING_PARAM, 			1.0f, 32.0f, 32.0f, "End reading");

		configOutput(CV_PURE_OUTPUT, "CV pure"); 
		configOutput(CV_INVERT_OUTPUT, "CV invert"); 
		configOutput(CV_PURE_AMPLIFIED_OUTPUT, "CV pure amplified"); 
		configOutput(CV_INVERT_AMPLIFIED_OUTPUT, "CV invert amplified"); 
		configOutput(CV_PURE_MODIFIED_OUTPUT, "CV pure modified"); 
		configOutput(CV_INVERT_MODIFIED_OUTPUT, "CV invert modified"); 
		configOutput(GATE_PURE_OUTPUT, "Gate pure"); 
		configOutput(GATE_INVERT_OUTPUT, "Gate invert"); 
		configOutput(GATE_MODIFIED_OUTPUT, "Gate modified"); 
		configOutput(GATE_INVERT_MODIFIED_OUTPUT, "Gate invert  modified"); 
	}

 	Sheep* findHostModulePtr(Module* module) {
		if (module) {
			if (module->leftExpander.module) {
				// if it's Sheep, we're done
				if (module->leftExpander.module->model == modelSheep) {
					return reinterpret_cast<Sheep*>(module->leftExpander.module);
				}
				// if it's a copy of this expander, keep recursing
				else if (module->leftExpander.module->model == modelSheepMore) {
					return findHostModulePtr(module->leftExpander.module);
				}
			}
		}
		return nullptr;
	}

 // --------------------------------------------------

	float newVolt=0;
	float newRange=0;
	bool newMutant=false;
	float modVolt=0;
	float indexGateTrsh=0.5;
		
	void process(const ProcessArgs& args) override {

		Sheep const* mother = findHostModulePtr(this);

		if (mother) {
						
			if (mother->offClock) {
				// lights[TEST_LIGHT].setBrightness(0);
				outputs[GATE_PURE_OUTPUT].setVoltage(0);
				outputs[GATE_INVERT_OUTPUT].setVoltage(0);
				outputs[GATE_MODIFIED_OUTPUT].setVoltage(0);
				outputs[GATE_INVERT_MODIFIED_OUTPUT].setVoltage(0);			
			}
			else if (mother->hitClock) {
				
				// collection some data from mother
				newVolt=mother->theSeq[mother->currStep-1];
				newRange=mother->paramVal[mother->RANGE_PARAM];
				newMutant=rack::random::uniform()<=mother->paramVal[mother->MUTATE_PARAM];

				// simplifying conditions below
				indexGateTrsh=params[GATE_TRIGGER_PARAM].getValue();
				modVolt=(newMutant)?rack::random::uniform():newVolt;
				// lights[TEST_LIGHT].setBrightness(0.5+rack::random::uniform()/2);
	
				// sending the CV outputs
				if (outputs[CV_PURE_OUTPUT].isConnected()) {
					outputs[CV_PURE_OUTPUT].setVoltage(newVolt);
				};
				if (outputs[CV_INVERT_OUTPUT].isConnected()) {
					outputs[CV_INVERT_OUTPUT].setVoltage(1-newVolt);
				};
				if (outputs[CV_PURE_AMPLIFIED_OUTPUT].isConnected()) {
					outputs[CV_PURE_AMPLIFIED_OUTPUT].setVoltage(newVolt*newRange);	
				};
				if (outputs[CV_INVERT_AMPLIFIED_OUTPUT].isConnected()) {
					outputs[CV_INVERT_AMPLIFIED_OUTPUT].setVoltage((1-newVolt)*newRange);
				};
				if (outputs[CV_PURE_MODIFIED_OUTPUT].isConnected()) {
					outputs[CV_PURE_MODIFIED_OUTPUT].setVoltage(modVolt*newRange);
				};
				if (outputs[CV_INVERT_MODIFIED_OUTPUT].isConnected()) {
					outputs[CV_INVERT_MODIFIED_OUTPUT].setVoltage((1-modVolt)*newRange);
				};

				// sending the gate outputs
				if (outputs[GATE_PURE_OUTPUT].isConnected()) {
					outputs[GATE_PURE_OUTPUT].setVoltage((newVolt>indexGateTrsh)?10:0);
				};
				if (outputs[GATE_INVERT_OUTPUT].isConnected()) {
					outputs[GATE_INVERT_OUTPUT].setVoltage((newVolt>indexGateTrsh)?0:10);	// OK
				};
				if (outputs[GATE_MODIFIED_OUTPUT].isConnected()) {
					outputs[GATE_MODIFIED_OUTPUT].setVoltage((modVolt>indexGateTrsh)?10:0);
				};
				if (outputs[GATE_INVERT_MODIFIED_OUTPUT].isConnected()) {
					outputs[GATE_INVERT_MODIFIED_OUTPUT].setVoltage((modVolt>indexGateTrsh)?0:10);	
				};
			}
		
		}
	}

	// #include "SheepMore/SheepMore_json.hpp"

};

// --------------------------------------------------

struct SheepMoreWidget : ModuleWidget {

	SheepMore* module;
	#include "share/widgetwizard.hpp"   // addChild abbreviations

	SheepMoreWidget(SheepMore* module) {

		this->module = module;
		setModule(module);
		// setPanel(createPanel(asset::plugin(pluginInstance, "res/components/Empty2hp.svg")));
		setPanel(createPanel(asset::plugin(pluginInstance, "res/SheepMore.svg")));
				
		// childKnob(SheepMore::START_READING_PARAM, 0, 		HP*1, HP*2.25);
		// childKnob(SheepMore::END_READING_PARAM, 0, 		HP*1, HP*4.25);
		childOutput(SheepMore::CV_PURE_OUTPUT, 				HP*1, HP*2.25);
		childOutput(SheepMore::CV_INVERT_OUTPUT, 			HP*1, HP*4.25);

		childOutput(SheepMore::CV_PURE_AMPLIFIED_OUTPUT, 	HP*1, HP*6.5-HP*0.25);
		childOutput(SheepMore::CV_INVERT_AMPLIFIED_OUTPUT, 	HP*1, HP*8.5-HP*0.25);
		childOutput(SheepMore::CV_PURE_MODIFIED_OUTPUT, 	HP*1, HP*10.5-HP*0.25);
		childOutput(SheepMore::CV_INVERT_MODIFIED_OUTPUT, 	HP*1, HP*12.5-HP*0.25);
		
		childKnob(SheepMore::GATE_TRIGGER_PARAM, 0, 		HP*1, HP*14.5);
		childOutput(SheepMore::GATE_PURE_OUTPUT, 			HP*1, HP*16.5);
		childOutput(SheepMore::GATE_INVERT_OUTPUT, 			HP*1, HP*18.5);
		childOutput(SheepMore::GATE_MODIFIED_OUTPUT, 		HP*1, HP*20.5);
		childOutput(SheepMore::GATE_INVERT_MODIFIED_OUTPUT, HP*1, HP*22.5);

		// childLight(SheepMore::TEST_LIGHT, 0, HP*1, HP*24.5);
		// childLabel(HP*1,HP*1, "DEV", 12);
	}

};

Model* modelSheepMore = createModel<SheepMore, SheepMoreWidget>("SheepMore");
