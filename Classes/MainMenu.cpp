#pragma once
#include "pch.h"
#include "MainMenu.h"

MainMenu::MainMenu()
{
}

cocos2d::Scene * MainMenu::CreateScene()
{
	//Create Scene
	cocos2d::Scene* scene = cocos2d::Scene::create();

	//Create layer
	cocos2d::Layer* layer = MainMenu::create();

	scene->addChild(layer);

	return scene;
}

bool MainMenu::init()
{
	//Tie instance point to object instance
	instance = this;
	
    if(!gameData)
        gameData = cocos2d::UserDefault::getInstance();
    
	mode = -1;

    //Allow the layer to have touch events so user can tap screen anywhere to start game
    addEvents();
    ChangeTouchListenerPriority(3);
    
    if(testing)
    {
		GameData::ResetPurchasedContent();
		GameData::ResetStats();

		//Temporarily set some stats for testing
		gameData->setIntegerForKey(best_score_countdown, 100);
		gameData->setIntegerForKey(best_score_classic, 200);
		gameData->setIntegerForKey(best_score_rushrelax, 300);
		gameData->setIntegerForKey(best_score_memory, 400);

		gameData->setIntegerForKey(best_match_countdown, 1);
		gameData->setIntegerForKey(best_match_classic, 2);
		gameData->setIntegerForKey(best_match_rushrelax, 3);
		gameData->setIntegerForKey(best_match_memory, 4);
    }
    
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


	center.x = SCREEN_WIDTH / 2;
	center.y = SCREEN_HEIGHT / 2;

	titleBottomPosition = SCREEN_HEIGHT * 0.6875;

	//Set clear color to white
	director->setClearColor(cocos2d::Color4F(1, 1, 1, 1));

	//Create cache and batch
	nrgFunctions::CreateSpriteBatch("spritebatch", batch, cache);
	addChild(batch);
	
	//Text Size for text objects

    //16x9
    if (screenRatio >= 1.7)
    {
        titleTextSize = 70;
        eyebrowTextSize = 30;
        displayTextSize = 27;
        pointsTextSize = 15;
        retryTextSize = 17;
        matchMadeStringsSize = 7;
        timeRemainingTextSize = 21;
        bonusTextSize = 7;
        scoreTextSize = 15;
        topMenuButtonTextSize = 9;
        baseTileMenuButtonTextSize = 13;
        gameModeTextSize = 11;
        
        if(glview->getFrameSize().width > glview->getFrameSize().height && glview->getFrameSize().width >= 1920)
        {
            baseTileTextSize = 12;
            gameModeTextSize = 10;
        }
        else if(glview->getFrameSize().height > glview->getFrameSize().width && glview->getFrameSize().height >= 1920)
        {
            baseTileTextSize = 12;
            gameModeTextSize = 10;
        }
        else
        {
            baseTileTextSize = 14;
        }
    }
    
	//5:3
	else if (screenRatio >= 1.65)
	{
		titleTextSize = 80;
		eyebrowTextSize = 28;
		displayTextSize = 25;
		baseTileTextSize = 15;
		pointsTextSize = 16;
		retryTextSize = 18;
		matchMadeStringsSize = 5;
		timeRemainingTextSize = 19;
		bonusTextSize = 5;
		scoreTextSize = 16;
		topMenuButtonTextSize = 10;
		baseTileMenuButtonTextSize = 14;
		gameModeTextSize = 12;
	}

	//16:10
	else if (screenRatio >= 1.59)
	{
		titleTextSize = 80;
		eyebrowTextSize = 28;
		displayTextSize = 25;
		baseTileTextSize = 13;
		pointsTextSize = 16;
		retryTextSize = 18;
		matchMadeStringsSize = 5;
		timeRemainingTextSize = 19;
		bonusTextSize = 5;
		scoreTextSize = 16;
		topMenuButtonTextSize = 10;
		baseTileMenuButtonTextSize = 12;
		gameModeTextSize = 11;
	}

    //3x2
    else if (screenRatio >= 1.49)
    {
        titleTextSize = 70;
        eyebrowTextSize = 30;
        displayTextSize = 28;
        baseTileTextSize = 14;
        pointsTextSize = 16;
        retryTextSize = 18;
        matchMadeStringsSize = 8;
        timeRemainingTextSize = 22;
        bonusTextSize = 8;
        scoreTextSize = 16;
        topMenuButtonTextSize = 9;
        baseTileMenuButtonTextSize = 14;
        gameModeTextSize = 11;
    }
    
    //4x3
    else if (screenRatio >= 1.32)
    {
        titleTextSize = 80;
        eyebrowTextSize = 40;
        displayTextSize = 32;
        baseTileTextSize = 15;
        pointsTextSize = 18;
        retryTextSize = 20;
        matchMadeStringsSize = 10;
        timeRemainingTextSize = 24;
        bonusTextSize = 10;
        scoreTextSize = 18;
        topMenuButtonTextSize = 10;
        baseTileMenuButtonTextSize = 15;
        gameModeTextSize = 12;
    }
    
    //Load game objects / assets
    baseTile = BaseTile::create(batch);
	baseTileShadow = nrgSprite::create(batch, "base_tile_shadow.png", false);
	titleShadow = nrgSprite::create(batch, "base_tile_shadow.png", false);
	bestScoreTileShadow = nrgSprite::create(batch, "base_tile_shadow.png", false);
	bestScoreTile = nrgSprite::create(batch, "best_score_tile_large.png", false);
	optionsButton = nrgButton::create(batch, "menu_button.png", "menu_button_pressed.png", false);
	highScoresButton = nrgButton::create(batch, "menu_button.png", "menu_button_pressed.png", false);
	buyBonusItemsButton = nrgButton::create(batch, "menu_button.png", "menu_button_pressed.png", false);
	optionsText = nrgText::create("Options", "fonts/alba.regular.ttf", topMenuButtonTextSize, 45, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER);
	highScoresText = nrgText::create("High Scores", "fonts/alba.regular.ttf", topMenuButtonTextSize, 45, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER);
	buyBonusItemsText = nrgText::create("Buy Bonus Items", "fonts/alba.regular.ttf", topMenuButtonTextSize, 45, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER);
	menuLeft = nrgButton::create(batch, "question_tile.png", "question_tile_pressed.png", false);
	menuRight = nrgButton::create(batch, "question_tile.png", "question_tile_pressed.png", false);

	addChild(optionsText);
	addChild(highScoresText);
	addChild(buyBonusItemsText);

	optionsButton->addEvents();
	highScoresButton->addEvents();
	buyBonusItemsButton->addEvents();

	optionsButton->ToggleTouch(false);
	highScoresButton->ToggleTouch(false);
	buyBonusItemsButton->ToggleTouch(false);

    optionsButton->ChangeTouchListenerPriority(2);
    highScoresButton->ChangeTouchListenerPriority(2);
    buyBonusItemsButton->ChangeTouchListenerPriority(2);
    
	//Title assets
    title = nrgText::create("Tap 3!", "fonts/alba.super.ttf", titleTextSize, SCREEN_WIDTH, 0, 0, cocos2d::Color4B(20, 175, 255, 128));
	eyebrow = nrgText::create("Welcome to...", "fonts/alba.regular.ttf", eyebrowTextSize, SCREEN_WIDTH, 0, 0, cocos2d::Color4B(127, 211, 255, 128));
	
	addChild(title);
	addChild(eyebrow);

	nrgFunctions::ResizeForDevice(optionsButton);
	nrgFunctions::ResizeForDevice(highScoresButton);
	nrgFunctions::ResizeForDevice(buyBonusItemsButton);

	highScoresButton->Center();
	highScoresButton->SetTop(SCREEN_HEIGHT - 20);
	optionsButton->SetRightSide(highScoresButton->getPositionX() - optionsButton->GetWidth() * 0.25);
	optionsButton->SetTop(SCREEN_HEIGHT - 20);
	buyBonusItemsButton->setPositionX(highScoresButton->GetRightSide() + buyBonusItemsButton->GetWidth() * 0.25);
	buyBonusItemsButton->SetTop(SCREEN_HEIGHT - 20);
	nrgFunctions::Center(optionsText, optionsButton);
	nrgFunctions::Center(highScoresText, highScoresButton);
	nrgFunctions::Center(buyBonusItemsText, buyBonusItemsButton);

	//Load quick pointers to objects
	topLeftTile = baseTile->GetTopLeftTile();
	topCenterTile = baseTile->GetTopCenterTile();
	topRightTile = baseTile->GetTopRightTile();
	middleLeftTile = baseTile->GetMiddleLeftTile();
	middleCenterTile = baseTile->GetMiddleCenterTile();
	middleRightTile = baseTile->GetMiddleRightTile();
	bottomLeftTile = baseTile->GetBottomLeftTile();
	bottomCenterTile = baseTile->GetBottomCenterTile();
	bottomRightTile = baseTile->GetBottomRightTile();

	//Resize assets for device
	nrgFunctions::ResizeForDevice(baseTile);
	float baseWidth = baseTile->GetWidth();
	float baseHeight = baseTile->GetHeight();
	float baseScale = baseTile->getScale();

	//Request for review assets
	int maxWidth = baseTile->GetWidth() * 0.9;

	rightMenuText = nrgText::create("Start", "fonts/alba.super.ttf", baseTileMenuButtonTextSize, maxWidth, 0, 0, cocos2d::Color4B(127, 211, 255, 255));
	leftMenuText = nrgText::create("Back", "fonts/alba.super.ttf", baseTileMenuButtonTextSize, maxWidth, 0, 0, cocos2d::Color4B(127, 211, 255, 255));
	addChild(leftMenuText);
	addChild(rightMenuText);

	menuLeft->addEvents();
	menuRight->addEvents();
	menuLeft->ToggleTouch(false);
	menuRight->ToggleTouch(false);

	nrgFunctions::ResizeForDevice(menuLeft);
	nrgFunctions::ResizeForDevice(menuRight);
	nrgFunctions::ResizeForDevice(baseTileShadow);
	nrgFunctions::ResizeForDevice(bestScoreTile);
	nrgFunctions::ResizeForDevice(bestScoreTileShadow);
	nrgFunctions::ResizeForDevice(titleShadow);

	baseTile->setPosition(center.x, center.y);
	titleShadow->setPosition(center.x, center.y - title->GetScaledHeight() * 0.75);
	title->Center();
	eyebrow->Center();
    eyebrow->SetBottom(baseTile->GetTop() + 20);
	baseTileShadow->Center();
	bestScoreTile->Center();
	bestScoreTileShadow->Center();
	baseTileShadow->SetTop(SCREEN_HEIGHT * 0.4);

	//Establish tile positions
	topLeft.first = baseWidth / baseScale * 0.07 + topLeftTile->GetWidth() / 2;
	topLeft.second = baseHeight / baseScale * 0.93 - topLeftTile->GetHeight() / 2;
	topRight.first = baseWidth / baseScale * 0.93 - topRightTile->GetWidth() / 2;
	topRight.second = baseHeight / baseScale * 0.93 - topRightTile->GetHeight() / 2;
	topCenter.first = baseWidth / baseScale / 2;
	topCenter.second = baseHeight / baseScale * 0.93 - topCenterTile->GetHeight() / 2;

	middleLeft.first = baseWidth / baseScale * 0.07 + middleLeftTile->GetWidth() / 2;
	middleLeft.second = baseHeight / baseScale / 2;
	middleRight.first = baseWidth / baseScale * 0.93 - middleRightTile->GetWidth() / 2;
	middleRight.second = baseHeight / baseScale / 2;
	middleCenter.first = baseWidth / baseScale / 2;
	middleCenter.second = baseHeight / baseScale / 2;

	bottomLeft.first = baseWidth / baseScale * 0.07 + bottomLeftTile->GetWidth() / 2;
	bottomLeft.second = baseHeight / baseScale * 0.07 + bottomLeftTile->GetHeight() / 2;
	bottomRight.first = baseWidth / baseScale * 0.93 - bottomRightTile->GetWidth() / 2;
	bottomRight.second = baseHeight / baseScale * 0.07 + bottomRightTile->GetHeight() / 2;
	bottomCenter.first = baseWidth / baseScale / 2;
	bottomCenter.second = baseHeight / baseScale * 0.07 + bottomCenterTile->GetHeight() / 2;

	tiles.push_back(topLeftTile);
	tiles.push_back(topCenterTile);
	tiles.push_back(topRightTile);
	tiles.push_back(middleLeftTile);
	tiles.push_back(middleCenterTile);
	tiles.push_back(middleRightTile);
	tiles.push_back(bottomLeftTile);
	tiles.push_back(bottomCenterTile);
	tiles.push_back(bottomRightTile);

    for(int iii = 0; iii < tiles.size(); iii++)
        tiles[iii]->setVisible(false);
    
	//Game mode assets
	countdownButton = nrgButton::create(batch, "game_mode_button.png", "game_mode_button_pressed.png", false);
	classicButton = nrgButton::create(batch, "game_mode_button.png", "game_mode_button_pressed.png", false);
	practiceButton = nrgButton::create(batch, "game_mode_button.png", "game_mode_button_pressed.png", false);
	memoryButton = nrgButton::create(batch, "game_mode_button.png", "game_mode_button_pressed.png", false);

	countdownButton->addEvents();
	countdownButton->ToggleEnabled(false);
	classicButton->addEvents();
	classicButton->ToggleEnabled(false);
	practiceButton->addEvents();
	practiceButton->ToggleEnabled(false);
	memoryButton->addEvents();
	memoryButton->ToggleEnabled(false);

	nrgFunctions::ResizeForDevice(countdownButton);
	nrgFunctions::ResizeForDevice(classicButton);
	nrgFunctions::ResizeForDevice(practiceButton);
	nrgFunctions::ResizeForDevice(memoryButton);

	classicButton->setPositionX(baseTile->getPositionX() - baseTile->GetWidth() / 2 + baseTile->GetWidth() * 0.1);
	countdownButton->SetRightSide(baseTile->getPositionX() - baseTile->GetWidth() / 2 + baseTile->GetWidth() * 0.9);
	memoryButton->setPositionX(baseTile->getPositionX() - baseTile->GetWidth() / 2 + baseTile->GetWidth() * 0.1);
	practiceButton->SetRightSide(baseTile->getPositionX() - baseTile->GetWidth() / 2 + baseTile->GetWidth() * 0.9);
	
	countdownButton->SetTop(baseTile->getPositionY() + baseTile->GetHeight() / 2 - baseTile->GetHeight() * 0.2);
	classicButton->SetTop(baseTile->getPositionY() + baseTile->GetHeight() / 2 - baseTile->GetHeight() * 0.2);
	practiceButton->setPositionY(baseTile->getPositionY() - baseTile->GetHeight() / 2 + baseTile->GetHeight() * 0.2);
	memoryButton->setPositionY(baseTile->getPositionY() - baseTile->GetHeight() / 2 + baseTile->GetHeight() * 0.2);

	//Basetile text assets
	baseTileTitle = nrgText::create("Choose Game Mode", "fonts/alba.regular.ttf", baseTileTextSize + 2, maxWidth, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER);
	addChild(baseTileTitle);
	baseTileTitle->GetLabel()->setOpacity(0);
	baseTileTitle->setVisible(false);
	baseTileTitle->Center();
	baseTileTitle->setPositionY(baseTile->getPositionY() + baseTile->GetHeight() * 0.45);

	baseTileText = nrgText::create("", "fonts/POE.ttf", baseTileTextSize, maxWidth, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER);
	addChild(baseTileText);
	baseTileText->GetLabel()->setOpacity(0);
	baseTileText->setVisible(false);
	baseTileText->Center();
	baseTileText->setPositionY(baseTile->getPositionY() + baseTile->GetHeight() * 0.45);

	scoreText = nrgText::create("Best Score (Countdown)", "fonts/POE.ttf", scoreTextSize, SCREEN_WIDTH, 0, 0, cocos2d::Color4B::WHITE);
	addChild(scoreText);
	pointsText = nrgText::create("0", "fonts/POE.ttf", pointsTextSize, SCREEN_WIDTH);
	addChild(pointsText);
	pointsText->Center();
	pointsText->SetBottom(pointsTextBottomPosition);
	bestScoreTile->setPositionY(pointsText->getPositionY() + 5);
	bestScoreTileShadow->setPositionY(bestScoreTile->getPositionY() - bestScoreTile->GetHeight() * 0.75);
	nrgFunctions::Center(scoreText, bestScoreTile);

	//Left side boxes
	menuLeft->Center();
	menuLeft->setPositionX(baseTile->getPositionX() - baseTile->GetWidth() / 2 + baseTile->GetWidth() * 0.25 - menuLeft->GetWidth() / 2);
	menuLeft->setPositionY(baseTile->getPositionY() - baseTile->GetHeight() / 2 + baseTile->GetHeight() * 0.025);

	//Righ side boxes
	menuRight->Center();
	menuRight->SetRightSide(baseTile->getPositionX() + baseTile->GetWidth() / 2 - baseTile->GetWidth() * 0.25 + menuRight->GetWidth() / 2);
	menuRight->setPositionY(baseTile->getPositionY() - baseTile->GetHeight() / 2 + baseTile->GetHeight() * 0.025);

	nrgFunctions::Center(leftMenuText, menuLeft);
	nrgFunctions::Center(rightMenuText, menuRight);
	menuRight->setPositionY(menuLeft->getPositionY());

	//Retry menu
	retry = nrgMenu::create("Tap Here To Start", "fonts/alba.regular.ttf", 0, retryTextSize, 0, cocos2d::Color4B::BLACK, cocos2d::Color4B::YELLOW, false);
	countdownText = nrgText::create("Countdown", "fonts/alba.super.ttf", gameModeTextSize - 1, countdownButton->GetWidth() * 0.9, 0, 0, cocos2d::Color4B(127, 211, 255, 255), cocos2d::TextHAlignment::CENTER);
	classicText = nrgText::create("Classic", "fonts/alba.super.ttf", gameModeTextSize, classicButton->GetWidth() * 0.9, 0, 0, cocos2d::Color4B(127, 211, 255, 255), cocos2d::TextHAlignment::CENTER);
	practiceText = nrgText::create("100 Matches", "fonts/alba.super.ttf", gameModeTextSize - 1, practiceButton->GetWidth() * 0.9, 0, 0, cocos2d::Color4B(127, 211, 255, 255), cocos2d::TextHAlignment::CENTER);
	memoryText = nrgText::create("Memory", "fonts/alba.super.ttf", gameModeTextSize, memoryButton->GetWidth() * 0.9, 0, 0, cocos2d::Color4B(127, 211, 255, 255), cocos2d::TextHAlignment::CENTER);
	addChild(retry);
	addChild(countdownText);
	addChild(classicText);
	addChild(practiceText);
	addChild(memoryText);

	countdownText->GetLabel()->setOpacity(0);
	classicText->GetLabel()->setOpacity(0);
	practiceText->GetLabel()->setOpacity(0);
	memoryText->GetLabel()->setOpacity(0);

	countdownButton->LinkWithText(countdownText, cocos2d::Color4B::GRAY);
	classicButton->LinkWithText(classicText, cocos2d::Color4B::GRAY);
	practiceButton->LinkWithText(practiceText, cocos2d::Color4B::GRAY);
	memoryButton->LinkWithText(memoryText, cocos2d::Color4B::GRAY);

	menuLeft->addEvents();
	menuRight->addEvents();
	menuLeft->ToggleTouch(false);
	menuRight->ToggleTouch(false);

	retry->GetItem(0)->runAction(cocos2d::RepeatForever::create(
		cocos2d::Sequence::create(
			cocos2d::DelayTime::create(0.125),
			cocos2d::FadeTo::create(0.5, 64),
			cocos2d::FadeTo::create(0.5, 255),
			cocos2d::DelayTime::create(0.125), NULL)));
	
	//Don't let user interrupt intro sequence
	ToggleTouch(false);

	RunIntro();

	scheduleUpdate();

	return true;
}

void MainMenu::update(float dt)
{

	if (optionsButton->WasPressed())
	{
		audio->PlayClip("double_tone_higher");
		optionsButton->ToggleTouch(false);
		highScoresButton->ToggleTouch(false);
		buyBonusItemsButton->ToggleTouch(false);
		director->pushScene(Options::CreateScene());
	}

	if (highScoresButton->WasPressed())
	{
		audio->PlayClip("double_tone_higher");
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
		optionsButton->ToggleTouch(false);
		highScoresButton->ToggleTouch(false);
		buyBonusItemsButton->ToggleTouch(false);
#endif
		SyncAchievements();
		SyncHighScores();
		ShowHighScores();
	}

	if (buyBonusItemsButton->WasPressed())
	{
		audio->PlayClip("double_tone_higher");
		optionsButton->ToggleTouch(false);
		highScoresButton->ToggleTouch(false);
		buyBonusItemsButton->ToggleTouch(false);
		director->pushScene(Store::CreateScene());
	}

	//Select game mode
	if (countdownButton->WasPressed())
	{
		audio->PlayClip("double_tone_higher");

		countdownButton->ToggleEnabled(false);
		countdownText->setVisible(false);
		classicButton->ToggleEnabled(false);
		classicText->setVisible(false);
		practiceButton->ToggleEnabled(false);
		practiceText->setVisible(false);
		memoryButton->ToggleEnabled(false);
		memoryText->setVisible(false);

		startMenuButtonOn = true;
		backMenuButtonOn = true;
		selectingGameMode = true;

		mode = COUNTDOWN;
		
		baseTileTitle->setString("COUNTDOWN");
		baseTileTitle->Center();
		baseTileTitle->setPositionY(baseTile->getPositionY() + baseTile->GetHeight() * 0.45);
		baseTileTitle->setVisible(true);
		baseTileTitle->GetLabel()->runAction(cocos2d::FadeIn::create(0.25));
		
		baseTileText->setString("\n\nMake as many matches as you can before time runs out. Time increases with every match, but so does the difficulty!\n(A Good Challenge!)");
		baseTileText->Center();
		baseTileText->setPositionY(baseTile->getPositionY() + baseTile->GetHeight() * 0.45);
		baseTileText->setVisible(true);
		baseTileText->GetLabel()->runAction(cocos2d::FadeIn::create(0.25));

		menuLeft->setPositionY(baseTile->getPositionY() - baseTile->GetHeight() / 2 + baseTile->GetHeight() * 0.025);
		menuRight->setPositionY(baseTile->getPositionY() - baseTile->GetHeight() / 2 + baseTile->GetHeight() * 0.025);

		nrgFunctions::Center(leftMenuText, menuLeft);
		nrgFunctions::Center(rightMenuText, menuRight);
		menuRight->setPositionY(menuLeft->getPositionY());

		menuLeft->runAction(cocos2d::FadeIn::create(0.25));
		menuRight->runAction(cocos2d::FadeIn::create(0.25));
		leftMenuText->GetLabel()->runAction(cocos2d::FadeIn::create(0.25));
		rightMenuText->GetLabel()->runAction(cocos2d::FadeIn::create(0.25));

		menuLeft->ToggleTouch(true);
		menuRight->ToggleTouch(true);

		displayingCountdown = false;

		if (bestScoreTile->getOpacity() < 255)
			bestScoreTile->runAction(cocos2d::FadeIn::create(0.25));
		if (bestScoreTileShadow->getOpacity() < 255)
			bestScoreTileShadow->runAction(cocos2d::FadeIn::create(0.25));

		scoreText->GetLabel()->setOpacity(0);
		pointsText->GetLabel()->setOpacity(0);

		scoreText->GetLabel()->runAction(cocos2d::Sequence::create(
			cocos2d::CallFunc::create([this]() {scoreText->setString("Best Score (Countdown)"); nrgFunctions::Center(scoreText, bestScoreTile);}),
			cocos2d::FadeIn::create(0.5), NULL));

		pointsText->GetLabel()->runAction(cocos2d::Sequence::create(
			cocos2d::CallFunc::create([this]()
		{
			if (highestMatchCountdown != 1)
				sprintf(sz, "%d points   |   %d matches", highestScoreCountdown, highestMatchCountdown);
			else
				sprintf(sz, "%d points   |   %d match", highestScoreCountdown, highestMatchCountdown);

			pointsText->setString(sz);
			pointsText->Center();
			pointsText->SetBottom(pointsTextBottomPosition);
		}),
			cocos2d::FadeIn::create(0.5), NULL));

		//Decide next score to display. Starting at hard should cycle to normal, then easy.
		if (highestMatchClassic > 0 || highestScoreClassic > 0)
			displayingClassic = true;
		else if (highestMatchMemory > 0 || highestScoreMemory > 0)
			displayingMemory = true;
		else if (highestMatchPractice > 0 || highestScorePractice)
			displayingPractice = true;
	}

	if (classicButton->WasPressed())
	{
		audio->PlayClip("double_tone_higher");

		countdownButton->ToggleEnabled(false);
		countdownText->setVisible(false);
		classicButton->ToggleEnabled(false);
		classicText->setVisible(false);
		practiceButton->ToggleEnabled(false);
		practiceText->setVisible(false);
		memoryButton->ToggleEnabled(false);
		memoryText->setVisible(false);

		startMenuButtonOn = true;
		backMenuButtonOn = true;
		selectingGameMode = true;

		mode = CLASSIC; 
		scoreText->setString("Best Score (Classic)");

		baseTileTitle->setString("CLASSIC");
		baseTileTitle->Center();
		baseTileTitle->setPositionY(baseTile->getPositionY() + baseTile->GetHeight() * 0.45);
		baseTileTitle->setVisible(true);
		baseTileTitle->GetLabel()->runAction(cocos2d::FadeIn::create(0.25));

		baseTileText->setString("\n\nThe original challenge. Put your match-making skills to the ultimate test. Only 3s per round!\n(Advanced Players Only!)");
		baseTileText->Center();
		baseTileText->setPositionY(baseTile->getPositionY() + baseTile->GetHeight() * 0.45);
		baseTileText->setVisible(true);
		baseTileText->GetLabel()->runAction(cocos2d::FadeIn::create(0.25));

		menuLeft->setPositionY(baseTile->getPositionY() - baseTile->GetHeight() / 2 + baseTile->GetHeight() * 0.1);
		menuRight->setPositionY(baseTile->getPositionY() - baseTile->GetHeight() / 2 + baseTile->GetHeight() * 0.1);

		nrgFunctions::Center(leftMenuText, menuLeft);
		nrgFunctions::Center(rightMenuText, menuRight);
		menuRight->setPositionY(menuLeft->getPositionY());

		menuLeft->runAction(cocos2d::FadeIn::create(0.25));
		menuRight->runAction(cocos2d::FadeIn::create(0.25));
		leftMenuText->GetLabel()->runAction(cocos2d::FadeIn::create(0.25));
		rightMenuText->GetLabel()->runAction(cocos2d::FadeIn::create(0.25));

		menuLeft->ToggleTouch(true);
		menuRight->ToggleTouch(true);

		displayingClassic = false;

		if (bestScoreTile->getOpacity() < 255)
			bestScoreTile->runAction(cocos2d::FadeIn::create(0.25));
		if (bestScoreTileShadow->getOpacity() < 255)
			bestScoreTileShadow->runAction(cocos2d::FadeIn::create(0.25));

		scoreText->GetLabel()->setOpacity(0);
		pointsText->GetLabel()->setOpacity(0);

		scoreText->GetLabel()->runAction(cocos2d::Sequence::create(
			cocos2d::CallFunc::create([this]() {scoreText->setString("Best Score (Classic)"); nrgFunctions::Center(scoreText, bestScoreTile);}),
			cocos2d::FadeIn::create(0.5), NULL));

		pointsText->GetLabel()->runAction(cocos2d::Sequence::create(
			cocos2d::CallFunc::create([this]()
		{
			if (highestMatchClassic != 1)
				sprintf(sz, "%d points   |   %d matches", highestScoreClassic, highestMatchClassic);
			else
				sprintf(sz, "%d points   |   %d match", highestScoreClassic, highestMatchClassic);

			pointsText->setString(sz);
			pointsText->Center();
			pointsText->SetBottom(pointsTextBottomPosition);
		}),
			cocos2d::FadeIn::create(0.5), NULL));

		//Decide next score to display. Starting at hard should cycle to normal, then easy.
		if (highestMatchCountdown > 0 || highestScoreCountdown > 0)
			displayingCountdown = true;
		else if (highestMatchMemory > 0 || highestScoreMemory > 0)
			displayingMemory = true;
		else if (highestMatchPractice > 0 || highestScorePractice)
			displayingPractice = true;
	}

	if (practiceButton->WasPressed())
	{
		audio->PlayClip("double_tone_higher");

		countdownButton->ToggleEnabled(false);
		countdownText->setVisible(false);
		classicButton->ToggleEnabled(false);
		classicText->setVisible(false);
		practiceButton->ToggleEnabled(false);
		practiceText->setVisible(false);
		memoryButton->ToggleEnabled(false);
		memoryText->setVisible(false);

		startMenuButtonOn = true;
		backMenuButtonOn = true;
		selectingGameMode = true;

		mode = RUSH_RELAX; 
		scoreText->setString("Best Score (100 Matches)");

		baseTileTitle->setString("100 MATCHES");
		baseTileTitle->Center();
		baseTileTitle->setPositionY(baseTile->getPositionY() + baseTile->GetHeight() * 0.45);
		baseTileTitle->setVisible(true);
		baseTileTitle->GetLabel()->runAction(cocos2d::FadeIn::create(0.25));

		baseTileText->setString("\n\nMake all 100 matches at your own pace. There's no timer, but the faster you match, the higher your score!\n(Fun for Everyone!)");
		baseTileText->Center();
		baseTileText->setPositionY(baseTile->getPositionY() + baseTile->GetHeight() * 0.45);
		baseTileText->setVisible(true);
		baseTileText->GetLabel()->runAction(cocos2d::FadeIn::create(0.25));

		menuLeft->setPositionY(baseTile->getPositionY() - baseTile->GetHeight() / 2 + baseTile->GetHeight() * 0.05);
		menuRight->setPositionY(baseTile->getPositionY() - baseTile->GetHeight() / 2 + baseTile->GetHeight() * 0.05);

		nrgFunctions::Center(leftMenuText, menuLeft);
		nrgFunctions::Center(rightMenuText, menuRight);
		menuRight->setPositionY(menuLeft->getPositionY());

		menuLeft->runAction(cocos2d::FadeIn::create(0.25));
		menuRight->runAction(cocos2d::FadeIn::create(0.25));
		leftMenuText->GetLabel()->runAction(cocos2d::FadeIn::create(0.25));
		rightMenuText->GetLabel()->runAction(cocos2d::FadeIn::create(0.25));

		menuLeft->ToggleTouch(true);
		menuRight->ToggleTouch(true);

		displayingPractice = false;

		if (bestScoreTile->getOpacity() < 255)
			bestScoreTile->runAction(cocos2d::FadeIn::create(0.25));
		if (bestScoreTileShadow->getOpacity() < 255)
			bestScoreTileShadow->runAction(cocos2d::FadeIn::create(0.25));

		scoreText->GetLabel()->setOpacity(0);
		pointsText->GetLabel()->setOpacity(0);

		scoreText->GetLabel()->runAction(cocos2d::Sequence::create(
			cocos2d::CallFunc::create([this]() {scoreText->setString("Best Score (100 Matches)"); nrgFunctions::Center(scoreText, bestScoreTile);}),
			cocos2d::FadeIn::create(0.5), NULL));

		pointsText->GetLabel()->runAction(cocos2d::Sequence::create(
			cocos2d::CallFunc::create([this]()
		{
			if (highestMatchPractice != 1)
				sprintf(sz, "%d points   |   %d matches", highestScorePractice, highestMatchPractice);
			else
				sprintf(sz, "%d points   |   %d match", highestScorePractice, highestMatchPractice);

			pointsText->setString(sz);
			pointsText->Center();
			pointsText->SetBottom(pointsTextBottomPosition);
		}),
			cocos2d::FadeIn::create(0.5), NULL));

		//Decide next score to display. Starting at hard should cycle to normal, then easy.				
		if (highestMatchClassic > 0 || highestScoreClassic > 0)
			displayingClassic = true;
		else if (highestMatchCountdown > 0 || highestScoreCountdown)
			displayingPractice = true;
		else if (highestMatchMemory > 0 || highestScoreMemory > 0)
			displayingMemory = true;
	}

	if (memoryButton->WasPressed())
	{
		audio->PlayClip("double_tone_higher");

		countdownButton->ToggleEnabled(false);
		countdownText->setVisible(false);
		classicButton->ToggleEnabled(false);
		classicText->setVisible(false);
		practiceButton->ToggleEnabled(false);
		practiceText->setVisible(false);
		memoryButton->ToggleEnabled(false);
		memoryText->setVisible(false);

		startMenuButtonOn = true;
		backMenuButtonOn = true;
		selectingGameMode = true;

		mode = MEMORY;
		scoreText->setString("Best Score (Memory)");

		baseTileTitle->setString("MEMORY");
		baseTileTitle->Center();
		baseTileTitle->setPositionY(baseTile->getPositionY() + baseTile->GetHeight() * 0.45);
		baseTileTitle->setVisible(true);
		baseTileTitle->GetLabel()->runAction(cocos2d::FadeIn::create(0.25));

		baseTileText->setString("\n\nA true test of mental power. Memorize the 9 tiles to make successful matches. Match as much as your mind can take!\n(Advanced Players Only!)");
		baseTileText->Center();
		baseTileText->setPositionY(baseTile->getPositionY() + baseTile->GetHeight() * 0.45);
		baseTileText->setVisible(true);
		baseTileText->GetLabel()->runAction(cocos2d::FadeIn::create(0.25));

		menuLeft->setPositionY(baseTile->getPositionY() - baseTile->GetHeight() / 2 + baseTile->GetHeight() * 0.05);
		menuRight->setPositionY(baseTile->getPositionY() - baseTile->GetHeight() / 2 + baseTile->GetHeight() * 0.05);

		nrgFunctions::Center(leftMenuText, menuLeft);
		nrgFunctions::Center(rightMenuText, menuRight);
		menuRight->setPositionY(menuLeft->getPositionY());

		menuLeft->runAction(cocos2d::FadeIn::create(0.25));
		menuRight->runAction(cocos2d::FadeIn::create(0.25));
		leftMenuText->GetLabel()->runAction(cocos2d::FadeIn::create(0.25));
		rightMenuText->GetLabel()->runAction(cocos2d::FadeIn::create(0.25));

		menuLeft->ToggleTouch(true);
		menuRight->ToggleTouch(true);

		displayingMemory = false;

		if (bestScoreTile->getOpacity() < 255)
			bestScoreTile->runAction(cocos2d::FadeIn::create(0.25));
		if (bestScoreTileShadow->getOpacity() < 255)
			bestScoreTileShadow->runAction(cocos2d::FadeIn::create(0.25));

		scoreText->GetLabel()->setOpacity(0);
		pointsText->GetLabel()->setOpacity(0);

		scoreText->GetLabel()->runAction(cocos2d::Sequence::create(
			cocos2d::CallFunc::create([this]() {scoreText->setString("Best Score (Memory)"); nrgFunctions::Center(scoreText, bestScoreTile);}),
			cocos2d::FadeIn::create(0.5), NULL));

		pointsText->GetLabel()->runAction(cocos2d::Sequence::create(
			cocos2d::CallFunc::create([this]()
		{
			if (highestMatchMemory != 1)
				sprintf(sz, "%d points   |   %d matches", highestScoreMemory, highestMatchMemory);
			else
				sprintf(sz, "%d points   |   %d match", highestScoreMemory, highestMatchMemory);

			pointsText->setString(sz);
			pointsText->Center();
			pointsText->SetBottom(pointsTextBottomPosition);
		}),
			cocos2d::FadeIn::create(0.5), NULL));

		//Decide next score to display. Starting at hard should cycle to normal, then easy.
		if (highestMatchClassic > 0 || highestScoreClassic > 0)
			displayingClassic = true;
		if (highestMatchCountdown > 0 || highestScoreCountdown > 0)
			displayingCountdown = true;
		else if (highestMatchPractice > 0 || highestScorePractice)
			displayingPractice = true;

	}

	if (menuRight->WasPressed(false))
	{
		menuRight->WasPressed();
		audio->PlayClip("triple_tone_high");
		switch (mode)
		{

		case COUNTDOWN:
			director->replaceScene(Countdown::CreateScene());
			break;
		case CLASSIC:
			director->replaceScene(Classic::CreateScene());
			break;
		case RUSH_RELAX:
			director->replaceScene(RushRelax::CreateScene());
			break;
		case MEMORY:
			director->replaceScene(MemoryMode::CreateScene());
			break;
		}
	}

	if (menuLeft->WasPressed(false))
	{
		menuLeft->WasPressed();
		audio->PlayClip("double_tone_low");

		countdownButton->ToggleEnabled(true);
		countdownText->setVisible(true);
		classicButton->ToggleEnabled(true);
		classicText->setVisible(true);
		practiceButton->ToggleEnabled(true);
		practiceText->setVisible(true);
		memoryButton->ToggleEnabled(true);
		memoryText->setVisible(true);

		baseTileTitle->GetLabel()->setString("");
		baseTileText->GetLabel()->setOpacity(0);
		baseTileText->setString("Choose Game Mode");
		baseTileText->SetCenterX(center.x);
		baseTileText->GetLabel()->runAction(cocos2d::FadeIn::create(0.25));

		menuLeft->runAction(cocos2d::FadeOut::create(0.25));
		menuRight->runAction(cocos2d::FadeOut::create(0.25));
		leftMenuText->GetLabel()->runAction(cocos2d::FadeOut::create(0.25));
		rightMenuText->GetLabel()->runAction(cocos2d::FadeOut::create(0.25));

		menuLeft->ToggleTouch(false);
		menuRight->ToggleTouch(false);
		selectingGameMode = false;

		sceneTimer.SetMark(0);
	}

	if (sceneTimer.IsMarkSet() && sceneTimer.GetElapsedTime() >= sceneTimer.GetMark())
	{
		if (displayingClassic && !selectingGameMode)
		{
			displayingClassic = false;

			scoreText->GetLabel()->setOpacity(0);
			pointsText->GetLabel()->setOpacity(0);

			scoreText->GetLabel()->runAction(cocos2d::Sequence::create(
				cocos2d::CallFunc::create([this]() {scoreText->setString("Best Score (Classic)"); nrgFunctions::Center(scoreText, bestScoreTile);}),
				cocos2d::FadeIn::create(0.5), NULL));

			pointsText->GetLabel()->runAction(cocos2d::Sequence::create(
				cocos2d::CallFunc::create([this]()
			{
				if (highestMatchClassic != 1)
					sprintf(sz, "%d points   |   %d matches", highestScoreClassic, highestMatchClassic);
				else
					sprintf(sz, "%d points   |   %d match", highestScoreClassic, highestMatchClassic);

				pointsText->setString(sz);
				pointsText->Center();
				pointsText->SetBottom(pointsTextBottomPosition);
			}),
				cocos2d::FadeIn::create(0.5), NULL));

			//Decide next score to display. Starting at hard should cycle to normal, then easy.
			if (highestMatchCountdown > 0 || highestScoreCountdown > 0)
				displayingCountdown = true;
			else if (highestMatchMemory > 0 || highestScoreMemory > 0)
				displayingMemory = true;
			else if (highestMatchPractice > 0 || highestScorePractice)
				displayingPractice = true;
		}

        else if (displayingCountdown && !selectingGameMode)
		{
			displayingCountdown = false;

			scoreText->GetLabel()->setOpacity(0);
			pointsText->GetLabel()->setOpacity(0);

			scoreText->GetLabel()->runAction(cocos2d::Sequence::create(

				cocos2d::CallFunc::create([this]() {scoreText->setString("Best Score (Countdown)"); nrgFunctions::Center(scoreText, bestScoreTile);}),
				cocos2d::FadeIn::create(0.5), NULL));

			pointsText->GetLabel()->runAction(cocos2d::Sequence::create(
				cocos2d::CallFunc::create([this]()
			{
				if (highestMatchCountdown != 1)
					sprintf(sz, "%d points   |   %d matches", highestScoreCountdown, highestMatchCountdown);
				else
					sprintf(sz, "%d points   |   %d match", highestScoreCountdown, highestMatchCountdown);

				pointsText->setString(sz);
				pointsText->Center();
				pointsText->SetBottom(pointsTextBottomPosition);
			}),
				cocos2d::FadeIn::create(0.5), NULL));

			//Decide next score to display. Starting at hard should cycle to normal, then easy.
			if (highestMatchMemory > 0 || highestScoreMemory > 0)
				displayingMemory = true;
			else if (highestMatchPractice > 0 || highestScorePractice)
				displayingPractice = true;
			else if (highestMatchClassic > 0 || highestScoreClassic > 0)
				displayingClassic = true;
		}

		else if (displayingMemory && !selectingGameMode)
		{
			displayingMemory = false;

			scoreText->GetLabel()->setOpacity(0);
			pointsText->GetLabel()->setOpacity(0);

			scoreText->GetLabel()->runAction(cocos2d::Sequence::create(
				cocos2d::CallFunc::create([this]() {scoreText->setString("Best Score (Memory)"); nrgFunctions::Center(scoreText, bestScoreTile);}),
				cocos2d::FadeIn::create(0.5), NULL));

			pointsText->GetLabel()->runAction(cocos2d::Sequence::create(
				cocos2d::CallFunc::create([this]()
			{
				if (highestMatchMemory != 1)
					sprintf(sz, "%d points   |   %d matches", highestScoreMemory, highestMatchMemory);
				else
					sprintf(sz, "%d points   |   %d match", highestScoreMemory, highestMatchMemory);

				pointsText->setString(sz);
				pointsText->Center();
				pointsText->SetBottom(pointsTextBottomPosition);
			}),
				cocos2d::FadeIn::create(0.5), NULL));

			//Decide next score to display. Starting at hard should cycle to normal, then easy.
			if (highestMatchPractice > 0 || highestScorePractice)
				displayingPractice = true;
			else if (highestMatchClassic > 0 || highestScoreClassic > 0)
				displayingClassic = true;
			else if (highestMatchCountdown > 0 || highestScoreCountdown > 0)
				displayingCountdown = true;
		}

		else if (displayingPractice && !selectingGameMode)
		{
			displayingPractice = false;

			scoreText->GetLabel()->setOpacity(0);
			pointsText->GetLabel()->setOpacity(0);

			scoreText->GetLabel()->runAction(cocos2d::Sequence::create(
				cocos2d::CallFunc::create([this]() {scoreText->setString("Best Score (100 Matches)"); nrgFunctions::Center(scoreText, bestScoreTile);}),
				cocos2d::FadeIn::create(0.5), NULL));

			pointsText->GetLabel()->runAction(cocos2d::Sequence::create(
				cocos2d::CallFunc::create([this]()
			{
				if (highestMatchPractice != 1)
					sprintf(sz, "%d points   |   %d matches", highestScorePractice, highestMatchPractice);
				else
					sprintf(sz, "%d points   |   %d match", highestScorePractice, highestMatchPractice);

				pointsText->setString(sz);
				pointsText->Center();
				pointsText->SetBottom(pointsTextBottomPosition);
			}),
				cocos2d::FadeIn::create(0.5), NULL));

			//Decide next score to display. Starting at hard should cycle to normal, then easy.				
			if (highestMatchClassic > 0 || highestScoreClassic > 0)
				displayingClassic = true;
			else if (highestMatchCountdown > 0 || highestScoreCountdown)
				displayingPractice = true;
			else if (highestMatchMemory > 0 || highestScoreMemory > 0)
				displayingMemory = true;
		}

		//Turn off touch until game starts
		for (int iii = 0; iii < tiles.size(); iii++)
			tiles[iii]->ToggleTouch(false);
		sceneTimer.Restart();
		
        //Display next score after 2 seconds
        sceneTimer.SetMark(scoreDisplayLength);
	}

	if (retry->GetConfirmedSelection() == 0 || WasTouched())
	{        
		//Keep menu buttons off at first. Pressing them quickly while SDKBox runs or while the actions below run seem to cause the engine to crash
		optionsButton->runAction(cocos2d::FadeTo::create(0.25, 64));
		highScoresButton->runAction(cocos2d::FadeTo::create(0.25, 64));
		buyBonusItemsButton->runAction(cocos2d::FadeTo::create(0.25, 64));
		optionsText->GetLabel()->runAction(cocos2d::FadeTo::create(0.25, 64));
		highScoresText->GetLabel()->runAction(cocos2d::FadeTo::create(0.25, 64));
		buyBonusItemsText->GetLabel()->runAction(cocos2d::FadeTo::create(0.25, 64));
		countdownText->GetLabel()->runAction(cocos2d::FadeTo::create(0.25, 128));
		classicText->GetLabel()->runAction(cocos2d::FadeTo::create(0.25, 128));
		practiceText->GetLabel()->runAction(cocos2d::FadeTo::create(0.25, 128));
		memoryText->GetLabel()->runAction(cocos2d::FadeTo::create(0.25, 128));

		classicButton->setVisible(true);
		countdownButton->setVisible(true);
		memoryButton->setVisible(true);
		practiceButton->setVisible(true);

		if (!gameData->getBoolForKey(tutorial_complete_how_to_play, false))
			director->pushScene(Tutorial_Matching::CreateScene());

		audio->PlayClip("double_tone_higher");

        int offset = 50;
        float duration = 0.5;
        
        //Run baseTile and baseTileShadow intro animation
        baseTileShadow->setOpacity(0);
        baseTileShadow->SetTop(baseTile->getPositionY() - baseTile->GetHeight() / 2);
		titleShadow->stopAllActions();
		titleShadow->runAction(cocos2d::FadeOut::create(0.5));
        eyebrow->GetLabel()->setOpacity(0);
        title->GetLabel()->stopAllActions(); //Make sure the old label stops all actions
        title->ChangeTextSize(titleTextSize * 0.85);
        title->GetLabel()->setOpacity(0);
        title->Center();
        title->SetBottom(titleBottomPosition);
		title->stopAllActions();
		title->runAction(cocos2d::RepeatForever::create(
			cocos2d::Sequence::create(
				cocos2d::MoveBy::create(2, cocos2d::Vec2(0, 10)),
				cocos2d::MoveBy::create(2, cocos2d::Vec2(0, -10)), NULL)));
        title->GetLabel()->runAction(cocos2d::FadeIn::create(1));
        
        baseTile->setPositionY(baseTile->getPositionY() + offset);
        baseTile->runAction(cocos2d::Spawn::create(
                                                   cocos2d::FadeIn::create(duration),
                                                      cocos2d::MoveBy::create(duration, cocos2d::Vec2(0, -offset)), NULL));
        baseTileShadow->runAction(cocos2d::Sequence::create(cocos2d::DelayTime::create(duration),cocos2d::FadeIn::create(0.5), NULL));
        
		//Turn menu buttons back on after performing animation (helps prevent crashing)
		runAction(cocos2d::Sequence::create(
			cocos2d::DelayTime::create(duration * 3),
			cocos2d::CallFunc::create([this]() {
				optionsButton->ToggleTouch(true);
				optionsButton->runAction(cocos2d::FadeIn::create(0.25));
				highScoresButton->ToggleTouch(true);
				highScoresButton->runAction(cocos2d::FadeIn::create(0.25));
				buyBonusItemsButton->ToggleTouch(true);
				buyBonusItemsButton->runAction(cocos2d::FadeIn::create(0.25));
				optionsText->GetLabel()->runAction(cocos2d::FadeIn::create(0.25));
				highScoresText->GetLabel()->runAction(cocos2d::FadeIn::create(0.25));
				buyBonusItemsText->GetLabel()->runAction(cocos2d::FadeIn::create(0.25));
				classicButton->ToggleTouch(true);
				countdownButton->ToggleTouch(true);
				memoryButton->ToggleTouch(true);
				practiceButton->ToggleTouch(true);

				countdownText->GetLabel()->runAction(cocos2d::RepeatForever::create(
					cocos2d::Sequence::create(
						cocos2d::DelayTime::create(0.125),
						cocos2d::FadeTo::create(0.5, 128),
						cocos2d::FadeTo::create(0.5, 255),
						cocos2d::DelayTime::create(0.125), NULL)));

				classicText->GetLabel()->runAction(cocos2d::RepeatForever::create(
					cocos2d::Sequence::create(
						cocos2d::DelayTime::create(0.125),
						cocos2d::FadeTo::create(0.5, 128),
						cocos2d::FadeTo::create(0.5, 255),
						cocos2d::DelayTime::create(0.125), NULL)));

				practiceText->GetLabel()->runAction(cocos2d::RepeatForever::create(
					cocos2d::Sequence::create(
						cocos2d::DelayTime::create(0.125),
						cocos2d::FadeTo::create(0.5, 128),
						cocos2d::FadeTo::create(0.5, 255),
						cocos2d::DelayTime::create(0.125), NULL)));

				memoryText->GetLabel()->runAction(cocos2d::RepeatForever::create(
					cocos2d::Sequence::create(
						cocos2d::DelayTime::create(0.125),
						cocos2d::FadeTo::create(0.5, 128),
						cocos2d::FadeTo::create(0.5, 255),
						cocos2d::DelayTime::create(0.125), NULL)));
		}), NULL));
		
        baseTileText->setString("Choose Game Mode");
        baseTileText->SetCenterX(center.x);
		baseTileText->setVisible(true);
		baseTileText->GetLabel()->runAction(cocos2d::FadeIn::create(0.25));
       
		retry->ToggleOn(false);
        this->ToggleTouch(false);

		nrgFunctions::Center(scoreText, bestScoreTile);

#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
		if (!gameData->getBoolForKey(ads_removed, false))
			sdkbox::PluginChartboost::show(sdkbox::CB_Location_Default);
#endif
	}

	//Check to see if player has started game
	if (gameModeSelected)
	{
		//Make sure unwanted elements fade out if rapidly entering scene (and previous fade in actions running)
		title->GetLabel()->stopAllActions();
		bestScoreTile->stopAllActions();
		bestScoreTile->stopAllActions();
		scoreText->GetLabel()->stopAllActions();
		pointsText->GetLabel()->stopAllActions();
		eyebrow->GetLabel()->stopAllActions();
		optionsButton->stopAllActions();
		optionsText->GetLabel()->stopAllActions();
		highScoresButton->stopAllActions();
		highScoresText->GetLabel()->stopAllActions();
		buyBonusItemsButton->stopAllActions();
		buyBonusItemsText->GetLabel()->stopAllActions();

		title->GetLabel()->runAction(cocos2d::FadeOut::create(0.25));
		eyebrow->GetLabel()->runAction(cocos2d::FadeOut::create(0.25));
		bestScoreTile->setOpacity(0);
		bestScoreTile->setOpacity(0);
		scoreText->GetLabel()->runAction(cocos2d::Sequence::create(cocos2d::FadeOut::create(0.25), cocos2d::CallFunc::create([this]() {scoreText->setString("Score"); nrgFunctions::Center(scoreText, bestScoreTile);}), cocos2d::DelayTime::create(0.25), cocos2d::FadeIn::create(0.25), NULL));
		optionsButton->runAction(cocos2d::FadeOut::create(0.25));
		optionsText->GetLabel()->runAction(cocos2d::FadeOut::create(0.25));
		highScoresButton->runAction(cocos2d::FadeOut::create(0.25));
		highScoresText->GetLabel()->runAction(cocos2d::FadeOut::create(0.25));
		buyBonusItemsButton->runAction(cocos2d::FadeOut::create(0.25));
		buyBonusItemsText->GetLabel()->runAction(cocos2d::FadeOut::create(0.25));
		optionsButton->ToggleTouch(false);
		highScoresButton->ToggleTouch(false);
		buyBonusItemsButton->ToggleTouch(false);
		pointsText->GetLabel()->setOpacity(255);
	}

	return;
}

void MainMenu::RunIntro()
{
	//Stop music
	audio->StopMusic();

	//Turn everything invisible
	eyebrow->GetLabel()->setOpacity(0);
	title->GetLabel()->setOpacity(0);
	baseTile->setOpacity(0);
	baseTileShadow->setOpacity(0);
	scoreText->GetLabel()->setOpacity(0);
	bestScoreTile->setOpacity(0);
	bestScoreTileShadow->setOpacity(0);
	titleShadow->setAnchorPoint(cocos2d::Vec2(0.5, 0.5));
	titleShadow->setOpacity(0);
	pointsText->GetLabel()->setOpacity(0);
	optionsButton->setOpacity(0);
	optionsText->GetLabel()->setOpacity(0);
	highScoresButton->setOpacity(0);
	highScoresText->GetLabel()->setOpacity(0);
	buyBonusItemsButton->setOpacity(0);
	buyBonusItemsText->GetLabel()->setOpacity(0);
	leftMenuText->GetLabel()->setOpacity(0);
	rightMenuText->GetLabel()->setOpacity(0);
	menuLeft->setOpacity(0);
	menuRight->setOpacity(0);
	
	//Reset tiles
	for (int iii = 0; iii < tiles.size(); iii++)
	{
		tiles[iii]->stopAllActions();
		tiles[iii]->ToggleEnabled(false);
	}

	//Set tile locations
	topLeftTile->setPosition(topLeft.first, topLeft.second);
	topRightTile->setPosition(topRight.first, topRight.second);
	topCenterTile->setPosition(topCenter.first, topCenter.second);

	middleLeftTile->setPosition(middleLeft.first, middleLeft.second);
	middleRightTile->setPosition(middleRight.first, middleRight.second);
	middleCenterTile->setPosition(middleCenter.first, middleCenter.second);

	bottomLeftTile->setPosition(bottomLeft.first, bottomLeft.second);
	bottomRightTile->setPosition(bottomRight.first, bottomRight.second);
	bottomCenterTile->setPosition(bottomCenter.first, bottomCenter.second);

	ResetScore();
	finalScore = 0;
	displayedScore = 0;
	
	retry->ChangeMenu("Tap Here To Start", 0, false);
	retry->Center();
	retry->SetBottom(retryBottomPosition);

	//Disable touch
	retry->ToggleOn(false);

	//Intro animations
	int offset = 50;
	float duration = 0.5;
	
	eyebrow->GetLabel()->setPositionY(offset);
	eyebrow->GetLabel()->runAction(cocos2d::Spawn::create(
		cocos2d::FadeIn::create(duration),
		cocos2d::MoveBy::create(duration, cocos2d::Vec2(0, -offset)), NULL));
	
	title->GetLabel()->setPositionY(offset);

	runAction(cocos2d::Sequence::create(
		cocos2d::DelayTime::create(duration),
		cocos2d::CallFunc::create([this]() {
			title->GetLabel()->runAction(
				cocos2d::Spawn::create(
					cocos2d::FadeIn::create(0.5),
					cocos2d::MoveBy::create(0.5, cocos2d::Vec2(0, -50)), NULL));
			}),
		cocos2d::CallFunc::create([this]()
	{
		highestMatchCountdown = gameData->getIntegerForKey(best_match_countdown, 0);
		highestScoreCountdown = gameData->getIntegerForKey(best_score_countdown, 0);
		highestMatchClassic = gameData->getIntegerForKey(best_match_classic, 0);
		highestScoreClassic = gameData->getIntegerForKey(best_score_classic, 0);
		highestMatchPractice = gameData->getIntegerForKey(best_match_rushrelax, 0);
		highestScorePractice = gameData->getIntegerForKey(best_score_rushrelax, 0);
		highestMatchMemory = gameData->getIntegerForKey(best_match_memory, 0);
		highestScoreMemory = gameData->getIntegerForKey(best_score_memory, 0);

		if (highestMatchClassic != 0 || highestScoreClassic != 0)
			displayingClassic = true;
		else if (highestMatchCountdown != 0 || highestScoreCountdown != 0)
			displayingCountdown = true;
		else if (highestMatchMemory != 0 || highestScoreMemory != 0)
			displayingMemory = true;
		else if (highestMatchPractice != 0 || highestScorePractice != 0)
			displayingPractice = true;

		if (displayingClassic)
		{
			displayingClassic = false;
			displayBestScore = true;

			scoreText->GetLabel()->setOpacity(0);
			scoreText->GetLabel()->runAction(cocos2d::Sequence::create(
				cocos2d::CallFunc::create([this]() {scoreText->setString("Best Score (Classic)"); nrgFunctions::Center(scoreText, bestScoreTile);}),
				cocos2d::FadeIn::create(0.5), NULL));

			if (highestMatchClassic != 1)
				sprintf(sz, "%d points   |   %d matches", highestScoreClassic, highestMatchClassic);
			else
				sprintf(sz, "%d points   |   %d match", highestScoreClassic, highestMatchClassic);

			//Decide next score to display. Starting at hard should cycle to normal, then easy.
			if (highestMatchCountdown > 0 || highestScoreCountdown > 0)
				displayingCountdown = true;
			else if (highestMatchMemory > 0 || highestScoreMemory > 0)
				displayingMemory = true;
			else if (highestMatchPractice > 0 || highestScorePractice)
				displayingPractice = true;
		}

		else if (displayingCountdown)
		{
			displayingCountdown = false;
			displayBestScore = true;

			scoreText->GetLabel()->setOpacity(0);
			scoreText->GetLabel()->runAction(cocos2d::Sequence::create(
				cocos2d::CallFunc::create([this]() {scoreText->setString("Best Score (Countdown)"); nrgFunctions::Center(scoreText, bestScoreTile);}),
				cocos2d::FadeIn::create(0.5), NULL));

			if (highestMatchCountdown != 1)
				sprintf(sz, "%d points   |   %d matches", highestScoreCountdown, highestMatchCountdown);
			else
				sprintf(sz, "%d points   |   %d match", highestScoreCountdown, highestMatchCountdown);

			//Decide next score to display. Starting at hard should cycle to normal, then easy.
			if (highestMatchMemory > 0 || highestScoreMemory > 0)
				displayingMemory = true;
			else if (highestMatchPractice > 0 || highestScorePractice)
				displayingPractice = true;
			else if (highestMatchClassic > 0 || highestScoreClassic > 0)
				displayingClassic = true;
		}

		else if (displayingMemory)
		{
			displayingMemory = false;
			displayBestScore = true;

			scoreText->GetLabel()->setOpacity(0);
			scoreText->GetLabel()->runAction(cocos2d::Sequence::create(
				cocos2d::CallFunc::create([this]() {scoreText->setString("Best Score (Memory)"); nrgFunctions::Center(scoreText, bestScoreTile);}),
				cocos2d::FadeIn::create(0.5), NULL));

			if (highestMatchMemory != 1)
				sprintf(sz, "%d points   |   %d matches", highestScoreMemory, highestMatchMemory);
			else
				sprintf(sz, "%d points   |   %d match", highestScoreMemory, highestMatchMemory);

			//Decide next score to display. Starting at hard should cycle to normal, then easy.
			if (highestMatchPractice > 0 || highestScorePractice)
				displayingPractice = true;
			else if (highestMatchClassic > 0 || highestScoreClassic > 0)
				displayingClassic = true;
			else if (highestMatchCountdown > 0 || highestScoreCountdown > 0)
				displayingMemory = true;
		}

		else if (displayingPractice)
		{
			displayingPractice = false;
			displayBestScore = true;

			scoreText->GetLabel()->setOpacity(0);
			scoreText->GetLabel()->runAction(cocos2d::Sequence::create(
				cocos2d::CallFunc::create([this]() {scoreText->setString("Best Score (100 Matches)"); nrgFunctions::Center(scoreText, bestScoreTile);}),
				cocos2d::FadeIn::create(0.5), NULL));

			if (highestMatchPractice != 1)
				sprintf(sz, "%d points   |   %d matches", highestScorePractice, highestMatchPractice);
			else
				sprintf(sz, "%d points   |   %d match", highestScorePractice, highestMatchPractice);

			//Decide next score to display. Starting at hard should cycle to normal, then easy.				
			if (highestMatchClassic > 0 || highestScoreClassic > 0)
				displayingClassic = true;
			else if (highestMatchCountdown > 0 || highestScoreCountdown)
				displayingPractice = true;
			else if (highestMatchMemory > 0 || highestScoreMemory > 0)
				displayingMemory = true;
		}

		if (displayBestScore)
		{
			pointsText->setString(sz);
			pointsText->Center();
			pointsText->SetBottom(pointsTextBottomPosition);
			pointsText->GetLabel()->runAction(cocos2d::FadeIn::create(0.5));
			nrgFunctions::Center(scoreText, bestScoreTile);
			bestScoreTile->runAction(cocos2d::FadeIn::create(0.5));
			bestScoreTileShadow->runAction(cocos2d::FadeIn::create(0.5));
		}

        titleShadow->runAction(cocos2d::FadeIn::create(0.5));
		retry->GetItem(0)->runAction(cocos2d::RepeatForever::create(
			cocos2d::Sequence::create(
				cocos2d::DelayTime::create(0.125),
				cocos2d::FadeTo::create(0.5, 64),
				cocos2d::FadeTo::create(0.5, 255),
				cocos2d::DelayTime::create(0.125), NULL)));
		retry->ToggleOn(true);
	}), 
	cocos2d::DelayTime::create(0.5),
	cocos2d::CallFunc::create([this]() {
		//Animation sequence to make title "hover" up and down
		title->GetLabel()->runAction(cocos2d::RepeatForever::create(
			cocos2d::Sequence::create(
				cocos2d::MoveBy::create(2, cocos2d::Vec2(0, 20)),
				cocos2d::MoveBy::create(2, cocos2d::Vec2(0, -20)), NULL)));
		//Animation sequence to make shadow shrink and grow
		titleShadow->runAction(cocos2d::RepeatForever::create(
			cocos2d::Sequence::create(
				cocos2d::ScaleBy::create(2, 0.8),
				cocos2d::ScaleBy::create(2, 1.25), NULL)));
		ToggleTouch(true);
        sceneTimer.Restart();
        sceneTimer.SetMark(scoreDisplayLength);
	}), 
	NULL));
	return;
}

void MainMenu::onEnter()
{
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    sdkbox::PluginAdMob::hide("home");
    sdkbox::PluginSdkboxPlay::setListener(this);
#endif
    
	//This means the user has not gone past the initial screen (where retry is visible and reads "Tap Here to Start"
	if (retry->isVisible())
    {
		retry->ToggleTouch(true);
        ToggleTouch(true);
    }

	//This means we're returning from pressing one of the menu it buttons
	if(optionsButton->getOpacity() > 0)
	{
		optionsButton->ToggleTouch(true);
		highScoresButton->ToggleTouch(true);
		buyBonusItemsButton->ToggleTouch(true);
	}

	if (countdownButton->isVisible())
		countdownButton->ToggleTouch(true);
	if (classicButton->isVisible())
		classicButton->ToggleTouch(true);
	if (practiceButton->isVisible())
		practiceButton->ToggleTouch(true);
	if (memoryButton->isVisible())
		memoryButton->ToggleTouch(true);

	if (startMenuButtonOn || backMenuButtonOn)
	{
		menuLeft->ToggleTouch(true);
		menuRight->ToggleTouch(true);
	}
    
	Game::onEnter();

	return;
}

void MainMenu::onExit()
{
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    sdkbox::PluginSdkboxPlay::removeListener();
#endif
	countdownButton->ToggleTouch(false);
	classicButton->ToggleTouch(false);
	practiceButton->ToggleTouch(false);
	memoryButton->ToggleTouch(false);
	menuLeft->ToggleTouch(false);
	menuRight->ToggleTouch(false);
	retry->ToggleTouch(false);
    ToggleTouch(false);
    
	Game::onExit();

	return;
}

MainMenu::~MainMenu()
{
}


#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
void MainMenu::onConnectionStatusChanged(int status)
{
}

void MainMenu::onScoreSubmitted(const std::string & leaderboard_name, int score, bool maxScoreAllTime, bool maxScoreWeek, bool maxScoreToday)
{
}

void MainMenu::onIncrementalAchievementUnlocked(const std::string & achievement_name)
{
}

void MainMenu::onIncrementalAchievementStep(const std::string & achievement_name, int step)
{
}

void MainMenu::onAchievementUnlocked(const std::string & achievement_name, bool newlyUnlocked)
{
}
#endif
