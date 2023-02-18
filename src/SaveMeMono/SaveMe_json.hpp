	// this block is to save and reload a variable
	json_t* dataToJson() override {
	json_t* rootJ = json_object();
	json_object_set_new(rootJ, "pw", json_integer(indexPW));
	json_object_set_new(rootJ, "lfo", json_integer(indexLFO));
	json_object_set_new(rootJ, "rec", json_integer(indexRec));
	// json_object_set_new(rootJ, "notes", json_float(theSeq));
	json_t* notes_json_array = json_array();
	for (int notes_index=0; notes_index < 256; notes_index++) {
		json_array_append_new(notes_json_array , json_real(theSeq[notes_index]));
	}	
	json_object_set_new(rootJ, "notes", notes_json_array);
	return rootJ;}

	void dataFromJson(json_t* rootJ) override {
	json_t *pwJ = json_object_get(rootJ, "pw");
	if (pwJ) indexPW = json_integer_value(pwJ);
	json_t *lfoJ = json_object_get(rootJ, "lfo");
	if (lfoJ) indexLFO = json_integer_value(lfoJ);
	json_t *recJ = json_object_get(rootJ, "rec");
	if (recJ) indexRec = json_integer_value(recJ);
	// json_t *seqJ = json_object_get(rootJ, "notes");
	// if (seqJ) theSeq = json_float_value(seqJ);
	json_t *notes_json_array = json_object_get(rootJ, "notes");
	size_t note_index;
	json_t *json_value;
	if (notes_json_array) {
		json_array_foreach(notes_json_array, note_index, json_value)
		{theSeq[note_index] = json_number_value(json_value);}
	}
	}
