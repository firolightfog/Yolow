#pragma once
#include <rack.hpp>


using namespace rack;

// Declare the Plugin, defined in plugin.cpp
extern Plugin* pluginInstance;

// Declare each Model, defined in each module source file
// extern Model* modelXxx;			// delete it
// extern Model* modelEuclk;

extern Model* modelPolyExp10;
extern Model* modelPolyCopy10;
extern Model* modelPolyCopy16;
extern Model* modelMKnob;		// I don't like it
extern Model* modelSlide6;
extern Model* modelSlide16;
extern Model* modelTrigPrec;
extern Model* modelTrigPrec6;
extern Model* modelTrigPrec2;
extern Model* modelTrigPrec1;
extern Model* modelSeqP5;
extern Model* modelSeqP16;
extern Model* modelChSel2;
extern Model* modelChSel6;
extern Model* modelChSel8;
extern Model* modeltHex;
extern Model* modeltXOr;
extern Model* modelnineLives;
extern Model* modelRandVolt10;
extern Model* modelEnveloop;
extern Model* modelCelesta;
