// Code is based on @clone45's demo
// What a great chap he is!

/* Remember that the main file needs to have 

	#define HP 5.08
	#define ROWS 8
	#define COLS 16

	#define CELL_WIDTH HP*5
	#define CELL_HEIGHT HP*5
	#define CELL_PADDING HP/3

	#define DRAW_AREA_WIDTH COLS*(CELL_WIDTH+CELL_PADDING)+CELL_PADDING
	#define DRAW_AREA_HEIGHT ROWS*(CELL_HEIGHT+CELL_PADDING)+CELL_PADDING

...

		GridWidget *grid_widget = new GridWidget();
		grid_widget->box.pos = Vec(HP*2,HP*12);
		grid_widget->module = module;
		addChild(grid_widget);

*/

struct GridWidget : TransparentWidget {

	Vulcan *module;

	Vec drag_position;
	bool mouse_lock = false;
	int old_row = -1;
	int old_column = -1;	
	int row, column;

	GridWidget() {box.size = Vec(DRAW_AREA_WIDTH, DRAW_AREA_HEIGHT);}

	// it's OK
	void draw(const DrawArgs &args) override {
		const auto vg = args.vg;

		// Save the drawing context to restore later
		nvgSave(vg);

		// Debugging code for draw area, which often has to be set experimentally
		nvgBeginPath(vg);
		nvgRect(vg, 0, 0, box.size.x, box.size.y);
		nvgFillColor(vg, nvgRGBA(25, 125, 125, 25));
		nvgFill(vg);
		
		if(module) {
			for(int column=0; column < COLS; column++) {
				for(int row=0; row < ROWS; row++) {
					float x = CELL_PADDING + (column * CELL_WIDTH) + (column * CELL_PADDING);
					float y = CELL_PADDING + ((ROWS - row - 1) * CELL_HEIGHT) + ((ROWS - row - 1) * CELL_PADDING);

					// Draw square
					nvgBeginPath(vg);
					nvgRect(vg, x, y, CELL_WIDTH, CELL_HEIGHT);

					if(module->grid_data[column][row] == true) {nvgFillColor(vg, nvgRGB(25,150,25));}
					else {nvgFillColor(vg, nvgRGB(160,160,160));}
					nvgFill(vg);
				}
			}
		}

		nvgRestore(vg);
	}

	// it's OK
	void getRowAndColumnFromVec(Vec position) {
		row = ROWS - (position.y / (CELL_HEIGHT + CELL_PADDING));
		column = position.x / (CELL_WIDTH + CELL_PADDING);
		row = clamp(row, 0, ROWS - 1);
		column = clamp(column, 0, COLS - 1);
	}

	// it's not so OK
	void onButton(const event::Button &e) override {
		// TransparentWidget::onButton(e);
		if(isMouseInDrawArea(e.pos)) {
			if(e.button == GLFW_MOUSE_BUTTON_LEFT && e.action == GLFW_PRESS) {
				e.consume(this);
				getRowAndColumnFromVec(e.pos);
				module->grid_data[column][row] = !module->grid_data[column][row];
				drag_position = e.pos;
				old_column = column;
				old_row = row;
			}
		}
	}

	// it's probably OK
	void onDragMove(const event::DragMove &e) override {
		// TransparentWidget::onDragMove(e);
		double zoom = getAbsoluteZoom();
		drag_position = drag_position.plus(e.mouseDelta.div(zoom));
		if(isMouseInDrawArea(drag_position)) {
			getRowAndColumnFromVec(drag_position);
			if((row != old_row) || (column != old_column)) {
				module->grid_data[column][row] = ! module->grid_data[column][row];
				old_column = column;
				old_row = row;
			}
		}
		else {this->mouse_lock = false;}
	}

	// it's OK
	void onEnter(const event::Enter &e) override {TransparentWidget::onEnter(e);}
	void onLeave(const event::Leave &e) override {TransparentWidget::onLeave(e);}
	void onHover(const event::Hover& e) override {TransparentWidget::onHover(e);}
	// void step() override {TransparentWidget::step();}

	// it's OK
	bool isMouseInDrawArea(Vec position) {
		if(position.x < 0) return(false);
		if(position.y < 0) return(false);
		if(position.x >= DRAW_AREA_WIDTH) return(false);
		if(position.y >= DRAW_AREA_HEIGHT) return(false);
		return(true);
	}

};
