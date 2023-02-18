/* 
This file is needed by PokeMe . 
I wanted to avoid typing the same thing twice, so here it is.
*/

		menu->addChild(new MenuSeparator);
		// menu->addChild(createIndexPtrSubmenuItem("Precision", {"Perfect (100%)","Very good (95%)","Good (85%)","Not bad (75%)","Wasted (60%)","Crap (50%)","Completely insane (35%)"}, &module->indexPrec));
		// menu->addChild(createIndexPtrSubmenuItem("Precision mode", {"All","Only add","Only skip"}, &module->indexPrecMode));
		menu->addChild(createIndexPtrSubmenuItem("Pulse width", {"As clocked","Full width"}, &module->indexPW));
		menu->addChild(createBoolPtrMenuItem("Choked by left", "", &module->indexChoke));
		menu->addChild(createBoolPtrMenuItem("Loop pattern", "", &module->indexLoop));
		menu->addChild(createBoolPtrMenuItem("Extended output", "", &module->indexExt));
