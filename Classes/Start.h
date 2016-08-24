#pragma once
#include "pch.h"

//Forward declaration for nrgSprite and nrgText
class nrgSprite;
class nrgText;
class nrgTimer;

class Start : public nrgScene
{
public:

	//Creates scene and initializes necessary variables using Init()
	static cocos2d::Scene* CreateScene();

	//Initializes members for scene (runs from CreateScene on instance)
	void Init();

	//Function that runs game loop using CREATE_FUNC from Cocos2dx
	virtual bool init();

	//Override update loop
	void update(float fl);

	void onExit();

	CREATE_FUNC(Start);

protected:

	//Loading text
	nrgText* loading = NULL;
	cocos2d::SpriteBatchNode* batch = NULL;
	cocos2d::SpriteFrameCache* cache = cocos2d::SpriteFrameCache::getInstance();
};
