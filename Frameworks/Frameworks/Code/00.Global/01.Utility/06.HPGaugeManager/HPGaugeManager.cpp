#include "stdafx.h"
#include "HPGaugeManager.h"

CHPGaugeManager::CHPGaugeManager()
{
}

CHPGaugeManager::~CHPGaugeManager()
{
}

void CHPGaugeManager::AddMinionObject(CBaseObject * pNewMinion)
{
	if (pNewMinion) {
		m_MinionObjectList.push_back(pNewMinion);

		m_naddCount++;
	}
}