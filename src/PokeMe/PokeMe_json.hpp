/* 
This file is needed by PokeME. 
I wanted to avoid typing the same thing twice, so here it is.
*/

	// this block is to save and reload a variable
	json_t* dataToJson() override {
		json_t* rootJ = json_object();
		// json_object_set_new(rootJ, "precmode", json_integer(indexPrecMode));
		// json_object_set_new(rootJ, "mode", json_integer(indexPrec));
		json_object_set_new(rootJ, "choke", json_boolean(indexChoke));
		json_object_set_new(rootJ, "loop", json_boolean(indexLoop));
		json_object_set_new(rootJ, "extendedout", json_boolean(indexExt));
		json_object_set_new(rootJ, "pw", json_integer(indexPW));
		json_t *notes_json_array = json_array();
		for (int notes_index=0; notes_index<SEQLEN; notes_index++) {
			json_array_append_new(notes_json_array, json_integer(grid_data[notes_index]));
		}	
		json_object_set_new(rootJ, "notes", notes_json_array);			
		return rootJ;
	}

	void dataFromJson(json_t* rootJ) override {
		// json_t *precmodeJ = json_object_get(rootJ, "precmode");
		// if (precmodeJ) indexPrecMode = json_integer_value(precmodeJ);
		// json_t *modeJ = json_object_get(rootJ, "mode");
		// if (modeJ) indexPrec = json_integer_value(modeJ);
		json_t *chokeJ = json_object_get(rootJ, "choke");
		if (chokeJ) indexChoke = json_boolean_value(chokeJ);
		json_t *loopJ = json_object_get(rootJ, "loop");
		if (loopJ) indexLoop = json_boolean_value(loopJ);
		json_t *extJ = json_object_get(rootJ, "extendedout");
		if (extJ) indexExt = json_boolean_value(extJ);
		json_t *pwJ = json_object_get(rootJ, "pw");
		if (pwJ) indexPW = json_integer_value(pwJ);	
		json_t *notes_json_array = json_object_get(rootJ, "notes");
		size_t note_index;
		json_t *json_value;
		if (notes_json_array) {
			json_array_foreach(notes_json_array, note_index, json_value) {
				grid_data[note_index] = json_integer_value(json_value);
				// grid_data[note_index] = rand() % 6;
			}
		}
	}
