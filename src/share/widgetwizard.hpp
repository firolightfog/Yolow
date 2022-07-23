// a few shortcuts for me

float hp=5.08;
float HP=5.08;

// to add a text label to this widget 
// https://github.com/squinkylabs/SquinkyVCV
// https://vcvrack.com/docs-v2/Label_8hpp_source
// https://vcvrack.com/docs-v2/structrack_1_1ui_1_1Label
Label* childLabel(float posA, float posB, const char* str, const int fSize=10, const NVGcolor& color = nvgRGBA(0, 0, 0, 255))
{
	Label* label = new Label();
	label->box.pos = Vec(posA, posB);
	label->text = str;
	label->fontSize = fSize;
	label->color = color;
	// label->alignment = rack::ui::Label::CENTER_ALIGNMENT;
	addChild(label);
	return label;
}		  

void childScrew(float posA, float posB) {
	addChild(createWidget<ScrewSilver>(Vec(posA, posB)));
} // common screw

void childInput(int objId, float posA, float posB) {
	addInput(createInputCentered<PJ301MPort>(mm2px(Vec(posA, posB)), module, objId));
} // common input socket

void childOutput(int objId, float posA, float posB) {
	addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(posA, posB)), module, objId));
} // common output socket

void childPushbutton(int objIdA, int objIdB, float posA, float posB) {
	addParam(createLightParamCentered<VCVLightLatch<MediumSimpleLight<YellowLight>>>(mm2px(Vec(posA, posB)), module, objIdA, objIdB));
	// addParam(createLightParamCentered<VCVLightLatch<MediumSimpleLight<WhiteLight>>>(mm2px(Vec(5.08, 22.86)), module, TrigPrec2::RNDBUTTON2_PARAM, TrigPrec2::RNDLED2_LIGHT));
} // pushbutton

void childSwitch(int objId, int sizeId, float posA, float posB) {
	switch (sizeId) {
		case 0:	addParam(createParamCentered<CKSS>(mm2px(Vec(posA, posB)), module, objId)); break;
		case 1:	addParam(createParamCentered<CKSSThree>(mm2px(Vec(posA, posB)), module, objId)); break;
		case 2:	addParam(createParamCentered<CKSSThreeHorizontal>(mm2px(Vec(posA, posB)), module, objId)); break;
		case 3:	addParam(createParamCentered<CKD6>(mm2px(Vec(posA, posB)), module, objId)); break;
		case 4:	addParam(createParamCentered<TL1105>(mm2px(Vec(posA, posB)), module, objId)); break;
		case 5:	addParam(createParamCentered<VCVButton>(mm2px(Vec(posA, posB)), module, objId)); break;
		default:	addParam(createParamCentered<CKSS>(mm2px(Vec(posA, posB)), module, objId)); break;
	};
} // common medium knob

int RED=0;
int GRN=1;
int BLU=2;
int WHT=3;
int YLW=4;

void childLed(int objId, int colorId, float posA, float posB) {
	childLight(objId, colorId, posA, posB);
}

void childLight(int objId, int colorId, float posA, float posB) {
	switch (colorId) {
		case 0:	addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(posA, posB)), module, objId)); break;
		case 1:	addChild(createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(posA, posB)), module, objId)); break;
		case 2:	addChild(createLightCentered<MediumLight<BlueLight>>(mm2px(Vec(posA, posB)), module, objId)); break;
		case 3:	addChild(createLightCentered<MediumLight<WhiteLight>>(mm2px(Vec(posA, posB)), module, objId)); break;
		case 4:	addChild(createLightCentered<MediumLight<YellowLight>>(mm2px(Vec(posA, posB)), module, objId)); break;
		case 1975125:	addChild(createLightCentered<TinySimpleLight<YellowLight>>(mm2px(Vec(posA, posB)), module, objId)); break;
		default: addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(posA, posB)), module, objId)); 
	}
} // common Led

void childButtonmom(int objId, int sizeId, float posA, float posB) {
	switch (sizeId) {
		case 0:	addParam(createParamCentered<VCVButton>(mm2px(Vec(posA, posB)), module, objId)); break;
		case 1:	addParam(createParamCentered<CKD6>(mm2px(Vec(posA, posB)), module, objId)); break;
		default:	addParam(createParamCentered<VCVButton>(mm2px(Vec(posA, posB)), module, objId)); break;
	};
} // common medium knob

void childKnob(int objId, int sizeId, float posA, float posB) {
	switch (sizeId) {
		case 0:	addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(posA, posB)), module, objId)); break;
		case 1:	addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(posA, posB)), module, objId)); break;
		case 2:	addParam(createParamCentered<RoundBigBlackKnob>(mm2px(Vec(posA, posB)), module, objId)); break;
		default:	addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(posA, posB)), module, objId)); break;
	};
} // common medium knob

void childLedSlide(int objIdA, int objIdB, int colorId, float posA, float posB) {
	switch (colorId) {
		case 0:
			addParam(createLightParamCentered<LEDSliderWhite>(mm2px(Vec(posA, posB)), module, objIdA, objIdB));
			break;
		case 1:
			addParam(createLightParamCentered<LEDSliderRed>(mm2px(Vec(posA, posB)), module, objIdA, objIdB));
			break;
		case 2:
			addParam(createLightParamCentered<LEDSliderGreen>(mm2px(Vec(posA, posB)), module, objIdA, objIdB));
			break;
		case 3:
			addParam(createLightParamCentered<LEDSliderBlue>(mm2px(Vec(posA, posB)), module, objIdA, objIdB));
			break;
		case 4:
			addParam(createLightParamCentered<LEDSliderYellow>(mm2px(Vec(posA, posB)), module, objIdA, objIdB));
			break;
		default:			
			addParam(createLightParamCentered<LEDSliderWhite>(mm2px(Vec(posA, posB)), module, objIdA, objIdB));
	}
} // common LED slider





