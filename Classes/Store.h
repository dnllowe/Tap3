#pragma once
#include "pch.h"

class Store : public nrgScene
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
	CREATE_FUNC(Store);

protected:

	//Batch and cache
	cocos2d::SpriteBatchNode* batch = NULL;
	cocos2d::SpriteFrameCache* cache = cocos2d::SpriteFrameCache::getInstance();

	int currentTimePlus = 0;
	int currentEliminate = 0;
	int currentShowMatch = 0;
	int currentNewCard = 0;
	bool updateQuantity = false;

	nrgButton* backButton = NULL;
	nrgText* title = NULL;
	nrgText* separator_1 = NULL;
	
	nrgButton* buy_1 = NULL;
	nrgButton* buy_2 = NULL;
	nrgButton* buy_3 = NULL;
	nrgButton* buy_4 = NULL;
	nrgButton* buy_5 = NULL;
	nrgButton* buy_6 = NULL;
	nrgButton* buy_7 = NULL;

	nrgText* buy_1_text = NULL;
	nrgText* buy_2_text = NULL;
	nrgText* buy_3_text = NULL;
	nrgText* buy_4_text = NULL;
	nrgText* buy_5_text = NULL;
	nrgText* buy_6_text = NULL;
	nrgText* buy_7_text = NULL;

	nrgText* price_1 = NULL;
	nrgText* price_2 = NULL;
	nrgText* price_3 = NULL;
	nrgText* price_4 = NULL;
	nrgText* price_5 = NULL;
	nrgText* price_6 = NULL;
	nrgText* price_7 = NULL;

	nrgSprite* timePlusMain = NULL;
	nrgSprite* timePlusSmallThumbnail_1 = NULL;
	nrgSprite* timePlusSmallThumbnail_2 = NULL;
	nrgSprite* timePlusSmallThumbnail_3 = NULL;
	nrgText* timePlusDescription = NULL;
	nrgText* timePlusTitle = NULL;

	nrgSprite* eliminateMain = NULL;
	nrgSprite* eliminateSmallThumbnail_1 = NULL;
	nrgSprite* eliminateSmallThumbnail_2 = NULL;
	nrgSprite* eliminateSmallThumbnail_3 = NULL;
	nrgText* eliminateDescription = NULL;
	nrgText* eliminateTitle = NULL;

	nrgSprite* showMatchMain = NULL;
	nrgSprite* showMatchSmallThumbnail_1 = NULL;
	nrgSprite* showMatchSmallThumbnail_2 = NULL;
	nrgSprite* showMatchSmallThumbnail_3 = NULL;
	nrgText* showMatchDescription = NULL;
	nrgText* showMatchTitle = NULL;

	nrgSprite* newCardMain = NULL;
	nrgSprite* newCardSmallThumbnail_1 = NULL;
	nrgSprite* newCardSmallThumbnail_2 = NULL;
	nrgSprite* newCardSmallThumbnail_3 = NULL;
	nrgText* newCardDescription = NULL;
	nrgText* newCardTitle = NULL;

	nrgText* comboDescription_1 = NULL;
	nrgText* comboTitle_1 = NULL;
	nrgText* comboDescription_2 = NULL;
	nrgText* comboTitle_2 = NULL;
	nrgText* comboDescription_3 = NULL;
	nrgText* comboTitle_3 = NULL;

	nrgText* timePlusQuantity = NULL;
	nrgText* eliminateQuantity = NULL;
	nrgText* showMatchQuantity = NULL;
	nrgText* newCardQuantity = NULL;
	nrgText* comboQuantity_1 = NULL;
	nrgText* comboQuantity_2 = NULL;
	nrgText* comboQuantity_3 = NULL;

	Store();
	~Store();

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

