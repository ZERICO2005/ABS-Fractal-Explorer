#ifndef FNV1A_HASH_HPP
#define FNV1A_HASH_HPP

#include <cstddef>
#include <cstdint>

/**
* @brief Used for obtaining hashes of integers, structs, etc.
* @warning NOT A CRYPTOGRAPHIC HASH FUNCTION
* (https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function)
*/ 
class FNV1A_Hash {
	private:
		uint64_t hash;
		static constexpr uint64_t fnv1a_Offset_Basis = 0xCBF29CE484222325; // FNV offset basis (64bit)
		static constexpr uint64_t fnv1a_Prime = 0x100000001B3; // FNV prime (64bit)
	
	public:
	/* Basic Operations */

	inline void reset(void) {
		hash = fnv1a_Prime;
	}

	inline uint64_t get_hash(void) {
		return hash;
	}

	inline void set_hash(const uint64_t& hash_value) {
		hash = hash_value;
	}

	inline void append(const void* data, size_t size) {
		for (size_t i = 0; i < size; i++) {
			hash ^= ((const uint8_t*)(data))[i];
			hash *= fnv1a_Prime;
		}
	}

	/* Constructors */

	FNV1A_Hash() {
		reset();
	}

	FNV1A_Hash(uint64_t hash_value) {
		set_hash(hash_value);
	}

	/* Operator Overloads */

	template <typename T>
	inline FNV1A_Hash& operator+=(const T& value) {
		this->append(&value, sizeof(value));
		return *this;
	}
};

#endif /* FNV1A_HASH_HPP */