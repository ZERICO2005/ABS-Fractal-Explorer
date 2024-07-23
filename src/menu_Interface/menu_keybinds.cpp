#include "display_GUI_Internal.h"

#include "../copyBuffer.h"
#include "../imageBuffer.h"
#include "../user_data.h"
#include "../temp_global_render.h"
#include "../render.h"


#include <SDL2/SDL.h>
// #include <SDL2/SDL_keycode.h>
// #include <SDL2/SDL_scancode.h>

#include "../fileManager.h"
#include "../keybind.h"

template <typename fpX>
static void sen_slider(const char* label, fpX& num, fp32 min_bound, fp32 max_bound) {
	fp32 temp_sensitivity_float_slider = (fp32)num;
	ImGui::SliderFloat(
		label, &temp_sensitivity_float_slider,
		min_bound, max_bound, "%.2f"
	);
	num = (fp64)temp_sensitivity_float_slider;
}

void Menu_Keybinds() {
	static int Combo_keyboardSize = 0;
	static const char* keyboardSizeText[] = {
		"ANSI (Default)","Extended (Contains some FN keys)","Complete (All 242 SDL Scancodes)"
	};
	static bool displayNumpad = true;
	ImGui_DefaultWindowSize(
		config_data.GUI_Settings,
		(int32_t)Master.resX, ImGui_WINDOW_MARGIN * 2, 320, 480,
		(int32_t)Master.resY, ImGui_WINDOW_MARGIN * 2, 240, 360
	);
	ImGui::Begin("Keybinds Menu",&ShowTheXButton,ImGui_WINDOW_FLAGS);
	ImGui_BoundWindowPosition(config_data.GUI_Settings);
	ImGui::Checkbox("Lock key inputs in menus",&config_data.GUI_Settings.LockKeyInputsInMenus);
	ImGui::Text("Keyboard Type:");
	if (ImGui::Combo("##keyboardSize", &Combo_keyboardSize, BufAndLen(keyboardSizeText))) {
		
	}
	ImGui::Checkbox("Display Numberpad",&displayNumpad);
	if (Combo_keyboardSize != 0) {
		ImGui::Text("Note: Some Scancodes may or may not trigger on modern Keyboards");
	}
	ImGui::NewLine();
	{
		constexpr dim32_t kMaxResX = 1440;
		constexpr dim32_t kMinResX = 300;
		__attribute__((unused)) constexpr dim32_t kMinResY = 140;
		
		//static uint32_t kX = kMargin;
		//static uint32_t kY = 0;
		dim32_t kResX = (dim32_t)ImGui::GetContentRegionAvail().x;
		if (kResX < kMinResX) {
			kResX = kMinResX;
		} else if (kResX > kMaxResX) {
			kResX = kMaxResX;
		}
		static int32_t kCurX = -1;
		static int32_t kCurY = -1;
		static uint32_t clickState = 0x0;
		static BufferBox kBuf; // Gets set when the keyboard is rendered

		ImVec2 CursorPos = {
			ImGui::GetMousePos().x - ImGui::GetCursorScreenPos().x,
			ImGui::GetMousePos().y - ImGui::GetCursorScreenPos().y - 17 // Magic Correction Amount
		};
		clickState = SDL_GetMouseState(NULL,NULL);
		static SDL_Scancode keyHover = SDL_SCANCODE_UNKNOWN;
		static SDL_Scancode keyClick = SDL_SCANCODE_UNKNOWN;
		bool hoverInBounds = false;
		renderKeyboard(
			&kBuf, kResX, 5.75, 12.0,
			(uint8_t)Combo_keyboardSize, displayNumpad,
			kCurX, kCurY, ((clickState & 0x1) ? true : false), &keyHover, nullptr, &hoverInBounds
		);
		SDL_Surface* kSurface = SDL_CreateRGBSurfaceWithFormatFrom(
			kBuf.vram,
			(dim32_t)kBuf.resX, (dim32_t)kBuf.resY,
			(int32_t)(IMAGE_BUFFER_CHANNELS * 8),
			(int32_t)(IMAGE_BUFFER_CHANNELS * (size_t)kBuf.resX),
			SDL_PIXELFORMAT_ABGR8888
		);
		if (kSurface == nullptr) {
			fprintf(stderr, "Failed to create SDL surface: %s\n", SDL_GetError());
		}
		kTexture = SDL_CreateTextureFromSurface(renderer, kSurface);
		if (kTexture == nullptr) {
			fprintf(stderr, "Failed to create SDL texture: %s\n", SDL_GetError());
		}
		SDL_FreeSurface(kSurface);
		kCurX = (int32_t)CursorPos.x;
		kCurY = (int32_t)CursorPos.y;

		ImGui::Text("Hover: %s",Scancode_Name[keyHover]);
		if (clickState & 0x1 && hoverInBounds == true) {
			keyClick = keyHover;
		}
		ImGui::Image((void*)kTexture, ImVec2((fp32)kBuf.resX, (fp32)kBuf.resY));
		// ImGui::Text("size = %" PRId32 " x %" PRId32, kBuf.resX, kBuf.resY);
		// ImGui::Text("Cursor Position: %" PRId32 ",%" PRId32,kCurX,kCurY);
		ImGui::Text("Clicked Key: %s",Scancode_Name[keyClick]);
		size_t funcCount = 0;
		if (keyClick != SDL_SCANCODE_UNKNOWN) {
			using namespace Key_Function;
			for (const auto& bind : currentKBPreset->kList) {
				if (bind.key == keyClick) {
					funcCount++;
				}
			}
			if (funcCount > 0) {
				//ImGui::Text("Key Functions:");
				for (const auto& bind : currentKBPreset->kList) {
					if (bind.key == keyClick) {
						ImGui::Text("- %s",Key_Function_Text[bind.func]);
					}
				}
			} else {
				ImGui::Text("- Key not bound to any functions");
			}
		} else {
			ImGui::NewLine();
		}

		ImGui::Separator();

		static int Combo_function_Select = 0;
		// static int Combo_function_category_Select = 0;
		// ImGui::Text("Select Function Category:");
		// if (ImGui::Combo("##Combo_function_category_Select",&Combo_function_category_Select,
		// 	Key_Function::Key_Function_Category_Text, ARRAY_LENGTH(Key_Function::Key_Function_Category_Text)
		// )) {
			
		// }

		//if (Combo_function_category_Select != Key_Function::Category_NONE) {
			// size_t key_func_offset = Key_Function::Key_Function_Map[Combo_function_category_Select];
			// size_t key_func_previous = Key_Function::Key_Function_Map[Combo_function_category_Select - 1];
			// const char* key_func_start = Key_Function::Key_Function_Text[key_func_previous];
			// const char* key_func_end = Key_Function::Key_Function_Text[key_func_offset];
			// size_t key_func_length = key_func_end - key_func_start;
			// ImGui::Text("Select Function to bind:");
			// if (ImGui::Combo("##Combo_functionSelect",&Combo_function_Select,
			// 	key_func_start, key_func_length
			// )) {

			ImGui::Text("Select Function to bind:");
			if (ImGui::Combo("##Combo_functionSelect",&Combo_function_Select,
					Key_Function::Key_Function_Text, ARRAY_LENGTH(Key_Function::Key_Function_Text)
			)) {
				if (Combo_function_Select == Key_Function::Parameter_Function_Count) {
					Combo_function_Select = Key_Function::NONE;
				} else {
					for (size_t i = 0; i < ARRAY_LENGTH(Key_Function::Key_Function_Map); i++) {
						if (Combo_function_Select == (int_enum)Key_Function::Key_Function_Map[i]) {
							Combo_function_Select = Key_Function::NONE;
						}
					}
				}
			}
		//}

		ImVec4 bind_select_color = get_Theme_Highlight_Color();

		if (Combo_function_Select != Key_Function::NONE && keyClick != SDL_SCANCODE_UNKNOWN) {
			//ImGui::Text("Bind key %s to function %s",Scancode_Name[keyClick],Key_Function::Key_Function_Text[Combo_functionSelect]);

			ImGui::Text("Bind key "); ImGui::SameLine(0.0,1.0);
			ImGui::TextColored(bind_select_color,"%s",Scancode_Name[keyClick]); ImGui::SameLine(0.0,1.0);
			ImGui::Text(" to function "); ImGui::SameLine(0.0,1.0);
			ImGui::TextColored(bind_select_color,"%s",Key_Function::Key_Function_Text[Combo_function_Select]);

			if(ImGui::Button("Set Key-Bind")) {
				if (addKeyBind(&currentKBPreset->kList,(Key_Function::Key_Function_Enum)Combo_function_Select,keyClick) >= 0) {
					recolorKeyboard();
					Combo_function_Select = Key_Function::NONE;
					keyClick = SDL_SCANCODE_UNKNOWN;
				} else {
					printError("addKeyBind(%s,%s) failed",Scancode_Name[keyClick],Key_Function::Key_Function_Text[Combo_function_Select]);
				}
			}
			if (funcCount != 0) {
				ImGui::SameLine();
				if(ImGui::Button("Clear Key-bind")) {
					if (removeKeyBind(&currentKBPreset->kList,(Key_Function::Key_Function_Enum)Combo_function_Select,keyClick) >= 0) {
						recolorKeyboard();
						Combo_function_Select = Key_Function::NONE;
					} else {
						printError("removeKeyBind(%s,%s) failed",Scancode_Name[keyClick],Key_Function::Key_Function_Text[Combo_function_Select]);
					}
				}
			}
		} else {
			ImGui::Text("Click on a Key and Select a Function");
			ImGui::NewLine();
		}
		if (keyClick != SDL_SCANCODE_UNKNOWN) {
			if(ImGui::Button("Clear Key bindings")) {
				if (removeKeyBind(&currentKBPreset->kList,keyClick) >= 0) {
					recolorKeyboard();
					keyClick = SDL_SCANCODE_UNKNOWN;
				} else {
					printError("removeKeyBind(%s) failed",Scancode_Name[keyClick]);
				}
			}
			ImGui::SameLine();
			ImGui::Text("%s",Scancode_Name[keyClick]);
		} else {
			ImGui::Button("Click on a Key");
		}
		if (Combo_function_Select != Key_Function::NONE) {
			if(ImGui::Button("Clear Function bindings")) {
				if (removeKeyBind(&currentKBPreset->kList,(Key_Function::Key_Function_Enum)Combo_function_Select) >= 0) {
					recolorKeyboard();
				} else {
					printError("removeKeyBind(%s) failed",Key_Function::Key_Function_Text[Combo_function_Select]);
				}
			}
			ImGui::SameLine();
			ImGui::Text("%s",Key_Function::Key_Function_Text[Combo_function_Select]);
		} else {
			ImGui::Button("Select a Function");
		}
		ImGui::NewLine();
		if (ImGui::Button("Reset current key-bind to defaults")) {
			Combo_function_Select = Key_Function::NONE;
			keyClick = SDL_SCANCODE_UNKNOWN;
			currentKBPreset->kList = defaultKeyBind;
			recolorKeyboard();
		}
		ImGui::NewLine();

		ImGui::Separator();

		static char KeyBindName[80];
		memset(KeyBindName,'\0',ARRAY_LENGTH(KeyBindName));
		memcpy(KeyBindName,currentKBPreset->name.c_str(),TEXT_LENGTH(KeyBindName));
		if (currentKBPreset->kList.size() < 6) {
			ImGui::Text("Warning: The current Key-bind Preset has %" PRIu64 " key-binds, and may not be functional or practical.",currentKBPreset->kList.size());
			ImGui::Text("Current Key-bind Preset[%" PRId32 "]: ",get_currentKBPreset_Pos()); ImGui::SameLine(0.0,1.0);
			ImGui::TextColored({1.0,0.5,0.5,1.0},"%" PRIu64,currentKBPreset->kList.size()); ImGui::SameLine(0.0,1.0);
			ImGui::Text(" key-binds");
		} else {
			ImGui::Text("Current Key-bind Preset[%" PRId32 "]: %" PRIu64 " key-binds",get_currentKBPreset_Pos(),currentKBPreset->kList.size());
		}

		ImGui::InputText("##KeyBindName",BufAndLen(KeyBindName));
		currentKBPreset->name = KeyBindName;
		if (ImGui::BeginCombo("##Combo_KeyBind", "Choose a key-bind")) {
			for (auto iterKB = KeyBind_PresetList.begin(); iterKB != KeyBind_PresetList.end(); iterKB++) {
				bool is_selected = (currentKBPreset == &(*iterKB));
				if (ImGui::Selectable(iterKB->name.c_str(), is_selected)) {
					Combo_function_Select = Key_Function::NONE;
					keyClick = SDL_SCANCODE_UNKNOWN;
					currentKBPreset = &(*iterKB);
					recolorKeyboard();
				}
				// Set the initial focus on the currently selected item
				if (is_selected) {
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}
		ImGui::NewLine();
		static uint32_t name_count = 1;
		if (ImGui::Button("Create key-bind")) {
			Combo_function_Select = Key_Function::NONE;
			keyClick = SDL_SCANCODE_UNKNOWN;
			KeyBind_Preset temp_KeyBind;
			temp_KeyBind = *currentKBPreset;
			static char rand_name[324]; memset(rand_name,'\0',324);
			snprintf(rand_name,320,"KeyBind_%" PRIu32,name_count++);
			temp_KeyBind.name = rand_name;
			KeyBind_PresetList.push_back(temp_KeyBind);
			currentKBPreset = &KeyBind_PresetList.back();
			recolorKeyboard();
		}
		if (KeyBind_PresetList.size() > 1 && ImGui::Button("Remove current key-bind preset")) {
			Combo_function_Select = Key_Function::NONE;
			keyClick = SDL_SCANCODE_UNKNOWN;
			remove_currentKBPreset();
			recolorKeyboard();
		}
		if (ImGui::Button("Clear all key-binds")) {
			Combo_function_Select = Key_Function::NONE;
			keyClick = SDL_SCANCODE_UNKNOWN;
			clearKeyBind(&currentKBPreset->kList);
			recolorKeyboard();
		}

		ImGui::NewLine();
		#ifdef PLATFORM_WINDOWS
			if (ImGui::Button("Import Key-bind (.FracExpKB)")) {
				static char importKeyBindFile[324]; memset(importKeyBindFile,'\0',sizeof(importKeyBindFile));
				int openFileState = openFileInterface(
					importKeyBindFile,sizeof(importKeyBindFile),
					"Select a FracExpKB file",
					"KeyBind Files (*.fracExpKB)\0*.fracExpKB\0"\
					"FracExp Files (*.fracExp)\0*.fracExp\0"\
					"All Files (*.*)\0*.*\0"
				);
				if (openFileState == 0) {
					Combo_function_Select = Key_Function::NONE;
					keyClick = SDL_SCANCODE_UNKNOWN;
					import_KeyBindPresets(&KeyBind_PresetList,&currentKBPreset,importKeyBindFile);
					recolorKeyboard();
				}
			}
			if (ImGui::Button("Export Current Key-bind (.FracExpKB)")) {
				
					static char exportKeyBindFile[324]; memset(exportKeyBindFile,'\0',sizeof(exportKeyBindFile));
					int saveFileState = saveFileInterface(
						exportKeyBindFile,sizeof(exportKeyBindFile),"Save FracExpKB file",
						"KeyBind Files (*.fracExpKB)\0*.fracExpKB\0"\
						"FracExp Files (*.fracExp)\0*.fracExp\0"\
						"All Files (*.*)\0*.*\0",
						"fracExpKB",
						currentKBPreset->name.c_str()
					);
					if (saveFileState == 0) {
						KeyBind_Preset temp_KeyBind = *currentKBPreset;
						export_KeyBind(&temp_KeyBind,exportKeyBindFile);
					}
			}
			#ifndef BUILD_RELEASE
				if (ImGui::Button("Export All Key-binds (.FracExpKB)")) {
					static char exportKeyBindFile[324]; memset(exportKeyBindFile,'\0',324);
					int saveFileState = saveFileInterface(
						exportKeyBindFile,324,"Save FracExpKB file",
						"KeyBind Files (*.fracExpKB)\0*.fracExpKB\0"\
						"FracExp Files (*.fracExp)\0*.fracExp\0"\
						"All Files (*.*)\0*.*\0",
						"fracExpKB",
						currentKBPreset->name.c_str()
					);
					if (saveFileState == 0) {
						export_KeyBindPresets(&KeyBind_PresetList,exportKeyBindFile);
					}
				}
			#endif
		#else
			{
				static char importKeyBindFile[324] = "./KeyBind.fracExpKB";
				ImGui::Text("Import .fracExpKB Path:");
				ImGui::InputText("##ImportCurrentKey_bindfrompath",importKeyBindFile, sizeof(importKeyBindFile));
				if (ImGui::Button("Import Current Key-bind from path")) {
					Combo_function_Select = Key_Function::NONE;
					keyClick = SDL_SCANCODE_UNKNOWN;
					import_KeyBindPresets(&KeyBind_PresetList,&currentKBPreset,importKeyBindFile);
					recolorKeyboard();
				}
				ImGui::NewLine();
			}
			{
				static char exportKeyBindFile[324] = "./KeyBind.fracExpKB";
				ImGui::Text("Export .fracExpKB Path:");
				ImGui::InputText("##ExportCurrentKey_bindtopath",exportKeyBindFile, sizeof(exportKeyBindFile));
				if (ImGui::Button("Export Current Key-bind to path")) {
					KeyBind_Preset temp_KeyBind = *currentKBPreset;
					export_KeyBind(&temp_KeyBind,exportKeyBindFile);
				}
				ImGui::NewLine();
			}
			#ifndef BUILD_RELEASE
				{
					static char exportKeyBindFile[324] = "./All-KeyBinds.fracExpKB";
					ImGui::Text("File Path:");
					ImGui::InputText("##ExportAllCurrentKey_bindstopath",exportKeyBindFile, sizeof(exportKeyBindFile));
					if (ImGui::Button("Export All Key-binds to path")) {
						export_KeyBindPresets(&KeyBind_PresetList,exportKeyBindFile);
					}
				}
			#endif
		#endif

		// //	Disabling the name field since it can cause confusion when typing "./folder" + "KeyBind.fracExpKB" = "./folderKeyBind.fracExpKB"
		// // static char exportFracExpKBName[324] = "KeyBind";
		// static char exportFracExpKBDir[324] = "./KeyBind"; // "./"
		// ImGui::Text("Export current key-bind to directory:");
		// ImGui::InputText("##exportFracExpKBDir",exportFracExpKBDir,TEXT_LENGTH(exportFracExpKBDir));
		// // ImGui::Text("File Name:");
		// // ImGui::InputText("##exportFracExpKBName",exportFracExpKBName,TEXT_LENGTH(exportFracExpKBName));
		// static TimerBox exportTimer(1.0);
		// if (ImGui::Button("Export .FracExpKB File") && exportTimer.timerReset()) {
		// 	/* Move this file path code into fracExpKB.cpp */
		// 	const char* const exportFracExpKBExtension = ".fracExpKB";
		// 	size_t sizeOfPath = 0;
		// 	sizeOfPath += strnlen(exportFracExpKBExtension,sizeof(exportFracExpKBExtension)) * sizeof(exportFracExpKBExtension[0]);
		// 	sizeOfPath += strnlen(exportFracExpKBDir,sizeof(exportFracExpKBDir)) * sizeof(exportFracExpKBDir[0]);
		// 	// sizeOfPath += strnlen(exportFracExpKBName,sizeof(exportFracExpKBName)) * sizeof(exportFracExpKBName[0]);
		// 	char* exportFracExpKBPath = (char*)malloc(sizeOfPath);
		// 	if (exportFracExpKBPath != NULL) {
		// 		memset(exportFracExpKBPath,'\0',sizeOfPath);
		// 		strncat(exportFracExpKBPath,exportFracExpKBDir,sizeOfPath);
		// 		// strncat(exportFracExpKBPath,exportFracExpKBName,sizeOfPath);
		// 		strncat(exportFracExpKBPath,exportFracExpKBExtension,sizeOfPath);
		// 		if (write_FracExpKB_File(&currentKeyBind,exportFracExpKBPath) >= 0) {
		// 			printFlush("\n%s was exported successfully",exportFracExpKBPath);
		// 		} else {
		// 			printError("%s failed to export",exportFracExpKBPath);
		// 		}
		// 	} else {
		// 		printError("Unable to allocate memory to char* exportFracExpKBPath");
		// 	}
		// 	FREE(exportFracExpKBPath);
		// }
	}
	ImGui::NewLine();
	ImGui::Separator();
	ImGui::NewLine();
	ImGui::Text("Movement Sensitivity:");
	ImGui::NewLine();
	ImGui::Text("Global Sensitivity Multiplier:");

	User_Parameter_Sensitivity& config_sensitivity = config_data.Parameter_Sensitivity;

	sen_slider("##sen_global",config_sensitivity.global,0.4f,2.5f);
	if (ImGui::Button("Reset Sensitvity")) {
		default_Parameter_Sensitivity(config_sensitivity);
	}
	ImGui::NewLine();
	ImGui::Text("Coordinate:");
	sen_slider("##sen_coordinate",config_sensitivity.coordinate,0.4f,2.5f);
	ImGui::Text("Zoom:");
	sen_slider("##sen_zoom",config_sensitivity.zoom,0.4f,2.5f);
	ImGui::Checkbox("Invert Zoom",&config_sensitivity.invert_zoom);
	ImGui::Text("Maximum Iterations:");
	sen_slider("##sen_maxIter",config_sensitivity.maxIter,0.4f,2.5f);
	ImGui::Text("Z-Value/Julia:");
	sen_slider("##sen_julia",config_sensitivity.julia,0.4f,2.5f);
	ImGui::Text("Rotation:");
	sen_slider("##sen_rotation",config_sensitivity.rotation,0.4f,2.5f);
	ImGui::Text("Stretch:");
	sen_slider("##sen_stretch",config_sensitivity.stretch,0.4f,2.5f);
	ImGui::Text("Polar Power:");
	sen_slider("##sen_polar_power",config_sensitivity.polar_power,0.4f,2.5f);
	ImGui::Text("Breakout Value:");
	sen_slider("##sen_breakout_value",config_sensitivity.breakout_value,0.4f,2.5f);
	ImGui::End();
}