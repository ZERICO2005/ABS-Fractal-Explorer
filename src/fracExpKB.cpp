/*
**	Author: zerico2005 (2023)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#include "Common_Def.h"
#include "fracExp_Internal.h"
#include "fracExpKB.h"

/* Key-bind List Functions */
	// Compares Key-binds for sorting purposes
	bool compareKeyBind(const KeyBind& a, const KeyBind& b) {
		if (a.key != b.key) {
			return a.key < b.key;
		}
		return a.func < b.func;
	}
	bool equalsKeyBind(const KeyBind& a, const KeyBind& b) {
		return a.key == b.key && a.func == b.func;
	}
	// Resets a Key-bind to defaults
	int initDefaultKeyBind(std::list<KeyBind>* keyBindList) {
		if (keyBindList == nullptr) { return -1; }
		keyBindList->clear();
		*keyBindList = defaultKeyBind;
		return 0;
	}
	// Clears a Key-bind
	int clearKeyBind(std::list<KeyBind>* keyBindList) {
		if (keyBindList == nullptr) { return -1; }
		keyBindList->clear();
		return 0;
	}
	// Add a Key-bind
	int addKeyBind(std::list<KeyBind>* keyBindList, Key_Function::Key_Function_Enum func, SDL_Scancode key) {
		if (keyBindList == nullptr) { return -1; }
		if (func == Key_Function::NONE && key == SDL_SCANCODE_UNKNOWN) {
			return -1;
		}
		KeyBind k; k.func = func; k.key = key;
		// if (std::find(keyBindList->begin(),keyBindList->end(),k) != keyBindList->end()) {
		// 	return 1;
		// }
		keyBindList->push_back(k);
		return 0;
	}
	// Removes a specific Key-bind
	int removeKeyBind(std::list<KeyBind>* keyBindList, Key_Function::Key_Function_Enum func, SDL_Scancode key) {
		if (keyBindList == nullptr) { return -1; }
		keyBindList->remove_if([&func,&key](const KeyBind& kb) {
			return kb.func == func && kb.key == key;
		});
		return 0;
	}
	// Removes any Key-bind matching a Function
	int removeKeyBind(std::list<KeyBind>* keyBindList, Key_Function::Key_Function_Enum func) {
		if (keyBindList == nullptr) { return -1; }
		keyBindList->remove_if([&func](const KeyBind& kb) {
			return kb.func == func;
		});
		return 0;
	}
	// Removes any Key-bind matching a SDL_Scancode
	int removeKeyBind(std::list<KeyBind>* keyBindList, SDL_Scancode key) {
		if (keyBindList == nullptr) { return -1; }
		keyBindList->remove_if([&key](const KeyBind& kb) {
			return kb.key == key;
		});
		return 0;
	}
	// Removes duplicated Key-binds and any Key-binds with nothing set
	int cleanKeyBind(std::list<KeyBind>* keyBindList) {
		if (keyBindList == nullptr) { return -1; }
		keyBindList->remove_if([](const KeyBind& kb) {
			return kb.key == 0 && kb.func == 0;
		});
		keyBindList->sort(compareKeyBind);
		keyBindList->unique(equalsKeyBind);
		return 0;
	}

const char* const FracExpKB_WriteDescription= {
	"# FracExp_KeyBind file\n"
	"# The only parts of this file you SHOULD modify are the Username and the Keybind_List.\n"
	"# It is RECOMMENDED that your text editor is set to ANSI encoding and that exclusively ASCII characters are used.\n"
	"# Modifying any other parts of the file can lead to file corruption or errors."
};
const char* const FracExpKB_WriteUserInfoNotice= {
	"# The user must opt-in for this information to be saved"
};
const char* const FracExpKB_WriteSDL_ScancodeWebsite {
	"# https://wiki.libsdl.org/SDL2/SDL_Scancode"
};

int create_FracExpKB_Preset(FracExpKB_File* frac, char* fracExpKB_raw, Param_List* param_list, size_t param_len);

int create_FracExpKB_File(FracExpKB_File* frac, char* path) {
	if (frac == nullptr) { printError("Unable to create_FracExp_File since FracExpKB_File* frac is NULL"); return -1; }
	if (path == nullptr) { printError("Unable to create_FracExp_File since char* path is NULL"); return -1; }
	/* Setup */
		char *pathF;
		correctWindowsFileName(&pathF,path);
		if (pathF == NULL) {
			printError("Unable to format pathF, switching to path");
			pathF = path;
		}
		FILE *ptrF;
		ptrF = fopen(path, "rb");
		if (ptrF == NULL) {
			printError("ptrF Null, unable to open \"%s\"",path);
			return -1;
		}
		fseek(ptrF, 0, SEEK_END);
		size_t len = ftell(ptrF);
		fseek(ptrF, 0, SEEK_SET);
		char* fracExpKB_raw = (char*)calloc(len + 1,sizeof(char));
		if (fracExpKB_raw == NULL) {
			FREE(pathF);
			return -1;
		}
		fread(fracExpKB_raw, sizeof(char), len, ptrF); fracExpKB_raw[len] = '\0'; // Manual string termination
		fclose(ptrF);
		FREE(pathF);
	Param_List* param_list; size_t param_len = 0;
	generate_Param_List(fracExpKB_raw,len,&param_list,&param_len);
	static char fileExtension[64];
	//printFlush("\n\nRetriving Data\n");
	Param_List* item = NULL;
	#define integerFromParam(base) ((item != NULL) ? getNumberFromText(&fracExpKB_raw[item->pos], item->len,(base)) : 0)
	#define stringFromParam(buf) getTextFromParam(fracExpKB_raw, item, (buf), ARRAY_LENGTH(buf))
	#define getParam(data) getParameter(fracExpKB_raw,(char*)(data),param_list,param_len)
	#define getHash(data) copyHex(fracExpKB_raw,item,(data),4)
	
	/* File_Extentsion */
		item = getParam("File_Extension");
		stringFromParam(fileExtension);
	/* File_Header/FracExp_KeyBind/Version/ */
		item = getParam("File_Header/FracExp_KeyBind/Version/Major");
		integerFromParam(frac->FracExpKB_Version_Major);
		item = getParam("File_Header/FracExp_KeyBind/Version/Minor");
		integerFromParam(frac->FracExpKB_Version_Minor);
		item = getParam("File_Header/FracExp_KeyBind/Version/Patch");
		integerFromParam(frac->FracExpKB_Version_Patch);
		item = getParam("File_Header/FracExp_KeyBind/Version/Build");
		integerFromParam(frac->FracExpKB_Version_Build);
		item = getParam("File_Header/FracExp_KeyBind/Version/Tags");
		stringFromParam(frac->FracExpKB_Version_Tags);
		item = getParam("File_Header/FracExp_KeyBind/Version/Major");
		integerFromParam(frac->FracExpKB_Version_Major);
		item = getParam("File_Header/FracExp_KeyBind/Version/Minor");
		integerFromParam(frac->FracExpKB_Version_Minor);
		item = getParam("File_Header/FracExp_KeyBind/Version/Patch");
		integerFromParam(frac->FracExpKB_Version_Patch);
		item = getParam("File_Header/FracExp_KeyBind/Version/Build");
		integerFromParam(frac->FracExpKB_Version_Build);
		item = getParam("File_Header/FracExp_KeyBind/Version/Tags");
		stringFromParam(frac->FracExpKB_Version_Tags);
	/* File_Header/ABS_Fractal_Explorer/Version/ */
		item = getParam("File_Header/ABS_Fractal_Explorer/Version/Major");
		integerFromParam(frac->ABS_Fractal_Explorer_Version_Major);
		item = getParam("File_Header/ABS_Fractal_Explorer/Version/Minor");
		integerFromParam(frac->ABS_Fractal_Explorer_Version_Minor);
		item = getParam("File_Header/ABS_Fractal_Explorer/Version/Patch");
		integerFromParam(frac->ABS_Fractal_Explorer_Version_Patch);
		item = getParam("File_Header/ABS_Fractal_Explorer/Version/Build");
		integerFromParam(frac->ABS_Fractal_Explorer_Version_Build);
		item = getParam("File_Header/ABS_Fractal_Explorer/Version/Tags");
		stringFromParam(frac->ABS_Fractal_Explorer_Version_Tags);
		item = getParam("File_Header/ABS_Fractal_Explorer/Version/Major");
		integerFromParam(frac->ABS_Fractal_Explorer_Version_Major);
		item = getParam("File_Header/ABS_Fractal_Explorer/Version/Minor");
		integerFromParam(frac->ABS_Fractal_Explorer_Version_Minor);
		item = getParam("File_Header/ABS_Fractal_Explorer/Version/Patch");
		integerFromParam(frac->ABS_Fractal_Explorer_Version_Patch);
		item = getParam("File_Header/ABS_Fractal_Explorer/Version/Build");
		integerFromParam(frac->ABS_Fractal_Explorer_Version_Build);
		item = getParam("File_Header/ABS_Fractal_Explorer/Version/Tags");
		stringFromParam(frac->ABS_Fractal_Explorer_Version_Tags);
	/* File_Header/Hash/ */
		item = getParam("File_Header/Hash/All");
		getHash(frac->hash_all);
		item = getParam("File_Header/Hash/Critical");
		getHash(frac->hash_critical);
		item = getParam("File_Header/Hash/File");
		getHash(frac->hash_file);
		item = getParam("File_Header/Hash/Time");
		getHash(frac->hash_time);
		item = getParam("File_Header/Hash/KeyBinds");
		getHash(frac->hash_keybind);
	/* File_Information*/
		item = getParam("File_Information/Platform");
		stringFromParam(frac->File_Platform);
		// item = getParam("File_Information/Created");
		// stringFromParam(frac->File_Created);
		// item = getParam("File_Information/Modified");
		// stringFromParam(frac->File_Modified);
		// item = getParam("File_Information/Opened");
		// stringFromParam(frac->File_Opened);
		item = getParam("File_Information/Username");
		stringFromParam(frac->File_Username);
	/* KeyBind_List */
	if (create_FracExpKB_Preset(frac,fracExpKB_raw,param_list,param_len) != 0) {
		return -1;
	}
	if (frac->KeyBind_Preset_List.size() == 0) {
		return -2;
	}
	if (frac->KeyBind_Preset_List.size() == 0) {
		return -2;
	}
	FREE(fracExpKB_raw);
	return 0;
}

int create_FracExpKB_Preset(FracExpKB_File* frac, char* fracExpKB_raw, Param_List* param_list, size_t param_len) {
	if (frac == nullptr) { printError("FracExpKB_File* frac is NULL"); return -1; }
	if (fracExpKB_raw == nullptr) { printError("char* fracExpKB_raw is NULL"); return -1; }
	if (param_list == nullptr) { printError("Param_List* param_list is NULL"); return -1; }
	if (param_len == 0) { printError("param_len is 0"); return -1; }
	{ // Insert some sort of loop here
		Param_List* item = nullptr;
		
		#define Key_Function_Skip 0
		#define SDL_Scancode_Skip 0
		//printFlush("\n\nKey-Function: Scancode\n");
		using namespace Key_Function;
		
		//#define bindData(index) frac->KeyBind_Preset_List[0].list[index]
		KeyBind_Preset fPreset;
		item = getParam("KeyBind_List/Preset/Name");
		static char temp_fPreset_name[96]; memset(temp_fPreset_name,'\0',96);
		stringFromParam(temp_fPreset_name);
		fPreset.name = temp_fPreset_name;
		size_t bindCount = 0;

		for (size_t i = Key_Function_Skip; i < Parameter_Function_Count; i++) {
			size_t strSize = 0;
			char CurDir[128] = "KeyBind_List/Preset/";
			// 20 is a magic number for the end of CurDir
			snprintf(&CurDir[20],ARRAY_LENGTH(CurDir)-4,"%s",Key_Function_Text[i]);
			item = getParam(CurDir);
			if (item == nullptr) { continue; }

			size_t paramArrayLen = getParameterArrayLength(fracExpKB_raw,item,param_list,param_len);
			//printf("\n%s",Key_Function_Text[i]);
			for (size_t p = 0; p < paramArrayLen; p++) {
				char temp[64];
				for (size_t a = 0; a < ARRAY_LENGTH(temp); a++) { temp[a] = 0x0; }
				strSize = getTextFromParam(fracExpKB_raw, item + p, temp, ARRAY_LENGTH(temp));
				if (strSize > 0) {
					for (size_t match = SDL_Scancode_Skip; match < ARRAY_LENGTH(Scancode_Name); match++) {
						if (strictCompareText(temp,strlen(temp),(char*)Scancode_Name[match],strlen((char*)Scancode_Name[match])) == true) {
							KeyBind bind;
							bind.func = (Key_Function_Enum)i;
							bind.key = (SDL_Scancode)match;
							fPreset.kList.push_back(bind);
							bindCount++;
							break;
						}
					}
				}
			}
		}
		for (size_t i = SDL_Scancode_Skip; i < ARRAY_LENGTH(Scancode_Name); i++) {
			size_t strSize = 0;
			char CurDir[128] = "KeyBind_List/Preset/";
			// 20 is a magic number for the end of CurDir
			snprintf(&CurDir[20],ARRAY_LENGTH(CurDir)-4,"%s",Scancode_Name[i]);
			item = getParam(CurDir);
			if (item == nullptr) { continue; }

			size_t paramArrayLen = getParameterArrayLength(fracExpKB_raw,item,param_list,param_len);
			//printf("\n%s",Key_Function_Text[i]);
			for (size_t p = 0; p < paramArrayLen; p++) {
				char temp[64];
				for (size_t a = 0; a < ARRAY_LENGTH(temp); a++) { temp[a] = 0x0; }
				strSize = getTextFromParam(fracExpKB_raw, item + p, temp, ARRAY_LENGTH(temp));
				if (strSize > 0) {
					for (size_t match = Key_Function_Skip; match < ARRAY_LENGTH(Key_Function_Text); match++) {
						if (strictCompareText(temp,strlen(temp),(char*)Key_Function_Text[match],strlen((char*)Key_Function_Text[match])) == true) {
							KeyBind bind;
							bind.func = (Key_Function_Enum)match;
							bind.key = (SDL_Scancode)i;
							fPreset.kList.push_back(bind);
							bindCount++;
							break;
						}
					}
				}
			}
		}
		if (bindCount != 0) {
			frac->KeyBind_Preset_List.push_back(fPreset);
		}
	}
	return 0;
}

/* Import/Read FracExpKB */

/* Imports KeyBind_Presets and updates the current preset */
int import_KeyBindPresets(std::list<KeyBind_Preset>* presetList, KeyBind_Preset** preset, char* path) {
	if (presetList == nullptr) {
		printError("std::list<KeyBind_Preset>* presetList is NULL");
		return -1;
	}
	if (preset == nullptr) {
		printWarning("KeyBind_PRESET** preset is NULL");
		return -1;
	}
	if (path == nullptr) {
		printWarning("Unable to read_FracExp_File due to NULL path");
		return -1;
	}
	FracExpKB_File frac;
	int ret = create_FracExpKB_File(&frac,path);
	if (ret != 0) {
		while (frac.KeyBind_Preset_List.empty() == false) {
			frac.KeyBind_Preset_List.back().kList.clear();
			frac.KeyBind_Preset_List.pop_back();
		}
		printError("read_FracExpKB_File() returned %d",ret);
		return ret;
	}
	if (frac.KeyBind_Preset_List.empty() == true) {
		printError("FracExpKB file is empty: %s",path);
		return -1;
	}
	size_t addedPresetCount = 0;
	while (frac.KeyBind_Preset_List.empty() == false) {
		addedPresetCount++;
		KeyBind_Preset* fPreset = &frac.KeyBind_Preset_List.front();
		
		fPreset->name.erase(std::remove(fPreset->name.begin(), fPreset->name.end(), '\"'), fPreset->name.end()); // Temporary
		bool uniqueName = true;
		do {
			uniqueName = true;
			for (const auto& checkPreset : *presetList) {
				if (checkPreset.name == fPreset->name) {
					uniqueName = false;
					char rand_num = (uint8_t)((getNanoTime() / 1000) % 10) + '0';
					fPreset->name += rand_num;
					break;
				}
			}
		} while (uniqueName == false);

		presetList->push_back(*fPreset);
		frac.KeyBind_Preset_List.pop_front();

		if (addedPresetCount == 1) {
			*preset = &presetList->back();
		}
	};
	return 0;
}

/* Export/Write FracExpKB */

int write_FracExpKB_Preset(const KeyBind_Preset* preset, FILE *ptrW) {
	if (preset == nullptr) { printError("KeyBind_PRESET* preset is NULL"); return -1; }
	if (ptrW == nullptr) { printError("FILE *ptrW is NULL"); return -1; }
	/* Preset Header */
	fprintf(ptrW,"\n\tPreset:");
	if (preset->name.empty() == true) {
		fprintf(ptrW,"\n\t\tName:\n\t\t\t\"KeyBind_%012llu\"",(getNanoTime() / 1000) % 1000000000000);
	}
	fprintf(ptrW,"\n\t\tName:\n\t\t\t\"%s\"",preset->name.c_str());
	/* Print KeyBinds */
	for (const auto& bind : preset->kList) {
		fprintf(ptrW,"\n\t\t%s:",Key_Function::Key_Function_Text[bind.func]);
		for (const auto& match : preset->kList) {
			if (match.func == bind.func) {
				fprintf(ptrW,"\n\t\t\t%s",Scancode_Name[match.key]);
			}
		}
	}
	return 0;
}

int write_FracExpKB_Header(FILE *ptrW) {
	if (ptrW == nullptr) { printError("FILE *ptrW is NULL"); return -1; }
	fprintf(ptrW,"%s",FracExpKB_WriteDescription);
	fprintf(ptrW,"\n\nFile_Extension:\n\t\".fracExpKB\"");
	fprintf(ptrW,"\n\nFile_Header:");
	fprintf(
		ptrW,"\n\tFracExp_KeyBind:\n\t\tVersion:\
		\n\t\t\tMajor:\n\t\t\t\t%u\n\t\t\tMinor:\n\t\t\t\t%u\n\t\t\tPatch:\n\t\t\t\t%u\n\t\t\tBuild:\n\t\t\t\t%u\n\t\t\tTags:\n\t\t\t\t\"%s\"",
		FRACEXPKB_V_MAJOR,FRACEXPKB_V_MINOR,FRACEXPKB_V_PATCH,0,FRACEXPKB_V_TAG
	);
	fprintf(
		ptrW,"\n\tFracExp_KeyBind:\n\t\tVersion:\
		\n\t\t\tMajor:\n\t\t\t\t%u\n\t\t\tMinor:\n\t\t\t\t%u\n\t\t\tPatch:\n\t\t\t\t%u\n\t\t\tBuild:\n\t\t\t\t%u\n\t\t\tTags:\n\t\t\t\t\"%s\"",
		PROGRAM_V_MAJOR,PROGRAM_V_MINOR,PROGRAM_V_PATCH,0,PROGRAM_V_TAG
	);
	fprintf(ptrW,"\n\tHash:");
	fprintf(ptrW,"\n\t\tAll:\n\t\t\t0x%016Xx%016Xx%016Xx%016X",0,0,0,0);
	fprintf(ptrW,"\n\t\tCritical:\n\t\t\t0x%016Xx%016Xx%016Xx%016X",0,0,0,0);
	fprintf(ptrW,"\n\t\tFile:\n\t\t\t0x%016Xx%016Xx%016Xx%016X",0,0,0,0);
	fprintf(ptrW,"\n\t\tKeyBinds:\n\t\t\t0x%016Xx%016Xx%016Xx%016X",0,0,0,0);
	fprintf(ptrW,"\n\nFile_Information:");
	fprintf(ptrW,"\n\tPlatform:\n\t\t\"%s\"","Unknown");
	fprintf(ptrW,"\n\tCreated:\n\t\t\"%s\"","2000/01/01 00:00");
	fprintf(ptrW,"\n\tModified\n\t\t\"%s\"","2000/01/01 00:00");
	fprintf(ptrW,"\n\tOpened:\n\t\t\"%s\"","2000/01/01 00:00");
	fprintf(ptrW,"\n\tUsername: %s\n\t\t\"%s\"",FracExpKB_WriteUserInfoNotice,"Unknown");
	fprintf(ptrW,"\n\nKeyBind_List: %s",FracExpKB_WriteSDL_ScancodeWebsite);
	return 0;
}

/* Exports multiple KeyBind_Presets */
int export_KeyBindPresets(const std::list<KeyBind_Preset>* presetList, char* path) {
	if (presetList == nullptr) { printError("std::list<KeyBind_PRESET>* presetList is NULL"); return -1; }
	if (path == nullptr) { printError("export_KeyBindPresets() failed due to NULL path"); return -1; }
	char *pathW;
	correctWindowsFileName(&pathW,path);
	if (pathW == nullptr) {
		printWarning("Unable to format pathW, switching to path");
		pathW = path;
	}
	FILE *ptrW;
	ptrW = fopen(path, "wb");
	if (ptrW == nullptr) {
		printError("Unable to open %s",path);
		FREE(pathW);
		return -1;
	}
	/* Write Data */
	write_FracExpKB_Header(ptrW);
	for (const auto& preset : *presetList) {
		write_FracExpKB_Preset(&preset,ptrW);
	}
	if (fclose(ptrW) != 0) {
		printError("\nFailed to close file: %s",pathW);
		return -1;
	}
	return 0;
}

/* Exports a singular KeyBind_Preset */
int export_KeyBind(const KeyBind_Preset* preset, char* path) {
	if (preset == nullptr) { printError("KeyBind_Preset* preset is NULL"); return -1; }
	if (path == nullptr) { printError("export_KeyBind() failed due to NULL path"); return -1; }
	char *pathW;
	correctWindowsFileName(&pathW,path);
	if (pathW == nullptr) {
		printWarning("Unable to format pathW, switching to path");
		pathW = path;
	}
	FILE *ptrW;
	ptrW = fopen(path, "wb");
	if (ptrW == nullptr) {
		printError("Unable to open %s",path);
		FREE(pathW);
		return -1;
	}
	/* Write Data */
	write_FracExpKB_Header(ptrW);
	write_FracExpKB_Preset(preset,ptrW);
	if (fclose(ptrW) != 0) {
		printError("\nFailed to close file: %s",pathW);
		return -1;
	}
	return 0;
}