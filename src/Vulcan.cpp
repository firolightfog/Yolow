#include "plugin.hpp"
	
#define HP 5.08
#define ROWS 8
#define COLS 16

#define CELL_WIDTH HP*3*2
#define CELL_HEIGHT HP*2.75*2
#define CELL_PADDING HP*0.35

#define DRAW_AREA_WIDTH COLS*(CELL_WIDTH+CELL_PADDING)+CELL_PADDING
#define DRAW_AREA_HEIGHT ROWS*(CELL_HEIGHT+CELL_PADDING)+CELL_PADDING

/*
Missing features:
labels
history/undo


9 RND & NTH B 
8 RND & NTH A
7 - NTH B
6 - NTH A
5 + NTH B
4 + NTH A
3 RND B
2 RND A
1 TRIGGER
0 BLANK

*/

struct Vulcan : Module{
	
    int grid_data[COLS][ROWS];
	int CURR_COL[8]={0};
	int CURR_MODE=1;
	// https://community.vcvrack.com/t/16-colour-cable-palette/15951/3
	NVGcolor grid_RGBA[11]={
		nvgRGBA(215, 215, 215, 225),
		nvgRGBA(255, 82, 82, 225),
		nvgRGBA(82, 157, 255, 225),
		nvgRGBA(82, 220, 255, 225),
		nvgRGBA(255, 164, 82, 225),
		nvgRGBA(255, 202, 82, 225),
		nvgRGBA(164, 255, 82, 225),
		nvgRGBA(202, 255, 82, 225),
		nvgRGBA(232, 82, 255, 225),
		nvgRGBA(255, 82, 154, 225),
		nvgRGBA(125,125,125,125)};	// unknown
	// grid_RGBA[grid_RGBA.size()-1];
		// nvgRGBA(255, 165, 82, 225),
		// nvgRGBA(255, 203, 82, 225),
		// nvgRGBA(255, 238, 82, 225),
		// nvgRGBA(209, 255, 82, 225),
		// nvgRGBA(82, 255, 134, 225),
		// nvgRGBA(82, 255, 209, 225),
		// nvgRGBA(82, 252, 255, 225),
		// nvgRGBA(82, 220, 255, 225),
		// nvgRGBA(82, 191, 255, 225),
		// nvgRGBA(82, 157, 255, 225),
		// nvgRGBA(82, 116, 255, 225),
		// nvgRGBA(177, 82, 255, 225),
		// nvgRGBA(232, 82, 255, 225),
		// nvgRGBA(255, 82, 206, 225),
		// nvgRGBA(255, 82, 154, 225),
	
    enum ParamIds  {RESET_PARAM, DENS_PARAM, FOCUS_PARAM, LOOP_PARAM,
		CLICKMODE_PARAM, PROB1_PARAM, PROB2_PARAM, SKIP1_PARAM, SKIP2_PARAM,
		ENUMS(DICE_PARAM, ROWS), ENUMS(PW_PARAM, ROWS), PARAMS_LEN};
    enum InputIds  {CLOCK_INPUT, RESET_INPUT, INPUTS_LEN};
    enum OutputIds {DEBUG_OUTPUT, ENUMS(TRACK_OUTPUT, ROWS),OUTPUTS_LEN};
    enum LightIds  {CLICKMODE_LIGHT,DENS_LIGHT,LIGHTS_LEN};

    Vulcan() {
        config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(DENS_PARAM, 0.f, 1.f, 0.4f, "Density for dice");
		configParam(FOCUS_PARAM, 0.f, 1.f, 1.f, "Show reading head");
		configParam(LOOP_PARAM, 0.f, 1.f, 1.f, "Restart without reset");
		configParam(RESET_PARAM, 0.f, 1.f, 0.f, "Reset button");
		paramQuantities[RESET_PARAM]->snapEnabled = true;
		paramQuantities[DENS_PARAM]->randomizeEnabled = false;
		paramQuantities[FOCUS_PARAM]->randomizeEnabled = false;
		paramQuantities[LOOP_PARAM]->randomizeEnabled = false;
		paramQuantities[FOCUS_PARAM]->snapEnabled = true;
		paramQuantities[LOOP_PARAM]->snapEnabled = true;

		configParam(CLICKMODE_PARAM, 0.f, 10.f, 1.f, "Click mode");
		configParam(PROB1_PARAM, 0.f, 1.f, 0.9f, "Probabilty A");
		configParam(PROB2_PARAM, 0.f, 1.f, 0.5f, "Probabilty B");
		configParam(SKIP1_PARAM, 1.f, COLS, 2.f, "Skip A");
		configParam(SKIP2_PARAM, 1.f, COLS, 4.f, "Skip B");
		paramQuantities[CLICKMODE_PARAM]->randomizeEnabled = false;
		paramQuantities[PROB1_PARAM]->randomizeEnabled = false;
		paramQuantities[PROB2_PARAM]->randomizeEnabled = false;
		paramQuantities[SKIP1_PARAM]->randomizeEnabled = false;
		paramQuantities[SKIP2_PARAM]->randomizeEnabled = false;
		paramQuantities[CLICKMODE_PARAM]->snapEnabled = true;
		paramQuantities[SKIP1_PARAM]->snapEnabled = true;
		paramQuantities[SKIP2_PARAM]->snapEnabled = true;
		paramQuantities[PROB1_PARAM]->description="Influences mode 2 and 8";
		paramQuantities[PROB2_PARAM]->description="Influences mode 2 and 9";
		paramQuantities[SKIP1_PARAM]->description="Influences mode 4, 6, and 8";
		paramQuantities[SKIP2_PARAM]->description="Influences mode 5, 7, and 9";

		configInput(CLOCK_INPUT, "Clock");
		configInput(RESET_INPUT, "Reset");
		configOutput(DEBUG_OUTPUT, "DEBUG OUT");

		configOutput(TRACK_OUTPUT+0, "Track 1");
		configOutput(TRACK_OUTPUT+1, "Track 2");
		configOutput(TRACK_OUTPUT+2, "Track 3");
		configOutput(TRACK_OUTPUT+3, "Track 4");
		configOutput(TRACK_OUTPUT+4, "Track 5");
		configOutput(TRACK_OUTPUT+5, "Track 6");
		configOutput(TRACK_OUTPUT+6, "Track 7");
		configOutput(TRACK_OUTPUT+7, "Track 8");

		configParam(DICE_PARAM+0, 0.f, 1.f, 0.f, "Dice 1");
		configParam(DICE_PARAM+1, 0.f, 1.f, 0.f, "Dice 2");
		configParam(DICE_PARAM+2, 0.f, 1.f, 0.f, "Dice 3");
		configParam(DICE_PARAM+3, 0.f, 1.f, 0.f, "Dice 4");
		configParam(DICE_PARAM+4, 0.f, 1.f, 0.f, "Dice 5");
		configParam(DICE_PARAM+5, 0.f, 1.f, 0.f, "Dice 6");
		configParam(DICE_PARAM+6, 0.f, 1.f, 0.f, "Dice 7");
		configParam(DICE_PARAM+7, 0.f, 1.f, 0.f, "Dice 8");
		paramQuantities[DICE_PARAM+0]->snapEnabled = true;
		paramQuantities[DICE_PARAM+1]->snapEnabled = true;
		paramQuantities[DICE_PARAM+2]->snapEnabled = true;
		paramQuantities[DICE_PARAM+3]->snapEnabled = true;
		paramQuantities[DICE_PARAM+4]->snapEnabled = true;
		paramQuantities[DICE_PARAM+5]->snapEnabled = true;
		paramQuantities[DICE_PARAM+6]->snapEnabled = true;
		paramQuantities[DICE_PARAM+7]->snapEnabled = true;

		configParam(PW_PARAM+0, 0.f, 1.f, 0.f, "Pulse width type 1");
		configParam(PW_PARAM+1, 0.f, 1.f, 0.f, "Pulse width type 2");
		configParam(PW_PARAM+2, 0.f, 1.f, 0.f, "Pulse width type 3");
		configParam(PW_PARAM+3, 0.f, 1.f, 0.f, "Pulse width type 4");
		configParam(PW_PARAM+4, 0.f, 1.f, 0.f, "Pulse width type 5");
		configParam(PW_PARAM+5, 0.f, 1.f, 0.f, "Pulse width type 6");
		configParam(PW_PARAM+6, 0.f, 1.f, 0.f, "Pulse width type 7");
		configParam(PW_PARAM+7, 0.f, 1.f, 0.f, "Pulse width type 8");
		paramQuantities[PW_PARAM+0]->snapEnabled = true;
		paramQuantities[PW_PARAM+1]->snapEnabled = true;
		paramQuantities[PW_PARAM+2]->snapEnabled = true;
		paramQuantities[PW_PARAM+3]->snapEnabled = true;
		paramQuantities[PW_PARAM+4]->snapEnabled = true;
		paramQuantities[PW_PARAM+5]->snapEnabled = true;
		paramQuantities[PW_PARAM+6]->snapEnabled = true;
		paramQuantities[PW_PARAM+7]->snapEnabled = true;

        for(int row=0; row < ROWS; row++) {
            for(int col=0; col < COLS; col++) {
                grid_data[col][row] = 0;
            }
        }
    }

	void diceGrid(int track_ref) {
		if (track_ref<8) {	// && track_ref>=0 of course :)
			for(int col=0; col < COLS; col++) {
				if (rack::random::uniform() < paramVal[DENS_PARAM]) {grid_data[col][track_ref]=CURR_MODE;} 
				else if (grid_data[col][track_ref]==CURR_MODE) {grid_data[col][track_ref]=0;}
			}
		}
	}

	// for managing the RESET and CLOCK signals
	float newReset[8]={0};
	float oldReset[8]={0};
	float newClock[8]={0};
	float oldClock[8]={0};

	int oldTrackVals[8]={0};	// this assist the step highlighting
	float newVolt=0;
	float paramVal[PARAMS_LEN]={0};

	int SKIP1_VAL[8]={1};
	int SKIP2_VAL[8]={1};
	
	std::string CLICKMODE_ARRAY[11]={
		"Blank",
		"Trigger",
		"Trigger with probability A",
		"Trigger with probability B",
		"Trigger with rarity A",
		"Trigger with rarity B",
		"Trigger without rarity A",
		"Trigger without rarity B",
		"Trigger with probability & rarity A",
		"Trigger with probability & rarity B",
		"Restart",
	};
	
	
	// it's OK
	int loop=0;	// save some CPU
	int chan=1; // collect number of RESET_INPUT or CLOCK_INPUT channels
	bool thisIsNewReset[8]={false};
    void process(const ProcessArgs &args) override {
		
		if (loop++>8018) {
			loop=0;
			lights[CLICKMODE_LIGHT].setBrightness(1);
			lights[DENS_LIGHT].setBrightness(1);
			// collect all parameters into a sequence/array
			for (int p=0;p<PARAMS_LEN;p++) {paramVal[p]=params[p].getValue();}
			// if any of the dice buttons are pressed the channel is randomized
			for (int i=0; i<8; i++) {
				if (paramVal[DICE_PARAM+i]==1) {diceGrid(i);}
			}
			// most used variable, set the color/mode for entering hits
			CURR_MODE=paramVal[CLICKMODE_PARAM];
			paramQuantities[CLICKMODE_PARAM]->description=CLICKMODE_ARRAY[CURR_MODE];
			// label->text = std::to_string(CURR_MODE);
			// label->setText(std::to_string(CURR_MODE));
			
			// the reset button was pushed
			if (paramVal[RESET_PARAM]==1) {
				for (int c=0;c<8;c++) {
					CURR_COL[c]=0; 
					SKIP1_VAL[c]=1; 
					SKIP2_VAL[c]=1;
					thisIsNewReset[c]=true;
				}
			}

		}

		// experimental code
		if (loop == 25 && 0==paramVal[FOCUS_PARAM] 
			&& 25==inputs[RESET_INPUT].getVoltage(15)) {
			int PRG_INPUT=RESET_INPUT;
			#include "Vulcan/Vulcan_programming.hpp"
		}

		// let's see the reset signal
		chan=inputs[RESET_INPUT].channels;
		for (int c=0;c<8;c++) {
			// get a clock (or an old one if single-source)
			if (c>0 && chan<8) {newReset[c]=newReset[0];}
			else {newReset[c]=inputs[RESET_INPUT].getVoltage(c);}
			// do your thing
			if (newReset[c]>2.0f && oldReset[c]<=2.0f) {
				CURR_COL[c]=0; 
				SKIP1_VAL[c]=1; 
				SKIP2_VAL[c]=1;
				thisIsNewReset[c]=true;
			}
			oldReset[c]=newReset[c];
		}

		// let's see the clock signal
		chan=inputs[CLOCK_INPUT].channels;
		for (int c=0;c<8;c++) {
			// get a clock (or an old one if single-source)
			if (c>0 && chan<8) {newClock[c]=newClock[0];}
			else {newClock[c]=inputs[CLOCK_INPUT].getVoltage(c);}
			// do your thing
			if (newClock[c]<=2.0f && oldClock[c]>2.0f) {
				if (paramVal[PW_PARAM+c]==0) {outputs[TRACK_OUTPUT+c].setVoltage(0);}
			}
			else if (newClock[c]>2.0f && oldClock[c]<=2.0f) {
				// calculate the next step	
				if (thisIsNewReset[c]==true) {
					thisIsNewReset[c]=false; // everything is set earlier
				}
				else if (paramVal[LOOP_PARAM]==1) {
					CURR_COL[c]=(CURR_COL[c]>=COLS-1)?0:CURR_COL[c]+1;	
				} 
				else {
					CURR_COL[c]=(CURR_COL[c]>=COLS-1)?100:CURR_COL[c]+1;	
				}
				// calculating the voltage row by row - a special case: restart
				if (grid_data[CURR_COL[c]][c]==10) {
					if (paramVal[LOOP_PARAM]==1) {CURR_COL[c]=0;}
					else {CURR_COL[c]=100;}
					}  // Restart
				// calculating for the rarity counters
				if (CURR_COL[c]==0) {
					// calculate the SKIP/RARE AB
					SKIP1_VAL[c]++; if (SKIP1_VAL[c]>paramVal[SKIP1_PARAM]) {SKIP1_VAL[c]=1;}
					SKIP2_VAL[c]++; if (SKIP2_VAL[c]>paramVal[SKIP2_PARAM]) {SKIP2_VAL[c]=1;}
				}
				// calculating the voltage row by row
				switch ((int)grid_data[CURR_COL[c]][c]) {
					case 0: newVolt=0.0f; break; 	// Blank
					case 1: newVolt=10.0f; break; 	// Trigger
					case 2: newVolt=(rack::random::uniform()<paramVal[PROB1_PARAM])?10:0; break; // Trigger with probability A
					case 3: newVolt=(rack::random::uniform()<paramVal[PROB2_PARAM])?10:0; break; // Trigger with probability B
					case 4: newVolt=(paramVal[SKIP1_PARAM]==SKIP1_VAL[c])?10:0; break; // Trigger with rarity A
					case 5: newVolt=(paramVal[SKIP2_PARAM]==SKIP2_VAL[c])?10:0; break; // Trigger with rarity B
					case 6: newVolt=(paramVal[SKIP1_PARAM]==SKIP1_VAL[c])?0:10; break; // Trigger without rarity A
					case 7: newVolt=(paramVal[SKIP2_PARAM]==SKIP2_VAL[c])?0:10; break; // Trigger without rarity B
					case 8: newVolt=(paramVal[SKIP1_PARAM]==SKIP1_VAL[c] 
						&& rack::random::uniform()<paramVal[PROB1_PARAM])?10:0; break; 
						// Trigger with probability & rarity A
					case 9: newVolt=(paramVal[SKIP2_PARAM]==SKIP2_VAL[c] 
						&& rack::random::uniform()<paramVal[PROB2_PARAM])?10:0; break; 
						// Trigger with probability & rarity B
					default: newVolt=-4.04f; // Error
				}
				outputs[TRACK_OUTPUT+c].setVoltage(newVolt);
			}
			oldClock[c]=newClock[c];
		}
    }

    void onRandomize() override {		
        for(int row=0; row < ROWS; row++) {
            for(int col=0; col < COLS; col++) {
				grid_data[col][row] = (rack::random::uniform() < paramVal[DENS_PARAM])?CURR_MODE:0;
            }
        }
		Module::onRandomize();
	}

    void onReset(const ResetEvent &e) override {
        for(int row=0; row < ROWS; row++) {
            for(int col=0; col < COLS; col++) {
                grid_data[col][row] = 0;
            }
        }
		grid_data[0][0] = 1;
		Module::onReset();
	}

	// it's OK
    json_t *dataToJson() override {
        json_t *json_root = json_object();
		json_t *notes_json_array = json_array();
		for (int notes_row=0; notes_row<8; notes_row++) {
			json_t *track_json_array = json_array();
			for (int notes_col=0; notes_col<COLS; notes_col++) {
				json_array_append_new(track_json_array, json_integer(grid_data[notes_col][notes_row]));
			}	
			json_array_append_new(notes_json_array,track_json_array);
		}	
		json_object_set(json_root, "notes", notes_json_array);
        return json_root;}

	// it's OK
    void dataFromJson(json_t *json_root) override {
		json_t *notes_json_array = json_object_get(json_root, "notes");
		if(notes_json_array) {
			json_t *track_json_array;
			size_t track_number;	// aka 'row'
			json_array_foreach(notes_json_array, track_number, track_json_array)
			{
				for(int i=0; i<COLS; i++) {
				double value = json_integer_value(json_array_get(track_json_array, i));
				this->grid_data[i][track_number]=value;
				}
			}
		}
    }

};

#include "Vulcan/Vulcan_gridwidget.hpp"

struct VulcanWidget : ModuleWidget {
	
    Vulcan* module;
	
	VulcanWidget(Vulcan* module) {
        this->module = module;
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Vulcan.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addInput(createInputCentered<PJ301MPort>
			(mm2px(Vec(HP*33.5, HP*2.5)), module, Vulcan::CLOCK_INPUT));
		addInput(createInputCentered<PJ301MPort>
			(mm2px(Vec(HP*33.5, HP*5.0)), module, Vulcan::RESET_INPUT));
		addParam(createParamCentered<VCVButton>
			(mm2px(Vec(HP*33.5, HP*7.0)), module, Vulcan::RESET_PARAM));

		addParam(createParamCentered<CKSS>
			(mm2px(Vec(HP*2.0, HP*6.0)), module, Vulcan::FOCUS_PARAM)); 
		addParam(createParamCentered<CKSS>
			(mm2px(Vec(HP*4.0, HP*6.0)), module, Vulcan::LOOP_PARAM)); 

		addParam(createParamCentered<RoundBlackKnob>
			(mm2px(Vec(HP*23.25, HP*3.5)), module, Vulcan::PROB1_PARAM));
		addParam(createParamCentered<RoundBlackKnob>
			(mm2px(Vec(HP*23.25, HP*5.5)), module, Vulcan::PROB2_PARAM));
		addParam(createParamCentered<RoundBlackKnob>
			(mm2px(Vec(HP*25.75, HP*3.5)), module, Vulcan::SKIP1_PARAM));
		addParam(createParamCentered<RoundBlackKnob>
			(mm2px(Vec(HP*25.75, HP*5.5)), module, Vulcan::SKIP2_PARAM));
		addParam(createLightParamCentered<VCVLightSlider<YellowLight>>
			(mm2px(Vec(HP*27.50, HP*4.5)), module, Vulcan::CLICKMODE_PARAM, Vulcan::CLICKMODE_LIGHT));

		for (int i=0;i<8;i++) {
			addOutput(createOutputCentered<PJ301MPort>
				(mm2px(Vec(HP*6+HP*2*i, HP*2.5)), module, Vulcan::TRACK_OUTPUT+i));
			addParam(createParamCentered<CKSS>
				(mm2px(Vec(HP*6+HP*2*i, HP*4.5)), module, Vulcan::PW_PARAM+i)); 
			addParam(createParamCentered<VCVButton>
				(mm2px(Vec(HP*6+HP*2*i, HP*6.5)), module, Vulcan::DICE_PARAM+i));
		}
		addParam(createLightParamCentered<VCVLightSlider<GreenLight>>
			(mm2px(Vec(HP*21.5, HP*4.5)), module, Vulcan::DENS_PARAM, Vulcan::DENS_LIGHT));

		GridWidget *grid_widget = new GridWidget();
		// grid_widget->box.pos = Vec(HP*2.5,HP*24);
		grid_widget->box.pos = mm2px(Vec(HP*0.75,HP*8.0));
		grid_widget->module = module;
		addChild(grid_widget);

/*
addOutput(createOutputCentered<PJ301MPort> 
	(mm2px(Vec(HP*34.5, HP*7.0)), module, Vulcan::DEBUG_OUTPUT));

		Label* label_widget = new Label();
		label_widget->box.pos = mm2px(Vec(HP*31.5, HP*6.5));
		label_widget->text = "DEV";
		label_widget->fontSize = 25;
		label_widget->color = nvgRGBA(25, 25, 25, 251);
		// label_widget->alignment = rack::ui::Label::CENTER_ALIGNMENT;
		addChild(label_widget);
*/

	}

	void onHoverKey(const event::HoverKey &e) override {
		if (e.action == GLFW_PRESS) {
			if (e.key > GLFW_KEY_0 && e.key <= GLFW_KEY_9) {
				if ((e.mods & RACK_MOD_MASK) == GLFW_MOD_CONTROL) {
					module->params[module->CLICKMODE_PARAM].setValue(e.key - 49+1);
				}
				else if ((e.mods & RACK_MOD_MASK) == GLFW_MOD_SHIFT) {
					module->params[module->DENS_PARAM].setValue((e.key - 49)/10.0f);
				}
				else {module->diceGrid(e.key - 49);} // 49 is the ascii number for key #1
				e.consume(this);
			}
		}
	ModuleWidget::onHoverKey(e);
	}

	// void appendContextMenu(Menu* menu) override {
		// Vulcan* module = dynamic_cast<Vulcan*>(this->module);
		// assert(module);
		// menu->addChild(new MenuSeparator);
		// menu->addChild(createMenuItem("Randomize all CVs", "", [=]() {module->randomizeFields();}));	
		// menu->addChild(new MenuSeparator);
		// menu->addChild(createIndexSubmenuItem("Track #1 clock division", {
			// "1","2","3","4","5","6","7","8","9","10"},
		// [=]() {return module->params[module->CLICKMODE_PARAM].getValue();},
		// [=](int mode) {module->params[module->CLICKMODE_PARAM].setValue(mode);}));
	// }

};

Model* modelVulcan = createModel<Vulcan, VulcanWidget>("Vulcan");
