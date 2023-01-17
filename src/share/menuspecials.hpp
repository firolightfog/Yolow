#include <assert.h>
#include <osdialog.h>
#include <regex>
#include "rack.hpp"

// as seen in https://github.com/Ahornberg/Ahornberg-VCV-Modules/blob/c9f11d396458cbe6fffe5ae6106a583a70bdca23/src/util/Widgets.cpp
static void appendSelectionItems(ui::Menu* menu, WeakPtr<ModuleWidget> moduleWidget, std::string presetDir) {
	bool hasPresets = false;
	// Note: This is not cached, so opening this menu each time might have a bit of latency.
	if (system::isDirectory(presetDir)) {
		std::vector<std::string> entries = system::getEntries(presetDir);
		std::sort(entries.begin(), entries.end());
		for (std::string path : entries) {
			std::string name = system::getStem(path);
			// Remove "1_", "42_", "001_", etc at the beginning of preset filenames
			std::regex r("^\\d+_");
			name = std::regex_replace(name, r, "");

			if (system::isDirectory(path)) {
				hasPresets = true;

				menu->addChild(createSubmenuItem(name, "", [=](ui::Menu* menu) {
					if (!moduleWidget) {
						return;
					}
					appendSelectionItems(menu, moduleWidget, path);
				}));
			}
			else if (system::getExtension(path) == ".vcvs") {
				hasPresets = true;

				menu->addChild(createMenuItem(name, "", [=]() {
					if (!moduleWidget) {
						return;
					}
					try {
						FILE* file = std::fopen(path.c_str(), "r");
						if (!file) {
							throw Exception("Could not load selection file %s", path.c_str());
						}
						DEFER({std::fclose(file);});

						INFO("Loading selection %s", path.c_str());

						json_error_t error;
						json_t* rootJ = json_loadf(file, 0, &error);
						if (!rootJ) {
							throw Exception("File is not a valid selection file. JSON parsing error at %s %d:%d %s", error.source, error.line, error.column, error.text);
						}
						DEFER({json_decref(rootJ);});
						
						// iterate modules and recalc position
						json_t* modulesJ = json_object_get(rootJ, "modules");
						if (modulesJ) {
							size_t moduleIndex;
							int minX = 1000000, minY = 100000;
							json_t* moduleJ;
							json_array_foreach(modulesJ, moduleIndex, moduleJ) {
								// read min pos
								json_t* posJ = json_object_get(moduleJ, "pos");
								int x = 0, y = 0;
								json_unpack(posJ, "[i, i]", &x, &y);
								minX = std::min(minX, x);
								minY = std::min(minY, y);
							}
							minX *= -1;
							minX += (int) (moduleWidget->getPosition().x / RACK_GRID_WIDTH) - 2000;
							minY *= -1;
							minY += (int) (moduleWidget->getPosition().y / RACK_GRID_HEIGHT) - 100;
							json_array_foreach(modulesJ, moduleIndex, moduleJ) {
								// recalc pos
								json_t* posJ = json_object_get(moduleJ, "pos");
								int x = 0, y = 0;
								json_unpack(posJ, "[i, i]", &x, &y);
								x += minX;
								y += minY;
								posJ = json_pack("[i, i]", x, y);
								json_object_set(moduleJ, "pos", posJ);
							}
						}

						APP->scene->rack->pasteJsonAction(rootJ);
						// delete calling module if not connected
						/*if (moduleWidget->module) {
							bool hasConnections = false;
							for (auto& port: moduleWidget->module->inputs) {
								if (port.isConnected()) {
									hasConnections = true;
									break;
								}
							}
							if (!hasConnections) {	
								for (auto& port: moduleWidget->module->outputs) {
									if (port.isConnected()) {
										hasConnections = true;
										break;
									}
								}
							}
							if (!hasConnections) {	
								moduleWidget->requestDelete();
							}
						}*/
					} catch (Exception& e) {
						osdialog_message(OSDIALOG_WARNING, OSDIALOG_OK, e.what());
					}
				}));
			}
		}
	}
	if (!hasPresets) {
		menu->addChild(createMenuLabel("(None)"));
	}
};
