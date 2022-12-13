#pragma once

namespace TGE {
	class TimeStep {
	public:
		TimeStep(float t): _time(t){}
		operator float() const { return _time; }
		/*void SetTime(const float t) { _time = t; }*/
		float GetTimeSeconds()const { return _time; }
		float GetTimemillSeconds()const { return _time * 1000; }
		//double GetTotalTime()const { return total_time; }
	private:
		float _time;//每帧所用时间
	};
}