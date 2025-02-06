// a few shortcuts for me

float hp=5.08f;
float HP=5.08f;

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
	// ScrewBlack
} // common screw

void childInput(int objId, float posA, float posB) {
	addInput(createInputCentered<PJ301MPort>(mm2px(Vec(posA, posB)), module, objId));
} // common input socket

void childOutput(int objId, float posA, float posB) {
	addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(posA, posB)), module, objId));
} // common output socket

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
		case 10:	addChild(createLightCentered<TinySimpleLight<RedLight>>(mm2px(Vec(posA, posB)), module, objId)); break;
		case 11:	addChild(createLightCentered<TinySimpleLight<GreenLight>>(mm2px(Vec(posA, posB)), module, objId)); break;
		case 12:	addChild(createLightCentered<TinySimpleLight<BlueLight>>(mm2px(Vec(posA, posB)), module, objId)); break;
		case 13:	addChild(createLightCentered<TinySimpleLight<WhiteLight>>(mm2px(Vec(posA, posB)), module, objId)); break;
		case 14:	addChild(createLightCentered<TinySimpleLight<YellowLight>>(mm2px(Vec(posA, posB)), module, objId)); break;
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

void childPushbutton(int objIdA, int objIdB, float posA, float posB) {
	addParam(createLightParamCentered<VCVLightLatch<MediumSimpleLight<YellowLight>>>(mm2px(Vec(posA, posB)), module, objIdA, objIdB));
	// VCVBezelLatch
	// VCVLightBezelLatch
	// addParam(createLightParamCentered<VCVLightLatch<MediumSimpleLight<WhiteLight>>>(mm2px(Vec(5.08, 22.86)), module, TrigPrec2::RNDBUTTON2_PARAM, TrigPrec2::RNDLED2_LIGHT));
} // pushbutton


/*
// addParam(createParamCentered<squareToggle>(themePos("RETRIGGER_SWITCH"), module, XY::RETRIGGER_SWITCH));
struct squareToggle : VoxglitchSwitch {

  ImageWidget* voxglitch_shadow;

  squareToggle() {
    momentary = false;
    shadow->opacity = 0;

    addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/square_light_off.svg")));
    addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/square_light_on.svg")));

    // Add the shadow below everything
    // voxglitch_shadow = new ImageWidget("res/themes/default/square_shadow.png", 15.0, 15.0, 0.4);
    // this->addChildBottom(voxglitch_shadow);
    // voxglitch_shadow->setPosition(Vec(-11, -8));

    box.size = Vec(22.0, 22.0); // was 15.5   (19.28)
}
*/

/* -------------------------------------------------- */

// --------------------------------------------------

// as seen in VCV Fundamental Logic
// usage: addParam(createLightParamCentered<VCVBezelLightBigRed>(mm2px(Vec(HP*3, HP*17)), module, 
//		SaveMeMonoWide::RECORD_PARAM, SaveMeMonoWide::LED_RECORD_PARAM));

struct VCVButton_square : app::SvgSwitch {
	VCVButton_square() {momentary = false;
	addFrame(Svg::load(asset::plugin(pluginInstance, "res/components/VCVButton_square.svg")));
	}
};

template <typename TBase>
struct VCVBezelLightBig : TBase {
	VCVBezelLightBig() {
		this->borderColor = color::BLACK_TRANSPARENT;
		this->bgColor = color::BLACK_TRANSPARENT;
		this->box.size = mm2px(math::Vec(5, 5));
	}
};

template <typename TBase, typename TLight = WhiteLight>
struct LightButton : TBase {
	app::ModuleLightWidget* light;
	LightButton() {light = new TLight;
		light->box.pos = this->box.size.div(2).minus(light->box.size.div(2));
		this->addChild(light);}
	app::ModuleLightWidget* getLight() {return light;}
};

using VCVButtonSquareRed = LightButton<VCVButton_square, VCVBezelLightBig<RedLight>>;
using VCVButtonSquareGreen = LightButton<VCVButton_square, VCVBezelLightBig<GreenLight>>;
using VCVButtonSquareBlue = LightButton<VCVButton_square, VCVBezelLightBig<BlueLight>>;
using VCVButtonSquareWhite = LightButton<VCVButton_square, VCVBezelLightBig<WhiteLight>>;
using VCVButtonSquareYellow = LightButton<VCVButton_square, VCVBezelLightBig<YellowLight>>;


