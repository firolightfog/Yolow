/* 
This file is needed by PokeMe. 
I wanted to avoid typing the same thing twice, so here it is.
*/

	void onHoverKey(const event::HoverKey &e) override {
		if (e.key >= GLFW_KEY_1 && e.key <= GLFW_KEY_9) {
			if (e.action == GLFW_PRESS && ((e.mods & RACK_MOD_MASK) == GLFW_MOD_CONTROL)) {
				module->CURR_MODE=(e.key-49+1);
				e.consume(this);
			}
		}
		else if (e.key == GLFW_KEY_X) {
			if (e.action == GLFW_PRESS && ((e.mods & RACK_MOD_MASK) == GLFW_MOD_CONTROL)) {
				module->CURR_MODE=10;
				e.consume(this);
			}
		}
		else if (e.key == GLFW_KEY_R) {
			if (e.action == GLFW_PRESS && ((e.mods & RACK_MOD_MASK) == GLFW_MOD_ALT)) {
				module->indexRandom=10;
				module->onRandomize();
				e.consume(this);
			}
		}
		ModuleWidget::onHoverKey(e);
	}
