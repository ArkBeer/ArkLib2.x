#pragma once
#include<chrono>
#include<thread>
namespace Ark {
	class FrameRate {
		using timepoint = std::chrono::time_point < std::chrono::steady_clock >;
		timepoint StartTime;
		const double passed() {
			auto end = std::chrono::steady_clock::now();
			auto passed = end - StartTime;
			return std::chrono::duration_cast<std::chrono::duration<double>>(passed).count();
		}
		const double needed() {
			return passed() - static_cast<double>(1 / Fps);
		}
	public:
		double Fps;
		FrameRate() :FrameRate(60.0) {}
		FrameRate(double fps) :Fps(fps), StartTime(std::chrono::steady_clock::now()) {}
		void Count() {
			while (needed() < 0)std::this_thread::sleep_for(std::chrono::duration<int>(static_cast<int>(1 / Fps)));
			StartTime = std::chrono::steady_clock::now();
		}
		template<typename T>
		auto Duration(timepoint s) {
			return std::chrono::duration_cast<std::chrono::duration<T>>(std::chrono::steady_clock::now() - s).count();
		}
		template<typename T>
		auto perDuration(timepoint s) {
			return static_cast<T>(1/std::chrono::duration_cast<std::chrono::duration<T>>(std::chrono::steady_clock::now() - s).count());
		}

	};
}