/* README.md

### TrigPrec2

This module is to modify the incoming two trigger sequences by randomly removing or adding pulses.

##### BUTTON:
- `SKIP` skips a few triggers from the sequence received from the `IN 1-2` inputs
- `ADD` adds a few triggers from the `CLK` input
##### INPUTS:
- `IN 1-2` inputs receive the trigger sequences
##### KNOBS:
- Six knobs are to define the probability/precision threshold. Setting it to 80% could randomly skip 20% of the original triggers and add about 20% of the `CLK` input. (Theoretically.)
##### OUTPUTS:
- `OUT 1-2` outputs the modified sequences.
##### ALSO RECOMMENDED:
- A similar effect can be achieved with a Bernoulli gate and a XOR module. 

*/

#include "plugin.hpp"

struct TrigPrec2 : Module {
	enum ParamId {
		KNOB1_PARAM,
		KNOB2_PARAM,
		RNDBUTTON1_PARAM,
		RNDBUTTON2_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		IN1_INPUT,
		IN2_INPUT,
		CLK_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		OUT1_OUTPUT,
		OUT2_OUTPUT,
		DEBUG_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		RNDLED1_LIGHT,
		RNDLED2_LIGHT,
		LIGHTS_LEN
	};

	TrigPrec2() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(KNOB1_PARAM, 0.f, 1.f, 0.95f, "Precision 1");
		configParam(KNOB2_PARAM, 0.f, 1.f, 0.95f, "Precision 2");
		configSwitch(RNDBUTTON1_PARAM, 0.f, 1.f, 1.f, "Skip triggers",{"Off","On"});
		configSwitch(RNDBUTTON2_PARAM, 0.f, 1.f, 1.f, "Add triggers",{"Off","On"});
		configInput(IN1_INPUT, "Triggers 1 to skip");
		configInput(IN2_INPUT, "Triggers 2 to skip");
		configInput(CLK_INPUT, "Triggers to add");
		configOutput(OUT1_OUTPUT, "Output 1");
		configOutput(OUT2_OUTPUT, "Output 2");
		configOutput(DEBUG_OUTPUT, "DEBUG");
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

int totalNr=2;

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


struct TrigPrec2Widget : ModuleWidget {

    TrigPrec2* module;

	TrigPrec2Widget(TrigPrec2* module) {
        this->module = module;
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/TrigPrec2.svg")));

		// addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(5.08, 48.26)), module, TrigPrec2::KNOB1_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(5.08, 86.36)), module, TrigPrec2::KNOB2_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(5.08, 10.16)), module, TrigPrec2::CLK_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(5.08, 60.96)), module, TrigPrec2::IN1_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(5.08, 99.06)), module, TrigPrec2::IN2_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(5.08, 73.66)), module, TrigPrec2::OUT1_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(5.08, 111.76)), module, TrigPrec2::OUT2_OUTPUT));
		// addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(5.08, 128.5-5.08)), module, TrigPrec2::DEBUG_OUTPUT));

		addParam(createLightParamCentered<VCVLightLatch<MediumSimpleLight<WhiteLight>>>(mm2px(Vec(5.08, 22.86)), module, TrigPrec2::RNDBUTTON2_PARAM, TrigPrec2::RNDLED2_LIGHT));
		addParam(createLightParamCentered<VCVLightLatch<MediumSimpleLight<WhiteLight>>>(mm2px(Vec(5.08, 33.02)), module, TrigPrec2::RNDBUTTON1_PARAM, TrigPrec2::RNDLED1_LIGHT));

	}

    // void appendContextMenu(Menu* menu) override {
    // TrigPrec2* module = dynamic_cast<TrigPrec2*>(this->module);
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


Model* modelTrigPrec2 = createModel<TrigPrec2, TrigPrec2Widget>("TrigPrec2");