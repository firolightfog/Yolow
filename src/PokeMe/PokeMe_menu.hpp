/* 
This file is needed by PokeMe . 
I wanted to avoid typing the same thing twice, so here it is.
*/

		menu->addChild(new MenuSeparator);
		
		// menu->addChild(createIndexPtrSubmenuItem("Programmer mode [RESET input]", {"Disabled","Enabled"}, &module->indexPRG));
		// menu->addChild(new MenuSeparator);
		menu->addChild(createIndexPtrSubmenuItem("Output mode", {"Simple hits","Extended","Values only"}, &module->indexOut));
		menu->addChild(createIndexPtrSubmenuItem("Pulse width", {"As clocked","Full width"}, &module->indexPW));
		menu->addChild(createBoolPtrMenuItem("Choked by left", "", &module->indexChoke));
		menu->addChild(createBoolPtrMenuItem("Loop pattern", "", &module->indexLoop));
		menu->addChild(createBoolPtrMenuItem("Sync to mother", "", &module->indexMumSync));

		menu->addChild(new MenuSeparator);

		menu->addChild(createIndexPtrSubmenuItem("Yellow 2 prob.", {"0%","10%","20%","30%","40%","50%","60%","70%","80%","90%","100%"}, &module->defProb2));
		menu->addChild(createIndexPtrSubmenuItem("Yellow 3 prob.", {"0%","10%","20%","30%","40%","50%","60%","70%","80%","90%","100%"}, &module->defProb3));
 		menu->addChild(createIndexPtrSubmenuItem("Yellow 4 prob.", {"0%","10%","20%","30%","40%","50%","60%","70%","80%","90%","100%"}, &module->defProb4));

		menu->addChild(createIndexSubmenuItem("Blue 7 freq.",
			{"not the 8th","not the 7th",
			"not the 6th","not the 5th",
			"not the 4th","not the 3rd",
			"not the 2nd","0th (mute)",
			"1st (always)","only 2nd",
			"only 3rd","only 4th",
			"only 5th","only 6th",
			"only 7th","only 8th"},
			[=]() {return (*&module->defProb7>=0?7:8) + *&module->defProb7;}, // get it
			[=](int mode) {*&module->defProb7 = mode - ((mode>=7)?7:8);} // set it
		));

		menu->addChild(createIndexSubmenuItem("Blue 8 freq.",
			{"not the 8th","not the 7th",
			"not the 6th","not the 5th",
			"not the 4th","not the 3rd",
			"not the 2nd","0th (mute)",
			"1st (always)","only 2nd",
			"only 3rd","only 4th",
			"only 5th","only 6th",
			"only 7th","only 8th"},
			[=]() {return (*&module->defProb8>=0?7:8) + *&module->defProb8;}, // get it
			[=](int mode) {*&module->defProb8 = mode - ((mode>=7)?7:8);} // set it
		));

		menu->addChild(createIndexSubmenuItem("Blue 9 freq.",
			{"not the 8th","not the 7th",
			"not the 6th","not the 5th",
			"not the 4th","not the 3rd",
			"not the 2nd","0th (mute)",
			"1st (always)","only 2nd",
			"only 3rd","only 4th",
			"only 5th","only 6th",
			"only 7th","only 8th"},
			[=]() {return (*&module->defProb9>=0?7:8) + *&module->defProb9;}, // get it
			[=](int mode) {*&module->defProb9 = mode - ((mode>=7)?7:8);} // set it
		));
		
 		// menu->addChild(createIndexPtrSubmenuItem("Blue 7 freq.", {"0th (mute)","1st (always)","only 2nd","only 3rd","only 4th","only 5th","only 6th","only 7th","only 8th"}, &module->defProb7));
 		// menu->addChild(createIndexPtrSubmenuItem("Blue 8 freq.", {"0th (mute)","1st (always)","only 2nd","only 3rd","only 4th","only 5th","only 6th","only 7th","only 8th"}, &module->defProb8));
 		// menu->addChild(createIndexPtrSubmenuItem("Blue 9 freq.", {"0th (mute)","1st (always)","only 2nd","only 3rd","only 4th","only 5th","only 6th","only 7th","only 8th"}, &module->defProb9));

		menu->addChild(new MenuSeparator);
		menu->addChild(createSubmenuItem("Shortcuts", "",
			[=](Menu* menu) {
				menu->addChild(createMenuLabel("Ctrl-1 sets the next mark 1 (green)"));
				menu->addChild(createMenuLabel("Ctrl-2 sets the next mark 2 (yellow)"));
				menu->addChild(createMenuLabel("Ctrl-3 sets the next mark 3 (yellow)"));
				menu->addChild(createMenuLabel("Ctrl-4 sets the next mark 4 (yellow)"));
				menu->addChild(createMenuLabel("Ctrl-5 sets the next mark 5 (orange)"));
				menu->addChild(createMenuLabel("Ctrl-6 sets the next mark 6 (purple)"));
				menu->addChild(createMenuLabel("Ctrl-7 sets the next mark 7 (blue)"));
				menu->addChild(createMenuLabel("Ctrl-8 sets the next mark 8 (blue)"));
				menu->addChild(createMenuLabel("Ctrl-9 sets the next mark 9 (blue)"));
				menu->addChild(createMenuLabel("Ctrl-x sets the next mark break (red)"));
				menu->addChild(createMenuLabel("Ctrl-Alt-R randomizes with mixed values"));
				// menu->addChild(createMenuItem("Import from OnePoint text file", "", [=]() {module->importFieldsFromTxt(1);}));
			}
		));