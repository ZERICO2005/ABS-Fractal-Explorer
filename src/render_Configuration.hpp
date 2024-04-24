/*
**	Author: zerico2005 (2023-2024)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#ifndef RENDER_CONFIGURATION_HPP
#define RENDER_CONFIGURATION_HPP

#include "Common_Def.h"
#include "Program_Def.h"

namespace Rendering_Configuration {
	enum Rendering_Precision {
		Render_Precision_Automatic,
		Render_Precision_Unknown = Render_Precision_Automatic,
		Render_Precision_Float16,
		Render_Precision_Float32,
		Render_Precision_Float64,
		Render_Precision_Float80,
		Render_Precision_Float128
	};
	const char* const Rendering_Precision_Text[] = {
		"Automatic Precision",
		"(10^1.8) 16bit Float",
		"(10^5.7) 32bit Float",
		"(10^14.4) 64bit Float",
		"(10^17.7) 80bit Float",
		"(10^32.5) 128bit Float"
	};
	enum Rendering_Method {
		Render_Method_Automatic,
		Render_Method_Unknown = Render_Method_Automatic,
		Render_Method_GPU,
		Render_Method_CPU_Generic,
		Render_Method_CPU_SSE2,
		Render_Method_CPU_AVX,
		Render_Method_CPU_AVX512
	};
	const char* const Rendering_Method_Text[] = {
		"Automatic Render Method",
		"GPU Rendering",
		"CPU-Generic Rendering",
		"CPU-SSE2 Rendering",
		"CPU-AVX Rendering",
		"CPU-AVX512 Rendering"
	};
	enum Rendering_Preset {
		Render_Preset_Automatic,
		Render_Preset_Unknown = Render_Preset_Automatic,
		/* GPU Rendering */
		Render_Preset_GPU_Float16,
		Render_Preset_GPU_Float32,
		Render_Preset_GPU_Float64,
		/* CPU Generic Rendering */
		Render_Preset_CPU_Generic_Float16,
		Render_Preset_CPU_Generic_Float32,
		Render_Preset_CPU_Generic_Float64,
		Render_Preset_CPU_Generic_Float80,
		Render_Preset_CPU_Generic_Float128,
		/* CPU SSE2 Rendering */
		Render_Preset_CPU_SSE2_Float32,
		Render_Preset_CPU_SSE2_Float64,
		/* CPU AVX Rendering */
		Render_Preset_CPU_AVX_Float32,
		Render_Preset_CPU_AVX_Float64,
		/* CPU AVX512 Rendering */
		Render_Preset_CPU_AVX512_Float16,
		Render_Preset_CPU_AVX512_Float32,
		Render_Preset_CPU_AVX512_Float64
	};
	const char* const Rendering_Preset_Text[] = {
		"Automatic Render Preset",
		/* GPU Rendering */
		"(10^1.8) GPU 16bit Float",
		"(10^5.7) GPU 32bit Float",
		"(10^14.4) GPU 64bit Float",
		/* CPU Generic Rendering */
		"(10^1.8) CPU-Generic 16bit Float",
		"(10^5.7) CPU-Generic 32bit Float",
		"(10^14.4) CPU-Generic 64bit Float",
		"(10^17.7) CPU-Generic 80bit Float",
		"(10^32.5) CPU-Generic 128bit Float",
		/* CPU SSE2 Rendering */
		"(10^5.7) CPU-SSE2 32bit Float",
		"(10^14.4) CPU-SSE2 64bit Float",
		/* CPU AVX Rendering */
		"(10^5.7) CPU-AVX 32bit Float",
		"(10^14.4) CPU-AVX 64bit Float",
		/* CPU AVX512 Rendering */
		"(10^1.8) CPU-AVX512 16bit Float",
		"(10^5.7) CPU-AVX512 32bit Float",
		"(10^14.4) CPU-AVX512 64bit Float"
	};

	struct Rendering_Preset_Conversion {
		Rendering_Preset preset;
		Rendering_Precision precision;
		Rendering_Method method;
	};
	const Rendering_Preset_Conversion Rendering_Preset_Conversion_Table[] {
		{Render_Preset_Automatic           , Render_Precision_Automatic, Render_Method_Automatic  },
		/* GPU Rendering */
		{Render_Preset_GPU_Float16         , Render_Precision_Float16  , Render_Method_GPU        },
		{Render_Preset_GPU_Float32         , Render_Precision_Float32  , Render_Method_GPU        },
		{Render_Preset_GPU_Float64         , Render_Precision_Float64  , Render_Method_GPU        },
		/* CPU Generic Rendering */
		{Render_Preset_CPU_Generic_Float16 , Render_Precision_Float16  , Render_Method_CPU_Generic},
		{Render_Preset_CPU_Generic_Float32 , Render_Precision_Float32  , Render_Method_CPU_Generic},
		{Render_Preset_CPU_Generic_Float64 , Render_Precision_Float64  , Render_Method_CPU_Generic},
		{Render_Preset_CPU_Generic_Float80 , Render_Precision_Float80  , Render_Method_CPU_Generic},
		{Render_Preset_CPU_Generic_Float128, Render_Precision_Float128 , Render_Method_CPU_Generic},
		/* CPU SSE2 Rendering */
		{Render_Preset_CPU_SSE2_Float32    , Render_Precision_Float32  , Render_Method_CPU_SSE2   },
		{Render_Preset_CPU_SSE2_Float64    , Render_Precision_Float64  , Render_Method_CPU_SSE2   },
		/* CPU AVX Rendering */
		{Render_Preset_CPU_AVX_Float32     , Render_Precision_Float32  , Render_Method_CPU_AVX    },
		{Render_Preset_CPU_AVX_Float64     , Render_Precision_Float64  , Render_Method_CPU_AVX    },
		/* CPU AVX512 Rendering */
		{Render_Preset_CPU_AVX512_Float16  , Render_Precision_Float16  , Render_Method_CPU_AVX512 },
		{Render_Preset_CPU_AVX512_Float32  , Render_Precision_Float32  , Render_Method_CPU_AVX512 },
		{Render_Preset_CPU_AVX512_Float64  , Render_Precision_Float64  , Render_Method_CPU_AVX512 }
	};
};

// Returns the closest valid rendering method supported on the hardware
Rendering_Configuration::Rendering_Preset validate_Rendering_Preset(
	Rendering_Configuration::Rendering_Preset render_preset
);

class Render_Configurator {
	public:
	/* Setup */

		void set_GPU_Configuration(
			bool enable_GPU_Float16,
			bool enable_GPU_Float32,
			bool enable_GPU_Float64
		);

		void reset_Render_Configurator(
			bool enable_GPU_Float16 = false,
			bool enable_GPU_Float32 = false,
			bool enable_GPU_Float64 = false
		);

		Render_Configurator(
			bool enable_GPU_Float16 = false,
			bool enable_GPU_Float32 = false,
			bool enable_GPU_Float64 = false
		);
		
		Render_Configurator();
	
	/* Conversion */
		
		// Merges `precision` and `method` into a `preset`
		Rendering_Configuration::Rendering_Preset get_Rendering_Preset_from_Precision_and_Method(
			const Rendering_Configuration::Rendering_Precision render_precision,
			const Rendering_Configuration::Rendering_Method render_method
		) const;

		// Splits `preset` into a `precision` and `method`
		void get_Rendering_Precision_and_Method_from_Preset(
			const Rendering_Configuration::Rendering_Preset render_preset,
			Rendering_Configuration::Rendering_Precision& render_precision,
			Rendering_Configuration::Rendering_Method& render_method
		) const;

	/* Validation */

		bool validate_Rendering_Preset(
			const Rendering_Configuration::Rendering_Preset render_preset
		) const;

		bool validate_Rendering_Precision_and_Method(
			const Rendering_Configuration::Rendering_Precision render_precision,
			const Rendering_Configuration::Rendering_Method render_method
		) const;

	/* Calculation */
		
		void calculate_Rendering_Precision_and_Method(
			const Rendering_Configuration::Rendering_Precision input_precision,
			Rendering_Configuration::Rendering_Precision& output_precision,
			Rendering_Configuration::Rendering_Method& output_method
		) const;

	/* Read Configuration */

		Rendering_Configuration::Rendering_Preset get_Render_Preset() const;
		Rendering_Configuration::Rendering_Precision get_Render_Precision() const;
		Rendering_Configuration::Rendering_Method get_Render_Method() const;

		void print_Rendering_Configuration() const;
		void print_Available_Rendering_Presets() const;

	/* Change Configuration */

		/**
		 * @brief Selects the closest available rendering configuration.
		 * 
		 * @return `true` if the render configuration was changed.
		 */
		bool suggest_Render_Precision_and_Method(
			const Rendering_Configuration::Rendering_Precision render_precision,
			const Rendering_Configuration::Rendering_Method render_method
		);

		/**
		 * @brief Selects the closest available rendering configuration.
		 * 
		 * @return `true` if the render configuration was changed.
		 */
		bool suggest_Render_Precision(
			const Rendering_Configuration::Rendering_Precision render_precision
		);

		/**
		 * @brief Selects the closest available rendering configuration.
		 * 
		 * @return `true` if the render configuration was changed.
		 */
		bool suggest_Render_Method(
			const Rendering_Configuration::Rendering_Method render_method
		);

		/**
		 * @brief Selects the closest available rendering configuration.
		 * 
		 * @return `true` if the render configuration was changed.
		 */
		bool suggest_Render_Preset(
			const Rendering_Configuration::Rendering_Preset render_preset
		);

		/**
		 * @brief Determines the Rendering Precision based on the zoom value `(10^zoom)`.
		 * 
		 * @return `true` if the render configuration was changed.
		 */
		bool update_Precision(fp64 zoom);

	private:

		bool GPU_Float16_Enabled;
		bool GPU_Float32_Enabled;
		bool GPU_Float64_Enabled;

		bool CPU_Float16_Enabled;
		bool CPU_Float80_Enabled;
		bool CPU_Float128_Enabled;

		bool CPU_SSE2_Enabled;
		bool CPU_AVX_Enabled;
		bool CPU_AVX512_F_Enabled;
		bool CPU_AVX512_FP16_Enabled;

		Rendering_Configuration::Rendering_Precision Render_Precision;
		Rendering_Configuration::Rendering_Method Render_Method;
		Rendering_Configuration::Rendering_Preset Render_Preset;

	/* Resolve Automatic */

		// Sets render_precision to an actual value if it is set to `Render_Precision_Automatic`
		void resolve_Automatic_Render_Precision(
			Rendering_Configuration::Rendering_Precision& render_precision
		) const;

		// Sets render_precision and render_method to an actual value if they are set to Automatic
		void resolve_Automatic_Render_Precision_and_Method(
			Rendering_Configuration::Rendering_Precision& render_precision,
			Rendering_Configuration::Rendering_Method& render_method
		) const;

		// Sets render_preset to an actual value if it is set to `Render_Preset_Automatic`
		void resolve_Automatic_Render_Preset(
			Rendering_Configuration::Rendering_Preset& render_preset
		) const;
};

#endif /* RENDER_CONFIGURATION_HPP */