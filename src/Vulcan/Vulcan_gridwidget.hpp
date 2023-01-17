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

    int grid_data[COLS][ROWS];
	NVGcolor grid_RGBA[5]={
		nvgRGBA(215,215,215,195),	// blank
		nvgRGBA(52,255,52,252),		// trigger
		nvgRGBA(183,195,75,125),	// blank highlighted
		nvgRGBA(152,152,25,125),	// trigger highlighted
		nvgRGBA(255,25,25,252),};	// unknown

...

		GridWidget *grid_widget = new GridWidget();
		grid_widget->box.pos = Vec(HP*2,HP*12);
		grid_widget->module = module;
		addChild(grid_widget);

*/

#define DRAW_AREA_WIDTH COLS*(CELL_WIDTH+CELL_PADDING)+CELL_PADDING
#define DRAW_AREA_HEIGHT ROWS*(CELL_HEIGHT+CELL_PADDING)+CELL_PADDING

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
		nvgFillColor(vg, nvgRGBA(25, 25, 25, 250));
		nvgFill(vg);
			
		if(module) {
			for(int xcol=0; xcol < COLS; xcol++) {
			// for(int xcol=0; xcol < module->paramVal[module->DEBUG_PARAM]; xcol++) {
				for(int xrow=0; xrow < ROWS; xrow++) {
					float x = CELL_PADDING + (xcol * CELL_WIDTH) + (xcol * CELL_PADDING);
					float y = CELL_PADDING + (xrow * CELL_HEIGHT) + (xrow * CELL_PADDING);

					// Draw square
					nvgBeginPath(vg);
					nvgRect(vg, x, y, CELL_WIDTH, CELL_HEIGHT);

					if (module->paramVal[module->FOCUS_PARAM]==1 
						&& xcol == module->CURR_COL[xrow]) {
						nvgFillColor(vg, nvgLerpRGBA(nvgRGBA(225,251,251,251),module->grid_RGBA[module->grid_data[xcol][xrow]],0.42));
					}
					else {
						nvgFillColor(vg, module->grid_RGBA[module->grid_data[xcol][xrow]]);
					}
					nvgFill(vg);
				}
			}

			// this is not good, itt
			if (module->paramVal[module->FOCUS_PARAM]==0
			 || mousey<0 || mousey>=box.size.y ) {
			nvgBeginPath(vg);
			nvgRect(vg, mousex-2, mousey+16, 42, 21);	
			nvgFillColor(vg, nvgRGBA(25,25,25,225));	// sötét
			nvgFill(vg);
			nvgFillColor(vg, nvgRGBA(225,225,225,225));	// világos
			nvgText(vg, mousex, mousey+30, tooltiptext, NULL);
			}

		}
		
		nvgRestore(vg);
	}

	// it's probably not OK at all
	// char colLetter[17]="ABCDEFGHIJKLMNOP";
	char colLetter[17]={'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P'};
	char tooltiptext[64];
	float mousex=0;
	float mousey=0;
	void onHover(const event::Hover& e) override {
		TransparentWidget::onHover(e);
		if(isMouseInDrawArea(e.pos)) {
			e.consume(this);
			mousex=e.pos[0];
			mousey=e.pos[1];		
			getRowAndColumnFromVec(e.pos);
			module->outputs[module->DEBUG_OUTPUT].setVoltage(module->grid_data[column][row]);
			sprintf(tooltiptext,"%c%d: %d",colLetter[column],row,module->grid_data[column][row]);
		} 
		else {mousex=0;mousey=0;}
	}

	// it's OK
	void getRowAndColumnFromVec(Vec position) {
		// row = ROWS - (position.y / (CELL_HEIGHT + CELL_PADDING));
		row = position.y / (CELL_HEIGHT + CELL_PADDING);
		column = position.x / (CELL_WIDTH + CELL_PADDING);
		row = clamp(row, 0, ROWS - 1);
		column = clamp(column, 0, COLS - 1);
		// column = clamp(column, 0, module->paramVal[module->DEBUG_PARAM] - 1);
	}

	// it's not so OK
	void onButton(const event::Button &e) override {
		// TransparentWidget::onButton(e);
		if(isMouseInDrawArea(e.pos)) {
			// mouse is clicking the area
			if(e.button == GLFW_MOUSE_BUTTON_LEFT && e.action == GLFW_PRESS) {
				// it is a click indeed
				e.consume(this);
				getRowAndColumnFromVec(e.pos);	// update column and row variables
				if ((e.mods & RACK_MOD_MASK) == GLFW_MOD_CONTROL) {	
					// if control-click then change color 
					module->grid_data[column][row]-=1;
					if (module->grid_data[column][row]<0) {module->grid_data[column][row]=0;} 
				}
				else if ((e.mods & RACK_MOD_MASK) == GLFW_MOD_SHIFT) {
					// if shift-click then change color
					module->grid_data[column][row]+=1;
					if (module->grid_data[column][row]>10) {module->grid_data[column][row]=10;} 
				}
				else if (module->grid_data[column][row]==0) {	
					// if blank then color it
					module->grid_data[column][row]=module->CURR_MODE;
				}				
				else {module->grid_data[column][row]=0;}	// or blank it
				drag_position = e.pos;
				old_column = column;
				old_row = row;
			}
		}
	}

	// it's OK
	void onDragMove(const event::DragMove &e) override {
		// TransparentWidget::onDragMove(e);
		double zoom = getAbsoluteZoom();
		drag_position = drag_position.plus(e.mouseDelta.div(zoom));
		if(isMouseInDrawArea(drag_position)) {
			getRowAndColumnFromVec(drag_position);
			if((row != old_row) || (column != old_column)) {
				if (module->grid_data[column][row]==0) {module->grid_data[column][row]=module->CURR_MODE;}
				else {module->grid_data[column][row]=0;}
				old_column = column;
				old_row = row;
			}
		}
		else {this->mouse_lock = false;}
	}

	// it's OK
	void onEnter(const event::Enter &e) override {TransparentWidget::onEnter(e);}
	void onLeave(const event::Leave &e) override {TransparentWidget::onLeave(e);}
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
