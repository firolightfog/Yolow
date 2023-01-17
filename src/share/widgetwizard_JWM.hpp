// this should go to after process, before the addChild block

struct Pres1tDisplay : LightWidget {
	Pres1t *module;
	Pres1tDisplay(){}

	void onButton(const event::Button &e) override {
		if (e.action == GLFW_PRESS && e.button == GLFW_MOUSE_BUTTON_LEFT) {
			e.consume(this);
			module->setCellOnByDisplayPos(e.pos.x, e.pos.y, true, true);
		}
		if (e.action == GLFW_PRESS && e.button == GLFW_MOUSE_BUTTON_RIGHT) {
			e.consume(this);
			module->setCellOnByDisplayPos(e.pos.x, e.pos.y, true, false);
		}
	}

	void drawLayer(const DrawArgs &args, int layer) override {
		//background
		nvgFillColor(args.vg, nvgRGB(0, 0, 0));
		nvgBeginPath(args.vg);
		nvgRect(args.vg, 0, 0, box.size.x, box.size.y);
		nvgFill(args.vg);

		if(layer == 1){
			//grid
			nvgStrokeColor(args.vg, nvgRGB(60, 70, 73));
			for(int i=1;i<COLS;i++){
				nvgStrokeWidth(args.vg, 1);
				nvgBeginPath(args.vg);
				nvgMoveTo(args.vg, i * HW, 0);
				nvgLineTo(args.vg, i * HW, HW*ROWS);
				nvgStroke(args.vg);
			}
			for(int i=1;i<ROWS+1;i++){
				nvgStrokeWidth(args.vg, 1);
				nvgBeginPath(args.vg);
				nvgMoveTo(args.vg, 0, i * HW);
				nvgLineTo(args.vg, box.size.x, i * HW);
				nvgStroke(args.vg);
			}

			if(!module){ return; }

			//cells
			for(int i=0;i<CELLS;i++){
			int y = i / COLS;
			int x = i % COLS;
			if(module->selectedWriteCellIdx == i){
				nvgStrokeColor(args.vg, nvgRGB(25, 150, 252)); //blue
				nvgStrokeWidth(args.vg, 2);
				nvgBeginPath(args.vg);
				nvgRect(args.vg, x * HW, y * HW, HW, HW);
				nvgStroke(args.vg);
			}
			
			if(module->selectedReadCellIdx == i){
				nvgStrokeColor(args.vg, nvgRGB(255, 243, 9)); //yellow
				nvgStrokeWidth(args.vg, 2);
				nvgBeginPath(args.vg);
				nvgRect(args.vg, x * HW, y * HW, HW, HW);
				nvgStroke(args.vg);
			}
			
			if(module->cells[i] != module->EMPTY_CELL_VAL){
				float yPos = rescalefjw(module->cells[i], -10, 10, 2, HW-4);
				nvgFillColor(args.vg, nvgRGB(60, 70, 73));
				nvgBeginPath(args.vg);
				float topOfCell = y * HW+2;
				nvgRect(args.vg, x * HW+2, topOfCell + HW - 4 - yPos, HW-4, yPos);
				nvgFill(args.vg);
			}
		}
		}
		Widget::drawLayer(args, layer);
	}
};

// this should go to the addChild block

	// Pres1tDisplay *display = new Pres1tDisplay();
	// display->module = module;
	// display->box.pos = Vec(0, 15);
	// display->box.size = Vec(box.size.x, 250);
	// addChild(display);