// a few predefined things not to waste time and code with it
// it should be too much of an overhead I assume

	enum ParamId 	{
		ENUMS(PARAM,50),
		PARAMS_LEN};
	enum InputId 	{
		ENUMS(IN,25),
		INPUTS_LEN};
	enum OutputId 	{
		ENUMS(OUT,25),
		DEBUG_OUTPUT, 
		OUTPUTS_LEN};
	enum LightId {
		ENUMS(LIGHT,25),
		LIGHTS_LEN};

	float paramVal[PARAMS_LEN];
	float inputVolt[INPUTS_LEN];
	float outputVolt[OUTPUTS_LEN];
	float lightVal[LIGHTS_LEN];
	
	int loop=0;		// for skipping rounds in process()
	
	