#include "pch.h"
#include "Tutorial_Matching.h"


Tutorial_Matching::Tutorial_Matching()
{
}


Tutorial_Matching::~Tutorial_Matching()
{
}

cocos2d::Scene * Tutorial_Matching::CreateScene()
{
	cocos2d::Scene* scene = cocos2d::Scene::create();

	cocos2d::Layer* layer = Tutorial_Matching::create();

	scene->addChild(layer);

	return scene;
}

bool Tutorial_Matching::init()
{
	instance = this;
	
    if(!gameData)
        gameData = cocos2d::UserDefault::getInstance();
    
	gameData->setBoolForKey(tutorial_complete_how_to_play, true);
	gameStarted = true;
	mode = -1;

	//Initialize origin
	SCREEN_WIDTH = glview->getDesignResolutionSize().width;
	SCREEN_HEIGHT = glview->getDesignResolutionSize().height;
	center.x = SCREEN_WIDTH / 2;
	center.y = SCREEN_HEIGHT / 2;

	if (!gameData->getBoolForKey(sound_on))
		audio->MuteSound();

	InitGameAssets();

	//Adjust header tile height for 5:3 and 16:10 aspect ratios
	if (nrgFunctions::GetRatio() < 1.7 && nrgFunctions::GetRatio() >= 1.59)
	{
		headerTile->Move(0, 20);
		nrgFunctions::Center(matchCriteriaText, headerTile);
	}
	
	Reset();

    timerText->ChangeTextSize(15);
    timerText->ChangeColor(cocos2d::Color4B::GRAY);
	timerText->setString("");
	timerText->Center();
	timerText->setPositionY(headerTile->getPositionY() - 10);
	
	sceneTimer.Restart();
	scheduleUpdate();

	return true;
}

void Tutorial_Matching::update(float dt)
{
	if (retry->GetConfirmedSelection() == 0)
	{
		if (step0)
		{
			step0 = false;
			step1 = true;
			timerText->GetLabel()->runAction(cocos2d::Sequence::create(
				cocos2d::FadeOut::create(0.5),
				cocos2d::CallFunc::create([this]() {
				timerText->setString("Tap 3 is a game where you must tap 3 matching tiles to win.");
				timerText->Center();
				timerText->setPositionY(headerTile->getPositionY() - 10);}),
				cocos2d::FadeIn::create(0.5), NULL));

			audio->PlayClip("tone1");
		}

		else if (step1)
		{
			step1 = false;
			step2 = true;
			timerText->GetLabel()->runAction(cocos2d::Sequence::create(
				cocos2d::FadeOut::create(0.5),
				cocos2d::CallFunc::create([this]() {
					timerText->setString("You must tap 3 matching tiles below, depending on the rule above...");
					timerText->Center();
					timerText->setPositionY(headerTile->getPositionY() - 10);}),
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
				timerText->setString("The text above will tell you the matching rule to follow...");
				timerText->Center();
				timerText->setPositionY(headerTile->getPositionY() - 10);}),
				cocos2d::FadeIn::create(0.5), NULL));

			audio->PlayClip("tone2");
		}

		else if (step3)
		{
			step3 = false;
			step4 = true;
			
			for (int iii = 0; iii < tiles.size(); iii++)
				tiles[iii]->ToggleTouch(true);

			Game::UpdateHeaderText(matchCriteria);
			timerText->GetLabel()->runAction(cocos2d::Sequence::create(
				cocos2d::FadeOut::create(0.5),
				cocos2d::CallFunc::create([this]() {
				timerText->setString("Here, you need to match Shapes...");
				timerText->Center();
				timerText->setPositionY(headerTile->getPositionY() - 10);}),
				cocos2d::FadeIn::create(0.5), NULL));

			audio->PlayClip("tone1");
		}

		else if (step4)
		{
			step4 = false;
			step5 = true;
			retry->ToggleOn(false);

			timerText->GetLabel()->runAction(cocos2d::Sequence::create(
				cocos2d::FadeOut::create(0.5),
				cocos2d::CallFunc::create([this]() {
				timerText->setString("Tap the 3 tiles that are the same shape to continue.");
				timerText->Center();
				timerText->setPositionY(headerTile->getPositionY() - 10);}),
				cocos2d::FadeIn::create(0.5), NULL));

			audio->PlayClip("double_tone_high");
		}

		else if (step6)
		{
			step6 = false;
			step7 = true;
			retry->ToggleOn(false);

			timerText->GetLabel()->runAction(cocos2d::Sequence::create(
				cocos2d::FadeOut::create(0.5),
				cocos2d::CallFunc::create([this]() {
				timerText->setString("Tap the 3 tiles that are the same color to continue.");
				timerText->Center();
				timerText->setPositionY(headerTile->getPositionY() - 10);}),
				cocos2d::FadeIn::create(0.5), NULL));

			audio->PlayClip("double_tone_high");
		}

		else if (step8)
		{
			step8 = false;
			step9 = true;
			timerText->GetLabel()->runAction(cocos2d::Sequence::create(
				cocos2d::FadeOut::create(0.5),
				cocos2d::CallFunc::create([this]() {
				timerText->setString("Style refers to the drawing style of the shape...");
				timerText->Center();
				timerText->setPositionY(headerTile->getPositionY() - 10);}),
				cocos2d::FadeIn::create(0.5), NULL));

			audio->PlayClip("tone2");

		}

		else if (step9)
		{
			step9 = false;
			step10 = true;
			timerText->GetLabel()->runAction(cocos2d::Sequence::create(
				cocos2d::FadeOut::create(0.5),
				cocos2d::CallFunc::create([this]() {
				timerText->setString("Solid, outlined, patterns, etc...");
				timerText->Center();
				timerText->setPositionY(headerTile->getPositionY() - 10);}),
				cocos2d::FadeIn::create(0.5), NULL));

			audio->PlayClip("tone1");

		}
		
		else if (step10)
		{
			step10 = false;
			step11 = true;
			
			retry->ToggleOn(false);

			for (int iii = 0; iii < tiles.size(); iii++)
			{
				tiles[iii]->ToggleEnabled(true);
				tiles[iii]->SetFade(0);
				tiles[iii]->RestoreTile();
			}

			timerText->GetLabel()->runAction(cocos2d::Sequence::create(
				cocos2d::FadeOut::create(0.5),
				cocos2d::CallFunc::create([this]() {
				timerText->setString("Tap the 3 tiles that are the same style to finish.");
				timerText->Center();
				timerText->setPositionY(headerTile->getPositionY() - 10);}),
				cocos2d::FadeIn::create(0.5), NULL));

			audio->PlayClip("double_tone_high");
		}

		else if (step12)
		{
			step12 = false;
			step13 = true;

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
				timerText->setPositionY(headerTile->getPositionY() - 10);}),
				cocos2d::FadeIn::create(0.5), NULL));

			audio->PlayClip("tone1");
			
		}

		else if (step13)
		{
			audio->PlayClip("triple_tone_high");
			instance = NULL;
			director->popScene();
		}
	}

	if(backButton->WasPressed())
	{
		audio->PlayClip("low_tone");
		instance = NULL;
		director->popScene();
	}

	//Cycle fade through all squares
	for (int iii = 0; iii < tiles.size(); iii++)
	{
		currentTile = iii;
		tile = tiles[currentTile];

		if (tile->WasTouched())
		{
			if (step4)
			{
				step4 = false;
				step5 = true;
				retry->ToggleOn(false);
			}

			else if (step6)
			{
				retry->ToggleOn(false);
				step6 = false;
				step7 = true;
			}

			switch (currentSelection)
			{
			case 1:
				tile->DismissTile();
				selection1 = tile->GetTilePosition();
				tile->ToggleTouch(false);
				currentSelection = 2;
				audio->PlayClip("tone1");
				break;
			case 2:
				tile->DismissTile();
				selection2 = tile->GetTilePosition();
				tile->ToggleTouch(false);
				currentSelection = 3;
				audio->PlayClip("tone2");
				break;
			case 3:
				tile->DismissTile();
				selection3 = tile->GetTilePosition();
				lastTouchLocation = tile->GetLastTouchLocation();

				//Prevent further taps until after tiles refresh
				for (int iii = 0; iii < tiles.size(); iii++)
					tiles[iii]->ToggleTouch(false);

				currentSelection = 4;
				break;
			}
		}
	}

	//Check for match or not
	if (currentSelection == 4)
	{
		currentSelection = 1;

		if (matchingTiles.find(selection1) != matchingTiles.end() && matchingTiles.find(selection2) != matchingTiles.end() && matchingTiles.find(selection3) != matchingTiles.end())
			matchMade = true;
		else
			gameOver = true;

		if (matchMade)
		{
			audio->PlayClip("double_tone_high");

			if (step5)
			{
				retry->ToggleOn(true);

				for (int iii = 0; iii < tiles.size(); iii++)
					tiles[iii]->ToggleTouch(true);

				step5 = false;
				step6 = true;
				timerText->GetLabel()->runAction(cocos2d::Sequence::create(
					cocos2d::FadeOut::create(0.5),
					cocos2d::CallFunc::create([this]() {
					timerText->setString("Good! Now, match colors...");
					timerText->Center();
					timerText->setPositionY(headerTile->getPositionY() - 10);}),
					cocos2d::FadeIn::create(0.5), NULL));
			}


			else if (step7)
			{
				retry->ToggleOn(true);

				for (int iii = 0; iii < tiles.size(); iii++)
					tiles[iii]->ToggleEnabled(false);

				step7 = false;
				step8 = true;
				timerText->GetLabel()->runAction(cocos2d::Sequence::create(
					cocos2d::FadeOut::create(0.5),
					cocos2d::CallFunc::create([this]() {
					timerText->setString("Great! Finally, you need to learn to match the style...");
					timerText->Center();
					timerText->setPositionY(headerTile->getPositionY() - 10);}),
					cocos2d::FadeIn::create(0.5), NULL));
			}

			else if (step11)
			{
				retry->ChangeMenu("Next", 0, false);
				retry->Center();
				retry->SetBottom(retryBottomPosition);
				retry->ToggleOn(true);
				retry->GetItem(0)->runAction(cocos2d::RepeatForever::create(
					cocos2d::Sequence::create(
						cocos2d::DelayTime::create(0.125),
						cocos2d::FadeTo::create(0.5, 64),
						cocos2d::FadeTo::create(0.5, 255),
						cocos2d::DelayTime::create(0.125), NULL)));

				step11 = false;
				step12 = true;
				timerText->GetLabel()->runAction(cocos2d::Sequence::create(
					cocos2d::FadeOut::create(0.5),
					cocos2d::CallFunc::create([this]() {
					timerText->setString("Congratulations! You have finished the Tap 3 Matching Tutorial...");
					timerText->Center();
					timerText->setPositionY(headerTile->getPositionY() - 10);}),
					cocos2d::FadeIn::create(0.5), NULL));

				UpdateHeaderText("Good Job!");
			}

			if (!step12 && !step13)
			{
				DismissHeaderText();
				dismissTiles = true;

				sceneTimer.SetMark(100);
				matchMade = false;
			}
		}

		else if (gameOver)
		{
			audio->PlayClip("double_tone_low");
			
			timerText->GetLabel()->runAction(cocos2d::Sequence::create(
				cocos2d::FadeOut::create(0.5),
				cocos2d::CallFunc::create([this]() {
				timerText->setString("Try again. Match the tiles based on the rule above.");
				timerText->Center();
				timerText->setPositionY(headerTile->getPositionY() - 10);}),
				cocos2d::FadeIn::create(0.5), NULL));
				dismissTiles = true;
		}
	}

	//Dismiss all tiles
	if (dismissTiles && sceneTimer.GetElapsedTime() >= sceneTimer.GetMark())
	{
		for (int iii = 0; iii < tiles.size(); iii++)
			tiles[iii]->DismissTile();

		dismissTiles = false;
		randomizeTiles = true;
		sceneTimer.SetMark(500);
	}

	//Randomize all tiles
	if (randomizeTiles && sceneTimer.GetElapsedTime() >= sceneTimer.GetMark())
	{
		SetMatchParameters();
		Game::UpdateHeaderText(matchCriteria);
		RandomizeTiles();
		randomizeTiles = false;
		restoreTiles = true;
		sceneTimer.SetMark(500);
	}

	//Restore all tiles
	if (restoreTiles && sceneTimer.GetElapsedTime() >= sceneTimer.GetMark())
	{
		RestoreTiles();

		if (step8)
		{
			for (int iii = 0; iii < tiles.size(); iii++)
				tiles[iii]->ToggleTouch(false);
		}

		restoreTiles = false;
		sceneTimer.ResetMark();
	}

	return;
}

void Tutorial_Matching::Reset()
{
	Game::Reset();

	timerText->ChangeColor(cocos2d::Color4B::GRAY);
	retry->ChangeMenu("Next", 0, false);
	retry->Center();
	retry->SetBottom(retryBottomPosition);

	//Intro animations
	int offset = 50;
	float duration = 0.5;

	baseTile->setPositionY(baseTile->getPositionY() + offset);
	baseTile->runAction(cocos2d::Sequence::create(
		cocos2d::Spawn::create(
			cocos2d::FadeIn::create(duration),
			cocos2d::MoveBy::create(duration, cocos2d::Vec2(0, -offset)), NULL),
		cocos2d::CallFunc::create([this]()
	{
		RestoreTiles();

		//Turn off touch until game starts
		for (int iii = 0; iii < tiles.size(); iii++)
			tiles[iii]->ToggleTouch(false);

		baseTileShadow->runAction(cocos2d::FadeIn::create(0.5));
		headerTile->runAction(cocos2d::FadeIn::create(0.5));
		matchCriteriaText->GetLabel()->runAction(cocos2d::FadeIn::create(0.5));
		timerText->setString("Welcome to Tap 3!\n(Tap \"Next\" to continue)");
		timerText->Center();
		timerText->setPositionY(headerTile->getPositionY() - 10);
		timerText->GetLabel()->runAction(cocos2d::FadeIn::create(2));
		retry->GetItem(0)->runAction(cocos2d::RepeatForever::create(
			cocos2d::Sequence::create(
				cocos2d::DelayTime::create(0.125),
				cocos2d::FadeTo::create(0.5, 64),
				cocos2d::FadeTo::create(0.5, 255),
				cocos2d::DelayTime::create(0.125), NULL)));
		retry->ToggleOn(true);
	}), NULL));

	//Randomize initial tile screen
	SetMatchParameters();
	RandomizeTiles();
	UpdateHeaderText("How To Play");

	return;
}

void Tutorial_Matching::SetMatchParameters(int excludedMatchCriteria, bool blind)
{
	Game::SetMatchParameters();
	excludedShapes.erase(HEXAGON);
	excludedShapes.erase(OCTAGON);
	matchingStyles = true;
	useDesignatedMatchStyle = true;
	designatedMatchStyle.first = 0;
	designatedMatchStyle.second = 0;
	useDesignatedNonmatchStyle = true;
	designatedNonmatchStyle.first = 0;
	designatedNonmatchStyle.second = 0;

	if (step1 || step5)
	{
		matchCriteria = SHAPES;
		matchingColors = true;
		useDesignatedMatchStyle = true;
		useDesignatedNonmatchStyle = true;
		designatedMatchStyle.first = 0;
		designatedMatchStyle.second = 0;
		designatedNonmatchStyle.first = 0;
		designatedNonmatchStyle.second = 0;
	}

	else if (step6 || step7)
	{
		matchCriteria = COLORS;
		matchingStyles = true;
		useDesignatedMatchStyle = true;
		useDesignatedNonmatchStyle = true;
		designatedMatchStyle.first = 0;
		designatedMatchStyle.second = 0;
		designatedNonmatchStyle.first = 0;
		designatedNonmatchStyle.second = 0;
	}

	else if (step8 || step11)
	{
		matchCriteria = STYLES;
		matchingStyles = true;
		matchingColors = true;
		useDesignatedMatchStyle = true;
		useDesignatedNonmatchStyle = false;
		designatedMatchStyle.first = 1;
		designatedMatchStyle.second = 1;
	}

	switch (matchCriteria)
	{
	case COLORS:
		matchingColors = true;
		nonmatchedColorsCanExceedTwo = false;
		break;
	case SHAPES:
		matchingShapes = true;
		nonmatchedShapesCanExceedTwo = false;
		break;
	case STYLES:
		matchingStyles = true;
		nonmatchedStylesCanExceedTwo = false;
		break;
	}

	return;
}

void Tutorial_Matching::onEnter()
{
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    sdkbox::PluginAdMob::hide("home");
#endif
	Game::onEnter();
	return;
}

void Tutorial_Matching::onExit()
{
	Game::onExit();
	return;
}
