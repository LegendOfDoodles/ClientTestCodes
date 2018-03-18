#pragma once

//50회의 프레임 처리시간을 누적하여 평균한다.
const ULONG MAX_SAMPLE_COUNT = 50;

class CGameTimer
{
private:
	bool m_bHardwareHasPerformanceCounter;
	float m_fTimeScale;
	float m_fTimeElapsed;

	__int64 m_nCurrentTime;
	__int64 m_nLastTime;

	__int64 m_nBaseTime;
	__int64 m_nPausedTime;
	__int64 m_nStopTime;

	__int64 m_nPerformanceFrequency;

	float m_fFrameTime[MAX_SAMPLE_COUNT];
	ULONG m_nSampleCount;

	unsigned long m_nCurrentFrameRate;
	unsigned long m_nFramesPerSecond;
	float m_fFPSTimeElapsed;

	bool m_bStopped;
public:
	CGameTimer();
	virtual ~CGameTimer();

	//타이머 시간 갱신한다.
	void Tick(float fLockFPS = 0.0f);
	void Reset();
	void Start();
	void Stop();
	//프레임 레이트 를 반환한다.
	unsigned long GetFrameRate(LPTSTR lpszString = NULL, int nCharacters = 0);
	//프레임의 평균 경과 시간을 반환한다.
	float GetTimeElapsed();
	float GetTotalTime();
};

