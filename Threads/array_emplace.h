#pragma once

#include <algorithm>
#include <array>

template<typename T, size_t N, typename ...Args>
void emplace_fill(std::array<T, N>& arr, Args&&... args) {
	std::for_each(arr.begin(), arr.end(), [&](T& t) {
		t = T(std::forward<Args>(args)...);
	});
}
