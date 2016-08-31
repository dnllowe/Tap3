#pragma once
#include "pch.h"
#include "Game.h"

Game* Game::instance = NULL;
bool Game::requestReview = false;
bool Game::forcedGameOver = false;
int Game::mode = -1;

Game::Game()
{
}

cocos2d::Scene * Game::CreateScene()
{
	//Create Scene
	cocos2d::Scene* scene = cocos2d::Scene::create();

	//Create layer
	cocos2d::Layer* layer = Game::create();

	scene->addChild(layer);

	return scene;
}

bool Game::init()
{
	//Tie instance point to object instance
	instance = this;
	
    if(!gameData)
        gameData = cocos2d::UserDefault::getInstance();
    
	if (!gameData->getBoolForKey(sound_on))
		audio->MuteSound();

	//Initialize origin
	SCREEN_WIDTH = glview->getDesignResolutionSize().width;
	SCREEN_HEIGHT = glview->getDesignResolutionSize().height;
	center.x = SCREEN_WIDTH / 2;
	center.y = SCREEN_HEIGHT / 2;

	//Set clear color to white
	director->setClearColor(cocos2d::Color4F(1, 1, 1, 1));

	
	InitGameAssets();
	Reset();
	backButton->stopAllActions();
	backButton->setOpacity(0);
	gameStarted = true;
	
	return true;
}

void Game::InitGameAssets()
{
	//Create cache and batch
	nrgFunctions::CreateSpriteBatch("spritebatch", batch, cache);
	addChild(batch);

	//Establish text size and spacing
    baseTile = BaseTile::create(batch);
    nrgFunctions::ResizeForDevice(baseTile);
    
	float FRAME_WIDTH = glview->getFrameSize().width;
	float FRAME_HEIGHT = glview->getFrameSize().height;
	float screenRatio;

	if (FRAME_WIDTH > FRAME_HEIGHT)
		screenRatio = FRAME_WIDTH / FRAME_HEIGHT;
	else
		screenRatio = FRAME_HEIGHT / FRAME_WIDTH;

    //16x9
    if (screenRatio >= 1.7)
    {
        displayTextSize = 24;
        baseTileTextSize = 16;
		sequenceTextSize = 24;
        pointsTextSize = 16;
        retryTextSize = 18;
        matchMadeStringsSize = 10;
        timeRemainingTextSize = 22;
        bonusTextSize = 8;
        scoreTextSize = 16;
        topMenuButtonTextSize = 9;
		headerTileOffset = 55;
		bestMatchIconOffset = 5;
        achievementTextSize = 14;
        achievementHeaderOffset = 12;
        generateQuestionMarkTextSize = 95;
        generateTextSize = 12;
 
        if(glview->getFrameSize().width > glview->getFrameSize().height && glview->getFrameSize().width >= 1920)
        {
            baseTileMenuButtonTextSize = 10;
            achievementHeaderSize = 15;
            achievementTextSize = 12;
            sequenceTextSize = 22;
            baseTileTextSize = 15;
        }
        else if(glview->getFrameSize().height > glview->getFrameSize().width && glview->getFrameSize().height >= 1920)
        {
            baseTileMenuButtonTextSize = 10;
            achievementHeaderSize = 15;
            achievementTextSize = 12;
            sequenceTextSize = 22;
            baseTileTextSize = 15;
        }
        else
        {
            baseTileMenuButtonTextSize = 11;
            achievementHeaderSize = 17;
        }
        
    }
    
	//5:3
	else if (screenRatio >= 1.65)
	{
		displayTextSize = 22;
		baseTileTextSize = 16;
		sequenceTextSize = 24;
		pointsTextSize = 18;
		retryTextSize = 20;
		matchMadeStringsSize = 11;
		timeRemainingTextSize = 20;
		bonusTextSize = 10;
		scoreTextSize = 18;
		topMenuButtonTextSize = 10;
		baseTileMenuButtonTextSize = 12;
		achievementHeaderSize = 17;
		achievementTextSize = 14;
		achievementHeaderOffset = 10;
		generateQuestionMarkTextSize = 100;
		generateTextSize = 15;
		headerTileOffset = 40;
		bestMatchIconOffset = 2;
	}

	//16:10
	else if (screenRatio >= 1.59)
	{
		displayTextSize = 22;
		baseTileTextSize = 16;
		sequenceTextSize = 24;
		pointsTextSize = 18;
		retryTextSize = 20;
		matchMadeStringsSize = 11;
		timeRemainingTextSize = 20;
		bonusTextSize = 10;
		scoreTextSize = 18;
		topMenuButtonTextSize =10;
		baseTileMenuButtonTextSize = 12;
		achievementHeaderSize = 17;
		achievementTextSize = 14;
		achievementHeaderOffset = 10;
		generateQuestionMarkTextSize = 100;
		generateTextSize = 15;
		headerTileOffset = 40;
		bestMatchIconOffset = 2;
	}

    //3x2
    else if (screenRatio >= 1.49)
    {
        displayTextSize = 24;
        baseTileTextSize = 14;
		sequenceTextSize = 21;
        pointsTextSize = 16;
        retryTextSize = 18;
        matchMadeStringsSize = 10;
        timeRemainingTextSize = 22;
        bonusTextSize = 8;
        scoreTextSize = 16;
        topMenuButtonTextSize = 9;
        baseTileMenuButtonTextSize = 11;
        achievementHeaderSize = 16;
        achievementTextSize = 13;
        achievementHeaderOffset = 12;
        generateQuestionMarkTextSize = 95;
        generateTextSize = 13;
		headerTileOffset = 55;
		bestMatchIconOffset = 5;
    }
    
    //4x3
    else if (screenRatio >= 1.32)
    {
        displayTextSize = 28;
        baseTileTextSize = 15;
		sequenceTextSize = 22;
        pointsTextSize = 18;
        retryTextSize = 20;
        matchMadeStringsSize = 12;
        timeRemainingTextSize = 24;
        bonusTextSize = 10;
        scoreTextSize = 18;
        topMenuButtonTextSize = 10;
        baseTileMenuButtonTextSize = 12;
        achievementHeaderSize = 16;
        achievementTextSize = 14;
        achievementHeaderOffset = 14;
        generateQuestionMarkTextSize = 100;
        generateTextSize = 14;
		headerTileOffset = 55;
		bestMatchIconOffset = 5;
    }
    
    //Load game objects / assets
    
	baseTileShadow = nrgSprite::create(batch, "base_tile_shadow.png", false);
	headerTile = nrgSprite::create(batch, "header_tile.png", false);
	scoreTile = nrgSprite::create(batch, "best_score_tile.png", false);
	backButton = nrgButton::create(batch, "back_button.png", "back_button_pressed.png", false);
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

	//Bonus Item Assets
	eliminateTile = nrgSprite::create(batch, "eliminate_tile.png", false);
	timePlusTile = nrgSprite::create(batch, "time_tile.png", false);
	showMatchTile = nrgSprite::create(batch, "show_match_tile.png", false);
	newCardTile = nrgSprite::create(batch, "new_card_tile.png", false);

	bestMatchIcon = nrgSprite::create(batch, "new_best_icon.png", false);
	bestScoreIcon = nrgSprite::create(batch, "new_best_icon.png", false);

	//Prevent use of powerups until game starts
	eliminateTile->addEvents();
	timePlusTile->addEvents();
	showMatchTile->addEvents();
	newCardTile->addEvents();

	nrgFunctions::ResizeForDevice(backButton);
	nrgFunctions::ResizeForDevice(optionsButton);
	nrgFunctions::ResizeForDevice(highScoresButton);
	nrgFunctions::ResizeForDevice(buyBonusItemsButton);
	nrgFunctions::ResizeForDevice(eliminateTile);
	nrgFunctions::ResizeForDevice(timePlusTile);
	nrgFunctions::ResizeForDevice(showMatchTile);
	nrgFunctions::ResizeForDevice(newCardTile);
	nrgFunctions::ResizeForDevice(bestMatchIcon);
	nrgFunctions::ResizeForDevice(bestScoreIcon);

	backButton->addEvents();
	backButton->ToggleEnabled(false);
	backButton->setPositionX(10);
	backButton->SetTop(SCREEN_HEIGHT - 20);
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
	float baseWidth = baseTile->GetWidth();
	float baseHeight = baseTile->GetHeight();
	float baseScale = baseTile->getScale();


	//Request for review assets
	int maxWidth = baseTile->GetWidth() * 0.85;

	baseTileText = nrgText::create("Enjoying Tap 3?", "fonts/POE.ttf", baseTileTextSize, maxWidth, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER);
	rightMenuText = nrgText::create("Yes!", "fonts/alba.super.ttf", baseTileMenuButtonTextSize, maxWidth, 0, 0, cocos2d::Color4B(127, 211, 255, 255));
	leftMenuText = nrgText::create("Not really.", "fonts/alba.super.ttf", baseTileMenuButtonTextSize, maxWidth, 0, 0, cocos2d::Color4B(127, 211, 255, 255));
	tradeText = nrgText::create("Trade (100 pts)", "fonts/alba.super.ttf", baseTileMenuButtonTextSize - 3, maxWidth, 0, 0, cocos2d::Color4B(127, 211, 255, 255));
	sequenceText = nrgText::create("Sequence: 01", "fonts/POE.ttf", sequenceTextSize, maxWidth, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER);
	sequenceText->Center();
	sequenceText->GetLabel()->runAction(cocos2d::RepeatForever::create(
		cocos2d::Sequence::create(
			cocos2d::DelayTime::create(0.125),
			cocos2d::FadeTo::create(0.5, 64),
			cocos2d::FadeTo::create(0.5, 255),
			cocos2d::DelayTime::create(0.125), NULL)));
	sequenceText->setVisible(false);

	addChild(leftMenuText);
	addChild(rightMenuText);
	addChild(tradeText);
	addChild(sequenceText);

	menuLeft->addEvents();
	menuRight->addEvents();
	menuLeft->ToggleTouch(false);
	menuRight->ToggleTouch(false);

	nrgFunctions::ResizeForDevice(menuLeft);
	nrgFunctions::ResizeForDevice(menuRight);

	addChild(baseTileText);

	nrgFunctions::ResizeForDevice(baseTileShadow);
	nrgFunctions::ResizeForDevice(headerTile);
	nrgFunctions::ResizeForDevice(scoreTile);

	baseTile->setPosition(center.x, center.y);
	baseTileShadow->Center();
	headerTile->Center();
	scoreTile->Center();
	baseTileShadow->SetTop(baseTile->getPositionY() - baseHeight / 2);
	headerTile->setPositionY(baseTile->getPositionY() + baseHeight / 2 + headerTileOffset);

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

	tiles.push_back(topLeftTile);
	tiles.push_back(topCenterTile);
	tiles.push_back(topRightTile);
	tiles.push_back(middleLeftTile);
	tiles.push_back(middleCenterTile);
	tiles.push_back(middleRightTile);
	tiles.push_back(bottomLeftTile);
	tiles.push_back(bottomCenterTile);
	tiles.push_back(bottomRightTile);

	matchCriteriaText = nrgText::create("Shapes", "fonts/POE.ttf", displayTextSize, SCREEN_WIDTH, 0, 0, cocos2d::Color4B::WHITE);
	addChild(matchCriteriaText);
	nrgFunctions::Center(matchCriteriaText, headerTile);

	scoreText = nrgText::create("Score   |   Matches", "fonts/POE.ttf", scoreTextSize, SCREEN_WIDTH, 0, 0, cocos2d::Color4B::WHITE);
	addChild(scoreText);
	pointsText = nrgText::create("0 pts    |    0 matches", "fonts/POE.ttf", pointsTextSize, SCREEN_WIDTH);
	addChild(pointsText);
	pointsText->Center();
	pointsText->SetBottom(pointsTextBottomPosition);
	scoreTile->setPositionY(pointsText->getPositionY() + 5);
	nrgFunctions::Center(scoreText, scoreTile);

	timerText = nrgText::create("5.00", "fonts/POE.ttf", timeRemainingTextSize, 230, 0, 0, cocos2d::Color4B::BLACK, cocos2d::TextHAlignment::CENTER);
	addChild(timerText);
	timerText->Center();
    
    timeRemainingBottomPosition = 0.5 * (baseTile->GetTop() + headerTile->getPositionY()) - timerText->GetScaledHeight() / 2;
	timerText->SetBottom(timeRemainingBottomPosition);

	std::string achievementText;

	achievementUnlockedHeader = nrgText::create("Achievement Unlocked!", "fonts/POE.ttf", achievementHeaderSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::YELLOW, cocos2d::TextHAlignment::CENTER);
	addChild(achievementUnlockedHeader);
	achievementUnlockedHeader->Center();
	achievementUnlockedHeader->setPositionY(baseTile->GetTop() - achievementHeaderOffset);

	timePlusText = nrgText::create("0 / 0", "fonts/POE.ttf", bonusTextSize);
	eliminateText = nrgText::create("0 / 0", "fonts/POE.ttf", bonusTextSize);
	showMatchText = nrgText::create("0 / 0", "fonts/POE.ttf", bonusTextSize);
	newCardText = nrgText::create("0 / 0", "fonts/POE.ttf", bonusTextSize);

	timePlusText->GetLabel()->setHorizontalAlignment(cocos2d::TextHAlignment::CENTER);
	eliminateText->GetLabel()->setHorizontalAlignment(cocos2d::TextHAlignment::CENTER);
	showMatchText->GetLabel()->setHorizontalAlignment(cocos2d::TextHAlignment::CENTER);
	newCardText->GetLabel()->setHorizontalAlignment(cocos2d::TextHAlignment::CENTER);

	addChild(timePlusText);
	addChild(eliminateText);
	addChild(showMatchText);
	addChild(newCardText);

	int bonusOffset = 15;

	timePlusStartingY = scoreTile->getPositionY() + scoreTile->GetHeight() + 7;
	newCardStartingY = timePlusStartingY;
	eliminateStartingY = timePlusStartingY;
	showMatchStartingY = timePlusStartingY;

    if(mode != RUSH_RELAX && mode != CLASSIC)
    {
        newCardStartingX = center.x + bonusOffset / 2;
        showMatchStartingX = center.x - bonusOffset / 2;
    }
    
    else if(mode == RUSH_RELAX || mode == CLASSIC)
    {
        newCardStartingX = center.x - newCardTile->GetWidth() / 2;
        showMatchStartingX = center.x - showMatchTile->GetWidth() / 2 - bonusOffset;
    }
    
	newCardTile->setPosition(newCardStartingX, newCardStartingY);
	showMatchTile->setPositionY(showMatchStartingY);
	showMatchTile->SetRightSide(showMatchStartingX);

	timePlusStartingX = showMatchTile->getPositionX() - bonusOffset - timePlusTile->GetWidth();
	eliminateStartingX = newCardTile->getPositionX() + bonusOffset + eliminateTile->GetWidth();
	
	timePlusTile->setPosition(timePlusStartingX, timePlusStartingY);	
	eliminateTile->setPosition(eliminateStartingX, eliminateStartingY);

	timePlusText->setPositionX(timePlusTile->getPositionX() + timePlusTile->GetWidth() / 2 - timePlusText->GetScaledWidth() / 2);
	timePlusText->SetBottom(timePlusTile->GetTop());

	eliminateText->setPositionX(eliminateTile->getPositionX() + eliminateTile->GetWidth() / 2 - eliminateText->GetScaledWidth() / 2);
	eliminateText->SetBottom(eliminateTile->GetTop());

	newCardText->setPositionX(newCardTile->getPositionX() + newCardTile->GetWidth() / 2 - newCardText->GetScaledWidth() / 2);
	newCardText->SetBottom(newCardTile->GetTop());

	showMatchText->setPositionX(showMatchTile->getPositionX() + showMatchTile->GetWidth() / 2 - showMatchText->GetScaledWidth() / 2);
	showMatchText->SetBottom(showMatchTile->GetTop());

	matchMadeList = nrgList::create("", "fonts/POE.ttf", matchMadeStringsSize, 0, cocos2d::Color4B::WHITE);
	matchMadeList->CenterText();
	matchMadeList->setAnchorPoint(cocos2d::Vec2(0, 0));

	bestMatchIcon->Center();
	bestScoreIcon->Center();

	bestMatchIcon->SetTop(timerText->GetBottom() - bestMatchIconOffset);
	bestScoreIcon->setPositionY(scoreTile->GetTop() + 5);

	int numberOfLabels = matchMadeList->GetNumberOfItems();
	for (int iii = 0; iii < numberOfLabels; iii++)
		matchMadeList->GetItem(iii)->setOpacity(0);
	matchMadeList->setVisible(false);
	addChild(matchMadeList);

	generateQuestionMark = nrgText::create("?", "fonts/POE.ttf", generateQuestionMarkTextSize, maxWidth, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER);
	generateQuestionMark->Center();
	generateQuestionMark->GetLabel()->runAction(cocos2d::RepeatForever::create(
		cocos2d::Sequence::create(
			cocos2d::DelayTime::create(0.125),
			cocos2d::FadeTo::create(0.5, 64),
			cocos2d::FadeTo::create(0.5, 255),
			cocos2d::DelayTime::create(0.125), NULL)));
	generateQuestionMark->setVisible(false);
	addChild(generateQuestionMark);

	generateText = nrgText::create("", "fonts/POE.ttf", generateTextSize, maxWidth, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER);
	generateText->GetLabel()->setOpacity(0);
	addChild(generateText);

	generateTimePlusThumbnail = nrgSprite::create(batch, "generate_time_tile.png", false);
	generateEliminateThumbnail = nrgSprite::create(batch, "generate_eliminate_tile.png", false);
	generateShowMatchThumbnail = nrgSprite::create(batch, "generate_show_match_tile.png", false);
	generateNewCardThumbnail = nrgSprite::create(batch, "generate_new_card_tile.png", false);

	generateTimePlusThumbnail->setOpacity(0);
	generateEliminateThumbnail->setOpacity(0);
	generateShowMatchThumbnail->setOpacity(0);
	generateNewCardThumbnail->setOpacity(0);

	generateTimePlusThumbnail->setVisible(false);
	generateEliminateThumbnail->setVisible(false);
	generateShowMatchThumbnail->setVisible(false);
	generateNewCardThumbnail->setVisible(false);

	nrgFunctions::ResizeForDevice(generateTimePlusThumbnail);
	nrgFunctions::ResizeForDevice(generateEliminateThumbnail);
	nrgFunctions::ResizeForDevice(generateShowMatchThumbnail);
	nrgFunctions::ResizeForDevice(generateNewCardThumbnail);

	generateTimePlusThumbnail->Center(0, 13);
	generateEliminateThumbnail->Center(0, 13);
	generateShowMatchThumbnail->Center(0, 13);
	generateNewCardThumbnail->Center(0, 13);

	generateText->Center();
	generateText->setPositionY(generateTimePlusThumbnail->getPositionY() - 10);

	//Retry menu
	retry = nrgMenu::create("Try Again", "fonts/alba.regular.ttf", 0, retryTextSize, 0, cocos2d::Color4B::BLACK, cocos2d::Color4B::YELLOW, false);
	addChild(retry);
	retry->setVisible(false);
	retry->Center();
	retry->SetBottom(retryBottomPosition);

	for (int iii = 0; iii < tiles.size(); iii++)
		tiles[iii]->ToggleTouch(false);

	retry->GetItem(0)->runAction(cocos2d::RepeatForever::create(
		cocos2d::Sequence::create(
			cocos2d::DelayTime::create(0.125),
			cocos2d::FadeTo::create(0.5, 64),
			cocos2d::FadeTo::create(0.5, 255),
			cocos2d::DelayTime::create(0.125), NULL)));

	return;
}

void Game::DisplaySequence()
{
	allowRerandomize = false;
	dismissTiles = false;
	disablePowerups = false;
	UpdateHeaderText("Get Ready!");

	for (int iii = 0; iii < tiles.size(); iii++)
		tiles[iii]->DismissTile();

	if (sequence < 10)
		sprintf(sz, "Sequence: 0%d", sequence);
	else if(sequence)
		sprintf(sz, "Sequence: %d", sequence);

	if (sequence == 99)
	{
		sprintf(sz, "Sequence:\nFinal");
		sequenceText->setString(sz);
		sequenceText->Center();
	}

	sequenceText->setString(sz);
	sequenceText->setVisible(true);

	countdownTimer.Pause();
	sceneTimer.Pause();

	if (mode == COUNTDOWN)
	{
		speedTimer_5_matches.Pause();
		speedTimer_10_matches.Pause();
		speedTimer_20_matches.Pause();
	}

	if (bonusEliminateLeft > 0)
	{
		eliminateTile->runAction(cocos2d::FadeTo::create(0.25, 64));
		eliminateText->GetLabel()->runAction(cocos2d::FadeTo::create(0.25, 64));
	}

	if (bonusShowMatchLeft > 0)
	{
		showMatchTile->runAction(cocos2d::FadeTo::create(0.25, 64));
		showMatchText->GetLabel()->runAction(cocos2d::FadeTo::create(0.25, 64));
	}

	if (bonusTimePlusLeft > 0)
	{
		timePlusTile->runAction(cocos2d::FadeTo::create(0.25, 64));
		timePlusText->GetLabel()->runAction(cocos2d::FadeTo::create(0.25, 64));
	}

	if (bonusNewCardLeft > 0)
	{
		newCardTile->runAction(cocos2d::FadeTo::create(0.25, 64));
		newCardText->GetLabel()->runAction(cocos2d::FadeTo::create(0.25, 64));
	}

	sequenceTimer.Restart();
	sequenceTimer.SetMark(3000);

	return;
}

void Game::CheckSequence()
{
	//Set sequence

	if (mode == CLASSIC)
	{
		switch (matches)
		{
		case 0:
			sequence = 1;
			displaySequence = true;
			break;
		case 25:
			sequence = 2;
			displaySequence = true;
			break;
		case 50:
			sequence = 3;
			displaySequence = true;
			break;
		case 75:
			sequence = 99;
			displaySequence = true;
			break;
		}
	}

	else if (mode == COUNTDOWN)
	{
		switch (matches)
		{
		case 0:
			sequence = 1;
			displaySequence = true;
			break;
		case 10:
			sequence = 2;
			displaySequence = true;
			break;
		case 20:
			sequence = 3;
			displaySequence = true;
			break;
		case 26:
			sequence = 4;
			displaySequence = true;
			break;
		case 30:
			sequence = 5;
			displaySequence = true;
			break;
		case 40:
			sequence = 6;
			displaySequence = true;
			break;
		case 50:
			sequence = 7;
			displaySequence = true;
			break;
		case 60:
			sequence = 8;
			displaySequence = true;
			break;
		case 70:
			sequence = 9;
			displaySequence = true;
			break;
		case 80:
			sequence = 10;
			displaySequence = true;
			break;
		case 90:
			sequence = 11;
			displaySequence = true;
			break;
		case 95:
			sequence = 99;
			displaySequence = true;
			break;
		}
	}

	else if (mode == MEMORY)
	{
		switch (matches)
		{
		case 0:
			sequence = 1;
			displaySequence = true;
			break;
		case 10:
			sequence = 2;
			displaySequence = true;
			break;
		case 20:
			sequence = 3;
			displaySequence = true;
			break;
		case 30:
			sequence = 4;
			displaySequence = true;
			break;
		case 40:
			sequence = 5;
			displaySequence = true;
			break;
		case 50:
			sequence = 6;
			displaySequence = true;
			break;
		case 60:
			sequence = 7;
			displaySequence = true;
			break;
		case 70:
			sequence = 8;
			displaySequence = true;
			break;
		case 75:
			sequence = 9;
			displaySequence = true;
			break;
		case 80:
			sequence = 10;
			displaySequence = true;
			break;
		case 90:
			sequence = 11;
			displaySequence = true;
			break;
		case 100:
			sequence = 12;
			displaySequence = true;
			break;
		case 101:
			sequence = 99;
			displaySequence = true;
			break;
		}
	}

	else if (mode == RUSH_RELAX)
	{
		switch (matches)
		{
		case 0:
			sequence = 1;
			displaySequence = true;
			break;
		case 10:
			sequence = 2;
			displaySequence = true;
			break;
		case 20:
			sequence = 3;
			displaySequence = true;
			break;
		case 26:
			sequence = 4;
			displaySequence = true;
			break;
		case 30:
			sequence = 5;
			displaySequence = true;
			break;
		case 40:
			sequence = 6;
			displaySequence = true;
			break;
		case 50:
			sequence = 7;
			displaySequence = true;
			break;
		case 60:
			sequence = 8;
			displaySequence = true;
			break;
		case 70:
			sequence = 9;
			displaySequence = true;
			break;
		case 80:
			sequence = 10;
			displaySequence = true;
			break;
		case 90:
			sequence = 11;
			displaySequence = true;
			break;
		case 95:
			sequence = 99;
			displaySequence = true;
			break;
		}
	}
    
	return;
}

void Game::GenerateBonus()
{
    
    //Must deal with timers before popping new scene
    
    //Pause Timers.
    countdownTimer.Stop();
    sceneTimer.Stop();
    sequenceTimer.Stop();
    matchTimer.Stop();
    
    if (mode == COUNTDOWN)
    {
        speedTimer_5_matches.Pause();
        speedTimer_10_matches.Pause();
        speedTimer_20_matches.Pause();
    }
    
    //Complete tutorial if first time receiving bonus item
	if (!gameData->getBoolForKey(tutorial_complete_using_bonus_items, false))
	{
		timePlusTile->ToggleTouch(false);
		director->pushScene(Tutorial_Bonus::CreateScene());
	}

	allowRerandomize = false;
	dismissTiles = false;
	disablePowerups = false;
	UpdateHeaderText("Bonus Earned!");

	for (int iii = 0; iii < tiles.size(); iii++)
		tiles[iii]->DismissTile();

	if (mode == MEMORY)
	{
		if (matches <= 100)
		{
			difficulty = matches;
			timelimit = 15000 - (difficulty % 10) * 1111;
		}
		else
			timelimit = 5000;

		msLeft = timelimit;
		timerText->GetLabel()->setTextColor(cocos2d::Color4B::BLACK);
		timerText->GetLabel()->runAction(cocos2d::FadeIn::create(0.5));
	}

	int bonusMin;
	int bonusMax;

	bonusMax = 3;

	if (mode == RUSH_RELAX || mode == CLASSIC)
		bonusMin = ELIMINATE;
	else
		bonusMin = TIME;

	randomizedBonus = nrgFunctions::GetRandom(bonusMin, bonusMax, lastBonus);
    lastBonus = randomizedBonus;
	generateQuestionMark->setVisible(true);
	baseTileText->setString("BONUS EARNED!");
	baseTileText->GetLabel()->setVisible(true);
	baseTileText->Center();
	baseTileText->setPositionY(baseTile->getPositionY() + baseTile->GetHeight() * 0.425);
	baseTileText->GetLabel()->runAction(cocos2d::FadeIn::create(0.25));
	bonusTimer.Start();
	bonusTimer.SetMark(3000);
	
	if (bonusEliminateLeft > 0)
	{
		eliminateTile->runAction(cocos2d::FadeTo::create(0.25, 64));
		eliminateText->GetLabel()->runAction(cocos2d::FadeTo::create(0.25, 64));
	}

	if (bonusShowMatchLeft > 0)
	{
		showMatchTile->runAction(cocos2d::FadeTo::create(0.25, 64));
		showMatchText->GetLabel()->runAction(cocos2d::FadeTo::create(0.25, 64));
	}

	if (bonusTimePlusLeft > 0)
	{
		timePlusTile->runAction(cocos2d::FadeTo::create(0.25, 64));
		timePlusText->GetLabel()->runAction(cocos2d::FadeTo::create(0.25, 64));
	}

	if (bonusNewCardLeft > 0)
	{
		newCardTile->runAction(cocos2d::FadeTo::create(0.25, 64));
		newCardText->GetLabel()->runAction(cocos2d::FadeTo::create(0.25, 64));
	}

	if(mode == MEMORY)
		UpdateTimer();

	generateBonus = false;

	return;
}

void Game::DisplayBonus()
{
	generateQuestionMark->setVisible(false);
	bonusTimer.Reset();
	bonusTimer.ResetMark();
	confirmingBonus = true;

	//Allow designated bonus if testing
	if (testing && testingBonus > 0)
		randomizedBonus = testingBonus;

	//Add appropriate bonus item
	switch (randomizedBonus)
	{
	case TIME: //Time Plus
		baseTileText->setString("Earned Time Plus Bonus");
		generateTimePlusThumbnail->setVisible(true);
		generateTimePlusThumbnail->runAction(cocos2d::FadeIn::create(0.25));
		generateText->setString("Adds 10.00s to clock");
		break;
	case ELIMINATE: //Eliminate 
		baseTileText->setString("Earned Eliminate Bonus");
		generateEliminateThumbnail->setVisible(true);
		generateEliminateThumbnail->runAction(cocos2d::FadeIn::create(0.25));
		generateText->setString("Removes two non-matching tiles");
		break;
	case SHOW: //Show Match
		baseTileText->setString("Earned Show Match Bonus");
		generateShowMatchThumbnail->setVisible(true);
		generateShowMatchThumbnail->runAction(cocos2d::FadeIn::create(0.25));
		generateText->setString("Reveals one matching tiles");
		break;
	case NEW: //New Card
		baseTileText->setString("Earned Reset Bonus");
		generateNewCardThumbnail->setVisible(true);
		generateNewCardThumbnail->runAction(cocos2d::FadeIn::create(0.25));
		generateText->setString("Creates a new set of tiles and matching pattern");
		break;
	}

	baseTileText->Center();
	baseTileText->setPositionY(baseTile->getPositionY() + baseTile->GetHeight() * 0.425);
	generateText->GetLabel()->runAction(cocos2d::FadeIn::create(0.5));
	generateText->Center();
	generateText->setPositionY(generateTimePlusThumbnail->getPositionY() - 10);

	menuLeft->ToggleTouch(true);
	menuRight->ToggleTouch(true);

	//Left side boxes
	menuLeft->setPositionY(baseTile->getPositionY() - baseTile->GetHeight() / 2 + 8);
	menuLeft->setPositionX(baseTile->getPositionX() - baseTile->GetWidth() / 2 + baseTile->GetWidth() * 0.25 - menuLeft->GetWidth() / 2);

	//Righ side boxes
	menuRight->setPositionY(baseTile->getPositionY() - baseTile->GetHeight() / 2 + 8);
	menuRight->SetRightSide(baseTile->getPositionX() + baseTile->GetWidth() / 2 - baseTile->GetWidth() * 0.25 + menuRight->GetWidth() / 2);

	rightMenuText->setString("Keep!");
	tradeText->setString("Trade (100 pts)");

	nrgFunctions::Center(rightMenuText, menuRight, -1, 1);
	nrgFunctions::Center(tradeText, menuLeft, -1, 1);
	menuRight->setPositionY(menuLeft->getPositionY());

	menuLeft->setVisible(true);
	menuRight->setVisible(true);
	rightMenuText->GetLabel()->setVisible(true);
	tradeText->GetLabel()->setVisible(true);

	baseTileText->GetLabel()->runAction(cocos2d::FadeIn::create(0.5));
	menuLeft->runAction(cocos2d::FadeIn::create(0.5));
	menuRight->runAction(cocos2d::FadeIn::create(0.5));
	rightMenuText->GetLabel()->runAction(cocos2d::FadeIn::create(0.5));
	tradeText->GetLabel()->runAction(cocos2d::FadeIn::create(0.5));

	return;
}

void Game::AcceptBonus()
{
	audio->PlayClip("double_tone_higher");

	//Add bonus item to inventory
	confirmingBonus = false;

	//Add appropriate bonus item
	switch (randomizedBonus)
	{
	case 0: //Time Plus
		generateTimePlusThumbnail->runAction(cocos2d::Sequence::create(cocos2d::FadeOut::create(0.25), cocos2d::CallFunc::create([this]() {generateTimePlusThumbnail->setVisible(false);}), NULL));
		AddTimePlusBonus();
		break;
	case 1: //Eliminate 
		generateEliminateThumbnail->runAction(cocos2d::Sequence::create(cocos2d::FadeOut::create(0.25), cocos2d::CallFunc::create([this]() {generateEliminateThumbnail->setVisible(false);}), NULL));
		AddEliminateBonus();
		break;
	case 2: //Show Match
		generateShowMatchThumbnail->runAction(cocos2d::Sequence::create(cocos2d::FadeOut::create(0.25), cocos2d::CallFunc::create([this]() {generateShowMatchThumbnail->setVisible(false);}), NULL));
		AddShowMatchBonus();
		break;
	case 3: //New Card
		generateNewCardThumbnail->runAction(cocos2d::Sequence::create(cocos2d::FadeOut::create(0.25), cocos2d::CallFunc::create([this]() {generateNewCardThumbnail->setVisible(false);}), NULL));
		AddNewCardBonus();
		break;
	}

	baseTileText->GetLabel()->stopAllActions();
	menuLeft->stopAllActions();
	menuRight->stopAllActions();
	rightMenuText->GetLabel()->stopAllActions();
	tradeText->GetLabel()->stopAllActions();
	generateText->GetLabel()->stopAllActions();

	generateText->GetLabel()->runAction(cocos2d::FadeOut::create(0.25));
	baseTileText->GetLabel()->runAction(cocos2d::FadeOut::create(0.25));
	menuLeft->runAction(cocos2d::Sequence::create(cocos2d::FadeOut::create(0.25), cocos2d::CallFunc::create([this]() {menuLeft->setVisible(false);}), NULL));
	menuRight->runAction(cocos2d::Sequence::create(cocos2d::FadeOut::create(0.25), cocos2d::CallFunc::create([this]() {menuRight->setVisible(false);}), NULL));
	rightMenuText->GetLabel()->runAction(cocos2d::Sequence::create(cocos2d::FadeOut::create(0.25), cocos2d::CallFunc::create([this]() {rightMenuText->GetLabel()->setVisible(false);}), NULL));
	tradeText->GetLabel()->runAction(cocos2d::Sequence::create(cocos2d::FadeOut::create(0.25), cocos2d::CallFunc::create([this]() {tradeText->GetLabel()->setVisible(false);}), NULL));
	menuLeft->ToggleTouch(false);
	menuRight->ToggleTouch(false);

    if(mode != CLASSIC)
        countdownTimer.Restart();
	sceneTimer.Restart();
	matchTimer.Restart();
	sequenceTimer.Restart();

	if (mode == COUNTDOWN)
	{
		speedTimer_5_matches.Unpause();
		speedTimer_10_matches.Unpause();
		speedTimer_20_matches.Unpause();
	}

	DisplayMatchSummary();
	allowRerandomize = true;
	randomizeTiles = true;
	sceneTimer.SetMark(0);
	gameData->setIntegerForKey(bonuses_accepted, gameData->getIntegerForKey(bonuses_accepted, 0) + 1);

	return;
}

void Game::TradeBonus()
{
	audio->PlayClip("double_tone_higher");

	//Turn bonus item into points. 
	extraPoints += 100;
	CalculateScore();
	UpdateScore();

	//Display match summary code, but just points added
	confirmingBonus = false;

	baseTileText->GetLabel()->stopAllActions();
	menuLeft->stopAllActions();
	menuRight->stopAllActions();
	rightMenuText->GetLabel()->stopAllActions();
	tradeText->GetLabel()->stopAllActions();
	generateText->GetLabel()->stopAllActions();

	generateText->GetLabel()->runAction(cocos2d::FadeOut::create(0.25));
	baseTileText->GetLabel()->runAction(cocos2d::FadeOut::create(0.25));
	menuLeft->runAction(cocos2d::Sequence::create(cocos2d::FadeOut::create(0.25), cocos2d::CallFunc::create([this]() {menuLeft->setVisible(false);}), NULL));
	menuRight->runAction(cocos2d::Sequence::create(cocos2d::FadeOut::create(0.25), cocos2d::CallFunc::create([this]() {menuRight->setVisible(false);}), NULL));
	rightMenuText->GetLabel()->runAction(cocos2d::Sequence::create(cocos2d::FadeOut::create(0.25), cocos2d::CallFunc::create([this]() {rightMenuText->GetLabel()->setVisible(false);}), NULL));
	tradeText->GetLabel()->runAction(cocos2d::Sequence::create(cocos2d::FadeOut::create(0.25), cocos2d::CallFunc::create([this]() {tradeText->GetLabel()->setVisible(false);}), NULL));
	menuLeft->ToggleTouch(false);
	menuRight->ToggleTouch(false);

	switch (randomizedBonus)
	{
	case 0: //Time Plus
		generateTimePlusThumbnail->runAction(cocos2d::Sequence::create(cocos2d::FadeOut::create(0.25), cocos2d::CallFunc::create([this]() {generateTimePlusThumbnail->setVisible(false);}), NULL));
		break;
	case 1: //Eliminate 
		generateEliminateThumbnail->runAction(cocos2d::Sequence::create(cocos2d::FadeOut::create(0.25), cocos2d::CallFunc::create([this]() {generateEliminateThumbnail->setVisible(false);}), NULL));
		break;
	case 2: //Show Match
		generateShowMatchThumbnail->runAction(cocos2d::Sequence::create(cocos2d::FadeOut::create(0.25), cocos2d::CallFunc::create([this]() {generateShowMatchThumbnail->setVisible(false);}), NULL));
		break;
	case 3: //New Card
		generateNewCardThumbnail->runAction(cocos2d::Sequence::create(cocos2d::FadeOut::create(0.25), cocos2d::CallFunc::create([this]() {generateNewCardThumbnail->setVisible(false);}), NULL));
		break;
	}

    if(mode != CLASSIC)
        countdownTimer.Restart();
	sceneTimer.Restart();
	matchTimer.Restart();
	sequenceTimer.Restart();

	if (mode == COUNTDOWN)
	{
		speedTimer_5_matches.Unpause();
		speedTimer_10_matches.Unpause();
		speedTimer_20_matches.Unpause();
	}

	DisplayMatchSummary();
	allowRerandomize = true;
	randomizeTiles = true;
	sceneTimer.SetMark(0);
	gameData->setIntegerForKey(trades_accepted, gameData->getIntegerForKey(trades_accepted, 0) + 1);

	return;
}

void Game::ShowHighScores()
{
#if(CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	if (!sdkbox::PluginSdkboxPlay::isSignedIn() && !testing)
		sdkbox::PluginSdkboxPlay::signin();
	sdkbox::PluginSdkboxPlay::showLeaderboard("high_score_classic");
#endif

#if(CC_TARGET_PLATFORM != CC_PLATFORM_IOS)
	director->pushScene(HighScores::CreateScene());
#endif

return;
}

void Game::SyncAchievements()
{
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	if (sdkbox::PluginSdkboxPlay::isSignedIn())
	{
		if (gameData->getBoolForKey(first_match, false) && !gameData->getBoolForKey(first_match_submitted, false))
		{
			sdkbox::PluginSdkboxPlay::unlockAchievement("first_match");
			gameData->setBoolForKey(first_match_submitted, true);
		}
		if (gameData->getBoolForKey(near_miss, false) && !gameData->getBoolForKey(near_miss_submitted, false))
		{
			sdkbox::PluginSdkboxPlay::unlockAchievement("just_in_time");
			gameData->setBoolForKey(near_miss_submitted, true);
		}
		if (gameData->getBoolForKey(ten_consecutive_near_misses, false) && !gameData->getBoolForKey(ten_consecutive_near_misses_submitted, false))
		{
			sdkbox::PluginSdkboxPlay::unlockAchievement("serial_procrastinator");
			gameData->setBoolForKey(ten_consecutive_near_misses_submitted, true);
		}
		if (gameData->getBoolForKey(take_the_points, false) && !gameData->getBoolForKey(take_the_points_submitted, false))
		{
			sdkbox::PluginSdkboxPlay::unlockAchievement("take_the_points");
			gameData->setBoolForKey(take_the_points_submitted, true);
		}
		if (gameData->getBoolForKey(take_the_bonus, false) && !gameData->getBoolForKey(take_the_bonus_submitted, false))
		{
			sdkbox::PluginSdkboxPlay::unlockAchievement("take_the_bonus");
			gameData->setBoolForKey(take_the_bonus_submitted, true);
		}
		if (gameData->getBoolForKey(all_the_time_in_the_world, false) && !gameData->getBoolForKey(all_the_time_in_the_world_submitted, false))
		{
			sdkbox::PluginSdkboxPlay::unlockAchievement("all_the_time_in_the_world");
			gameData->setBoolForKey(all_the_time_in_the_world_submitted, true);
		}
		if (gameData->getBoolForKey(process_of_elimination, false) && !gameData->getBoolForKey(process_of_elimination_submitted, false))
		{
			sdkbox::PluginSdkboxPlay::unlockAchievement("process_of_elimination");
			gameData->setBoolForKey(process_of_elimination_submitted, true);
		}
		if (gameData->getBoolForKey(seeing_is_believing, false) && !gameData->getBoolForKey(seeing_is_believing_submitted, false))
		{
			sdkbox::PluginSdkboxPlay::unlockAchievement("seeing_is_believing");
			gameData->setBoolForKey(seeing_is_believing_submitted, true);
		}
		if (gameData->getBoolForKey(clean_slate, false) && !gameData->getBoolForKey(clean_slate_submitted, false))
		{
			sdkbox::PluginSdkboxPlay::unlockAchievement("clean_slate");
			gameData->setBoolForKey(clean_slate_submitted, true);
		}
		if (gameData->getBoolForKey(two_hundred_matches_in_one_day, false) && !gameData->getBoolForKey(two_hundred_matches_in_one_day_submitted, false))
		{
			sdkbox::PluginSdkboxPlay::unlockAchievement("match_marathon");
			gameData->setBoolForKey(two_hundred_matches_in_one_day_submitted, true);
		}
		if (gameData->getBoolForKey(five_hundred_color_matches, false) && !gameData->getBoolForKey(five_hundred_color_matches_submitted, false))
		{
			sdkbox::PluginSdkboxPlay::unlockAchievement("color_matching_pro");
			gameData->setBoolForKey(five_hundred_color_matches_submitted, true);
		}
		if (gameData->getBoolForKey(five_hundred_shape_matches, false) && !gameData->getBoolForKey(five_hundred_shape_matches_submitted, false))
		{
			sdkbox::PluginSdkboxPlay::unlockAchievement("shape_matching_pro");
			gameData->setBoolForKey(five_hundred_shape_matches_submitted, true);
		}
		if (gameData->getBoolForKey(five_hundred_style_matches, false) && !gameData->getBoolForKey(five_hundred_style_matches_submitted, false))
		{
			sdkbox::PluginSdkboxPlay::unlockAchievement("style_matching_pro");
			gameData->setBoolForKey(five_hundred_style_matches_submitted, true);
		}
		if (gameData->getBoolForKey(matchmaker_training, false) && !gameData->getBoolForKey(matchmaker_training_submitted, false))
		{
			sdkbox::PluginSdkboxPlay::unlockAchievement("matchmaker_training");
			gameData->setBoolForKey(matchmaker_training_submitted, true);
		}
		if (gameData->getBoolForKey(matchmaker_junior, false) && !gameData->getBoolForKey(matchmaker_junior_submitted, false))
		{
			sdkbox::PluginSdkboxPlay::unlockAchievement("matchmaker_junior");
			gameData->setBoolForKey(matchmaker_junior_submitted, true);
		}
		if (gameData->getBoolForKey(classically_trained, false) && !gameData->getBoolForKey(classically_trained_submitted, false))
		{
			sdkbox::PluginSdkboxPlay::unlockAchievement("classically_trained");
			gameData->setBoolForKey(classically_trained_submitted, true);
		}
		if (gameData->getBoolForKey(time_management, false) && !gameData->getBoolForKey(time_management_submitted, false))
		{
			sdkbox::PluginSdkboxPlay::unlockAchievement("time_management");
			gameData->setBoolForKey(time_management_submitted, true);
		}
		if (gameData->getBoolForKey(practice_makes_perfect, false) && !gameData->getBoolForKey(practice_makes_perfect_submitted, false))
		{
			sdkbox::PluginSdkboxPlay::unlockAchievement("practice_makes_perfect");
			gameData->setBoolForKey(practice_makes_perfect_submitted, true);
		}
		if (gameData->getBoolForKey(photographic_memory, false) && !gameData->getBoolForKey(photographic_memory_submitted, false))
		{
			sdkbox::PluginSdkboxPlay::unlockAchievement("photographic_memory");
			gameData->setBoolForKey(photographic_memory_submitted, true);
		}
		if (gameData->getBoolForKey(matchmaker_seasoned, false) && !gameData->getBoolForKey(matchmaker_seasoned_submitted, false))
		{
			sdkbox::PluginSdkboxPlay::unlockAchievement("matchmaker_seasoned");
			gameData->setBoolForKey(matchmaker_seasoned_submitted, true);
		}
		if (gameData->getBoolForKey(earned_10000_total_matches, false) && !gameData->getBoolForKey(earned_10000_total_matches_submitted, false))
		{
			sdkbox::PluginSdkboxPlay::unlockAchievement("match_maker");
			gameData->setBoolForKey(earned_10000_total_matches_submitted, true);
		}
		if (gameData->getBoolForKey(the_10000_club, false) && !gameData->getBoolForKey(the_10000_club_submitted, false))
		{
			sdkbox::PluginSdkboxPlay::unlockAchievement("the_10000_club");
			gameData->setBoolForKey(the_10000_club_submitted, true);
		}
		if (gameData->getBoolForKey(aspiring_star, false) && !gameData->getBoolForKey(aspiring_star_submitted, false))
		{
			sdkbox::PluginSdkboxPlay::unlockAchievement("aspiring_star");
			gameData->setBoolForKey(aspiring_star_submitted, true);
		}
		if (gameData->getBoolForKey(six_figures, false) && !gameData->getBoolForKey(six_figures_submitted, false))
		{
			sdkbox::PluginSdkboxPlay::unlockAchievement("six_figures");
			gameData->setBoolForKey(six_figures_submitted, true);
		}
		if (gameData->getBoolForKey(the_500000_club, false) && !gameData->getBoolForKey(the_500000_club_submitted, false))
		{
			sdkbox::PluginSdkboxPlay::unlockAchievement("the_500000_club");
			gameData->setBoolForKey(the_500000_club_submitted, true);
		}
		if (gameData->getBoolForKey(earned_1000000_total_points, false) && !gameData->getBoolForKey(earned_1000000_total_points_submitted, false))
		{
			sdkbox::PluginSdkboxPlay::unlockAchievement("top_earner");
			gameData->setBoolForKey(earned_1000000_total_points_submitted, true);
		}
		if (gameData->getBoolForKey(reach_match_1_classic, false) && !gameData->getBoolForKey(reach_match_1_classic_submitted, false))
		{
			sdkbox::PluginSdkboxPlay::unlockAchievement("first_match_classic");
			gameData->setBoolForKey(reach_match_1_classic_submitted, true);
		}
		if (gameData->getBoolForKey(reach_match_10_classic, false) && !gameData->getBoolForKey(reach_match_10_classic_submitted, false))
		{
			sdkbox::PluginSdkboxPlay::unlockAchievement("10_matches_classic");
			gameData->setBoolForKey(reach_match_10_classic_submitted, true);
		}
		if (gameData->getBoolForKey(reach_match_20_classic, false) && !gameData->getBoolForKey(reach_match_20_classic_submitted, false))
		{
			sdkbox::PluginSdkboxPlay::unlockAchievement("20_matches_classic");
			gameData->setBoolForKey(reach_match_20_classic_submitted, true);
		}
		if (gameData->getBoolForKey(reach_match_30_classic, false) && !gameData->getBoolForKey(reach_match_30_classic_submitted, false))
		{
			sdkbox::PluginSdkboxPlay::unlockAchievement("30_matches_classic");
			gameData->setBoolForKey(reach_match_30_classic_submitted, true);
		}
		if (gameData->getBoolForKey(reach_match_50_classic, false) && !gameData->getBoolForKey(reach_match_50_classic_submitted, false))
		{
			sdkbox::PluginSdkboxPlay::unlockAchievement("50_matches_classic");
			gameData->setBoolForKey(reach_match_50_classic_submitted, true);
		}
		if (gameData->getBoolForKey(reach_match_60_classic, false) && !gameData->getBoolForKey(reach_match_60_classic_submitted, false))
		{
			sdkbox::PluginSdkboxPlay::unlockAchievement("60_matches_classic");
			gameData->setBoolForKey(reach_match_60_classic_submitted, true);
		}
		if (gameData->getBoolForKey(reach_match_70_classic, false) && !gameData->getBoolForKey(reach_match_70_classic_submitted, false))
		{
			sdkbox::PluginSdkboxPlay::unlockAchievement("70_matches_classic");
			gameData->setBoolForKey(reach_match_70_classic_submitted, true);
		}
		if (gameData->getBoolForKey(reach_match_80_classic, false) && !gameData->getBoolForKey(reach_match_80_classic_submitted, false))
		{
			sdkbox::PluginSdkboxPlay::unlockAchievement("80_matches_classic");
			gameData->setBoolForKey(reach_match_80_classic_submitted, true);
		}
		if (gameData->getBoolForKey(reach_match_90_classic, false) && !gameData->getBoolForKey(reach_match_90_classic_submitted, false))
		{
			sdkbox::PluginSdkboxPlay::unlockAchievement("90_matches_classic");
			gameData->setBoolForKey(reach_match_90_classic_submitted, true);
		}
		if (gameData->getBoolForKey(no_bonus_items_used_first_50_matches_classic, false) && !gameData->getBoolForKey(no_bonus_items_used_first_50_matches_classic_submitted, false))
		{
			sdkbox::PluginSdkboxPlay::unlockAchievement("no_assistance_classic");
			gameData->setBoolForKey(no_bonus_items_used_first_50_matches_classic_submitted, true);
		}
		if (gameData->getBoolForKey(reach_match_100_classic, false) && !gameData->getBoolForKey(reach_match_100_classic_submitted, false))
		{
			sdkbox::PluginSdkboxPlay::unlockAchievement("mastered_classic");
			gameData->setBoolForKey(reach_match_100_classic_submitted, true);
		}
		if (gameData->getBoolForKey(reach_match_10_countdown, false) && !gameData->getBoolForKey(reach_match_10_countdown_submitted, false))
		{
			sdkbox::PluginSdkboxPlay::unlockAchievement("10_matches_countdown");
			gameData->setBoolForKey(reach_match_10_countdown_submitted, true);
		}
		if (gameData->getBoolForKey(reach_match_20_countdown, false) && !gameData->getBoolForKey(reach_match_20_countdown_submitted, false))
		{
			sdkbox::PluginSdkboxPlay::unlockAchievement("20_matches_countdown");
			gameData->setBoolForKey(reach_match_20_countdown_submitted, true);
		}
		if (gameData->getBoolForKey(reach_match_30_countdown, false) && !gameData->getBoolForKey(reach_match_30_countdown_submitted, false))
		{
			sdkbox::PluginSdkboxPlay::unlockAchievement("30_matches_countdown");
			gameData->setBoolForKey(reach_match_30_countdown_submitted, true);
		}
		if (gameData->getBoolForKey(reach_match_50_countdown, false) && !gameData->getBoolForKey(reach_match_50_countdown_submitted, false))
		{
			sdkbox::PluginSdkboxPlay::unlockAchievement("50_matches_countdown");
			gameData->setBoolForKey(reach_match_50_countdown_submitted, true);
		}
		if (gameData->getBoolForKey(reach_match_60_countdown, false) && !gameData->getBoolForKey(reach_match_60_countdown_submitted, false))
		{
			sdkbox::PluginSdkboxPlay::unlockAchievement("60_matches_countdown");
			gameData->setBoolForKey(reach_match_60_countdown_submitted, true);
		}
		if (gameData->getBoolForKey(reach_match_70_countdown, false) && !gameData->getBoolForKey(reach_match_70_countdown_submitted, false))
		{
			sdkbox::PluginSdkboxPlay::unlockAchievement("70_matches_countdown");
			gameData->setBoolForKey(reach_match_70_countdown_submitted, true);
		}
		if (gameData->getBoolForKey(reach_match_80_countdown, false) && !gameData->getBoolForKey(reach_match_80_countdown_submitted, false))
		{
			sdkbox::PluginSdkboxPlay::unlockAchievement("80_matches_countdown");
			gameData->setBoolForKey(reach_match_80_countdown_submitted, true);
		}
		if (gameData->getBoolForKey(reach_match_90_countdown, false) && !gameData->getBoolForKey(reach_match_90_countdown_submitted, false))
		{
			sdkbox::PluginSdkboxPlay::unlockAchievement("90_matches_countdown");
			gameData->setBoolForKey(reach_match_90_countdown_submitted, true);
		}
		if (gameData->getBoolForKey(no_bonus_items_used_first_50_matches_countdown, false) && !gameData->getBoolForKey(no_bonus_items_used_first_50_matches_countdown_submitted, false))
		{
			sdkbox::PluginSdkboxPlay::unlockAchievement("no_assistance_countdown");
			gameData->setBoolForKey(no_bonus_items_used_first_50_matches_countdown_submitted, true);
		}
		if (gameData->getBoolForKey(remaining_time_90_seconds_countdown, false) && !gameData->getBoolForKey(remaining_time_90_seconds_countdown_submitted, false))
		{
			sdkbox::PluginSdkboxPlay::unlockAchievement("time_to_spare");
			gameData->setBoolForKey(remaining_time_90_seconds_countdown_submitted, true);
		}
		if (gameData->getBoolForKey(maintain_time_above_20, false) && !gameData->getBoolForKey(maintain_time_above_20_submitted, false))
		{
			sdkbox::PluginSdkboxPlay::unlockAchievement("time_keeper");
			gameData->setBoolForKey(maintain_time_above_20_submitted, true);
		}
		if (gameData->getBoolForKey(first_5_matches_in_12_seconds, false) && !gameData->getBoolForKey(first_5_matches_in_12_seconds_submitted, false))
		{
			sdkbox::PluginSdkboxPlay::unlockAchievement("speed_demon");
			gameData->setBoolForKey(first_5_matches_in_12_seconds_submitted, true);
		}
		if (gameData->getBoolForKey(first_10_matches_in_24_seconds, false) && !gameData->getBoolForKey(first_10_matches_in_24_seconds_submitted, false))
		{
			sdkbox::PluginSdkboxPlay::unlockAchievement("speed_demon_2");
			gameData->setBoolForKey(first_10_matches_in_24_seconds_submitted, true);
		}
		if (gameData->getBoolForKey(first_20_matches_in_50_seconds, false) && !gameData->getBoolForKey(first_20_matches_in_50_seconds_submitted, false))
		{
			sdkbox::PluginSdkboxPlay::unlockAchievement("speed_demon_3");
			gameData->setBoolForKey(first_20_matches_in_50_seconds_submitted, true);
		}
		if (gameData->getBoolForKey(reach_match_100_countdown, false) && !gameData->getBoolForKey(reach_match_100_countdown_submitted, false))
		{
			sdkbox::PluginSdkboxPlay::unlockAchievement("mastered_countdown");
			gameData->setBoolForKey(reach_match_100_countdown_submitted, true);
		}
		if (gameData->getBoolForKey(reach_match_10_memory, false) && !gameData->getBoolForKey(reach_match_10_memory_submitted, false))
		{
			sdkbox::PluginSdkboxPlay::unlockAchievement("10_matches_memory");
			gameData->setBoolForKey(reach_match_10_memory_submitted, true);
		}
		if (gameData->getBoolForKey(reach_match_20_memory, false) && !gameData->getBoolForKey(reach_match_20_memory_submitted, false))
		{
			sdkbox::PluginSdkboxPlay::unlockAchievement("20_matches_memory");
			gameData->setBoolForKey(reach_match_20_memory_submitted, true);
		}
		if (gameData->getBoolForKey(reach_match_30_memory, false) && !gameData->getBoolForKey(reach_match_30_memory_submitted, false))
		{
			sdkbox::PluginSdkboxPlay::unlockAchievement("30_matches_memory");
			gameData->setBoolForKey(reach_match_30_memory_submitted, true);
		}
		if (gameData->getBoolForKey(reach_match_50_memory, false) && !gameData->getBoolForKey(reach_match_50_memory_submitted, false))
		{
			sdkbox::PluginSdkboxPlay::unlockAchievement("50_matches_memory");
			gameData->setBoolForKey(reach_match_50_memory_submitted, true);
		}
		if (gameData->getBoolForKey(reach_match_60_memory, false) && !gameData->getBoolForKey(reach_match_60_memory_submitted, false))
		{
			sdkbox::PluginSdkboxPlay::unlockAchievement("60_matches_memory");
			gameData->setBoolForKey(reach_match_60_memory_submitted, true);
		}
		if (gameData->getBoolForKey(reach_match_70_memory, false) && !gameData->getBoolForKey(reach_match_70_memory_submitted, false))
		{
			sdkbox::PluginSdkboxPlay::unlockAchievement("70_matches_memory");
			gameData->setBoolForKey(reach_match_70_memory_submitted, true);
		}
		if (gameData->getBoolForKey(reach_match_80_memory, false) && !gameData->getBoolForKey(reach_match_80_memory_submitted, false))
		{
			sdkbox::PluginSdkboxPlay::unlockAchievement("80_matches_memory");
			gameData->setBoolForKey(reach_match_80_memory_submitted, true);
		}
		if (gameData->getBoolForKey(reach_match_90_memory, false) && !gameData->getBoolForKey(reach_match_90_memory_submitted, false))
		{
			sdkbox::PluginSdkboxPlay::unlockAchievement("90_matches_memory");
			gameData->setBoolForKey(reach_match_90_memory_submitted, true);
		}
		if (gameData->getBoolForKey(no_bonus_items_used_first_50_matches_memory, false) && !gameData->getBoolForKey(no_bonus_items_used_first_50_matches_memory_submitted, false))
		{
			sdkbox::PluginSdkboxPlay::unlockAchievement("no_assistance_memory");
			gameData->setBoolForKey(no_bonus_items_used_first_50_matches_memory_submitted, true);
		}
		if (gameData->getBoolForKey(reach_match_100_memory, false) && !gameData->getBoolForKey(reach_match_100_memory_submitted, false))
		{
			sdkbox::PluginSdkboxPlay::unlockAchievement("mastered_memory");
			gameData->setBoolForKey(reach_match_100_memory_submitted, true);
		}
		if (gameData->getBoolForKey(slow_poke, false) && !gameData->getBoolForKey(slow_poke_submitted, false))
		{
			sdkbox::PluginSdkboxPlay::unlockAchievement("slow_poke");
			gameData->setBoolForKey(slow_poke_submitted, true);
		}
		if (gameData->getBoolForKey(golf_rules, false) && !gameData->getBoolForKey(golf_rules_submitted, false))
		{
			sdkbox::PluginSdkboxPlay::unlockAchievement("golf_rules");
			gameData->setBoolForKey(golf_rules_submitted, true);
		}
		if (gameData->getBoolForKey(earned_all_achievements, false) && !gameData->getBoolForKey(earned_all_achievements_submitted, false))
		{
			sdkbox::PluginSdkboxPlay::unlockAchievement("mastered_tap_3");
			gameData->setBoolForKey(first_match_submitted, true);
		}
	}

#endif

		return;
}

void Game::SyncHighScores()
{
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	sdkbox::PluginSdkboxPlay::submitScore("high_score_classic", gameData->getIntegerForKey(best_score_classic, 0));
	sdkbox::PluginSdkboxPlay::submitScore("highest_match_classic", gameData->getIntegerForKey(best_match_classic, 0));
	sdkbox::PluginSdkboxPlay::submitScore("high_score_countdown", gameData->getIntegerForKey(best_score_countdown, 0));
	sdkbox::PluginSdkboxPlay::submitScore("highest_match_countdown", gameData->getIntegerForKey(best_match_countdown, 0));
	sdkbox::PluginSdkboxPlay::submitScore("high_score_memory", gameData->getIntegerForKey(best_score_memory, 0));
	sdkbox::PluginSdkboxPlay::submitScore("highest_match_memory", gameData->getIntegerForKey(best_match_memory, 0));
	sdkbox::PluginSdkboxPlay::submitScore("high_score_practice", 0);
	sdkbox::PluginSdkboxPlay::submitScore("total_score", gameData->getIntegerForKey(total_points, 0));
	sdkbox::PluginSdkboxPlay::submitScore("total_matches", gameData->getIntegerForKey(total_matches, 0));
#endif
	return;
}

void Game::ShowMatchingTiles()
{
	for (int iii = 0; iii < matchingTilesVector.size(); iii++)
	{
		if (tiles[matchingTilesVector[iii]]->getOpacity() != 64)
			tiles[matchingTilesVector[iii]]->SetFade(64);
	}

	return;
}

void Game::CalculateScore()
{
	return;
}

void Game::ConvertRemainingBonusItemsToPoints()
{
	return;
}

void Game::ResetScore()
{
	matches = 0;
	speedBonus = 0;
	previousSpeedBonus = 0;
	earnedEliminateBonuses = 0;
	earnedTimePlusBonuses = 0;
	earnedShowMatchBonuses = 0;
	earnedNewCardBonuses = 0;
	msAdded = 0;
	return;
}

void Game::UseBonusTimePlus()
{
	if (timePlusUsedOnCurrentCard < timePlusUseLimit)
	{
		msLeft += 10000;

		//Reset warning time to correct threshold 
		if (msLeft >= 3000)
			warningTime = 3000;
		else if (msLeft >= 2000)
			warningTime = 2000;
		else if (msLeft >= 1000)
			warningTime = 1000;

		timePlusUsedOnCurrentCard++;
		bonusesUsed++;
		gameData->setIntegerForKey(time_plus_used, gameData->getIntegerForKey(time_plus_used, 0) + 1);
	}

	if (timePlusUsedOnCurrentCard >= timePlusUseLimit)
	{
		timePlusTile->runAction(cocos2d::FadeTo::create(0.25, 64));
		timePlusText->GetLabel()->runAction(cocos2d::FadeTo::create(0.25, 64));
	}

	return;
}

void Game::UseBonusEliminate()
{
	//Prevent exceeding limit of how many tiles can logically be eliminated
	if (excludedEliminateTiles.size() == tiles.size())
		return;

	bonusesUsed++;
	eliminateUsedOnCurrentCard++;
	gameData->setIntegerForKey(eliminate_used, gameData->getIntegerForKey(eliminate_used, 0) + 1);

	//Exclude all matching tiles
	if (excludedEliminateTiles.size() == 0)
		excludedEliminateTiles = matchingTiles;
	int eliminatedTile1 = nrgFunctions::GetRandom(0, tiles.size() - 1, excludedEliminateTiles);
	excludedEliminateTiles.emplace(eliminatedTile1);
	tiles[eliminatedTile1]->stopAllActions();
	tiles[eliminatedTile1]->DismissTile();

	//Prevent exceeding limit of how many tiles can logically be eliminated
	if (excludedEliminateTiles.size() == tiles.size())
	{
		eliminateTile->runAction(cocos2d::FadeTo::create(0.25, 64));
		eliminateText->GetLabel()->runAction(cocos2d::FadeTo::create(0.25, 64));
		return;
	}

	int eliminatedTile2 = nrgFunctions::GetRandom(0, tiles.size() - 1, excludedEliminateTiles);
	excludedEliminateTiles.emplace(eliminatedTile2);
	tiles[eliminatedTile2]->stopAllActions();
	tiles[eliminatedTile2]->DismissTile();

	//Prevent exceeding limit of how many tiles can logically be eliminated
	if (excludedEliminateTiles.size() == tiles.size())
	{
		eliminateTile->runAction(cocos2d::FadeTo::create(0.25, 64));
		eliminateText->GetLabel()->runAction(cocos2d::FadeTo::create(0.25, 64));
		return;
	}

	return;
}

void Game::UseShowMatch()
{
	if (matchedTilesShowing == 3)
		return;

	tiles[matchingTilesVector[matchedTilesShowing]]->stopAllActions();
	tiles[matchingTilesVector[matchedTilesShowing]]->SetFade(128, true);

	matchedTilesShowing++;
	bonusesUsed++;
	gameData->setIntegerForKey(show_match_used, gameData->getIntegerForKey(show_match_used, 0) + 1);

	//Disable item if all three matches are showing
	if (matchedTilesShowing == 3)
	{
		showMatchTile->runAction(cocos2d::FadeTo::create(0.25, 64));
		showMatchText->GetLabel()->runAction(cocos2d::FadeTo::create(0.25, 64));
	}

	return;
}

void Game::UseNewCard()
{
	currentSelection = 1;
	SetMatchParameters(matchCriteria);
	UpdateHeaderText(matchCriteria);
	RandomizeTiles();
	RestoreTiles(0);

	//Re-enable bonus items if any are left
	if (bonusEliminateLeft > 0)
	{
		eliminateTile->runAction(cocos2d::FadeTo::create(0.5, 255));
		eliminateTile->ToggleTouch(true);
		eliminateText->GetLabel()->runAction(cocos2d::FadeTo::create(0.5, 255));
	}

	if (bonusShowMatchLeft > 0)
	{
		showMatchTile->runAction(cocos2d::FadeTo::create(0.5, 255));
		showMatchTile->ToggleTouch(true);
		showMatchText->GetLabel()->runAction(cocos2d::FadeTo::create(0.5, 255));
	}
	
	if (bonusTimePlusLeft > 0)
	{
		timePlusTile->runAction(cocos2d::FadeTo::create(0.5, 255));
		timePlusTile->ToggleTouch(true);
		timePlusText->GetLabel()->runAction(cocos2d::FadeTo::create(0.5, 255));
		timePlusUsedOnCurrentCard = 0;
	}

	bonusesUsed++;
	gameData->setIntegerForKey(new_card_used, gameData->getIntegerForKey(new_card_used, 0) + 1);

	return;
}

void Game::UpdateScore()
{
	//Count display score up until it reaches final score (with bonus points added for unused items)
	if (displayedScore < finalScore)
	{
		if (finalScore - displayedScore > 500)
			displayedScore += 33;
		else if (finalScore - displayedScore > 1000)
			displayedScore += 133;
		else
			displayedScore += 3;
	}

	if (displayedScore > finalScore)
		displayedScore = finalScore;

	if(matches == 1)
		sprintf(sz, "%d pts   |   %d match", displayedScore, matches);
	else
		sprintf(sz, "%d pts   |   %d matches", displayedScore, matches);

	pointsText->setString(sz);
	pointsText->Center();
	pointsText->SetBottom(pointsTextBottomPosition);
	
	return;
}

void Game::CheckForNewBests()
{
	return;
}

void Game::UpdateTimer()
{
	//Get how much time has passed in ms
	msLeft -= countdownTimer.GetElapsedTime();
	countdownTimer.Restart();

	if (msLeft <= warningTime)
	{
		audio->PlayClip("warning_tone");
		warningTime -= 1000;
	}

	//Set clock display to 0 after time is up
	if (msLeft <= 0)
	{
		if (msLeft > -225)
		{
			timerText->setString("0.00");
			timerText->Center();
			timerText->SetBottom(timeRemainingBottomPosition);

			timePlusTile->runAction(cocos2d::FadeTo::create(0.25, 64));
			timePlusText->GetLabel()->runAction(cocos2d::FadeTo::create(0.25, 64));
		}

		//Give player an extra 225ms to account for human delayed perception. In other words, they don't think the game is cheating them.
		else if (msLeft <= -225)
		{
			gameOver = true;

			//Disable all powerups
			if (bonusTimePlusLeft > 0)
			{
				timePlusTile->runAction(cocos2d::FadeTo::create(0.25, 64));
				timePlusText->GetLabel()->runAction(cocos2d::FadeTo::create(0.25, 64));
			}
			//timePlusTile->ToggleTouch(false);

			if (bonusNewCardLeft > 0)
			{
				newCardTile->runAction(cocos2d::FadeTo::create(0.25, 64));
				newCardText->GetLabel()->runAction(cocos2d::FadeTo::create(0.25, 64));
			}
			//newCardTile->ToggleTouch(false);

			if (bonusEliminateLeft > 0)
			{
				eliminateTile->runAction(cocos2d::FadeTo::create(0.25, 64));
				eliminateText->GetLabel()->runAction(cocos2d::FadeTo::create(0.25, 64));
			}
			//eliminateTile->ToggleTouch(false);

			if (bonusShowMatchLeft > 0)
			{
				showMatchTile->runAction(cocos2d::FadeTo::create(0.25, 64));
				showMatchText->GetLabel()->runAction(cocos2d::FadeTo::create(0.25, 64));
			}
			//showMatchTile->ToggleTouch(false);

			restoreTiles = true;
			currentSelection = 1;
			audio->PlayClip("double_tone_low");

			ConvertRemainingBonusItemsToPoints();
			if (matches == 1)
				sprintf(sz, "You Got 1 Match!");
			else
				sprintf(sz, "You Got %d Matches!", matches);

			timerText->setString(sz);
			timerText->Center();
			timerText->SetBottom(timeRemainingBottomPosition);
			timerText->GetLabel()->setTextColor(cocos2d::Color4B::BLACK);

			CalculateScore();
			UpdateScore();

			//Check for new best score
			CheckForNewBests();

			//Check for achievements
			CheckForAchievements();

			if (achievementsUnlocked > 0)
			{
				achievementUnlockedHeader->GetLabel()->runAction(cocos2d::FadeIn::create(0.25));
				retry->ChangeMenu("Continue", 0, false);
				for (int iii = 0; iii < tiles.size(); iii++)
					tiles[iii]->DismissTile();
			}

			else
			{
				retry->ChangeMenu("Try Again", 0, false);
				restoreTiles = true;
			}

			retry->Center();
			retry->SetBottom(retryBottomPosition);
			retry->ToggleOn(true);
			retry->GetItem(0)->runAction(cocos2d::RepeatForever::create(
				cocos2d::Sequence::create(
					cocos2d::DelayTime::create(0.125),
					cocos2d::FadeTo::create(0.5, 64),
					cocos2d::FadeTo::create(0.5, 255),
					cocos2d::DelayTime::create(0.125), NULL)));

			//Reveal match type for blind match
			if (scoreText->getString() == "???")
				UpdateHeaderText(matchCriteria);

			getInstance()->ResetScore();

			//Turn menu buttons back on
			optionsButton->runAction(cocos2d::FadeIn::create(0.25));
			optionsText->GetLabel()->runAction(cocos2d::FadeIn::create(0.25));
			highScoresButton->runAction(cocos2d::FadeIn::create(0.25));
			highScoresText->GetLabel()->runAction(cocos2d::FadeIn::create(0.25));
			buyBonusItemsButton->runAction(cocos2d::FadeIn::create(0.25));
			buyBonusItemsText->GetLabel()->runAction(cocos2d::FadeIn::create(0.25));
			optionsButton->ToggleTouch(true);
			highScoresButton->ToggleTouch(true);
			buyBonusItemsButton->ToggleTouch(true);

			//Count how many times app has been played today
			std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
			time_t timeObject = std::chrono::system_clock::to_time_t(now);
			tm* currentTime = localtime(&timeObject);

			int today = currentTime->tm_yday;
			int year = currentTime->tm_year;
			int lastDayPlayed = gameData->getIntegerForKey(last_day_opened, today);
			int lastYearPlayed = gameData->getIntegerForKey(last_year_opened, year);

			if (lastDayPlayed == today && lastYearPlayed == year)
				gameData->setIntegerForKey(times_played_today, gameData->getIntegerForKey(times_played_today) + 1);
			else
				gameData->setIntegerForKey(times_played_today, 1);

			if (gameData->getIntegerForKey(times_played_today) >= 25)
			{
				RequestReview();
				gameData->setIntegerForKey(matches_made_today_for_review, 0);
				gameData->setIntegerForKey(times_opened_today, 0);
				gameData->setIntegerForKey(times_played_today, 0);
			}
		}

		return;
	}

	int ms = msLeft;
	int sec = 0;

	//Count up add ms passed into sec passed
	while (ms >= 1000)
	{
		sec++;
		ms -= 1000;
	}

	//Format string for timer decimal display
	std::string secString;
	sprintf(sz, "%d", sec);
	secString = sz;
	std::string msString;

	if (ms < 10)
	{
		sprintf(sz, "00");
		msString = sz;
	}

	else if (ms < 100)
	{
		sprintf(sz, "0%d", ms / 10);
		msString = sz;
	}

	else if (ms < 1000)
	{
		sprintf(sz, "%d", ms / 10);
		msString = sz;
	}

	//Update timer string
	timerText->setString(secString + "." + msString);

	//Change timer text color if seconds are low (below 1.5 second left)
	if (msLeft <= msWarning)
		timerText->ChangeColor(cocos2d::Color4B::RED);
	else
		timerText->ChangeColor(cocos2d::Color4B::BLACK);
}

void Game::DismissHeaderText()
{
	matchCriteriaText->GetLabel()->stopAllActions();
	matchCriteriaText->GetLabel()->runAction(cocos2d::FadeOut::create(0.25));
	return;
}

void Game::UpdateHeaderText(int value)
{
	matchCriteriaText->GetLabel()->stopAllActions();
	matchCriteriaText->GetLabel()->setOpacity(0);

	if (blindMatch)
		matchCriteriaText->setString("???");

	else if (!blindMatch)
	{
		if (matchCriteria == COLORS)
			matchCriteriaText->setString("Colors");
		else if (matchCriteria == SHAPES)
			matchCriteriaText->setString("Shapes");
		else if (matchCriteria == STYLES)
			matchCriteriaText->setString("Styles");
	}

	nrgFunctions::Center(matchCriteriaText, headerTile);
	float offset = 25;

	matchCriteriaText->GetLabel()->setPositionY(-offset);
	matchCriteriaText->GetLabel()->runAction(
		cocos2d::Spawn::create(
			cocos2d::FadeIn::create(2),
			cocos2d::MoveBy::create(1, cocos2d::Vec2(0, offset)), NULL));
	return;
}

void Game::UpdateHeaderText(std::string input)
{
	matchCriteriaText->GetLabel()->stopAllActions();
	matchCriteriaText->GetLabel()->setOpacity(0);
	matchCriteriaText->setString(input);

	nrgFunctions::Center(matchCriteriaText, headerTile);
	float offset = 25;

	matchCriteriaText->GetLabel()->setPositionY(-offset);
	matchCriteriaText->GetLabel()->runAction(
		cocos2d::Spawn::create(
			cocos2d::FadeIn::create(2),
			cocos2d::MoveBy::create(1, cocos2d::Vec2(0, offset)), NULL));
	return;
}

void Game::DisplayMatchSummary()
{
	std::vector<std::string> matchMadeStrings;
	std::string matchPoints;
	std::string speedPoints;
	std::string timeAdded;
	std::string nextBonus;

	sprintf(sz, "Match +100");
	matchPoints.append(sz);
	matchMadeStrings.push_back(matchPoints);

	if (previousSpeedBonus > 0)
	{
		sprintf(sz, "Speed +%d", previousSpeedBonus);
		speedPoints.append(sz);
		matchMadeStrings.push_back(speedPoints);
	}

	float decimal = (float(msAdded) / 1000 - int(msAdded / 1000)) * 10;
	int whole = msAdded / 1000;

	sprintf(sz, "+%d.%d seconds", whole, int(decimal));
	timeAdded.append(sz);
	matchMadeStrings.push_back(timeAdded);

	sprintf(sz, "Bonus in %d", matchMilestone - matches);
	nextBonus.append(sz);
	matchMadeStrings.push_back(nextBonus);

	matchMadeList->ChangeList(matchMadeStrings);

	float scale = baseTile->getScale();
	float baseX = baseTile->getPositionX() - baseTile->GetWidth() / 2;
	float baseY = baseTile->getPositionY() - baseTile->GetHeight() / 2;
	float x = baseX + (tiles[selection3]->getPositionX() * scale - matchMadeList->GetScaledWidth() / 2);
	float y = baseY + (tiles[selection3]->getPositionY() * scale + matchMadeList->GetScaledHeight() / 2) - matchMadeList->GetLetterHeight();
	matchMadeList->setPosition(x, y);

	int numberOfLabels = matchMadeList->GetNumberOfItems();
	for (int iii = 0; iii < numberOfLabels; iii++)
		matchMadeList->GetItem(iii)->runAction(cocos2d::Sequence::create
		(
			cocos2d::FadeIn::create(0.25),
			cocos2d::DelayTime::create(0.75),
			cocos2d::FadeOut::create(0.25), NULL
		));

	return;
}

void Game::AddTimePlusBonus()
{
	earnedTimePlusBonuses++;
	bonusTimePlusLeft++;

	if (bonusTimePlusLeft == 0)
	{
		timePlusTile->setVisible(true);
		timePlusTile->runAction(cocos2d::FadeIn::create(0.25));
		timePlusTile->ToggleTouch(true);
		timePlusText->GetLabel()->runAction(cocos2d::FadeIn::create(0.25));
	}

	sprintf(sz, "%d / %d", earnedTimePlusBonuses, purchasedTimePlusBonuses);
	timePlusText->setString(sz);
	timePlusText->setPositionX(timePlusTile->getPositionX() + timePlusTile->GetWidth() / 2 - timePlusText->GetScaledWidth() / 2);

	return;
}

void Game::AddEliminateBonus()
{
	earnedEliminateBonuses++;
	bonusEliminateLeft++;

	if (bonusEliminateLeft == 0)
	{
		eliminateTile->setVisible(true);
		eliminateTile->runAction(cocos2d::FadeTo::create(0.25, 64));
		eliminateText->GetLabel()->runAction(cocos2d::FadeTo::create(0.25, 64));
	}

	sprintf(sz, "%d / %d", earnedEliminateBonuses, purchasedEliminateBonuses);
	eliminateText->setString(sz);
	eliminateText->setPositionX(eliminateTile->getPositionX() + eliminateTile->GetWidth() / 2 - eliminateText->GetScaledWidth() / 2);

	return;
}

void Game::AddShowMatchBonus()
{
	earnedShowMatchBonuses++;
	bonusShowMatchLeft++;

	if (bonusShowMatchLeft == 0)
	{
		showMatchTile->setVisible(true);
		showMatchTile->runAction(cocos2d::FadeTo::create(0.25, 64));
		showMatchText->GetLabel()->runAction(cocos2d::FadeTo::create(0.25, 64));
	}

	sprintf(sz, "%d / %d", earnedShowMatchBonuses, purchasedShowMatchBonuses);
	showMatchText->setString(sz);
	showMatchText->setPositionX(showMatchTile->getPositionX() + showMatchTile->GetWidth() / 2 - showMatchText->GetScaledWidth() / 2);
	
	return;
}

void Game::AddNewCardBonus()
{
	earnedNewCardBonuses++;
	bonusNewCardLeft++;

	if (bonusNewCardLeft == 0)
	{
		newCardTile->setVisible(true);
		newCardTile->runAction(cocos2d::FadeTo::create(0.25, 64));
		newCardText->GetLabel()->runAction(cocos2d::FadeTo::create(0.25, 64));
	}

	sprintf(sz, "%d / %d", earnedNewCardBonuses, purchasedNewCardBonuses);
	newCardText->setString(sz);
	newCardText->setPositionX(newCardTile->getPositionX() + newCardTile->GetWidth() / 2 - newCardText->GetScaledWidth() / 2);

	return;
}

void Game::LowerTimePlusBonus()
{
	bonusTimePlusLeft--;

	//Decide if earned or purchased item was used
	if (earnedTimePlusBonuses > 0)
		earnedTimePlusBonuses--;

	else
	{
		purchasedTimePlusBonuses--;
		gameData->setIntegerForKey(purchased_time_plus_left, purchasedTimePlusBonuses);
	}

	sprintf(sz, "%d / %d", earnedTimePlusBonuses, purchasedTimePlusBonuses);
	timePlusText->setString(sz);
	timePlusText->setPositionX(timePlusTile->getPositionX() + timePlusTile->GetWidth() / 2 - timePlusText->GetScaledWidth() / 2);

	if (bonusTimePlusLeft == 0)
	{
		timePlusTile->stopAllActions();
		timePlusText->GetLabel()->stopAllActions();

		timePlusTile->runAction(cocos2d::FadeOut::create(0.25));
		timePlusTile->ToggleTouch(false);
		timePlusText->GetLabel()->runAction(cocos2d::FadeOut::create(0.25));
	}

	return;
}

void Game::LowerEliminateBonus()
{
	bonusEliminateLeft--;

	//Decide if earned or purchased item was used
	if (earnedEliminateBonuses > 0)
		earnedEliminateBonuses--;
	else
	{
		purchasedEliminateBonuses--;
		gameData->setIntegerForKey(purchased_eliminate_left, purchasedEliminateBonuses);
	}

	sprintf(sz, "%d / %d", earnedEliminateBonuses, purchasedEliminateBonuses);
	eliminateText->setString(sz);
	eliminateText->setPositionX(eliminateTile->getPositionX() + eliminateTile->GetWidth() / 2 - eliminateText->GetScaledWidth() / 2);

	if (bonusEliminateLeft == 0)
	{
		eliminateTile->stopAllActions();
		eliminateText->GetLabel()->stopAllActions();

		eliminateTile->runAction(cocos2d::FadeOut::create(0.25));
		eliminateTile->ToggleTouch(false);
		eliminateText->GetLabel()->runAction(cocos2d::FadeOut::create(0.25));
	}

	return;
}

void Game::LowerShowMatchBonus()
{
	bonusShowMatchLeft--;

	//Decide if earned or purchased item was used
	if (earnedShowMatchBonuses > 0)
		earnedShowMatchBonuses--;
	else
	{
		purchasedShowMatchBonuses--;
		gameData->setIntegerForKey(purchased_show_match_left, purchasedShowMatchBonuses);
	}

	sprintf(sz, "%d / %d", earnedShowMatchBonuses, purchasedShowMatchBonuses);
	showMatchText->setString(sz);
	showMatchText->setPositionX(showMatchTile->getPositionX() + showMatchTile->GetWidth() / 2 - showMatchText->GetScaledWidth() / 2);

	if (bonusShowMatchLeft == 0)
	{
		showMatchTile->stopAllActions();
		showMatchText->GetLabel()->stopAllActions();

		showMatchTile->runAction(cocos2d::FadeOut::create(0.25));
		showMatchTile->ToggleTouch(false);
		showMatchText->GetLabel()->runAction(cocos2d::FadeOut::create(0.25));
	}

	return;
}

void Game::LowerNewCardBonus()
{
	bonusNewCardLeft--;

	//Decide if earned or purchased item was used
	if (earnedNewCardBonuses > 0)
		earnedNewCardBonuses--;
	else
	{
		purchasedNewCardBonuses--;
		gameData->setIntegerForKey(purchased_new_card_left, purchasedNewCardBonuses);
	}

	sprintf(sz, "%d / %d", earnedNewCardBonuses, purchasedNewCardBonuses);
	newCardText->setString(sz);
	newCardText->setPositionX(newCardTile->getPositionX() + newCardTile->GetWidth() / 2 - newCardText->GetScaledWidth() / 2);

	if (bonusNewCardLeft == 0)
	{
		newCardTile->stopAllActions();
		newCardText->GetLabel()->stopAllActions();

		newCardTile->runAction(cocos2d::FadeOut::create(0.25));
		newCardTile->ToggleTouch(false);
		newCardText->GetLabel()->runAction(cocos2d::FadeOut::create(0.25));
	}

	return;
}

void Game::RestoreTiles(int patternInput)
{
	//Sequentially set how tiles will fade in
	if (patternInput == -1)
	{
		patternInput = pattern;
		pattern++;

		//Reset after limit is reached
		if (pattern >= 8)
			pattern = 0;
	}

	int offset = 35;
	
	switch (patternInput)
	{
	case 0: //Regular fade in
		for (int iii = 0; iii < tiles.size(); iii++)
			tiles[iii]->RestoreTile();
		break;
	case 1: //Columns slide in from left
		//First, place first column slightly to the left
		tiles[TOP_LEFT]->setPositionX(topLeft.first - offset);
		tiles[MIDDLE_LEFT]->setPositionX(middleLeft.first - offset);
		tiles[BOTTOM_LEFT]->setPositionX(bottomLeft.first - offset);
		
		//Second column will start at first column
		tiles[TOP_CENTER]->setPositionX(topLeft.first);
		tiles[MIDDLE_CENTER]->setPositionX(middleLeft.first);
		tiles[BOTTOM_CENTER]->setPositionX(bottomLeft.first);

		//Third column will start at second column
		tiles[TOP_RIGHT]->setPositionX(topCenter.first);
		tiles[MIDDLE_RIGHT]->setPositionX(middleCenter.first);
		tiles[BOTTOM_RIGHT]->setPositionX(bottomCenter.first);

		//Move first colum into place over 150 ms, starting at 0 ms
		tiles[TOP_LEFT]->RestoreTile(topLeft.first, topLeft.second, 0.15, 0);
		tiles[MIDDLE_LEFT]->RestoreTile(middleLeft.first, middleLeft.second, 0.15, 0);
		tiles[BOTTOM_LEFT]->RestoreTile(bottomLeft.first, bottomLeft.second, 0.15, 0);

		//Move second colum into place over 300 ms, starting at 80 ms
		tiles[TOP_CENTER]->RestoreTile(topCenter.first, topCenter.second, 0.3, 0.08);
		tiles[MIDDLE_CENTER]->RestoreTile(middleCenter.first, middleCenter.second, 0.3, 0.08);
		tiles[BOTTOM_CENTER]->RestoreTile(bottomCenter.first, bottomCenter.second, 0.3, 0.08);

		//Move first colum into place over 300 ms, starting at 300 ms
		tiles[TOP_RIGHT]->RestoreTile(topRight.first, topRight.second, 0.3, 0.3);
		tiles[MIDDLE_RIGHT]->RestoreTile(middleRight.first, middleRight.second, 0.3, 0.3);
		tiles[BOTTOM_RIGHT]->RestoreTile(bottomRight.first, bottomRight.second, 0.3, 0.3);

		break;
	case 2: //Slide columns up from bottom, starting with left column
		//First, place all columns slightly down
		tiles[TOP_LEFT]->setPositionY(topLeft.second - offset);
		tiles[MIDDLE_LEFT]->setPositionY(middleLeft.second - offset);
		tiles[BOTTOM_LEFT]->setPositionY(bottomLeft.second - offset);

		tiles[TOP_CENTER]->setPositionY(topLeft.second - offset);
		tiles[MIDDLE_CENTER]->setPositionY(middleLeft.second - offset);
		tiles[BOTTOM_CENTER]->setPositionY(bottomLeft.second - offset);

		tiles[TOP_RIGHT]->setPositionY(topCenter.second - offset);
		tiles[MIDDLE_RIGHT]->setPositionY(middleCenter.second - offset);
		tiles[BOTTOM_RIGHT]->setPositionY(bottomCenter.second - offset);

		//Move first colum into place over 150 ms, starting at 0 ms
		tiles[TOP_LEFT]->RestoreTile(topLeft.first, topLeft.second, 0.15, 0);
		tiles[MIDDLE_LEFT]->RestoreTile(middleLeft.first, middleLeft.second, 0.15, 0);
		tiles[BOTTOM_LEFT]->RestoreTile(bottomLeft.first, bottomLeft.second, 0.15, 0);

		//Move second colum into place over 300 ms, starting at 80 ms
		tiles[TOP_CENTER]->RestoreTile(topCenter.first, topCenter.second, 0.3, 0.08);
		tiles[MIDDLE_CENTER]->RestoreTile(middleCenter.first, middleCenter.second, 0.3, 0.08);
		tiles[BOTTOM_CENTER]->RestoreTile(bottomCenter.first, bottomCenter.second, 0.3, 0.08);

		//Move first colum into place over 300 ms, starting at 300 ms
		tiles[TOP_RIGHT]->RestoreTile(topRight.first, topRight.second, 0.3, 0.3);
		tiles[MIDDLE_RIGHT]->RestoreTile(middleRight.first, middleRight.second, 0.3, 0.3);
		tiles[BOTTOM_RIGHT]->RestoreTile(bottomRight.first, bottomRight.second, 0.3, 0.3);

		break;
	case 3: //Slide columns down from top, starting with left column
		//First, place all columns slightly up
		tiles[TOP_LEFT]->setPositionY(topLeft.second + offset);
		tiles[MIDDLE_LEFT]->setPositionY(middleLeft.second + offset);
		tiles[BOTTOM_LEFT]->setPositionY(bottomLeft.second + offset);

		tiles[TOP_CENTER]->setPositionY(topLeft.second + offset);
		tiles[MIDDLE_CENTER]->setPositionY(middleLeft.second + offset);
		tiles[BOTTOM_CENTER]->setPositionY(bottomLeft.second + offset);

		tiles[TOP_RIGHT]->setPositionY(topCenter.second + offset);
		tiles[MIDDLE_RIGHT]->setPositionY(middleCenter.second + offset);
		tiles[BOTTOM_RIGHT]->setPositionY(bottomCenter.second + offset);

		//Move first colum into place over 150 ms, starting at 0 ms
		tiles[TOP_LEFT]->RestoreTile(topLeft.first, topLeft.second, 0.15, 0);
		tiles[MIDDLE_LEFT]->RestoreTile(middleLeft.first, middleLeft.second, 0.15, 0);
		tiles[BOTTOM_LEFT]->RestoreTile(bottomLeft.first, bottomLeft.second, 0.15, 0);

		//Move second colum into place over 300 ms, starting at 80 ms
		tiles[TOP_CENTER]->RestoreTile(topCenter.first, topCenter.second, 0.3, 0.08);
		tiles[MIDDLE_CENTER]->RestoreTile(middleCenter.first, middleCenter.second, 0.3, 0.08);
		tiles[BOTTOM_CENTER]->RestoreTile(bottomCenter.first, bottomCenter.second, 0.3, 0.08);

		//Move first colum into place over 300 ms, starting at 300 ms
		tiles[TOP_RIGHT]->RestoreTile(topRight.first, topRight.second, 0.3, 0.3);
		tiles[MIDDLE_RIGHT]->RestoreTile(middleRight.first, middleRight.second, 0.3, 0.3);
		tiles[BOTTOM_RIGHT]->RestoreTile(bottomRight.first, bottomRight.second, 0.3, 0.3);
		break;
	case 4: //Columns slide out from middle
		//First, place all columns in middle
		tiles[TOP_LEFT]->setPositionX(topCenter.first);
		tiles[MIDDLE_LEFT]->setPositionX(middleCenter.first);
		tiles[BOTTOM_LEFT]->setPositionX(bottomCenter.first);

		tiles[TOP_CENTER]->setPositionX(topCenter.first);
		tiles[MIDDLE_CENTER]->setPositionX(middleCenter.first);
		tiles[BOTTOM_CENTER]->setPositionX(bottomCenter.first);

		tiles[TOP_RIGHT]->setPositionX(topCenter.first);
		tiles[MIDDLE_RIGHT]->setPositionX(middleCenter.first);
		tiles[BOTTOM_RIGHT]->setPositionX(bottomCenter.first);

		//Middle column fades in first over 400 ms, starting at 0 ms
		tiles[TOP_CENTER]->RestoreTile(0, 0, 0.4, 0);
		tiles[MIDDLE_CENTER]->RestoreTile(0, 0, 0.4, 0);
		tiles[BOTTOM_CENTER]->RestoreTile(0, 0, 0.4, 0);

		//Move first colum into place over 400 ms, starting at 0 ms
		tiles[TOP_LEFT]->RestoreTile(topLeft.first, topLeft.second, 0.4, 0.0);
		tiles[MIDDLE_LEFT]->RestoreTile(middleLeft.first, middleLeft.second, 0.4, 0.0);
		tiles[BOTTOM_LEFT]->RestoreTile(bottomLeft.first, bottomLeft.second, 0.4, 0.0);

		//Move rightcolum into place over 400 ms, starting at 0 ms
		tiles[TOP_RIGHT]->RestoreTile(topRight.first, topRight.second, 0.4, 0.0);
		tiles[MIDDLE_RIGHT]->RestoreTile(middleRight.first, middleRight.second, 0.4, 0.0);
		tiles[BOTTOM_RIGHT]->RestoreTile(bottomRight.first, bottomRight.second, 0.4, 0.0);
		break;
	case 5: //Slide rows down from top
		//First, place top row slightly up
		tiles[TOP_LEFT]->setPositionY(topLeft.second + offset);
		tiles[TOP_CENTER]->setPositionY(topCenter.second + offset);
		tiles[TOP_RIGHT]->setPositionY(topRight.second + offset);

		//Middle row will start at top row
		tiles[MIDDLE_LEFT]->setPositionY(topLeft.second);
		tiles[MIDDLE_CENTER]->setPositionY(topCenter.second);
		tiles[MIDDLE_RIGHT]->setPositionY(topRight.second);

		//Bottom row will start at middle row
		tiles[BOTTOM_LEFT]->setPositionY(middleLeft.second);
		tiles[BOTTOM_CENTER]->setPositionY(middleCenter.second);
		tiles[BOTTOM_RIGHT]->setPositionY(middleRight.second);

		//Move top row into place over 150 ms, starting at 0 ms
		tiles[TOP_LEFT]->RestoreTile(topLeft.first, topLeft.second, 0.15, 0);
		tiles[TOP_CENTER]->RestoreTile(topCenter.first, topCenter.second, 0.15, 0);
		tiles[TOP_RIGHT]->RestoreTile(topRight.first, topRight.second, 0.15, 0);

		//Move middle row into place over 300 ms, starting at 80 ms
		tiles[MIDDLE_LEFT]->RestoreTile(middleLeft.first, middleLeft.second, 0.3, 0.08);
		tiles[MIDDLE_CENTER]->RestoreTile(middleCenter.first, middleCenter.second, 0.3, 0.08);
		tiles[MIDDLE_RIGHT]->RestoreTile(middleRight.first, middleRight.second, 0.3, 0.08);

		//Move bottom row into place over 300 ms, starting at 250 ms
		tiles[BOTTOM_LEFT]->RestoreTile(bottomLeft.first, bottomLeft.second, 0.3, 0.25);
		tiles[BOTTOM_CENTER]->RestoreTile(bottomCenter.first, bottomCenter.second, 0.3, 0.25);
		tiles[BOTTOM_RIGHT]->RestoreTile(bottomRight.first, bottomRight.second, 0.3, 0.25);
		break;
	case 6: //Slide rows up from bottom
		//First, place bottom row slightly down
		tiles[BOTTOM_LEFT]->setPositionY(bottomLeft.second - offset);
		tiles[BOTTOM_CENTER]->setPositionY(bottomCenter.second - offset);
		tiles[BOTTOM_RIGHT]->setPositionY(bottomRight.second - offset);

		//Middle row will start at bottom row
		tiles[MIDDLE_LEFT]->setPositionY(bottomLeft.second);
		tiles[MIDDLE_CENTER]->setPositionY(bottomCenter.second);
		tiles[MIDDLE_RIGHT]->setPositionY(bottomRight.second);

		//Top row will start at middle row
		tiles[TOP_LEFT]->setPositionY(middleLeft.second);
		tiles[TOP_CENTER]->setPositionY(middleCenter.second);
		tiles[TOP_RIGHT]->setPositionY(middleRight.second);

		//Move bottom row into place over 150 ms, starting at 0 ms
		tiles[BOTTOM_LEFT]->RestoreTile(bottomLeft.first, bottomLeft.second, 0.15, 0);
		tiles[BOTTOM_CENTER]->RestoreTile(bottomCenter.first, bottomCenter.second, 0.15, 0);
		tiles[BOTTOM_RIGHT]->RestoreTile(bottomRight.first, bottomRight.second, 0.15, 0);

		//Move middle row into place over 300 ms, starting at 80 ms
		tiles[MIDDLE_LEFT]->RestoreTile(middleLeft.first, middleLeft.second, 0.3, 0.08);
		tiles[MIDDLE_CENTER]->RestoreTile(middleCenter.first, middleCenter.second, 0.3, 0.08);
		tiles[MIDDLE_RIGHT]->RestoreTile(middleRight.first, middleRight.second, 0.3, 0.08);

		//Move top row into place over 300 ms, starting at 250 ms
		tiles[TOP_LEFT]->RestoreTile(topLeft.first, topLeft.second, 0.3, 0.25);
		tiles[TOP_CENTER]->RestoreTile(topCenter.first, topCenter.second, 0.3, 0.25);
		tiles[TOP_RIGHT]->RestoreTile(topRight.first, topRight.second, 0.3, 0.25);
		break;
	case 7: //Rows slide out from middle
		//First, place all rows in middle
		tiles[TOP_LEFT]->setPositionY(middleLeft.second);
		tiles[TOP_CENTER]->setPositionY(middleLeft.second);
		tiles[TOP_RIGHT]->setPositionY(middleLeft.second);

		tiles[MIDDLE_LEFT]->setPositionY(middleCenter.second);
		tiles[MIDDLE_CENTER]->setPositionY(middleCenter.second);
		tiles[MIDDLE_RIGHT]->setPositionY(middleCenter.second);

		tiles[BOTTOM_LEFT]->setPositionY(middleRight.second);
		tiles[BOTTOM_CENTER]->setPositionY(middleRight.second);
		tiles[BOTTOM_RIGHT]->setPositionY(middleRight.second);

		//Middle row fades in
		tiles[MIDDLE_LEFT]->RestoreTile(0, 0, 0.4, 0);
		tiles[MIDDLE_CENTER]->RestoreTile(0, 0, 0.4, 0);
		tiles[MIDDLE_RIGHT]->RestoreTile(0, 0, 0.4, 0);

		tiles[TOP_LEFT]->RestoreTile(topLeft.first, topLeft.second, 0.4, 0);
		tiles[TOP_CENTER]->RestoreTile(topCenter.first, topCenter.second, 0.4, 0);
		tiles[TOP_RIGHT]->RestoreTile(topRight.first, topRight.second, 0.4, 0);

		tiles[BOTTOM_LEFT]->RestoreTile(bottomLeft.first, bottomLeft.second, 0.4, 0);
		tiles[BOTTOM_CENTER]->RestoreTile(bottomCenter.first, bottomCenter.second, 0.4, 0);
		tiles[BOTTOM_RIGHT]->RestoreTile(bottomRight.first, bottomRight.second, 0.4, 0);
		break;
	}

	return;
}

bool Game::CheckForAchievements()
{
	bool earnedAllAchievements = true;

    if(testing && testingAchievements)
    {
		//THIS IS FOR RESTING PURPOSES. UNLOCK ALL ACHIEVEMENTS EVERY GAME TO CHECK FONT SIZES
		achievementsUnlocked++;
		achievementsText.push_back(nrgText::create("First Match\n\nYou're on your way to becoming a matching machine.", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));

		achievementsUnlocked++;
		achievementsText.push_back(nrgText::create("Just in Time: Make a Match With 0 Seconds Left on the Clock\n\nPulling off a match at the last second is a great feeling.", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));

		achievementsUnlocked++;
		achievementsText.push_back(nrgText::create("Serial Procrastinator: Get 10 Consecutive Matches With Less Than 0 Seconds Left on the Clock\n\nCareful, you're matching on borrowed time.", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));

		achievementsUnlocked++;
		achievementsText.push_back(nrgText::create("Slow Poke\n\nTake over 20 seconds to make a match. Hey, slow and steady wins the race, right?", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));

		achievementsUnlocked++;
		achievementsText.push_back(nrgText::create("Golf Rules\n\nBeat 100 Matches Mode with a score of 500 points. If patience is a virtue, you have a lot of virtue!", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));

		achievementsUnlocked++;
		achievementsText.push_back(nrgText::create("Take the Points: Trade 100x bonus items for points\n\nYou have confidence in your natural skills.", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));

		achievementsUnlocked++;
		achievementsText.push_back(nrgText::create("Take the Bonus: Accept 100x bonus items\n\nYou've mastered the tricks of the trade.", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));

		achievementsUnlocked++;
		achievementsText.push_back(nrgText::create("All the Time in the World: Use 50x Time Plus bonus items\n\nTime is on your side. You've seen to it.", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));

		achievementsUnlocked++;
		achievementsText.push_back(nrgText::create("Process of Elimination: Use 50x Eliminate bonus items\n\nNothing wrong with evening the odds.", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));

		achievementsUnlocked++;
		achievementsText.push_back(nrgText::create("Seeing is Believing: Use 50x Show Match bonus items\n\nSometimes you just need a sure thing.", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));

		achievementsUnlocked++;
		achievementsText.push_back(nrgText::create("Clean Slate: Use 50x Reset bonus items\n\nNothing like a fresh start, right?", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));

		achievementsUnlocked++;
		achievementsText.push_back(nrgText::create("Match Marathon: Get 200 Matches in One Day\n\nThat's what I call dedication.", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));

		achievementsUnlocked++;
		achievementsText.push_back(nrgText::create("Color Matching Pro: Make 500 Total Color Matches\n\nColor me impressed!", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));

		achievementsUnlocked++;
		achievementsText.push_back(nrgText::create("Shape Matching Pro: Make 500 Total Shape Matches\n\nThose shapes never saw it coming.", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));

		achievementsUnlocked++;
		achievementsText.push_back(nrgText::create("Style Matching Pro: Make 500 Total Style Matches\n\nYou match so stylishly.", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));

		achievementsUnlocked++;
		achievementsText.push_back(nrgText::create("Matchmaker in Training: Get 100 Total Matches\n\nThere's a bright future for you, here.", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));

		achievementsUnlocked++;
		achievementsText.push_back(nrgText::create("Junior Matchmaker: Get 1,000 Total Matches\n\nYou're becoming quite the matchmaker.", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));

		achievementsUnlocked++;
		achievementsText.push_back(nrgText::create("Classically Trained: Get 1,000 Total Matches (Classic)\n\nYou match well under pressure!", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));

		achievementsUnlocked++;
		achievementsText.push_back(nrgText::create("Time Management: Get 1,000 Total Matches (Countdown)\n\nThe timer is no match for your matchmaking skills!", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));

		achievementsUnlocked++;
		achievementsText.push_back(nrgText::create("Practice Makes Perfect: Get 1,000 Total Matches (100 Matches)\n\nAll that practice will definitely pay off!", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));

		achievementsUnlocked++;
		achievementsText.push_back(nrgText::create("Photographic Memory: Get 1,000 Total Matches (Memory)\n\nYour matchmaking ability will never be forgotten!", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));

		achievementsUnlocked++;
		achievementsText.push_back(nrgText::create("Seasoned Matchmaker: Get 5,000 Total Matches\n\nImpressive. Now, go for Pro!", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));

		achievementsUnlocked++;
		achievementsText.push_back(nrgText::create("Pro Matchmaker: Get 10,000 Total Matches\n\nYou're a certified Matching Machine.", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));

		achievementsUnlocked++;
		achievementsText.push_back(nrgText::create("The 10,000 Club: Earn 10,000 Total Points\n\nThis is only the beginning. Show us what you've got!", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));

		achievementsUnlocked++;
		achievementsText.push_back(nrgText::create("Aspiring Star: Earn 50,000 Total Points\n\nKeep it up! You're doing awesome.", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));

		achievementsUnlocked++;
		achievementsText.push_back(nrgText::create("Six Figures: Earn 100,000 Total Points\n\nYou're kind of a big deal, now.", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));

		achievementsUnlocked++;
		achievementsText.push_back(nrgText::create("The 500,000 Club: Earn 500,000 Total Points\n\nWe didn't want to say it earlier, but The 10,000 Club is nothing. This is where it's at.", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));

		achievementsUnlocked++;
		achievementsText.push_back(nrgText::create("Top Earner: Earn 1,000,000 Total Points\n\nIt is with great honor that we hand you this award. You've put in the hours. You've earned it.", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));

		achievementsUnlocked++;
		achievementsText.push_back(nrgText::create("Difficult. Not Impossible\n\nYou got your first Classic Match! Give yourself a pat on the back. Not everyone can do that.", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));

		achievementsUnlocked++;
		achievementsText.push_back(nrgText::create("Reach 10 Matches (Classic)\n\nI see potential in you...", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));

		achievementsUnlocked++;
		achievementsText.push_back(nrgText::create("Reach 20 Matches (Classic)\n\nSee? 3 seconds is plenty of time!", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));

		achievementsUnlocked++;
		achievementsText.push_back(nrgText::create("Reach 30 Matches (Classic)\n\nSorry, it's only going to get harder from here...", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));

		achievementsUnlocked++;
		achievementsText.push_back(nrgText::create("Reach 50 Matches (Classic)\n\nI knew it. When I saw you 20 or so matches ago, I knew you had something special.", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));

		achievementsUnlocked++;
		achievementsText.push_back(nrgText::create("Reach 60 Matches (Classic)\n\nYou should be feeling good about yourself. Classic is no walk in the park. Keep it up!", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));

		achievementsUnlocked++;
		achievementsText.push_back(nrgText::create("Reach 70 Matches (Classic)\n\nWhew. Give your thumbs the rest of the week off!", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));

		achievementsUnlocked++;
		achievementsText.push_back(nrgText::create("Reach 80 Matches (Classic)\n\nLet's take a moment to remember when you thought the first 20 matches were hard...", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));

		achievementsUnlocked++;
		achievementsText.push_back(nrgText::create("Reach 90 Matches (Classic)\n\nMan, we're running out of achievements for you!", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));

		achievementsUnlocked++;
		achievementsText.push_back(nrgText::create("No Assistance Necessary (Classic): Get to 50 Matches in Classic Without Using Bonus Items\n\nImpressive. That kind of restraint doesn't go unnoticed!", "fonts/POE.ttf", achievementTextSize - 1, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));

		achievementsUnlocked++;
		achievementsUnlocked++; //Need extra one for second page of text
		achievementsText.push_back(nrgText::create("Mastered Classic: Reach 100 Matches\n\nCongratulations! You've completed 100 Classic Matches! You've officially been classically trained in the art of Tap 3!", "fonts/POE.ttf", achievementTextSize - 2, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));
		achievementsText.push_back(nrgText::create("Mastered Classic: Reach 100 Matches\n\nThank you soooo-oooo much for enjoying Tap 3!!!", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));

		achievementsUnlocked++;
		achievementsText.push_back(nrgText::create("Reach 10 Matches (Countdown)\n\nHey, you're getting the hang of this!", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));

		achievementsUnlocked++;
		achievementsText.push_back(nrgText::create("Reach 20 Matches (Countdown)\n\nThat wasn't too hard, right?", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));

		achievementsUnlocked++;
		achievementsText.push_back(nrgText::create("Reach 30 Matches (Countdown)\n\nOk then, we'll have to crank up the difficulty...", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));

		achievementsUnlocked++;
		achievementsText.push_back(nrgText::create("Reach 50 Matches (Countdown)\n\nYou've got 50 matches under your belt. You know the drill. You can do this.", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));

		achievementsUnlocked++;
		achievementsText.push_back(nrgText::create("Reach 60 Matches (Countdown)\n\nI think we have a natural-born matcher over here!", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));

		achievementsUnlocked++;
		achievementsText.push_back(nrgText::create("Reach 70 Matches (Countdown)\n\nSuch precision. Such efficiency. You are a true Tap 3 Prodigy.", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));

		achievementsUnlocked++;
		achievementsText.push_back(nrgText::create("Reach 80 Matches (Countdown)\n\nYou just earned some bragging rights.", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));

		achievementsUnlocked++;
		achievementsText.push_back(nrgText::create("Reach 90 Matches (Countdown)\n\nIs there nothing we can do to stop you???", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));

		achievementsUnlocked++;
		achievementsText.push_back(nrgText::create("No Assistance Necessary (Countdown): Get to 50 Matches in Countdown Without Using Bonus Items\n\nImpressive. That kind of restraint doesn't go unnoticed!", "fonts/POE.ttf", achievementTextSize - 1, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));

		achievementsUnlocked++;
		achievementsText.push_back(nrgText::create("\nTime to Spare: Increase Remaining Time to 90 Seconds or More in Countdown\n\nYou're either very fast, or used some bonus items. Either way, nice work.", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));

		achievementsUnlocked++;
		achievementsText.push_back(nrgText::create("\nTime Keeper: Keep Your Remaining Time Above 20 Seconds for 50 Matches in Countdown\n\nLook at those thumbs go! You waste no time making matches.", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));

		achievementsUnlocked++;
		achievementsText.push_back(nrgText::create("Speed Demon: Get First 5 Matches in 12 Seconds in Countdown\n\nThat's some quick thinking!", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));

		achievementsUnlocked++;
		achievementsText.push_back(nrgText::create("Speed Demon II: Get First 10 Matches in 24 Seconds in Countdown\n\nYou are lightning fast!", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));

		achievementsUnlocked++;
		achievementsText.push_back(nrgText::create("Speed Demon III: Get First 20 Matches in 50 Seconds in Countdown\n\nCongrats. That's actually faster than we thought humanly possible!", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));

		achievementsUnlocked++;
		achievementsUnlocked++; //Need extra one for second page of text
		achievementsText.push_back(nrgText::create("Mastered Countdown: Reach 100 Matches\n\nCongratulations! You've completed 100 Countdown Matches! You've got some fast thumbs and a keen eye!", "fonts/POE.ttf", achievementTextSize - 1, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));
		achievementsText.push_back(nrgText::create("Mastered Countdown: Reach 100 Matches\n\nThank you soooo-oooo much for enjoying Tap 3!!! We hope you still have time for a few more rounds!", "fonts/POE.ttf", achievementTextSize - 1, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));
			
		achievementsUnlocked++;
		achievementsText.push_back(nrgText::create("Reach 10 Matches (Memory)\n\nYou see? It's possible when you put your mind to it!", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));

		achievementsUnlocked++;
		achievementsText.push_back(nrgText::create("Reach 20 Matches (Memory)\n\nYou've got a knack for this!", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));

		achievementsUnlocked++;
		achievementsText.push_back(nrgText::create("Reach 30 Matches (Memory)\n\nMy mind can't comprehend what your mind is doing!", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));

		achievementsUnlocked++;
		achievementsText.push_back(nrgText::create("Reach 50 Matches (Memory)\n\nRemember when this seemed impossible? (That's a memory-themed joke).", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));

		achievementsUnlocked++;
		achievementsText.push_back(nrgText::create("Reach 60 Matches (Memory)\n\nOk... You don't have a photographic memory, do you?", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));

		achievementsUnlocked++;
		achievementsText.push_back(nrgText::create("Reach 70 Matches (Memory)\n\nWhew. Doesn't your head hurt? Of course, not. You're some kind of genius...", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));

		achievementsUnlocked++;
		achievementsText.push_back(nrgText::create("Reach 80 Matches (Memory)\n\nYou're definitely one of those people who remembers what they had for breakfast.", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));

		achievementsUnlocked++;
		achievementsText.push_back(nrgText::create("Reach 90 Matches (Memory)\n\nI guess you CAN do anything when you put your mind to it!", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));

		achievementsUnlocked++;
		achievementsText.push_back(nrgText::create("No Assistance Necessary (Memory): Get to 50 Matches in Memory Without Using Bonus Items\n\nImpressive. That kind of restraint doesn't go unnoticed!", "fonts/POE.ttf", achievementTextSize - 1, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));

		achievementsUnlocked++;
		achievementsUnlocked++; //Need extra one for second page of text
		achievementsText.push_back(nrgText::create("Mastered Memory: Reach 100 Matches\n\nCongratulations! You've completed 100 Memory Matches! Your mind is a force to be reckoned with!", "fonts/POE.ttf", achievementTextSize - 1, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));
		achievementsText.push_back(nrgText::create("Mastered Memory: Reach 100 Matches\n\nThank you soooo-oooo much for enjoying Tap 3!!! We hope you cherish the memories!", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));

		achievementsUnlocked++;
		achievementsText.push_back(nrgText::create("Tap 3 Platinum Trophy: You've Earned Every Achievement!\n\nCongratulations! You're in a league of your own! Thank you so much for enjoying Tap 3!!!", "fonts/POE.ttf", achievementTextSize - 1, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));
        
		for (int iii = 0; iii < achievementsUnlocked; iii++)
		{
			addChild(achievementsText[iii]);
			achievementsText[iii]->GetLabel()->setOpacity(0);
		}

		//Unlock all achievements with Game Center
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
		sdkbox::PluginSdkboxPlay::unlockAchievement("time_to_spare");
		sdkbox::PluginSdkboxPlay::unlockAchievement("time_keeper");
		sdkbox::PluginSdkboxPlay::unlockAchievement("speed_demon");
		sdkbox::PluginSdkboxPlay::unlockAchievement("speed_demon_2");
		sdkbox::PluginSdkboxPlay::unlockAchievement("speed_demon_3");

		sdkbox::PluginSdkboxPlay::unlockAchievement("10_matches_classic");
		sdkbox::PluginSdkboxPlay::unlockAchievement("20_matches_classic");
		sdkbox::PluginSdkboxPlay::unlockAchievement("30_matches_classic");
		sdkbox::PluginSdkboxPlay::unlockAchievement("50_matches_classic");
		sdkbox::PluginSdkboxPlay::unlockAchievement("60_matches_classic");
		sdkbox::PluginSdkboxPlay::unlockAchievement("70_matches_classic");
		sdkbox::PluginSdkboxPlay::unlockAchievement("80_matches_classic");
		sdkbox::PluginSdkboxPlay::unlockAchievement("90_matches_classic");
		sdkbox::PluginSdkboxPlay::unlockAchievement("no_assistance_classic");
		sdkbox::PluginSdkboxPlay::unlockAchievement("mastered_classic");

		sdkbox::PluginSdkboxPlay::unlockAchievement("10_matches_countdown");
		sdkbox::PluginSdkboxPlay::unlockAchievement("20_matches_countdown");
		sdkbox::PluginSdkboxPlay::unlockAchievement("30_matches_countdown");
		sdkbox::PluginSdkboxPlay::unlockAchievement("50_matches_countdown");
		sdkbox::PluginSdkboxPlay::unlockAchievement("60_matches_countdown");
		sdkbox::PluginSdkboxPlay::unlockAchievement("70_matches_countdown");
		sdkbox::PluginSdkboxPlay::unlockAchievement("80_matches_countdown");
		sdkbox::PluginSdkboxPlay::unlockAchievement("90_matches_countdown");
		sdkbox::PluginSdkboxPlay::unlockAchievement("no_assistance_countdown");
		sdkbox::PluginSdkboxPlay::unlockAchievement("mastered_countdown");

		sdkbox::PluginSdkboxPlay::unlockAchievement("10_matches_memory");
		sdkbox::PluginSdkboxPlay::unlockAchievement("20_matches_memory");
		sdkbox::PluginSdkboxPlay::unlockAchievement("30_matches_memory");
		sdkbox::PluginSdkboxPlay::unlockAchievement("50_matches_memory");
		sdkbox::PluginSdkboxPlay::unlockAchievement("60_matches_memory");
		sdkbox::PluginSdkboxPlay::unlockAchievement("70_matches_memory");
		sdkbox::PluginSdkboxPlay::unlockAchievement("80_matches_memory");
		sdkbox::PluginSdkboxPlay::unlockAchievement("90_matches_memory");
		sdkbox::PluginSdkboxPlay::unlockAchievement("no_assistance_memory");
		sdkbox::PluginSdkboxPlay::unlockAchievement("mastered_memory");

		sdkbox::PluginSdkboxPlay::unlockAchievement("first_match");
		sdkbox::PluginSdkboxPlay::unlockAchievement("just_in_time");
		sdkbox::PluginSdkboxPlay::unlockAchievement("serial_procrastinator");
		sdkbox::PluginSdkboxPlay::unlockAchievement("slow_poke");
		sdkbox::PluginSdkboxPlay::unlockAchievement("golf_rules");
		sdkbox::PluginSdkboxPlay::unlockAchievement("match_marathon");
		sdkbox::PluginSdkboxPlay::unlockAchievement("color_matching_pro");
		sdkbox::PluginSdkboxPlay::unlockAchievement("shape_matching_pro");
		sdkbox::PluginSdkboxPlay::unlockAchievement("style_matching_pro");
		sdkbox::PluginSdkboxPlay::unlockAchievement("match_maker");
		sdkbox::PluginSdkboxPlay::unlockAchievement("top_earner");

		sdkbox::PluginSdkboxPlay::unlockAchievement(take_the_points);
		sdkbox::PluginSdkboxPlay::unlockAchievement(take_the_bonus);
		sdkbox::PluginSdkboxPlay::unlockAchievement(all_the_time_in_the_world);
		sdkbox::PluginSdkboxPlay::unlockAchievement(process_of_elimination);
		sdkbox::PluginSdkboxPlay::unlockAchievement(seeing_is_believing);
		sdkbox::PluginSdkboxPlay::unlockAchievement(clean_slate);
		sdkbox::PluginSdkboxPlay::unlockAchievement(matchmaker_training);
		sdkbox::PluginSdkboxPlay::unlockAchievement(matchmaker_junior);
		sdkbox::PluginSdkboxPlay::unlockAchievement(classically_trained);
		sdkbox::PluginSdkboxPlay::unlockAchievement(time_management);
		sdkbox::PluginSdkboxPlay::unlockAchievement(practice_makes_perfect);
		sdkbox::PluginSdkboxPlay::unlockAchievement(photographic_memory);
		sdkbox::PluginSdkboxPlay::unlockAchievement(matchmaker_seasoned);
		sdkbox::PluginSdkboxPlay::unlockAchievement(the_10000_club);
		sdkbox::PluginSdkboxPlay::unlockAchievement(aspiring_star);
		sdkbox::PluginSdkboxPlay::unlockAchievement(six_figures);
		sdkbox::PluginSdkboxPlay::unlockAchievement(the_500000_club);

		sdkbox::PluginSdkboxPlay::unlockAchievement("mastered_tap_3");
#endif
      
    }
    
    else
    {
        //Check if achievement criteria has been met
        if (!gameData->getBoolForKey(first_match, false))
        {
            if (matches > 0)
            {
				gameData->setBoolForKey(first_match, true);
				achievementsUnlocked++;
				achievementsText.push_back(nrgText::create("First Match\n\nYou're on your way to becoming a matching machine.", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));
			}
			else
				earnedAllAchievements = false;
		}

		if (!gameData->getBoolForKey(near_miss, false))
		{
			if (nearMiss)
			{
				gameData->setBoolForKey(near_miss, true);
				achievementsUnlocked++;
				achievementsText.push_back(nrgText::create("Just in Time: Make a Match With 0 Seconds Left on the Clock\n\nPulling off a match at the last second is a great feeling.", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));
			}
			else
				earnedAllAchievements = false;
		}

		if (!gameData->getBoolForKey(ten_consecutive_near_misses, false))
		{
			if (gameData->getIntegerForKey(consecutive_near_misses, 0) >= 10)
			{
				gameData->setBoolForKey(ten_consecutive_near_misses, true);
				achievementsUnlocked++;
				achievementsText.push_back(nrgText::create("Serial Procrastinator: Get 10 Consecutive Matches With Less Than 0 Seconds Left on the Clock\n\nCareful, you're matching on borrowed time.", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));
			}
			else
				earnedAllAchievements = false;
		}

		if (!gameData->getBoolForKey(take_the_points, false))
		{
			if (gameData->getIntegerForKey(trades_accepted, 0) >= 100)
			{
				gameData->setBoolForKey(take_the_points, true);
				achievementsUnlocked++;
				achievementsText.push_back(nrgText::create("Take the Points: Trade 100x bonus items for points\n\nYou have confidence in your natural skills.", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));
			}
			else
				earnedAllAchievements = false;
		}

		if (!gameData->getBoolForKey(take_the_bonus, false))
		{
			if (gameData->getIntegerForKey(bonuses_accepted, 0) >= 100)
			{
				gameData->setBoolForKey(take_the_bonus, true);
				achievementsUnlocked++;
				achievementsText.push_back(nrgText::create("Take the Bonus: Accept 100x bonus items\n\nYou've mastered the tricks of the trade.", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));
			}
			else
				earnedAllAchievements = false;
		}

		if (!gameData->getBoolForKey(all_the_time_in_the_world, false))
		{
			if (gameData->getIntegerForKey(time_plus_used, 0) >= 50)
			{
				gameData->setBoolForKey(all_the_time_in_the_world, true);
				achievementsUnlocked++;
				achievementsText.push_back(nrgText::create("All the Time in the World: Use 50x Time Plus bonus items\n\nTime is on your side. You've seen to it.", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));
			}
			else
				earnedAllAchievements = false;
		}
		
		if (!gameData->getBoolForKey(process_of_elimination, false))
		{
			if (gameData->getIntegerForKey(eliminate_used, 0) >= 50)
			{
				gameData->setBoolForKey(process_of_elimination, true);
				achievementsUnlocked++;
				achievementsText.push_back(nrgText::create("Process of Elimination: Use 50x Eliminate bonus items\n\nNothing wrong with evening the odds.", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));
			}
			else
				earnedAllAchievements = false;
		}

		if (!gameData->getBoolForKey(seeing_is_believing, false))
		{
			if (gameData->getIntegerForKey(show_match_used, 0) >= 50)
			{
				gameData->setBoolForKey(seeing_is_believing, true);
				achievementsUnlocked++;
				achievementsText.push_back(nrgText::create("Seeing is Believing: Use 50x Show Match bonus items\n\nSometimes you just need a sure thing.", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));
			}
			else
				earnedAllAchievements = false;
		}

		if (!gameData->getBoolForKey(clean_slate, false))
		{
			if (gameData->getIntegerForKey(new_card_used, 0) >= 50)
			{
				gameData->setBoolForKey(clean_slate, true);
				achievementsUnlocked++;
				achievementsText.push_back(nrgText::create("Clean Slate: Use 50x Reset bonus items\n\nNothing like a fresh start, right?", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));
			}
			else
				earnedAllAchievements = false;
		}

		if (!gameData->getBoolForKey(two_hundred_matches_in_one_day, false))
		{
			if (gameData->getIntegerForKey(matches_made_today, 0) >= 200)
			{
				gameData->setBoolForKey(two_hundred_matches_in_one_day, true);
				achievementsUnlocked++;
				achievementsText.push_back(nrgText::create("Match Marathon: Get 200 Matches in One Day\n\nThat's what I call dedication.", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));
			}
			else
				earnedAllAchievements = false;
		}

		if (!gameData->getBoolForKey(five_hundred_color_matches, false))
		{
			if (gameData->getIntegerForKey(total_color_matches, 0) >= 500)
			{
				gameData->setBoolForKey(five_hundred_color_matches, true);
				achievementsUnlocked++;
				achievementsText.push_back(nrgText::create("Color Matching Pro: Make 500 Total Color Matches\n\nColor me impressed!", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));
			}
			else
				earnedAllAchievements = false;
		}

		if (!gameData->getBoolForKey(five_hundred_shape_matches, false))
		{
			if (gameData->getIntegerForKey(total_shape_matches, 0) >= 500)
			{
				gameData->setBoolForKey(five_hundred_shape_matches, true);
				achievementsUnlocked++;
				achievementsText.push_back(nrgText::create("Shape Matching Pro: Make 500 Total Shape Matches\n\nThose shapes never saw it coming.", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));
			}
			else
				earnedAllAchievements = false;
		}

		if (!gameData->getBoolForKey(five_hundred_style_matches, false))
		{
			if (gameData->getIntegerForKey(total_style_matches, 0) >= 500)
			{
				gameData->setBoolForKey(five_hundred_style_matches, true);
				achievementsUnlocked++;
				achievementsText.push_back(nrgText::create("Style Matching Pro: Make 500 Total Style Matches\n\nYou match so stylishly.", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));
			}
			else
				earnedAllAchievements = false;
		}

		if (!gameData->getBoolForKey(matchmaker_training, false))
		{
			if (gameData->getIntegerForKey(total_matches, 0) >= 100)
			{
				gameData->setBoolForKey(matchmaker_training, true);
				achievementsUnlocked++;
				achievementsText.push_back(nrgText::create("Matchmaker in Training: Get 100 Total Matches\n\nThere's a bright future for you, here.", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));
			}
			else
				earnedAllAchievements = false;
		}

		if (!gameData->getBoolForKey(matchmaker_junior, false))
		{
			if (gameData->getIntegerForKey(total_matches, 0) >= 1000)
			{
				gameData->setBoolForKey(matchmaker_junior, true);
				achievementsUnlocked++;
				achievementsText.push_back(nrgText::create("Junior Matchmaker: Get 1,000 Total Matches\n\nYou're becoming quite the matchmaker.", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));
			}
			else
				earnedAllAchievements = false;
		}

		if (!gameData->getBoolForKey(classically_trained, false))
		{
			if (gameData->getIntegerForKey(classic_matches, 0) >= 1000)
			{
				gameData->setBoolForKey(classically_trained, true);
				achievementsUnlocked++;
				achievementsText.push_back(nrgText::create("Classically Trained: Get 1,000 Total Matches (Classic)\n\nYou match well under pressure!", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));
			}
			else
				earnedAllAchievements = false;
		}

		if (!gameData->getBoolForKey(time_management, false))
		{
			if (gameData->getIntegerForKey(countdown_matches, 0) >= 1000)
			{
				gameData->setBoolForKey(time_management, true);
				achievementsUnlocked++;
				achievementsText.push_back(nrgText::create("Time Management: Get 1,000 Total Matches (Countdown)\n\nThe timer is no match for your matchmaking skills!", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));
			}
			else
				earnedAllAchievements = false;
		}

		if (!gameData->getBoolForKey(practice_makes_perfect, false))
		{
			if (gameData->getIntegerForKey(practice_matches, 0) >= 1000)
			{
				gameData->setBoolForKey(practice_makes_perfect, true);
				achievementsUnlocked++;
				achievementsText.push_back(nrgText::create("Practice Makes Perfect: Get 1,000 Total Matches (100 Matches)\n\nAll that practice will definitely pay off!", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));
			}
			else
				earnedAllAchievements = false;
		}

		if (!gameData->getBoolForKey(photographic_memory, false))
		{
			if (gameData->getIntegerForKey(memory_matches, 0) >= 1000)
			{
				gameData->setBoolForKey(photographic_memory, true);
				achievementsUnlocked++;
				achievementsText.push_back(nrgText::create("Photographic Memory: Get 1,000 Total Matches (Memory)\n\nYour matchmaking ability will never be forgotten!", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));
			}
			else
				earnedAllAchievements = false;
		}

		if (!gameData->getBoolForKey(matchmaker_seasoned, false))
		{
			if (gameData->getIntegerForKey(total_matches, 0) >= 5000)
			{
				gameData->setBoolForKey(matchmaker_seasoned, true);
				achievementsUnlocked++;
				achievementsText.push_back(nrgText::create("Seasoned Matchmaker: Get 5,000 Total Matches\n\nImpressive. Now, go for Pro!", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));
			}
			else
				earnedAllAchievements = false;
		}

		if (!gameData->getBoolForKey(earned_10000_total_matches, false))
		{
			if (gameData->getIntegerForKey(total_matches, 0) >= 10000)
			{
				gameData->setBoolForKey(earned_10000_total_matches, true);
				achievementsUnlocked++;
				achievementsText.push_back(nrgText::create("Pro Matchmaker: Get 10,000 Total Matches\n\nYou're a certified Matching Machine.", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));
			}
			else
				earnedAllAchievements = false;
		}

		if (!gameData->getBoolForKey(the_10000_club, false))
		{
			if (gameData->getIntegerForKey(total_points, 0) >= 10000)
			{
				gameData->setBoolForKey(the_10000_club, true);
				achievementsUnlocked++;
				achievementsText.push_back(nrgText::create("The 10,000 Club: Earn 10,000 Total Points\n\nThis is only the beginning. Show us what you've got!", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));
			}
			else
				earnedAllAchievements = false;
		}

		if (!gameData->getBoolForKey(aspiring_star, false))
		{
			if (gameData->getIntegerForKey(total_points, 0) >= 50000)
			{
				gameData->setBoolForKey(aspiring_star, true);
				achievementsUnlocked++;
				achievementsText.push_back(nrgText::create("Aspiring Star: Earn 50,000 Total Points\n\nKeep it up! You're doing awesome.", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));
			}
			else
				earnedAllAchievements = false;
		}

		if (!gameData->getBoolForKey(six_figures, false))
		{
			if (gameData->getIntegerForKey(total_points, 0) >= 100000)
			{
				gameData->setBoolForKey(six_figures, true);
				achievementsUnlocked++;
				achievementsText.push_back(nrgText::create("Six Figures: Earn 100,000 Total Points\n\nYou're kind of a big deal, now.", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));
			}
			else
				earnedAllAchievements = false;
		}

		if (!gameData->getBoolForKey(the_500000_club, false))
		{
			if (gameData->getIntegerForKey(total_points, 0) >= 500000)
			{
				gameData->setBoolForKey(the_500000_club, true);
				achievementsUnlocked++;
				achievementsText.push_back(nrgText::create("The 500,000 Club: Earn 500,000 Total Points\n\nWe didn't want to say it earlier, but The 10,000 Club is nothing. This is where it's at.", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));
			}
			else
				earnedAllAchievements = false;
		}

		if (!gameData->getBoolForKey(earned_1000000_total_points, false))
		{
			if (gameData->getIntegerForKey(total_points, 0) >= 1000000)
			{
				gameData->setBoolForKey(earned_1000000_total_points, true);
				achievementsUnlocked++;
				achievementsText.push_back(nrgText::create("Top Earner: Earn 1,000,000 Total Points\n\nIt is with great honor that we hand you this award. You've put in the hours. You've earned it.", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));
			}
			else
				earnedAllAchievements = false;
		}    
    }
    
	//Checks earned achievements against GameCenter and GooglePlay if not already submitted
	SyncAchievements();

	return earnedAllAchievements;
}

void Game::DisplayAchievements()
{
	if (achievementsText[0]->GetLabel()->getNumberOfRunningActions() == 0)
	{
		achievementsText[0]->GetLabel()->stopAllActions();
		achievementsText[0]->Center();
		achievementsText[0]->GetLabel()->runAction(cocos2d::FadeIn::create(0.25));
	}

	return;
}

void Game::RandomizeTiles()
{
	//Erase previous records of matching tiles
	matchingTiles.clear();
	matchingTilesVector.clear();
	matchedTilesShowing = 0;
	eliminateUsedOnCurrentCard = 0;
	excludedEliminateTiles.clear();
	excludedTiles.clear();
	
	if (useDesignatedMatchColor)
	{
		colorKey.first = designatedMatchColor.first;
		colorKey.second = designatedMatchColor.second;
		color = colorKey.first;
	}

	else
	{
		//Get randomized match color
		if (!splitColor)
		{ 
			color = nrgFunctions::GetRandom(0, 6, excludedColors);
			colorKey.first = color;
			colorKey.second = color;
		}
		else
			//Decide matching color
			colorKey = nrgFunctions::GetRandomPair(0, 6, 0, 6, excludedColorPairs);
	}

	if (useDesignatedMatchStyle)
	{
		outlinePair.first = designatedMatchStyle.first;
		outlinePair.second = designatedMatchStyle.second;
		outline = outlinePair.first;
	}

	else
	{
		//Get randomized style
		if (!splitStyle)
		{
			outline = nrgFunctions::GetRandom(0, 11, excludedStyles);
			outlinePair.first = outline;
			outlinePair.second = outline;

		}
		else
			outlinePair = nrgFunctions::GetRandomPair(0, 11, 0, 11, excludedStylePairs);
	}

	//Match Parameters for tiles
	if (useDesignatedMatchShape)
		shape = designatedMatchShape;
	else
		shape = nrgFunctions::GetRandom(CIRCLE, OCTAGON, excludedShapes);
	colorPair = GetColorFromKey(colorKey);
	rotation = 0;

	//Exclude matching parameters from future randomization
	if (matchingShapes)
		excludedShapes.emplace(shape);
	if (matchingColors)
	{
		if (!splitColor)
			excludedColors.emplace(color);
		else
			excludedColorPairs.emplace(colorKey);
	}
	if (matchingStyles)
	{
		if (!splitStyle)
			excludedStyles.emplace(outline);
		else
			excludedStylePairs.emplace(outlinePair);
	}
	
	//Randomized parameters for tiles
	int randShape = shape;
	int randColor = color;
	std::pair<int, int> randColorKey = colorKey;
	std::pair<cocos2d::Color3B, cocos2d::Color3B> randColorPair = colorPair;
	int randRotation = rotation;
	int randOutline = outline;
	std::pair<int, int> randOutlinePair = outlinePair;

	
	//Decide which tiles will be matching tiles, and set to match parameters
	for (int iii = 0; iii < numberOfMatchingTiles; iii++)
	{
		int matchTile = nrgFunctions::GetRandom(TOP_LEFT, BOTTOM_RIGHT, excludedTiles);
		matchingTiles.emplace(matchTile);
		matchingTilesVector.push_back(matchTile);

		//Remove tile from being selected again
		excludedTiles.emplace(matchTile);

		//Get which parameters to match, and which to randomize
		if (matchingShapes)
			randShape = shape;
		else
		{
			if (useDesignatedMatchShape)
				randShape = designatedMatchShape;
			else
				randShape = nrgFunctions::GetRandom(CIRCLE, OCTAGON, excludedShapes);
		}

		if (matchingColors)
		{
			randColor = color;
			randColorPair = colorPair;
		}

		else
		{
			if (useDesignatedMatchColor)
			{
				randColorKey.first = designatedMatchColor.first;
				randColorKey.second = designatedMatchColor.second;
				randColor = randColorKey.first;
			}

			else
			{
				if (!splitColor)
				{
					randColor = nrgFunctions::GetRandom(0, 6, excludedColors);
					randColorKey.first = randColor;
					randColorKey.second = randColor;
				}
				else
					randColorKey = nrgFunctions::GetRandomPair(0, 6, 0, 6, excludedColorPairs);
			}
			randColorPair = GetColorFromKey(randColorKey);
		}

		if (matchingStyles)
		{
			randOutline = outline;
			randOutlinePair = outlinePair;
		}

		else
		{
			if (useDesignatedMatchStyle)
			{
				randOutlinePair.first = designatedMatchStyle.first;
				randOutlinePair.second = designatedMatchStyle.second;
				randOutline = randOutlinePair.first;
			}

			else
			{
				if (!splitStyle)
				{
					randOutline = nrgFunctions::GetRandom(0, 11, excludedStyles);
					randOutlinePair.first = randOutline;
					randOutlinePair.second = randOutline;
				}
				else
					randOutlinePair = nrgFunctions::GetRandomPair(0, 11, 0, 11, excludedStylePairs);
			}
		}

		//Keep non matching shape count below 2
		switch (randShape)
		{
		case CIRCLE:
			nonmatchCircleFrequency++;
			if (!nonmatchedShapesCanExceedTwo && nonmatchCircleFrequency == 2)
				excludedShapes.emplace(CIRCLE);
			break;
		case SQUARE:
			nonmatchSquareFrequency++;
			if (!nonmatchedShapesCanExceedTwo && nonmatchSquareFrequency == 2)
				excludedShapes.emplace(SQUARE);
			break;
		case DIAMOND:
			nonmatchDiamondFrequency++;
			if (!nonmatchedShapesCanExceedTwo && nonmatchDiamondFrequency == 2)
				excludedShapes.emplace(DIAMOND);
			break;
		case TRIANGLE:
			nonmatchTriangleFrequency++;
			if (!nonmatchedShapesCanExceedTwo && nonmatchTriangleFrequency == 2)
				excludedShapes.emplace(TRIANGLE);
			break;
		case HEXAGON:
			nonmatchHexagonFrequency++;
			if (!nonmatchedShapesCanExceedTwo && nonmatchHexagonFrequency == 2)
				excludedShapes.emplace(HEXAGON);
			break;
		case OCTAGON:
			nonmatchOctagonFrequency++;
			if (!nonmatchedShapesCanExceedTwo && nonmatchOctagonFrequency == 2)
				excludedShapes.emplace(OCTAGON);
			break;
		}

		//Keep nonmatching color count below 2
		if (!splitColor)
		{
			//Add this color to the list of colors generated
			nonmatchedSolidColors.push_back(randColor);

			//Check through all previously generated colors
			for (int iii = 0; iii < nonmatchedSolidColors.size(); iii++)
			{
				//If color appears in previously generated color, increase frequency count
				if (randColor == nonmatchedSolidColors[iii])
					nonmatchSolidColorsFrequency++;
			}

			//If frequency of color appearance is 2, exclude color from appearing again
			if (!nonmatchedColorsCanExceedTwo && nonmatchSolidColorsFrequency == 2)
				excludedColors.emplace(randColor);

			//Reset frequency count for new color
			nonmatchSolidColorsFrequency = 0;
		}

		else
		{
			//Add this color pair to the list of pairs generated
			nonmatchedColorPairs.push_back(randColorKey);

			//Check through all previously generated color pairs
			for (int iii = 0; iii < nonmatchedColorPairs.size(); iii++)
			{
				//If color appears in generated color pairs, increase frequency count
				if (randColorKey == nonmatchedColorPairs[iii])
					nonmatchColorPairsFrequency++;
			}

			//If frequency of color pair appearance is 2, exclude color pair from appearing again
			if (!nonmatchedColorsCanExceedTwo && nonmatchColorPairsFrequency == 2)
				excludedColorPairs.emplace(randColorKey);

			//Reset frequency count for new color pair
			nonmatchColorPairsFrequency = 0;
		}

		//Keep matching style count below 2
		if (!splitStyle)
		{
			//Add this style to the list of styles generated
			nonmatchedStyles.push_back(randOutline);

			//Check through all previously generated styles
			for (int iii = 0; iii < nonmatchedStyles.size(); iii++)
			{
				//If style appears in previously generated styles, increase frequency count
				if (randOutline == nonmatchedStyles[iii])
					nonmatchStyleFrequency++;
			}

			//If frequency of style appearance is 2, exclude style from appearing again
			if (!nonmatchedStylesCanExceedTwo && nonmatchStyleFrequency == 2)
				excludedStyles.emplace(randOutline);

			//Reset frequency count for new style
			nonmatchStyleFrequency = 0;
		}

		else
		{
			//Add this style pair to the list of pairs generated
			nonmatchedStylePairs.push_back(randOutlinePair);

			//Check through all previously generated style pairs
			for (int iii = 0; iii < nonmatchedStylePairs.size(); iii++)
			{
				//If style appears in previously generated style pairs, increase frequency count
				if (randOutlinePair == nonmatchedStylePairs[iii])
					nonmatchStylePairsFrequency++;
			}

			//If frequency of style pair appearance is 2, exclude style pair from appearing again
			if (!nonmatchedStylesCanExceedTwo && nonmatchStylePairsFrequency == 2)
				excludedStylePairs.emplace(randOutlinePair);

			//Reset frequency count for new style pairs
			nonmatchStylePairsFrequency = 0;
		}

		//Update tile to randomized parameters
		tiles[matchTile]->ChangeShape(randShape, randColorPair, randRotation, randOutlinePair);
	}
	
	//
	//NON MATCH TILES
	//

	//Get initial randomized color, shape, and style for first nonmatching tile
	if (useDesignatedNonmatchShape)
		randShape = designatedNonmatchShape;
	else
		randShape = nrgFunctions::GetRandom(CIRCLE, OCTAGON, excludedShapes);

	if (useDesignatedNonmatchColor)
	{
		randColorKey.first = designatedNonmatchColor.first;
		randColorKey.second = designatedNonmatchColor.second;
		randColor = randColorKey.first;
	}

	else
	{
		if (!splitColor)
		{
			randColor = nrgFunctions::GetRandom(0, 6, excludedColors);
			randColorKey.first = randColor;
			randColorKey.second = randColor;
		}
		else
			randColorKey = nrgFunctions::GetRandomPair(0, 6, 0, 6, excludedColorPairs);
	}
	
	randColorPair = GetColorFromKey(randColorKey);

	if (useDesignatedNonmatchStyle)
	{
		randOutlinePair.first = designatedNonmatchStyle.first;
		randOutlinePair.second = designatedNonmatchStyle.second;
		randOutline = randOutlinePair.first;
	}

	else
	{
		if (!splitStyle)
		{
			randOutline = nrgFunctions::GetRandom(0, 11, excludedStyles);
			randOutlinePair.first = randOutline;
			randOutlinePair.second = randOutline;
		}
		else
			randOutlinePair = nrgFunctions::GetRandomPair(0, 11, 0, 11, excludedStylePairs);
	}
	
	//Randomize non match tiles
	for (int iii = 0; iii < tiles.size() - numberOfMatchingTiles; iii++)
	{
		//First, decide which tile to run logic on
		int nonmatchTile = nrgFunctions::GetRandom(TOP_LEFT, BOTTOM_RIGHT, excludedTiles);

		//Prevent tile from appearing again
		excludedTiles.emplace(nonmatchTile);

		//Use same shape until limit has been reached ( == -1)
		if (numberOfSameNonmatchingShapes > 0)
			numberOfSameNonmatchingShapes--;

		else//Get new randomized shape
		{
			if (useDesignatedNonmatchShape && designatedNonmatchShapeLimit > 0) //First tile will automatically be set to designation, so stop after limit == 1
			{
				randShape = designatedNonmatchShape;
				designatedNonmatchShapeLimit--;
			}
			else
				randShape = nrgFunctions::GetRandom(CIRCLE, OCTAGON, excludedShapes);
		}

		//Use same color until limit has been reached ( == 1)
		if (numberOfSameNonmatchingColors > 0)
			numberOfSameNonmatchingColors--;

		else //Get new randomized color
		{
			if (useDesignatedNonmatchColor && designatedNonmatchColorLimit > 0) //First tile will automatically be set to designation, so stop after limit == 1
			{
				randColorKey.first = designatedNonmatchColor.first;
				randColorKey.second = designatedNonmatchColor.second;
				randColor = randColorKey.first;
				designatedNonmatchColorLimit--;
			}

			else
			{
				if (!splitColor)
				{
					randColor = nrgFunctions::GetRandom(0, 6, excludedColors);
					randColorKey.first = randColor;
					randColorKey.second = randColor;
				}
				else
					randColorKey = nrgFunctions::GetRandomPair(0, 6, 0, 6, excludedColorPairs);
			}
			randColorPair = GetColorFromKey(randColorKey);
		}

		//Use same style until limit has been reached ( == -1)
		if (numberOfSameNonmatchingStyles > 0)
			numberOfSameNonmatchingStyles--;

		else //Get new randomized style
		{
			if (useDesignatedNonmatchStyle && designatedNonmatchStyleLimit > 0) //First tile will automatically be set to designation, so stop after limit == 1
			{
				randOutlinePair.first = designatedNonmatchStyle.first;
				randOutlinePair.second = designatedNonmatchStyle.second;
				randOutline = randOutlinePair.first;
				designatedNonmatchStyleLimit--;
			}

			else
			{
				if (!splitStyle)
				{
					randOutline = nrgFunctions::GetRandom(0, 11, excludedStyles);
					randOutlinePair.first = randOutline;
					randOutlinePair.second = randOutline;
				}
				else
					randOutlinePair = nrgFunctions::GetRandomPair(0, 11, 0, 11, excludedStylePairs);
			}
		}

		//Keep non matching shape count below 2
		switch (randShape)
		{
		case CIRCLE:
			nonmatchCircleFrequency++;
			if (!nonmatchedShapesCanExceedTwo && nonmatchCircleFrequency == 2)
				excludedShapes.emplace(CIRCLE);
			break;
		case SQUARE:
			nonmatchSquareFrequency++;
			if (!nonmatchedShapesCanExceedTwo && nonmatchSquareFrequency == 2)
				excludedShapes.emplace(SQUARE);
			break;
		case DIAMOND:
			nonmatchDiamondFrequency++;
			if (!nonmatchedShapesCanExceedTwo && nonmatchDiamondFrequency == 2)
				excludedShapes.emplace(DIAMOND);
			break;
		case TRIANGLE:
			nonmatchTriangleFrequency++;
			if (!nonmatchedShapesCanExceedTwo && nonmatchTriangleFrequency == 2)
				excludedShapes.emplace(TRIANGLE);
			break;
		case HEXAGON:
			nonmatchHexagonFrequency++;
			if (!nonmatchedShapesCanExceedTwo && nonmatchHexagonFrequency == 2)
				excludedShapes.emplace(HEXAGON);
			break;
		case OCTAGON:
			nonmatchOctagonFrequency++;
			if (!nonmatchedShapesCanExceedTwo && nonmatchOctagonFrequency == 2)
				excludedShapes.emplace(OCTAGON);
			break;
		}

		//Keep nonmatching color count below 2
		if (!splitColor)
		{
			//Add this color to the list of colors generated
			nonmatchedSolidColors.push_back(randColor);

			//Check through all previously generated colors
			for (int iii = 0; iii < nonmatchedSolidColors.size(); iii++)
			{
				//If color appears in previously generated color, increase frequency count
				if (randColor == nonmatchedSolidColors[iii])
					nonmatchSolidColorsFrequency++;
			}

			//If frequency of color appearance is 2, exclude color from appearing again
			if (!nonmatchedColorsCanExceedTwo && nonmatchSolidColorsFrequency == 2)
				excludedColors.emplace(randColor);

			//Reset frequency count for new color
			nonmatchSolidColorsFrequency = 0;
		}

		else
		{
			//Add this color pair to the list of pairs generated
			nonmatchedColorPairs.push_back(randColorKey);

			//Check through all previously generated color pairs
			for (int iii = 0; iii < nonmatchedColorPairs.size(); iii++)
			{
				//If color appears in generated color pairs, increase frequency count
				if (randColorKey == nonmatchedColorPairs[iii])
					nonmatchColorPairsFrequency++;
			}

			//If frequency of color pair appearance is 2, exclude color pair from appearing again
			if (!nonmatchedColorsCanExceedTwo && nonmatchColorPairsFrequency == 2)
				excludedColorPairs.emplace(randColorKey);

			//Reset frequency count for new color pair
			nonmatchColorPairsFrequency = 0;
		}

		//Keep matching style count below 2
		if (!splitStyle)
		{
			//Add this style to the list of styles generated
			nonmatchedStyles.push_back(randOutline);

			//Check through all previously generated styles
			for (int iii = 0; iii < nonmatchedStyles.size(); iii++)
			{
				//If style appears in previously generated styles, increase frequency count
				if (randOutline == nonmatchedStyles[iii])
					nonmatchStyleFrequency++;
			}

			//If frequency of style appearance is 2, exclude style from appearing again
			if (!nonmatchedStylesCanExceedTwo && nonmatchStyleFrequency == 2)
				excludedStyles.emplace(randOutline);

			//Reset frequency count for new style
			nonmatchStyleFrequency = 0;
		}

		else
		{
			//Add this style pair to the list of pairs generated
			nonmatchedStylePairs.push_back(randOutlinePair);

			//Check through all previously generated style pairs
			for (int iii = 0; iii < nonmatchedStylePairs.size(); iii++)
			{
				//If style appears in previously generated style pairs, increase frequency count
				if (randOutlinePair == nonmatchedStylePairs[iii])
					nonmatchStylePairsFrequency++;
			}

			//If frequency of style pair appearance is 2, exclude style pair from appearing again
			if (!nonmatchedStylesCanExceedTwo && nonmatchStylePairsFrequency == 2)
				excludedStylePairs.emplace(randOutlinePair);

			//Reset frequency count for new style pairs
			nonmatchStylePairsFrequency = 0;
		}

		//Change tile
		tiles[nonmatchTile]->ChangeShape(randShape, randColorPair, rotation, randOutlinePair);
	}

	return;
}

void Game::ReRandomizeTiles()
{
	//Forced failed state if user already selected a tile
	if (currentSelection != 1)
	{
		currentSelection = 4;
		selection1 = -1;
		selection2 = -1;
		selection3 = -1;
		return;
	}

	//Remember how many matched tiles were showing
	int tempShowMatchesUsed = matchedTilesShowing;
	matchedTilesShowing = 0;

	//Remember how many tiles were eliminated (matching tiles are always excluded, so total excluded tiles minus matching tiles)
	int tempEliminatesUsed = eliminateUsedOnCurrentCard;
	eliminateUsedOnCurrentCard = 0;
	excludedEliminateTiles.clear();

	//If blind match, randomize match parameters again
	if (blindMatch)
	{
		getInstance()->SetMatchParameters(-1, true);
		UpdateHeaderText("???");
	}
	else
	{
		getInstance()->SetMatchParameters(-2);
		UpdateHeaderText(matchCriteria);
	}

	//Randomize and display all tiles again
	getInstance()->RandomizeTiles();
	RestoreTiles(0);

	//Lower bonuses used to balance out re-using bonuses below
	bonusesUsed -= tempEliminatesUsed;
	bonusesUsed -= tempShowMatchesUsed;

	//Re-use the same amount of bonuses as before re-randomize
	for (int iii = 0; iii < tempEliminatesUsed; iii++)
		UseBonusEliminate();
	for (int iii = 0; iii < tempShowMatchesUsed; iii++)
		UseShowMatch();

	warningTime = 3000;

    if(mode == CLASSIC)
    {
        countdownTimer.Restart();
        warningTime = 1000;
    }
    
	return;
}

bool Game::AllowRerandomize()
{
	return allowRerandomize;
}

std::pair<cocos2d::Color3B, cocos2d::Color3B> Game::GetColorFromKey(std::pair<int, int> colorInput)
{
	std::pair<cocos2d::Color3B, cocos2d::Color3B> color;
	switch (colorInput.first)
	{
	case RED:
		color.first = cocos2d::Color3B(255, 100, 100);
		break;
	case GREEN:
		color.first = cocos2d::Color3B(87, 225, 90);
		break;
	case BLUE:
		color.first = cocos2d::Color3B(39, 103, 255);
		break;
	case YELLOW:
		color.first = cocos2d::Color3B(250, 238, 46);
		break;
	case PURPLE:
		color.first = cocos2d::Color3B(211, 101, 207);
		break;
	case CYAN:
		color.first = cocos2d::Color3B(49, 224, 255);
		break;
	case ORANGE:
		color.first = cocos2d::Color3B(255, 180, 61);
		break;
	}

	switch (colorInput.second)
	{
	case RED:
		color.second = cocos2d::Color3B(255, 50, 50);
		break;
	case GREEN:
		color.second = cocos2d::Color3B(22, 205, 25);
		break;
	case BLUE:
		color.second = cocos2d::Color3B(0, 57, 236);
		break;
	case YELLOW:
		color.second = cocos2d::Color3B(234, 220, 2);
		break;
	case PURPLE:
		color.second = cocos2d::Color3B(194, 54, 189);
		break;
	case CYAN:
		color.second = cocos2d::Color3B(20, 203, 255);
		break;
	case ORANGE:
		color.second = cocos2d::Color3B(255, 152, 7);
		break;
	}
    
	return color;
}

std::pair<int, int> Game::DecodeColor(std::pair<cocos2d::Color3B, cocos2d::Color3B>)
{
	return std::pair<int, int>();
}

void Game::Reset()
{
	yesMenuButtonOn = false;
	notReallyMenuButtonOn = false;
	sureMenuButtonOn = false;
	okMenuButtonOn = false;
	noThanksMenuButtonOn = false;

	//Turn everything invisible
	headerTile->setOpacity(0);
	baseTile->setOpacity(0);
	baseTileShadow->setOpacity(0);
	scoreTile->setOpacity(0);
	scoreText->GetLabel()->setOpacity(0);
	pointsText->GetLabel()->setOpacity(0);
	matchCriteriaText->GetLabel()->setOpacity(0);
	timePlusText->GetLabel()->setOpacity(0);
	eliminateText->GetLabel()->setOpacity(0);
	showMatchText->GetLabel()->setOpacity(0);
	newCardText->GetLabel()->setOpacity(0);
	timerText->GetLabel()->setOpacity(0);
	bestMatchIcon->setOpacity(0);
	bestScoreIcon->setOpacity(0);
	timePlusTile->setOpacity(0);
	newCardTile->setOpacity(0);
	showMatchTile->setOpacity(0);
	eliminateTile->setOpacity(0);
	optionsButton->setOpacity(0);
	optionsText->GetLabel()->setOpacity(0);
	highScoresButton->setOpacity(0);
	highScoresText->GetLabel()->setOpacity(0);
	buyBonusItemsButton->setOpacity(0);
	buyBonusItemsText->GetLabel()->setOpacity(0);
	baseTileText->GetLabel()->setOpacity(0);
	leftMenuText->GetLabel()->setOpacity(0);
	rightMenuText->GetLabel()->setOpacity(0);
	tradeText->GetLabel()->setOpacity(0);
	menuLeft->setOpacity(0);
	menuRight->setOpacity(0);
	achievementUnlockedHeader->GetLabel()->setOpacity(0);

	bestMatchIcon->setVisible(false);
	bestScoreIcon->setVisible(false);
	optionsButton->setVisible(false);
	optionsText->GetLabel()->setVisible(false);
	highScoresButton->setVisible(false);
	highScoresText->GetLabel()->setVisible(false);
	buyBonusItemsButton->setVisible(false);
	buyBonusItemsText->GetLabel()->setVisible(false);
	leftMenuText->GetLabel()->setVisible(false);
	rightMenuText->GetLabel()->setVisible(false);
	tradeText->GetLabel()->setVisible(false);
	menuLeft->setVisible(false);
	menuRight->setVisible(false);
	achievementUnlockedHeader->GetLabel()->setVisible(false);

	int numberOfLabels = matchMadeList->GetNumberOfItems();

	for (int iii = 0; iii < numberOfLabels; iii++)
		matchMadeList->GetItem(iii)->setOpacity(0);
	matchMadeList->setVisible(true);

	//Reset all game related variables
	gameOver = false;
	gameWon = false;
	
	//Check purchased items
	purchasedEliminateBonuses = gameData->getIntegerForKey(purchased_eliminate_left, 0);
	purchasedTimePlusBonuses = gameData->getIntegerForKey(purchased_time_plus_left, 0);
	purchasedShowMatchBonuses = gameData->getIntegerForKey(purchased_show_match_left, 0);
	purchasedNewCardBonuses = gameData->getIntegerForKey(purchased_new_card_left, 0);

	if (testing && testingBonuses)
	{
		gameData->setIntegerForKey(purchased_eliminate_left, 50);
		gameData->setIntegerForKey(purchased_time_plus_left, 50);
		gameData->setIntegerForKey(purchased_show_match_left, 50);
		gameData->setIntegerForKey(purchased_new_card_left, 50);

		purchasedEliminateBonuses = 50;
		purchasedTimePlusBonuses = 50;
		purchasedShowMatchBonuses = 50;
		purchasedNewCardBonuses = 50;
	}

    if(mode == RUSH_RELAX || mode == CLASSIC)
        bonusTimePlusLeft = 0;
    else
        bonusTimePlusLeft = purchasedTimePlusBonuses;
	bonusEliminateLeft = purchasedEliminateBonuses;
	bonusShowMatchLeft = purchasedShowMatchBonuses;
	bonusNewCardLeft = purchasedNewCardBonuses;
	earnedTimePlusBonuses = 0;
	earnedEliminateBonuses = 0;
	earnedShowMatchBonuses = 0;
	earnedNewCardBonuses = 0;
	timePlusUsedOnCurrentCard = 0;
	eliminateUsedOnCurrentCard = 0;
	matchedTilesShowing = 0;

	getInstance()->ResetScore();
	finalScore = 0;
	displayedScore = 0;
	matchMilestone = 5;
	achievementsUnlocked = 0;

	menuLeft->ToggleTouch(false);
	menuRight->ToggleTouch(false);

	disablePowerups = false;
	powerupsAreOn = true;
	bonusesUsed = 0;
	lastMatchWasNearMiss = false;
	nearMiss = false;
	noBonusesUsedBefore50Matches = false;
	gameData->setIntegerForKey(consecutive_near_misses, 0);
	achievementsUnlocked = 0;
	sequence = 1;
	displaySequence = true;

	//Get rid of achievements text
	for (int iii = 0; iii < achievementsText.size(); iii++)
	{
		achievementsText[0]->removeFromParent();
		achievementsText.erase(achievementsText.begin());
	}
	achievementsText.clear();

	//Reset tiles
	for (int iii = 0; iii < tiles.size(); iii++)
	{
		tiles[iii]->stopAllActions();
		tiles[iii]->ToggleTouch(false);
		tiles[iii]->SetFade(0);
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

	//Disable touch
	eliminateTile->ToggleEnabled(false);
	timePlusTile->ToggleEnabled(false);
	showMatchTile->ToggleEnabled(false);
	newCardTile->ToggleEnabled(false);
	retry->ToggleOn(false);

	excludedColors.clear();
	excludedColorPairs.clear();
	excludedShapes.clear();
	excludedStyles.clear();
	excludedStylePairs.clear();
	excludedTiles.clear();
	nonmatchCircleFrequency = 0;
	nonmatchSquareFrequency = 0;
	nonmatchDiamondFrequency = 0;
	nonmatchTriangleFrequency = 0;
	nonmatchHexagonFrequency = 0;
	nonmatchOctagonFrequency = 0;
	nonmatchedSolidColors.clear();
	nonmatchedColorPairs.clear();
	nonmatchedStyles.clear();
	nonmatchedStylePairs.clear();
	nonmatchSolidColorsFrequency = 0;
	nonmatchColorPairsFrequency = 0;
	nonmatchStyleFrequency = 0;
	nonmatchStylePairsFrequency = 0;
	matchingShapes = false;
	matchingColors = false;
	matchingStyles = false;
	blindMatch = false;
	nonmatchedShapesCanExceedTwo = false;
	nonmatchedColorsCanExceedTwo = false;
	nonmatchedStylesCanExceedTwo = false;
	useDesignatedMatchColor = false;
	useDesignatedMatchShape = false;
	useDesignatedMatchStyle = false;
	useDesignatedNonmatchColor = false;
	useDesignatedNonmatchShape = false;
	useDesignatedNonmatchStyle = false;
	numberOfSameNonmatchingColors = 0;
	numberOfSameNonmatchingShapes = 0;
	numberOfSameNonmatchingStyles = 0;
	designatedNonmatchColorLimit = 6;
	designatedNonmatchStyleLimit = 6;
	designatedNonmatchShapeLimit = 6;
	currentSelection = 1;
	
	//Set bonus item quantity text and position
	sprintf(sz, "%d / %d", earnedTimePlusBonuses, purchasedTimePlusBonuses);
	timePlusText->setString(sz);
	timePlusText->setPositionX(timePlusTile->getPositionX() + timePlusTile->GetWidth() / 2 - timePlusText->GetScaledWidth() / 2);

	sprintf(sz, "%d / %d", earnedEliminateBonuses, purchasedEliminateBonuses);
	eliminateText->setString(sz);
	eliminateText->setPositionX(eliminateTile->getPositionX() + eliminateTile->GetWidth() / 2 - eliminateText->GetScaledWidth() / 2);

	sprintf(sz, "%d / %d", earnedShowMatchBonuses, purchasedShowMatchBonuses);
	showMatchText->setString(sz);
	showMatchText->setPositionX(showMatchTile->getPositionX() + showMatchTile->GetWidth() / 2 - showMatchText->GetScaledWidth() / 2);

	sprintf(sz, "%d / %d", earnedNewCardBonuses, purchasedNewCardBonuses);
	newCardText->setString(sz);
	newCardText->setPositionX(newCardTile->getPositionX() + newCardTile->GetWidth() / 2 - newCardText->GetScaledWidth() / 2);

	retry->ChangeMenu("", 0, false);
	retry->Center();
	retry->SetBottom(retryBottomPosition);

	switch (mode)
	{
	case CLASSIC:
		highestMatch = gameData->getIntegerForKey(best_match_classic, 0);
		highestScore = gameData->getIntegerForKey(best_score_classic, 0);

		sprintf(sz, "3.25");
		timerText->setString(sz);
		timerText->Center();
		timerText->SetBottom(timeRemainingBottomPosition);
		timerText->GetLabel()->setTextColor(cocos2d::Color4B::BLACK);

		break;
	case COUNTDOWN:
		highestMatch = gameData->getIntegerForKey(best_match_countdown, 0);
		highestScore = gameData->getIntegerForKey(best_score_countdown, 0);

		timerText->setString("30.00");
		timerText->Center();
		timerText->SetBottom(timeRemainingBottomPosition);
		timerText->GetLabel()->setTextColor(cocos2d::Color4B::BLACK);

		break;
	case MEMORY:
		highestMatch = gameData->getIntegerForKey(best_match_memory, 0);
		highestScore = gameData->getIntegerForKey(best_score_memory, 0);

		sprintf(sz, "%d.00", startingMs / 1000);
		timerText->setString(sz);
		timerText->Center();
		timerText->SetBottom(timeRemainingBottomPosition);

		break;
	case RUSH_RELAX:
		highestMatch = gameData->getIntegerForKey(best_match_rushrelax, 0);
		highestScore = gameData->getIntegerForKey(best_score_rushrelax, 0);

		timerText->setString("");
		timerText->Center();
		timerText->SetBottom(timeRemainingBottomPosition);
		timerText->GetLabel()->setTextColor(cocos2d::Color4B::BLACK);

		break;

	}

	if (mode != MEMORY)
	{
		if (mode != RUSH_RELAX && mode != CLASSIC)
		{
			if (bonusTimePlusLeft > 0)
			{
				timePlusTile->ToggleEnabled(true);
				timePlusTile->setOpacity(255);
				timePlusText->GetLabel()->setOpacity(255);
			}
		}

		if (bonusEliminateLeft > 0)
		{
			eliminateTile->ToggleEnabled(true);
			eliminateTile->setOpacity(255);
			eliminateText->GetLabel()->setOpacity(255);
		}

		if (bonusShowMatchLeft > 0)
		{
			showMatchTile->ToggleEnabled(true);
			showMatchTile->setOpacity(255);
			showMatchText->GetLabel()->setOpacity(255);
		}

		if (bonusNewCardLeft > 0)
		{
			newCardTile->ToggleEnabled(true);
			newCardTile->setOpacity(255);
			newCardText->GetLabel()->setOpacity(255);
		}
	}

	else if (mode == MEMORY)
	{
		if (bonusTimePlusLeft > 0)
		{
			timePlusTile->setVisible(true);
			timePlusText->GetLabel()->setOpacity(64);
		}

		if (bonusEliminateLeft > 0)
		{
			eliminateTile->setVisible(true);
			eliminateText->GetLabel()->setOpacity(64);
		}

		if (bonusShowMatchLeft > 0)
		{
			showMatchTile->setVisible(true);
			showMatchText->GetLabel()->setOpacity(64);
		}

		if (bonusNewCardLeft > 0)
		{
			newCardTile->setVisible(true);
			newCardText->GetLabel()->setOpacity(64);
		}
	}

	sceneTimer.Reset();
	countdownTimer.Reset();
	matchTimer.Reset();

	return;
}

void Game::SetMatchParameters(int excludedMatchCriteria, bool blind)
{
	//Default parameters
	numberOfMatchingTiles = 3;
	excludedColors.clear();
	excludedColorPairs.clear();
	excludedShapes.clear();
	excludedStyles.clear();
	excludedStylePairs.clear();
	excludedTiles.clear();
	nonmatchCircleFrequency = 0;
	nonmatchSquareFrequency = 0;
	nonmatchDiamondFrequency = 0;
	nonmatchTriangleFrequency = 0;
	nonmatchHexagonFrequency = 0;
	nonmatchOctagonFrequency = 0;
	nonmatchedSolidColors.clear();
	nonmatchedColorPairs.clear();
	nonmatchedStyles.clear();
	nonmatchedStylePairs.clear();
	nonmatchSolidColorsFrequency = 0;
	nonmatchColorPairsFrequency = 0;
	nonmatchStyleFrequency = 0;
	nonmatchStylePairsFrequency = 0;
	useDesignatedMatchColor = false;
	useDesignatedMatchShape = false;
	useDesignatedMatchStyle = false;
	useDesignatedNonmatchColor = false;
	useDesignatedNonmatchShape = false;
	useDesignatedNonmatchStyle = false;
	excludedShapes.emplace(HEXAGON);
	excludedShapes.emplace(OCTAGON);
	excludedStyles.emplace(BLINDS);
	excludedStyles.emplace(BLINDS_QUAD);
	excludedStyles.emplace(OUTLINE_BLINDS);
	excludedStyles.emplace(OUTLINE_QUAD);
	excludedStyles.emplace(OUTLINE_BLINDS_QUAD);
	excludedStyles.emplace(CONCENTRIC_BLINDS);
	excludedStyles.emplace(CONCENTRIC_QUAD);
	excludedStyles.emplace(CONCENTRIC_BLINDS_QUAD);
	splitColor = false;
	splitStyle = false;
	blindMatch = blind;
	matchingColors = false;
	matchingShapes = false;
	matchingStyles = false;
	nonmatchedColorsCanExceedTwo = false;
	nonmatchedShapesCanExceedTwo = false;
	nonmatchedStylesCanExceedTwo = false;
	numberOfSameNonmatchingColors = 0;
	numberOfSameNonmatchingShapes = 0;
	numberOfSameNonmatchingStyles = 0;
	designatedNonmatchColorLimit = 6;
	designatedNonmatchStyleLimit = 6;
	designatedNonmatchShapeLimit = 6;

	return;
}

int Game::GetMatchCriteria()
{
	return matchCriteria;
}

bool Game::IsGameOver()
{
	return gameOver;
}

bool Game::IsGameStarted()
{
	return gameStarted;
}

int Game::GetMode()
{
	return mode;
}

void Game::SetForcedGameOver(bool isGameOver)
{
	forcedGameOver = true;
	return;
}

void Game::RequestReview()
{
	if (!gameData->getBoolForKey(app_rated, false))
		requestReview = true;
	else
		requestReview = false;
}

void Game::CheckForReview()
{
    //Display review request instead of final match
    if (displayReviewRequest)
    {
        for (int iii = 0; iii < tiles.size(); iii++)
        {
            tiles[iii]->SetFade(0);
            tiles[iii]->DismissTile();
        }
        
        menuLeft->ToggleTouch(true);
        menuRight->ToggleTouch(true);
        yesMenuButtonOn = true;
        notReallyMenuButtonOn = true;
        sureMenuButtonOn = false;
        okMenuButtonOn = false;
        noThanksMenuButtonOn = false;
        
        baseTileText->setString("Enjoying Tap 3?");
        baseTileText->Center();
        baseTileText->setPositionY(baseTile->getPositionY() + baseTile->GetHeight() * 0.35);
        
        //Left side boxes
        menuLeft->Center();
        menuLeft->setPositionX(baseTile->getPositionX() - baseTile->GetWidth() / 2 + baseTile->GetWidth() * 0.25 - menuLeft->GetWidth() / 2);
        
        //Righ side boxes
        menuRight->Center();
        menuRight->SetRightSide(baseTile->getPositionX() + baseTile->GetWidth() / 2 - baseTile->GetWidth() * 0.25 + menuRight->GetWidth() / 2);
        
        rightMenuText->setString("Yes!");
        leftMenuText->setString("Not really.");
        
        nrgFunctions::Center(leftMenuText, menuLeft, -1, 1);
        nrgFunctions::Center(rightMenuText, menuRight, -1, 1);
        menuRight->setPositionY(menuLeft->getPositionY());
        
        baseTileText->GetLabel()->setVisible(true);
        menuLeft->setVisible(true);
        menuRight->setVisible(true);
        leftMenuText->GetLabel()->setVisible(true);
        rightMenuText->GetLabel()->setVisible(true);
        
        baseTileText->GetLabel()->runAction(cocos2d::FadeIn::create(0.5));
        menuLeft->runAction(cocos2d::FadeIn::create(0.5));
        menuRight->runAction(cocos2d::FadeIn::create(0.5));
        leftMenuText->GetLabel()->runAction(cocos2d::FadeIn::create(0.5));
        rightMenuText->GetLabel()->runAction(cocos2d::FadeIn::create(0.5));
        
        //Reset threshold for asking for review
        requestReview = false;
        gameData->setIntegerForKey(times_opened_today, 0);
        gameData->setIntegerForKey(times_opened_yesterday, 0);
        gameData->setIntegerForKey(times_opened_two_days_ago, 0);
        gameData->setIntegerForKey(times_played_today, 0);
        
        displayReviewRequest = false;
        testingReview = false;
		allowRerandomize = false;
    }
    
	//Enjoying Tap 3
    if (menuRight->WasPressed(true) && yesMenuButtonOn)
    {
        audio->PlayClip("tone1");
		menuRight->WasPressed();
        yesMenuButtonOn = false;
        notReallyMenuButtonOn = false;
        sureMenuButtonOn = true;
        okMenuButtonOn = false;
        noThanksMenuButtonOn = true;
        
        baseTileText->GetLabel()->runAction(cocos2d::Sequence::create(
                                                                      cocos2d::FadeOut::create(0.25),
                                                                      cocos2d::CallFunc::create([this]() {
            baseTileText->setString("Would you please rate Tap 3?");
            baseTileText->Center();
            baseTileText->setPositionY(baseTile->getPositionY() + baseTile->GetHeight() * 0.35);}),
                                                                      cocos2d::FadeIn::create(0.25), NULL));
        
        rightMenuText->GetLabel()->runAction(cocos2d::Sequence::create(
                                                                      cocos2d::FadeOut::create(0.25),
                                                                      cocos2d::CallFunc::create([this]() {rightMenuText->setString("Sure!");}),
                                                                      cocos2d::DelayTime::create(0.25),
                                                                      cocos2d::FadeIn::create(0.25), NULL));
        
        leftMenuText->GetLabel()->runAction(cocos2d::Sequence::create(
                                                                       cocos2d::FadeOut::create(0.25),
                                                                       cocos2d::CallFunc::create([this]() {leftMenuText->setString("No, thanks.");}),
                                                                       cocos2d::DelayTime::create(0.25),
                                                                       cocos2d::FadeIn::create(0.25), NULL));
    }
    
	//Not Enjoying Tap 3
    if (menuLeft->WasPressed(true) && notReallyMenuButtonOn)
    {
        audio->PlayClip("tone1");
		menuLeft->WasPressed();
        yesMenuButtonOn = false;
        notReallyMenuButtonOn = false;
        sureMenuButtonOn = false;
        okMenuButtonOn = true;
        noThanksMenuButtonOn = true;
        
        baseTileText->GetLabel()->runAction(cocos2d::Sequence::create(
                                                                      cocos2d::FadeOut::create(0.25),
                                                                      cocos2d::CallFunc::create([this]() {
            baseTileText->setString("Could you share your feedback on how we can improve Tap 3?");
            baseTileText->Center();
            baseTileText->setPositionY(baseTile->getPositionY() + baseTile->GetHeight() * 0.45);}),
                                                                      cocos2d::FadeIn::create(0.25), NULL));
        
        rightMenuText->GetLabel()->runAction(cocos2d::Sequence::create(
                                                                      cocos2d::FadeOut::create(0.25),
                                                                      cocos2d::CallFunc::create([this]() {rightMenuText->setString("Ok.");}),
                                                                      cocos2d::DelayTime::create(0.25),
                                                                      cocos2d::FadeIn::create(0.25), NULL));
        
        leftMenuText->GetLabel()->runAction(cocos2d::Sequence::create(
                                                                       cocos2d::FadeOut::create(0.25),
                                                                       cocos2d::CallFunc::create([this]() {leftMenuText->setString("No, thanks.");}),
                                                                       cocos2d::DelayTime::create(0.25),
                                                                       cocos2d::FadeIn::create(0.25), NULL));
        
        menuLeft->runAction(cocos2d::Sequence::create(
                                                      cocos2d::FadeOut::create(0.25),
                                                      cocos2d::CallFunc::create([this](){nrgFunctions::Center(leftMenuText, menuLeft, -1, 1);}),
                                                      cocos2d::FadeIn::create(0.25), NULL));
        
        menuRight->runAction(cocos2d::Sequence::create(
                                                       cocos2d::FadeOut::create(0.25),
                                                       cocos2d::CallFunc::create([this]() {nrgFunctions::Center(rightMenuText, menuRight, -1, 1); menuRight->setPositionY(menuLeft->getPositionY());}),
                                                       cocos2d::FadeIn::create(0.25), NULL));
    }
    
	//Does not want to provide feedback or rate
    if (menuLeft->WasPressed(true) && noThanksMenuButtonOn)
    {
        audio->PlayClip("double_tone_low");
		menuLeft->WasPressed();
        yesMenuButtonOn = false;
        notReallyMenuButtonOn = false;
        sureMenuButtonOn = false;
        okMenuButtonOn = false;
        noThanksMenuButtonOn = false;
        
        baseTileText->GetLabel()->stopAllActions();
        menuLeft->stopAllActions();
        menuRight->stopAllActions();
        leftMenuText->GetLabel()->stopAllActions();
        rightMenuText->GetLabel()->stopAllActions();
        
        baseTileText->GetLabel()->runAction(cocos2d::FadeOut::create(0.25));
        menuLeft->runAction(cocos2d::Sequence::create(cocos2d::FadeOut::create(0.25), cocos2d::CallFunc::create([this]() {menuLeft->setVisible(false);}), NULL));
        menuRight->runAction(cocos2d::Sequence::create(cocos2d::FadeOut::create(0.25), cocos2d::CallFunc::create([this]() {menuRight->setVisible(false);}), NULL));
        leftMenuText->GetLabel()->runAction(cocos2d::Sequence::create(cocos2d::FadeOut::create(0.25), cocos2d::CallFunc::create([this]() {leftMenuText->GetLabel()->setVisible(false);}), NULL));
        rightMenuText->GetLabel()->runAction(cocos2d::Sequence::create(cocos2d::FadeOut::create(0.25), cocos2d::CallFunc::create([this]() {rightMenuText->GetLabel()->setVisible(false);}), NULL));
        menuLeft->ToggleTouch(false);
        menuRight->ToggleTouch(false);
        restoreTiles = true;
		allowRerandomize = true;
		sceneTimer.SetMark(0);
    }
    
	//Will rate Tap 3
    if (menuRight->WasPressed(true) && sureMenuButtonOn)
    {
        audio->PlayClip("double_tone_high");
		menuRight->WasPressed();
        yesMenuButtonOn = false;
        notReallyMenuButtonOn = false;
        sureMenuButtonOn = false;
        okMenuButtonOn = false;
        noThanksMenuButtonOn = false;
        
        baseTileText->GetLabel()->stopAllActions();
        menuLeft->stopAllActions();
        menuRight->stopAllActions();
        leftMenuText->GetLabel()->stopAllActions();
        rightMenuText->GetLabel()->stopAllActions();
        
        baseTileText->GetLabel()->runAction(cocos2d::FadeOut::create(0.25));
        menuLeft->runAction(cocos2d::Sequence::create(cocos2d::FadeOut::create(0.25), cocos2d::CallFunc::create([this]() {menuLeft->setVisible(false);}), NULL));
        menuRight->runAction(cocos2d::Sequence::create(cocos2d::FadeOut::create(0.25), cocos2d::CallFunc::create([this]() {menuRight->setVisible(false);}), NULL));
        leftMenuText->GetLabel()->runAction(cocos2d::Sequence::create(cocos2d::FadeOut::create(0.25), cocos2d::CallFunc::create([this]() {leftMenuText->GetLabel()->setVisible(false);}), NULL));
        rightMenuText->GetLabel()->runAction(cocos2d::Sequence::create(cocos2d::FadeOut::create(0.25), cocos2d::CallFunc::create([this]() {rightMenuText->GetLabel()->setVisible(false);}), NULL));
        menuLeft->ToggleTouch(false);
        menuRight->ToggleTouch(false);
        restoreTiles = true;
		allowRerandomize = true;
		sceneTimer.SetMark(0);

        //GO TO REVIEW APP URL
		if (cocos2d::Application::getInstance()->openURL("http://ooo-games.com"))
			gameData->setBoolForKey(app_rated, true);
    }
    
	//Will provide feedback for Tap 3
    if (menuRight->WasPressed(true) && okMenuButtonOn)
    {
        audio->PlayClip("double_tone_high");
		menuRight->WasPressed();
        yesMenuButtonOn = false;
        notReallyMenuButtonOn = false;
        sureMenuButtonOn = false;
        okMenuButtonOn = false;
        noThanksMenuButtonOn = false;
        
        baseTileText->GetLabel()->stopAllActions();
        menuLeft->stopAllActions();
        menuRight->stopAllActions();
        leftMenuText->GetLabel()->stopAllActions();
        rightMenuText->GetLabel()->stopAllActions();
        
        baseTileText->GetLabel()->runAction(cocos2d::Sequence::create(cocos2d::FadeOut::create(0.25), cocos2d::CallFunc::create([this]() {baseTileText->GetLabel()->setVisible(false);}), NULL));
        menuLeft->runAction(cocos2d::Sequence::create(cocos2d::FadeOut::create(0.25), cocos2d::CallFunc::create([this]() {menuLeft->setVisible(false);}), NULL));
        menuRight->runAction(cocos2d::Sequence::create(cocos2d::FadeOut::create(0.25), cocos2d::CallFunc::create([this]() {menuRight->setVisible(false);}), NULL));
        leftMenuText->GetLabel()->runAction(cocos2d::Sequence::create(cocos2d::FadeOut::create(0.25), cocos2d::CallFunc::create([this]() {leftMenuText->GetLabel()->setVisible(false);}), NULL));
        rightMenuText->GetLabel()->runAction(cocos2d::Sequence::create(cocos2d::FadeOut::create(0.25), cocos2d::CallFunc::create([this]() {rightMenuText->GetLabel()->setVisible(false);}), NULL));
        menuLeft->ToggleTouch(false);
        menuRight->ToggleTouch(false);
        restoreTiles = true;
		allowRerandomize = true;
		sceneTimer.SetMark(0);

        //EMAIL SUPPORT WITH FEEDBACK
		cocos2d::Application::getInstance()->openURL("mailto:support@ooo-games.com?subject=Tap 3 Feedback&body=Hi. I want to provide the following feedback for Tap 3:");
    }

	return;
}

Game * Game::getInstance()
{
    return instance;
}

void Game::RestartTimers()
{
	return;
}

void Game::CheckPlayFrequency()
{
    //Count how many times app has been opened today
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
	time_t timeObject = std::chrono::system_clock::to_time_t(now);
	tm* currentTime = localtime(&timeObject);
	
    if(!gameData)
        gameData = cocos2d::UserDefault::getInstance();
    
	int today = currentTime->tm_yday;
	int year = currentTime->tm_year;
	int timesOpened = 0;
	int lastDayPlayed = gameData->getIntegerForKey(last_day_opened, today);
	int lastYearPlayed = gameData->getIntegerForKey(last_year_opened, year);

	//Last played today
	if (lastDayPlayed - today == 0 && lastYearPlayed - year == 0)
		SetTimesOpenedToday(GetTimesOpenedToday() + 1);

	//Last played yesterday
	else if (lastDayPlayed - today == -1 && lastYearPlayed - year == 0)
	{
		SetTimesOpenedTwoDaysAgo(GetTimesOpenedYesterday());
		SetTimesOpenedYesterday(GetTimesOpenedToday());
		SetTimesOpenedToday(1);
		SetLastDayPlayed(today);
	}

	//Last played two days ago
	else if (lastDayPlayed - today == -2 && lastYearPlayed - year == 0)
	{
		SetTimesOpenedTwoDaysAgo(GetTimesPlayedToday());
		SetTimesOpenedYesterday(0);
		SetTimesOpenedToday(1);
		SetLastDayPlayed(today);
	}

	//Last played over two days ago
	else if (lastDayPlayed - today < -2 && lastYearPlayed - year == 0)
	{
		SetTimesOpenedTwoDaysAgo(0);
		SetTimesOpenedYesterday(0);
		SetTimesOpenedToday(1);
		SetLastDayPlayed(today);
	}

	//Last played December 31, currently January 1st (or December 30th and January 1st for leap year)
	else if (currentTime->tm_mon == 0 && currentTime->tm_yday == 0 && (lastDayPlayed - today == 364 || lastDayPlayed - today == 365))
	{
		SetTimesOpenedTwoDaysAgo(GetTimesOpenedYesterday());
		SetTimesOpenedYesterday(GetTimesOpenedToday());
		SetTimesOpenedToday(1);
		SetLastDayPlayed(today);
	}

	//Last played December 31, currently January 2nd (or December 30th and January 2nd for leap year)
	else if (currentTime->tm_mon == 0 && currentTime->tm_yday == 1 && (lastDayPlayed - today == 363))
	{
		SetTimesOpenedTwoDaysAgo(GetTimesOpenedToday());
		SetTimesOpenedYesterday(0);
		SetTimesOpenedToday(1);
		SetLastDayPlayed(today);
	}
	//Last played December 30, currently January 1st (or December 29th and January 1st for leap year)
	else if (currentTime->tm_mon == 0 && currentTime->tm_yday == 0 && (lastDayPlayed - today == 363))
	{
		SetTimesOpenedTwoDaysAgo(GetTimesOpenedToday());
		SetTimesOpenedYesterday(0);
		SetTimesOpenedToday(1);
		SetLastDayPlayed(today);
	}

	//Last played last year, over two days ago
	else if (lastYearPlayed - year > 0)
	{
		SetTimesOpenedTwoDaysAgo(0);
		SetTimesOpenedYesterday(0);
		SetTimesOpenedToday(1);
		SetLastDayPlayed(today);
	}

	timesOpened = GetTimesOpenedToday() + GetTimesOpenedYesterday() + GetTimesOpenedTwoDaysAgo();

	if (timesOpened >= 10)
	{
		RequestReview();
		gameData->setIntegerForKey(matches_made_today_for_review, 0);
		gameData->setIntegerForKey(times_opened_today, 0);
		gameData->setIntegerForKey(times_played_today, 0);
	}
}

void Game::SetLastDayPlayed(int value)
{
    if(!gameData)
        gameData = cocos2d::UserDefault::getInstance();
    
	gameData->setIntegerForKey(last_day_opened, value);
	return;
}

int Game::GetLastDayPlayed()
{
	std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
	time_t timeObject = std::chrono::system_clock::to_time_t(now);
	tm* currentTime = localtime(&timeObject);

    if(!gameData)
        gameData = cocos2d::UserDefault::getInstance();
    
	int today = currentTime->tm_yday;

	return gameData->getIntegerForKey(last_day_opened, today);
}

void Game::SetTimesPlayedToday(int value)
{
    if(!gameData)
        gameData = cocos2d::UserDefault::getInstance();
    
	gameData->setIntegerForKey(times_played_today, value);
	return;
}

int Game::GetTimesPlayedToday()
{
    if(!gameData)
        gameData = cocos2d::UserDefault::getInstance();
    
	return gameData->getIntegerForKey(times_played_today, 0);
}

void Game::SetTimesOpenedToday(int value)
{
    if(!gameData)
        gameData = cocos2d::UserDefault::getInstance();
    
	gameData->setIntegerForKey(times_opened_today, value);
	return;
}

int Game::GetTimesOpenedToday()
{
    if(!gameData)
        gameData = cocos2d::UserDefault::getInstance();
    
	return gameData->getIntegerForKey(times_opened_today, 0);
}

void Game::SetTimesOpenedYesterday(int value)
{
    if(!gameData)
        gameData = cocos2d::UserDefault::getInstance();
    
	gameData->setIntegerForKey(times_opened_yesterday, value);
	return;
}

int Game::GetTimesOpenedYesterday()
{
    if(!gameData)
        gameData = cocos2d::UserDefault::getInstance();
    
	return gameData->getIntegerForKey(times_opened_yesterday, 0);
}

void Game::SetTimesOpenedTwoDaysAgo(int value)
{
    if(!gameData)
        gameData = cocos2d::UserDefault::getInstance();
    
	gameData->setIntegerForKey(times_opened_two_days_ago, value);
	return;
}

int Game::GetTimesOpenedTwoDaysAgo()
{
    if(!gameData)
        gameData = cocos2d::UserDefault::getInstance();
    
	return gameData->getIntegerForKey(times_opened_two_days_ago, 0);
}

void Game::SetLastYearOpened(int value)
{
    if(!gameData)
        gameData = cocos2d::UserDefault::getInstance();
    
	gameData->setIntegerForKey(last_year_opened, value);
	return;
}

int Game::GetLastYearOpened()
{
	std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
	time_t timeObject = std::chrono::system_clock::to_time_t(now);
	tm* currentTime = localtime(&timeObject);
	int year = currentTime->tm_year;

    if(!gameData)
        gameData = cocos2d::UserDefault::getInstance();
    
	return gameData->getIntegerForKey(last_year_opened, year);
}

void Game::onEnter()
{
    if (gameData->getBoolForKey(music_on, true) && !CocosDenshion::SimpleAudioEngine::getInstance()->isBackgroundMusicPlaying())
        audio->PlayMusic("Tap3-Theme", true);
    
	if (gameOver && mode != -1)
	{
		retry->ToggleTouch(true);
		optionsButton->ToggleTouch(true);
		highScoresButton->ToggleTouch(true);
		buyBonusItemsButton->ToggleTouch(true);	
	}

	if (yesMenuButtonOn || notReallyMenuButtonOn || okMenuButtonOn || sureMenuButtonOn || noThanksMenuButtonOn)
	{
		menuLeft->ToggleTouch(true);
		menuRight->ToggleTouch(true);
	}

	if(backButton)
		backButton->ToggleEnabled(true);
	cocos2d::Layer::onEnter();

	return;
}

void Game::onExit()
{
	if(backButton)
		backButton->ToggleEnabled(false);
	retry->ToggleTouch(false);
	optionsButton->ToggleTouch(false);
	highScoresButton->ToggleTouch(false);
	buyBonusItemsButton->ToggleTouch(false);

	menuLeft->ToggleTouch(false);
	menuRight->ToggleTouch(false);

	cocos2d::Layer::onExit();
	return;
}

Game::~Game()
{
}
