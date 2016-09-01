#include "pch.h"
#include "Tutorial_Bonus.h"


Tutorial_Bonus::Tutorial_Bonus()
{
}


Tutorial_Bonus::~Tutorial_Bonus()
{
}

cocos2d::Scene * Tutorial_Bonus::CreateScene()
{
	cocos2d::Scene* scene = cocos2d::Scene::create();

	cocos2d::Layer* layer = Tutorial_Bonus::create();

	scene->addChild(layer);

	return scene;
}

bool Tutorial_Bonus::init()
{

	instance = this;
	
    if(!gameData)
        gameData = cocos2d::UserDefault::getInstance();
    
	gameData->setBoolForKey(tutorial_complete_using_bonus_items, true);

	//Initialize origin
	SCREEN_WIDTH = glview->getDesignResolutionSize().width;
	SCREEN_HEIGHT = glview->getDesignResolutionSize().height;
	center.x = SCREEN_WIDTH / 2;
	center.y = SCREEN_HEIGHT / 2;

	if (!gameData->getBoolForKey(sound_on))
		audio->MuteSound();

	InitGameAssets();

	//Need to make sure assets are in the right place regardles of game mode
	int bonusOffset = 15;
	newCardStartingX = center.x + bonusOffset / 2;
	showMatchStartingX = center.x - bonusOffset / 2;

	timePlusStartingY = scoreTile->getPositionY() + scoreTile->GetHeight() + 7;
	newCardStartingY = timePlusStartingY;
	eliminateStartingY = timePlusStartingY;
	showMatchStartingY = timePlusStartingY;

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

	//Adjust header tile height for 5:3 and 16:10 aspect ratios
	if (nrgFunctions::GetRatio() < 1.7 && nrgFunctions::GetRatio() >= 1.59)
	{
		headerTile->Move(0, 20);
		nrgFunctions::Center(matchCriteriaText, headerTile);
	}

	headerTile->setVisible(false);
	matchCriteriaText->GetLabel()->setVisible(false);

	generateTimePlusThumbnail = nrgSprite::create(batch, "generate_time_tile.png", false);
	generateEliminateThumbnail = nrgSprite::create(batch, "generate_eliminate_tile.png", false);
	generateShowMatchThumbnail = nrgSprite::create(batch, "generate_show_match_tile.png", false);
	generateNewCardThumbnail = nrgSprite::create(batch, "generate_new_card_tile.png", false);

	generateTimePlusThumbnail->setOpacity(0);
	generateEliminateThumbnail->setOpacity(0);
	generateShowMatchThumbnail->setOpacity(0);
	generateNewCardThumbnail->setOpacity(0);

	nrgFunctions::ResizeForDevice(generateTimePlusThumbnail);
	nrgFunctions::ResizeForDevice(generateEliminateThumbnail);
	nrgFunctions::ResizeForDevice(generateShowMatchThumbnail);
	nrgFunctions::ResizeForDevice(generateNewCardThumbnail);

	generateTimePlusThumbnail->setAnchorPoint(cocos2d::Vec2(0.5, 0.5));
	generateEliminateThumbnail->setAnchorPoint(cocos2d::Vec2(0.5, 0.5));
	generateShowMatchThumbnail->setAnchorPoint(cocos2d::Vec2(0.5, 0.5));
	generateNewCardThumbnail->setAnchorPoint(cocos2d::Vec2(0.5, 0.5));

	Reset();

    
    timerText->ChangeTextSize(15);
    timerText->ChangeColor(cocos2d::Color4B::GRAY);
	timerText->setString("");
	timerText->Center();
	timerText->setPositionY(timeRemainingBottomPosition + textOffsetY);
    
	sceneTimer.Restart();
	scheduleUpdate();

	return true;
}

void Tutorial_Bonus::update(float dt)
{
	if (retry->GetConfirmedSelection() == 0)
	{
		if (step1)
		{
			step1 = false;
			step2 = true;
			timerText->GetLabel()->runAction(cocos2d::Sequence::create(
				cocos2d::FadeOut::create(0.5),
				cocos2d::CallFunc::create([this]() {
				timerText->setString("When offered a Bonus Item, you can either keep it for use later, or exchange it for points...");
				timerText->Center();
				timerText->setPositionY(timeRemainingBottomPosition + textOffsetY);}),
				cocos2d::FadeIn::create(0.5), NULL));

			audio->PlayClip("tone1");
		}

		else if (step2)
		{
			step2 = false;
			step3 = true;
			timerText->GetLabel()->runAction(cocos2d::Sequence::create(
				cocos2d::FadeOut::create(0.5),
				cocos2d::CallFunc::create([this]() {
				timerText->setString("There are 4 types of Bonus Items. Each one provides a different advantage...");
				timerText->Center();
				timerText->setPositionY(timeRemainingBottomPosition + textOffsetY);}),
				cocos2d::FadeIn::create(0.5), NULL));

			audio->PlayClip("tone2");
		}

		else if (step3)
		{
			step3 = false;
			step4 = true;

			Game::UpdateHeaderText(matchCriteria);
			timerText->GetLabel()->runAction(cocos2d::Sequence::create(
				cocos2d::FadeOut::create(0.5),
				cocos2d::CallFunc::create([this]() {
				timerText->setString("\"Time Plus\" adds time to the clock in Countdown and Memory mode.");
				timerText->Center();
				timerText->setPositionY(timeRemainingBottomPosition + textOffsetY);}),
				cocos2d::FadeIn::create(0.5), NULL));

			generateTimePlusThumbnail->stopAllActions();
			generateEliminateThumbnail->stopAllActions();
			generateShowMatchThumbnail->stopAllActions();
			generateNewCardThumbnail->stopAllActions();

			generateTimePlusThumbnail->setOpacity(255);
			generateEliminateThumbnail->setOpacity(255);
			generateShowMatchThumbnail->setOpacity(255);
			generateNewCardThumbnail->setOpacity(255);

			generateTimePlusThumbnail->runAction(cocos2d::Spawn::create(
				cocos2d::FadeTo::create(1, 255),
				cocos2d::ScaleTo::create(1, originalScale * 1.5),
				NULL));

			generateEliminateThumbnail->runAction(cocos2d::Spawn::create(
				cocos2d::FadeTo::create(1, 128),
				cocos2d::ScaleTo::create(1, originalScale),
				NULL));

			generateShowMatchThumbnail->runAction(cocos2d::Spawn::create(
				cocos2d::FadeTo::create(1, 128),
				cocos2d::ScaleTo::create(1, originalScale),
				NULL));

			generateNewCardThumbnail->runAction(cocos2d::Spawn::create(
				cocos2d::FadeTo::create(1, 128),
				cocos2d::ScaleTo::create(1, originalScale),
				NULL));

			//FADE IN ICONS
			timePlusTile->runAction(cocos2d::FadeIn::create(1));

			audio->PlayClip("tone1");
		}

		else if (step4)
		{
			step4 = false;
			step5 = true;

			timerText->GetLabel()->runAction(cocos2d::Sequence::create(
				cocos2d::FadeOut::create(0.5),
				cocos2d::CallFunc::create([this]() {
				timerText->setString("\"Eliminate Tiles\" removes 2x non-matching tiles from the set.");
				timerText->Center();
				timerText->setPositionY(timeRemainingBottomPosition + textOffsetY);}),
				cocos2d::FadeIn::create(0.5), NULL));

			generateTimePlusThumbnail->stopAllActions();
			generateEliminateThumbnail->stopAllActions();
			generateShowMatchThumbnail->stopAllActions();
			generateNewCardThumbnail->stopAllActions();

			generateTimePlusThumbnail->runAction(cocos2d::Spawn::create(
				cocos2d::FadeTo::create(1, 128),
				cocos2d::ScaleTo::create(1, originalScale),
				NULL));

			generateEliminateThumbnail->runAction(cocos2d::Spawn::create(
				cocos2d::FadeTo::create(1, 255),
				cocos2d::ScaleTo::create(1, originalScale * 1.5),
				NULL));

			generateShowMatchThumbnail->runAction(cocos2d::Spawn::create(
				cocos2d::FadeTo::create(1, 128),
				cocos2d::ScaleTo::create(1, originalScale),
				NULL));

			generateNewCardThumbnail->runAction(cocos2d::Spawn::create(
				cocos2d::FadeTo::create(1, 128),
				cocos2d::ScaleTo::create(1, originalScale),
				NULL));

			//FADE IN ICONS
			eliminateTile->runAction(cocos2d::FadeIn::create(1));

			audio->PlayClip("tone2");
		}

		else if (step5)
		{
			step5 = false;
			step6 = true;
			
			timerText->GetLabel()->runAction(cocos2d::Sequence::create(
				cocos2d::FadeOut::create(0.5),
				cocos2d::CallFunc::create([this]() {
				timerText->setString("\"Show Match\" reveals one of the matching tiles.");
				timerText->Center();
				timerText->setPositionY(timeRemainingBottomPosition + textOffsetY);}),
				cocos2d::FadeIn::create(0.5), NULL));

			generateTimePlusThumbnail->stopAllActions();
			generateEliminateThumbnail->stopAllActions();
			generateShowMatchThumbnail->stopAllActions();
			generateNewCardThumbnail->stopAllActions();

			generateTimePlusThumbnail->runAction(cocos2d::Spawn::create(
				cocos2d::FadeTo::create(1, 128),
				cocos2d::ScaleTo::create(1, originalScale),
				NULL));

			generateEliminateThumbnail->runAction(cocos2d::Spawn::create(
				cocos2d::FadeTo::create(1, 128),
				cocos2d::ScaleTo::create(1, originalScale),
				NULL));

			generateShowMatchThumbnail->runAction(cocos2d::Spawn::create(
				cocos2d::FadeTo::create(1, 255),
				cocos2d::ScaleTo::create(1, originalScale * 1.5),
				NULL));

			generateNewCardThumbnail->runAction(cocos2d::Spawn::create(
				cocos2d::FadeTo::create(1, 128),
				cocos2d::ScaleTo::create(1, originalScale),
				NULL));

			//FADE IN ICONS
			showMatchTile->runAction(cocos2d::FadeIn::create(1));

			audio->PlayClip("tone1");
		}

		else if (step6)
		{
			step6 = false;
			step7 = true;
			timerText->GetLabel()->runAction(cocos2d::Sequence::create(
				cocos2d::FadeOut::create(0.5),
				cocos2d::CallFunc::create([this]() {
				timerText->setString("\"Reset\" creates a new random set of tiles with a different matching rule.");
				timerText->Center();
				timerText->setPositionY(timeRemainingBottomPosition + textOffsetY);}),
				cocos2d::FadeIn::create(0.5), NULL));

			generateTimePlusThumbnail->stopAllActions();
			generateEliminateThumbnail->stopAllActions();
			generateShowMatchThumbnail->stopAllActions();
			generateNewCardThumbnail->stopAllActions();

			generateTimePlusThumbnail->runAction(cocos2d::Spawn::create(
				cocos2d::FadeTo::create(1, 128),
				cocos2d::ScaleTo::create(1, originalScale),
				NULL));

			generateEliminateThumbnail->runAction(cocos2d::Spawn::create(
				cocos2d::FadeTo::create(1, 128),
				cocos2d::ScaleTo::create(1, originalScale),
				NULL));

			generateShowMatchThumbnail->runAction(cocos2d::Spawn::create(
				cocos2d::FadeTo::create(1, 128),
				cocos2d::ScaleTo::create(1, originalScale),
				NULL));

			generateNewCardThumbnail->runAction(cocos2d::Spawn::create(
				cocos2d::FadeTo::create(1, 255),
				cocos2d::ScaleTo::create(1, originalScale * 1.5),
				NULL));

			//FADE IN ICONS
			newCardTile->runAction(cocos2d::FadeIn::create(1));

			audio->PlayClip("tone2");

		}

		else if (step7)
		{
			step7 = false;
			step8 = true;
			timerText->GetLabel()->runAction(cocos2d::Sequence::create(
				cocos2d::FadeOut::create(0.5),
				cocos2d::CallFunc::create([this]() {
				timerText->setString("When you have Bonus Items available, they will appear on the screen.");
				timerText->Center();
				timerText->setPositionY(timeRemainingBottomPosition + textOffsetY);}),
				cocos2d::FadeIn::create(0.5), NULL));

			generateTimePlusThumbnail->stopAllActions();
			generateEliminateThumbnail->stopAllActions();
			generateShowMatchThumbnail->stopAllActions();
			generateNewCardThumbnail->stopAllActions();

			generateTimePlusThumbnail->runAction(cocos2d::Spawn::create(
				cocos2d::FadeTo::create(1, 255),
				cocos2d::ScaleTo::create(1, originalScale),
				NULL));

			generateEliminateThumbnail->runAction(cocos2d::Spawn::create(
				cocos2d::FadeTo::create(1, 255),
				cocos2d::ScaleTo::create(1, originalScale),
				NULL));

			generateShowMatchThumbnail->runAction(cocos2d::Spawn::create(
				cocos2d::FadeTo::create(1, 255),
				cocos2d::ScaleTo::create(1, originalScale),
				NULL));

			generateNewCardThumbnail->runAction(cocos2d::Spawn::create(
				cocos2d::FadeTo::create(1, 255),
				cocos2d::ScaleTo::create(1, originalScale),
				NULL));

			//FADE IN TEXT
			timePlusText->GetLabel()->runAction(cocos2d::FadeIn::create(1));
			eliminateText->GetLabel()->runAction(cocos2d::FadeIn::create(1));
			showMatchText->GetLabel()->runAction(cocos2d::FadeIn::create(1));
			newCardText->GetLabel()->runAction(cocos2d::FadeIn::create(1));

			audio->PlayClip("tone1");
		}

		else if (step8)
		{
			step8 = false;
			step9 = true;
			timerText->GetLabel()->runAction(cocos2d::Sequence::create(
				cocos2d::FadeOut::create(0.5),
				cocos2d::CallFunc::create([this]() {
				timerText->setString("The numbers above the Bonus Items show how many you have left.");
				timerText->Center();
				timerText->setPositionY(timeRemainingBottomPosition + textOffsetY);}),
				cocos2d::FadeIn::create(0.5), NULL));

			timePlusText->GetLabel()->stopAllActions();
			eliminateText->GetLabel()->stopAllActions();
			showMatchText->GetLabel()->stopAllActions();
			newCardText->GetLabel()->stopAllActions();

			timePlusText->GetLabel()->runAction(cocos2d::RepeatForever::create(
				cocos2d::Sequence::create(
					cocos2d::DelayTime::create(0.125),
					cocos2d::FadeTo::create(0.5, 64),
					cocos2d::FadeTo::create(0.5, 255),
					cocos2d::DelayTime::create(0.125), NULL)));

			eliminateText->GetLabel()->runAction(cocos2d::RepeatForever::create(
				cocos2d::Sequence::create(
					cocos2d::DelayTime::create(0.125),
					cocos2d::FadeTo::create(0.5, 64),
					cocos2d::FadeTo::create(0.5, 255),
					cocos2d::DelayTime::create(0.125), NULL)));

			showMatchText->GetLabel()->runAction(cocos2d::RepeatForever::create(
				cocos2d::Sequence::create(
					cocos2d::DelayTime::create(0.125),
					cocos2d::FadeTo::create(0.5, 64),
					cocos2d::FadeTo::create(0.5, 255),
					cocos2d::DelayTime::create(0.125), NULL)));

			newCardText->GetLabel()->runAction(cocos2d::RepeatForever::create(
				cocos2d::Sequence::create(
					cocos2d::DelayTime::create(0.125),
					cocos2d::FadeTo::create(0.5, 64),
					cocos2d::FadeTo::create(0.5, 255),
					cocos2d::DelayTime::create(0.125), NULL)));

			audio->PlayClip("tone2");

		}

		else if (step9)
		{
			step9 = false;
			step10 = true;
			timerText->GetLabel()->runAction(cocos2d::Sequence::create(
				cocos2d::FadeOut::create(0.5),
				cocos2d::CallFunc::create([this]() {
				timerText->setString("The number on the left is how many items you have earned while playing.");
				timerText->Center();
				timerText->setPositionY(timeRemainingBottomPosition + textOffsetY);}),
				cocos2d::FadeIn::create(0.5), NULL));

			audio->PlayClip("tone2");

		}

		else if (step10)
		{
			step10 = false;
			step11 = true;
			timerText->GetLabel()->runAction(cocos2d::Sequence::create(
				cocos2d::FadeOut::create(0.5),
				cocos2d::CallFunc::create([this]() {
				timerText->setString("The number on the right is how many items you purchased beforehand.");
				timerText->Center();
				timerText->setPositionY(timeRemainingBottomPosition + textOffsetY);}),
				cocos2d::FadeIn::create(0.5), NULL));

			audio->PlayClip("tone1");

		}

		else if (step11)
		{
			step11 = false;
			step12 = true;
			timerText->GetLabel()->runAction(cocos2d::Sequence::create(
				cocos2d::FadeOut::create(0.5),
				cocos2d::CallFunc::create([this]() {
				timerText->setString("When using Bonus Items, you will always use earned items first.");
				timerText->Center();
				timerText->setPositionY(timeRemainingBottomPosition + textOffsetY);}),
				cocos2d::FadeIn::create(0.5), NULL));

			timePlusText->GetLabel()->stopAllActions();
			timePlusText->GetLabel()->setOpacity(255);

			eliminateText->GetLabel()->stopAllActions();
			eliminateText->GetLabel()->setOpacity(255);

			showMatchText->GetLabel()->stopAllActions();
			showMatchText->GetLabel()->setOpacity(255);

			newCardText->GetLabel()->stopAllActions();
			newCardText->GetLabel()->setOpacity(255);

			audio->PlayClip("tone2");

		}

		else if (step12)
		{
			step12 = false;
			step13 = true;
			timerText->GetLabel()->runAction(cocos2d::Sequence::create(
				cocos2d::FadeOut::create(0.5),
				cocos2d::CallFunc::create([this]() {
				timerText->setString("If you haven't earned any Bonus Items, your purchased items will be used.");
				timerText->Center();
				timerText->setPositionY(timeRemainingBottomPosition + textOffsetY);}),
				cocos2d::FadeIn::create(0.5), NULL));

			audio->PlayClip("tone1");

		}

		else if (step13)
		{
			step13 = false;
			step14 = true;
			timerText->GetLabel()->runAction(cocos2d::Sequence::create(
				cocos2d::FadeOut::create(0.5),
				cocos2d::CallFunc::create([this]() {
				timerText->setString("You can purchase Bonus Items in the \"Buy Bonus Items\" menu.");
				timerText->Center();
				timerText->setPositionY(timeRemainingBottomPosition + textOffsetY);}),
				cocos2d::FadeIn::create(0.5), NULL));

			audio->PlayClip("tone2");

		}

		else if (step14)
		{
			step14 = false;
			step15 = true;

			timerText->GetLabel()->runAction(cocos2d::Sequence::create(
				cocos2d::FadeOut::create(0.5),
				cocos2d::CallFunc::create([this]() {
				timerText->setString("To use a Bonus Item, simply tap it while playing.");
				timerText->Center();
				timerText->setPositionY(timeRemainingBottomPosition + textOffsetY);}),
				cocos2d::FadeIn::create(0.5), NULL));

			audio->PlayClip("tone1");
		}

		else if (step15)
		{
			step15 = false;
			retry->ToggleOn(false);

			timerText->GetLabel()->runAction(cocos2d::Sequence::create(
				cocos2d::FadeOut::create(0.5),
				cocos2d::CallFunc::create([this]() {
				timerText->setString("Use 2x Time Plus Bonus Items, now.");
				timerText->Center();
				timerText->setPositionY(timeRemainingBottomPosition + textOffsetY);}),
				cocos2d::FadeIn::create(0.5), NULL));

			timePlusTile->ToggleTouch(true);

			audio->PlayClip("tone1");
		}

		else if (step16)
		{
			step16 = false;
			step17 = true;
			audio->PlayClip("tone2");
            
            timerText->GetLabel()->runAction(cocos2d::Sequence::create(
                                                                       cocos2d::FadeOut::create(0.5),
                                                                       cocos2d::CallFunc::create([this]() {
                timerText->setString("1. You cannot use more than 2x Time Plus bonuses per set of tiles.");
                timerText->Center();
                timerText->setPositionY(timeRemainingBottomPosition + textOffsetY);}),
                                                                       cocos2d::FadeIn::create(0.5), NULL));
            
        }

		else if (step17)
		{
			step17 = false;
			step18 = true;

            timerText->GetLabel()->runAction(cocos2d::Sequence::create(
                                                                       cocos2d::FadeOut::create(0.5),
                                                                       cocos2d::CallFunc::create([this]() {
                timerText->setString("2. You can only use Eliminate Tiles and Show Match if you haven't selected any tiles.");
                timerText->Center();
                timerText->setPositionY(timeRemainingBottomPosition + textOffsetY);}),
                                                                       cocos2d::FadeIn::create(0.5), NULL));
            
            eliminateTile->runAction(cocos2d::FadeTo::create(0.5, 64));
            eliminateText->GetLabel()->runAction(cocos2d::FadeTo::create(0.5, 64));
            
            showMatchTile->runAction(cocos2d::FadeTo::create(0.5, 64));
            showMatchText->GetLabel()->runAction(cocos2d::FadeTo::create(0.5, 64));
            
            audio->PlayClip("tone1");

		}

		else if (step18)
		{
			step18 = false;
			step19 = true;

            timerText->GetLabel()->runAction(cocos2d::Sequence::create(
                                                                       cocos2d::FadeOut::create(0.5),
                                                                       cocos2d::CallFunc::create([this]() {
                timerText->setString("3. You cannot use Eliminate Tiles, Show Match, or Reset while new tiles are loading.");
                timerText->Center();
                timerText->setPositionY(timeRemainingBottomPosition + textOffsetY);}),
                                                                       cocos2d::FadeIn::create(0.5), NULL));
            
            newCardTile->runAction(cocos2d::FadeTo::create(0.5, 64));
            newCardText->GetLabel()->runAction(cocos2d::FadeTo::create(0.5, 64));
            
            audio->PlayClip("tone2");

		}

		else if (step19)
		{
			step19 = false;
			step20 = true;

            timerText->GetLabel()->runAction(cocos2d::Sequence::create(
                                                                       cocos2d::FadeOut::create(0.5),
                                                                       cocos2d::CallFunc::create([this]() {
                timerText->setString("Strategically use Bonus Items to get as many matches as possible!");
                timerText->Center();
                timerText->setPositionY(timeRemainingBottomPosition + textOffsetY);}),
                                                                       cocos2d::FadeIn::create(0.5), NULL));
            
            timePlusTile->runAction(cocos2d::FadeTo::create(0.5, 255));
            timePlusText->GetLabel()->runAction(cocos2d::FadeTo::create(0.5, 255));
            
            eliminateTile->runAction(cocos2d::FadeTo::create(0.5, 255));
            eliminateText->GetLabel()->runAction(cocos2d::FadeTo::create(0.5, 255));
            
            showMatchTile->runAction(cocos2d::FadeTo::create(0.5, 255));
            showMatchText->GetLabel()->runAction(cocos2d::FadeTo::create(0.5, 255));
            
            newCardTile->runAction(cocos2d::FadeTo::create(0.5, 255));
            newCardText->GetLabel()->runAction(cocos2d::FadeTo::create(0.5, 255));
            
            audio->PlayClip("tone1");

		}

		else if (step20)
		{
			step20 = false;
			step21 = true;

            timerText->GetLabel()->runAction(cocos2d::Sequence::create(
                                                                       cocos2d::FadeOut::create(0.5),
                                                                       cocos2d::CallFunc::create([this]() {
                timerText->setString("And remember, you can always purchase extra Bonus Items in the \"Buy Bonus Items\" menu!");
                timerText->Center();
                timerText->setPositionY(timeRemainingBottomPosition + textOffsetY);}),
                                                                       cocos2d::FadeIn::create(0.5), NULL));
            
            audio->PlayClip("tone1");

		}

		else if (step21)
		{
			step21 = false;
			step22 = true;

            timerText->GetLabel()->runAction(cocos2d::Sequence::create(
                                                                       cocos2d::FadeOut::create(0.5),
                                                                       cocos2d::CallFunc::create([this]() {
                timerText->setString("You have finished the Using Bonus Items tutorial...");
                timerText->Center();
                timerText->setPositionY(timeRemainingBottomPosition + textOffsetY);}),
                                                                       cocos2d::FadeIn::create(0.5), NULL));
            
            audio->PlayClip("tone2");

		}

		else if (step22)
		{
			step22 = false;
			step23 = true;

            retry->ChangeMenu("Finish", 0, false);
            retry->Center();
            retry->SetBottom(retryBottomPosition);
            retry->ToggleOn(true);
            retry->GetItem(0)->runAction(cocos2d::RepeatForever::create(
                                                                        cocos2d::Sequence::create(
                                                                                                  cocos2d::DelayTime::create(0.125),
                                                                                                  cocos2d::FadeTo::create(0.5, 64),
                                                                                                  cocos2d::FadeTo::create(0.5, 255),
                                                                                                  cocos2d::DelayTime::create(0.125), NULL)));
            
            timerText->GetLabel()->runAction(cocos2d::Sequence::create(
                                                                       cocos2d::FadeOut::create(0.5),
                                                                       cocos2d::CallFunc::create([this]() {
                timerText->setString("You can view this tutorial again anytime from the \"Options\" menu.");
                timerText->Center();
                timerText->setPositionY(timeRemainingBottomPosition + textOffsetY);}),
                                                                       cocos2d::FadeIn::create(0.5), NULL));
            
            audio->PlayClip("tone2");

		}

		else if (step23)
		{
			step23 = false;
            audio->PlayClip("triple_tone_high");
            instance = NULL;
            director->popScene();
		}
	}

	if (timePlusTile->WasTouched())
	{
		if (!used1TimePlus)
		{
			audio->PlayClip("double_tone_higher");
			timePlusText->setString("0 / 2");
			timePlusText->setPositionX(timePlusTile->getPositionX() + timePlusTile->GetWidth() / 2 - timePlusText->GetScaledWidth() / 2);
			used1TimePlus = true;
		}

		else if (used1TimePlus)
		{
			audio->PlayClip("double_tone_higher");
			timePlusText->setString("0 / 1");
			timePlusText->setPositionX(timePlusTile->getPositionX() + timePlusTile->GetWidth() / 2 - timePlusText->GetScaledWidth() / 2);

			timePlusTile->runAction(cocos2d::FadeTo::create(0.5, 64));
			timePlusText->GetLabel()->runAction(cocos2d::FadeTo::create(0.5, 64));

            timerText->GetLabel()->runAction(cocos2d::Sequence::create(
                                                                       cocos2d::FadeOut::create(0.5),
                                                                       cocos2d::CallFunc::create([this]() {
                timerText->setString("Sometimes, you won't be allowed to use a Bonus item. When this happens, the item will fade.");
                timerText->Center();
                timerText->setPositionY(timeRemainingBottomPosition + textOffsetY);}),
                                                                       cocos2d::FadeIn::create(0.5), NULL));
            step16 = true;
			timePlusTile->ToggleTouch(false);
			retry->ToggleOn(true);
		}
	}

	if (backButton->WasPressed())
	{
		audio->PlayClip("low_tone");
		instance = NULL;
		director->popScene();
	}

	return;
}

void Tutorial_Bonus::Reset()
{
	Game::Reset();

	timePlusText->setString("1 / 2");
	timePlusText->setPositionX(timePlusTile->getPositionX() + timePlusTile->GetWidth() / 2 - timePlusText->GetScaledWidth() / 2);

	eliminateText->setString("1 / 1");
	eliminateText->setPositionX(eliminateTile->getPositionX() + eliminateTile->GetWidth() / 2 - eliminateText->GetScaledWidth() / 2);

	showMatchText->setString("1 / 1");
	showMatchText->setPositionX(showMatchTile->getPositionX() + showMatchTile->GetWidth() / 2 - showMatchText->GetScaledWidth() / 2);

	newCardText->setString("1 / 1");
	newCardText->setPositionX(newCardTile->getPositionX() + newCardTile->GetWidth() / 2 - newCardText->GetScaledWidth() / 2);

	timePlusTile->ToggleEnabled(true);
	eliminateTile->ToggleEnabled(true);
	showMatchTile->ToggleEnabled(true);
	newCardTile->ToggleEnabled(true);

	timePlusTile->ToggleTouch(false);
	eliminateTile->ToggleTouch(false);
	showMatchTile->ToggleTouch(false);
	newCardTile->ToggleTouch(false);

	timePlusTile->setOpacity(0);
	eliminateTile->setOpacity(0);
	showMatchTile->setOpacity(0);
	newCardTile->setOpacity(0);

	timePlusText->GetLabel()->setOpacity(0);
	eliminateText->GetLabel()->setOpacity(0);
	showMatchText->GetLabel()->setOpacity(0);
	newCardText->GetLabel()->setOpacity(0);

	timerText->ChangeColor(cocos2d::Color4B::GRAY);
	retry->ChangeMenu("Next", 0, false);
	retry->Center();
	retry->SetBottom(retryBottomPosition);

	//Intro animations
	int offset = 50;
	float duration = 0.5;
	originalScale = generateTimePlusThumbnail->getScale();

	baseTile->setPositionY(baseTile->getPositionY() + offset);
	baseTile->runAction(cocos2d::Sequence::create(
		cocos2d::Spawn::create(
			cocos2d::FadeIn::create(duration),
			cocos2d::MoveBy::create(duration, cocos2d::Vec2(0, -offset)), NULL),
		cocos2d::CallFunc::create([this]()
	{
		float baseTileWidth = baseTile->GetWidth();
		float baseTileHeight = baseTile->GetHeight();

		generateTimePlusThumbnail->setPositionX(baseTile->getPositionX() - baseTileWidth * 7 / 32);
		generateTimePlusThumbnail->setPositionY(baseTile->getPositionY() + baseTileHeight * 7 / 32);
		generateTimePlusThumbnail->runAction(cocos2d::FadeIn::create(1));

		generateEliminateThumbnail->setPositionX(baseTile->getPositionX() + baseTileWidth * 7 / 32);
		generateEliminateThumbnail->setPositionY(baseTile->getPositionY() + baseTileHeight * 7 / 32);
		generateEliminateThumbnail->runAction(cocos2d::Sequence::create(cocos2d::DelayTime::create(1), cocos2d::FadeIn::create(1), NULL));

		generateShowMatchThumbnail->setPositionX(baseTile->getPositionX() - baseTileWidth * 7 / 32);
		generateShowMatchThumbnail->setPositionY(baseTile->getPositionY() - baseTileHeight * 7 / 32);
		generateShowMatchThumbnail->runAction(cocos2d::Sequence::create(cocos2d::DelayTime::create(2), cocos2d::FadeIn::create(1), NULL));

		generateNewCardThumbnail->setPositionX(baseTile->getPositionX() + baseTileWidth * 7 / 32);
		generateNewCardThumbnail->setPositionY(baseTile->getPositionY() - baseTileHeight * 7 / 32);
		generateNewCardThumbnail->runAction(cocos2d::Sequence::create(cocos2d::DelayTime::create(3), cocos2d::FadeIn::create(1), NULL));

		baseTileShadow->runAction(cocos2d::FadeIn::create(0.5));
		timerText->setString("After every 5 matches, you will receive 1 of 4 random Bonus Items...\n(Tap \"Next\" to continue)");
		timerText->Center();
		timerText->setPositionY(timeRemainingBottomPosition + textOffsetY);
		timerText->GetLabel()->runAction(cocos2d::FadeIn::create(2));
		retry->GetItem(0)->runAction(cocos2d::RepeatForever::create(
			cocos2d::Sequence::create(
				cocos2d::DelayTime::create(0.125),
				cocos2d::FadeTo::create(0.5, 64),
				cocos2d::FadeTo::create(0.5, 255),
				cocos2d::DelayTime::create(0.125), NULL)));
		retry->ToggleOn(true);
	}), NULL));

	return;
}

void Tutorial_Bonus::onEnter()
{
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	sdkbox::PluginAdMob::hide("home");
#endif
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	audio->SetMusicVolume(0.35);
#endif

#if(CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	audio->SetMusicVolume(Game::GetiOSMinVolumeBG());
#endif
	Game::onEnter();
	return;
}

void Tutorial_Bonus::onExit()
{
	Game::onExit();
	return;
}
