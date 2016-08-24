#include "pch.h"

HighScores::HighScores()
{
}

HighScores::~HighScores()
{
}

cocos2d::Scene* HighScores::CreateScene()
{
	cocos2d::Scene* scene = cocos2d::Scene::create();

	cocos2d::Layer* layer = HighScores::create();

	scene->addChild(layer);

	return scene;
}

bool HighScores::init()
{
    if(!gameData)
        gameData = cocos2d::UserDefault::getInstance();

	if (!gameData->getBoolForKey(sound_on, true))
		audio->MuteSound();

	//Initialize origin
	SCREEN_WIDTH = glview->getDesignResolutionSize().width;
	SCREEN_HEIGHT = glview->getDesignResolutionSize().height;
	float FRAME_WIDTH = glview->getFrameSize().width;
	float FRAME_HEIGHT = glview->getFrameSize().height;
	float screenRatio;

	if (FRAME_WIDTH > FRAME_HEIGHT)
		screenRatio = FRAME_WIDTH / FRAME_HEIGHT;
	else
		screenRatio = FRAME_HEIGHT / FRAME_WIDTH;

    int textSizeShrink = 0;
    
    //16x9
    if (screenRatio >= 1.7)
    {
        if(glview->getFrameSize().width > glview->getFrameSize().height && glview->getFrameSize().width >= 1920)
        {
            textSizeShrink = 1;
        }
        else if(glview->getFrameSize().height > glview->getFrameSize().width && glview->getFrameSize().height >= 1920)
        {
            textSizeShrink = 1;
        }
        else
        {
        }
    }
    
    //5:3
    else if (screenRatio >= 1.65)
    {
    }
    
    //16:10
    else if (screenRatio >= 1.59)
    {
    }
    
    //3x2
    else if (screenRatio >= 1.49)
    {
    }
    
    //4x3
    else if (screenRatio >= 1.32)
    {
        textSizeShrink = 1;
    }
    
    center.x = SCREEN_WIDTH / 2;
    center.y = SCREEN_HEIGHT / 2;
    
    //Set clear color to white
    director->setClearColor(cocos2d::Color4F(1, 1, 1, 1));

	//Create cache and batch
	nrgFunctions::CreateSpriteBatch("spritebatch", batch, cache);
	addChild(batch);
	
	//Text Size for text objects

	//16x9
	if (screenRatio >= 1.7)
		tileTextSize = 15;

	//5:3
	else if (screenRatio >= 1.65)
		tileTextSize = 16;

	//16:10
	else if (screenRatio >= 1.59)
		tileTextSize = 16;

	//3x2
	else if (screenRatio >= 1.49)
		tileTextSize = 16;

	//4x3
	else if (screenRatio >= 1.32)
		tileTextSize = 18;

	backButton = nrgButton::create("back_button.png", "back_button_pressed.png", false);
	addChild(backButton);

	fillScreen = nrgSprite::create(batch, "square_light.png", false);
	fillScreen->setLocalZOrder(1);

	//We want the image of the square to take up the entire screen, so let's do the math
	float fillWidth = fillScreen->GetWidth();
	float fillHeight = fillScreen->GetHeight();
	float stretchWidth = SCREEN_WIDTH / fillWidth;
	float stretchHeight = SCREEN_HEIGHT / fillHeight;
	fillScreen->setScale(stretchWidth, stretchHeight);

	classicScoreTile = nrgButton::create(batch, "best_score_tile_large.png", "best_score_tile_large_pressed.png", false);
	classicMatchTile = nrgButton::create(batch, "best_score_tile_large.png", "best_score_tile_large_pressed.png", false);
	countdownScoreTile = nrgButton::create(batch, "best_score_tile_large.png", "best_score_tile_large_pressed.png", false);
	countdownMatchTile = nrgButton::create(batch, "best_score_tile_large.png", "best_score_tile_large_pressed.png", false);
	memoryScoreTile = nrgButton::create(batch, "best_score_tile_large.png", "best_score_tile_large_pressed.png", false);
	memoryMatchTile = nrgButton::create(batch, "best_score_tile_large.png", "best_score_tile_large_pressed.png", false);
	practiceScoreTile = nrgButton::create(batch, "best_score_tile_large.png", "best_score_tile_large_pressed.png", false);
	scoreTile = nrgButton::create(batch, "best_score_tile_large.png", "best_score_tile_large_pressed.png", false);
	matchTile = nrgButton::create(batch, "best_score_tile_large.png", "best_score_tile_large_pressed.png", false);
	achievementsTile = nrgButton::create(batch, "best_score_tile_large.png", "best_score_tile_large_pressed.png", false);

	backButton->addEvents();
	classicScoreTile->addEvents();
	classicMatchTile->addEvents();
	countdownScoreTile->addEvents();
	countdownMatchTile->addEvents();
	memoryScoreTile->addEvents();
	memoryMatchTile->addEvents();
	practiceScoreTile->addEvents();
	scoreTile->addEvents();
	matchTile->addEvents();
	achievementsTile->addEvents();

	classicScoreTile->ToggleTouch(false);
	classicMatchTile->ToggleTouch(false);
	countdownScoreTile->ToggleTouch(false);
	countdownMatchTile->ToggleTouch(false);
	memoryScoreTile->ToggleTouch(false);
	memoryMatchTile->ToggleTouch(false);
	practiceScoreTile->ToggleTouch(false);
	scoreTile->ToggleTouch(false);
	matchTile->ToggleTouch(false);
	achievementsTile->ToggleTouch(false);

	nrgFunctions::ResizeForDevice(backButton);
	nrgFunctions::ResizeForDevice(classicScoreTile);
	nrgFunctions::ResizeForDevice(classicMatchTile);
	nrgFunctions::ResizeForDevice(countdownScoreTile);
	nrgFunctions::ResizeForDevice(countdownMatchTile);
	nrgFunctions::ResizeForDevice(memoryScoreTile);
	nrgFunctions::ResizeForDevice(memoryMatchTile);
	nrgFunctions::ResizeForDevice(practiceScoreTile);
	nrgFunctions::ResizeForDevice(scoreTile);
	nrgFunctions::ResizeForDevice(matchTile);
	nrgFunctions::ResizeForDevice(achievementsTile);

	classicScoreText = nrgText::create("Best Classic Score", "fonts/POE.ttf", tileTextSize, classicScoreTile->GetWidth() * 0.9, 0, 0, cocos2d::Color4B::WHITE);
	classicMatchText = nrgText::create("Best Classic Match", "fonts/POE.ttf", tileTextSize, classicScoreTile->GetWidth() * 0.9, 0, 0, cocos2d::Color4B::WHITE);
	countdownScoreText = nrgText::create("Best Countdown Score", "fonts/POE.ttf", tileTextSize - textSizeShrink, classicScoreTile->GetWidth() * 0.9, 0, 0, cocos2d::Color4B::WHITE);
	countdownMatchText = nrgText::create("Best Countdown Match", "fonts/POE.ttf", tileTextSize - textSizeShrink, classicScoreTile->GetWidth() * 0.9, 0, 0, cocos2d::Color4B::WHITE);
	memoryScoreText = nrgText::create("Best Memory Score", "fonts/POE.ttf", tileTextSize, classicScoreTile->GetWidth() * 0.9, 0, 0, cocos2d::Color4B::WHITE);
	memoryMatchText = nrgText::create("Best Memory Match", "fonts/POE.ttf", tileTextSize, classicScoreTile->GetWidth() * 0.9, 0, 0, cocos2d::Color4B::WHITE);
	practiceScoreText = nrgText::create("Best 100 Matches Score", "fonts/POE.ttf", tileTextSize, classicScoreTile->GetWidth() * 0.9, 0, 0, cocos2d::Color4B::WHITE);
	scoreText = nrgText::create("Total Score", "fonts/POE.ttf", tileTextSize, classicScoreTile->GetWidth() * 0.9, 0, 0, cocos2d::Color4B::WHITE);
	matchText = nrgText::create("Total Matches", "fonts/POE.ttf", tileTextSize, classicScoreTile->GetWidth() * 0.9, 0, 0, cocos2d::Color4B::WHITE);
	achievementsText = nrgText::create("Achievements Unlocked", "fonts/POE.ttf", tileTextSize - textSizeShrink, classicScoreTile->GetWidth() * 0.9, 0, 0, cocos2d::Color4B::WHITE);

	classicScoreTile->LinkWithText(classicScoreText, cocos2d::Color4B::YELLOW);
	classicMatchTile->LinkWithText(classicMatchText, cocos2d::Color4B::YELLOW);
	countdownScoreTile->LinkWithText(countdownScoreText, cocos2d::Color4B::YELLOW);
	countdownMatchTile->LinkWithText(countdownMatchText, cocos2d::Color4B::YELLOW);
	memoryScoreTile->LinkWithText(memoryScoreText, cocos2d::Color4B::YELLOW);
	memoryMatchTile->LinkWithText(memoryMatchText, cocos2d::Color4B::YELLOW);
	practiceScoreTile->LinkWithText(practiceScoreText, cocos2d::Color4B::YELLOW);
	scoreTile->LinkWithText(scoreText, cocos2d::Color4B::YELLOW);
	matchTile->LinkWithText(matchText, cocos2d::Color4B::YELLOW);
	achievementsTile->LinkWithText(achievementsText, cocos2d::Color4B::YELLOW);

	addChild(classicScoreText);
	addChild(classicMatchText);
	addChild(countdownScoreText);
	addChild(countdownMatchText);
	addChild(memoryScoreText);
	addChild(memoryMatchText);
	addChild(practiceScoreText);
	addChild(scoreText);
	addChild(matchText);
	addChild(achievementsText);

	float spacing = (SCREEN_HEIGHT - classicScoreTile->GetHeight() * 10) / 11;
	backButton->setPositionX(10);
	backButton->SetTop(SCREEN_HEIGHT - 20);

	classicScoreTile->Center();
	classicMatchTile->Center();
	countdownScoreTile->Center();
	countdownMatchTile->Center();
	memoryScoreTile->Center();
	memoryMatchTile->Center();
	practiceScoreTile->Center();
	scoreTile->Center();
	matchTile->Center();
	achievementsTile->Center();

	classicScoreTile->SetTop(SCREEN_HEIGHT - spacing * 2 - classicScoreTile->GetHeight());
	classicMatchTile->SetTop(classicScoreTile->getPositionY() - spacing / 2);
	countdownScoreTile->SetTop(classicMatchTile->getPositionY() - spacing / 2);
	countdownMatchTile->SetTop(countdownScoreTile->getPositionY() - spacing / 2);
	memoryScoreTile->SetTop(countdownMatchTile->getPositionY() - spacing / 2);
	memoryMatchTile->SetTop(memoryScoreTile->getPositionY() - spacing / 2);
	practiceScoreTile->SetTop(memoryMatchTile->getPositionY() - spacing / 2);
	scoreTile->SetTop(practiceScoreTile->getPositionY() - spacing / 2);
	matchTile->SetTop(scoreTile->getPositionY() - spacing / 2);
	achievementsTile->SetTop(matchTile->getPositionY() - spacing / 2);

	fillScreen->runAction(cocos2d::Sequence::create(
		cocos2d::FadeOut::create(0.5),
		cocos2d::CallFunc::create([this]() {
		classicScoreTile->ToggleTouch(true);
		classicMatchTile->ToggleTouch(true);
		countdownScoreTile->ToggleTouch(true);
		countdownMatchTile->ToggleTouch(true);
		memoryScoreTile->ToggleTouch(true);
		memoryMatchTile->ToggleTouch(true);
		practiceScoreTile->ToggleTouch(true);
		scoreTile->ToggleTouch(true);
		matchTile->ToggleTouch(true);
		achievementsTile->ToggleTouch(true);
		}), NULL)
	);
	
	scheduleUpdate();

	return true;
}

void HighScores::update(float dt)
{
	if (backButton->WasPressed())
    {
        audio->PlayClip("low_tone");
        director->popScene();
    }

#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	if (classicScoreTile->WasPressed())
	{
		if (!sdkbox::PluginSdkboxPlay::isSignedIn() && !testing)
			sdkbox::PluginSdkboxPlay::signin();
		sdkbox::PluginSdkboxPlay::showLeaderboard("high_score_classic");
	}

	if (classicMatchTile->WasPressed())
	{
		if (!sdkbox::PluginSdkboxPlay::isSignedIn() && !testing)
			sdkbox::PluginSdkboxPlay::signin();
		sdkbox::PluginSdkboxPlay::showLeaderboard("highest_match_classic");
	}

	if (countdownScoreTile->WasPressed())
	{
		if (!sdkbox::PluginSdkboxPlay::isSignedIn() && !testing)
			sdkbox::PluginSdkboxPlay::signin();
		sdkbox::PluginSdkboxPlay::showLeaderboard("high_score_countdown");
	}

	if (countdownMatchTile->WasPressed())
	{
		if (!sdkbox::PluginSdkboxPlay::isSignedIn() && !testing)
			sdkbox::PluginSdkboxPlay::signin();
		sdkbox::PluginSdkboxPlay::showLeaderboard("highest_match_countdown");
	}

	if (memoryScoreTile->WasPressed())
	{
		if (!sdkbox::PluginSdkboxPlay::isSignedIn() && !testing)
			sdkbox::PluginSdkboxPlay::signin();
		sdkbox::PluginSdkboxPlay::showLeaderboard("high_score_memory");
	}

	if (memoryMatchTile->WasPressed())
	{
		if (!sdkbox::PluginSdkboxPlay::isSignedIn() && !testing)
			sdkbox::PluginSdkboxPlay::signin();
		sdkbox::PluginSdkboxPlay::showLeaderboard("highest_match_memory");
	}

	if (practiceScoreTile->WasPressed())
	{
		if (!sdkbox::PluginSdkboxPlay::isSignedIn() && !testing)
			sdkbox::PluginSdkboxPlay::signin();
		sdkbox::PluginSdkboxPlay::showLeaderboard("high_score_practice");
	}

	if (scoreTile->WasPressed())
	{
		if (!sdkbox::PluginSdkboxPlay::isSignedIn() && !testing)
			sdkbox::PluginSdkboxPlay::signin();
		sdkbox::PluginSdkboxPlay::showLeaderboard("total_score");
	}

	if (matchTile->WasPressed())
	{
		if (!sdkbox::PluginSdkboxPlay::isSignedIn() && !testing)
			sdkbox::PluginSdkboxPlay::signin();
		sdkbox::PluginSdkboxPlay::showLeaderboard("total_matches");
	}

	if (achievementsTile->WasPressed())
	{
		if (!sdkbox::PluginSdkboxPlay::isSignedIn() && !testing)
			sdkbox::PluginSdkboxPlay::signin();
		sdkbox::PluginSdkboxPlay::showAchievements();
	}
#endif
	return;
}

void HighScores::onEnter()
{
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	sdkbox::PluginAdMob::hide("home");
    sdkbox::PluginSdkboxPlay::setListener(this);
#endif

	cocos2d::Layer::onEnter();
	return;
}

void HighScores::onExit()
{
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    sdkbox::PluginSdkboxPlay::removeListener();
#endif
	backButton->ToggleEnabled(false);
	classicScoreTile->ToggleEnabled(false);
	classicMatchTile->ToggleEnabled(false);
	countdownScoreTile->ToggleEnabled(false);
	countdownMatchTile->ToggleEnabled(false);
	memoryScoreTile->ToggleEnabled(false);
	memoryMatchTile->ToggleEnabled(false);
	practiceScoreTile->ToggleEnabled(false);
	scoreTile->ToggleEnabled(false);
	matchTile->ToggleEnabled(false);
	achievementsTile->ToggleEnabled(false);
	cocos2d::Layer::onExit();
	return;
}


#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
void HighScores::onConnectionStatusChanged(int status)
{
}

void HighScores::onScoreSubmitted(const std::string & leaderboard_name, int score, bool maxScoreAllTime, bool maxScoreWeek, bool maxScoreToday)
{
}

void HighScores::onIncrementalAchievementUnlocked(const std::string & achievement_name)
{
}

void HighScores::onIncrementalAchievementStep(const std::string & achievement_name, int step)
{
}

void HighScores::onAchievementUnlocked(const std::string & achievement_name, bool newlyUnlocked)
{
}
#endif
