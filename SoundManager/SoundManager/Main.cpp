// SoundManager.cpp: 콘솔 응용 프로그램의 진입점을 정의합니다.
//

#include "stdafx.h"
#include "SoundManager.h"

class CSoundManager;

int main()
{
	CSoundManager *m_SoundManager;

	m_SoundManager = new CSoundManager();

	m_SoundManager->init();
	m_SoundManager->loading();
	
	int inputSoundNum = 0;
	int prevSoundNum = -1;


	while(1) {
		printf("Sound Num\n");
		scanf_s("%d", &inputSoundNum);

		if (prevSoundNum != -1)
			m_SoundManager->stop(prevSoundNum);

		m_SoundManager->play(inputSoundNum);

		prevSoundNum = inputSoundNum;
	}

    return 0;
}

