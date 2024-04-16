/*
**	Author: zerico2005 (2023-2024)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#include "Common_Def.h"
#include "Program_Def.h"
#include "user_data.h"

/* Headers containing enums for default values */
	#include "display_GUI.h"
	#include "render.h"
	#include "displayInfo.h"

constexpr User_Configuration_Data Default_Config = {
	.Automatic_Behaviour = {
		.AutoLoad_Config_File = true,
		.AutoSave_Config_File = true
	},
	.Parameter_Sensitivity = {
		.global = 1.0,
		.coordinate = 1.0,
		.zoom = 1.0,
		.invert_zoom = false,
		.maxIter = 1.0,
		.julia = 1.0,
		.rotation = 1.0,
		.stretch = 1.0,
		.polar_power = 1.0,
		.breakout_value = 1.0
	},
	.Display_Preferences = {
		.Display_Config_Hash = 0x0,
		.Display_Bootup_Type = Display_Bootup::Automatic,
		.Specific_Bootup_Display = 1,
		.Previous_Display_Used = 0,
		.Bootup_Fullscreen = Display_Fullscreen::Windowed,
		.ScaleWindowToScreenSize = false,
		.Bootup_Window_Scale = 0.7,
		.Display_RefreshRate_Type = Display_RefreshRate::Automatic,
		.Automatically_Update_RefreshRate = true,
		.Constant_RefreshRate_Value = 60.0,
		.Maximum_FPS_Multiplier = (1) - 1
	},
	.GUI_Settings = {
		.LockKeyInputsInMenus = true,
		.AutoResizeWindows = false,
		.PreventOutOfBoundsWindows = false,
		.GUI_Theme = Display_GUI::IMGUI_Theme_Dark,
		.WindowOpacity = 0.95f,
		.WindowAutoScale = 0.7
	},
	.Screenshot_Settings = {
		.screenshotFileType = Image_File_Format::PNG,
		.PNG_Compression_Level = 8,
		.JPG_Quality_Level = 95
	},
	.Rendering_Settings = {
		.Hardware_Hash = 0x0,
		.Frame_Interpolation_Method = OPENCV_Interpolation::OPENCV_INTER_NEAREST,
		.Image_Render_Bounding_Box = Namespace_Image_Render_Bounding_Box::Fill_Area,
		.JuliaPoint_Enabled = true,
		.JuliaPoint_OuterRadius = 8.0f,
		.JuliaPoint_InnerRadius = 2.4f
	}
};



/* clean_config_data */
	#define clean_config_data(value,min,max)\
	valueRestore(config_data.value,config_default.value,(min),(max))

	void clean_Automatic_Behaviour(User_Automatic_Behaviour& config_data) {
		const User_Automatic_Behaviour& config_default = Default_Config.Automatic_Behaviour;
		return; // Automatic_Behaviour only has bools
	}

	void clean_Parameter_Sensitivity(User_Parameter_Sensitivity& config_data) {
		const User_Parameter_Sensitivity& config_default = Default_Config.Parameter_Sensitivity;
		clean_config_data(global,         0.1, 10.0);
		clean_config_data(coordinate,     0.1, 10.0);
		clean_config_data(zoom,           0.1, 10.0);
		clean_config_data(maxIter,        0.1, 10.0);
		clean_config_data(julia,          0.1, 10.0);
		clean_config_data(rotation,       0.1, 10.0);
		clean_config_data(stretch,        0.1, 10.0);
		clean_config_data(polar_power,    0.1, 10.0);
		clean_config_data(breakout_value, 0.1, 10.0);
	}

	void clean_Display_Preferences(User_Display_Preferences& config_data) {
		const User_Display_Preferences& config_default = Default_Config.Display_Preferences;
		// display_config_hash doesn't need to be cleaned
		clean_config_data(Display_Bootup_Type, 0, Display_Bootup::Length - 1);
		clean_config_data(Specific_Bootup_Display, 1, 144); // Does anyone even have 144 displays? Probably not.
		clean_config_data(Bootup_Window_Scale, 0.01, 1.0);
		clean_config_data(Bootup_Fullscreen, 0, Display_Fullscreen::Length - 1);
		clean_config_data(Display_RefreshRate_Type, 0, Display_RefreshRate::Length - 1);
		clean_config_data(Constant_RefreshRate_Value, 12.0, 1200.0);
		clean_config_data(Maximum_FPS_Multiplier, (-6) + 1, (6) - 1);
	}

	void clean_GUI_Settings(User_GUI_Settings& config_data) {
		const User_GUI_Settings& config_default = Default_Config.GUI_Settings;
		clean_config_data(GUI_Theme, 0, Display_GUI::IMGUI_Theme_Count - 1);
		clean_config_data(WindowOpacity, 0.2, 1.0);
		clean_config_data(WindowAutoScale, 0.1, 1.0);
	}

	void clean_Screenshot_Settings(User_Screenshot_Settings& config_data) {
		const User_Screenshot_Settings& config_default = Default_Config.Screenshot_Settings;
		clean_config_data(screenshotFileType, 0, Image_File_Format::Image_File_Format_Count - 1);
		clean_config_data(PNG_Compression_Level, 1, 9);
		clean_config_data(JPG_Quality_Level, 20, 100);
	}

	void clean_Rendering_Settings(User_Rendering_Settings& config_data) {
		const User_Rendering_Settings& config_default = Default_Config.Rendering_Settings;
		clean_config_data(Frame_Interpolation_Method, 0, OPENCV_Interpolation::OPENCV_INTER_COUNT - 1);
		clean_config_data(Image_Render_Bounding_Box, 0, Namespace_Image_Render_Bounding_Box::Image_Render_Bounding_Box_Count - 1);
		clean_config_data(JuliaPoint_OuterRadius, 1.0f, 24.0f);
		clean_config_data(JuliaPoint_InnerRadius, 0.0f, config_data.JuliaPoint_OuterRadius - 1.0f);
	}

	void clean_User_Configuration_Data(User_Configuration_Data& config_data) {
		clean_Automatic_Behaviour(config_data.Automatic_Behaviour);
		clean_Parameter_Sensitivity(config_data.Parameter_Sensitivity);
		clean_Display_Preferences(config_data.Display_Preferences);
		clean_GUI_Settings(config_data.GUI_Settings);
		clean_Screenshot_Settings(config_data.Screenshot_Settings);
		clean_Rendering_Settings(config_data.Rendering_Settings);
	}
	#undef clean_config_data

/* default_config_data */

	void default_Automatic_Behaviour(User_Automatic_Behaviour& config_data) { config_data = Default_Config.Automatic_Behaviour; }
	void default_Parameter_Sensitivity(User_Parameter_Sensitivity& config_data, bool reset_Invert) {
		bool invertZoom = config_data.invert_zoom;
		config_data = Default_Config.Parameter_Sensitivity;
		if (reset_Invert == false) {
			config_data.invert_zoom = invertZoom; // Preserves this setting
		}
	}
	void default_Display_Preferences(User_Display_Preferences& config_data) { config_data = Default_Config.Display_Preferences; }
	void default_GUI_Settings(User_GUI_Settings& config_data, bool reset_GUI_Theme, bool reset_LockKeyInputsInMenus) {
		bool LockKeyInputsInMenus = config_data.LockKeyInputsInMenus;
		bool GUI_Theme = config_data.GUI_Theme;
		config_data = Default_Config.GUI_Settings;
		if (reset_LockKeyInputsInMenus == false) {
			config_data.LockKeyInputsInMenus = LockKeyInputsInMenus; // Preserves this setting
		}
		if (reset_GUI_Theme == false) {
			config_data.GUI_Theme = GUI_Theme; // Preserves this setting
		}
	}
	void default_Screenshot_Settings(User_Screenshot_Settings& config_data) { config_data = Default_Config.Screenshot_Settings; }
	void default_Rendering_Settings(User_Rendering_Settings& config_data) { config_data = Default_Config.Rendering_Settings; }

	void default_User_Configuration_Data(User_Configuration_Data& config_data, bool reset_Extra) {
		default_Automatic_Behaviour(config_data.Automatic_Behaviour);
		default_Parameter_Sensitivity(config_data.Parameter_Sensitivity, reset_Extra);
		default_Display_Preferences(config_data.Display_Preferences);
		default_GUI_Settings(config_data.GUI_Settings, reset_Extra, reset_Extra);
		default_Screenshot_Settings(config_data.Screenshot_Settings);
		default_Rendering_Settings(config_data.Rendering_Settings);
	}

const char* get_config_value(const char* Config_Text, const char* Header, const char* Value) {
	if (Config_Text == nullptr || Header == nullptr || Value == nullptr) { return nullptr; }
	size_t i = 0;
	// Find Header
	while (Config_Text[i] != '\0') {
		if (
			((Config_Text[i] == '\n') || (i == 0)) &&
			(Config_Text[i + 1] != '\0')
		) {
			i++;
			if (
				(strncmp(&Config_Text[i],Header,strlen(Header)) == 0) &&
				(Config_Text[i + strlen(Header)] == ':')
			) {
				i += strlen(Header);
				i++;
				break;
			}
			i--;
		}
		i++;
	};
	if (Config_Text[i] == '\0') {
		return nullptr;
	}
	
	// Find Value
	while (Config_Text[i] != '\0') {
		if (
			(Config_Text[i] == '\n') && (
				(Config_Text[i + 1] != '\t') &&
				(Config_Text[i + 1] != '\n') &&
				(Config_Text[i + 1] != ' ')
			)
		) {
			return nullptr;
		}
		if (
			((Config_Text[i] == '\n') && (Config_Text[i + 1] == '\t')) &&
			(Config_Text[i + 2] != '\0')
		) {
			i += 2;
			if (
				(strncmp(&Config_Text[i],Value,strlen(Value)) == 0) &&
				(Config_Text[i + strlen(Value)] == ':')
			) {
				i += strlen(Value);
				i++;
				break;
			}
			i -= 2;
		}
		i++;
	};
	while (Config_Text[i] == ' ') {
		i++;
	};
	if (Config_Text[i] == '\0' || Config_Text[i] == '\n') {
		return nullptr;
	}
	return &Config_Text[i];
}

// Checks if the Text matches "true", "True", or "TRUE"; Returns false otherwise.
bool textToBool_FalseDefault(const char* Text) {
	return
	(Text == nullptr) ? false :
	(strncmp(Text,"true",strlen("true")) == 0) ? true :
	(strncmp(Text,"True",strlen("True")) == 0) ? true :
	(strncmp(Text,"TRUE",strlen("TRUE")) == 0) ? true : false;
}

// Checks if the Text matches "false", "False", or "FALSE"; Returns true otherwise.
bool textToBool_TrueDefault(const char* Text) {
	return
	(Text == nullptr) ? true :
	(strncmp(Text,"false",strlen("false")) == 0) ? false :
	(strncmp(Text,"False",strlen("False")) == 0) ? false :
	(strncmp(Text,"FALSE",strlen("FALSE")) == 0) ? false : true;
}

void compare_Versions(int32_t version_major, int32_t version_minor, int32_t version_patch, bool warnPatch) {
	// Out of date Software
	if (
		(PROGRAM_V_MAJOR < version_major) ||
		(PROGRAM_V_MAJOR == version_major && PROGRAM_V_MINOR < version_minor)
	) {
		printFlush(
			"\nWarning: Config-File might not be backwards compatible:"\
			"\n(Current) v%" PRId32 ".%" PRId32 ".%" PRId32 " < (Config-File) v%" PRId32 ".%" PRId32 ".%" PRId32,
			PROGRAM_V_MAJOR,PROGRAM_V_MINOR,PROGRAM_V_PATCH,
			version_major,version_minor,version_patch
		);
	}
	// Out of date Config-File
	if (
		(PROGRAM_V_MAJOR > version_major) ||
		(PROGRAM_V_MAJOR == version_major && PROGRAM_V_MINOR > version_minor)
	) {
		printFlush(
			"\nWarning: Config-File might not be fully supported:"\
			"\n(Current) v%" PRId32 ".%" PRId32 ".%" PRId32 " > (Config-File) v%" PRId32 ".%" PRId32 ".%" PRId32,
			PROGRAM_V_MAJOR,PROGRAM_V_MINOR,PROGRAM_V_PATCH,
			version_major,version_minor,version_patch
		);
	}
	if (warnPatch == false) {
		return;
	}
	if (PROGRAM_V_PATCH != version_patch) {
		printFlush(
			"\nNote: Config-File doesn't match current software version:"\
			"\n(Current) v%" PRId32 ".%" PRId32 ".%" PRId32 " != (Config-File) v%" PRId32 ".%" PRId32 ".%" PRId32,
			PROGRAM_V_MAJOR,PROGRAM_V_MINOR,PROGRAM_V_PATCH,
			version_major,version_minor,version_patch
		);
	}
}

void load_config_values(User_Configuration_Data& config_data, const char* Config_Text) {
	if (Config_Text == nullptr) { return; }

	// textToEnum will need to support text values too
	#define textToEnum(str) stringTo_Int32(str)

	const char* config_label = nullptr;

	config_label = "Version";
	int32_t version_major = stringTo_Int32(get_config_value(Config_Text,config_label,"Major"));
	int32_t version_minor = stringTo_Int32(get_config_value(Config_Text,config_label,"Minor"));
	int32_t version_patch = stringTo_Int32(get_config_value(Config_Text,config_label,"Patch"));
	compare_Versions(version_major,version_minor,version_patch,true);

	config_label = User_Configuration_Labels[Automatic_Behaviour];
		// if (textToBool_TrueDefault(get_config_value(Config_Text,config_label,"AutoLoad_Config_File")) == false) {
		// 	default_User_Configuration_Data(config_data);
		// 	return;
		// } // Uses the defualts
		config_data.Automatic_Behaviour.AutoLoad_Config_File =
		textToBool_TrueDefault(get_config_value(Config_Text,config_label,"AutoLoad_Config_File"));
		config_data.Automatic_Behaviour.AutoSave_Config_File =
		textToBool_TrueDefault(get_config_value(Config_Text,config_label,"AutoSave_Config_File"));

	config_label = User_Configuration_Labels[Parameter_Sensitivity];
		config_data.Parameter_Sensitivity.global =
		stringTo_Float64(get_config_value(Config_Text,config_label,"global"));
		config_data.Parameter_Sensitivity.coordinate =
		stringTo_Float64(get_config_value(Config_Text,config_label,"coordinate"));
		config_data.Parameter_Sensitivity.zoom =
		stringTo_Float64(get_config_value(Config_Text,config_label,"zoom"));
		config_data.Parameter_Sensitivity.invert_zoom =
		textToBool_FalseDefault(get_config_value(Config_Text,config_label,"invert_zoom"));
		config_data.Parameter_Sensitivity.maxIter =
		stringTo_Float64(get_config_value(Config_Text,config_label,"maxIter"));
		config_data.Parameter_Sensitivity.julia =
		stringTo_Float64(get_config_value(Config_Text,config_label,"julia"));
		config_data.Parameter_Sensitivity.rotation =
		stringTo_Float64(get_config_value(Config_Text,config_label,"rotation"));
		config_data.Parameter_Sensitivity.stretch =
		stringTo_Float64(get_config_value(Config_Text,config_label,"stretch"));
		config_data.Parameter_Sensitivity.polar_power =
		stringTo_Float64(get_config_value(Config_Text,config_label,"polar_power"));
		config_data.Parameter_Sensitivity.breakout_value =
		stringTo_Float64(get_config_value(Config_Text,config_label,"breakout_value"));

	config_label = User_Configuration_Labels[Display_Preferences];
		config_data.Display_Preferences.Display_Config_Hash =
		stringTo_Uint64(get_config_value(Config_Text,config_label,"Display_Config_Hash"));
		config_data.Display_Preferences.Display_Bootup_Type =
		textToEnum(get_config_value(Config_Text,config_label,"Display_Bootup_Type"));
		config_data.Display_Preferences.Specific_Bootup_Display =
		stringTo_Int32(get_config_value(Config_Text,config_label,"Specific_Bootup_Display"));
		config_data.Display_Preferences.Previous_Display_Used =
		stringTo_Int32(get_config_value(Config_Text,config_label,"Previous_Display_Used"));
		config_data.Display_Preferences.Bootup_Fullscreen =
		textToEnum(get_config_value(Config_Text,config_label,"Bootup_Fullscreen"));
		config_data.Display_Preferences.ScaleWindowToScreenSize =
		textToBool_TrueDefault(get_config_value(Config_Text,config_label,"ScaleWindowToScreenSize"));
		config_data.Display_Preferences.Bootup_Window_Scale =
		stringTo_Float64(get_config_value(Config_Text,config_label,"Bootup_Window_Scale"));
		config_data.Display_Preferences.Display_RefreshRate_Type =
		textToEnum(get_config_value(Config_Text,config_label,"Display_RefreshRate_Type"));
		config_data.Display_Preferences.Automatically_Update_RefreshRate =
		textToBool_TrueDefault(get_config_value(Config_Text,config_label,"Automatically_Update_RefreshRate"));
		config_data.Display_Preferences.Constant_RefreshRate_Value =
		stringTo_Float64(get_config_value(Config_Text,config_label,"Constant_RefreshRate_Value"));
		config_data.Display_Preferences.Maximum_FPS_Multiplier =
		stringTo_Int32(get_config_value(Config_Text,config_label,"Maximum_FPS_Multiplier"));

	config_label = User_Configuration_Labels[GUI_Settings];
		config_data.GUI_Settings.LockKeyInputsInMenus =
		textToBool_TrueDefault(get_config_value(Config_Text,config_label,"LockKeyInputsInMenus"));
		config_data.GUI_Settings.AutoResizeWindows =
		textToBool_FalseDefault(get_config_value(Config_Text,config_label,"AutoResizeWindows"));
		config_data.GUI_Settings.PreventOutOfBoundsWindows =
		textToBool_FalseDefault(get_config_value(Config_Text,config_label,"PreventOutOfBoundsWindows"));
		config_data.GUI_Settings.GUI_Theme =
		textToEnum(get_config_value(Config_Text,config_label,"GUI_Theme"));
		config_data.GUI_Settings.WindowOpacity =
		stringTo_Float32(get_config_value(Config_Text,config_label,"WindowOpacity"));
		config_data.GUI_Settings.WindowAutoScale =
		stringTo_Float64(get_config_value(Config_Text,config_label,"WindowAutoScale"));

	config_label = User_Configuration_Labels[Screenshot_Settings];
		config_data.Screenshot_Settings.screenshotFileType =
		textToEnum(get_config_value(Config_Text,config_label,"screenshotFileType"));
		config_data.Screenshot_Settings.PNG_Compression_Level =
		stringTo_Uint32(get_config_value(Config_Text,config_label,"PNG_Compression_Level"));
		config_data.Screenshot_Settings.JPG_Quality_Level =
		stringTo_Uint32(get_config_value(Config_Text,config_label,"JPG_Quality_Level"));

	config_label = User_Configuration_Labels[Rendering_Settings];
		config_data.Rendering_Settings.Hardware_Hash =
		stringTo_Uint64(get_config_value(Config_Text,config_label,"Hardware_Hash"));
		config_data.Rendering_Settings.Frame_Interpolation_Method =
		textToEnum(get_config_value(Config_Text,config_label,"Frame_Interpolation_Method"));
		config_data.Rendering_Settings.Image_Render_Bounding_Box =
		textToEnum(get_config_value(Config_Text,config_label,"Image_Render_Bounding_Box"));
		config_data.Rendering_Settings.JuliaPoint_Enabled =
		textToBool_TrueDefault(get_config_value(Config_Text,config_label,"JuliaPoint_Enabled"));
		config_data.Rendering_Settings.JuliaPoint_OuterRadius =
		stringTo_Float32(get_config_value(Config_Text,config_label,"JuliaPoint_OuterRadius"));
		config_data.Rendering_Settings.JuliaPoint_InnerRadius =
		stringTo_Float32(get_config_value(Config_Text,config_label,"JuliaPoint_InnerRadius"));
}

int import_config_data(User_Configuration_Data& config_data, const char* path) {
	if (path == nullptr) { printError("import_config_data has nullptr path"); return -1; }
	FILE* file = fopen(path, "rb");
	if (file == nullptr) {
		printError("Unable to import_config_data from: \"%s\"", path);
		return -1;
	}

	// Arbitrary minimum for blank/empty file detection
	constexpr size_t Minimum_User_Config_File_Size = 10;
	// Arbitrary maximum for invalid file-type detection
	constexpr size_t Maximum_User_Config_File_Size = 250000;

	fseek(file, 0, SEEK_END);

	long ftell_len = ftell(file);
	size_t fileSize = (ftell_len >= 0) ? (size_t)ftell_len : 0;
	rewind(file);
	if (fileSize >= Maximum_User_Config_File_Size) {
		printError("config file is abnormally large (%zubytes)",fileSize);
		fclose(file);
		return -1;
	} else if (fileSize <= Minimum_User_Config_File_Size) {
		printError("config file is too small (%zubytes)",fileSize);
		fclose(file);
		return -1;
	}

	char* config_text = nullptr;
	config_text = (char*)calloc(fileSize + 1, sizeof(char));
	if (config_text == nullptr) {
		printError("import_config_data failed to calloc");
		fclose(file);
		return -1;
	}
	size_t importLen = fread(config_text, sizeof(char), fileSize, file);
	config_text[fileSize] = '\0';
	if (fileSize != importLen) {
		printWarning("import_config_data: len %zu fread %zu",fileSize,importLen);
	}

	load_config_values(config_data,config_text);
	clean_User_Configuration_Data(config_data);

	FREE(config_text);
	fclose(file);

	return 0;
}

const char* getDateAndTimeUTC(
	char DateDelimiter, char DateSeparator, char TimeDelimiter
) {
	static char timeFormat[sizeof("%Y-%m-%d_%H-%M-%S")];
	memset(timeFormat,'\0',sizeof(timeFormat));

	char* timeText = nullptr;
	size_t timeTextLen = sizeof("YYYY-mm-dd_HH-MM-SS");
	timeText = (char*)calloc(timeTextLen,sizeof(char));
	if (timeText == nullptr) {
		return nullptr;
	}
	
	snprintf(timeFormat,sizeof(timeFormat),"%%Y%c%%m%c%%" PRId32 "%c%%H%c%%M%c%%S",
		DateDelimiter,DateDelimiter,DateSeparator,TimeDelimiter,TimeDelimiter
	);
	time_t currentTime;
	struct tm *localTime;
	time(&currentTime);
	localTime = localtime(&currentTime);
	strftime(timeText, timeTextLen, timeFormat, localTime);
	return timeText;
}

int export_config_data(User_Configuration_Data& config_data, const char* path) {
	if (path == nullptr) { printError("export_config_data has nullptr path"); return -1; }
	// if (config_data.Automatic_Behaviour.AutoSave_Config_File == false) { return 0; }
	FILE* file = fopen(path, "w");
	if (file == nullptr) {
		printError("Unable to export_config_data to: \"%s\"", path);
		return -1;
	}
	//char buf_fullpath[1024]; memset(buf_fullpath,'\0',sizeof(buf_fullpath));
	//_fullpath(buf_fullpath,path,1024);
	//printFlush("\nFull-Path: %s",buf_fullpath);

	fprintf(file,"%s Config File",PROGRAM_NAME);
	fprintf(file,"\n\nVersion:");
		fprintf(file,"\n\tDate: %s",PROGRAM_DATE);
		fprintf(file,"\n\tMajor: %" PRId32,PROGRAM_V_MAJOR);
		fprintf(file,"\n\tMinor: %" PRId32,PROGRAM_V_MINOR);
		fprintf(file,"\n\tPatch: %" PRId32,PROGRAM_V_PATCH);
		fprintf(file,"\n\tTag: %s",PROGRAM_V_TAG);
		{
			static char timeText[sizeof("YYYY/mm/dd HH:MM:SS")]; memset(timeText,'\0',sizeof(timeText));
			time_t currentTime;
			struct tm *utcTime;
			time(&currentTime);
			utcTime = gmtime(&currentTime);
			strftime(timeText, sizeof(timeText), "%Y/%m/%" PRId32 " %H:%M:%S", utcTime);
			fprintf(file, "\n\tExport_Time: %s UTC", timeText);
		}

	fprintf(file,"\n\n%s:",User_Configuration_Labels[Automatic_Behaviour]);
		fprintf(file,"\n\tAutoLoad_Config_File: %s",
			bool_Text(config_data.Automatic_Behaviour.AutoLoad_Config_File)
		);
		fprintf(file,"\n\tAutoSave_Config_File: %s",
			bool_Text(config_data.Automatic_Behaviour.AutoSave_Config_File)
		);

	fprintf(file,"\n\n%s:",User_Configuration_Labels[Parameter_Sensitivity]);
		fprintf(file,"\n\tglobal: %.6lf",
			config_data.Parameter_Sensitivity.global
		);
		fprintf(file,"\n\tcoordinate: %.6lf",
			config_data.Parameter_Sensitivity.coordinate
		);
		fprintf(file,"\n\tzoom: %.6lf",
			config_data.Parameter_Sensitivity.zoom
		);
		fprintf(file,"\n\tinvert_zoom: %s",
			bool_Text(config_data.Parameter_Sensitivity.invert_zoom)
		);
		fprintf(file,"\n\tmaxIter: %.6lf",
			config_data.Parameter_Sensitivity.maxIter
		);
		fprintf(file,"\n\tjulia: %.6lf",
			config_data.Parameter_Sensitivity.julia
		);
		fprintf(file,"\n\trotation: %.6lf",
			config_data.Parameter_Sensitivity.rotation
		);
		fprintf(file,"\n\tstretch: %.6lf",
			config_data.Parameter_Sensitivity.stretch
		);
		fprintf(file,"\n\tpolar_power: %.6lf",
			config_data.Parameter_Sensitivity.polar_power
		);
		fprintf(file,"\n\tbreakout_value: %.6lf",
			config_data.Parameter_Sensitivity.breakout_value
		);

	fprintf(file,"\n\n%s:",User_Configuration_Labels[Display_Preferences]);
		fprintf(file,"\n\tDisplay_Config_Hash: %" PRId64,
			config_data.Display_Preferences.Display_Config_Hash
		);
		fprintf(file,"\n\tDisplay_Bootup_Type: %" PRId32,
			config_data.Display_Preferences.Display_Bootup_Type
		);
		fprintf(file,"\n\tSpecific_Bootup_Display: %" PRId32,
			config_data.Display_Preferences.Specific_Bootup_Display
		);
		fprintf(file,"\n\tPrevious_Display_Used: %" PRId32,
			config_data.Display_Preferences.Previous_Display_Used
		);
		fprintf(file,"\n\tBootup_Fullscreen: %" PRId32,
			config_data.Display_Preferences.Bootup_Fullscreen
		);
		fprintf(file,"\n\tScaleWindowToScreenSize: %s",
			bool_Text(config_data.Display_Preferences.ScaleWindowToScreenSize)
		);
		fprintf(file,"\n\tBootup_Window_Scale: %.6lf",
			config_data.Display_Preferences.Bootup_Window_Scale
		);
		fprintf(file,"\n\tDisplay_RefreshRate_Type: %" PRId32,
			config_data.Display_Preferences.Display_RefreshRate_Type
		);
		fprintf(file,"\n\tAutomatically_Update_RefreshRate: %" PRId32,
			config_data.Display_Preferences.Automatically_Update_RefreshRate
		);
		fprintf(file,"\n\tConstant_RefreshRate_Value: %.6lf",
			config_data.Display_Preferences.Constant_RefreshRate_Value
		);
		fprintf(file,"\n\tMaximum_FPS_Multiplier: %" PRId32,
			config_data.Display_Preferences.Maximum_FPS_Multiplier
		);

	fprintf(file,"\n\n%s:",User_Configuration_Labels[GUI_Settings]);
		fprintf(file,"\n\tLockKeyInputsInMenus: %s",
			bool_Text(config_data.GUI_Settings.LockKeyInputsInMenus)
		);
		fprintf(file,"\n\tAutoResizeWindows: %s",
			bool_Text(config_data.GUI_Settings.AutoResizeWindows)
		);
		fprintf(file,"\n\tPreventOutOfBoundsWindows: %s",
			bool_Text(config_data.GUI_Settings.PreventOutOfBoundsWindows)
		);
		fprintf(file,"\n\tGUI_Theme: %" PRId32,
			config_data.GUI_Settings.GUI_Theme
		);
		fprintf(file,"\n\tWindowOpacity: %.6f",
			config_data.GUI_Settings.WindowOpacity
		);
		fprintf(file,"\n\tWindowAutoScale: %.6lf",
			config_data.GUI_Settings.WindowAutoScale
		);

	fprintf(file,"\n\n%s:",User_Configuration_Labels[Screenshot_Settings]);
		fprintf(file,"\n\tscreenshotFileType: %" PRId32,
			config_data.Screenshot_Settings.screenshotFileType
		);
		fprintf(file,"\n\tPNG_Compression_Level: %" PRIu32,
			config_data.Screenshot_Settings.PNG_Compression_Level
		);
		fprintf(file,"\n\tJPG_Quality_Level: %" PRIu32,
			config_data.Screenshot_Settings.JPG_Quality_Level
		);

	fprintf(file,"\n\n%s:",User_Configuration_Labels[Rendering_Settings]);
		fprintf(file,"\n\tHardware_Hash: %" PRId64,
			config_data.Rendering_Settings.Hardware_Hash
		);
		fprintf(file,"\n\tFrame_Interpolation_Method: %" PRId32,
			config_data.Rendering_Settings.Frame_Interpolation_Method
		);
		fprintf(file,"\n\tImage_Render_Bounding_Box: %" PRId32,
			config_data.Rendering_Settings.Image_Render_Bounding_Box
		);
		fprintf(file,"\n\tJuliaPoint_Enabled: %s",
			bool_Text(config_data.Rendering_Settings.JuliaPoint_Enabled)
		);
		fprintf(file,"\n\tJuliaPoint_OuterRadius: %.6f",
			config_data.Rendering_Settings.JuliaPoint_OuterRadius
		);
		fprintf(file,"\n\tJuliaPoint_InnerRadius: %.6f",
			config_data.Rendering_Settings.JuliaPoint_InnerRadius
		);

	fclose(file);
	return 0;
}