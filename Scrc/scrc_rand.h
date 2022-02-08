#pragma once

#include "scrc_common.h"

namespace scrc
{

	//get value from the random generator
	//out range from the "start" to the "end"
	static uint32_t rand32(uint32_t start = 1, uint32_t end = (uint32_t)(-1))
	{

		//get random number
		std::uniform_int_distribution<uint32_t> range_distribution(start, end);
		uint64_t time = uint64_t(std::chrono::high_resolution_clock::now().time_since_epoch().count());

		std::mt19937 random_number_engine((uint32_t)(time ^ (time >> 32))); // pseudorandom number generator
		auto get_number = std::bind(range_distribution, random_number_engine);
		return get_number();
	}
}