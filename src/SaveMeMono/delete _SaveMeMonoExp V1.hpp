***********/
// EXPANDER CODE - starting here
/**************************************************/

struct SaveMeMonoExp : Module {
	
	enum ParamId    {START_PARAM, THEND_PARAM, PARAMS_LEN};
	enum InputId    {INPUTS_LEN};
	enum OutputId	{MONO_REPLAY_OUTPUT, REVERSE_REPLAY_OUTPUT, RANDOM_REPLAY_OUTPUT, OUTPUTS_LEN};
	enum LightId	{LIGHTS_LEN};

	SaveMeMonoExp() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(START_PARAM, 	1.0f, 256.0f, 1.0f, "From position");
		configParam(THEND_PARAM, 	1.0f, 256.0f, 8.0f, "To position");
		paramQuantities[START_PARAM]->snapEnabled = true;
		paramQuantities[THEND_PARAM]->snapEnabled = true;
		configOutput(MONO_REPLAY_OUTPUT, "Mono replay"); 
		configOutput(REVERSE_REPLAY_OUTPUT, "Reverse replay"); 
		configOutput(RANDOM_REPLAY_OUTPUT, "Random replay"); 
	}

 	SaveMeMono* findHostModulePtr(Module* module) {
		if (module) {
			if (module->leftExpander.module) {
				// if it's the mother module, we're done
				if (module->leftExpander.module->model == modelSaveMeMono) {
					return reinterpret_cast<SaveMeMono*>(module->leftExpander.module);
				}
			}
		}
		return nullptr;
	}

	// Ctrl-E bypassing: no sound but the counting stays
	void processBypass(const ProcessArgs& args) override {
		outputs[MONO_REPLAY_OUTPUT].setVoltage(1);
		outputs[REVERSE_REPLAY_OUTPUT].setVoltage(2);
		outputs[RANDOM_REPLAY_OUTPUT].setVoltage(3);
	}

	float theSeq[256]={0};	// this contains the sequence, 256 slots available
	int theStep=0;
	int minStep=0;
	int maxStep=0;
	int rndStep=0;
	int START_VAL=0;
	int THEND_VAL=0;
	
	void process(const ProcessArgs& args) override {
		
		SaveMeMono const* mother = findHostModulePtr(this);
		if (mother) {
			if (mother->childClockWarning==true) {
				// let's remember the knob settings
				START_VAL=params[START_PARAM].getValue()-1;
				THEND_VAL=params[THEND_PARAM].getValue()-1;
				// we may need info if the end position is bigger than the start
				minStep=std::min(START_VAL,THEND_VAL);
				maxStep=std::max(START_VAL,THEND_VAL);
				rndStep=(rand() % (maxStep-minStep+1)) + minStep;
				int dirStep=(THEND_VAL>=START_VAL)?1:-1;
				
				// let's calculate the current step
				if (mother->childResetWarning==true) {theStep=START_VAL;}
				else {
					theStep+=dirStep;
					if (dirStep==1) {if (theStep>THEND_VAL) {theStep=START_VAL;}}
					else {if (theStep<THEND_VAL) {theStep=START_VAL;}}
				}
				
				// let's collect the value related to the current step
				outputs[MONO_REPLAY_OUTPUT].setVoltage(mother->theSeq[theStep]);
				// outputs[REVERSE_REPLAY_OUTPUT].setVoltage(mother->theSeq[THEND_VAL-(theStep-START_VAL)-2)]);
				outputs[REVERSE_REPLAY_OUTPUT].setVoltage(mother->theSeq[THEND_VAL-theStep+START_VAL]);
				outputs[RANDOM_REPLAY_OUTPUT].setVoltage(mother->theSeq[rndStep]);
			}
		}
		else {
			outputs[MONO_REPLAY_OUTPUT].setVoltage(-4.04);
			outputs[REVERSE_REPLAY_OUTPUT].setVoltage(-4.04);
			outputs[RANDOM_REPLAY_OUTPUT].setVoltage(-4.04);
		}
		
	}
};

struct SaveMeMonoExpWidget : ModuleWidget {

	SaveMeMonoExp* module;
	#include "share/widgetwizard.hpp"   // addChild abbreviations

	SaveMeMonoExpWidget(SaveMeMonoExp* module) {

		this->module = module;
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/SaveMeMonoExp.svg")));

		// addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

// add a precision / sinner knob 
// mother's PW settings to be considered

		childKnob(SaveMeMonoExp::START_PARAM, 1, HP*1.5, HP*12.5);
		childKnob(SaveMeMonoExp::THEND_PARAM, 1, HP*1.5, HP*15.5);
				
		childOutput(SaveMeMono::MONO_REPLAY_OUTPUT, HP*1.5, HP*19);
		childOutput(SaveMeMono::REVERSE_REPLAY_OUTPUT, HP*1.5, HP*21);
		childOutput(SaveMeMono::RANDOM_REPLAY_OUTPUT, HP*1.5, HP*23);

		childLabel(HP*1, HP*12, "TEST", 8);
	}
};


Model* modelSaveMeMonoExp = createModel<SaveMeMonoExp, SaveMeMonoExpWidget>("SaveMeMonoExp");

