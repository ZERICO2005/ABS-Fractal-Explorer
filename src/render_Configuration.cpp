/*
**	Author: zerico2005 (2023-2024)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#include "Common_Def.h"
#include "Program_Def.h"

#include "render_Configuration.hpp"

using namespace Rendering_Configuration;

/* class Render_Configurator */
/* public */
	/* Setup */

		void Render_Configurator::set_GPU_Configuration(
			bool enable_GPU_Float16,
			bool enable_GPU_Float32,
			bool enable_GPU_Float64
		) {
			GPU_Float16_Enabled = enable_GPU_Float16;
			GPU_Float32_Enabled = enable_GPU_Float32;
			GPU_Float64_Enabled = enable_GPU_Float64;
		}

		void Render_Configurator::reset_Render_Configurator(
				bool enable_GPU_Float16,
				bool enable_GPU_Float32,
				bool enable_GPU_Float64
		) {
			Render_Precision = Render_Precision_Automatic;
			Render_Method = Render_Method_Automatic;

			set_GPU_Configuration(
				enable_GPU_Float16,
				enable_GPU_Float32,
				enable_GPU_Float64
			);

			const Supported_CPU_Instruction& Available_CPU_Instruction = get_Available_CPU_Instruction();
			CPU_SSE2_Enabled = (
				Available_CPU_Instruction.SSE_Family.SSE2 &&
				Available_CPU_Instruction.SSE_Family.SSE
			);
			CPU_AVX_Enabled = Available_CPU_Instruction.AVX_Family.AVX;
			CPU_AVX512_F_Enabled = Available_CPU_Instruction.AVX512_Family.AVX512_F;
			CPU_AVX512_FP16_Enabled = (
				Available_CPU_Instruction.AVX512_Family.AVX512_F &&
				Available_CPU_Instruction.AVX512_Family.AVX512_FP16
			);

			CPU_Float16_Enabled = false;

			#ifdef Enable_Float80
				CPU_Float80_Enabled = true;
			#else
				CPU_Float80_Enabled = false;
			#endif
			#ifdef Enable_Float128
				CPU_Float128_Enabled = true;
			#else
				CPU_Float128_Enabled = false;
			#endif
		}

		Render_Configurator::Render_Configurator(
				bool enable_GPU_Float16,
				bool enable_GPU_Float32,
				bool enable_GPU_Float64
		) {
			reset_Render_Configurator(
				enable_GPU_Float16,
				enable_GPU_Float32,
				enable_GPU_Float64
			);
		}

		Render_Configurator::Render_Configurator() {
			reset_Render_Configurator(
				false,
				false,
				false
			);
		}

	/* Conversion */

		void Render_Configurator::get_Rendering_Precision_and_Method_from_Preset(
			const Rendering_Configuration::Rendering_Preset render_preset,
			Rendering_Configuration::Rendering_Precision& render_precision,
			Rendering_Configuration::Rendering_Method& render_method
		) const {
			for (size_t i = 0; i < ARRAY_LENGTH(Rendering_Preset_Conversion_Table); i++) {
				if (Rendering_Preset_Conversion_Table[i].preset == render_preset) {
					render_precision = Rendering_Preset_Conversion_Table[i].precision;
					render_method = Rendering_Preset_Conversion_Table[i].method;
					return;
				}
			}
			render_precision = Render_Precision_Unknown;
			render_method = Render_Method_Unknown;
		}

		Rendering_Configuration::Rendering_Preset Render_Configurator::
		Render_Configurator::get_Rendering_Preset_from_Precision_and_Method(
			const Rendering_Configuration::Rendering_Precision render_precision,
			const Rendering_Configuration::Rendering_Method render_method
		) const {
			for (size_t i = 0; i < ARRAY_LENGTH(Rendering_Preset_Conversion_Table); i++) {
				if (
					(Rendering_Preset_Conversion_Table[i].precision == render_precision) &&
					(Rendering_Preset_Conversion_Table[i].method == render_method)
				) {
					return Rendering_Preset_Conversion_Table[i].preset;
				}
			}
			return Render_Preset_Unknown;
		}

	/* Validation */

		bool Render_Configurator::validate_Rendering_Preset(
			const Rendering_Preset render_preset
		) const {
			switch(render_preset) {
			/* GPU Rendering */
				case Render_Preset_GPU_Float16:
					return (GPU_Float16_Enabled) ? true : false;
				case Render_Preset_GPU_Float32:
					return (GPU_Float32_Enabled) ? true : false;
				case Render_Preset_GPU_Float64:
					return (GPU_Float64_Enabled) ? true : false;
			/* CPU Generic Rendering */
				case Render_Preset_CPU_Generic_Float16:
					return (CPU_Float16_Enabled) ? true : false;
				case Render_Preset_CPU_Generic_Float32:
					return true; // Always supported
				case Render_Preset_CPU_Generic_Float64:
					return true; // Always supported
				case Render_Preset_CPU_Generic_Float80:
					return (CPU_Float80_Enabled) ? true : false;
				case Render_Preset_CPU_Generic_Float128:
					return (CPU_Float128_Enabled) ? true : false;
			/* CPU SSE2 Rendering */
				case Render_Preset_CPU_SSE2_Float32:
					return (CPU_SSE2_Enabled) ? true : false;
				case Render_Preset_CPU_SSE2_Float64:
					return (CPU_SSE2_Enabled) ? true : false;
			/* CPU AVX Rendering */
				case Render_Preset_CPU_AVX_Float32:
					return (CPU_AVX_Enabled) ? true : false;
				case Render_Preset_CPU_AVX_Float64:
					return (CPU_AVX_Enabled) ? true : false;
			/* CPU AVX512 Rendering */
				case Render_Preset_CPU_AVX512_Float16:
					return (CPU_AVX512_FP16_Enabled) ? true : false;
				case Render_Preset_CPU_AVX512_Float32:
					return (CPU_AVX512_F_Enabled) ? true : false;
				case Render_Preset_CPU_AVX512_Float64:
					return (CPU_AVX512_F_Enabled) ? true : false;
				default:
					return false;
			}
		}

		bool Render_Configurator::validate_Rendering_Precision_and_Method(
			const Rendering_Precision render_precision,
			const Rendering_Method render_method
		) const {
			Rendering_Preset render_preset = get_Rendering_Preset_from_Precision_and_Method(
					render_precision,
					render_method
			);
			if (render_preset == Render_Preset_Unknown) { return false; }
			return validate_Rendering_Preset(render_preset);
		}

		bool Render_Configurator::validate_Rendering_Precision(
			Rendering_Precision render_precision
		) const {
			switch (render_precision) {
				case Render_Precision_Float16:
					return (GPU_Float16_Enabled || CPU_Float16_Enabled || CPU_AVX512_FP16_Enabled)
						? true : false;
				case Render_Precision_Float32:
					return true; // Always supported
				case Render_Precision_Float64:
					return true; // Always supported
				case Render_Precision_Float80:
					return (CPU_Float80_Enabled) ? true : false;
				case Render_Precision_Float128:
					return (CPU_Float128_Enabled) ? true : false;
				default:
					return false;
			}
		}
		bool Render_Configurator::validate_Rendering_Method(
			Rendering_Method render_method
		) const {
			switch (render_method) {
				case Render_Method_GPU:
					return (GPU_Float16_Enabled || GPU_Float32_Enabled || GPU_Float64_Enabled) ? true : false;
				case Render_Method_CPU_Generic:
					return true; // Always supported
				case Render_Method_CPU_SSE2:
					return (CPU_SSE2_Enabled) ? true : false;
				case Render_Method_CPU_AVX:
					return (CPU_AVX_Enabled) ? true : false;
				case Render_Method_CPU_AVX512:
					return (CPU_AVX512_F_Enabled) ? true : false;
				default:
					return false;
			}
		}

	/* Calculation */

		void Render_Configurator::calculate_Rendering_Precision_and_Method(
			const Rendering_Precision input_precision,
			Rendering_Precision& output_precision,
			Rendering_Method& output_method
		) const {
			output_precision = input_precision;
			bool repeatLoop = false;
			do {
				repeatLoop = false;
				switch (output_precision) {
					case Render_Precision_Automatic: {
						resolve_Automatic_Render_Precision(output_precision);
						repeatLoop = true;
						continue;
					}
					case Render_Precision_Float16: {
						if (GPU_Float16_Enabled) { output_method = Render_Method_GPU; }
						if (CPU_AVX512_FP16_Enabled) { output_method = Render_Method_CPU_AVX512; }
						output_precision = Render_Precision_Float32;
						repeatLoop = true;
						continue;
					}
					case Render_Precision_Float32: {
						if (GPU_Float32_Enabled) { output_method = Render_Method_GPU; return; }
						if (CPU_AVX512_F_Enabled) { output_method = Render_Method_CPU_AVX512; return; }
						if (CPU_AVX_Enabled) { output_method = Render_Method_CPU_AVX; return; }
						if (CPU_SSE2_Enabled) { output_method = Render_Method_CPU_SSE2; return; }
						output_method = Render_Method_CPU_Generic; return;
					}
					case Render_Precision_Float64: {
						if (GPU_Float64_Enabled) { output_method = Render_Method_GPU; return; }
						if (CPU_AVX512_F_Enabled) { output_method = Render_Method_CPU_AVX512; return; }
						if (CPU_AVX_Enabled) { output_method = Render_Method_CPU_AVX; return; }
						if (CPU_SSE2_Enabled) { output_method = Render_Method_CPU_SSE2; return; }
						output_method = Render_Method_CPU_Generic; return; 
					}
					case Render_Precision_Float80: {
						if (CPU_Float80_Enabled) { output_method = Render_Method_CPU_Generic; return; }
						output_precision = Render_Precision_Float64;
						repeatLoop = true;
						continue;
					}
					case Render_Precision_Float128: {
						if (CPU_Float128_Enabled) { output_method = Render_Method_CPU_Generic; return; }
						output_precision = Render_Precision_Float80;
						repeatLoop = true;
						continue;
					}
				}
			} while (repeatLoop == true);
			output_precision = Render_Precision_Unknown;
			output_method = Render_Method_Unknown;
		}

		void Render_Configurator::calculate_Rendering_Precision_and_Method(
			const Rendering_Precision input_precision,
			const Rendering_Method input_method,
			Rendering_Precision& output_precision,
			Rendering_Method& output_method
		) const {
			if (
				(input_method == Render_Method_Automatic) ||
				(validate_Rendering_Method(input_method) == false)
			 ) {
				calculate_Rendering_Precision_and_Method(
					input_precision,
					output_precision,
					output_method
				);
				return;
			}
			if (
				validate_Rendering_Precision_and_Method(input_precision, input_method) == true
			) { return; }
			// Rendering_Precision current_precision = input_precision;
			// // If render_precision and render_method failed validation
			// bool repeatLoop = false;
			// do {
			// 	// This loop will try to find the closest precision that uses a given method
			// 	repeatLoop = false;
			// 	calculate_Rendering_Precision_and_Method(
			// 		current_precision,
			// 		output_precision,
			// 		output_method
			// 	);
			// 	switch (output_precision) {
			// 		case Render_Precision_Automatic:
			// 			continue; // Gives up
			// 		case Render_Precision_Float16: {
			// 			if (output_method == input_method) { return; }
			// 			current_precision = Render_Precision_Float32;
			// 			repeatLoop = true;
			// 			continue;
			// 		}
			// 		case Render_Precision_Float32:
			// 			continue; // Gives up
			// 		case Render_Precision_Float64:
			// 			continue; // Gives up
			// 		case Render_Precision_Float80: {
			// 			if (output_method == input_method) { return; }
			// 			current_precision = Render_Precision_Float64;
			// 			repeatLoop = true;
			// 			continue;
			// 		}
			// 		case Render_Precision_Float128: {
			// 			if (output_method == input_method) { return; }
			// 			current_precision = Render_Precision_Float80;
			// 			repeatLoop = true;
			// 			continue;
			// 		}
			// 	}
			// } while (repeatLoop == true);

			// Proitizes searching the most common precisions first
			constexpr Rendering_Precision Render_Precision_Attempt_Order[] = {
				Render_Precision_Float64,
				Render_Precision_Float32,
				Render_Precision_Float80,
				Render_Precision_Float128,
				Render_Precision_Float16
			};
			// Attempts to find any precision that supports the render_method
			for (size_t i = 0; i < sizeof(Render_Precision_Attempt_Order) / sizeof(Rendering_Precision); i++) {
				if (validate_Rendering_Precision_and_Method(Render_Precision_Attempt_Order[i], input_method)) {
					output_precision = Render_Precision_Attempt_Order[i];
					output_method = input_method;
					return; // Match was found
				}
			}
			// If no results are found
			calculate_Rendering_Precision_and_Method(
				input_precision,
				output_precision,
				output_method
			);
		}

	/* Read Configuartion */

		Rendering_Precision Render_Configurator::get_Render_Precision() const {
			return Render_Precision;
		}
		Rendering_Method Render_Configurator::get_Render_Method() const {
			return Render_Method;
		}
		Rendering_Preset Render_Configurator::get_Render_Preset() const {
			return get_Rendering_Preset_from_Precision_and_Method(
				Render_Precision, Render_Method
			);
		}
		
		bool Render_Configurator::current_Render_Method_GPU() const {
			return (Render_Method == Render_Method_GPU) ? true : false;
		}
		bool Render_Configurator::current_Render_Method_CPU() const {
			return (Render_Method != Render_Method_GPU) ? true : false;
		}

		size_t Render_Configurator::get_Float_Size(Rendering_Precision render_precision) const {
			switch (render_precision) {
				case Render_Precision_Float16:
					return 16;
				case Render_Precision_Float32:
					return 32;
				case Render_Precision_Float64:
					return 64;
				case Render_Precision_Float80:
					return 80;
				case Render_Precision_Float128:
					return 128;
				default:
					return 0;
			}
		};
		size_t Render_Configurator::get_Float_Mantissa_Size(Rendering_Precision render_precision) const {
			switch (render_precision) {
				case Render_Precision_Float16:
					return 10;
				case Render_Precision_Float32:
					return 23;
				case Render_Precision_Float64:
					return 52;
				case Render_Precision_Float80:
					return 63;
				case Render_Precision_Float128:
					return 112;
				default:
					return 0;
			}
		};
		size_t Render_Configurator::get_Float_Exponent_Size(Rendering_Precision render_precision) const {
			switch (render_precision) {
				case Render_Precision_Float16:
					return 5;
				case Render_Precision_Float32:
					return 8;
				case Render_Precision_Float64:
					return 11;
				case Render_Precision_Float80:
					return 15;
				case Render_Precision_Float128:
					return 15;
				default:
					return 0;
			}
		};

		size_t Render_Configurator::get_Current_Float_Size() const { return get_Float_Size(Render_Precision); };
		size_t Render_Configurator::get_Current_Float_Mantissa_Size() const { return get_Float_Mantissa_Size(Render_Precision); };
		size_t Render_Configurator::get_Current_Float_Exponent_Size() const { return get_Float_Exponent_Size(Render_Precision); };
		
		void Render_Configurator::print_Rendering_Configuration() const {
			printf(
				"\nRendering Configuration: %s | %s",
				Rendering_Precision_Text[Render_Precision],
				Rendering_Method_Text[Render_Method]
			);
		}

		void Render_Configurator::print_Available_Rendering_Presets() const {
			printf("\nAvailable Rendering Methods:\n\t");
			if (GPU_Float16_Enabled) { printf("GPU_Float16 "); }
			if (GPU_Float32_Enabled) { printf("GPU_Float32 "); }
			if (GPU_Float64_Enabled) { printf("GPU_Float64 "); }
			if (CPU_Float16_Enabled) { printf("CPU_Float16 "); }
			printf("CPU_Float32 "); // Always Available
			printf("CPU_Float64 "); // Always Available
			if (CPU_Float80_Enabled) { printf("CPU_Float80 "); }
			if (CPU_Float128_Enabled) { printf("CPU_Float128 "); }
			if (CPU_SSE2_Enabled) {
				printf("CPU_SSE2_Float32 ");
				printf("CPU_SSE2_Float64 ");
			}
			if (CPU_AVX_Enabled) {
				printf("CPU_AVX_Float32 ");
				printf("CPU_AVX_Float64 ");
			}
			if (CPU_AVX512_FP16_Enabled) { printf("CPU_AVX512_Float16 "); }
			if (CPU_AVX512_F_Enabled) {
				printf("CPU_AVX512_Float32 ");
				printf("CPU_AVX512_Float64 ");
			}
			printf("\n");
		}

	/* Change Configuration */

		bool Render_Configurator::suggest_Render_Precision_and_Method(
			const Rendering_Precision render_precision,
			const Rendering_Method render_method
		) {
			if (validate_Rendering_Precision_and_Method(
					render_precision, render_method
			)) {
				if (
					(Render_Precision == render_precision) &&
					(Render_Method == render_method)
				) { return false; }
				Render_Precision = render_precision;
				Render_Method = render_method;
				return true;
			}

			// If render_precision and render_method failed validation
			Rendering_Precision temp_precision = render_precision;
			Rendering_Method temp_method = render_method;
			calculate_Rendering_Precision_and_Method(
				temp_precision,
				temp_precision,
				temp_method
			);

			bool changes_detected = (
				(temp_precision != Render_Precision) &&
				(temp_method != Render_Method)
			) ? true : false;
			Render_Precision = temp_precision;
			Render_Method = temp_method;
			return changes_detected;
		}

		bool Render_Configurator::suggest_Render_Precision(
			const Rendering_Precision render_precision
		) {
			return suggest_Render_Precision_and_Method(
				render_precision, Render_Method
			);
		}

		bool Render_Configurator::suggest_Render_Method(
			const Rendering_Method render_method
		) {
			if (validate_Rendering_Precision_and_Method(
					Render_Precision, render_method
			)) {
				if (Render_Method == render_method) { return false; }
				Render_Method = render_method;
				return true;
			}

			// If Render_Precision and render_method failed validation
			Rendering_Precision temp_precision = Render_Precision;
			Rendering_Method temp_method = render_method;
			calculate_Rendering_Precision_and_Method(
				temp_precision,
				temp_method,
				temp_precision,
				temp_method
			);
			bool changes_detected = (
				(temp_precision != Render_Precision) &&
				(temp_method != Render_Method)
			) ? true : false;
			Render_Precision = temp_precision;
			Render_Method = temp_method;
			return changes_detected;
		}

		bool Render_Configurator::suggest_Render_Preset(
			const Rendering_Preset render_preset
		) {
			Rendering_Precision render_precision;
			Rendering_Method render_method;
			get_Rendering_Precision_and_Method_from_Preset(
				render_preset,
				render_precision,
				render_method
			);
			return suggest_Render_Precision_and_Method(
				render_precision, render_method
			);
		}

		bool Render_Configurator::update_Precision(fp64 zoom) {
			Rendering_Precision temp_precision = Render_Precision;
			Rendering_Method temp_method = Render_Method;

			constexpr fp64 zoom_offset = 2.3;
			constexpr fp64 zoom_float16  =  1.8 - zoom_offset;
			constexpr fp64 zoom_float32  =  5.7 - zoom_offset;
			constexpr fp64 zoom_float64  = 14.4 - zoom_offset;
			constexpr fp64 zoom_float80  = 17.7 - zoom_offset;
			__attribute__((unused)) constexpr fp64 zoom_float128 = 32.5 - zoom_offset;

			if (zoom < zoom_float16) {
				calculate_Rendering_Precision_and_Method(Render_Precision_Float16, Render_Precision, Render_Method);
			} else if (zoom < zoom_float32) {
				calculate_Rendering_Precision_and_Method(Render_Precision_Float32, Render_Precision, Render_Method);
			} else if (zoom < zoom_float64) {
				calculate_Rendering_Precision_and_Method(Render_Precision_Float64, Render_Precision, Render_Method);
			} else if (zoom < zoom_float80) {
				calculate_Rendering_Precision_and_Method(Render_Precision_Float80, Render_Precision, Render_Method);
			} else {
				calculate_Rendering_Precision_and_Method(Render_Precision_Float128, Render_Precision, Render_Method);
			}
			
			bool changes_detected = (
				(temp_precision != Render_Precision) ||
				(temp_method != Render_Method)
			) ? true : false;
			Render_Precision = temp_precision;
			Render_Method = temp_method;
			return changes_detected;
		}

/* private */

	/* Resolve Automatic */

		void Render_Configurator::resolve_Automatic_Render_Precision(
			Rendering_Configuration::Rendering_Precision& render_precision
		) const {
			if (render_precision != Render_Precision_Automatic) {
				return;
			}
			if (GPU_Float32_Enabled) {
				render_precision = Render_Precision_Float32;
			}
			render_precision = Render_Precision_Float64;
		}

		void Render_Configurator::resolve_Automatic_Render_Precision_and_Method(
			Rendering_Precision& render_precision,
			Rendering_Method& render_method
		) const {
			calculate_Rendering_Precision_and_Method(
				render_precision,
				render_precision,
				render_method
			);
		}

		void Render_Configurator::resolve_Automatic_Render_Preset(
			Rendering_Configuration::Rendering_Preset& render_preset
		) const {
			Rendering_Precision render_precision;
			Rendering_Method render_method;
			get_Rendering_Precision_and_Method_from_Preset(
				render_preset,
				render_precision,
				render_method
			);
			resolve_Automatic_Render_Precision_and_Method(
				render_precision, render_method
			);
		}