#pragma once

typedef std::list<CBaseObject*> ObjectList;

class CHPGaugeManager
{
public:
	CHPGaugeManager();
	~CHPGaugeManager();

	void AddMinionObject(CBaseObject *pNewMinion);
	int GetCount() { return m_naddCount; };
	void ResetCount() { m_naddCount = 0; };

	ObjectList *GetMinionObjectList() { return &m_MinionObjectList; };

private:
	ObjectList m_MinionObjectList;
	int m_naddCount{ 0 };
};
