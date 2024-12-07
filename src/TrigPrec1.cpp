/* README.md

### TrigPrec1

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

struct TrigPrec1 : Module {
	enum ParamId {
		KNOB1_PARAM,
		RNDBUTTON1_PARAM,
		RNDBUTTON2_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		IN1_INPUT,
		KNOB1_INPUT,
		CLK_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		OUT1_OUTPUT,
		OUT2_OUTPUT,
		OUT3_OUTPUT,
		OUT4_OUTPUT,
		DEBUG_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		RNDLED1_LIGHT,
		RNDLED2_LIGHT,
		LIGHTS_LEN
	};

	TrigPrec1() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configInput(CLK_INPUT, "Triggers to add");
		configSwitch(RNDBUTTON1_PARAM, 0.f, 1.f, 1.f, "Skip triggers",{"Off","On"});
		configSwitch(RNDBUTTON2_PARAM, 0.f, 1.f, 1.f, "Add triggers",{"Off","On"});
		configParam(KNOB1_PARAM, 0.f, 1.f, 0.95f, "Precision 1");
		configInput(KNOB1_INPUT, "Precision CV");	// add
		configInput(IN1_INPUT, "Triggers 1 to skip");
		
		// configParam(KNOB2_PARAM, 0.f, 1.f, 0.95f, "Precision 2");
		// configInput(IN2_INPUT, "Triggers 2 to skip");
		configOutput(OUT1_OUTPUT, "Output with addition");
		configOutput(OUT2_OUTPUT, "Output with skips");
		configOutput(OUT3_OUTPUT, "Output variant A");
		configOutput(OUT4_OUTPUT, "Output  variant B");
		configOutput(DEBUG_OUTPUT, "DEBUG");
	}

float newVal=0;
float clkVal=0;
float paramVal[PARAMS_LEN]={0.95};
float inputVal[INPUTS_LEN]={0};
bool hideTrig[OUTPUTS_LEN]={false};
int loopKnobs=1;
bool addsw=true;
bool skpsw=true;

bool newTrig=false;
bool oldTrig=false;
bool shwTrig[OUTPUTS_LEN]={true};
bool newGhost=false;
bool oldGhost=false;
bool shwGhost[OUTPUTS_LEN]={false};
float newVolt=0.0f;
float xVal=0.0f;

    // the main routine
	void process(const ProcessArgs& args) override {

		// sometimes collect button and knob information
		if (loopKnobs-- ==0) {
			loopKnobs=500;	
			// first the buttons to addsw and skpsw
			skpsw = params[RNDBUTTON1_PARAM].getValue() > 0.f;
			lights[RNDLED1_LIGHT].setBrightness(skpsw);
			if (!inputs[IN1_INPUT].isConnected()) {skpsw=false;}
			addsw = params[RNDBUTTON2_PARAM].getValue() > 0.f;
			lights[RNDLED2_LIGHT].setBrightness(addsw);
			if (!inputs[CLK_INPUT].isConnected()) {addsw=false;}
		};				

		// well, it's anything but efficient :(
		if (inputs[KNOB1_INPUT].isConnected()) {
			params[KNOB1_PARAM].setValue(abs(inputs[KNOB1_INPUT].getVoltage()/10));
		}
		
		// case 1: no change to the original sequences
		if (!addsw && !skpsw) {
			newVolt=inputs[IN1_INPUT].getVoltage();
			inputVal[IN1_INPUT]=newVolt;
			outputs[OUT1_OUTPUT].setVoltage((newVolt>2)?10:0);		
			outputs[OUT2_OUTPUT].setVoltage((newVolt>2)?10:0);		
			outputs[OUT3_OUTPUT].setVoltage((newVolt>2)?10:0);		
			outputs[OUT4_OUTPUT].setVoltage((newVolt>2)?10:0);		
		}
		// case 2: a few triggers can be skipped
		else if (!addsw && skpsw) {
			oldTrig=inputVal[IN1_INPUT]>2;
			newTrig=inputs[IN1_INPUT].getVoltage()>2;
			inputVal[IN1_INPUT]=newTrig?10:0;
			if (newTrig) {
				newVolt=10;
				if (!oldTrig) {
					xVal=params[KNOB1_PARAM].getValue();
					hideTrig[0]=(rack::random::uniform()>xVal);
					hideTrig[1]=(rack::random::uniform()>xVal);
					hideTrig[2]=(rack::random::uniform()>xVal);
					hideTrig[3]=(rack::random::uniform()>xVal);
					}
			} 
			else if (!newTrig) {newVolt=0;}
			outputs[OUT1_OUTPUT].setVoltage(hideTrig[0]?0:newVolt);					
			outputs[OUT2_OUTPUT].setVoltage(hideTrig[1]?0:newVolt);					
			outputs[OUT3_OUTPUT].setVoltage(hideTrig[2]?0:newVolt);					
			outputs[OUT4_OUTPUT].setVoltage(hideTrig[3]?0:newVolt);					
		}
		// case 3: a few triggers can be added from the CLK input
		else if (addsw && !skpsw) {
			newGhost=inputs[CLK_INPUT].getVoltage()>2;	// get new CLK status
			oldGhost=inputVal[CLK_INPUT]>2;				// get old CLK status
			inputVal[CLK_INPUT]=newGhost?10:0;			// save new CLK status
			newTrig=inputs[IN1_INPUT].getVoltage()>2;
			if (newTrig) {newVolt=10;
				outputs[OUT1_OUTPUT].setVoltage(10);		
				outputs[OUT2_OUTPUT].setVoltage(10);		
				outputs[OUT3_OUTPUT].setVoltage(10);		
				outputs[OUT4_OUTPUT].setVoltage(10);		
			}				// all hits of the main signal is needed
			else {									// newTrig==false
				if (newGhost && !oldGhost) {		// new ghost CLK on
					xVal=params[KNOB1_PARAM].getValue();
					shwGhost[0]=rack::random::uniform()>xVal;
					shwGhost[1]=rack::random::uniform()>xVal;
					shwGhost[2]=rack::random::uniform()>xVal;
					shwGhost[3]=rack::random::uniform()>xVal;
					newVolt=shwGhost[0]?9:0.1;			// bleeding :)
					outputs[OUT1_OUTPUT].setVoltage(shwGhost[0]?9:0.1);		
					outputs[OUT2_OUTPUT].setVoltage(shwGhost[1]?9:0.1);		
					outputs[OUT3_OUTPUT].setVoltage(shwGhost[2]?9:0.1);		
					outputs[OUT4_OUTPUT].setVoltage(shwGhost[3]?9:0.1);		
				}
				else if (newGhost && oldGhost) {
					newVolt=shwGhost[0]?8:0.2;
					outputs[OUT1_OUTPUT].setVoltage(shwGhost[0]?8:0.2);		
					outputs[OUT2_OUTPUT].setVoltage(shwGhost[1]?8:0.2);		
					outputs[OUT3_OUTPUT].setVoltage(shwGhost[2]?8:0.2);		
					outputs[OUT4_OUTPUT].setVoltage(shwGhost[3]?8:0.2);		
					}	// bleeding :)
				else {newVolt=0;
					outputs[OUT1_OUTPUT].setVoltage(0);		
					outputs[OUT2_OUTPUT].setVoltage(0);		
					outputs[OUT3_OUTPUT].setVoltage(0);		
					outputs[OUT4_OUTPUT].setVoltage(0);		
				}					
			}
		} 	
// itt tartok
		// case 4: a few triggers can be added from CLK and skipped from the original input
		else if (addsw && skpsw) {
			newGhost=inputs[CLK_INPUT].getVoltage()>2;	// get new CLK status
			oldGhost=inputVal[CLK_INPUT]>2;				// get old CLK status
			inputVal[CLK_INPUT]=newGhost?10:0;			// save new CLK status
			newTrig=inputs[IN1_INPUT].getVoltage()>2;
			oldTrig=inputVal[IN1_INPUT]>2;
			inputVal[IN1_INPUT]=newTrig?10:0.4;
			if (newTrig) {
				if (!oldTrig) {
					xVal=params[KNOB1_PARAM].getValue();					
					shwTrig[0]=rack::random::uniform()<xVal;
					shwTrig[1]=rack::random::uniform()<xVal;
					shwTrig[2]=rack::random::uniform()<xVal;
					shwTrig[3]=rack::random::uniform()<xVal;
					}
				newVolt=shwTrig[KNOB1_PARAM]?10:0.5;	// bleeding :)
				outputs[OUT1_OUTPUT].setVoltage(shwTrig[0]?10:0.5);		
				outputs[OUT2_OUTPUT].setVoltage(shwTrig[1]?10:0.5);		
				outputs[OUT3_OUTPUT].setVoltage(shwTrig[2]?10:0.5);		
				outputs[OUT4_OUTPUT].setVoltage(shwTrig[3]?10:0.5);					
				}								
			else {									// newTrig==false
				if (newGhost && !oldGhost) {		// new ghost CLK on
					xVal=params[KNOB1_PARAM].getValue();					
					// shwGhost[KNOB1_PARAM+c]=rack::random::uniform()>params[KNOB1_PARAM+c].getValue();
					shwGhost[0]=rack::random::uniform()>xVal;
					shwGhost[1]=rack::random::uniform()>xVal;
					shwGhost[2]=rack::random::uniform()>xVal;
					shwGhost[3]=rack::random::uniform()>xVal;
					newVolt=shwGhost[0]?9:0.1;			// bleeding :)
					outputs[OUT1_OUTPUT].setVoltage(shwGhost[0]?9:0.1);		
					outputs[OUT2_OUTPUT].setVoltage(shwGhost[1]?9:0.1);		
					outputs[OUT3_OUTPUT].setVoltage(shwGhost[2]?9:0.1);		
					outputs[OUT4_OUTPUT].setVoltage(shwGhost[3]?9:0.1);		
				}
				else if (newGhost && oldGhost) {
					newVolt=shwGhost[0]?8:0.2;	// bleeding :)
					outputs[OUT1_OUTPUT].setVoltage(shwGhost[0]?8:0.2);		
					outputs[OUT2_OUTPUT].setVoltage(shwGhost[1]?8:0.2);		
					outputs[OUT3_OUTPUT].setVoltage(shwGhost[2]?8:0.2);		
					outputs[OUT4_OUTPUT].setVoltage(shwGhost[3]?8:0.2);		
					}
				else {
					newVolt=0;
					outputs[OUT1_OUTPUT].setVoltage(0);		
					outputs[OUT2_OUTPUT].setVoltage(0);		
					outputs[OUT3_OUTPUT].setVoltage(0);		
					outputs[OUT4_OUTPUT].setVoltage(0);						
				}					
			}
			oldTrig=newTrig;
			// outputs[OUT1_OUTPUT+c].setVoltage(newVolt);		
		}
	};
};

struct TrigPrec1Widget : ModuleWidget {

    TrigPrec1* module;

	TrigPrec1Widget(TrigPrec1* module) {
        this->module = module;
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/TrigPrec1.svg")));

		// addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		float HP=5.08f;

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(HP*1.5, 2*HP)), module, TrigPrec1::CLK_INPUT));
		addParam(createLightParamCentered<VCVLightLatch<MediumSimpleLight<WhiteLight>>>(mm2px(Vec(HP*1.5, 4.5*HP)), module, TrigPrec1::RNDBUTTON2_PARAM, TrigPrec1::RNDLED2_LIGHT));
		addParam(createLightParamCentered<VCVLightLatch<MediumSimpleLight<WhiteLight>>>(mm2px(Vec(HP*1.5, 6.5*HP)), module, TrigPrec1::RNDBUTTON1_PARAM, TrigPrec1::RNDLED1_LIGHT));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(HP*1.5, 9.5*HP)), module, TrigPrec1::KNOB1_PARAM));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(HP*1.5, 12*HP)), module, TrigPrec1::KNOB1_INPUT));
		
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(HP*1.5, 14.5*HP)), module, TrigPrec1::IN1_INPUT));
		// addInput(createInputCentered<PJ301MPort>(mm2px(Vec(HP*1.5, 99.06)), module, TrigPrec1::IN2_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(HP*1.5, 17*HP)), module, TrigPrec1::OUT1_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(HP*1.5, 19*HP)), module, TrigPrec1::OUT2_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(HP*1.5, 21*HP)), module, TrigPrec1::OUT3_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(HP*1.5, 23*HP)), module, TrigPrec1::OUT4_OUTPUT));
		// addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(HP*1.5, 86.36)), module, TrigPrec1::KNOB2_PARAM));
		// addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(HP*1.5, 128.5-HP)), module, TrigPrec1::DEBUG_OUTPUT));

	}

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


Model* modelTrigPrec1 = createModel<TrigPrec1, TrigPrec1Widget>("TrigPrec1");