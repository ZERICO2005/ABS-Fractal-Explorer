/*
**	Author: zerico2005 (2023-2024)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#include "display_GUI_Internal.h"

#ifdef Enable_OpenCL

#include "display_GPU_info.h"

#include "display_GUI.h"
#include "imgui.h"


#include "../render_GPU/fracCL_Internal.h"
#include "../render_GPU/fracCL.h"

	uint64_t WorkGroup_Size;
	uint64_t Local_Memory_Size;
	size_t   Preferred_WorkGroup_Size_Multiple;
	uint64_t Private_Memory_Size;

static void SubMenu_Float_Config(const char* title, const OpenCL_Float_Config& float_config) {
	if (ImGui::CollapsingHeader(title)) { ImGui::Indent();
		ImGui::Text("Hardware Acceleration: %s",     Bool_Text(!float_config.Soft_Float        ));
		ImGui::Text("Denormalized Values: %s" , Available_Text( float_config.Denormalized      ));
		Item_Tooltip("Allows for values very close to 0.0 to be represented");
		ImGui::Text("Infinity and NAN: %s"    , Available_Text( float_config.Infinity_NAN      ));
		ImGui::Text("Fused Multiply Add: %s"  , Available_Text( float_config.Fused_Multiply_Add));
		Item_Tooltip("Calculates a * b + c with higher speed/accuracy");
		ImGui::NewLine();
		ImGui::Text("Divide and Square-Root rounded correctly: %s", Bool_Text(float_config.Correctly_Rounded_Divide_SquareRoot));
		ImGui::Text("Round to Nearest: %s" , Available_Text(float_config.Round_to_Nearest ));
		ImGui::Text("Round to Zero: %s"    , Available_Text(float_config.Round_to_Zero    ));
		ImGui::Text("Round to Infinity: %s", Available_Text(float_config.Round_to_Infinity));
		ImGui::NewLine();
	ImGui::Unindent(); }
}

void SubMenu_GPU_Information() {
	const OpenCL_Engine engine = get_GPU_Information();
	const OpenCL_Device_Properties& device = engine.device_properties;
	const OpenCL_Kernel_Properties& kernel = engine.kernel_properties;
	/* Text */
		ImGui::Text("GPU Name: %s | %s", device.Name.c_str(), device.Vendor.c_str());
		ImGui::Text("Driver Version: %s", device.Driver_Version.c_str());
		ImGui::Text("Total VRAM: %" PRIu64 "MiB", device.Global_Memory_Size / 1048576);
		ImGui::NewLine();
		ImGui::Text("OpenCL Version: %s | %s", device.OpenCL_Version.c_str(), device.Profile.c_str());
		ImGui::NewLine();
	/* Floating Point */
		SubMenu_Float_Config("32bit Float Support", device.Float32_Config);
		SubMenu_Float_Config("64bit Float Support", device.Float64_Config);


	if (ImGui::CollapsingHeader("Other GPU Information")) {
		ImGui::SeparatorText("Device Properties"); { ImGui::Indent();
			ImGui::Text("Maximum Work-Group Size: %"         "zu"        , device.Maximum_WorkGroup_Size      );
			ImGui::Text("Maximum Compute Units: %"         PRIu32        , device.Maximum_Compute_Units       );
			ImGui::Text("Global Memory Size: %"            PRIu64 "bytes", device.Global_Memory_Size          );
			ImGui::Text("Maximum Memory Alloc Size: %"     PRIu64 "bytes", device.Maximum_Memory_Alloc_Size   );
			ImGui::Text("Maximum Clock Frequency: %"       PRIu32 "MHz"  , device.Maximum_Clock_Frequency     );
			ImGui::Text("Maximum Parameter Size: %"          "zu" "bytes", device.Maximum_Parameter_Size      );
			ImGui::Text("Maximum Constant Buffer Size: %"  PRIu64 "bytes", device.Maximum_Constant_Buffer_Size);
			ImGui::Text("Maximum Constant Args: %"         PRIu32        , device.Maximum_Constant_Args       );
			ImGui::Text("Local Memory Size: %"             PRIu64 "bytes", device.Local_Memory_Size           );
			ImGui::Text("Profiling Timer Resolution: %"      "zu" "ns"   , device.Profiling_Timer_Resolution  );
			ImGui::NewLine();
		ImGui::Unindent(); }
		
		
		ImGui::SeparatorText("Kernel Properties"); { ImGui::Indent();
			ImGui::Text("Work-Group Size: %"               PRIu64, kernel.WorkGroup_Size                   );
			ImGui::Text("Preferred Work-Group Size Multiple: %zu", kernel.Preferred_WorkGroup_Size_Multiple);
			// ImGui::Text("Local Memory Size: %"     PRIu64 "bytes", kernel.Local_Memory_Size                );
			// ImGui::Text("Private Memory Size: %"   PRIu64 "bytes", kernel.Private_Memory_Size              );
			ImGui::NewLine();
		ImGui::Unindent(); }
	}
}

#endif