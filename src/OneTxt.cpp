// Copyright (c) 2022 András Szabó
#include "plugin.hpp"

#include <fstream>
#include <osdialog.h>

struct OneTxt : Module {

// --------------------------------------------------

	enum ParamId  {
		LOAD_PARAM, RESET_PARAM, PARAMS_LEN};

	enum InputId  {
		CLOCK_INPUT, RESET_INPUT, INPUTS_LEN};

	enum OutputId {
		TRACK_1_OUTPUT, TRACK_2_OUTPUT, TRACK_3_OUTPUT, TRACK_4_OUTPUT, 
		TRACK_5_OUTPUT, TRACK_6_OUTPUT, TRACK_7_OUTPUT, TRACK_8_OUTPUT, 
		OUTPUTS_LEN};

	enum LightId  {LIGHTS_LEN};

	// small assistance to save older values for reference;
	float paramVal[PARAMS_LEN]={0};
	// float inputVolt[INPUTS_LEN]={0};
	// float lightVal[LIGHTS_LEN]={0};

// --------------------------------------------------

	OneTxt() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(LOAD_PARAM, 	0.0f, 1.0f, 0.0f, "Load");
		configParam(RESET_PARAM, 	0.0f, 1.0f, 0.0f, "Reset");
		paramQuantities[LOAD_PARAM]->snapEnabled = true;
		paramQuantities[RESET_PARAM]->snapEnabled = true;
		paramQuantities[LOAD_PARAM]->randomizeEnabled = false;
		paramQuantities[RESET_PARAM]->randomizeEnabled = false;

		configInput(CLOCK_INPUT, "Clock"); 
		configInput(RESET_INPUT, "Reset"); 

		configOutput(TRACK_1_OUTPUT, "Track 1"); 
		configOutput(TRACK_2_OUTPUT, "Track 2"); 
		configOutput(TRACK_3_OUTPUT, "Track 3"); 
		configOutput(TRACK_4_OUTPUT, "Track 4"); 
		configOutput(TRACK_5_OUTPUT, "Track 5"); 
		configOutput(TRACK_6_OUTPUT, "Track 6"); 
		configOutput(TRACK_7_OUTPUT, "Track 7"); 
		configOutput(TRACK_8_OUTPUT, "Track 8"); 
	}

// --------------------------------------------------

	int loop=0;     // save some CPU in process()

	// for managing the RESET and CLOCK signals
	float newReset=0.0f;
	float oldReset=0.0f;
	float newClock=0.0f;
	float oldClock=0.0f;
	bool hitClock=false;
	bool hitReset=false;
	int CURR_STEP[8]={0};

	// -------------------------------------

	// open a dialog and return a filename
	std::string selectionDir = asset::user("selections"); //my preferred directory
	std::string selectionFile = ""; 					//my preferred file
	std::string selectFileVCV(osdialog_file_action OSDIALOG_MODE) {
		osdialog_filters *filters = osdialog_filters_parse("TXT:txt");
		char *filename = osdialog_file(OSDIALOG_MODE, 
			selectionDir.c_str(), 
			selectionFile.c_str(), 
			filters);
		// char *filename = osdialog_file(OSDIALOG_MODE, selectionDir.c_str(), NULL, filters);
		// char *filename = osdialog_file(OSDIALOG_SAVE, selectionDir.c_str(), NULL, filters);
		// char *filename = osdialog_file(OSDIALOG_OPEN, "", NULL, filters);
		if (filename != NULL) {
			selectionFile.assign(filename);
			osdialog_filters_free(filters);
			std::free(filename);
		}
		selectionDir=selectionFile.substr(0, selectionFile.find_last_of("/\\"));
		return (selectionFile);
	}

// float theSeq[8][256];	// this contains the sequence, 256 slots available
float theSeq[1];	// this contains the sequence, 256 slots available
float theTune[8][1];	// this contains the sequence, 256 slots available

	// VoxGlitch One Point (1) & Zero (0) text file export
	void importFieldsFromTxt(int method) {
		std::ifstream input_file(selectFileVCV(OSDIALOG_OPEN));
		// test if file open
		if (input_file)	{
			int row=0;
			std::string line = "";
			// VoxGlitch One Zero file to be collected
			if (method==0) {
				/*
				char ch; int step=0;
				while (ch!=EOF) {
					input_file.get(ch);
					if (ch=='1') {theTune[row][step]=10;}
					else if (ch=='0') {theTune[row][step]=0;}
					step++;
					// else if (ch==','){i++;}
					// else if (ch=='\n') {i++;}
					// else skipping spaces, A-Z letters, whatever
				}
				row++;
				*/
			}
			// VoxGlitch One Point file to be collected
			else if (method==1) {
				std::string line = ""; int step=0; int track=0;
				while (std::getline(input_file, line) && track<8) {
					// std::vector<float> sequence;
					std::stringstream ss(line);
					while( ss.good()) {
						std::string substr;
						getline(ss, substr,',');
						theTune[track][step]=std::stod(substr);
						step++; 
					}
					track++;
				}
			}
			input_file.close();
		}
	}

	// -------------------------------------

	void process(const ProcessArgs& args) override {

		if (loop--<=0) {
			loop=9002;
			// save some more CPU
			for (int p=0;p<PARAMS_LEN;p++) {paramVal[p]=params[p].getValue();}
			if (paramVal[LOAD_PARAM]==1) {importFieldsFromTxt(0);}
			if (paramVal[RESET_PARAM]==1) {hitReset=true;}
		}

		// let's see the reset signal
		newReset=inputs[RESET_INPUT].getVoltage();
		if (newReset>2.0f && oldReset<=2.0f) {hitReset=true;}
		// else if (newReset>2.0f && oldReset>2.0f) {}
		// else if (newReset<=2.0f && oldReset>2.0f) {}
		// else if (newReset<=2.0f && oldReset<=2.0f) {}
		oldReset=newReset;

		// let's see the clock signal
		hitClock=false;
		newClock=inputs[CLOCK_INPUT].getVoltage();
		if (newClock>2.0f && oldClock<=2.0f) {hitClock=true;}
		// else if (newClock>2.0f && oldClock>2.0f) {}
		// else if (newClock<=2.0f && oldClock<=2.0f) {}
		// else if (newClock<=2.0f && oldClock>2.0f) {}
		oldClock=newClock;

		if (hitClock) {
			hitClock=false;
			if (hitReset) {
				hitReset=false; 
				for (int i=0;i<8;i++) {CURR_STEP[i]=0;}
				};
			outputs[TRACK_1_OUTPUT].setVoltage(theTune[0][CURR_STEP[0]]);
			outputs[TRACK_2_OUTPUT].setVoltage(theTune[1][CURR_STEP[1]]);
			outputs[TRACK_3_OUTPUT].setVoltage(theTune[2][CURR_STEP[2]]);
			outputs[TRACK_4_OUTPUT].setVoltage(theTune[3][CURR_STEP[3]]);
			outputs[TRACK_5_OUTPUT].setVoltage(theTune[4][CURR_STEP[4]]);
			outputs[TRACK_6_OUTPUT].setVoltage(theTune[5][CURR_STEP[5]]);
			outputs[TRACK_7_OUTPUT].setVoltage(theTune[6][CURR_STEP[6]]);
			outputs[TRACK_8_OUTPUT].setVoltage(theTune[7][CURR_STEP[7]]);
			for (int i=0;i<8;i++) {CURR_STEP[i]++;}
		
		}

	}

	// it's OK
    json_t *dataToJson() override {
        json_t *json_root = json_object();
		json_object_set_new(json_root, "selectionDir", json_string(selectionDir.c_str()));
		json_object_set_new(json_root, "selectionFile", json_string(selectionFile.c_str()));
/*		json_t *notes_json_array = json_array();
		for (int notes_row=0; notes_row<8; notes_row++) {
			json_t *track_json_array = json_array();
			for (int notes_col=0; notes_col<2048; notes_col++) {
				json_array_append_new(track_json_array, json_integer(theTune[notes_col][notes_row]));
			}	
			json_array_append_new(notes_json_array,track_json_array);
		}	
		json_object_set(json_root, "notes", notes_json_array);
*/      return json_root;}


	// it's OK
    void dataFromJson(json_t *json_root) override {
		json_t *json_dir = json_object_get(json_root, "selectionDir");
		if (json_dir) selectionDir = json_string_value(json_dir);
		json_t *json_file = json_object_get(json_root, "selectionFile");
		if (json_file) selectionFile = json_string_value(json_file);
		json_t *notes_json_array = json_object_get(json_root, "notes");
		if(notes_json_array) {
			json_t *track_json_array;
			size_t track_number;	// aka 'row'
			json_array_foreach(notes_json_array, track_number, track_json_array)
			{
				for(int i=0; i<2048; i++) {
				double value = json_integer_value(json_array_get(track_json_array, i));
				this->theTune[i][track_number]=value;
				}
			}
		}
    }

};

// --------------------------------------------------

struct OneTxtWidget : ModuleWidget {

	OneTxt* module;
	#include "share/widgetwizard.hpp"   // addChild abbreviations

	OneTxtWidget(OneTxt* module) {

		this->module = module;
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/OneTxt.svg")));

		// addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		
		// childButtonmom(OneTxt::LOAD_PARAM, 0, HP*1, HP*7.5);
		childButtonmom(OneTxt::RESET_PARAM, 0, HP*1, HP*2.5);
		childInput(OneTxt::RESET_INPUT, HP*1, HP*4);
		childInput(OneTxt::CLOCK_INPUT, HP*1, HP*7);
		childOutput(OneTxt::TRACK_1_OUTPUT, HP*1, HP*9);
		childOutput(OneTxt::TRACK_2_OUTPUT, HP*1, HP*11);
		childOutput(OneTxt::TRACK_3_OUTPUT, HP*1, HP*13);
		childOutput(OneTxt::TRACK_4_OUTPUT, HP*1, HP*15);
		childOutput(OneTxt::TRACK_5_OUTPUT, HP*1, HP*17);
		childOutput(OneTxt::TRACK_6_OUTPUT, HP*1, HP*19);
		childOutput(OneTxt::TRACK_7_OUTPUT, HP*1, HP*21);
		childOutput(OneTxt::TRACK_8_OUTPUT, HP*1, HP*23);
		
		childLabel(HP*1,HP*1, "DEV", 12);
	}

	void appendContextMenu(Menu* menu) override {
		OneTxt* module = dynamic_cast<OneTxt*>(this->module);
		assert(module);
		menu->addChild(new MenuSeparator);
		menu->addChild(createMenuLabel("WARNING! Buggy & incomplete module! Subject to deletion!")); 
		menu->addChild(createSubmenuItem("Text export/import", "",
			[=](Menu* menu) {
				menu->addChild(createMenuLabel("Currently loaded: " 
					// + module->selectionFile.substr(module->selectionFile.find_last_of("/\\"))
					// + "N/A"
					));
				menu->addChild(new MenuSeparator);			
				menu->addChild(createMenuLabel("(OneZero format: 10101101)"));
				// menu->addChild(createMenuItem("Export to OneZero text file", "", [=]() {module->exportFieldsToTxt(0);}));	
				menu->addChild(createMenuItem("Import from OneZero text file", "", [=]() {module->importFieldsFromTxt(0);}));
				menu->addChild(createMenuLabel("(OnePoint format: 1.16, 2.25, 4.22)"));
				// menu->addChild(createMenuItem("Export to OnePoint text file", "", [=]() {module->exportFieldsToTxt(1);}));	
				menu->addChild(createMenuItem("Import from OnePoint text file", "", [=]() {module->importFieldsFromTxt(1);}));
			}
		));
	}

	// #include "OneTxt/OneTxt_keys.hpp"

};

Model* modelOneTxt = createModel<OneTxt, OneTxtWidget>("OneTxt");
