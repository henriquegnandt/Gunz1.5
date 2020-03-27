#ifndef _ZRULE_CTF_H
#define _ZRULE_CTF_H

#include "ZRule.h"


class ZRuleCTF: public ZRule
{
public:
	ZRuleCTF(ZMatch* pMatch);
	virtual ~ZRuleCTF();

	virtual bool OnCommand(MCommand* pCommand);
};



#endif