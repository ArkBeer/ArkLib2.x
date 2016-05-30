#pragma once
namespace Ark {
	class Random {
		std::mt19937 mt;
	public:
		Random() {
			std::random_device rnd;
			mt = std::mt19937(rnd());
		}
		const int Range(const int x, const int y) {
			if (x <= y) {
				std::uniform_int_distribution<> rand_(x, y);
				return rand_(mt);
			}
			else return -1;
		}

	};
}
