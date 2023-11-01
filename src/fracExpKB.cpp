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
		return;
	}
	fread(fracExpKB_raw, 1, len, ptrF); fracExpKB_raw[len] = '\0'; // Manual string termination
	fclose(ptrF);
	FREE(pathF);
	
	Param_List* param_list; size_t param_len = 0;
	generate_Param_List(fracExpKB_raw,len,&param_list,&param_len);
	static char fileExtension[64];

	printFlush("\n\nRetriving Data\n");
	Param_List* item = NULL;
	#define integerFromParam(base) ((item != NULL) ? getNumberFromText(&fracExpKB_raw[item->pos], item->len,(base)) : 0)
	#define stringFromParam(buf) getTextFromParam(fracExpKB_raw, item, (buf), ARRAY_LENGTH(buf));
	#define getParam(data) getParameter(fracExpKB_raw,(char*)(data),param_list,param_len)
	#define getHash(data) copyHex(fracExpKB_raw,item,(data),4);
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
		using namespace Key_Function;
		item = getParam("KeyBind_List/Preset/Name");
		stringFromParam(frac->KeyBind_PresetList.name);
		
		for (size_t i = 0; i < Parameter_Function_Count; i++) {
			char CurDir[128] = "KeyBind_List/Preset/";
			// 20 is a magic number for the end of CurDir
			snprintf(&CurDir[20],ARRAY_LENGTH(CurDir)-4,"%s",Key_Function_Text[i]);
			item = getParam(CurDir);
			char temp[64];
			for (size_t a = 0; a < ARRAY_LENGTH(temp); a++) { temp[a] = 0x0; }
			stringFromParam(temp);
			printFlush("\n%s",temp);
		}
	}
	FREE(fracExpKB_raw);
	//FREE(param_list);
	printFlush("\n");
		
	//size_t param_start;  size_t param_end;
}

int read_FracExpKB_File(KeyBind_Preset* preset, char* path) {
	if (path == NULL) {
		printError("Unable to read_FracExp_File due to NULL path");
		return -1;
	}
	FracExpKB_File* frac = (FracExpKB_File*)malloc(sizeof(FracExpKB_File));
	create_FracExpKB_File(frac,path);
	FREE(frac);
	return 0;
}

int write_FracExpKB_File(KeyBind_Preset* preset, char* path) {
	if (preset == NULL || path == NULL) {
		printError("Unable to write_FracExp_File due to NULL parameters");
		return -1;
	}
	return 0;
}





