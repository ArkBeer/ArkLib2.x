#pragma once
#include<chrono>
#include<thread>
namespace Ark {
	class Frame_Rate {
		std::chrono::time_point<std::chrono::steady_clock> Start_Time;
		double passed() {
			auto end = std::chrono::steady_clock::now();
			auto passed = end - Start_Time;
			return std::chrono::duration_cast<std::chrono::duration<double>>(passed).count();
		}
		double needed() {
			return passed() - static_cast<double>(1 / Fps);
		}
	public:
		double Fps;
		Frame_Rate() :Frame_Rate(60.0) {}
		Frame_Rate(double fps) :Fps(fps), Start_Time(std::chrono::steady_clock::now()) {}
		void Count() {
			while (needed() < 0)std::this_thread::sleep_for(std::chrono::duration<int>(static_cast<int>(1 / Fps)));
			Start_Time = std::chrono::steady_clock::now();
		}
		template<typename T>
		auto Duration(std::chrono::time_point<std::chrono::steady_clock> s) {
			return std::chrono::duration_cast<std::chrono::duration<T>>(std::chrono::steady_clock::now() - s).count();
		}

	};
}