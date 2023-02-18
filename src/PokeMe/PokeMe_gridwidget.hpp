// Code is based on @clone45's demo
// What a great chap he is!

/* Remember that the main file needs to have 

	#define HP 5.08
	#define ROWS 8
	#define COLS 16

	#define CELL_WIDTH HP*5
	#define CELL_HEIGHT HP*5
	#define CELL_PADDING HP/3

...

		GridWidget *grid_widget = new GridWidget();
		grid_widget->box.pos = Vec(HP*2,HP*12);
		grid_widget->module = module;
		addChild(grid_widget);

*/

#define DRAW_AREA_WIDTH COLS*(CELL_WIDTH+CELL_PADDING)+CELL_PADDING
#define DRAW_AREA_HEIGHT ROWS*(CELL_HEIGHT+CELL_PADDING)+CELL_PADDING

struct GridWidgetPokeMe : TransparentWidget {

	PokeMe *module;

	Vec drag_position;
	bool mouse_lock = false;
	int old_row = -1;
	int old_column = -1;	
	int row, column;

	GridWidgetPokeMe() {box.size = Vec(DRAW_AREA_WIDTH, DRAW_AREA_HEIGHT);}

	// it's OK
	void draw(const DrawArgs &args) override {
		const auto vg = args.vg;

		// Save the drawing context to restore later
		nvgSave(vg);

		// Debugging code for draw area, which often has to be set experimentally
		nvgBeginPath(vg);
		nvgRect(vg, 0, 0, box.size.x, box.size.y);
		nvgFillColor(vg, nvgRGBA(25, 25, 25, 225));
		nvgFill(vg);
		
		if(module) {
			for(int column=0; column < COLS; column++) {
				for(int row=0; row < ROWS; row++) {
					float x = CELL_PADDING + (column * CELL_WIDTH) + (column * CELL_PADDING);
					// float y = CELL_PADDING + ((ROWS - row - 1) * CELL_HEIGHT) + ((ROWS - row - 1) * CELL_PADDING);
					float y = CELL_PADDING + (row * CELL_HEIGHT) + (row * CELL_PADDING);

					// Draw square
					nvgBeginPath(vg);
					nvgRect(vg, x, y, CELL_WIDTH, CELL_HEIGHT);

					int highlight=255;
					if (ROWS*column+row==module->CURR_STEP) {highlight=205;}
					float this_step=module->grid_data[ROWS*column+row];
					if (this_step == 0) {nvgFillColor(vg, nvgRGBA(160,160,160,highlight));}
					else if (this_step == 1) {nvgFillColor(vg, nvgRGBA(25,160,25,highlight));}
			
					else if (this_step == 2) {nvgFillColor(vg, nvgRGBA(190,190,90,highlight));}
					else if (this_step == 3) {nvgFillColor(vg, nvgRGBA(180,180,60,highlight));}
					else if (this_step == 4) {nvgFillColor(vg, nvgRGBA(170,170,30,highlight));}
					else if (this_step == 5) {nvgFillColor(vg, nvgRGBA(160,160,00,highlight));}
					
					else if (this_step == 6) {nvgFillColor(vg, nvgRGBA(190,90,190,highlight));}
					else if (this_step == 7) {nvgFillColor(vg, nvgRGBA(60,180,180,highlight));}
					else if (this_step == 8) {nvgFillColor(vg, nvgRGBA(30,170,170,highlight));}
					else if (this_step == 9) {nvgFillColor(vg, nvgRGBA(00,160,160,highlight));}

					else if (this_step == 10) {nvgFillColor(vg, nvgRGBA(160,25,25,highlight));}
					else {nvgFillColor(vg, nvgRGBA(160,160,160,highlight));}
					nvgFill(vg);
				}
			}
		}

		nvgRestore(vg);
	}

	// it's OK
	void getRowAndColumnFromVec(Vec position) {
		// row = ROWS - (position.y / (CELL_HEIGHT + CELL_PADDING));
		row = position.y / (CELL_HEIGHT + CELL_PADDING);
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
				// if (module->grid_data[ROWS*column+row]==0)
					// {module->grid_data[ROWS*column+row]=1;}
				// else {module->grid_data[ROWS*column+row]=0;}
				if ((e.mods & RACK_MOD_MASK) == GLFW_MOD_CONTROL) {	
					// if control-click then change color 
					module->grid_data[ROWS*column+row]-=1;
					if (module->grid_data[ROWS*column+row]<0) {module->grid_data[ROWS*column+row]=0;} 
				}
				else if ((e.mods & RACK_MOD_MASK) == GLFW_MOD_SHIFT) {
					// if shift-click then change color
					module->grid_data[ROWS*column+row]+=1;
					if (module->grid_data[ROWS*column+row]>10) {module->grid_data[ROWS*column+row]=10;} 
				}
				else if (module->grid_data[ROWS*column+row]==0) {	
					// if blank then color it
					module->grid_data[ROWS*column+row]=module->CURR_MODE;
				}				
				else {module->grid_data[ROWS*column+row]=0;}	// or blank it
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
				// module->grid_data[ROWS*column+row] = ! module->grid_data[ROWS*column+row];
				if (module->grid_data[ROWS*column+row]==0) {
					module->grid_data[ROWS*column+row]=module->CURR_MODE;
				}
				else {module->grid_data[ROWS*column+row]=0;}	// or blank it
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
