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

void create_FracExpKB_File(FracExpKB_File* frac, char* path) {
	if (path == NULL) {
		printError("Unable to create_FracExp_File due to NULL parameters");
		return; 
	}
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
		printError("Unable to open %s",path);
		return;
	}
	fseek(ptrF, 0, SEEK_END);
	size_t len = ftell(ptrF);
	fseek(ptrF, 0, SEEK_SET);
	char* fracExpKB_raw = (char*)malloc(len + 1);
	if (fracExpKB_raw == NULL) {
		printError("Unable to allocate memory for fracExpKB_raw");
		FREE(pathF);
		return;
	}
	fread(fracExpKB_raw, 1, len, ptrF); fracExpKB_raw[len] = '\0'; // Manual string termination
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
		getHash(frac->hash_all);
		item = getParam("File_Header/Hash/File");
		getHash(frac->hash_all);
		item = getParam("File_Header/Hash/Time");
		getHash(frac->hash_all);
		item = getParam("File_Header/Hash/KeyBinds");
		getHash(frac->hash_all);
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
	{
		//printFlush("\n\nKey-Function: Scancode\n");
		using namespace Key_Function;
		size_t totalSize = 0;
		for (size_t i = Key_Function::NONE + 1; i < Parameter_Function_Count; i++) {
			size_t strSize = 0;
			char CurDir[128] = "KeyBind_List/Preset/";
			// 20 is a magic number for the end of CurDir
			snprintf(&CurDir[20],ARRAY_LENGTH(CurDir)-4,"%s",Key_Function_Text[i]);
			item = getParam(CurDir);
			if (item == NULL) { continue; }

			size_t paramArrayLen = getParameterArrayLength(fracExpKB_raw,item,param_list,param_len);
			//printf("\n%s",Key_Function_Text[i]);
			for (size_t p = 0; p < paramArrayLen; p++) {
				char temp[64];
				for (size_t a = 0; a < ARRAY_LENGTH(temp); a++) { temp[a] = 0x0; }
				strSize = getTextFromParam(fracExpKB_raw, item + p, temp, ARRAY_LENGTH(temp));
				if (strSize > 0) {
					//printf("\n\t%s",temp);
					totalSize++;
				}
			}
		}
		//printFlush("\n\nScancode: Key-Function\n");
		for (size_t i = SDL_SCANCODE_UNKNOWN + 1; i < ARRAY_LENGTH(Scancode_Name); i++) {
			size_t strSize = 0;
			char CurDir[128] = "KeyBind_List/Preset/";
			// 20 is a magic number for the end of CurDir
			snprintf(&CurDir[20],ARRAY_LENGTH(CurDir)-4,"%s",Scancode_Name[i]);
			item = getParam(CurDir);
			if (item == NULL) { continue; }

			size_t paramArrayLen = getParameterArrayLength(fracExpKB_raw,item,param_list,param_len);
			//printf("\n%s\n",Scancode_Name[i]);
			for (size_t p = 0; p < paramArrayLen; p++) {
				char temp[64];
				for (size_t a = 0; a < ARRAY_LENGTH(temp); a++) { temp[a] = 0x0; }
				strSize = getTextFromParam(fracExpKB_raw, item + p, temp, ARRAY_LENGTH(temp));
				if (strSize > 0) {
					//printf("\n\t%s",temp);
					totalSize++;
				}
			}
		}
		//printFlush("\n\nTotal Size: %zu",totalSize);

		/*
		item = getParam("KeyBind_List/Preset/Name");
		stringFromParam(frac->KeyBind_Preset_List[0].name);
		item = getParam("KeyBind_List/Preset/Hash");
		*/

		frac->KeyBind_Preset_Count = 1;
		frac->KeyBind_Preset_List = (KeyBind_Preset*)malloc(sizeof(KeyBind_Preset) * frac->KeyBind_Preset_Count);
		snprintf(frac->KeyBind_Preset_List[0].name,TEXT_LENGTH(frac->KeyBind_Preset_List[0].name),"Imported Key-bind");
		getHash((uint64_t*)(frac->KeyBind_Hash) + (0 * 4));
		
		frac->KeyBind_Preset_List[0].list = (KeyBind*)malloc(sizeof(KeyBind) * totalSize);
		frac->KeyBind_Preset_List[0].length = totalSize;
		#define bindData(index) frac->KeyBind_Preset_List[0].list[index]
		size_t bindPtr = 0;


		for (size_t i = Key_Function::NONE + 1; i < Parameter_Function_Count; i++) {
			size_t strSize = 0;
			char CurDir[128] = "KeyBind_List/Preset/";
			// 20 is a magic number for the end of CurDir
			snprintf(&CurDir[20],ARRAY_LENGTH(CurDir)-4,"%s",Key_Function_Text[i]);
			item = getParam(CurDir);
			if (item == NULL) { continue; }

			size_t paramArrayLen = getParameterArrayLength(fracExpKB_raw,item,param_list,param_len);
			//printf("\n%s",Key_Function_Text[i]);
			for (size_t p = 0; p < paramArrayLen; p++) {
				char temp[64];
				for (size_t a = 0; a < ARRAY_LENGTH(temp); a++) { temp[a] = 0x0; }
				strSize = getTextFromParam(fracExpKB_raw, item + p, temp, ARRAY_LENGTH(temp));
				if (strSize > 0) {
					for (size_t match = SDL_SCANCODE_UNKNOWN + 1; match < ARRAY_LENGTH(Scancode_Name); match++) {
						if (strictCompareText(temp,strlen(temp),(char*)Scancode_Name[match],strlen((char*)Scancode_Name[match])) == true) {
							bindData(bindPtr).func = (Key_Function_Enum)i;
							bindData(bindPtr).key = (SDL_Scancode)match;
							bindPtr++;
							break;
						}
					}
				}
			}
		}
		for (size_t i = SDL_SCANCODE_UNKNOWN + 1; i < ARRAY_LENGTH(Scancode_Name); i++) {
			size_t strSize = 0;
			char CurDir[128] = "KeyBind_List/Preset/";
			// 20 is a magic number for the end of CurDir
			snprintf(&CurDir[20],ARRAY_LENGTH(CurDir)-4,"%s",Scancode_Name[i]);
			item = getParam(CurDir);
			if (item == NULL) { continue; }

			size_t paramArrayLen = getParameterArrayLength(fracExpKB_raw,item,param_list,param_len);
			//printf("\n%s",Key_Function_Text[i]);
			for (size_t p = 0; p < paramArrayLen; p++) {
				char temp[64];
				for (size_t a = 0; a < ARRAY_LENGTH(temp); a++) { temp[a] = 0x0; }
				strSize = getTextFromParam(fracExpKB_raw, item + p, temp, ARRAY_LENGTH(temp));
				if (strSize > 0) {
					for (size_t match = Key_Function::NONE + 1; match < ARRAY_LENGTH(Key_Function_Text); match++) {
						if (strictCompareText(temp,strlen(temp),(char*)Key_Function_Text[match],strlen((char*)Key_Function_Text[match])) == true) {
							bindData(bindPtr).func = (Key_Function_Enum)match;
							bindData(bindPtr).key = (SDL_Scancode)i;
							bindPtr++;
							break;
						}
					}
				}
			}
		}
	}
	FREE(fracExpKB_raw);
	//FREE(param_list);
	//printFlush("\n");
		
	//size_t param_start;  size_t param_end;
}

int read_FracExpKB_File(KeyBind_Preset* preset, char* path) {
	if (preset == NULL) {
		printError("KeyBind_Preset* preset is NULL");
		return -1;
	}
	if (path == NULL) {
		printWarning("Unable to read_FracExp_File due to NULL path");
		return -1;
	}
	FracExpKB_File* frac = (FracExpKB_File*)malloc(sizeof(FracExpKB_File));
	if (frac == NULL) {
		printError("Unable to allocate memory for FracExpKB_File* frac");
	}
	frac->KeyBind_Preset_Count = 0;
	create_FracExpKB_File(frac,path);

	if (frac->KeyBind_Preset_List == NULL) {
		FREE(frac);
		printError("FracExpKB_File* frac->KeyBind_Preset_List is NULL");
		return -1;
	}
	// preset = (KeyBind_Preset*)malloc(sizeof(KeyBind_Preset));
	preset->length = frac->KeyBind_Preset_List[0].length; // Length
	
	preset->list = (KeyBind*)malloc(sizeof(KeyBind) * preset->length); // List
	snprintf(preset->name,TEXT_LENGTH(preset->name),"%s",frac->KeyBind_Preset_List[0].name); // Name
	for (size_t i = 0; i < preset->length; i++) {
		preset->list[i] = frac->KeyBind_Preset_List[0].list[i];
	}
	//printf("\nLength: %zu List: %p Name: %s",preset->length,preset->list,preset->name);
	for (size_t i = 0; i < frac->KeyBind_Preset_Count; i++) {
		FREE(frac->KeyBind_Preset_List[i].list);
	}
	FREE(frac->KeyBind_Preset_List);
	FREE(frac->KeyBind_Hash);
	FREE(frac);
	// printFlush("\nPrinting: ");
	// printFlush("%s\n",preset->name);
	// for (size_t i = 0; i < preset->length; i++) {
	// 	printf("\nKeyBind:\n\t%s\n\t%s",Key_Function::Key_Function_Text[preset->list[i].func],Scancode_Name[preset->list[i].key]);
	// }
	// fflush(stdout);
	return 0;
}

int write_FracExpKB_File(KeyBind_Preset* preset, char* path) {
	if (preset == NULL) {
		printError("KeyBind_Preset* preset is NULL");
		return -1;
	}
	if (path == NULL) {
		printError("Unable to read_FracExp_File due to NULL path");
		return -1;
	}

	/* Setup */
	char *pathW;
	correctWindowsFileName(&pathW,path);
	if (pathW == NULL) {
		printWarning("Unable to format pathF, switching to path");
		pathW = path;
	}
	FILE *ptrW;
	ptrW = fopen(path, "wb");
	if (ptrW == NULL) {
		printError("Unable to open %s",path);
		FREE(pathW);
		return -1;
	}
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
	fprintf(ptrW,"\n\nKeyBind_List: %s\n\tPreset:\n\t\tName:\n\t\t\t\"%s\"",FracExpKB_WriteSDL_ScancodeWebsite,preset->name);
	for (size_t i = 0; i < preset->length; i++) {
		//fprintf(ptrW,"\n\t\t%s:\n\t\t\t%s",Key_Function::Key_Function_Text[preset->list[i].func],Scancode_Name[preset->list[i].key]);
		bool duplicateFunc = false;
		for (size_t a = 0; a < i; a++) {
			if (preset->list[a].func == preset->list[i].func) {
				duplicateFunc = true;
				break;
			}
		}
		if (duplicateFunc == false) {
			fprintf(ptrW,"\n\t\t%s:",Key_Function::Key_Function_Text[preset->list[i].func]);
			for (size_t f = i; f < preset->length; f++) {
				if (preset->list[f].func == preset->list[i].func) {
					fprintf(ptrW,"\n\t\t\t%s",Scancode_Name[preset->list[f].key]);
				}
			}
		}
		
	}
	fclose(ptrW);
	FREE(pathW);
	return 0;
}