/* 
This file is needed by PushmeSeq and its PushmeMore expander. 
I wanted to avoid typing the same thing twice, so here it is.
*/

	void onHoverKey(const event::HoverKey &e) override {
		if (e.key >= GLFW_KEY_1 && e.key <= GLFW_KEY_6) {
			if (e.action == GLFW_PRESS && ((e.mods & RACK_MOD_MASK) == GLFW_MOD_SHIFT)) {
				float key_number = e.key - 49; // 49 is the ascii number for key #1
				module->clearKnob(key_number);	// this doesn't work
				e.consume(this);
			}
			else if (e.action == GLFW_PRESS) {
				float key_number = e.key - 49; // 49 is the ascii number for key #1
				module->keyKnob(key_number);
				e.consume(this);
			}
		}
		else if ((e.key == GLFW_KEY_X) || (e.key == GLFW_KEY_C) || (e.key == GLFW_KEY_V) || (e.key == GLFW_KEY_B) || (e.key == GLFW_KEY_N) || (e.key == GLFW_KEY_M)) {
			if (e.action == GLFW_PRESS && ((e.mods & RACK_MOD_MASK) != GLFW_MOD_CONTROL)) {
				module->beatKnob(e.key); 
				e.consume(this);
			}
		}
		else if ((e.key == GLFW_KEY_R) && e.action == GLFW_PRESS && ((e.mods & RACK_MOD_MASK) != GLFW_MOD_CONTROL)) {
			module->xrndKnob(); 
			e.consume(this);
		}
		else if ((e.key == GLFW_KEY_I) && e.action == GLFW_PRESS && ((e.mods & RACK_MOD_MASK) != GLFW_MOD_CONTROL)) {
		// else if((e.key == GLFW_KEY_I) && ((e.mods & RACK_MOD_MASK) != GLFW_MOD_CONTROL)) {module->invKnob(); e.consume(this);}
			module->invKnob(); 
			e.consume(this);
		}
		ModuleWidget::onHoverKey(e);
	}
