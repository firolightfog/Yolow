/* 
This file is needed by PokeMe . 
I wanted to avoid typing the same thing twice, so here it is.
*/

		menu->addChild(new MenuSeparator);
		menu->addChild(createIndexPtrSubmenuItem("Programmer mode [RESET input]", {"Disabled","Enabled"}, &module->indexPRG));
		// menu->addChild(createIndexPtrSubmenuItem("Precision", {"Perfect (100%)","Very good (95%)","Good (85%)","Not bad (75%)","Wasted (60%)","Crap (50%)","Completely insane (35%)"}, &module->indexPrec));
		// menu->addChild(createIndexPtrSubmenuItem("Precision mode", {"All","Only add","Only skip"}, &module->indexPrecMode));
		menu->addChild(new MenuSeparator);
		// menu->addChild(createBoolPtrMenuItem("Extended output", "", &module->indexExt));
		menu->addChild(createIndexPtrSubmenuItem("Output mode", {"Simple hits","Extended","Values only"}, &module->indexOut));
		menu->addChild(createIndexPtrSubmenuItem("Pulse width", {"As clocked","Full width"}, &module->indexPW));
		menu->addChild(createBoolPtrMenuItem("Choked by left", "", &module->indexChoke));
		menu->addChild(createBoolPtrMenuItem("Loop pattern", "", &module->indexLoop));

		menu->addChild(new MenuSeparator);
		// menu->addChild(createIndexPtrSubmenuItem("Green 1 prob.", {"1.0","0.8","0.5"}, &module->defProb1));
/* don't pass the integer, I need the value! 
		menu->addChild(createIndexPtrSubmenuItem("Yellow 2 prob.", {"100%","90%","80%","70%","60%","50%","40%","30%","20%","10%"}, &module->defProb2));
		menu->addChild(createIndexPtrSubmenuItem("Yellow 3 prob.", {"0.95","0.85","0.75","0.65","0.55","0.45","0.35","0.25","0.15","0.05"}, &module->defProb3));
		menu->addChild(createIndexPtrSubmenuItem("Yellow 4 prob.", {"0.95","0.85","0.75","0.65","0.55","0.45","0.35","0.25","0.15","0.05"}, &module->defProb4));

 */
 		menu->addChild(createIndexPtrSubmenuItem("Blue 7 prob.", {"0th (mute)","1st (always)","2nd","3rd","4th","5th","6th","7th"}, &module->defProb7));
 		menu->addChild(createIndexPtrSubmenuItem("Blue 8 prob.", {"0th (mute)","1st (always)","2nd","3rd","4th","5th","6th","7th"}, &module->defProb8));
 		menu->addChild(createIndexPtrSubmenuItem("Blue 9 prob.", {"0th (mute)","1st (always)","2nd","3rd","4th","5th","6th","7th"}, &module->defProb9));
