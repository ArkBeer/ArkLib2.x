#include"Header.hpp"
#include"D3D11_1.hpp"
#include"WIC.hpp"
#include<array>
#include<vector>
#include<fstream>
#include"Matrix.hpp"
Ark::FrameRate fps;
Ark::Random rnd;
Ark::Key key;
int Main();
bool flag = true;
int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow) {
	return Main();
}

struct Notes {
	enum class State {
		None,Good,Miss
	};
	State state;
	enum class Type {
		Normal,
		Head,
		Tail,
		Simul
	};
	Type type;
	float time;
	float line;
	float y;
	float speed;
	Notes(const float _time, const float _speed, const float _line,Type _type) :time(_time), speed(_speed), line(_line), type(_type){
		y = line-fps.Fps * time*speed;
		state = State::None;
	
	}
	Notes(const float _time, const float _speed, const float _line):Notes(_time,_speed,_line,Type::Normal) {
	}
	void move() {
		y += speed;
	}
	const float diff() {
		return y - line;
	}
	const float dist() {
		return diff() > 0 ? diff() : -diff();
	}
	void init() {
		y = line - fps.Fps * time*speed;
		state = State::None;
	}
#define AS(STATE) void as_##STATE(){state=State::##STATE;}
	AS(None)
	AS(Good)
	AS(Miss)
#undef AS
#define IS(STATE) const bool is_##STATE() const{return state==State::##STATE;}
		IS(None)
		IS(Good)
		IS(Miss)
#undef IS
};
Notes::Type operator|(Notes::Type lt, Notes::Type rt) {
	return static_cast<Notes::Type>(static_cast<int>(lt) | static_cast<int>(rt));
}
Notes::Type operator&(Notes::Type lt, Notes::Type rt) {
	return static_cast<Notes::Type>(static_cast<int>(lt) & static_cast<int>(rt));
}
bool operator<<(Notes::Type lt, Notes::Type rt) {
	return (lt&rt) == rt;
}

std::vector<std::vector<Notes>> readData(LPCTSTR lp) {
	std::ifstream ifs;
	ifs.open(lp,std::ios::in);
	while (!ifs.eof()) {

	}
	return std::vector<std::vector<Notes>>();
}
int Main() {
	Ark::WinClass wc;
	wc.AddStyle(WS_THICKFRAME).SetTitle(_T("v0.01")).SetSize(1200,800);
	Ark::D2D1_1 d;
	Ark::WIC w;
	Ark::Mouse m;
	Ark::D3D11_1::Texture tex;
	Ark::Key k;
	bool flag = false;
	float line = wc.GetSize().bottom*3.0 / 4;
	float width = wc.GetSize().right;
	std::string pushkey = "SDFJKL";
	std::vector<Notes> lane0{
		Notes(2,6, line),
		Notes(3,6, line),
		Notes(4,6, line),
		Notes(5,6, line),
		Notes(6,6, line,Notes::Type::Head),
		Notes(7,6, line,Notes::Type::Tail)
	};
	std::vector<Notes> lane1{
		Notes(2,6, line),
		Notes(3,6, line),
		Notes(4,6, line),
		Notes(5,6, line,Notes::Type::Head),
		Notes(6,6, line,Notes::Type::Tail),
		Notes(7,6, line)
	};
	std::vector<Notes> lane2{
		Notes(2,6, line),
		Notes(3,6, line),
		Notes(4,6, line,Notes::Type::Head),
		Notes(5,6, line,Notes::Type::Tail),
		Notes(6,6, line),
		Notes(7,6, line)
	};
	std::vector<Notes> lane3{
		Notes(2,6, line),
		Notes(3,6, line,Notes::Type::Head),
		Notes(4,6, line,Notes::Type::Tail),
		Notes(5,6, line),
		Notes(6,6, line),
		Notes(7,6, line)
	};
	std::vector<Notes> lane4{
		Notes(2,6, line,Notes::Type::Head),
		Notes(3,6, line,Notes::Type::Tail),
		Notes(4,6, line),
		Notes(5,6, line),
		Notes(6,6, line),
		Notes(7,6, line)
	};
	std::vector<Notes> lane5{
		Notes(2,6, line),
		Notes(3,6, line),
		Notes(4,6, line),
		Notes(5,6, line),
		Notes(6,6, line),
		Notes(7,6, line)
	};

	std::vector<std::vector<Notes>> score{
		lane0,lane1,lane2,lane3,lane4,lane5
	};
	std::array<std::vector<Notes>::iterator, 6> lanen = {
		score.begin()->begin() ,
		(score.begin() + 1)->begin() ,
		(score.begin() + 2)->begin(),
		(score.begin() + 3)->begin(),
		(score.begin() + 4)->begin(),
		(score.begin() + 5)->begin(),

	};
	auto s2 = std::chrono::steady_clock::now();
	while (!wc.EndFlag()) {
		auto s = std::chrono::steady_clock::now();
		wc.LockAspectRatio(16, 9);
		d.BeginDraw();
		d.DrawClear(RGB(200,200,200));
		d.DrawLine(0, wc.GetSize().bottom*3.0 / 4, 1200, wc.GetSize().bottom*3.0 / 4, RGB(255, 255, 255), 1.0f, 2);
		for (auto it = score.begin(); it != score.end();++it) {
			for (auto it2 = it->begin(); it2 != it->end();++it2) {
				const int n = std::distance(score.begin(), it);
				d.DrawEllipse(100+n*150,it2->y,50,50,RGB(0,120,255),1.0f);
				d.DrawLine(50 + n * 150, it2->y, 150 + n * 150, it2->y, RGB(0, 0, 0), 1.0f, 2);
				if (it2->type==Notes::Type::Head) {
					d.DrawRectangle(50+n*150,(it2+1)->y,150+n*150,it2->y,RGB(0,120,255),0.5f);
				}
			}
		}
		Ark::TstringStream time;
		time<< (std::chrono::steady_clock::now()-s2).count();
		if (k.KeyDown(VK_RETURN, false)) {
			flag = !flag;
		}
		if (!flag) {
				s2 = std::chrono::steady_clock::now();
				for (auto& notes:score) {
					for (auto& v : notes) { v.init(); }
				}
				lanen = {
					score.begin()->begin() ,
					(score.begin() + 1)->begin() ,
					(score.begin() + 2)->begin(),
					(score.begin() + 3)->begin(),
					(score.begin() + 4)->begin(),
					(score.begin() + 5)->begin(),
				};

		}
		if (flag) {
			for (auto& notes:score) {
				for (auto& v : notes) {
					v.move();
				}
			}
		}
		for (int i = 0; i < lanen.size();++i) {
			auto& it = lanen.at(i);
			auto& lane = score.at(i);
			if (it != lane.end()) {
				if (it->diff()<=100) {
					if (it->type != Notes::Type::Tail) {
						if (key.KeyDown(pushkey.at(i), false)) {
							if (it->dist()<=100) {
								it->as_Good();
								++it;
							}
							else if(it->dist()<=150) {
								it->as_Miss();
								++it;
							}
						}
					}
					else {
						if ((it - 1)->is_Good()) {
							if (key.KeyUp(pushkey.at(i))) {
								if (it->dist() <= 100) {
									it->as_Good();
									++it;
								}
								else {
									it->as_Miss();
									++it;
								}
							}
						}
						else {
							it->as_Miss();
							++it;
						}
					}
				}
				else {
					it->as_Miss();
					++it;
				}
			}
		}
		d.DrawString(0, 0, time.str().c_str(),0x000000, 1.0f, 10);
		int c = 0;
		for (const auto& notes:score) {
		Ark::TstringStream result;
			for (const auto& v : notes) {
				if (v.is_Good())result << "Good\n";
				if (v.is_Miss())result << "Miss\n";
				if (v.is_None())result << "None\n";
			}
		d.DrawString(0+c*50,10,result.str().c_str(), 0x000000, 1.0f, 10);
		c += 1;
		}
		d.DrawString(0, 100, _T("(re)start:enter\nkey:S,D,F,J,K,L"), 0x000000, 1.0f, 10);
		d.EndDraw();
		fps.Count();
		Ark::TstringStream tstr;
		tstr << fps.perDuration<double>(s)<<"\n";
		OutputDebugString(tstr.str().c_str());
	}
	return 0;
}
