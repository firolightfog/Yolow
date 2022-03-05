#include "plugin.hpp"


Plugin* pluginInstance;


void init(Plugin* p) {
	pluginInstance = p;

	// Add modules here
	// p->addModel(modelXxx);
	// p->addModel(modelEuclk);

	p->addModel(modelPolyExp10);
	p->addModel(modelPolyCopy10);
	p->addModel(modelPolyCopy16);
	p->addModel(modelMKnob);
	p->addModel(modelSlide6);
	p->addModel(modelSlide16);
	p->addModel(modelTrigPrec6);
	p->addModel(modelTrigPrec2);
	p->addModel(modelSeqP5);
	p->addModel(modelSeqP16);
	p->addModel(modelChSel2);
	p->addModel(modelChSel6);
	p->addModel(modelChSel8);
	p->addModel(modeltXOr);
	p->addModel(modeltHex);
	p->addModel(modelnineLives);
	p->addModel(modelRandVolt10);

	// Any other plugin initialization may go here.
	// As an alternative, consider lazy-loading assets and lookup tables when your module is created to reduce startup times of Rack.
}
