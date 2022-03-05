/* README.md

### Euclk

This is a sequencer module. 

##### INPUTS:
- `RST` resets the sequencer
- `CLK` advances to the next step 
- 'LENGTH' modifes the relevant knob value
- 'HITS' modifes the relevant knob value
- 'SHIFT' modifes the relevant knob value
##### KNOBS:
- 'LENGTH' defines the length of the pattern (range 0-64) 
- 'HITS' defines the number of hits distributed in the pattern (range 0-64)
- 'SHIFT' defines the rotation (or delay) of the pattern in steps (range 0-64)
##### OUTPUTS:
- `OUT` outputs 0V or 10V pulses 
##### RIGHT-CLICK MENU:
- `PRECISION` messes up the pattern upon request (range perfect to drunk)
- `LENGTH` outputs 0V or 10V pulses 
- `HIT KNOB MODE` allows selection betwee "solid hits" or "percentages"

*/

#include <fstream>
#include "plugin.hpp"


struct Euclk : Module {
	enum ParamId {
		MONOIN1_PARAM,
		MONOIN3_PARAM,
		MONOIN5_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		RST_INPUT,
		CLK_INPUT,
		MONOIN2_INPUT,
		MONOIN4_INPUT,
		MONOIN6_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		POLYOUT_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		LIGHTS_LEN
	};

	Euclk() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(MONOIN1_PARAM, 1.f, 64.f, 32.f, "Length");
		configParam(MONOIN3_PARAM, 0.f, 64.f, 12.f, "Hits");
		configParam(MONOIN5_PARAM, 0.f, 64.f, 0.f, "Shift");
		paramQuantities[MONOIN1_PARAM]->snapEnabled = true;
		paramQuantities[MONOIN3_PARAM]->snapEnabled = true;
		paramQuantities[MONOIN5_PARAM]->snapEnabled = true;
		configInput(RST_INPUT, "");
		configInput(CLK_INPUT, "");
		configInput(MONOIN2_INPUT, "");
		configInput(MONOIN4_INPUT, "");
		configInput(MONOIN6_INPUT, "");
		configOutput(POLYOUT_OUTPUT, "");
	}

int indexPrec=0;	// context menu settings
int indexMaxLen=2;	// context menu settings
int indexHitMode=0;	// context menu settings

bool oldRst=false;
bool newRst=false;
bool oldClk=false;
bool newClk=false;

int loopKnobs=1;	// save some CPU to check the knobs only occaisonly
int currStep=0;
int tStp=16;	// the length of the pattern
int tHit=3;		// the number of hits in a pattern
int tShf=0;		// the amount of shift to the pattern
int tPattern[64]={0}; // 64 is the maximum selectable length of a pattern

// function euclidTxt(steps,  pulses, rotation){
	// https://www.computermusicdesign.com/simplest-euclidean-rhythm-algorithm-explained/
    // storedRhythm = []; 
    // var bucket = 0; //out variable to add pulses together for each step
    // for( var i=0 ; i < steps ; i++){ 
        // bucket += pulses; 
            // if(bucket >= steps) {
            // bucket -= steps; storedRhythm.push(1); 
        // } else {storedRhythm.push(0);}
    // }
	// return storedRhythm.reverse().join('')
// }

// function rotateSeq(seq, rotate){
    // var output = new Array(seq.length); //new array to store shifted rhythm
    // var val = seq.length - rotate;
    // for( var i=0; i < seq.length ; i++){
        // output[i] = seq[ Math.abs( (i+val) % seq.length) ];
    // }
    // return output;
// }


	void process(const ProcessArgs& args) override {
		
		// if (loopKnobs-- ==0) {
			// loopKnobs=500;

			// 0-10V controlls the knobs
			if (inputs[MONOIN2_INPUT].isConnected()) {params[MONOIN1_PARAM].setValue(abs(inputs[MONOIN2_INPUT].getVoltage()/10*32));}
			if (inputs[MONOIN4_INPUT].isConnected()) {params[MONOIN3_PARAM].setValue(abs(inputs[MONOIN4_INPUT].getVoltage()/10*32));}
			if (inputs[MONOIN6_INPUT].isConnected()) {params[MONOIN5_PARAM].setValue(abs(inputs[MONOIN6_INPUT].getVoltage()/10*32));}

			// the knobs define the parameters
			tStp=round(params[MONOIN1_PARAM].getValue());
			tHit=round(params[MONOIN3_PARAM].getValue());
			tShf=round(params[MONOIN5_PARAM].getValue());

			// refresh the pattern if a knob is changed
			// let's go, you know tStp, tHit, tShf
			// https://www.computermusicdesign.com/simplest-euclidean-rhythm-algorithm-explained/

			int bucket=0;
			for( int i=0; i<tStp; i++){ 
				bucket += tHit; 
				if(bucket >= tStp) {
					bucket -= tStp; 
					tPattern[i]=1; 
				} 
				else {tPattern[i]=0;}
			}
			
		// }	// loop
			
		// to catch the reset signal
		newRst=inputs[RST_INPUT].getVoltage()>2;
		if (oldRst!=newRst) {currStep=0;oldRst=newRst;}
		
		// to catch the clock signal
		newClk=inputs[CLK_INPUT].getVoltage()>2;
		if (oldClk!=newClk) {
			if (newClk) {
				if (currStep>=tStp) {currStep=0;} else {currStep++;}
				outputs[POLYOUT_OUTPUT].setVoltage(tPattern[currStep]*10);
			} 
			else {
				outputs[POLYOUT_OUTPUT].setVoltage(0);
				}
			oldClk=newClk;
		}		
		
/*		
		// number of polyinput channels
		polyChans=inputs[POLYIN_INPUT].channels;
		// the output should be the same
		outputs[POLYOUT_OUTPUT].channels=polyChans;

		// is there a reset?
		rstNew=inputs[RST_INPUT].getVoltage()>2;
		if (rstNew==true && (stOld==false) {stepPos=1;}

		// is there a clock?
		clkNew=inputs[CLK_INPUT].getVoltage()>2;
		if ((clkNew==true) && (clkOld==false)) {
			if (stepPos>=tStp) {stepPos=1;} else {stepPos++;}
		}
		// let's remember these values	
		clkOld=clkNew;
		rstOld=rstNew;		
*/

		// send the voltage


	};

	// this block is to save and reload a variable
	json_t* dataToJson() override {
		json_t* rootJ = json_object();
		// json_object_set_new(rootJ, "tPattern", json_array());
		json_t* gatesJ = json_array();
		for (int i = 0; i < 64; i++) {
			json_array_insert_new(gatesJ, i, json_integer((int) tPattern[i]));
		}
		json_object_set_new(rootJ, "tPattern", gatesJ);
		return rootJ;
	}

	void dataFromJson(json_t* rootJ) override {
		json_t* gatesJ = json_object_get(rootJ, "tPattern");
		if (gatesJ) {
			for (int i = 0; i < 64; i++) {
				json_t* gateJ = json_array_get(gatesJ, i);
				if (gateJ)
					tPattern[i] = !!json_integer_value(gateJ);
			}
		}
	}

};


struct EuclkWidget : ModuleWidget {
	EuclkWidget(Euclk* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/Euclk.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(10.16, 66.04)), module, Euclk::MONOIN1_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(10.16, 76.2)), module, Euclk::MONOIN3_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(10.16, 86.36)), module, Euclk::MONOIN5_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(5.08, 25.4)), module, Euclk::RST_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(15.24, 25.4)), module, Euclk::CLK_INPUT));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(10.16, 96.52)), module, Euclk::MONOIN2_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(10.16, 106.68)), module, Euclk::MONOIN4_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(10.16, 116.84)), module, Euclk::MONOIN6_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(10.16, 50.588)), module, Euclk::POLYOUT_OUTPUT));
	}
	
	void appendContextMenu(Menu* menu) override {
		Euclk* module = dynamic_cast<Euclk*>(this->module);
		assert(module);
		menu->addChild(new MenuSeparator);
		menu->addChild(createIndexPtrSubmenuItem("Precision", {"Perfect (100%)","Very good (95%)","Good (85%)","Not bad (75%)","Wasted (60%)","Crap (50%)","Completely insane (35%)"}, &module->indexPrec));
		menu->addChild(createIndexPtrSubmenuItem("Max. pattern length", {"8","16","32","64"}, &module->indexMaxLen));
		menu->addChild(createIndexPtrSubmenuItem("Hit knob mode", {"Solid steps","Percentage"}, &module->indexHitMode));

	}
	
	// void onHoverKey(const event::HoverKey &e) override {
		// if (e.key >= GLFW_KEY_1 && e.key <= GLFW_KEY_4) {
			// if (e.action == GLFW_PRESS) {
				// float key_number = e.key - 49; // 49 is the ascii number for key #1
				// if ((e.mods & RACK_MOD_MASK) == GLFW_MOD_SHIFT) {module->contextRndK(key_number);}
				// else {module->contextRnd(key_number);}
				// e.consume(this);
			// }
		// }
	// ModuleWidget::onHoverKey(e);
	// }
	
};


Model* modelEuclk = createModel<Euclk, EuclkWidget>("Euclk");