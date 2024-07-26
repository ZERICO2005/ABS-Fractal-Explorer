/*
**	Author: zerico2005 (2023-2024)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#include <cstddef>
#include <cstdint>
#include <cstring>

#include <cstdio>

#include <cpuid.h>

#include "CPU_Information.h"

// The commented out sections of code are to ensure compatibility with GCC and Clang

// Debug function
inline void print_Registers(unsigned int info[4]) {
	printf("Registers: %08X %08X %08X %08X\n", info[0], info[1], info[2], info[3]);
}

inline void cpuid_func(unsigned int info[4], unsigned int InfoType) {
	__cpuid_count(InfoType, 0, info[0], info[1], info[2], info[3]);
}

struct CPU_Signature_Info {
	const char* CPU_Signature;
	unsigned int REG_EBX;
	unsigned int REG_ECX;
	unsigned int REG_EDX;
};

const CPU_Signature_Info CPU_Signature_List[] = {
	{"Unknown CPU ", 0x00000000           , 0x00000000           , 0x00000000           },
	{"AuthenticAMD", signature_AMD_ebx    , signature_AMD_ecx    , signature_AMD_edx    },
	{"CentaurHauls", signature_CENTAUR_ebx, signature_CENTAUR_ecx, signature_CENTAUR_edx},
	{"CyrixInstead", signature_CYRIX_ebx  , signature_CYRIX_ecx  , signature_CYRIX_edx  },
	{"GenuineIntel", signature_INTEL_ebx  , signature_INTEL_ecx  , signature_INTEL_edx  },
	{"TransmetaCPU", signature_TM1_ebx    , signature_TM1_ecx    , signature_TM1_edx    },
	{"GenuineTMx86", signature_TM2_ebx    , signature_TM2_ecx    , signature_TM2_edx    },
	{"Geode by NSC", signature_NSC_ebx    , signature_NSC_ecx    , signature_NSC_edx    },
	{"NexGenDriven", signature_NEXGEN_ebx , signature_NEXGEN_ecx , signature_NEXGEN_edx },
	{"RiseRiseRise", signature_RISE_ebx   , signature_RISE_ecx   , signature_RISE_edx   },
	{"SiS SiS SiS ", signature_SIS_ebx    , signature_SIS_ecx    , signature_SIS_edx    },
	{"UMC UMC UMC ", signature_UMC_ebx    , signature_UMC_ecx    , signature_UMC_edx    },
	{"VIA VIA VIA ", signature_VIA_ebx    , signature_VIA_ecx    , signature_VIA_edx    },
	{"Vortex86 SoC", signature_VORTEX_ebx , signature_VORTEX_ecx , signature_VORTEX_edx }
};

const char* get_CPU_Name(unsigned int REG_EBX, unsigned int REG_ECX, unsigned int REG_EDX) {
	for (size_t i = 0; i < sizeof(CPU_Signature_List) / sizeof(CPU_Signature_Info); i++) {
		if (
			(CPU_Signature_List[i].REG_EBX == REG_EBX) &&
			(CPU_Signature_List[i].REG_ECX == REG_ECX) &&
			(CPU_Signature_List[i].REG_EDX == REG_EDX)
		) {
			return CPU_Signature_List[i].CPU_Signature;
		}
	}
	printf("Unknown CPU Signature: %08X %08X %08X\n", REG_EBX, REG_ECX, REG_EDX);
	return CPU_Signature_List[0].CPU_Signature;
}

void get_Supported_CPU_Instruction(Supported_CPU_Instruction& instruction_list) {
	memset(&instruction_list, 0, sizeof(instruction_list));
	enum Registers {
		EAX, EBX, ECX, EDX
	};
	unsigned int info[4] = {0x00000000, 0x00000000, 0x00000000, 0x00000000};
	cpuid_func(info, 0x00000000);
	instruction_list.CPU_Signature = get_CPU_Name(info[EBX], info[ECX], info[EDX]);
	const unsigned int Feature_ID = info[EAX];
	cpuid_func(info, 0x80000000);
	const unsigned Extended_Feature_ID = info[EAX];
	
	if (Feature_ID >= 0x00000001) {
		cpuid_func(info, 0x00000001);
		/* SSE Family */
			instruction_list.SSE_Family.SSE    = (info[EDX] & bit_SSE   ) ? true : false;
			instruction_list.SSE_Family.SSE2   = (info[EDX] & bit_SSE2  ) ? true : false;
			instruction_list.SSE_Family.SSE3   = (info[ECX] & bit_SSE3  ) ? true : false;
			instruction_list.SSE_Family.SSSE3  = (info[ECX] & bit_SSSE3 ) ? true : false;
			instruction_list.SSE_Family.SSE4_1 = (info[ECX] & bit_SSE4_1) ? true : false;
			instruction_list.SSE_Family.SSE4_2 = (info[ECX] & bit_SSE4_2) ? true : false;
		/* AVX Family */
			instruction_list.AVX_Family.AVX            = (info[ECX] & bit_AVX         ) ? true : false;
			instruction_list.AVX_Family.F16C           = (info[ECX] & bit_F16C        ) ? true : false;
			instruction_list.AVX_Family.FMA            = (info[ECX] & bit_FMA         ) ? true : false;
			instruction_list.AVX_Family.AVX_VNNI       = (info[EAX] & bit_AVXVNNI     ) ? true : false;
			instruction_list.AVX_Family.AVX_IFMA       = (info[EAX] & bit_AVXIFMA     ) ? true : false;
			instruction_list.AVX_Family.AVX_VNNI_INT8  = (info[EDX] & bit_AVXVNNIINT8 ) ? true : false;
			instruction_list.AVX_Family.AVX_NE_CONVERT = (info[EDX] & bit_AVXNECONVERT) ? true : false;
		/* AVX512 Family */
			instruction_list.AVX512_Family.AVX512_BF16 = (info[EAX] & bit_AVX512BF16) ? true : false;
		/* AMX Family */
			// instruction_list.AMX_Family.AMX_FP16 = (info[EAX] & bit_AMX_FP16) ? true : false;
		/* Others */
			instruction_list.MMX = (info[EDX] & bit_MMX) ? true : false;
	}
	if (Feature_ID >= 0x00000007) {
		cpuid_func(info, 0x00000007);
		/* AVX Family */
			instruction_list.AVX_Family.AVX2  = (info[EBX] & bit_AVX2) ? true : false;
		/* AVX512 Family */
			instruction_list.AVX512_Family.AVX512_F            = (info[EBX] & bit_AVX512F           ) ? true : false;
			instruction_list.AVX512_Family.AVX512_BW           = (info[EBX] & bit_AVX512BW          ) ? true : false;
			instruction_list.AVX512_Family.AVX512_CD           = (info[EBX] & bit_AVX512CD          ) ? true : false;
			instruction_list.AVX512_Family.AVX512_DQ           = (info[EBX] & bit_AVX512DQ          ) ? true : false;
			instruction_list.AVX512_Family.AVX512_IFMA52       = (info[EBX] & bit_AVX512IFMA        ) ? true : false;
			instruction_list.AVX512_Family.AVX512_VL           = (info[EBX] & bit_AVX512VL          ) ? true : false;
			instruction_list.AVX512_Family.AVX512_VPOPCNTDQ    = (info[ECX] & bit_AVX512VPOPCNTDQ   ) ? true : false;
			instruction_list.AVX512_Family.AVX512_BITALG       = (info[ECX] & bit_AVX512BITALG      ) ? true : false;
			instruction_list.AVX512_Family.AVX512_VBMI         = (info[ECX] & bit_AVX512VBMI        ) ? true : false;
			instruction_list.AVX512_Family.AVX512_VBMI2        = (info[ECX] & bit_AVX512VBMI2       ) ? true : false;
			instruction_list.AVX512_Family.AVX512_VNNI         = (info[ECX] & bit_AVX512VNNI        ) ? true : false;
			// instruction_list.AVX512_Family.AVX512_VP2INTERSECT = (info[EDX] & bit_AVX512VP2INTERSECT) ? true : false;
			instruction_list.AVX512_Family.AVX512_FP16         = (info[EDX] & bit_AVX512FP16        ) ? true : false;
		/* AMX Family */
			// instruction_list.AMX_Family.AMX_BF16    = (info[EDX] & bit_AMX_BF16   ) ? true : false;
			// instruction_list.AMX_Family.AMX_INT8    = (info[EDX] & bit_AMX_INT8   ) ? true : false;
			// instruction_list.AMX_Family.AMX_TILE    = (info[EDX] & bit_AMX_TILE   ) ? true : false;
			// instruction_list.AMX_Family.AMX_COMPLEX = (info[EDX] & bit_AMX_COMPLEX) ? true : false;
	}
	if (Extended_Feature_ID >= 0x80000001) {
		/* SSE Family */
		cpuid_func(info, 0x80000001);
		instruction_list.SSE_Family.SSE4a = (info[ECX] & bit_SSE4a) ? true : false;
	}
}

static inline size_t count_if_supported(const bool& input) {
	return (input == true) ? 1 : 0;
}

size_t count_Supported_CPU_Instruction(const Supported_CPU_Instruction& instruction_list) {
	size_t count = 0;
	/* SSE Family */
		count += count_if_supported(instruction_list.SSE_Family.SSE);
		count += count_if_supported(instruction_list.SSE_Family.SSE2);
		count += count_if_supported(instruction_list.SSE_Family.SSE3);
		count += count_if_supported(instruction_list.SSE_Family.SSSE3);
		count += count_if_supported(instruction_list.SSE_Family.SSE4_1);
		count += count_if_supported(instruction_list.SSE_Family.SSE4_2);
		count += count_if_supported(instruction_list.SSE_Family.SSE4a);
	/* AVX Family */
		count += count_if_supported(instruction_list.AVX_Family.AVX);
		count += count_if_supported(instruction_list.AVX_Family.F16C);
		count += count_if_supported(instruction_list.AVX_Family.FMA);
		count += count_if_supported(instruction_list.AVX_Family.AVX2);
		count += count_if_supported(instruction_list.AVX_Family.AVX_VNNI);
		count += count_if_supported(instruction_list.AVX_Family.AVX_VNNI_INT8);
		count += count_if_supported(instruction_list.AVX_Family.AVX_NE_CONVERT);
		count += count_if_supported(instruction_list.AVX_Family.AVX_IFMA);
		// count += count_if_supported(instruction_list.AVX_Family.AVX_VNNI_INT16);
		// count += count_if_supported(instruction_list.AVX_Family.SHA512);
		// count += count_if_supported(instruction_list.AVX_Family.SM3);
		// count += count_if_supported(instruction_list.AVX_Family.SM4);
	/* AVX512 Family */
		count += count_if_supported(instruction_list.AVX512_Family.AVX512_F);
		count += count_if_supported(instruction_list.AVX512_Family.AVX512_BW);
		count += count_if_supported(instruction_list.AVX512_Family.AVX512_CD);
		count += count_if_supported(instruction_list.AVX512_Family.AVX512_DQ);
		count += count_if_supported(instruction_list.AVX512_Family.AVX512_IFMA52);
		count += count_if_supported(instruction_list.AVX512_Family.AVX512_VL);
		count += count_if_supported(instruction_list.AVX512_Family.AVX512_VPOPCNTDQ);
		count += count_if_supported(instruction_list.AVX512_Family.AVX512_BF16);
		count += count_if_supported(instruction_list.AVX512_Family.AVX512_BITALG);
		count += count_if_supported(instruction_list.AVX512_Family.AVX512_VBMI);
		count += count_if_supported(instruction_list.AVX512_Family.AVX512_VBMI2);
		count += count_if_supported(instruction_list.AVX512_Family.AVX512_VNNI);
		// count += count_if_supported(instruction_list.AVX512_Family.AVX512_VP2INTERSECT);
		count += count_if_supported(instruction_list.AVX512_Family.AVX512_FP16);
	/* AMX Family */
		// count += count_if_supported(instruction_list.AMX_Family.AMX_BF16);
		// count += count_if_supported(instruction_list.AMX_Family.AMX_INT8);
		// count += count_if_supported(instruction_list.AMX_Family.AMX_TILE);
		// count += count_if_supported(instruction_list.AMX_Family.AMX_FP16);
		// count += count_if_supported(instruction_list.AMX_Family.AMX_COMPLEX);
	/* Others */
		count += count_if_supported(instruction_list.MMX);

	return count;
}

/* Print Instruction List */

	constexpr inline const char* bool_Text(const bool& b) { return b ? "true" : "false"; }

	void print_Supported_SSE_Family_Instruction(const Supported_CPU_Instruction& instruction_list) {
		const Supported_SSE_Family_Instruction& SSE_Family = instruction_list.SSE_Family;
		printf("\n");
		printf(
			"SSE Family:\n"\
			"\tSSE   | SSE2  | SSE3  | SSSE3 | SSE4.1 | SSE4.2 | SSE4a\n"\
			"\t%-5s | %-5s | %-5s | %-5s | %-5s  | %-5s  | %-5s\n",
			bool_Text(SSE_Family.SSE), bool_Text(SSE_Family.SSE2),
			bool_Text(SSE_Family.SSE3), bool_Text(SSE_Family.SSSE3),
			bool_Text(SSE_Family.SSE4_1), bool_Text(SSE_Family.SSE4_1),
			bool_Text(SSE_Family.SSE4a)
		);
	}

	void print_Supported_AVX_Family_Instruction(const Supported_CPU_Instruction& instruction_list) {
		const Supported_AVX_Family_Instruction& AVX_Family = instruction_list.AVX_Family;
		printf("\n");
		printf(
			"AVX Family:\n"\
			"\tAVX      | F16C          | FMA            | AVX2\n"\
			"\t%-5s    | %-5s         | %-5s          | %-5s\n",
			bool_Text(AVX_Family.AVX), bool_Text(AVX_Family.F16C),
			bool_Text(AVX_Family.FMA), bool_Text(AVX_Family.AVX2)
		);
		printf(
			"\t====================================================\n"
		);
		printf(
			"\tAVX_VNNI | AVX_VNNI_INT8 | AVX_NE_CONVERT | AVX_IFMA\n"\
			"\t%-5s    | %-5s         | %-5s          | %-5s\n",
			bool_Text(AVX_Family.AVX_VNNI), bool_Text(AVX_Family.AVX_VNNI_INT8),
			bool_Text(AVX_Family.AVX_NE_CONVERT), bool_Text(AVX_Family.AVX_IFMA)
		);
	}

	void print_Supported_AVX512_Family_Instruction(const Supported_CPU_Instruction& instruction_list) {
		const Supported_AVX512_Family_Instruction& AVX512_Family = instruction_list.AVX512_Family;
		printf("\n");
		/*
		printf(
			"AVX512 Family:\n"\
			"\tF     | BW     | CD    | DQ    | IFMA52 | VL           | VPOPCNTDQ\n"\
			"\t%-5s | %-5s  | %-5s | %-5s | %-5s  | %-5s        | %-5s\n",
			bool_Text(AVX512_Family.AVX512_F), bool_Text(AVX512_Family.AVX512_BW),
			bool_Text(AVX512_Family.AVX512_CD), bool_Text(AVX512_Family.AVX512_DQ),
			bool_Text(AVX512_Family.AVX512_IFMA52), bool_Text(AVX512_Family.AVX512_VL),
			bool_Text(AVX512_Family.AVX512_VPOPCNTDQ)
		);
		printf(
			"\t==================================================================\n"
		);
		printf(
			"\tBF16  | BITALG | VBMI  | VBMI2 | VNNI   | VP2INTERSECT | FP16\n"\
			"\t%-5s | %-5s  | %-5s | %-5s | %-5s  | %-5s        | %-5s\n",
			bool_Text(AVX512_Family.AVX512_BF16), bool_Text(AVX512_Family.AVX512_BITALG),
			bool_Text(AVX512_Family.AVX512_VBMI), bool_Text(AVX512_Family.AVX512_VBMI2),
			bool_Text(AVX512_Family.AVX512_VNNI), bool_Text(AVX512_Family.AVX512_VP2INTERSECT), 
			bool_Text(AVX512_Family.AVX512_FP16)
		);
		*/
		printf(
			"AVX512 Family:\n"\
			"\tF     | BW     | CD    | DQ    | IFMA52 | VL    | VPOPCNTDQ\n"\
			"\t%-5s | %-5s  | %-5s | %-5s | %-5s  | %-5s | %-5s\n",
			bool_Text(AVX512_Family.AVX512_F), bool_Text(AVX512_Family.AVX512_BW),
			bool_Text(AVX512_Family.AVX512_CD), bool_Text(AVX512_Family.AVX512_DQ),
			bool_Text(AVX512_Family.AVX512_IFMA52), bool_Text(AVX512_Family.AVX512_VL),
			bool_Text(AVX512_Family.AVX512_VPOPCNTDQ)
		);
		printf(
			"\t===========================================================\n"
		);
		printf(
			"\tBF16  | BITALG | VBMI  | VBMI2 | VNNI   | FP16\n"\
			"\t%-5s | %-5s  | %-5s | %-5s | %-5s  | %-5s\n",
			bool_Text(AVX512_Family.AVX512_BF16), bool_Text(AVX512_Family.AVX512_BITALG),
			bool_Text(AVX512_Family.AVX512_VBMI), bool_Text(AVX512_Family.AVX512_VBMI2),
			bool_Text(AVX512_Family.AVX512_VNNI), bool_Text(AVX512_Family.AVX512_FP16)
		);
	}

	/*
	void print_Supported_AMX_Family_Instruction(const Supported_CPU_Instruction& instruction_list) {
		const Supported_AMX_Family_Instruction& AMX_Family = instruction_list.AMX_Family;
		printf(
			"AMX Family:\n"\
			"\tBF16  | INT8  | TILE  | FP16  | COMPLEX\n"\
			"\t%-5s | %-5s | %-5s | %-5s | %-5s\n",
			bool_Text(AMX_Family.AMX_BF16), bool_Text(AMX_Family.AMX_INT8),
			bool_Text(AMX_Family.AMX_TILE), bool_Text(AMX_Family.AMX_FP16),
			bool_Text(AMX_Family.AMX_COMPLEX)
		);
		printf("\n");
	}
	*/

	void print_Supported_Other_Instruction(const Supported_CPU_Instruction& instruction_list) {
		printf("\n");
		printf(
			"Other Instruction Sets:\n"\
			"\tMMX\n"\
			"\t%-5s\n",
			bool_Text(instruction_list.MMX)
		);
	}

	void print_Supported_CPU_Instruction(const Supported_CPU_Instruction& instruction_list) {
		printf(
			"Supported CPU Instruction Sets: %zu\nCPU Signature: <%12s>\n",
			count_Supported_CPU_Instruction(instruction_list),
			instruction_list.CPU_Signature
		);
		print_Supported_SSE_Family_Instruction   (instruction_list);
		print_Supported_AVX_Family_Instruction   (instruction_list);
		print_Supported_AVX512_Family_Instruction(instruction_list);
		//print_Supported_AMX_Family_Instruction   (instruction_list);
		print_Supported_Other_Instruction        (instruction_list);
	}