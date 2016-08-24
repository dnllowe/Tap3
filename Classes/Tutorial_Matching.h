#pragma once
#include "pch.h"
#include "Game.h"

class Tutorial_Matching : public Game
{

public:

	Tutorial_Matching();
	~Tutorial_Matching();

	static cocos2d::Scene* CreateScene();

	bool init();

	void update(float dt);

	void Reset();

	void SetMatchParameters(int excludedMatchCriteria = -1, bool blind = false);

	CREATE_FUNC(Tutorial_Matching);

	void onEnter();

	void onExit();

protected:
	
	bool step0 = true;
	bool step1 = false;
	bool step2 = false;
	bool step3 = false;
	bool step4 = false;
	bool step5 = false;
	bool step6 = false;
	bool step7 = false;
	bool step8 = false;
	bool step9 = false;
	bool step10 = false;
	bool step11 = false;
	bool step12 = false;
	bool step13 = false;
};

