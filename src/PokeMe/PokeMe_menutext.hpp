struct TextFieldMenuItem : TextField {
	TextFieldMenuItem() {
		box.size = Vec(120, 20);
		multiline = false;
	}
};

struct ProgramNameMenuItem : TextFieldMenuItem {
	PokeMe* module;
	ProgramNameMenuItem(PokeMe* basically_module) {
		module=basically_module;
		if (module) {text=module->text_data;} else {text="";}
	}
	void onChange(const event::Change& e) override {
		TextFieldMenuItem::onChange(e);
		if (module) {module->text_data=text;}
	}
};
