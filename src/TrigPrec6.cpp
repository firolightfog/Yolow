/* README.md

### TrigPrec6

This module is to modify the incoming six trigger sequences by randomly removing or adding pulses.

##### BUTTON:
- `SKIP` skips a few triggers from the sequence received from the `IN 1-6` inputs
- `ADD` adds a few triggers from the `CLK` input
##### INPUTS:
- `IN 1-6` inputs receive the trigger sequences
##### KNOBS:
- Six knobs are to define the probability/precision threshold. Setting it to 80% could randomly skip 20% of the original triggers and add about 20% of the `CLK` input. (Theoretically.)
##### OUTPUTS:
- `OUT 1-6` outputs the modified sequences.
##### ALSO RECOMMENDED:
- A similar effect can be achieved with a Bernoulli gate and a XOR module.  

*/

#include "plugin.hpp"


struct TrigPrec6 : Module {
	enum ParamId {
		KNOB1_PARAM,
		KNOB2_PARAM,
		KNOB3_PARAM,
		KNOB4_PARAM,
		KNOB5_PARAM,
		KNOB6_PARAM,
		RNDBUTTON1_PARAM,
		RNDBUTTON2_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		IN1_INPUT,
		IN2_INPUT,
		IN3_INPUT,
		IN4_INPUT,
		IN5_INPUT,
		IN6_INPUT,
		CLK_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		OUT1_OUTPUT,
		OUT2_OUTPUT,
		OUT3_OUTPUT,
		OUT4_OUTPUT,
		OUT5_OUTPUT,
		OUT6_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		RNDLED1_LIGHT,
		RNDLED2_LIGHT,
		LIGHTS_LEN
	};

	TrigPrec6() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(KNOB1_PARAM, 0.f, 1.f, 0.95f, "Precision");
		configParam(KNOB2_PARAM, 0.f, 1.f, 0.95f, "Precision");
		configParam(KNOB3_PARAM, 0.f, 1.f, 0.95f, "Precision");
		configParam(KNOB4_PARAM, 0.f, 1.f, 0.95f, "Precision");
		configParam(KNOB5_PARAM, 0.f, 1.f, 0.95f, "Precision");
		configParam(KNOB6_PARAM, 0.f, 1.f, 0.95f, "Precision");
		configSwitch(RNDBUTTON1_PARAM, 0.f, 1.f, 1.f, "Skip triggers",{"Off","On"});
		configSwitch(RNDBUTTON2_PARAM, 0.f, 1.f, 1.f, "Add triggers",{"Off","On"});
		configInput(IN1_INPUT, "Triggers to skip");
		configInput(IN2_INPUT, "Triggers to skip");
		configInput(IN3_INPUT, "Triggers to skip");
		configInput(IN4_INPUT, "Triggers to skip");
		configInput(IN5_INPUT, "Triggers to skip");
		configInput(IN6_INPUT, "Triggers to skip");
		configInput(CLK_INPUT, "Triggers to add");
		configOutput(OUT1_OUTPUT, "");
		configOutput(OUT2_OUTPUT, "");
		configOutput(OUT3_OUTPUT, "");
		configOutput(OUT4_OUTPUT, "");
		configOutput(OUT5_OUTPUT, "");
		configOutput(OUT6_OUTPUT, "");
	}

float newVal=0;
float clkVal=0;
float paramVal[PARAMS_LEN]={0.95};
float inputVal[INPUTS_LEN]={0};
bool hideTrig[INPUTS_LEN]={false};
int loopKnobs=1;
bool addsw=true;
bool skpsw=true;

bool newTrig=false;
bool oldTrig=false;
bool shwTrig[INPUTS_LEN]={true};
bool newGhost=false;
bool oldGhost=false;
bool shwGhost[INPUTS_LEN]={false};
float newVolt=0;

int totalNr=6;

    // the main routine
	void process(const ProcessArgs& args) override {

		// sometimes collect button and knob information
		if (loopKnobs-- ==0) {
			loopKnobs=500;	
			// first the buttons to addsw and skpsw
			skpsw = params[RNDBUTTON1_PARAM].getValue() > 0.f;
			lights[RNDLED1_LIGHT].setBrightness(skpsw);
			addsw = params[RNDBUTTON2_PARAM].getValue() > 0.f;
			lights[RNDLED2_LIGHT].setBrightness(addsw);
		};				

		// well, it's anything but efficient :(
		// case 1: no change to the original sequences
		if (!addsw && !skpsw) {
			for (int c=0; c<totalNr; c++) {
				newVolt=inputs[IN1_INPUT+c].getVoltage();
				inputVal[IN1_INPUT+c]=newVolt;
				outputs[OUT1_OUTPUT+c].setVoltage((newVolt>2)?10:0);		
			}
		}
		// case 2: a few triggers can be skipped
		else if (!addsw && skpsw) {
			for (int c=0; c<totalNr; c++) {
				oldTrig=inputVal[IN1_INPUT+c]>2;
				newTrig=inputs[IN1_INPUT+c].getVoltage()>2;
				inputVal[IN1_INPUT+c]=newTrig?10:0;
				if (newTrig) {
					newVolt=10;
					if (!oldTrig) {hideTrig[IN1_INPUT+c]=(rack::random::uniform()>params[KNOB1_PARAM+c].getValue());}
				}
				else if (!newTrig) {newVolt=0;}
				outputs[OUT1_OUTPUT+c].setVoltage(hideTrig[IN1_INPUT+c]?0:newVolt);					
			} 	
		}
		// case 3: a few triggers can be added from the CLK input
		else if (addsw && !skpsw) {
			newGhost=inputs[CLK_INPUT].getVoltage()>2;	// get new CLK status
			oldGhost=inputVal[CLK_INPUT]>2;				// get old CLK status
			inputVal[CLK_INPUT]=newGhost?10:0;			// save new CLK status
			for (int c=0; c<totalNr; c++) {
				newTrig=inputs[IN1_INPUT+c].getVoltage()>2;
				if (newTrig) {newVolt=10;}				// all hits of the main signal is needed
				else {									// newTrig==false
					if (newGhost && !oldGhost) {		// new ghost CLK on
						shwGhost[KNOB1_PARAM+c]=rack::random::uniform()>params[KNOB1_PARAM+c].getValue();
						newVolt=shwGhost[KNOB1_PARAM+c]?9:0.1;			// bleeding :)
					}
					else if (newGhost && oldGhost) {newVolt=shwGhost[KNOB1_PARAM+c]?8:0.2;}	// bleeding :)
					else {newVolt=0;}					
				}
				outputs[OUT1_OUTPUT+c].setVoltage(newVolt);		
			}
		} 	
		// case 4: a few triggers can be added from CLK and skipped from the original input
		else if (addsw && skpsw) {
			newGhost=inputs[CLK_INPUT].getVoltage()>2;	// get new CLK status
			oldGhost=inputVal[CLK_INPUT]>2;				// get old CLK status
			inputVal[CLK_INPUT]=newGhost?10:0;			// save new CLK status
			for (int c=0; c<totalNr; c++) {
				newTrig=inputs[IN1_INPUT+c].getVoltage()>2;
				oldTrig=inputVal[IN1_INPUT+c]>2;
				inputVal[IN1_INPUT+c]=newTrig?10:0.4;
				if (newTrig) {
					if (!oldTrig) {shwTrig[KNOB1_PARAM+c]=rack::random::uniform()<params[KNOB1_PARAM+c].getValue();}
					newVolt=shwTrig[KNOB1_PARAM+c]?10:0.5;	// bleeding :)
					}								
				else {									// newTrig==false
					if (newGhost && !oldGhost) {		// new ghost CLK on
						shwGhost[KNOB1_PARAM+c]=rack::random::uniform()>params[KNOB1_PARAM+c].getValue();
						newVolt=shwGhost[KNOB1_PARAM+c]?9:0.1;			// bleeding :)
					}
					else if (newGhost && oldGhost) {newVolt=shwGhost[KNOB1_PARAM+c]?8:0.2;}	// bleeding :)
					else {newVolt=0;}					
				}
				oldTrig=newTrig;
				outputs[OUT1_OUTPUT+c].setVoltage(newVolt);		
			}
		}
	};
};

/*
    // the main routine
	void process(const ProcessArgs& args) override {
		
		// sometimes collect button and knob information
		if (loopKnobs-- ==0) {
			loopKnobs=500;	
			// first the buttons to addsw and skpsw
			skpsw = params[RNDBUTTON1_PARAM].getValue() > 0.f;
			lights[RNDLED1_LIGHT].setBrightness(skpsw);
			addsw = params[RNDBUTTON2_PARAM].getValue() > 0.f;
			lights[RNDLED2_LIGHT].setBrightness(addsw);
			// then the knobs to paramVal[] array
			for (int c=0; c<6; c++) {
				paramVal[KNOB1_PARAM+c]=params[KNOB1_PARAM+c].getValue();
			}
		};				

		// well, it's anything but efficient :(
		if (addsw) {newGhost=inputs[CLK_INPUT].getVoltage()>2;};
		// case 1: no change to the original sequences
		if (!addsw && !skpsw) {
			for (int c=0; c<6; c++) {
				newVolt=inputs[IN1_INPUT+c].getVoltage();
				inputVal[IN1_INPUT+c]=newVolt>2;
				outputs[OUT1_OUTPUT+c].setVoltage(newVolt);		
			}
		}
		// case 2: a few triggers can be skipped
		else if (!addsw && skpsw) {
			for (int c=0; c<6; c++) {
				newTrig=inputs[IN1_INPUT+c].getVoltage()>2;
				if (inputVal[IN1_INPUT+c]!=newTrig) { 	// there is a change in main clock			
					if (newTrig && rack::random::uniform()<paramVal[KNOB1_PARAM+c]) {newVolt=10.f;}
					else {newVolt=0.f;} 					// clock is off or not precise
				inputVal[IN1_INPUT+c]=newTrig;		
				outputs[OUT1_OUTPUT+c].setVoltage(newVolt);					
				} 	
			} 	
		}
		// case 3: a few triggers can added from the CLK input
		else if (addsw && !skpsw) {
			for (int c=0; c<6; c++) {
				newVolt=inputs[IN1_INPUT+c].getVoltage();
				if (!newGhost) {}
				else if (newVolt>2) {}
				else if (inputVal[CLK_INPUT]==newGhost) {} 
				else if (rack::random::uniform()<paramVal[KNOB1_PARAM+c]) {;}
				else {newVolt=9.f;}
				outputs[OUT1_OUTPUT+c].setVoltage(newVolt);					
				inputVal[IN1_INPUT+c]=newVolt>2;
			} 	
		} 	
		// case 4: a few triggers can be added from CLK and skipped from the original input
		else if (addsw && skpsw) {
			for (int c=0; c<6; c++) {
				newVolt=inputs[IN1_INPUT+c].getVoltage();
				newTrig=newVolt>2;

				if (!newGhost) {}
				else if (newVolt>2) {;}
				else if (inputVal[CLK_INPUT]==newGhost) {} 
				else if (rack::random::uniform()<paramVal[KNOB1_PARAM+c]) {}
				else {newVolt=9.f;}

				if (inputVal[IN1_INPUT+c]!=newTrig) { 	// there is a change in main clock			
					if (newTrig && rack::random::uniform()<paramVal[KNOB1_PARAM+c]) {newVolt=10.f;}
					else {newVolt=0.f;} 					// clock is off or not precise
				} 	
				outputs[OUT1_OUTPUT+c].setVoltage(newVolt);					
				inputVal[IN1_INPUT+c]=newVolt>2;		
			} 	
		}
		inputVal[CLK_INPUT]=newGhost;
	};
};
*/


struct TrigPrec6Widget : ModuleWidget {

    TrigPrec6* module;

	TrigPrec6Widget(TrigPrec6* module) {
        this->module = module;
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/TrigPrec6.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(20.32, 30.48)), module, TrigPrec6::KNOB1_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(20.32, 43.18)), module, TrigPrec6::KNOB2_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(20.32, 55.88)), module, TrigPrec6::KNOB3_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(20.32, 68.58)), module, TrigPrec6::KNOB4_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(20.32, 81.28)), module, TrigPrec6::KNOB5_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(20.32, 93.98)), module, TrigPrec6::KNOB6_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(33.02, 12.7)), module, TrigPrec6::CLK_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.62, 30.48)), module, TrigPrec6::IN1_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.62, 43.18)), module, TrigPrec6::IN2_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.62, 55.88)), module, TrigPrec6::IN3_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.62, 68.58)), module, TrigPrec6::IN4_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.62, 81.28)), module, TrigPrec6::IN5_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.62, 93.98)), module, TrigPrec6::IN6_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(33.02, 30.48)), module, TrigPrec6::OUT1_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(33.02, 43.18)), module, TrigPrec6::OUT2_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(33.02, 55.88)), module, TrigPrec6::OUT3_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(33.02, 68.58)), module, TrigPrec6::OUT4_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(33.02, 81.28)), module, TrigPrec6::OUT5_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(33.02, 93.98)), module, TrigPrec6::OUT6_OUTPUT));

		addParam(createLightParamCentered<VCVLightLatch<MediumSimpleLight<WhiteLight>>>(mm2px(Vec(15.24, 114.3)), module, TrigPrec6::RNDBUTTON1_PARAM, TrigPrec6::RNDLED1_LIGHT));
		addParam(createLightParamCentered<VCVLightLatch<MediumSimpleLight<WhiteLight>>>(mm2px(Vec(33.02, 114.3)), module, TrigPrec6::RNDBUTTON2_PARAM, TrigPrec6::RNDLED2_LIGHT));

	}

    // void appendContextMenu(Menu* menu) override {
    // TrigPrec6* module = dynamic_cast<TrigPrec6*>(this->module);
    // assert(module);
    // menu->addChild(new MenuSeparator);
    // menu->addChild(createMenuLabel("l: Toggle link button"));
    // menu->addChild(createMenuLabel("1-9: Set top precision between 10%-90%"));
    // }

	// void onHoverKey(const event::HoverKey &e) override {
		// if (e.key >= GLFW_KEY_1 && e.key <= GLFW_KEY_4) {
			// if (e.action == GLFW_PRESS) {
				// float key_number = e.key - 49; // 49 is the ascii number for key #1
				// module->contextRnd(key_number);
				// e.consume(this);
			// }
		// }
	// ModuleWidget::onHoverKey(e);
	// }

};


Model* modelTrigPrec6 = createModel<TrigPrec6, TrigPrec6Widget>("TrigPrec6");