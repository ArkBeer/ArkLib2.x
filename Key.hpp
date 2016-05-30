#pragma once
namespace Ark {
	class Key {
		std::array<bool, 256> key_arr;
	public:
		Key() {
			key_arr.fill(false);
		}
		const bool Key_Check(const int key, const bool cond) {
			if (GetAsyncKeyState(key)) {
				bool result;
				result = cond ? true : (cond == key_arr[key] ? true : false);
				key_arr[key] = true;
				return result;
			}
			else key_arr[key] = false;
			return false;
		}
	};
}
