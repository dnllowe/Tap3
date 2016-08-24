#pragma once
#include "pch.h"
#include "Game.h"

class Tutorial_Bonus : public Game
{	
	

public:
	Tutorial_Bonus();
	~Tutorial_Bonus();

	static cocos2d::Scene* CreateScene();

	bool init();

	void update(float dt);

	void Reset();

	CREATE_FUNC(Tutorial_Bonus);

	void onEnter();

	void onExit();

protected:

	int textOffsetY = 58;
	bool step1 = true;
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
	bool step14 = false;
	bool step15 = false;
	bool step16 = false;
	bool step17 = false;
	bool step18 = false;
	bool step19 = false;
	bool step20 = false;
	bool step21 = false;
	bool step22 = false;
	bool step23 = false;

	bool used1TimePlus = false;
	bool used2TimePlus = false;

	float originalScale;
};

