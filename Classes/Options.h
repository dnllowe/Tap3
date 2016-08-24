#pragma once
#include "pch.h"

class Options : public nrgScene
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	, public sdkbox::IAPListener
#endif
{
public:

	//Creates scene and initializes necessary variables using Init()
	static cocos2d::Scene* CreateScene();

	//Function that runs game loop using CREATE_FUNC from Cocos2dx
	bool init();

	void update(float dt);

	void onEnter();
	void onExit();

	//Creates the "create" function
	CREATE_FUNC(Options);

protected:
	Options();
	~Options();

	//Batch and cache
	cocos2d::SpriteBatchNode* batch = NULL;
	cocos2d::SpriteFrameCache* cache = cocos2d::SpriteFrameCache::getInstance();

	nrgText* title = NULL; //Title for options menu
	nrgText* musicText = NULL; //Text for "Music"
	nrgText* soundText = NULL; //Text for "Sound"
	nrgMenu* musicOnOff = NULL; //Menu for music "On" and "Off"
	nrgMenu* soundOnOff = NULL; //Menu for sound "On" and "Off"
	nrgMenu* removeAds = NULL; //Menu for remove ads
	nrgMenu* howToPlay = NULL;
	nrgMenu* usingBonusItems = NULL;
	nrgMenu* restorePurchases = NULL;
	nrgButton* backButton = NULL; //Back button

private:
	
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	virtual void onSuccess(sdkbox::Product const& p) override;

	virtual void onFailure(sdkbox::Product const& p, const std::string &msg) override;

	virtual void onCanceled(sdkbox::Product const& p) override;

	virtual void onRestored(sdkbox::Product const& p) override;

	virtual void onProductRequestSuccess(std::vector<sdkbox::Product> const &products) override;

	virtual void onProductRequestFailure(const std::string &msg) override;

	virtual void onInitialized(bool ok) override;

	virtual void onRestoreComplete(bool ok, const std::string &msg) override;
#endif
};

