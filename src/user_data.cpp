/*
**	Author: zerico2005 (2023-2024)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#include "Common_Def.h"
#include "Program_Def.h"
#include "User_Data.h"

struct Point {
	int x;
	int y;
};
typedef struct Point Point;

struct Triangle {
	Point a;
	Point b;
	Point c;
};
typedef struct Triangle Triangle;

Point point = {
	.x = 10, .y = 20
};

Triangle tri = {
	.a = {.x = 1, .y = 2},
	.b = {.x = 3, .y = 4},
	.c = {.x = 5, .y = 6},
};

constexpr User_Configuration_Data Default_Config = {
	.Configuration_Behaviour = {
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
		.Display_Bootup_Type = 0, // 0 == Automatic (Enum)
		.Specific_Bootup_Display = 0,
		.Bootup_Fullscreen = false,
		.ScaleWindowToScreenSize = false,
		.Bootup_Window_Scale = 0.7
	},
	.GUI_Settings = {
		.LockKeyInputsInMenus = true,
		.AutoResizeWindows = false,
		.PreventOutOfBoundsWindows = false,
		.WindowOpacity = 0.95f,
		.WindowAutoScale = 0.7
	},
	.Screenshot_Settings = {
		.screenshotFileType = 0, // 0 == PNG (Enum)
		.PNG_Compression_Level = 8,
		.JPG_Quality_Level = 95
	}
};

#define clean_config_data(value,min,max)\
valueRestore(config_data.value,config_default.value,(min),(max))

/* clean_config_data */

	void clean_Configuration_Behaviour(User_Configuration_Behaviour& config_data) {
		const User_Configuration_Behaviour& config_default = Default_Config.Configuration_Behaviour;
		return; // Configuration_Behaviour only has bools
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
		// clean_config_data(Display_Bootup_Type, 0, 0);
		clean_config_data(Specific_Bootup_Display, 144, 0); // Does anyone even have 144 displays? Probably not.
		clean_config_data(Bootup_Window_Scale, 0.01, 1.0);
	}

	void clean_GUI_Settings(User_GUI_Settings& config_data) {
		const User_GUI_Settings& config_default = Default_Config.GUI_Settings;
		clean_config_data(WindowOpacity, 0.2, 1.0);
		clean_config_data(WindowAutoScale, 0.1, 1.0);
	}

	void clean_Screenshot_Settings(User_Screenshot_Settings& config_data) {
		const User_Screenshot_Settings& config_default = Default_Config.Screenshot_Settings;
		// valueRestore(config_data.screenshotFileType, 0, 0, 0);
		clean_config_data(PNG_Compression_Level, 1, 9);
		clean_config_data(JPG_Quality_Level, 20, 100);
	}

	void clean_User_Configuration_Data(User_Configuration_Data& config_data) {
		clean_Configuration_Behaviour(config_data.Configuration_Behaviour);
		clean_Parameter_Sensitivity(config_data.Parameter_Sensitivity);
		clean_Display_Preferences(config_data.Display_Preferences);
		clean_GUI_Settings(config_data.GUI_Settings);
		clean_Screenshot_Settings(config_data.Screenshot_Settings);
	}

/* default_config_data */

	void default_Configuration_Behaviour(User_Configuration_Behaviour& config_data) { config_data = Default_Config.Configuration_Behaviour; }
	void default_Parameter_Sensitivity(User_Parameter_Sensitivity& config_data) { config_data = Default_Config.Parameter_Sensitivity; }
	void default_Display_Preferences(User_Display_Preferences& config_data) { config_data = Default_Config.Display_Preferences; }
	void default_GUI_Settings(User_GUI_Settings& config_data) { config_data = Default_Config.GUI_Settings; }
	void default_Screenshot_Settings(User_Screenshot_Settings& config_data) { config_data = Default_Config.Screenshot_Settings; }

	void default_User_Configuration_Data(User_Configuration_Data& config_data) {
		default_Configuration_Behaviour(config_data.Configuration_Behaviour);
		default_Parameter_Sensitivity(config_data.Parameter_Sensitivity);
		default_Display_Preferences(config_data.Display_Preferences);
		default_GUI_Settings(config_data.GUI_Settings);
		default_Screenshot_Settings(config_data.Screenshot_Settings);
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

void load_config_values(User_Configuration_Data& config_data, const char* Config_Text) {
	if (Config_Text == nullptr) { return; }

	#define textToFloat32(str) (fp32)atof(str)
	#define textToFloat64(str) atof(str)
	#define textToInt32(str) atoi(str)
	// textToEnum will need to support text values too
	#define textToEnum(str) atoi(str)
	#define textToUint32(str) (uint32_t)atoll(str)
	#define textToInt64(str) atoll(str)
	#define textToUint64(str) (uint64_t)atoll(str)

	const char* config_label = nullptr;

	config_label = User_Configuration_Labels[Configuration_Behaviour];
		// if (textToBool(get_config_value(Config_Text,config_label,"AutoLoad_Config_File")) == false) {
		// 	return;
		// } // Uses the defualts
		config_data.Configuration_Behaviour.AutoLoad_Config_File =
		textToBool_TrueDefault(get_config_value(Config_Text,config_label,"AutoLoad_Config_File"));
		config_data.Configuration_Behaviour.AutoSave_Config_File =
		textToBool_TrueDefault(get_config_value(Config_Text,config_label,"AutoSave_Config_File"));

	config_label = User_Configuration_Labels[Parameter_Sensitivity];
		config_data.Parameter_Sensitivity.global =
		textToFloat64(get_config_value(Config_Text,config_label,"global"));
		config_data.Parameter_Sensitivity.coordinate =
		textToFloat64(get_config_value(Config_Text,config_label,"coordinate"));
		config_data.Parameter_Sensitivity.zoom =
		textToFloat64(get_config_value(Config_Text,config_label,"zoom"));
		config_data.Parameter_Sensitivity.invert_zoom =
		textToBool_FalseDefault(get_config_value(Config_Text,config_label,"invert_zoom"));
		config_data.Parameter_Sensitivity.maxIter =
		textToFloat64(get_config_value(Config_Text,config_label,"maxIter"));
		config_data.Parameter_Sensitivity.julia =
		textToFloat64(get_config_value(Config_Text,config_label,"julia"));
		config_data.Parameter_Sensitivity.rotation =
		textToFloat64(get_config_value(Config_Text,config_label,"rotation"));
		config_data.Parameter_Sensitivity.stretch =
		textToFloat64(get_config_value(Config_Text,config_label,"stretch"));
		config_data.Parameter_Sensitivity.polar_power =
		textToFloat64(get_config_value(Config_Text,config_label,"polar_power"));
		config_data.Parameter_Sensitivity.breakout_value =
		textToFloat64(get_config_value(Config_Text,config_label,"breakout_value"));

	config_label = User_Configuration_Labels[Display_Preferences];
		config_data.Display_Preferences.Display_Bootup_Type =
		textToEnum(get_config_value(Config_Text,config_label,"Display_Bootup_Type"));
		config_data.Display_Preferences.Specific_Bootup_Display =
		textToInt32(get_config_value(Config_Text,config_label,"Specific_Bootup_Display"));
		config_data.Display_Preferences.Bootup_Fullscreen =
		textToBool_TrueDefault(get_config_value(Config_Text,config_label,"Bootup_Fullscreen"));
		config_data.Display_Preferences.ScaleWindowToScreenSize =
		textToBool_TrueDefault(get_config_value(Config_Text,config_label,"ScaleWindowToScreenSize"));
		config_data.Display_Preferences.Bootup_Window_Scale =
		textToFloat64(get_config_value(Config_Text,config_label,"Bootup_Window_Scale"));

	config_label = User_Configuration_Labels[GUI_Settings];
		config_data.GUI_Settings.LockKeyInputsInMenus =
		textToBool_TrueDefault(get_config_value(Config_Text,config_label,"LockKeyInputsInMenus"));
		config_data.GUI_Settings.AutoResizeWindows =
		textToBool_FalseDefault(get_config_value(Config_Text,config_label,"AutoResizeWindows"));
		config_data.GUI_Settings.PreventOutOfBoundsWindows =
		textToBool_FalseDefault(get_config_value(Config_Text,config_label,"PreventOutOfBoundsWindows"));
		config_data.GUI_Settings.WindowOpacity =
		textToFloat32(get_config_value(Config_Text,config_label,"WindowOpacity"));
		config_data.GUI_Settings.WindowAutoScale =
		textToFloat64(get_config_value(Config_Text,config_label,"WindowAutoScale"));

	config_label = User_Configuration_Labels[Screenshot_Settings];
		config_data.Screenshot_Settings.screenshotFileType =
		textToEnum(get_config_value(Config_Text,config_label,"screenshotFileType"));
		config_data.Screenshot_Settings.PNG_Compression_Level =
		textToUint32(get_config_value(Config_Text,config_label,"PNG_Compression_Level"));
		config_data.Screenshot_Settings.JPG_Quality_Level =
		textToUint32(get_config_value(Config_Text,config_label,"JPG_Quality_Level"));
}

int import_config_data(User_Configuration_Data& config_data, const char* path) {
	if (path == nullptr) { printError("import_config_data has nullptr path"); return -1; }
	FILE* file = fopen(path, "rb");
	if (file == nullptr) {
		printError("Unable to import_config_data from %s", path);
		return -1;
	}

	fseek(file, 0, SEEK_END);
	size_t fileSize = ftell(file);
	rewind(file);
	if (fileSize >= 250000) {
		printError("config file is abnormally large (%zubytes)",fileSize);
		fclose(file);
		return -1;
	} else if (fileSize <= 10) {
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
	fread(config_text, sizeof(char), fileSize, file);
	config_text[fileSize] = '\0';

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
	
	snprintf(timeFormat,sizeof(timeFormat),"%%Y%c%%m%c%%d%c%%H%c%%M%c%%S",
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
	// if (config_data.Configuration_Behaviour.AutoSave_Config_File == false) { return 0; }
	FILE* file = fopen(path, "w");
	if (file == nullptr) {
		printError("Unable to export_config_data to %s", path);
		return -1;
	}

	fprintf(file,"%s Config File",PROGRAM_NAME);
	fprintf(file,"\n\nVersion:");
		fprintf(file,"\n\tDate: %s",PROGRAM_DATE);
		fprintf(file,"\n\tMajor: %d",PROGRAM_V_MAJOR);
		fprintf(file,"\n\tMinor: %d",PROGRAM_V_MINOR);
		fprintf(file,"\n\tPatch: %d",PROGRAM_V_PATCH);
		fprintf(file,"\n\tTag: %s",PROGRAM_V_TAG);
		{
			static char timeText[sizeof("YYYY/mm/dd HH:MM:SS")]; memset(timeText,'\0',sizeof(timeText));
			time_t currentTime;
			struct tm *utcTime;
			time(&currentTime);
			utcTime = gmtime(&currentTime);
			strftime(timeText, sizeof(timeText), "%Y/%m/%d %H:%M:%S", utcTime);
			fprintf(file, "\n\tExport_Time: %s UTC", timeText);
		}

	fprintf(file,"\n\n%s:",User_Configuration_Labels[Configuration_Behaviour]);
		fprintf(file,"\n\tAutoLoad_Config_File: %s",
			boolText(config_data.Configuration_Behaviour.AutoLoad_Config_File)
		);
		fprintf(file,"\n\tAutoSave_Config_File: %s",
			boolText(config_data.Configuration_Behaviour.AutoSave_Config_File)
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
			boolText(config_data.Parameter_Sensitivity.invert_zoom)
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
		fprintf(file,"\n\t: %d",
			config_data.Display_Preferences.Display_Bootup_Type
		);
		fprintf(file,"\n\t: %d",
			config_data.Display_Preferences.Specific_Bootup_Display
		);
		fprintf(file,"\n\t: %s",
			boolText(config_data.Display_Preferences.Bootup_Fullscreen)
		);
		fprintf(file,"\n\t: %s",
			boolText(config_data.Display_Preferences.ScaleWindowToScreenSize)
		);
		fprintf(file,"\n\t: %.6lf",
			config_data.Display_Preferences.Bootup_Window_Scale
		);

	fprintf(file,"\n\n%s:",User_Configuration_Labels[GUI_Settings]);
		fprintf(file,"\n\tLockKeyInputsInMenus: %s",
			boolText(config_data.GUI_Settings.LockKeyInputsInMenus)
		);
		fprintf(file,"\n\tAutoResizeWindows: %s",
			boolText(config_data.GUI_Settings.AutoResizeWindows)
		);
		fprintf(file,"\n\tPreventOutOfBoundsWindows: %s",
			boolText(config_data.GUI_Settings.PreventOutOfBoundsWindows)
		);
		fprintf(file,"\n\tWindowOpacity: %.6f",
			config_data.GUI_Settings.WindowOpacity
		);
		fprintf(file,"\n\tWindowAutoScale: %.6lf",
			config_data.GUI_Settings.WindowAutoScale
		);

	fprintf(file,"\n\n%s:",User_Configuration_Labels[Screenshot_Settings]);
		fprintf(file,"\n\tscreenshotFileType: %d",
			config_data.Screenshot_Settings.screenshotFileType
		);
		fprintf(file,"\n\tPNG_Compression_Level: %u",
			config_data.Screenshot_Settings.PNG_Compression_Level
		);
		fprintf(file,"\n\tJPG_Quality_Level: %u",
			config_data.Screenshot_Settings.JPG_Quality_Level
		);
		
	fclose(file);
	return 0;
}

bool set_default_sensitivity(User_Parameter_Sensitivity* sen) {
	if (sen == nullptr) { return false; }
	sen->global = 1.0;
	sen->coordinate = 1.0;
	sen->zoom = 1.0;
	sen->maxIter = 1.0;
	sen->julia = 1.0;
	sen->rotation = 1.0;
	sen->stretch = 1.0;
	sen->polar_power = 1.0;
	sen->breakout_value = 1.0;
	return true;
}

bool init_default_sensitivity(User_Parameter_Sensitivity* sen) {
	if (sen == nullptr) { return false; }
	sen->invert_zoom = false;
	return set_default_sensitivity(sen);
}