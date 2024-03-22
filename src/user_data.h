/*
**	Author: zerico2005 (2023-2024)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/
#ifndef USER_DATA_H
#define USER_DATA_H

#include "Common_Def.h"

/* File Settings */
	struct User_Automatic_Behaviour {
		bool AutoLoad_Config_File;
		bool AutoSave_Config_File;
	}; typedef struct User_Automatic_Behaviour User_Automatic_Behaviour;

/* Parameter Sensitivity */
	const char* const Sensitivity_Labels[] {
		"Global Sensitivity","Coordinate","Zoom","Iteration","Julia Coordinate","Rotation","Stretch","Polar Power","Breakout Value"
	};
	struct User_Parameter_Sensitivity {
		fp64 global; // Applies to moveDelta
		fp64 coordinate;
		fp64 zoom;
		bool invert_zoom;
		fp64 maxIter;
		fp64 julia;
		fp64 rotation;
		fp64 stretch;
		fp64 polar_power;
		fp64 breakout_value;
	}; typedef User_Parameter_Sensitivity User_Parameter_Sensitivity;

/* Display Settings */
	struct User_Display_Preferences {
		uint64_t Display_Config_Hash;
		int_enum Display_Bootup_Type;
		int Specific_Bootup_Display;
		int Previous_Display_Used;
		bool Bootup_Fullscreen;
		bool ScaleWindowToScreenSize;
		fp64 Bootup_Window_Scale;
		int_enum Display_RefreshRate_Type;
		fp64 Constant_RefreshRate_Value;
		int32_t Maximum_FPS_Multiplier;
	}; typedef struct User_Display_Preferences User_Display_Preferences;

/* GUI Settings */
	struct User_GUI_Settings {
		bool LockKeyInputsInMenus;
		bool AutoResizeWindows;
		bool PreventOutOfBoundsWindows;
		int_enum GUI_Theme;
		fp32 WindowOpacity;
		fp64 WindowAutoScale;
	}; typedef struct User_GUI_Settings User_GUI_Settings;

/* Screenshot Settings */
	struct User_Screenshot_Settings {
		int_enum screenshotFileType;
		uint32_t PNG_Compression_Level;
		uint32_t JPG_Quality_Level;
	}; typedef struct User_Screenshot_Settings User_Screenshot_Settings;

/* User Data Configuration */
	enum User_Configuration_Enum {
		Automatic_Behaviour,Parameter_Sensitivity,Display_Preferences,GUI_Settings,Screenshot_Settings,User_Configuration_Enum_Count
	};
	const char* const User_Configuration_Labels[] = {
		"Automatic_Behaviour","Parameter_Sensitivity","Display_Preferences","GUI_Settings","Screenshot_Settings"
	};
	struct User_Configuration_Data {
		User_Automatic_Behaviour Automatic_Behaviour;
		User_Parameter_Sensitivity Parameter_Sensitivity;
		User_Display_Preferences Display_Preferences;
		User_GUI_Settings GUI_Settings;
		User_Screenshot_Settings Screenshot_Settings;
	}; typedef struct User_Configuration_Data User_Configuration_Data;

// Import from file-path
int import_config_data(User_Configuration_Data& config_data, const char* path);

// Export to file-path
int export_config_data(User_Configuration_Data& config_data, const char* path);

/* clean_config_data */
	// Sets everything to valid values
	void clean_User_Configuration_Data(User_Configuration_Data& config_data);

	void clean_Automatic_Behaviour(User_Automatic_Behaviour& config_data);
	void clean_Parameter_Sensitivity(User_Parameter_Sensitivity& config_data);
	void clean_Display_Preferences(User_Display_Preferences& config_data);
	void clean_GUI_Settings(User_GUI_Settings& config_data);
	void clean_Screenshot_Settings(User_Screenshot_Settings& config_data);

/* default_config_data */
	// Sets everything to default values
	void default_User_Configuration_Data(User_Configuration_Data& config_data, bool reset_Extra = false);

	void default_Automatic_Behaviour(User_Automatic_Behaviour& config_data);
	void default_Parameter_Sensitivity(User_Parameter_Sensitivity& config_data, bool reset_Invert = false);
	void default_Display_Preferences(User_Display_Preferences& config_data);
	void default_GUI_Settings(User_GUI_Settings& config_data, bool reset_GUI_Theme = false, bool reset_LockKeyInputsInMenus = false);
	void default_Screenshot_Settings(User_Screenshot_Settings& config_data);


// Legacy

/*
struct OpenCL_Hardware_Info {
	uint32_t DeviceMaxComputeUnits;
	uint64_t DeviceMaxWorkGroupSize;
	uint32_t DeviceMaxClockFrequency;
	uint64_t DeviceMaxMemAllocSize;
	size_t DeviceMaxParameterSize;
	uint64_t DeviceGlobalMemSize;
	uint64_t DeviceMaxConstantBufferSize;
	uint32_t DeviceMaxConstantArgs;
	uint64_t DeviceLocalMemSize;
	size_t DeviceProfilingTimerResolution;
}; typedef OpenCL_Hardware_Info OpenCL_Hardware_Info;
*/

#endif /* USER_DATA_H */