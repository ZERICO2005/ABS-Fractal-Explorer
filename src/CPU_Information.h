/*
**	Author: zerico2005 (2023-2024)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#include <cstddef>

struct Supported_SSE_Family_Instruction {
	bool SSE;
	bool SSE2;
	bool SSE3;
	bool SSSE3;
	bool SSE4_1;
	bool SSE4_2;
	bool SSE4a;
};

struct Supported_AVX_Family_Instruction {
	bool AVX;
	bool F16C;
	bool FMA;
	bool AVX2;
	bool AVX_VNNI;
	bool AVX_VNNI_INT8;
	bool AVX_NE_CONVERT;
	bool AVX_IFMA;
	// bool AVX_VNNI_INT16;
	// bool SHA512;
	// bool SM3;
	// bool SM4;
};

struct Supported_AVX512_Family_Instruction {
	bool AVX512_F;
	bool AVX512_BW;
	bool AVX512_CD;
	bool AVX512_DQ;
	bool AVX512_IFMA52;
	bool AVX512_VL;
	bool AVX512_VPOPCNTDQ;
	bool AVX512_BF16;
	bool AVX512_BITALG;
	bool AVX512_VBMI;
	bool AVX512_VBMI2;
	bool AVX512_VNNI;
	bool AVX512_VP2INTERSECT;
	bool AVX512_FP16;
};

struct Supported_AMX_Family_Instruction {
	bool AMX_BF16;
	bool AMX_INT8;
	bool AMX_TILE;
	bool AMX_FP16;
	bool AMX_COMPLEX;
};

struct Supported_CPU_Instruction {
	Supported_SSE_Family_Instruction SSE_Family;
	Supported_AVX_Family_Instruction AVX_Family;
	Supported_AVX512_Family_Instruction AVX512_Family;
	Supported_AMX_Family_Instruction AMX_Family;
	bool MMX;
};

void get_CPU_Supported_Instruction_Set_List(Supported_CPU_Instruction& instruction_list);

/* Print Instruction List */

	// Formatted to fit on an 80 column screen
	void print_Supported_SSE_Family_Instruction   (const Supported_CPU_Instruction& instruction_list);
	
	// Formatted to fit on an 80 column screen
	void print_Supported_AVX_Family_Instruction   (const Supported_CPU_Instruction& instruction_list);
	
	// Formatted to fit on an 80 column screen
	void print_Supported_AVX512_Family_Instruction(const Supported_CPU_Instruction& instruction_list);
	
	// Formatted to fit on an 80 column screen
	void print_Supported_AMX_Family_Instruction   (const Supported_CPU_Instruction& instruction_list);
	
	// Formatted to fit on an 80 column screen
	void print_Supported_Other_Instruction        (const Supported_CPU_Instruction& instruction_list);

	// Prints all available CPU instruction sets, formatted to fit on an 80 column screen.
	void print_Supported_CPU_Instruction(const Supported_CPU_Instruction& instruction_list);