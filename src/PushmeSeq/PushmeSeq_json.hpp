/* 
This file is needed by PushmeSeq and its PushmeMore expander. 
I wanted to avoid typing the same thing twice, so here it is.
*/

	// this block is to save and reload a variable
	json_t* dataToJson() override {
	json_t* rootJ = json_object();
	json_object_set_new(rootJ, "mode", json_integer(indexPrec));
	return rootJ;}

	void dataFromJson(json_t* rootJ) override {
	json_t *modeJ = json_object_get(rootJ, "mode");
	if (modeJ) indexPrec = json_integer_value(modeJ);}
