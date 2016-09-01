#pragma once
#include "pch.h"
#include "MemoryMode.h"

MemoryMode::MemoryMode()
{
}

cocos2d::Scene * MemoryMode::CreateScene()
{
	//Create Scene
	cocos2d::Scene* scene = cocos2d::Scene::create();

	//Create layer
	cocos2d::Layer* layer = MemoryMode::create();

	scene->addChild(layer);

	return scene;
}

bool MemoryMode::init()
{
	instance = this;
	
    if(!gameData)
        gameData = cocos2d::UserDefault::getInstance();
    
	if (!gameData->getBoolForKey(sound_on, true))
		audio->MuteSound();

	//Initialize origin
	SCREEN_WIDTH = glview->getDesignResolutionSize().width;
	SCREEN_HEIGHT = glview->getDesignResolutionSize().height;
	center.x = SCREEN_WIDTH / 2;
	center.y = SCREEN_HEIGHT / 2;

	//Set clear color to white
	director->setClearColor(cocos2d::Color4F(1, 1, 1, 1));

	//Game Mode Specific variables
	matchMilestone = 5;
	nextMilestone = 5;
	startingMs = 15000;
	warningTime = 3000;
	msLeft = startingMs;
	gameStarted = true;
	mode = MEMORY;

	InitGameAssets();
	sprintf(sz, "%d.00", startingMs / 1000);
	timerText->setString(sz);
	timerText->Center();
	timerText->SetBottom(timeRemainingBottomPosition);

	Reset();

	highestMatch = gameData->getIntegerForKey(best_match_memory, 0);
	highestScore = gameData->getIntegerForKey(best_score_memory, 0);

	//Begin game loop
	retry->ChangeMenu("Ready!", 0, false);
	retry->setVisible(true);
	retry->ToggleOn(false);
	retry->Center();
	retry->SetBottom(pointsTextBottomPosition - readyOffset);

    retry->GetItem(0)->runAction(cocos2d::RepeatForever::create(
        cocos2d::Sequence::create(
            cocos2d::DelayTime::create(0.125),
            cocos2d::FadeTo::create(0.5, 64),
            cocos2d::FadeTo::create(0.5, 255),
            cocos2d::DelayTime::create(0.125), NULL)));
    
	pointsText->GetLabel()->setOpacity(0);

    //Add timers to vector so they can be controlled outside of class methods
	timers.push_back(&sceneTimer);
	timers.push_back(&countdownTimer);
	timers.push_back(&bonusTimer);
	timers.push_back(&sequenceTimer);

    //Begin Game Loop
	scheduleUpdate();
	return true;
}

void MemoryMode::update(float dt)
{
	if (testing)
		ShowMatchingTiles();

	if (!gameOver && !showingAd && !gameData->getBoolForKey(ads_removed, false))
	{
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
		sdkbox::PluginAdMob::show("home");
#endif
		showingAd = true;
	}

	else if (gameOver && showingAd)
	{
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
		sdkbox::PluginAdMob::hide("home");
#endif
		showingAd = false;
	}

#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    if(!gameOver && sdkbox::PluginChartboost::isAnyViewVisible())
        sdkbox::PluginChartboost::closeImpression();
#endif
    
	if (backButton->WasPressed())
	{
		audio->PlayClip("low_tone");

		//If user quits game early, best scores will still be recorded
		CheckForNewBests();

		//Go back to main menu
		director->replaceScene(MainMenu::CreateScene());
	}

	//Randomize bonus item
	if (generateBonus)
	{
		GenerateBonus();
		forcedGameOver = false;
	}

	//Display bonus item chosen
	if (bonusTimer.IsRunning() && bonusTimer.GetElapsedTime() >= bonusTimer.GetMark())
	{
		DisplayBonus();
		forcedGameOver = false;
	}

	//KEEP BONUS ITEM
	if (confirmingBonus && menuRight->WasPressed())
	{
		AcceptBonus();
		forcedGameOver = false;
	}

	//EXCHANGE BONUS FOR POINTS
	if (confirmingBonus && menuLeft->WasPressed())
	{
		TradeBonus();
		forcedGameOver = false;
	}

	//Don't perform any actions if generating bonus items
	if (confirmingBonus || (bonusTimer.IsRunning() && bonusTimer.GetElapsedTime() <= bonusTimer.GetMark()))
	{
		forcedGameOver = false;
		return;
	}

	//Displays sequence text for beginning game
	if (displaySequence && !sequenceTimer.IsMarkSet())
	{
		CheckSequence();
		DisplaySequence();
	}

	//End Display Sequence after time is up.
	if (displaySequence && sequenceTimer.IsMarkSet() && sequenceTimer.GetElapsedTime() >= sequenceTimer.GetMark())
	{
		sequenceText->setVisible(false);
		sequenceTimer.Reset();
		countdownTimer.Reset();
		sceneTimer.Restart();
		displaySequence = false;
		forcedGameOver = false; //Allows player to exit app and resume if sequence is being displayed
		allowRerandomize = true;
		randomizeTiles = true;
		sceneTimer.SetMark(0);
	}

	//Leave loop if displaying sequence
	if (displaySequence)
	{
		forcedGameOver = false;
		return;
	}

	if (optionsButton->WasPressed())
	{
		audio->PlayClip("double_tone_higher");
		backButton->ToggleEnabled(false);
		director->pushScene(Options::CreateScene());
	}

	if (highScoresButton->WasPressed())
	{
		audio->PlayClip("double_tone_higher");
		SyncAchievements();
		SyncHighScores();
#if(CC_TARGET_PLATFORM != CC_PLATFORM_IOS)
		backButton->ToggleEnabled(false);
#endif
		ShowHighScores();
	}

	if (buyBonusItemsButton->WasPressed())
	{
		audio->PlayClip("double_tone_higher");
		backButton->ToggleEnabled(false);
		director->pushScene(Store::CreateScene());
	}

	//Turn off bonus item touch listeners if game over
	if (gameOver)
	{
		timePlusTile->ToggleTouch(false);
		eliminateTile->ToggleTouch(false);
		showMatchTile->ToggleTouch(false);
		newCardTile->ToggleTouch(false);
	}

	//Check if eliminate tile bonus is used
	if (eliminateTile->WasTouched() && bonusEliminateLeft > 0 && powerupsAreOn)
	{
		//Don't use if there are no more tiles to eliminate
		if (excludedEliminateTiles.size() < tiles.size() && eliminateTile->getOpacity() == 255)
		{
			audio->PlayClip("double_tone_higher");
			UseBonusEliminate();
			LowerEliminateBonus();
		}
		else
			audio->PlayClip("low_tone");
	}

	//Check if time plus bonus is used
	if (timePlusTile->WasTouched() && bonusTimePlusLeft > 0)
	{
		if (timePlusTile->getOpacity() == 255)
		{
			audio->PlayClip("double_tone_higher");
			UseBonusTimePlus();
			LowerTimePlusBonus();
		}
		else
			audio->PlayClip("low_tone");
	}

	//Check if show match bonus is used
	if (showMatchTile->WasTouched() && bonusShowMatchLeft > 0 && powerupsAreOn)
	{
		//Don't use if 3 match already showing
		if (matchedTilesShowing < 3 && showMatchTile->getOpacity() == 255)
		{
			audio->PlayClip("double_tone_higher");
			UseShowMatch();
			LowerShowMatchBonus();
		}
		else
			audio->PlayClip("low_tone");
	}

	//Check if new card bonus is used
	if (newCardTile->WasTouched() && bonusNewCardLeft > 0 && powerupsAreOn)
	{
		if (newCardTile->getOpacity() == 255)
		{
			audio->PlayClip("double_tone_higher");
			UseNewCard();
			LowerNewCardBonus();
		}
		else
			audio->PlayClip("low_tone");
	}

	//Cycle through all squares to check if user touched them
	for (int iii = 0; iii < tiles.size(); iii++)
	{
		currentTile = iii;
		tile = tiles[currentTile];

		if (tile->WasTouched())
		{
			switch (currentSelection)
			{
			case 1:
				if (matchedTilesShowing > 0)
					tile->SetFade(128);
				else
					tile->UnhideShape();
				selection1 = tile->GetTilePosition();
				tile->ToggleTouch(false);
				currentSelection = 2;
				audio->PlayClip("tone1");

				//For these matches, order matters
				if (difficulty >= 70 && difficulty <= 79)
				{
					if (tile->GetShape() != tiles[matchCriteriaOrder[0]]->GetShape())
					{
						gameOver = true;
						currentSelection = 4; //Force game over stsate
						losingSelection = 0;
					}
				}

				if (!gameOver && (difficulty >= 70 && difficulty <= 79))
				{
					UpdateHeaderText(nextHeaderStringVector[0]);
					nextHeaderStringVector.erase(nextHeaderStringVector.begin());
				}

				//Keep eliminate tile and show match tile off if currentselection isn't 1 (can only use when player hasn't already selected a tile
				if (bonusEliminateLeft > 0)
				{
					eliminateTile->stopAllActions();
					eliminateText->GetLabel()->stopAllActions();
					eliminateTile->runAction(cocos2d::FadeTo::create(0.25, 64));
					eliminateText->GetLabel()->runAction(cocos2d::FadeTo::create(0.25, 64));
				}

				if (bonusShowMatchLeft > 0)
				{
					showMatchTile->stopAllActions();
					showMatchText->GetLabel()->stopAllActions();
					showMatchTile->runAction(cocos2d::FadeTo::create(0.25, 64));
					showMatchText->GetLabel()->runAction(cocos2d::FadeTo::create(0.25, 64));
				}

				break;
			case 2:
				if (matchedTilesShowing > 0)
					tile->SetFade(128);
				else
					tile->UnhideShape();
				selection2 = tile->GetTilePosition();
				tile->ToggleTouch(false);
				currentSelection = 3;
				audio->PlayClip("tone2");

				//For these matches, order matters
				if (difficulty >= 70 && difficulty <= 79)
				{
					if (tile->GetShape() != tiles[matchCriteriaOrder[1]]->GetShape())
					{
						gameOver = true;
						currentSelection = 4; //Force game over stsate
						losingSelection = 1;
					}
				}

				if (!gameOver && (difficulty >= 70 && difficulty <= 79))
				{
					UpdateHeaderText(nextHeaderStringVector[0]);
					nextHeaderStringVector.erase(nextHeaderStringVector.begin());
				}

				break;
			case 3:
				if (matchedTilesShowing > 0)
					tile->SetFade(128);
				else
					tile->UnhideShape();
				selection3 = tile->GetTilePosition();
				lastTouchLocation = tile->GetLastTouchLocation();

				//Prevent further taps until after tiles refresh
				for (int iii = 0; iii < tiles.size(); iii++)
					tiles[iii]->ToggleTouch(false);

				//For these matches, order matters
				if (difficulty >= 70 && difficulty <= 79)
				{
					if (tile->GetShape() != tiles[matchCriteriaOrder[2]]->GetShape())
					{
						gameOver = true;
						currentSelection = 4; //Force game over stsate
						losingSelection = 2;
					}
					else
						matchMade = true;
				}

				currentSelection = 4;

				break;
			}
		}
	}

	//Check for match success or failure once 3 tiles have been touched
	if (currentSelection == 4)
	{
		currentSelection = 1;

		if (difficulty < 70 || difficulty >= 80)
		{
			if (matchingTiles.find(selection1) != matchingTiles.end() && matchingTiles.find(selection2) != matchingTiles.end() && matchingTiles.find(selection3) != matchingTiles.end())
				matchMade = true;
			else
				gameOver = true;
		}

		if (matchMade)
		{
			matches++;
			gameData->setIntegerForKey(total_matches, gameData->getIntegerForKey(total_matches, 0) + 1);
			gameData->setIntegerForKey(memory_matches, gameData->getIntegerForKey(memory_matches, 0) + 1);
			CheckSequence();
			msLeft = timelimit;
			warningTime = 3000;
			timePlusUsedOnCurrentCard = 0;
			matchedTilesShowing = 0;
			eliminateUsedOnCurrentCard = 0;
			excludedEliminateTiles.clear();

			timerText->GetLabel()->setTextColor(cocos2d::Color4B::BLACK);
			timerText->GetLabel()->runAction(cocos2d::FadeIn::create(0.5));

			//Check for no bonuses used achievement
			if (matches == 50 && bonusesUsed == 0)
				noBonusesUsedBefore50Matches = true;

			if (matchCriteria == COLORS)
				gameData->setIntegerForKey(total_color_matches, gameData->getIntegerForKey(total_color_matches, 0) + 1);
			if (matchCriteria == SHAPES)
				gameData->setIntegerForKey(total_shape_matches, gameData->getIntegerForKey(total_shape_matches, 0) + 1);
			if (matchCriteria == STYLES)
				gameData->setIntegerForKey(total_style_matches, gameData->getIntegerForKey(total_style_matches, 0) + 1);
			if (matchCriteria == COLORS_SHAPES)
			{
				gameData->setIntegerForKey(total_color_matches, gameData->getIntegerForKey(total_color_matches, 0) + 1);
				gameData->setIntegerForKey(total_shape_matches, gameData->getIntegerForKey(total_shape_matches, 0) + 1);
			}
			
			if (matches != matchMilestone)
				audio->PlayClip("double_tone_high");

			if (matches == matchMilestone)
			{
				audio->PlayClip("triple_tone_high");
				generateBonus = true;
				matchMilestone += nextMilestone;
			}

			CalculateScore();
			UpdateScore();
			DismissHeaderText();
			dismissTiles = true;
			disablePowerups = true;
			sceneTimer.SetMark(100);
			matchMade = false;

			//Count how many times app has been played today
			std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
			time_t timeObject = std::chrono::system_clock::to_time_t(now);
			tm* currentTime = localtime(&timeObject);

			int today = currentTime->tm_yday;
			int year = currentTime->tm_year;
			int lastDayPlayed = gameData->getIntegerForKey(last_day_opened, today);
			int lastYearPlayed = gameData->getIntegerForKey(last_year_opened, year);

			if (lastDayPlayed == today && lastYearPlayed == year)
			{
				gameData->setIntegerForKey(matches_made_today, gameData->getIntegerForKey(matches_made_today, 0) + 1);
				gameData->setIntegerForKey(matches_made_today, gameData->getIntegerForKey(matches_made_today_for_review, 0) + 1);
			}
			else
			{
				gameData->setIntegerForKey(matches_made_today, 1);
				gameData->setIntegerForKey(matches_made_today_for_review, 1);
			}

			if (gameData->getIntegerForKey(matches_made_today_for_review, 0) >= 150)
			{
				RequestReview();
				gameData->setIntegerForKey(matches_made_today_for_review, 0);
				gameData->setIntegerForKey(times_opened_today, 0);
				gameData->setIntegerForKey(times_played_today, 0);
			}
		}
		else if (gameOver)
		{
			audio->PlayClip("double_tone_low");
            
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
            audio->SetMusicVolume(0.35);
#endif
            
#if(CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
            audio->SetMusicVolume(Game::GetiOSMinVolumeBG());
#endif
            
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
            if(!gameData->getBoolForKey(ads_removed, false))
				sdkbox::PluginChartboost::show(sdkbox::CB_Location_Default);
#endif
			if (matches == 1)
				sprintf(sz, "You Got 1 Match!");
			else
				sprintf(sz, "You Got %d Matches!", matches);
			timerText->setString(sz);
			timerText->Center();
			timerText->SetBottom(timeRemainingBottomPosition);
			timerText->GetLabel()->setTextColor(cocos2d::Color4B::BLACK);
			timerText->GetLabel()->runAction(cocos2d::FadeIn::create(0.5));
			CalculateScore();
			UpdateScore();

			//Check for new best score
			CheckForNewBests();

			//Disable all powerups
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

			CheckForAchievements();
			currentSelection = 1;

			//Turn menu buttons back on
			optionsButton->setVisible(true);
			highScoresButton->setVisible(true);
			buyBonusItemsButton->setVisible(true);
			optionsText->GetLabel()->setVisible(true);
			highScoresText->GetLabel()->setVisible(true);
			buyBonusItemsText->GetLabel()->setVisible(true);
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
				gameData->setIntegerForKey(times_played_today, gameData->getIntegerForKey(times_played_today, 0) + 1);
			else
				gameData->setIntegerForKey(times_played_today, 1);

			if (gameData->getIntegerForKey(times_played_today, 0) >= 25)
			{
				RequestReview();
				gameData->setIntegerForKey(matches_made_today_for_review, 0);
				gameData->setIntegerForKey(times_opened_today, 0);
				gameData->setIntegerForKey(times_played_today, 0);
			}
            
            //Need to know if any achievements were unlocked or if review request is true before calling
            displayGameOver = true;
            DisplayGameOver();
		}
	}

	//Leave loop and begin generate bonus on next loop
	if (generateBonus)
		return;

	if (displaySequence && !sequenceTimer.IsMarkSet())
		DisplaySequence();

	if (displaySequence)
		return;

	//If user left app and resumed, game over will be forced in App Delegate. Follow usual gameOver code
	if (forcedGameOver)
	{
		audio->PlayClip("double_tone_low");
        
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
        audio->SetMusicVolume(0.35);
#endif
        
#if(CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
        audio->SetMusicVolume(Game::GetiOSMinVolumeBG());
#endif
        
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
		if (!gameData->getBoolForKey(ads_removed, false))
			sdkbox::PluginChartboost::show(sdkbox::CB_Location_Default);
#endif
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

		//Reveal match type
		if (difficulty <= 29 || (difficulty >= 40 && difficulty <= 49) || (difficulty >= 60 && difficulty <= 69) || (difficulty >= 80 && difficulty <= 89))
			UpdateHeaderText(matchCriteria);
		else if ((difficulty >= 30 && difficulty <= 39) || (difficulty >= 50 && difficulty <= 59) || difficulty >= 90)
			UpdateHeaderText(nextHeaderString);
		
		//Match type already revealed, do nothing to (to avoid complexity of figuring out match type once string has already been erased from vector)
		else if (difficulty >= 70 && difficulty <= 79)
			;

		//Check for new best score
		CheckForNewBests();

		//Disable all powerups
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

		CheckForAchievements();
		currentSelection = 1;

		//Turn menu buttons back on
		optionsButton->setVisible(true);
		highScoresButton->setVisible(true);
		buyBonusItemsButton->setVisible(true);
		optionsText->GetLabel()->setVisible(true);
		highScoresText->GetLabel()->setVisible(true);
		buyBonusItemsText->GetLabel()->setVisible(true);
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
			gameData->setIntegerForKey(times_played_today, gameData->getIntegerForKey(times_played_today, 0) + 1);
		else
			gameData->setIntegerForKey(times_played_today, 1);

		if (gameData->getIntegerForKey(times_played_today, 0) >= 25)
		{
			RequestReview();
			gameData->setIntegerForKey(matches_made_today_for_review, 0);
			gameData->setIntegerForKey(times_opened_today, 0);
			gameData->setIntegerForKey(times_played_today, 0);
		}

		forcedGameOver = false;
		gameOver = true;
        
        //Need to know if any achievements were unlocked or if review request is true before calling
        displayGameOver = true;
        DisplayGameOver();
	}
	
	//Dismiss all tiles
	if (dismissTiles && sceneTimer.GetElapsedTime() >= sceneTimer.GetMark())
	{
		for (int iii = 0; iii < tiles.size(); iii++)
			tiles[iii]->DismissTile();

		DisplayMatchSummary();
		dismissTiles = false;
		randomizeTiles = true;

		if (bonusNewCardLeft > 0)
		{
			newCardTile->runAction(cocos2d::FadeTo::create(0.25, 64));
			newCardText->GetLabel()->runAction(cocos2d::FadeTo::create(0.25, 64));
		}

		sceneTimer.SetMark(500);
	}

	//Randomize all tiles
	if (randomizeTiles && sceneTimer.GetElapsedTime() >= sceneTimer.GetMark())
	{
		SetMatchParameters();

		//Only reveal match rule after shapes disasppear
		if (difficulty < 40 || (difficulty >= 50 && difficulty <= 59) || (difficulty >= 70 && difficulty <= 79) || (difficulty >= 90) )
		{
			matchCriteriaText->GetLabel()->stopAllActions();
			UpdateHeaderText("Memorize...");
		}
		
		if (difficulty <= 9)
		{
			matchCriteriaText->GetLabel()->runAction(cocos2d::Sequence::create(
				cocos2d::DelayTime::create(2),
				cocos2d::CallFunc::create([this]() {
					UpdateHeaderText(matchCriteria);}),
				NULL));
		}

		if (specialRestore)
			UpdateHeaderText(matchCriteria);

		RandomizeTiles();
		randomizeTiles = false;
		restoreTiles = true;
		sceneTimer.SetMark(500);
	}

	//Update time until game over. Update AFTER checking for any user input (to allow for a match first)
	if (!gameOver)
		UpdateTimer();

	//Restore all tiles
	if (restoreTiles && sceneTimer.GetElapsedTime() >= sceneTimer.GetMark())
	{
		if (gameOver)
		{
			for (int iii = 0; iii < tiles.size(); iii++)
			{
				tiles[iii]->RestoreTile();

				//Only set current selection tile to not be faded
				if (difficulty >= 70 && difficulty <= 79)
				{
					//Current selection - 2 corresponds to the match criteria the user failed on. Fade any tiles not matching the current criteria's shape
					if(tiles[matchCriteriaOrder[losingSelection]]->GetShape() != tiles[iii]->GetShape())
						tiles[iii]->SetFade(128);
				}

				//If game over, look for non matching tiles (int matchingTiles does not contain) and set to half opacity
				else if (matchingTiles.find(iii) == matchingTiles.end())
					tiles[iii]->SetFade(128);
			}
		}

		else if (!specialRestore)
		{
			RestoreTiles();
			countdownTimer.Restart();
			retry->ToggleOn(true);
			pointsText->GetLabel()->setOpacity(0);

			for (int iii = 0; iii < tiles.size(); iii++)
				tiles[iii]->ToggleTouch(false);
		}

		else if(specialRestore)
		{
			//SpecialRestore();
			countdownTimer.Restart();
			
			for (int iii = 0; iii < tiles.size(); iii++)
				tiles[iii]->ToggleTouch(false);

			retry->ToggleOn(false);
			pointsText->GetLabel()->runAction(cocos2d::FadeIn::create(0.25));
		}

		restoreTiles = false;

		if (!gameOver)
		{
			//Special restore handles hide tiles within its own function
			if (!specialRestore)
			{
				hideTiles = true;
				sceneTimer.SetMark(msLeft);
			}

			//Enable time plus when tiles restore
			if (bonusTimePlusLeft > 0 && timePlusUsedOnCurrentCard < timePlusUseLimit)
			{
				timePlusTile->setVisible(true);
				timePlusTile->runAction(cocos2d::FadeTo::create(0.5, 255));
				timePlusTile->ToggleTouch(true);
				timePlusText->GetLabel()->runAction(cocos2d::FadeTo::create(0.5, 255));
			}
		}
	}

	if (specialRestore && !gameOver && restoreTiles == false)
		SpecialRestore();

	if (hideTiles && sceneTimer.GetElapsedTime() >= sceneTimer.GetMark() && !gameOver)
	{
		for (int iii = 0; iii < tiles.size(); iii++)
		{
			tiles[iii]->HideShape();
			tiles[iii]->ToggleTouch(true);
		}

		hideTiles = false;
		enablePowerups = true;
		retry->ToggleOn(false);
		pointsText->GetLabel()->runAction(cocos2d::FadeIn::create(0.25));
		sceneTimer.ResetMark();

		//Only reveal match type after shapes disappear
		if ((difficulty >= 10 && difficulty <= 29))
			UpdateHeaderText(matchCriteria);

		//No need to update header on special restore rounds
		else if ((difficulty >= 40 && difficulty <= 49) || (difficulty >= 60 && difficulty <= 69) || (difficulty >= 80 && difficulty <= 89))
			;
		else if((difficulty >= 30 && difficulty <= 39) || (difficulty >= 50 && difficulty <= 59) || difficulty >= 90)
			UpdateHeaderText(nextHeaderString);
		else if (difficulty >= 70 && difficulty <= 79)
		{
			UpdateHeaderText(nextHeaderStringVector[0]);
			nextHeaderStringVector.erase(nextHeaderStringVector.begin());
		}

		//Disable Time Plus bonus to prevent user from using it and "time up" nullifying it
		if (bonusTimePlusLeft > 0)
		{
			timePlusTile->setOpacity(64);
			timePlusText->GetLabel()->runAction(cocos2d::FadeTo::create(0.25, 64));
		}

		//If we're coming back on app resume, repeat bonuses used
		if (reuseBonuses)
		{
			ReuseBonuses();
			reuseBonuses = false;
		}
	}

	if (disablePowerups)
	{
		powerupsAreOn = false;

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

		if (bonusNewCardLeft > 0)
		{
			newCardTile->runAction(cocos2d::FadeTo::create(0.25, 64));
			newCardText->GetLabel()->runAction(cocos2d::FadeTo::create(0.25, 64));
		}

		disablePowerups = false;
	}

	if (enablePowerups && sceneTimer.GetElapsedTime() >= sceneTimer.GetMark())
	{
		powerupsAreOn = true;

		eliminateTile->setVisible(true);
		showMatchTile->setVisible(true);
		newCardTile->setVisible(true);

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

		if (bonusNewCardLeft > 0)
		{
			newCardTile->runAction(cocos2d::FadeTo::create(0.5, 255));
			newCardTile->ToggleTouch(true);
			newCardText->GetLabel()->runAction(cocos2d::FadeTo::create(0.5, 255));
		}

		enablePowerups = false;
		sceneTimer.ResetMark();
	}
	//Turn off touch if game over
	if (gameOver)
	{
		for (int iii = 0; iii < tiles.size(); iii++)
			tiles[iii]->ToggleTouch(false);

		if (requestReview && achievementsUnlocked == 0)
			displayReviewRequest = true;
        
        if(testing && testingReview && achievementsUnlocked == 0)
            displayReviewRequest = true;
	}

    //Player chooses to advance clock and match tiles now
    if (retry->GetItem(0)->getString() == "Ready!" && retry->GetConfirmedSelection() == 0)
    {
        audio->PlayClip("double_tone_higher");
        hideTiles = true;
        retry->ToggleOn(false);
        pointsText->GetLabel()->runAction(cocos2d::FadeIn::create(0.25));
        sceneTimer.SetMark(0);
        timerText->GetLabel()->runAction(cocos2d::FadeOut::create(0.5));
        msLeft = 0;
        warningTime = -1000;
        
        //Only reveal match type after shapes disappear
        if (matchCriteriaText->GetLabel()->getString() == "Memorize...")
            UpdateHeaderText(matchCriteria);
    }

    if(displayGameOver && retry->GetConfirmedSelection() == 0)
    {
        audio->PlayClip("double_tone_higher");
        displayGameOver = false;
        baseTileText->GetLabel()->setVisible(false);
        bool menuChanged = false;
        
        if (achievementsUnlocked > 0)
        {
            achievementUnlockedHeader->GetLabel()->setVisible(true);
            achievementUnlockedHeader->GetLabel()->runAction(cocos2d::FadeIn::create(0.25));
            
            if(achievementsUnlocked == 1 && !requestReview)
            {
                retry->ChangeMenu("See Matching Tiles", 0, false);
                menuChanged = true;
            }
        }
        
        else
        {
            if(!requestReview)
            {
                if(matches >= 100)
                    retry->ChangeMenu("Play Again", 0, false);
                else
                    retry->ChangeMenu("Try Again", 0, false);
                
                restoreTiles = true;
            }
            
            else
                retry->ChangeMenu("See Matching Tiles", 0, false);
            
            menuChanged = true;
        }
        
        if(menuChanged)
        {
            retry->Center();
            retry->SetBottom(retryBottomPosition);
            retry->ToggleOn(true);
            retry->GetItem(0)->runAction(cocos2d::RepeatForever::create(
                cocos2d::Sequence::create(
                    cocos2d::DelayTime::create(0.125),
                    cocos2d::FadeTo::create(0.5, 64),
                    cocos2d::FadeTo::create(0.5, 255),
                    cocos2d::DelayTime::create(0.125), NULL)));
        }
    }
    
    //Check to see if app should ask user for review
    if(!displayGameOver)
        CheckForReview();

    //Close review request forms if user presses continue or see matching tiles
    if(showingReviewRequest && retry->GetConfirmedSelection() == 0)
    {
        audio->PlayClip("double_tone_higher");
        
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
        showingReviewRequest = false;
        
        if(matches >= 100)
            retry->ChangeMenu("Play Again", 0, false);
        else
            retry->ChangeMenu("Try Again", 0, false);
        
        retry->Center();
            retry->SetBottom(retryBottomPosition);
            retry->ToggleOn(true);
            retry->GetItem(0)->runAction(cocos2d::RepeatForever::create(
                cocos2d::Sequence::create(
                    cocos2d::DelayTime::create(0.125),
                    cocos2d::FadeTo::create(0.5, 64),
                    cocos2d::FadeTo::create(0.5, 255),
                    cocos2d::DelayTime::create(0.125), NULL)));
    }
    
    if (!displayGameOver && achievementsUnlocked > 0)
		DisplayAchievements();

	//Cycle through unlocked achievements
	if (!displayGameOver && achievementsUnlocked > 0 && retry->GetConfirmedSelection() == 0)
	{
		audio->PlayClip("double_tone_higher");

		//Remove current achievement being show
        achievementsText[0]->removeFromParent();
		achievementsText.erase(achievementsText.begin());
		achievementsUnlocked--;

        if(achievementsUnlocked == 1 && !requestReview)
        {
            sprintf(sz, "See Matching Tiles");
            
            retry->ChangeMenu(sz, 0, false);
            retry->Center();
            retry->SetBottom(retryBottomPosition);
            retry->ToggleOn(true);
            retry->GetItem(0)->runAction(cocos2d::RepeatForever::create(
                cocos2d::Sequence::create(
                    cocos2d::DelayTime::create(0.125),
                    cocos2d::FadeTo::create(0.5, 64),
                    cocos2d::FadeTo::create(0.5, 255),
                    cocos2d::DelayTime::create(0.125), NULL)));
        }
        
		else if (achievementsUnlocked == 0)
		{
			achievementUnlockedHeader->GetLabel()->stopAllActions();
			achievementUnlockedHeader->GetLabel()->runAction(cocos2d::Sequence::create(cocos2d::FadeOut::create(0.25), cocos2d::CallFunc::create([this]() {achievementUnlockedHeader->GetLabel()->setVisible(false);}), NULL));

            if(!requestReview)
            {
                if(matches >= 100)
                    retry->ChangeMenu("Play Again", 0, false);
                else
                    retry->ChangeMenu("Try Again", 0, false);
                
                restoreTiles = true;
            }
            
            else
                retry->ChangeMenu("See Matching Tiles", 0, false);
            
			retry->Center();
			retry->SetBottom(retryBottomPosition);
			retry->ToggleOn(true);
			retry->GetItem(0)->runAction(cocos2d::RepeatForever::create(
				cocos2d::Sequence::create(
					cocos2d::DelayTime::create(0.125),
					cocos2d::FadeTo::create(0.5, 64),
					cocos2d::FadeTo::create(0.5, 255),
					cocos2d::DelayTime::create(0.125), NULL)));
		}
	}

	//Restart game from 0 matches
	else if (!displayGameOver && achievementsUnlocked == 0 && retry->GetConfirmedSelection() == 0)
	{
		//Make sure to close chartboost ad so it doesn't interrupt gameplay
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
		sdkbox::PluginChartboost::closeImpression();
#endif
		audio->PlayClip("triple_tone_high");

#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
        audio->SetMusicVolume(1);
#endif
        
#if(CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
        audio->SetMusicVolume(Game::GetiOSMaxVolumeBG());
#endif
        
        retry->ChangeMenu("Ready!", 0, false);
		retry->Center();
		retry->SetBottom(pointsTextBottomPosition - readyOffset);
	
		retry->GetItem(0)->runAction(cocos2d::RepeatForever::create(
			cocos2d::Sequence::create(
				cocos2d::DelayTime::create(0.125),
				cocos2d::FadeTo::create(0.5, 64),
				cocos2d::FadeTo::create(0.5, 255),
				cocos2d::DelayTime::create(0.125), NULL)));
		retry->ToggleOn(false);
	
		gameOver = false;
		gameWon = false;

		//For testing purposes
		matches = startingMatches;

		matchMilestone = 5;
		nextMilestone = 5;
		msLeft = startingMs;
		warningTime = 3000;
		sprintf(sz, "%d.00", startingMs / 1000);
		timerText->setString(sz);
		timerText->Center();
		timerText->SetBottom(timeRemainingBottomPosition);
		timerText->GetLabel()->setTextColor(cocos2d::Color4B::BLACK);
		timerText->GetLabel()->runAction(cocos2d::FadeIn::create(0.5));

		//Turn menu buttons off
		optionsButton->runAction(cocos2d::Sequence::create(cocos2d::FadeOut::create(0.25), cocos2d::CallFunc::create([this]() {optionsButton->setVisible(false);}), NULL));
		optionsText->GetLabel()->runAction(cocos2d::Sequence::create(cocos2d::FadeOut::create(0.25), cocos2d::CallFunc::create([this]() {optionsText->GetLabel()->setVisible(false);}), NULL));
		highScoresButton->runAction(cocos2d::Sequence::create(cocos2d::FadeOut::create(0.25), cocos2d::CallFunc::create([this]() {highScoresButton->setVisible(false);}), NULL));
		highScoresText->GetLabel()->runAction(cocos2d::Sequence::create(cocos2d::FadeOut::create(0.25), cocos2d::CallFunc::create([this]() {highScoresText->GetLabel()->setVisible(false);}), NULL));
		buyBonusItemsButton->runAction(cocos2d::Sequence::create(cocos2d::FadeOut::create(0.25), cocos2d::CallFunc::create([this]() {buyBonusItemsButton->setVisible(false);}), NULL));
		buyBonusItemsText->runAction(cocos2d::Sequence::create(cocos2d::FadeOut::create(0.25), cocos2d::CallFunc::create([this]() {buyBonusItemsText->GetLabel()->setVisible(false);}), NULL));
		optionsButton->ToggleTouch(false);
		highScoresButton->ToggleTouch(false);
		buyBonusItemsButton->ToggleTouch(false);

		//Get rid of review request items if present
		requestReview = false;
		baseTileText->GetLabel()->runAction(cocos2d::Sequence::create(cocos2d::FadeOut::create(0.25), cocos2d::CallFunc::create([this]() {baseTileText->GetLabel()->setVisible(false);}), NULL));
		menuLeft->runAction(cocos2d::Sequence::create(cocos2d::FadeOut::create(0.25), cocos2d::CallFunc::create([this]() {menuLeft->setVisible(false);}), NULL));
		menuRight->runAction(cocos2d::Sequence::create(cocos2d::FadeOut::create(0.25), cocos2d::CallFunc::create([this]() {menuRight->setVisible(false);}), NULL));
		leftMenuText->GetLabel()->runAction(cocos2d::Sequence::create(cocos2d::FadeOut::create(0.25), cocos2d::CallFunc::create([this]() {leftMenuText->GetLabel()->setVisible(false);}), NULL));
		rightMenuText->GetLabel()->runAction(cocos2d::Sequence::create(cocos2d::FadeOut::create(0.25), cocos2d::CallFunc::create([this]() {rightMenuText->GetLabel()->setVisible(false);}), NULL));
		menuLeft->ToggleTouch(false);
		menuRight->ToggleTouch(false);
		yesMenuButtonOn = false;
		notReallyMenuButtonOn = false;
		sureMenuButtonOn = false;
		okMenuButtonOn = false;
		noThanksMenuButtonOn = false;

		disablePowerups = true;
		powerupsAreOn = false;
		bestMatchIcon->setOpacity(0);
		bestMatchIcon->setVisible(false);
		bestScoreIcon->setOpacity(0);
		bestScoreIcon->setVisible(false);
		ResetScore();
		finalScore = 0;
		displayedScore = 0;
		UpdateScore();
		pointsText->GetLabel()->setOpacity(0);
		currentSelection = 1;
		bonusesUsed = 0;
		lastMatchWasNearMiss = false;
		noBonusesUsedBefore50Matches = false;
		nearMiss = false;
		gameData->setIntegerForKey(consecutive_near_misses, 0);
		achievementsUnlocked = 0;
		sequence = 1;
		displaySequence = true;
		specialRestore = false;
		specialRestoreFinished = true;

		generateBonus = false;
		confirmingBonus = false;

		//Get rid of achievements text
		achievementUnlockedHeader->GetLabel()->runAction(cocos2d::Sequence::create(cocos2d::FadeOut::create(0.25), cocos2d::CallFunc::create([this]() {achievementUnlockedHeader->GetLabel()->setVisible(false);}), NULL));
		for (int iii = 0; iii < achievementsText.size(); iii++)
		{
			achievementsText[0]->removeFromParent();
			achievementsText.erase(achievementsText.begin());
		}

		achievementsText.clear();

		SetMatchParameters();
		UpdateHeaderText("Memorize");
		RandomizeTiles();
		RestoreTiles(0);

		purchasedEliminateBonuses = gameData->getIntegerForKey(purchased_eliminate_left, 0);
		purchasedTimePlusBonuses = gameData->getIntegerForKey(purchased_time_plus_left, 0);
		purchasedShowMatchBonuses = gameData->getIntegerForKey(purchased_show_match_left, 0);
		purchasedNewCardBonuses = gameData->getIntegerForKey(purchased_new_card_left, 0);

		bonusEliminateLeft = purchasedEliminateBonuses;
		bonusTimePlusLeft = purchasedTimePlusBonuses;
		bonusShowMatchLeft = purchasedShowMatchBonuses;
		bonusNewCardLeft = purchasedNewCardBonuses;

		matchedTilesShowing = 0;
		timePlusUsedOnCurrentCard = 0;
		eliminateUsedOnCurrentCard = 0;
		excludedEliminateTiles.clear();

		timePlusTile->setVisible(true);
		eliminateTile->setVisible(true);
		showMatchTile->setVisible(true);
		newCardTile->setVisible(true);

		if (bonusTimePlusLeft == 0)
		{
			timePlusTile->setOpacity(0);
			timePlusText->GetLabel()->setOpacity(0);
		}
		else
			timePlusTile->ToggleTouch(true);

		if (bonusEliminateLeft == 0)
		{
			eliminateTile->setOpacity(0);
			eliminateText->GetLabel()->setOpacity(0);
		}
		else
			eliminateTile->ToggleTouch(true);

		if (bonusShowMatchLeft == 0)
		{
			showMatchTile->setOpacity(0);
			showMatchText->GetLabel()->setOpacity(0);
		}
		else
			showMatchTile->ToggleTouch(true);

		if (bonusNewCardLeft == 0)
		{
			newCardTile->setOpacity(0);
			newCardText->GetLabel()->setOpacity(0);
		}
		else
			newCardTile->ToggleTouch(true);

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

		for (int iii = 0; iii < tiles.size(); iii++)
			tiles[iii]->ToggleTouch(false);

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

		sceneTimer.Restart();
		countdownTimer.Restart();
		hideTiles = true;
		sceneTimer.SetMark(timelimit);
		
	}

	return;
}

void MemoryMode::ShowMatchingTiles()
{
	if (difficulty < 70 || difficulty >= 80)
		Game::ShowMatchingTiles();
	else if (difficulty >= 70 && difficulty <= 79)
		tiles[matchCriteriaOrder[currentSelection - 1]]->SetFade(128);

	return;
}

void MemoryMode::SpecialRestore(int patternInput)
{
	//Establish pattern
	if (patternInput == -1)
	{
		//Reset after limit is reached
		if (pattern >= 5)
			pattern = 0;

		patternInput = pattern;
	}

	if (specialRestoreStart)
	{
		//Stop all tile actions
		tiles[TOP_LEFT]->StopActions();
		tiles[MIDDLE_LEFT]->StopActions();
		tiles[BOTTOM_LEFT]->StopActions();
		tiles[TOP_CENTER]->StopActions();
		tiles[MIDDLE_CENTER]->StopActions();
		tiles[BOTTOM_CENTER]->StopActions();
		tiles[TOP_RIGHT]->StopActions();
		tiles[MIDDLE_RIGHT]->StopActions();
		tiles[BOTTOM_RIGHT]->StopActions();

		//Disable time plus once tiles are hidden (and timer is out)
		if (bonusTimePlusLeft > 0)
		{
			timePlusTile->runAction(cocos2d::FadeTo::create(0.25, 64));
			timePlusText->GetLabel()->runAction(cocos2d::FadeTo::create(0.25, 64));
		}

		specialRestoreStart = false;
		specialRestoreFinished = false;
		sceneTimer.SetMark(0);
		runGroup1a = true;
		runGroup1b = false;
		runGroup2a = false;
		runGroup2b = false;
		runGroup3a = false;
		runGroup3b = false;
	}

	switch (patternInput)
	{
	case 0: //Regular fade in
			//SHOW FIRST TILES
		if (runGroup1a)
		{
			tiles[TOP_LEFT]->runAction(
				cocos2d::CallFunc::create([this]() {
					//Move first colum into place over 150 ms, starting at 0 ms
					tiles[TOP_LEFT]->RestoreTile(0, 0, 0, 0, false);
					tiles[MIDDLE_LEFT]->RestoreTile(0, 0, 0, 0, false);
					tiles[BOTTOM_LEFT]->RestoreTile(0, 0, 0, 0, false);
					tiles[TOP_LEFT]->ToggleTouch(false);
					tiles[MIDDLE_LEFT]->ToggleTouch(false);
					tiles[BOTTOM_LEFT]->ToggleTouch(false);
			}));
			sceneTimer.SetMark(msLeft / 3);
			runGroup1a = false;
			runGroup1b = true;
		}

		else if (runGroup1b && sceneTimer.GetElapsedTime() >= sceneTimer.GetMark())
		{
			tiles[TOP_LEFT]->runAction(
				cocos2d::CallFunc::create([this]() {
					tiles[TOP_LEFT]->HideShape();
					tiles[MIDDLE_LEFT]->HideShape();
					tiles[BOTTOM_LEFT]->HideShape();;}));

			runGroup1b = false;
			runGroup2a = true;
		}
		

		//SHOW SECOND TILES
		if (runGroup2a)
		{
			tiles[TOP_CENTER]->runAction(
				cocos2d::CallFunc::create([this]() {
					//Move first colum into place over 150 ms, starting at 0 ms
					tiles[TOP_CENTER]->RestoreTile(0, 0, 0, 0, false);
					tiles[MIDDLE_CENTER]->RestoreTile(0, 0, 0, 0, false);
					tiles[BOTTOM_CENTER]->RestoreTile(0, 0, 0, 0, false);
					tiles[TOP_CENTER]->ToggleTouch(false);
					tiles[MIDDLE_CENTER]->ToggleTouch(false);
					tiles[BOTTOM_CENTER]->ToggleTouch(false);}));

			sceneTimer.SetMark(msLeft / 2);
			runGroup2a = false;
			runGroup2b = true;
		}

		else if (runGroup2b && sceneTimer.GetElapsedTime() >= sceneTimer.GetMark())
		{
			tiles[TOP_CENTER]->runAction(
				cocos2d::CallFunc::create([this]() {
					tiles[TOP_CENTER]->HideShape();
					tiles[MIDDLE_CENTER]->HideShape();
					tiles[BOTTOM_CENTER]->HideShape();}));

			runGroup2b = false;
			runGroup3a = true;
		}

		//SHOW LAST TILES
		if (runGroup3a)
		{
			tiles[TOP_RIGHT]->runAction(
				cocos2d::CallFunc::create([this]() {
					//Move first colum into place over 150 ms, starting at 0 ms
					tiles[TOP_RIGHT]->RestoreTile(0, 0, 0, 0, false);
					tiles[MIDDLE_RIGHT]->RestoreTile(0, 0, 0, 0, false);
					tiles[BOTTOM_RIGHT]->RestoreTile(0, 0, 0, 0, false);
					tiles[TOP_RIGHT]->ToggleTouch(false);
					tiles[MIDDLE_RIGHT]->ToggleTouch(false);
					tiles[BOTTOM_RIGHT]->ToggleTouch(false);}));

			sceneTimer.SetMark(msLeft);
			runGroup3a = false;
			runGroup3b = true;
		}

		else if (runGroup3b && sceneTimer.GetElapsedTime() >= sceneTimer.GetMark())
		{
			tiles[TOP_RIGHT]->runAction(
				cocos2d::CallFunc::create([this]() {
					tiles[TOP_RIGHT]->HideShape();
					tiles[MIDDLE_RIGHT]->HideShape();
					tiles[BOTTOM_RIGHT]->HideShape();}));

			runGroup3b = false;
			specialRestoreFinished = true;
		}
		break;

	case 1: //Columns slide in from left
		//SHOW FIRST TILES
		if (runGroup1a)
		{
			tiles[TOP_LEFT]->runAction(
				cocos2d::CallFunc::create([this]() {
				
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
				tiles[TOP_LEFT]->RestoreTile(topLeft.first, topLeft.second, 0.15, 0, false);
				tiles[MIDDLE_LEFT]->RestoreTile(middleLeft.first, middleLeft.second, 0.15, 0, false);
				tiles[BOTTOM_LEFT]->RestoreTile(bottomLeft.first, bottomLeft.second, 0.15, 0, false);
				tiles[TOP_LEFT]->ToggleTouch(false);
				tiles[MIDDLE_LEFT]->ToggleTouch(false);
				tiles[BOTTOM_LEFT]->ToggleTouch(false);}));

			runGroup1a = false;
			runGroup1b = true;
			sceneTimer.SetMark(msLeft / 3);
		}

		else if (runGroup1b && sceneTimer.GetElapsedTime() >= sceneTimer.GetMark())
		{
			tiles[TOP_LEFT]->runAction(
				cocos2d::CallFunc::create([this]() {
				tiles[TOP_LEFT]->HideShape();
				tiles[MIDDLE_LEFT]->HideShape();
				tiles[BOTTOM_LEFT]->HideShape();}));

			runGroup1b = false;
			runGroup2a = true;
		}

		//SHOW SECOND TILES
		if (runGroup2a)
		{
			tiles[TOP_CENTER]->runAction(
				cocos2d::CallFunc::create([this]() {
				//Move first colum into place over 150 ms, starting at 0 ms
				tiles[TOP_CENTER]->RestoreTile(topCenter.first, topCenter.second, 0.3, 0.08, false);
				tiles[MIDDLE_CENTER]->RestoreTile(middleCenter.first, middleCenter.second, 0.3, 0.08, false);
				tiles[BOTTOM_CENTER]->RestoreTile(bottomCenter.first, bottomCenter.second, 0.3, 0.08, false);
				tiles[TOP_CENTER]->ToggleTouch(false);
				tiles[MIDDLE_CENTER]->ToggleTouch(false);
				tiles[BOTTOM_CENTER]->ToggleTouch(false);}));

			runGroup2a = false;
			runGroup2b = true;
			sceneTimer.SetMark(msLeft / 2);
		}

		else if (runGroup2b && sceneTimer.GetElapsedTime() >= sceneTimer.GetMark())
		{
			tiles[TOP_CENTER]->runAction(
				cocos2d::CallFunc::create([this]() {
				tiles[TOP_CENTER]->HideShape();
				tiles[MIDDLE_CENTER]->HideShape();
				tiles[BOTTOM_CENTER]->HideShape();}));

			runGroup2b = false;
			runGroup3a = true;
		}

		//SHOW LAST TILES
		if (runGroup3a)
		{
			tiles[TOP_RIGHT]->runAction(
				cocos2d::CallFunc::create([this]() {
				//Move first colum into place over 150 ms, starting at 0 ms
				tiles[TOP_RIGHT]->RestoreTile(topRight.first, topRight.second, 0.3, 0.3, false);
				tiles[MIDDLE_RIGHT]->RestoreTile(middleRight.first, middleRight.second, 0.3, 0.3, false);
				tiles[BOTTOM_RIGHT]->RestoreTile(bottomRight.first, bottomRight.second, 0.3, 0.3, false);
				tiles[TOP_RIGHT]->ToggleTouch(false);
				tiles[MIDDLE_RIGHT]->ToggleTouch(false);
				tiles[BOTTOM_RIGHT]->ToggleTouch(false);}));

			runGroup3a = false;
			runGroup3b = true;
			sceneTimer.SetMark(msLeft);
		}

		else if (runGroup3b && sceneTimer.GetElapsedTime() >= sceneTimer.GetMark())
		{
			tiles[TOP_RIGHT]->runAction(
				cocos2d::CallFunc::create([this]() {
				tiles[TOP_RIGHT]->HideShape();
				tiles[MIDDLE_RIGHT]->HideShape();
				tiles[BOTTOM_RIGHT]->HideShape();
				tiles[TOP_LEFT]->ToggleTouch(true);
				tiles[MIDDLE_LEFT]->ToggleTouch(true);
				tiles[BOTTOM_LEFT]->ToggleTouch(true);
				tiles[TOP_CENTER]->ToggleTouch(true);
				tiles[MIDDLE_CENTER]->ToggleTouch(true);
				tiles[BOTTOM_CENTER]->ToggleTouch(true);
				tiles[TOP_RIGHT]->ToggleTouch(true);
				tiles[MIDDLE_RIGHT]->ToggleTouch(true);
				tiles[BOTTOM_RIGHT]->ToggleTouch(true);}));

			runGroup3b = false;
			specialRestoreFinished = true;
		}

		break;
	case 2: //Slide columns up from bottom, starting with left column
		

		//SHOW FIRST TILES
		if (runGroup1a)
		{
			tiles[TOP_LEFT]->runAction(
				cocos2d::CallFunc::create([this]() {

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
				tiles[TOP_LEFT]->RestoreTile(topLeft.first, topLeft.second, 0.15, 0, false);
				tiles[MIDDLE_LEFT]->RestoreTile(middleLeft.first, middleLeft.second, 0.15, 0, false);
				tiles[BOTTOM_LEFT]->RestoreTile(bottomLeft.first, bottomLeft.second, 0.15, 0, false);
				tiles[TOP_LEFT]->ToggleTouch(false);
				tiles[MIDDLE_LEFT]->ToggleTouch(false);
				tiles[BOTTOM_LEFT]->ToggleTouch(false);}));

			runGroup1a = false;
			runGroup1b = true;
			sceneTimer.SetMark(msLeft / 3);
		}

		else if (runGroup1b && sceneTimer.GetElapsedTime() >= sceneTimer.GetMark())
		{
			tiles[TOP_LEFT]->runAction(
				cocos2d::CallFunc::create([this]() {
				tiles[TOP_LEFT]->HideShape();
				tiles[MIDDLE_LEFT]->HideShape();
				tiles[BOTTOM_LEFT]->HideShape();}));

			runGroup1b = false;
			runGroup2a = true;
		}

		//SHOW SECOND TILES
		if (runGroup2a)
		{
			tiles[TOP_CENTER]->runAction(
				cocos2d::CallFunc::create([this]() {
				//Move first colum into place over 150 ms, starting at 0 ms
				tiles[TOP_CENTER]->RestoreTile(topCenter.first, topCenter.second, 0.3, 0.08, false);
				tiles[MIDDLE_CENTER]->RestoreTile(middleCenter.first, middleCenter.second, 0.3, 0.08, false);
				tiles[BOTTOM_CENTER]->RestoreTile(bottomCenter.first, bottomCenter.second, 0.3, 0.08, false);
				tiles[TOP_CENTER]->ToggleTouch(false);
				tiles[MIDDLE_CENTER]->ToggleTouch(false);
				tiles[BOTTOM_CENTER]->ToggleTouch(false);}));

			runGroup2a = false;
			runGroup2b = true;
			sceneTimer.SetMark(msLeft / 2);
		}

		else if (runGroup2b && sceneTimer.GetElapsedTime() >= sceneTimer.GetMark())
		{
			tiles[TOP_CENTER]->runAction(
				cocos2d::CallFunc::create([this]() {
				tiles[TOP_CENTER]->HideShape();
				tiles[MIDDLE_CENTER]->HideShape();
				tiles[BOTTOM_CENTER]->HideShape();}));

			runGroup2b = false;
			runGroup3a = true;
		}

		//SHOW LAST TILES
		if (runGroup3a)
		{
			tiles[TOP_RIGHT]->runAction(
				cocos2d::CallFunc::create([this]() {
				//Move first colum into place over 150 ms, starting at 0 ms
				tiles[TOP_RIGHT]->RestoreTile(topRight.first, topRight.second, 0.3, 0.3, false);
				tiles[MIDDLE_RIGHT]->RestoreTile(middleRight.first, middleRight.second, 0.3, 0.3, false);
				tiles[BOTTOM_RIGHT]->RestoreTile(bottomRight.first, bottomRight.second, 0.3, 0.3, false);
				tiles[TOP_RIGHT]->ToggleTouch(false);
				tiles[MIDDLE_RIGHT]->ToggleTouch(false);
				tiles[BOTTOM_RIGHT]->ToggleTouch(false);}));

			runGroup3a = false;
			runGroup3b = true;
			sceneTimer.SetMark(msLeft);
		}

		else if (runGroup3b && sceneTimer.GetElapsedTime() >= sceneTimer.GetMark())
		{
			tiles[TOP_RIGHT]->runAction(
				cocos2d::CallFunc::create([this]() {
				tiles[TOP_RIGHT]->HideShape();
				tiles[MIDDLE_RIGHT]->HideShape();
				tiles[BOTTOM_RIGHT]->HideShape();
				tiles[TOP_LEFT]->ToggleTouch(true);
				tiles[MIDDLE_LEFT]->ToggleTouch(true);
				tiles[BOTTOM_LEFT]->ToggleTouch(true);
				tiles[TOP_CENTER]->ToggleTouch(true);
				tiles[MIDDLE_CENTER]->ToggleTouch(true);
				tiles[BOTTOM_CENTER]->ToggleTouch(true);
				tiles[TOP_RIGHT]->ToggleTouch(true);
				tiles[MIDDLE_RIGHT]->ToggleTouch(true);
				tiles[BOTTOM_RIGHT]->ToggleTouch(true);}));

			runGroup3b = false;
			specialRestoreFinished = true;
		}

		break;
	case 3: //Slide columns down from top, starting with left column

		//SHOW FIRST TILES
		if (runGroup1a)
		{
			tiles[TOP_LEFT]->runAction(
				cocos2d::CallFunc::create([this]() {

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
				tiles[TOP_LEFT]->RestoreTile(topLeft.first, topLeft.second, 0.15, 0, false);
				tiles[MIDDLE_LEFT]->RestoreTile(middleLeft.first, middleLeft.second, 0.15, 0, false);
				tiles[BOTTOM_LEFT]->RestoreTile(bottomLeft.first, bottomLeft.second, 0.15, 0, false);
				tiles[TOP_LEFT]->ToggleTouch(false);
				tiles[MIDDLE_LEFT]->ToggleTouch(false);
				tiles[BOTTOM_LEFT]->ToggleTouch(false);
			}));

			runGroup1a = false;
			runGroup1b = true;
			sceneTimer.SetMark(msLeft / 3);
		}

		else if (runGroup1b && sceneTimer.GetElapsedTime() >= sceneTimer.GetMark())
		{
			tiles[TOP_LEFT]->runAction(
				cocos2d::CallFunc::create([this]() {
				tiles[TOP_LEFT]->HideShape();
				tiles[MIDDLE_LEFT]->HideShape();
				tiles[BOTTOM_LEFT]->HideShape();}));

			runGroup1b = false;
			runGroup2a = true;
		}

		//SHOW SECOND TILES
		if (runGroup2a)
		{
			tiles[TOP_CENTER]->runAction(
				cocos2d::CallFunc::create([this]() {
				//Move first colum into place over 150 ms, starting at 0 ms
				tiles[TOP_CENTER]->RestoreTile(topCenter.first, topCenter.second, 0.3, 0.08, false);
				tiles[MIDDLE_CENTER]->RestoreTile(middleCenter.first, middleCenter.second, 0.3, 0.08, false);
				tiles[BOTTOM_CENTER]->RestoreTile(bottomCenter.first, bottomCenter.second, 0.3, 0.08, false);
				tiles[TOP_CENTER]->ToggleTouch(false);
				tiles[MIDDLE_CENTER]->ToggleTouch(false);
				tiles[BOTTOM_CENTER]->ToggleTouch(false);}));

			runGroup2a = false;
			runGroup2b = true;
			sceneTimer.SetMark(msLeft / 2);
		}

		else if (runGroup2b && sceneTimer.GetElapsedTime() >= sceneTimer.GetMark())
		{
			tiles[TOP_CENTER]->runAction(
				cocos2d::CallFunc::create([this]() {
				tiles[TOP_CENTER]->HideShape();
				tiles[MIDDLE_CENTER]->HideShape();
				tiles[BOTTOM_CENTER]->HideShape();}));

			runGroup2b = false;
			runGroup3a = true;
		}

		//SHOW LAST TILES
		if (runGroup3a)
		{
			tiles[TOP_RIGHT]->runAction(
				cocos2d::CallFunc::create([this]() {
				//Move first colum into place over 150 ms, starting at 0 ms
				tiles[TOP_RIGHT]->RestoreTile(topRight.first, topRight.second, 0.3, 0.3, false);
				tiles[MIDDLE_RIGHT]->RestoreTile(middleRight.first, middleRight.second, 0.3, 0.3, false);
				tiles[BOTTOM_RIGHT]->RestoreTile(bottomRight.first, bottomRight.second, 0.3, 0.3, false);
				tiles[TOP_RIGHT]->ToggleTouch(false);
				tiles[MIDDLE_RIGHT]->ToggleTouch(false);
				tiles[BOTTOM_RIGHT]->ToggleTouch(false);}));

			runGroup3a = false;
			runGroup3b = true;
			sceneTimer.SetMark(msLeft);
		}

		else if (runGroup3b && sceneTimer.GetElapsedTime() >= sceneTimer.GetMark())
		{
			tiles[TOP_RIGHT]->runAction(
				cocos2d::CallFunc::create([this]() {
				tiles[TOP_RIGHT]->HideShape();
				tiles[MIDDLE_RIGHT]->HideShape();
				tiles[BOTTOM_RIGHT]->HideShape();
				tiles[TOP_LEFT]->ToggleTouch(true);
				tiles[MIDDLE_LEFT]->ToggleTouch(true);
				tiles[BOTTOM_LEFT]->ToggleTouch(true);
				tiles[TOP_CENTER]->ToggleTouch(true);
				tiles[MIDDLE_CENTER]->ToggleTouch(true);
				tiles[BOTTOM_CENTER]->ToggleTouch(true);
				tiles[TOP_RIGHT]->ToggleTouch(true);
				tiles[MIDDLE_RIGHT]->ToggleTouch(true);
				tiles[BOTTOM_RIGHT]->ToggleTouch(true);}));

			runGroup3b = false;
			specialRestoreFinished = true;
		}

		break;
	case 4: //Slide rows down from top

		//SHOW FIRST TILES
		if (runGroup1a)
		{
			tiles[TOP_LEFT]->runAction(
				cocos2d::CallFunc::create([this]() {

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
				tiles[TOP_LEFT]->RestoreTile(topLeft.first, topLeft.second, 0.15, 0, false);
				tiles[TOP_CENTER]->RestoreTile(topCenter.first, topCenter.second, 0.15, 0, false);
				tiles[TOP_RIGHT]->RestoreTile(topRight.first, topRight.second, 0.15, 0, false);
				tiles[TOP_LEFT]->ToggleTouch(false);
				tiles[TOP_CENTER]->ToggleTouch(false);
				tiles[TOP_RIGHT]->ToggleTouch(false);}));

			runGroup1a = false;
			runGroup1b = true;
			sceneTimer.SetMark(msLeft / 3);
		}

		else if (runGroup1b && sceneTimer.GetElapsedTime() >= sceneTimer.GetMark())
		{
			tiles[TOP_LEFT]->runAction(
				cocos2d::CallFunc::create([this]() {
				tiles[TOP_LEFT]->HideShape();
				tiles[TOP_CENTER]->HideShape();
				tiles[TOP_RIGHT]->HideShape();}));

			runGroup1b = false;
			runGroup2a = true;
		}

		//SHOW SECOND TILES
		if (runGroup2a)
		{
			tiles[MIDDLE_LEFT]->runAction(
				cocos2d::CallFunc::create([this]() {
				//Move middle row into place over 300 ms, starting at 80 ms
				tiles[MIDDLE_LEFT]->RestoreTile(middleLeft.first, middleLeft.second, 0.3, 0.08, false);
				tiles[MIDDLE_CENTER]->RestoreTile(middleCenter.first, middleCenter.second, 0.3, 0.08, false);
				tiles[MIDDLE_RIGHT]->RestoreTile(middleRight.first, middleRight.second, 0.3, 0.08, false);
				tiles[MIDDLE_LEFT]->ToggleTouch(false);
				tiles[MIDDLE_CENTER]->ToggleTouch(false);
				tiles[MIDDLE_RIGHT]->ToggleTouch(false);}));

			runGroup2a = false;
			runGroup2b = true;
			sceneTimer.SetMark(msLeft / 2);
		}

		else if (runGroup2b && sceneTimer.GetElapsedTime() >= sceneTimer.GetMark())
		{
			tiles[MIDDLE_LEFT]->runAction(
				cocos2d::CallFunc::create([this]() {
				tiles[MIDDLE_LEFT]->HideShape();
				tiles[MIDDLE_CENTER]->HideShape();
				tiles[MIDDLE_RIGHT]->HideShape();}));

			runGroup2b = false;
			runGroup3a = true;
		}

		//SHOW LAST TILES
		if (runGroup3a)
		{
			tiles[BOTTOM_LEFT]->runAction(
				cocos2d::CallFunc::create([this]() {
				//Move bottom row into place over 300 ms, starting at 250 ms
				tiles[BOTTOM_LEFT]->RestoreTile(bottomLeft.first, bottomLeft.second, 0.3, 0.25, false);
				tiles[BOTTOM_CENTER]->RestoreTile(bottomCenter.first, bottomCenter.second, 0.3, 0.25, false);
				tiles[BOTTOM_RIGHT]->RestoreTile(bottomRight.first, bottomRight.second, 0.3, 0.25, false);
				tiles[BOTTOM_LEFT]->ToggleTouch(false);
				tiles[BOTTOM_CENTER]->ToggleTouch(false);
				tiles[BOTTOM_RIGHT]->ToggleTouch(false);}));

			runGroup3a = false;
			runGroup3b = true;
			sceneTimer.SetMark(msLeft);
		}

		else if (runGroup3b && sceneTimer.GetElapsedTime() >= sceneTimer.GetMark())
		{
			tiles[BOTTOM_LEFT]->runAction(
				cocos2d::CallFunc::create([this]() {
				tiles[BOTTOM_LEFT]->HideShape();
				tiles[BOTTOM_CENTER]->HideShape();
				tiles[BOTTOM_RIGHT]->HideShape();
				tiles[TOP_LEFT]->ToggleTouch(true);
				tiles[MIDDLE_LEFT]->ToggleTouch(true);
				tiles[BOTTOM_LEFT]->ToggleTouch(true);
				tiles[TOP_CENTER]->ToggleTouch(true);
				tiles[MIDDLE_CENTER]->ToggleTouch(true);
				tiles[BOTTOM_CENTER]->ToggleTouch(true);
				tiles[TOP_RIGHT]->ToggleTouch(true);
				tiles[MIDDLE_RIGHT]->ToggleTouch(true);
				tiles[BOTTOM_RIGHT]->ToggleTouch(true);}));

			runGroup3b = false;
			specialRestoreFinished = true;
		}

		break;
	}

	if (specialRestoreFinished)
	{
		specialRestore = false;
		pattern++;

		for (int iii = 0; iii < tiles.size(); iii++)
			tiles[iii]->ToggleTouch(true);

		enablePowerups = true;
		retry->ToggleOn(false);
		sceneTimer.ResetMark();

		//If we're coming back on app resume, repeat bonuses used
		if (reuseBonuses)
		{
			ReuseBonuses();
			reuseBonuses = false;
		}
	}

	return;
}

void MemoryMode::UpdateTimer()
{
	//Get how much time has passed in ms
	msLeft -= countdownTimer.GetElapsedTime();

	if(!countdownTimer.IsPaused())
		countdownTimer.Restart();

	if (msLeft <= warningTime && warningTime > 0)
	{
		audio->PlayClip("warning_tone");
		warningTime -= 1000;
	}

	//Set clock display to 0 after time is up
	if (msLeft <= 0)
	{
		if (timerText->GetLabel()->getOpacity() == 255)
		{
			timerText->setString("00.00");
			timerText->Center();
			timerText->SetBottom(timeRemainingBottomPosition);
			timerText->GetLabel()->setOpacity(254); //Hack it so that opacity is less than 255 before beginning fadeout action to prevent entering this loop repeatedly.
			timerText->GetLabel()->runAction(cocos2d::FadeOut::create(0.5));
			countdownTimer.Reset();
			countdownTimer.Pause();
			retry->ToggleOn(false);
			pointsText->GetLabel()->runAction(cocos2d::FadeIn::create(0.25));

			//Disable Time Plus bonus to prevent user from using it and "time up" nullifying it
			if (bonusTimePlusLeft > 0)
			{
				timePlusTile->setOpacity(64);
				timePlusText->GetLabel()->runAction(cocos2d::FadeTo::create(0.25, 64));
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
	if (sec < 10)
		sprintf(sz, "0%d", sec);
	else
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

	return;
}

void MemoryMode::RestartTimers()
{
	msLeft = timelimit + timePlusUsedOnCurrentCard * 10000;
	countdownTimer.Restart();
	sceneTimer.Restart();
	sceneTimer.SetMark(0);

	return;
}

void MemoryMode::CalculateScore()
{
	finalScore = (matches * matchMultiplier) + extraPoints;
	displayedScore = finalScore;

	return;
}

void MemoryMode::ResetScore()
{
	matches = startingMatches;
	extraPoints = 0;
	earnedTimePlusBonuses = 0;
	earnedEliminateBonuses = 0;
	earnedShowMatchBonuses = 0;
	earnedNewCardBonuses = 0;
	return;
}

void MemoryMode::UseBonusTimePlus()
{
	if (timePlusUsedOnCurrentCard < timePlusUseLimit)
	{
		msLeft += 10000;

		if (!specialRestore)
			sceneTimer.SetMark(msLeft);
		else if (specialRestore)
		{
			//Need to divide new time by all 3 sets of tiles fading in
			if (runGroup1a || runGroup1b)
				sceneTimer.AddTimeToMark(10000 / 3);

			//Need to divide time by 2 remaining sets of tiles fading in
			else if (runGroup2a || runGroup2b)
				sceneTimer.AddTimeToMark(10000 / 2);

			//Give all added time to last set of tiles fading in
			else if (runGroup3a || runGroup3b)
				sceneTimer.AddTimeToMark(10000);
		}

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

void MemoryMode::UseNewCard()
{
	currentSelection = 1;
	SetMatchParameters(matchCriteria);
	msLeft = timelimit;
	timerText->GetLabel()->runAction(cocos2d::FadeIn::create(0.5));
	warningTime = 3000;
	timePlusUsedOnCurrentCard = 0;
	matchedTilesShowing = 0;
	eliminateUsedOnCurrentCard = 0;
	excludedEliminateTiles.clear();

	for (int iii = 0; iii < tiles.size(); iii++)
	{
		tiles[iii]->DismissTile();
		tiles[iii]->ToggleTouch(false);
	}

	//Only reveal match rule after shapes disasppear
	if (difficulty < 40 || (difficulty >= 50 && difficulty <= 59) || (difficulty >= 70 && difficulty <= 79) || (difficulty >= 90))
	{
		matchCriteriaText->GetLabel()->stopAllActions();
		UpdateHeaderText("Memorize...");
	}

	if (matches <= 9)
	{
		matchCriteriaText->GetLabel()->runAction(cocos2d::Sequence::create(
			cocos2d::DelayTime::create(2),
			cocos2d::CallFunc::create([this]() {
			UpdateHeaderText(matchCriteria);}),
			NULL));
	}

	if (specialRestore)
		UpdateHeaderText(matchCriteria);

	RandomizeTiles();
	randomizeTiles = false;
	restoreTiles = true;
	sceneTimer.SetMark(500);
	disablePowerups = true;

	bonusesUsed++;
	gameData->setIntegerForKey(new_card_used, gameData->getIntegerForKey(new_card_used, 0) + 1);

	return;
}

void MemoryMode::UseBonusEliminate()
{
	//Prevent exceeding limit of how many tiles can logically be eliminated
	if (excludedEliminateTiles.size() == tiles.size())
		return;

	bonusesUsed++;
	eliminateUsedOnCurrentCard++;
	gameData->setIntegerForKey(eliminate_used, gameData->getIntegerForKey(eliminate_used, 0) + 1);

	//For matches 70-79 need to eliminate any non-unique matching tiles (since there could be duplicates)
	if (difficulty >= 70 && difficulty <= 79)
	{
		//Exclude all unique matching tiles
		if (excludedEliminateTiles.size() == 0)
		{
			excludedEliminateTiles.emplace(matchCriteriaOrder[0]);
			excludedEliminateTiles.emplace(matchCriteriaOrder[1]);
			excludedEliminateTiles.emplace(matchCriteriaOrder[2]);
		}

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
	}
		
	else
		Game::UseBonusEliminate();

	return;
}

void MemoryMode::UseShowMatch()
{
	if (matchedTilesShowing == 3)
		return;

	//Need  to reveal matches using matchCriteriaOrder for difficulty 70-79
	if (difficulty >= 70 && difficulty <= 79)
	{
		tiles[matchCriteriaOrder[matchedTilesShowing]]->stopAllActions();
		tiles[matchCriteriaOrder[matchedTilesShowing]]->SetFade(128, true);

		matchedTilesShowing++;
		bonusesUsed++;
		gameData->setIntegerForKey(show_match_used, gameData->getIntegerForKey(show_match_used, 0) + 1);

		//Disable item if all three matches are showing
		if (matchedTilesShowing == 3)
		{
			showMatchTile->runAction(cocos2d::FadeTo::create(0.25, 64));
			showMatchText->GetLabel()->runAction(cocos2d::FadeTo::create(0.25, 64));
		}
	}

	else
		Game::UseShowMatch();

	return;
}

void MemoryMode::CheckForNewBests()
{
	CalculateScore();
	//Check for new best score
	if (finalScore > highestScore)
	{
		bestScoreIcon->setVisible(true);
		bestScoreIcon->runAction(cocos2d::FadeIn::create(0.25));
		highestScore = finalScore;
		gameData->setIntegerForKey(best_score_memory, highestScore);
	}

	//Check for new best match
	if (matches > highestMatch)
	{
		bestMatchIcon->setVisible(true);
		bestMatchIcon->runAction(cocos2d::FadeIn::create(0.25));
		highestMatch = matches;
		gameData->setIntegerForKey(best_match_memory, highestMatch);
	}

	gameData->setIntegerForKey(total_points, gameData->getIntegerForKey(total_points, 0) + finalScore);
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	sdkbox::PluginSdkboxPlay::submitScore("high_score_memory", gameData->getIntegerForKey(best_score_memory, highestScore));
	sdkbox::PluginSdkboxPlay::submitScore("highest_match_memory", gameData->getIntegerForKey(best_match_memory, highestMatch));
	sdkbox::PluginSdkboxPlay::submitScore("total_score", gameData->getIntegerForKey(total_points, 0));
	sdkbox::PluginSdkboxPlay::submitScore("total_matches", gameData->getIntegerForKey(total_matches, 0));
#endif
	return;
}

void MemoryMode::DisplayMatchSummary()
{
	std::vector<std::string> matchMadeListStrings;
	std::string matchPoints;
	std::string nextBonus;

	sprintf(sz, "Match +100");
	matchPoints.append(sz);
	matchMadeListStrings.push_back(matchPoints);

	sprintf(sz, "Bonus in %d", matchMilestone - matches);
	nextBonus.append(sz);
	matchMadeListStrings.push_back(nextBonus);

	matchMadeList->ChangeList(matchMadeListStrings);

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

bool MemoryMode::CheckForAchievements()
{
	bool earnedAllAchievements = Game::CheckForAchievements();

    //Ignore other achievements if testing
    if(testing && testingAchievements)
        return true;

	if (!gameData->getBoolForKey(reach_match_10_memory, false))
	{
		if (matches >= 10)
		{
			gameData->setBoolForKey(reach_match_10_memory, true);
			achievementsUnlocked++;
			achievementsText.push_back(nrgText::create("Reach 10 Matches (Memory)\n\nYou see? It's possible when you put your mind to it!", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));
		}
		else
			earnedAllAchievements = false;
	}
 
    if (!gameData->getBoolForKey(reach_match_20_memory, false))
	{
		if (matches >= 20)
		{
			gameData->setBoolForKey(reach_match_20_memory, true);
			achievementsUnlocked++;
			achievementsText.push_back(nrgText::create("Reach 20 Matches (Memory)\n\nYou've got a knack for this!", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));
		}
		else
			earnedAllAchievements = false;
	}

    if (!gameData->getBoolForKey(reach_match_30_memory, false))
	{
		if (matches >= 30)
		{
			gameData->setBoolForKey(reach_match_30_memory, true);
			achievementsUnlocked++;
			achievementsText.push_back(nrgText::create("Reach 30 Matches (Memory)\n\nMy mind can't comprehend what your mind is doing!", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));
		}
		else
			earnedAllAchievements = false;
	}

	if (!gameData->getBoolForKey(reach_match_50_memory, false))
	{
		if (matches >= 50)
		{
			gameData->setBoolForKey(reach_match_50_memory, true);
			achievementsUnlocked++;
			achievementsText.push_back(nrgText::create("Reach 50 Matches (Memory)\n\nRemember when this seemed impossible? (That's a memory-themed joke).", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));
		}
		else
			earnedAllAchievements = false;
	}
    
    if (!gameData->getBoolForKey(reach_match_60_memory, false))
	{
		if (matches >= 60)
		{
			gameData->setBoolForKey(reach_match_60_memory, true);
			achievementsUnlocked++;
			achievementsText.push_back(nrgText::create("Reach 60 Matches (Memory)\n\nOk... You don't have a photographic memory, do you?", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));
		}
		else
			earnedAllAchievements = false;
	}

	if (!gameData->getBoolForKey(reach_match_70_memory, false))
	{
		if (matches >= 70)
		{
			gameData->setBoolForKey(reach_match_70_memory, true);
			achievementsUnlocked++;
			achievementsText.push_back(nrgText::create("Reach 70 Matches (Memory)\n\nWhew. Doesn't your head hurt? Of course, not. You're some kind of genius...", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));
		}
		else
			earnedAllAchievements = false;
	}

	if (!gameData->getBoolForKey(reach_match_80_memory, false))
	{
		if (matches >= 80)
		{
			gameData->setBoolForKey(reach_match_80_memory, true);
			achievementsUnlocked++;
			achievementsText.push_back(nrgText::create("Reach 80 Matches (Memory)\n\nYou're definitely one of those people who remembers what they had for breakfast.", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));
		}
		else
			earnedAllAchievements = false;
	}

	if (!gameData->getBoolForKey(reach_match_90_memory, false))
	{
		if (matches >= 90)
		{
			gameData->setBoolForKey(reach_match_90_memory, true);
			achievementsUnlocked++;
			achievementsText.push_back(nrgText::create("Reach 90 Matches (Memory)\n\nI guess you CAN do anything when you put your mind to it!", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));
		}
		else
			earnedAllAchievements = false;
	}

	if (!gameData->getBoolForKey(no_bonus_items_used_first_50_matches_memory, false))
	{
		if (noBonusesUsedBefore50Matches)
		{
			gameData->setBoolForKey(no_bonus_items_used_first_50_matches_memory, true);
			achievementsUnlocked++;
			achievementsText.push_back(nrgText::create("No Assistance Necessary (Memory): Get to 50 Matches in Memory Without Using Bonus Items\n\nImpressive. That kind of restraint doesn't go unnoticed!", "fonts/POE.ttf", achievementTextSize - 1, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));
		}
		else
			earnedAllAchievements = false;
	}

    if (!gameData->getBoolForKey(reach_match_100_memory, false))
    {
        if (matches >= 100)
        {
			gameData->setBoolForKey(reach_match_100_memory, true);
            achievementsUnlocked++;
            achievementsUnlocked++; //Need extra one for second page of text
            achievementsText.push_back(nrgText::create("Mastered Memory: Reach 100 Matches\n\nCongratulations! You've completed 100 Memory Matches! Your mind is a force to be reckoned with!", "fonts/POE.ttf", achievementTextSize - 1, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));
            achievementsText.push_back(nrgText::create("Mastered Memory: Reach 100 Matches\n\nThank you soooo-oooo much for enjoying Tap 3!!! We hope you cherish the memories!", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));
        }
        else
            earnedAllAchievements = false;
    }
    
    //Check for Platinum Trophy
	if (!gameData->getBoolForKey(earned_all_achievements, false))
	{
		if (earnedAllAchievements)
		{
			gameData->setBoolForKey(earned_all_achievements, true);
			achievementsUnlocked++;
			achievementsText.push_back(nrgText::create("Tap 3 Platinum Trophy: You've Earned Every Achievement!\n\nCongratulations! You're in a league of your own! Thank you so much for enjoying Tap 3!!!", "fonts/POE.ttf", achievementTextSize - 1, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));
		}
		else
			earnedAllAchievements = false;
	}

	for (int iii = 0; iii < achievementsUnlocked; iii++)
	{
		addChild(achievementsText[iii]);
		achievementsText[iii]->GetLabel()->setOpacity(0);
	}

	//Checks earned achievements against GameCenter and GooglePlay if not already submitted
	SyncAchievements();

	return earnedAllAchievements;
}

void MemoryMode::RandomizeTiles()
{
	std::vector<int> matchingShapesVector; //The 3 matching tiles for shapes
	std::vector<int> matchingColorsVector; //The 3 matching tiles for colors
	std::vector<int> matchingStylesVector; //The 3 matching tiles for styles

	//Reset tiles and matches
	excludedTiles.clear();
	matchingTiles.clear();
	matchingTilesVector.clear();
	excludedColorPairs.clear();
	excludedStylePairs.clear();
	excludedColors.clear();
	excludedShapes.clear();
	excludedStyles.clear();
	nextHeaderStringVector.clear();
	matchCriteriaOrder.clear();

	if (matches <= 100)
	{
		excludedStyles.emplace(OUTLINE_BLINDS_QUAD);
		excludedStyles.emplace(CONCENTRIC_BLINDS_QUAD);
	}

	int randShape; //Randomly generated shape
	int randColorTop; //Randomly generated color top
	int randColorBottom; //Randomly generated color bottom
	int randStyleTop; //Randomly generated style top
	int randStyleBottom; //Randoml generated style bottom
	std::pair<int, int> randColorPairKey; //Color pair for randomly generated colors
	std::pair<int, int> randStylePairKey; //Style pair for randomly generated styles

	int matchShape; //The shape for the 3 matching shape tiles
	int matchColorTop; //The top color for the 3 matching color tiles
	int matchColorBottom; //The bottom color for the 3 matching color tiles
	int matchStyleTop; //The top style for the 3 matching style tiles
	int matchStyleBottom; //The bottom style for the 3 matching style tiles
	std::pair<int, int> matchColorPairKey; //The key for the matching colors pair
	std::pair<int, int> matchStylePairKey; //The key for the matching styles pair
	std::pair<cocos2d::Color3B, cocos2d::Color3B> colorPair; //Color pair to convert colorKeys into usable color pairs

	std::vector<int> shapesUsed; //Which shapes have been used
	std::vector<std::pair<int, int>> colorKeysUsed; //Which color keys have been used
	std::vector<std::pair<int, int>> styleKeysUsed; //Which style keys have been used

	//4 of same shape. 4 of same color. 1 random. 
	if (difficulty < 20)
	{
		//First get the match shape or color
		matchShape = nrgFunctions::GetRandom(CIRCLE, OCTAGON, excludedShapes);
		matchColorTop = nrgFunctions::GetRandom(RED, ORANGE, excludedColors);
		matchColorBottom = matchColorTop;
		
		excludedShapes.emplace(matchShape);
		excludedColors.emplace(matchColorTop);

		//Set Matching Tiles
		for (int iii = 0; iii < numberOfMatchingTiles; iii++)
		{
			int randTile = nrgFunctions::GetRandom(TOP_LEFT, BOTTOM_RIGHT, excludedTiles);
			excludedTiles.emplace(randTile);
			matchingTiles.emplace(randTile);
			matchingTilesVector.push_back(randTile);

			if (matchCriteria != SHAPES)
				randShape = nrgFunctions::GetRandom(CIRCLE, OCTAGON, excludedShapes);
			else
				randShape = matchShape;

			if (matchCriteria != COLORS)
			{
				randColorTop = nrgFunctions::GetRandom(RED, ORANGE, excludedColors);
				randColorBottom = randColorTop;
				colorKey.first = randColorTop;
				colorKey.second = randColorBottom;
			}
			else
			{
				colorKey.first = matchColorTop;
				colorKey.second = matchColorBottom;
			}

			colorPair = GetColorFromKey(colorKey);
			tiles[randTile]->ChangeShape(randShape, colorPair);

			shapesUsed.push_back(randShape);
			colorKeysUsed.push_back(colorKey);


			int shapeUseFrequency = 0;
			int colorUseFrequency = 0;

			//Prevent non match shapes from exceeding 2
			for (int jjj = 0; jjj < shapesUsed.size(); jjj++)
			{
				if (randShape == shapesUsed[jjj])
					shapeUseFrequency++;

				if (shapeUseFrequency >= 2)
					excludedShapes.emplace(randShape);
			}

			//Prevent non match colors from exceeding 2
			for (int nnn = 0; nnn < colorKeysUsed.size(); nnn++)
			{
				if (colorKey.first == colorKeysUsed[nnn].first)
					colorUseFrequency++;

				if (colorUseFrequency >= 2)
					excludedColors.emplace(colorKey.first);
			}
		}

		//Set the 4 Non Matching tiles of the same shape/color
		for (int iii = 0; iii < tiles.size() - numberOfMatchingTiles - 1; iii++)
		{
			int randTile = nrgFunctions::GetRandom(TOP_LEFT, BOTTOM_RIGHT, excludedTiles);
			excludedTiles.emplace(randTile);
			
			//Get random shape for non matching tiles if match criteria is shapes
			if (matchCriteria == SHAPES)
				randShape = nrgFunctions::GetRandom(CIRCLE, OCTAGON, excludedShapes);
			
			//Use same shape for non matching tiles if match criteria is colors
			else
				randShape = matchShape;

			//Get random color for non matching tiles if match criteria is colors
			if (matchCriteria == COLORS)
			{
				randColorTop = nrgFunctions::GetRandom(RED, ORANGE, excludedColors);
				randColorBottom = randColorTop;
				colorKey.first = randColorTop;
				colorKey.second = randColorBottom;
			}

			//Use same color for non matching tiles if match criteria is shapes
			else
			{
				colorKey.first = matchColorTop;
				colorKey.second = matchColorBottom;
			}

			colorPair = GetColorFromKey(colorKey);
			tiles[randTile]->ChangeShape(randShape, colorPair);

			shapesUsed.push_back(randShape);
			colorKeysUsed.push_back(colorKey);


			int shapeUseFrequency = 0;
			int colorUseFrequency = 0;

			//Prevent non match shapes from exceeding 2
			for (int jjj = 0; jjj < shapesUsed.size(); jjj++)
			{
				if (randShape == shapesUsed[jjj])
					shapeUseFrequency++;

				if (shapeUseFrequency >= 2)
					excludedShapes.emplace(randShape);
			}

			//Prevent non match colors from exceeding 2
			for (int nnn = 0; nnn < colorKeysUsed.size(); nnn++)
			{
				if (colorKey.first == colorKeysUsed[nnn].first)
					colorUseFrequency++;

				if (colorUseFrequency >= 2)
					excludedColors.emplace(colorKey.first);
			}
		}

		//Set the last remaining tile
		int randTile = nrgFunctions::GetRandom(TOP_LEFT, BOTTOM_RIGHT, excludedTiles);
		excludedTiles.emplace(randTile);

		randShape = nrgFunctions::GetRandom(CIRCLE, OCTAGON, excludedShapes);
		randColorTop = nrgFunctions::GetRandom(RED, ORANGE, excludedColors);
		randColorBottom = randColorTop;
		colorKey.first = randColorTop;
		colorKey.second = randColorBottom;
		colorPair = GetColorFromKey(colorKey);

		tiles[randTile]->ChangeShape(randShape, colorPair);
	}

	//3 Color Matches. 3 Shape Matches. 3 Style Matches. All mutually exclusive
	else if ( (difficulty >= 20 && difficulty <= 29) || (difficulty >= 40 && difficulty <= 49) || (difficulty >= 60 && difficulty <= 69) || (difficulty >= 80 && difficulty <= 89) )
	{
		//Randomize which tiles are the shapes, colors, and styles
		for (int iii = 0; iii < 3; iii++)
		{
			matchingShapesVector.push_back(nrgFunctions::GetRandom(TOP_LEFT, BOTTOM_RIGHT, excludedTiles));
			excludedTiles.emplace(matchingShapesVector[iii]);

			matchingColorsVector.push_back(nrgFunctions::GetRandom(TOP_LEFT, BOTTOM_RIGHT, excludedTiles));
			excludedTiles.emplace(matchingColorsVector[iii]);

			matchingStylesVector.push_back(nrgFunctions::GetRandom(TOP_LEFT, BOTTOM_RIGHT, excludedTiles));
			excludedTiles.emplace(matchingStylesVector[iii]);
		}

		//Set which tiles are the matching tiles
		switch (matchCriteria)
		{
		case COLORS:
			for (int iii = 0; iii < 3; iii++)
			{
				matchingTiles.emplace(matchingColorsVector[iii]);
				matchingTilesVector.push_back(matchingColorsVector[iii]);
			}
			break;
		case SHAPES:
			for (int iii = 0; iii < 3; iii++)
			{
				matchingTiles.emplace(matchingShapesVector[iii]);
				matchingTilesVector.push_back(matchingShapesVector[iii]);
			}
			break;
		case STYLES:
			for (int iii = 0; iii < 3; iii++)
			{
				matchingTiles.emplace(matchingStylesVector[iii]);
				matchingTilesVector.push_back(matchingStylesVector[iii]);
			}
			break;
		}
	
		//Custom randomization code. Clear out previous code

		//First get matching shapes
		randShape = nrgFunctions::GetRandom(CIRCLE, OCTAGON, excludedShapes);
		matchShape = randShape;
		shapesUsed.push_back(matchShape);
		excludedShapes.emplace(matchShape);

		//Now get matching color
		randColorTop = nrgFunctions::GetRandom(0, 6);
		matchColorTop = randColorTop;

		//If not splitting color, make both top and bottom same color
		if (!splitColor)
			randColorBottom = randColorTop;
		else
			randColorBottom = nrgFunctions::GetRandom(0, 6);

		matchColorBottom = randColorBottom;

		//Exclude this color pair
		randColorPairKey.first = randColorTop;
		randColorPairKey.second = randColorBottom;
		matchColorPairKey = randColorPairKey;
		colorKeysUsed.push_back(matchColorPairKey);
		excludedColorPairs.emplace(matchColorPairKey);
		if (!splitColor)
			excludedColors.emplace(randColorTop);

		//Now get matching style
		randStyleTop = nrgFunctions::GetRandom(SOLID, CONCENTRIC_BLINDS_QUAD, excludedStyles);
		matchStyleTop = randStyleTop;

		//If not splitting style, make both top and bottom same style
		if (!splitStyle)
			randStyleBottom = randStyleTop;
		else
			randStyleBottom = nrgFunctions::GetRandom(SOLID, CONCENTRIC_BLINDS_QUAD, excludedStyles);

		matchStyleBottom = randStyleBottom;

		//Exclude this style pair
		randStylePairKey.first = randStyleTop;
		randStylePairKey.second = randStyleBottom;
		matchStylePairKey = randStylePairKey;
		styleKeysUsed.push_back(matchStylePairKey);
		excludedStylePairs.emplace(matchStylePairKey);
		if (!splitStyle)
			excludedStyles.emplace(randStyleTop);

		//Now create the shape tiles
		for (int iii = 0; iii < 3; iii++)
		{
			//Get color and style
			if (!splitColor)
			{
				randColorTop = nrgFunctions::GetRandom(0, 6, excludedColors);
				randColorBottom = randColorTop;
				randColorPairKey.first = randColorTop;
				randColorPairKey.second = randColorBottom;
			}
			else
				randColorPairKey = nrgFunctions::GetRandomPair(0, 6, 0, 6, excludedColorPairs);

			colorPair = GetColorFromKey(randColorPairKey);

			if (difficulty >= 40)
				excludedColors.emplace(randColorTop);

			if (!splitStyle)
			{
				randStyleTop = nrgFunctions::GetRandom(SOLID, CONCENTRIC_BLINDS_QUAD, excludedStyles);
				randStyleBottom = randStyleTop;
				randStylePairKey.first = randStyleTop;
				randStylePairKey.second = randStyleBottom;
			}
			else
				randStylePairKey = nrgFunctions::GetRandomPair(SOLID, CONCENTRIC_BLINDS_QUAD, SOLID, CONCENTRIC_BLINDS_QUAD, excludedStylePairs);
			
			if(difficulty >= 40)
				excludedStyles.emplace(randStyleTop);

			//Create tile
			tiles[matchingShapesVector[iii]]->ChangeShape(matchShape, colorPair, 0, randStylePairKey);

			//Add color and style to used set
			colorKeysUsed.push_back(randColorPairKey);
			styleKeysUsed.push_back(randStylePairKey);

			//Make sure these don't exceed two if not allowed
			if (!nonmatchedColorsCanExceedTwo)
			{
				int frequency = 0;

				//If used twice, it should be emplaced in the exclusion set
				for (int iii = 0; iii < colorKeysUsed.size(); iii++)
				{
					if (colorKeysUsed[iii] == randColorPairKey)
						frequency++;
				}

				if (frequency >= 2)
				{
					excludedColorPairs.emplace(randColorPairKey);
					excludedColors.emplace(randColorPairKey.first);
				}
			}

			if (!nonmatchedStylesCanExceedTwo)
			{
				int frequency = 0;

				//If used twice, it should be emplaced in the exclusion set
				for (int iii = 0; iii < styleKeysUsed.size(); iii++)
				{
					if (styleKeysUsed[iii] == randStylePairKey)
						frequency++;
				}

				if (frequency >= 2)
				{
					excludedStylePairs.emplace(randStylePairKey);
					excludedStyles.emplace(randStylePairKey.first);
				}
			}
		}

		//Decide if color or style tiles will have 2 of same shape
		int limitShapesToOneType = nrgFunctions::GetRandomChoice(COLORS, STYLES);

		//Now create the color tiles
		for (int iii = 0; iii < 3; iii++)
		{
			//Use match color
			colorPair = GetColorFromKey(matchColorPairKey);

			//Get shape and style
			randShape = nrgFunctions::GetRandom(CIRCLE, OCTAGON, excludedShapes);
			
			if (limitShapesToOneType == COLORS && difficulty >= 40)
				excludedShapes.emplace(randShape);

			if (!splitStyle)
			{
				randStyleTop = nrgFunctions::GetRandom(SOLID, CONCENTRIC_BLINDS_QUAD, excludedStyles);
				randStyleBottom = randStyleTop;
				randStylePairKey.first = randStyleTop;
				randStylePairKey.second = randStyleBottom;
			}
			else
				randStylePairKey = nrgFunctions::GetRandomPair(SOLID, CONCENTRIC_BLINDS_QUAD, SOLID, CONCENTRIC_BLINDS_QUAD, excludedStylePairs);

			if(difficulty >= 40)
				excludedStyles.emplace(randStyleTop);

			//Create tile
			tiles[matchingColorsVector[iii]]->ChangeShape(randShape, colorPair, 0, randStylePairKey);

			//Add shape and style to used set
			shapesUsed.push_back(randShape);
			styleKeysUsed.push_back(randStylePairKey);

			//Make sure these don't exceed two if not allowed
			if (!nonmatchedShapesCanExceedTwo)
			{
				int frequency = 0;

				//If used twice, it should be emplaced in the exclusion set
				for (int iii = 0; iii < shapesUsed.size(); iii++)
				{
					if (shapesUsed[iii] == randShape)
						frequency++;
				}

				if (frequency >= 2)
					excludedShapes.emplace(randShape);
			}

			if (!nonmatchedStylesCanExceedTwo)
			{
				int frequency = 0;

				//If used twice, it should be emplaced in the exclusion set
				for (int iii = 0; iii < styleKeysUsed.size(); iii++)
				{
					if (styleKeysUsed[iii] == randStylePairKey)
						frequency++;
				}

				if (frequency >= 2)
				{
					excludedStylePairs.emplace(randStylePairKey);
					excludedStyles.emplace(randStylePairKey.first);
				}
			}
		}

		//Now create the style tiles
		for (int iii = 0; iii < 3; iii++)
		{
			//Get shape and color
			randShape = nrgFunctions::GetRandom(CIRCLE, OCTAGON, excludedShapes);

			if (limitShapesToOneType == STYLES && excludedShapes.size() < 5 && difficulty >= 40)
				excludedShapes.emplace(randShape);

			if (!splitColor)
			{
				randColorTop = nrgFunctions::GetRandom(0, 6, excludedColors);
				randColorBottom = randColorTop;
				randColorPairKey.first = randColorTop;
				randColorPairKey.second = randColorBottom;
			}
			else
				randColorPairKey = nrgFunctions::GetRandomPair(0, 6, 0, 6, excludedColorPairs);

			if(difficulty >= 40)
				excludedColors.emplace(randColorTop);
			colorPair = GetColorFromKey(randColorPairKey);

			//Create tile
			tiles[matchingStylesVector[iii]]->ChangeShape(randShape, colorPair, 0, matchStylePairKey);

			//Add shape and style to used set
			shapesUsed.push_back(randShape);
			colorKeysUsed.push_back(randColorPairKey);

			//Make sure these don't exceed two if not allowed
			if (!nonmatchedShapesCanExceedTwo)
			{
				int frequency = 0;

				//If used twice, it should be emplaced in the exclusion set
				for (int iii = 0; iii < shapesUsed.size(); iii++)
				{
					if (shapesUsed[iii] == randShape)
						frequency++;
				}

				if (frequency >= 2)
					excludedShapes.emplace(randShape);
			}

			//Make sure these don't exceed two if not allowed
			if (!nonmatchedColorsCanExceedTwo)
			{
				int frequency = 0;

				//If used twice, it should be emplaced in the exclusion set
				for (int iii = 0; iii < colorKeysUsed.size(); iii++)
				{
					if (colorKeysUsed[iii] == randColorPairKey)
						frequency++;
				}

				if (frequency >= 2)
				{
					excludedColorPairs.emplace(randColorPairKey);
					excludedColors.emplace(randColorPairKey.first);
				}
			}
		}
	}

	//3 Shapes, different colors. 3 Colors, different shapes. 3 Shapes, same Color
	else if (difficulty >= 30 && difficulty <= 39)
	{
		int shape1; //The shape that appears six times
		shape1 = nrgFunctions::GetRandom(CIRCLE, OCTAGON, excludedShapes);
		excludedShapes.emplace(shape1);

		int shape2; //The shape for Color 1
		shape2 = nrgFunctions::GetRandom(CIRCLE, OCTAGON, excludedShapes);
		excludedShapes.emplace(shape2);

		int shape3; //The shape for Color 2
		shape3 = nrgFunctions::GetRandom(CIRCLE, OCTAGON, excludedShapes);
		excludedShapes.emplace(shape3);

		int shape4; //The shape for Color 3
		shape4 = nrgFunctions::GetRandom(CIRCLE, OCTAGON, excludedShapes);
		excludedShapes.emplace(shape4);

		int matchingColorValue = nrgFunctions::GetRandom(0, 6, excludedColors);
		excludedColors.emplace(matchingColorValue);
		matchColorPairKey.first = matchingColorValue;
		matchColorPairKey.second = matchingColorValue;
		std::pair<cocos2d::Color3B, cocos2d::Color3B> color1; //The color that appears 6 times
		color1 = GetColorFromKey(matchColorPairKey);
		
		std::pair<cocos2d::Color3B, cocos2d::Color3B> color2; //Color for Shape 1
		matchingColorValue = nrgFunctions::GetRandom(0, 6, excludedColors);
		excludedColors.emplace(matchingColorValue);
		matchColorPairKey.first = matchingColorValue;
		matchColorPairKey.second = matchingColorValue;
		color2 = GetColorFromKey(matchColorPairKey);

		std::pair<cocos2d::Color3B, cocos2d::Color3B> color3; //Color for Shape 2
		matchingColorValue = nrgFunctions::GetRandom(0, 6, excludedColors);
		excludedColors.emplace(matchingColorValue);
		matchColorPairKey.first = matchingColorValue;
		matchColorPairKey.second = matchingColorValue;
		color3 = GetColorFromKey(matchColorPairKey);

		std::pair<cocos2d::Color3B, cocos2d::Color3B> color4; //Color for Shape 3
		matchingColorValue = nrgFunctions::GetRandom(0, 6, excludedColors);
		excludedColors.emplace(matchingColorValue);
		matchColorPairKey.first = matchingColorValue;
		matchColorPairKey.second = matchingColorValue;
		color4 = GetColorFromKey(matchColorPairKey);

		std::vector<int> shapesOnly;
		std::vector<int> colorsOnly;
		std::vector<int> shapesAndColors;

		std::vector<int> shapeVector; //Holds shape2, shape3, and shape4 for Colors
		std::vector<std::pair<cocos2d::Color3B, cocos2d::Color3B>> colorVector; //Holds color2, color3, and color4 for Shapes

		shapeVector.push_back(shape2);
		shapeVector.push_back(shape3);
		shapeVector.push_back(shape4);

		colorVector.push_back(color2);
		colorVector.push_back(color3);
		colorVector.push_back(color4);

		//Randomly designate tiles
		for (int iii = 0; iii < 3; iii++)
		{
			shapesOnly.push_back(nrgFunctions::GetRandom(TOP_LEFT, BOTTOM_RIGHT, excludedTiles));
			excludedTiles.emplace(shapesOnly[iii]);
			tiles[shapesOnly[iii]]->ChangeShape(shape1, colorVector[iii]);
			
			colorsOnly.push_back(nrgFunctions::GetRandom(TOP_LEFT, BOTTOM_RIGHT, excludedTiles));
			excludedTiles.emplace(colorsOnly[iii]);
			tiles[colorsOnly[iii]]->ChangeShape(shapeVector[iii], color1);

			shapesAndColors.push_back(nrgFunctions::GetRandom(TOP_LEFT, BOTTOM_RIGHT, excludedTiles));
			excludedTiles.emplace(shapesAndColors[iii]);
			tiles[shapesAndColors[iii]]->ChangeShape(shape1, color1);
		}

		//Decide which tiles are the matching set
		switch (matchCriteria)
		{
		case COLORS:
			for (int iii = 0; iii < 3; iii++)
			{
				matchingTiles.emplace(colorsOnly[iii]);
				matchingTilesVector.push_back(colorsOnly[iii]);
				nextHeaderString = "Colors Only";
			}
			break;
		case SHAPES:
			for (int iii = 0; iii < 3; iii++)
			{
				matchingTiles.emplace(shapesOnly[iii]);
				matchingTilesVector.push_back(shapesOnly[iii]);
				nextHeaderString = "Shapes Only";
			}
			break;
		case COLORS_SHAPES:
			for (int iii = 0; iii < 3; iii++)
			{
				matchingTiles.emplace(shapesAndColors[iii]);
				matchingTilesVector.push_back(shapesAndColors[iii]);
				nextHeaderString = "Shapes + Colors";
			}
			break;
		}
	}

	//4 of one Shape. 5 of another Shape. 4 of one Color. 5 of another Color.
	else if (difficulty >= 50 && difficulty <= 59)
	{
		int shape1 = nrgFunctions::GetRandom(CIRCLE, OCTAGON, excludedShapes);
		excludedShapes.emplace(shape1);
		int shape2 = nrgFunctions::GetRandom(CIRCLE, OCTAGON, excludedShapes);
		excludedShapes.emplace(shape2);
		int color1 = nrgFunctions::GetRandom(0, 6, excludedColors);
		excludedColors.emplace(color1);
		int color2 = nrgFunctions::GetRandom(0, 6, excludedColors);
		excludedColors.emplace(color2);

		int shape1Frequency = 0;
		int shape2Frequency = 0;
		int color1Frequency = 0;
		int color2Frequency = 0;

		int randColor;

		std::vector<int> shape1Vector;
		std::vector<int> shape2Vector;
		std::vector<int> color1Vector;
		std::vector<int> color2Vector;

		for (int iii = 0; iii < tiles.size(); iii++)
		{
			//Randomly pick one of the 2 shapes
			if (shape1Frequency == 5)
			{
				randShape = shape2;
				shape2Vector.push_back(iii);
			}

			else if (shape2Frequency == 5)
			{
				randShape = shape1;
				shape1Vector.push_back(iii);
			}
			else
			{
				randShape = nrgFunctions::GetRandomChoice(1, 2);

				if (randShape == 1)
				{
					randShape = shape1;
					shape1Frequency++;
					shape1Vector.push_back(iii);
				}
				else if (randShape == 2)
				{
					randShape = shape2;
					shape2Frequency++;
					shape2Vector.push_back(iii);
				}
			}

			//Randomly pick one of the 2 colors
			if (color1Frequency == 5)
			{
				randColor = color2;
				color2Vector.push_back(iii);
			}
			else if (color2Frequency == 5)
			{
				randColor = color1;
				color1Vector.push_back(iii);
			}
			else
			{
				randColor = nrgFunctions::GetRandomChoice(1, 2);

				if (randColor == 1)
				{
					randColor = color1;
					color1Frequency++;
					color1Vector.push_back(iii);
				}
				else if (randColor == 2)
				{
					randColor = color2;
					color2Frequency++;
					color2Vector.push_back(iii);
				}
			}
			colorKey.first = randColor;
			colorKey.second = randColor;
			colorPair = GetColorFromKey(colorKey);

			tiles[iii]->ChangeShape(randShape, colorPair);
		}

		//Decide which tile set will be matching criteria
		int rand = nrgFunctions::GetRandom(0, 3);

		switch (rand)
		{
		case 0: //Shape 1
			matchCriteria = SHAPES;
			for (int iii = 0; iii < shape1Vector.size(); iii++)
			{
				matchingTiles.emplace(shape1Vector[iii]);
				matchingTilesVector.push_back(shape1Vector[iii]);
			}

			switch (shape1)
			{
			case CIRCLE:
				nextHeaderString = "Circles";
				break;
			case SQUARE:
				nextHeaderString = "Squares";
				break;
			case DIAMOND:
				nextHeaderString = "Diamonds";
				break;
			case TRIANGLE:
				nextHeaderString = "Triangles";
				break;
			case HEXAGON:
				nextHeaderString = "Hexagons";
				break;
			case OCTAGON:
				nextHeaderString = "Octagons";
				break;
			}
			break;
		case 1: //Color 1
			matchCriteria = COLORS;
			for (int iii = 0; iii < color1Vector.size(); iii++)
			{
				matchingTiles.emplace(color1Vector[iii]);
				matchingTilesVector.push_back(color1Vector[iii]);
			}

			switch (color1)
			{
			case RED:
				nextHeaderString = "Reds";
				break;
			case GREEN:
				nextHeaderString = "Greens";
				break;
			case BLUE:
				nextHeaderString = "Dark Blues";
				break;
			case YELLOW:
				nextHeaderString = "Yellows";
				break;
			case PURPLE:
				nextHeaderString = "Purples";
				break;
			case CYAN:
				nextHeaderString = "Light Blues";
				break;
			case ORANGE:
				nextHeaderString = "Oranges";
				break;
			}
			break;
		case 2: //Color 2
			matchCriteria = COLORS;
			for (int iii = 0; iii < color2Vector.size(); iii++)
			{
				matchingTiles.emplace(color2Vector[iii]);
				matchingTilesVector.push_back(color2Vector[iii]);
			}

			switch (color2)
			{
			case RED:
				nextHeaderString = "Reds";
				break;
			case GREEN:
				nextHeaderString = "Greens";
				break;
			case BLUE:
				nextHeaderString = "Dark Blues";
				break;
			case YELLOW:
				nextHeaderString = "Yellows";
				break;
			case PURPLE:
				nextHeaderString = "Purples";
				break;
			case CYAN:
				nextHeaderString = "Light Blues";
				break;
			case ORANGE:
				nextHeaderString = "Oranges";
				break;
			}
			break;
		case 3: //Shape 2
			matchCriteria = SHAPES;
			for (int iii = 0; iii < shape2Vector.size(); iii++)
			{
				matchingTiles.emplace(shape2Vector[iii]);
				matchingTilesVector.push_back(shape2Vector[iii]);
			}

			switch (shape2)
			{
			case CIRCLE:
				nextHeaderString = "Circles";
				break;
			case SQUARE:
				nextHeaderString = "Squares";
				break;
			case DIAMOND:
				nextHeaderString = "Diamonds";
				break;
			case TRIANGLE:
				nextHeaderString = "Triangles";
				break;
			case HEXAGON:
				nextHeaderString = "Hexagons";
				break;
			case OCTAGON:
				nextHeaderString = "Octagons";
				break;
			}
			break;
		}
	}

	//4 pairs of two shapes. One other shape. Pairs match color from 70-74
	else if (difficulty >= 70 && difficulty <= 79)
	{
		matchCriteria = SHAPES;

		std::vector<int> shapesVector;
		std::vector<int> colorsVector;

		//First, get the 4 random shape pairs
		for (int iii = 0; iii < 8; iii += 2)
		{
			shapesVector.push_back(nrgFunctions::GetRandom(CIRCLE, OCTAGON, excludedShapes));
			shapesVector.push_back(shapesVector[iii]);
			excludedShapes.emplace(shapesVector[iii]);
		}

		//Finally, get the 9th shape, which won't match any of the previous
		shapesVector.push_back(nrgFunctions::GetRandom(CIRCLE, OCTAGON, excludedShapes));

		//Do the same for colors
		for (int iii = 0; iii < 8; iii += 2)
		{
			colorsVector.push_back(nrgFunctions::GetRandom(RED, ORANGE, excludedColors));
			colorsVector.push_back(colorsVector[iii]);
			excludedColors.emplace(colorsVector[iii]);
		}

		//Finally, get the 9th color, which won't match any of the previous
		colorsVector.push_back(nrgFunctions::GetRandom(RED, ORANGE, excludedColors));


		//Randomize tiles
		int randTile;
		std::unordered_set<int> excludedColors2;

		for (int iii = 0; iii < tiles.size(); iii++)
		{
			//If first 5 matches, color-coordinate matching shapes
			if(difficulty < 75)
				colorKey.first = colorsVector[iii];
			
			//Otherwise, randomly assign a color, and remove it from the possible selection list
			else
			{
				int randColor2 = nrgFunctions::GetRandom(0, colorsVector.size() - 1, excludedColors2);
				colorKey.first = colorsVector[randColor2];
				excludedColors2.emplace(randColor2);
			}

			colorKey.second = colorKey.first;
			colorPair = GetColorFromKey(colorKey);
			randTile = nrgFunctions::GetRandom(TOP_LEFT, BOTTOM_RIGHT, excludedTiles);
			excludedTiles.emplace(randTile);
			tiles[randTile]->ChangeShape(shapesVector[iii], colorPair);
		}

		//NEXT, NEED TO DECIDE ON 3 SHAPES TO MATCH SPECIFIC SHAPES ON
		int match1a;
		int match1b;
		int match2a;
		int match2b;
		int match3a;
		int match3b;
		std::unordered_set<int> excludedRands;

		match1a = nrgFunctions::GetRandom(TOP_LEFT, BOTTOM_RIGHT, excludedRands);
		excludedRands.emplace(match1a);
		matchingTilesVector.push_back(match1a);
		matchingTiles.emplace(match1a);

		//Find the other tile that matches match1a's shape
		for (int iii = 0; iii < tiles.size(); iii++)
		{
			if (tiles[iii]->GetShape() == tiles[match1a]->GetShape() && tiles[iii]->GetTilePosition() != tiles[match1a]->GetTilePosition())
			{
				match1b = tiles[iii]->GetTilePosition();
				matchingTilesVector.push_back(match1b);
				excludedRands.emplace(match1b);
				matchingTiles.emplace(match1b);
			}
		}

		match2a = nrgFunctions::GetRandom(TOP_LEFT, BOTTOM_RIGHT, excludedRands);
		excludedRands.emplace(match2a);
		matchingTilesVector.push_back(match2a);
		matchingTiles.emplace(match2a);

		//Find the other tile that matches match2a's shape
		for (int iii = 0; iii < tiles.size(); iii++)
		{
			if (tiles[iii]->GetShape() == tiles[match2a]->GetShape() && tiles[iii]->GetTilePosition() != tiles[match2a]->GetTilePosition())
			{
				match2b = tiles[iii]->GetTilePosition();
				matchingTilesVector.push_back(match2b);
				excludedRands.emplace(match2b);
				matchingTiles.emplace(match2b);
			}
		}

		match3a = nrgFunctions::GetRandom(TOP_LEFT, BOTTOM_RIGHT, excludedRands);
		excludedRands.emplace(match3a);
		matchingTilesVector.push_back(match3a);
		matchingTiles.emplace(match3a);

		//Find the other tile that matches match3a's shape
		for (int iii = 0; iii < tiles.size(); iii++)
		{
			if (tiles[iii]->GetShape() == tiles[match3a]->GetShape() && tiles[iii]->GetTilePosition() != tiles[match3a]->GetTilePosition())
			{
				match3b = tiles[iii]->GetTilePosition();
				matchingTilesVector.push_back(match3b);
				excludedRands.emplace(match3b);
				matchingTiles.emplace(match3b);
			}
		}

		//Get the order of match criteria, regardless of whether 1 or 2 shapes fit criteria in matchingTilesVector
		matchCriteriaOrder.push_back(match1a);
		matchCriteriaOrder.push_back(match2a);
		matchCriteriaOrder.push_back(match3a);

		//Get Header Strings based on each shape
		for (int iii = 0; iii < matchCriteriaOrder.size(); iii++)
		{
			switch (tiles[matchCriteriaOrder[iii]]->GetShape())
			{
			case CIRCLE:
				nextHeaderStringVector.push_back("Circle");
				break;
			case SQUARE:
				nextHeaderStringVector.push_back("Square");
				break;
			case DIAMOND:
				nextHeaderStringVector.push_back("Diamond");
				break;
			case TRIANGLE:
				nextHeaderStringVector.push_back("Triangle");
				break;
			case HEXAGON:
				nextHeaderStringVector.push_back("Hexagon");
				break;
			case OCTAGON:
				nextHeaderStringVector.push_back("Octagon");
				break;
			}
		}
	}

	//Three shape matches. Three color matches
	else if (difficulty >= 90 && difficulty <= 99)
	{
		int shape1 = nrgFunctions::GetRandom(CIRCLE, OCTAGON, excludedShapes);
		excludedShapes.emplace(shape1);
		int shape2 = nrgFunctions::GetRandom(CIRCLE, OCTAGON, excludedShapes);
		excludedShapes.emplace(shape2);
		int shape3 = nrgFunctions::GetRandom(CIRCLE, OCTAGON, excludedShapes);
		excludedShapes.emplace(shape3);
		int color1 = nrgFunctions::GetRandom(RED, ORANGE, excludedColors);
		excludedColors.emplace(color1);
		int color2 = nrgFunctions::GetRandom(RED, ORANGE, excludedColors);
		excludedColors.emplace(color2);
		int color3 = nrgFunctions::GetRandom(RED, ORANGE, excludedColors);
		excludedColors.emplace(color3);

		int shape1Frequency = 0;
		int shape2Frequency = 0;
		int shape3Frequency = 0;
		int color1Frequency = 0;
		int color2Frequency = 0;
		int color3Frequency = 0;

		int randColor;

		std::vector<int> shape1Vector;
		std::vector<int> shape2Vector;
		std::vector<int> shape3Vector;
		std::vector<int> color1Vector;
		std::vector<int> color2Vector;
		std::vector<int> color3Vector;

		std::unordered_set<int> excludedShapeRands;
		std::unordered_set<int> excludedColorRands;

		for(int iii = 0; iii < tiles.size(); iii++)
		{

			randShape = nrgFunctions::GetRandom(1, 3, excludedShapeRands);

			if (randShape == 1)
			{
				randShape = shape1;
				shape1Frequency++;
				shape1Vector.push_back(iii);

				if (shape1Frequency >= 3)
					excludedShapeRands.emplace(1);
			}

			else if (randShape == 2)
			{
				randShape = shape2;
				shape2Frequency++;
				shape2Vector.push_back(iii);

				if (shape2Frequency >= 3)
					excludedShapeRands.emplace(2);
			}

			else if (randShape == 3)
			{
				randShape = shape3;
				shape3Frequency++;
				shape3Vector.push_back(iii);

				if (shape3Frequency >= 3)
					excludedShapeRands.emplace(3);
			}
			
			randColor = nrgFunctions::GetRandom(1, 3, excludedColorRands);

			if (randColor == 1)
			{
				randColor = color1;
				color1Frequency++;
				color1Vector.push_back(iii);

				if (color1Frequency >= 3)
					excludedColorRands.emplace(1);
			}

			else if (randColor == 2)
			{
				randColor = color2;
				color2Frequency++;
				color2Vector.push_back(iii);

				if (color2Frequency >= 3)
					excludedColorRands.emplace(2);
			}

			else if (randColor == 3)
			{
				randColor = color3;
				color3Frequency++;
				color3Vector.push_back(iii);

				if (color3Frequency >= 3)
					excludedColorRands.emplace(3);
			}

			colorKey.first = randColor;
			colorKey.second = randColor;
			colorPair = GetColorFromKey(colorKey);

			tiles[iii]->ChangeShape(randShape, colorPair);
		}

		//Decide which tile set will be matching criteria
		int rand = nrgFunctions::GetRandom(0, 5);

		switch (rand)
		{
		case 0: //Shape 1
			matchCriteria = SHAPES;
			for (int iii = 0; iii < shape1Vector.size(); iii++)
			{
				matchingTiles.emplace(shape1Vector[iii]);
				matchingTilesVector.push_back(shape1Vector[iii]);
			}

			switch (shape1)
			{
			case CIRCLE:
				nextHeaderString = "Circles";
				break;
			case SQUARE:
				nextHeaderString = "Squares";
				break;
			case DIAMOND:
				nextHeaderString = "Diamonds";
				break;
			case TRIANGLE:
				nextHeaderString = "Triangles";
				break;
			case HEXAGON:
				nextHeaderString = "Hexagons";
				break;
			case OCTAGON:
				nextHeaderString = "Octagons";
				break;
			}
			break;
		case 1: //Color 1
			matchCriteria = COLORS;
			for (int iii = 0; iii < color1Vector.size(); iii++)
			{
				matchingTiles.emplace(color1Vector[iii]);
				matchingTilesVector.push_back(color1Vector[iii]);
			}

			switch (color1)
			{
			case RED:
				nextHeaderString = "Reds";
				break;
			case GREEN:
				nextHeaderString = "Greens";
				break;
			case BLUE:
				nextHeaderString = "Dark Blues";
				break;
			case YELLOW:
				nextHeaderString = "Yellows";
				break;
			case PURPLE:
				nextHeaderString = "Purples";
				break;
			case CYAN:
				nextHeaderString = "Light Blues";
				break;
			case ORANGE:
				nextHeaderString = "Oranges";
				break;
			}
			break;
		case 2: //Color 2
			matchCriteria = COLORS;
			for (int iii = 0; iii < color2Vector.size(); iii++)
			{
				matchingTiles.emplace(color2Vector[iii]);
				matchingTilesVector.push_back(color2Vector[iii]);
			}

			switch (color2)
			{
			case RED:
				nextHeaderString = "Reds";
				break;
			case GREEN:
				nextHeaderString = "Greens";
				break;
			case BLUE:
				nextHeaderString = "Dark Blues";
				break;
			case YELLOW:
				nextHeaderString = "Yellows";
				break;
			case PURPLE:
				nextHeaderString = "Purples";
				break;
			case CYAN:
				nextHeaderString = "Light Blues";
				break;
			case ORANGE:
				nextHeaderString = "Oranges";
				break;
			}
			break;
		case 3: //Shape 2
			matchCriteria = SHAPES;
			for (int iii = 0; iii < shape2Vector.size(); iii++)
			{
				matchingTiles.emplace(shape2Vector[iii]);
				matchingTilesVector.push_back(shape2Vector[iii]);
			}

			switch (shape2)
			{
			case CIRCLE:
				nextHeaderString = "Circles";
				break;
			case SQUARE:
				nextHeaderString = "Squares";
				break;
			case DIAMOND:
				nextHeaderString = "Diamonds";
				break;
			case TRIANGLE:
				nextHeaderString = "Triangles";
				break;
			case HEXAGON:
				nextHeaderString = "Hexagons";
				break;
			case OCTAGON:
				nextHeaderString = "Octagons";
				break;
			}
			break;

		case 4: //Shape 3
			matchCriteria = SHAPES;
			for (int iii = 0; iii < shape3Vector.size(); iii++)
			{
				matchingTiles.emplace(shape3Vector[iii]);
				matchingTilesVector.push_back(shape3Vector[iii]);
			}

			switch (shape3)
			{
			case CIRCLE:
				nextHeaderString = "Circles";
				break;
			case SQUARE:
				nextHeaderString = "Squares";
				break;
			case DIAMOND:
				nextHeaderString = "Diamonds";
				break;
			case TRIANGLE:
				nextHeaderString = "Triangles";
				break;
			case HEXAGON:
				nextHeaderString = "Hexagons";
				break;
			case OCTAGON:
				nextHeaderString = "Octagons";
				break;
			}
			break;

		case 5: //Color 3
			matchCriteria = COLORS;
			for (int iii = 0; iii < color3Vector.size(); iii++)
			{
				matchingTiles.emplace(color3Vector[iii]);
				matchingTilesVector.push_back(color3Vector[iii]);
			}

			switch (color3)
			{
			case RED:
				nextHeaderString = "Reds";
				break;
			case GREEN:
				nextHeaderString = "Greens";
				break;
			case BLUE:
				nextHeaderString = "Dark Blues";
				break;
			case YELLOW:
				nextHeaderString = "Yellows";
				break;
			case PURPLE:
				nextHeaderString = "Purples";
				break;
			case CYAN:
				nextHeaderString = "Light Blues";
				break;
			case ORANGE:
				nextHeaderString = "Oranges";
				break;
			}
			break;
		}
	}

	//3 Circles. 3 Hexagons. 3 Octagons. Randomly assigned Red, Orange, or Yellow. Inherently confusing round.
	else if (difficulty == 100)
	{
		int shape1 = CIRCLE;
		excludedShapes.emplace(shape1);
		int shape2 = HEXAGON;
		excludedShapes.emplace(shape2);
		int shape3 = OCTAGON;
		excludedShapes.emplace(shape3);
		int color1 = RED;
		excludedColors.emplace(color1);
		int color2 = YELLOW;
		excludedColors.emplace(color2);
		int color3 = ORANGE;
		excludedColors.emplace(color3);

		int shape1Frequency = 0;
		int shape2Frequency = 0;
		int shape3Frequency = 0;
		int color1Frequency = 0;
		int color2Frequency = 0;
		int color3Frequency = 0;

		int randColor;

		std::vector<int> shape1Vector;
		std::vector<int> shape2Vector;
		std::vector<int> shape3Vector;
		std::vector<int> color1Vector;
		std::vector<int> color2Vector;
		std::vector<int> color3Vector;

		std::unordered_set<int> excludedShapeRands;
		std::unordered_set<int> excludedColorRands;

		for (int iii = 0; iii < tiles.size(); iii++)
		{

			randShape = nrgFunctions::GetRandom(1, 3, excludedShapeRands);

			if (randShape == 1)
			{
				randShape = shape1;
				shape1Frequency++;
				shape1Vector.push_back(iii);

				if (shape1Frequency >= 3)
					excludedShapeRands.emplace(1);
			}

			else if (randShape == 2)
			{
				randShape = shape2;
				shape2Frequency++;
				shape2Vector.push_back(iii);

				if (shape2Frequency >= 3)
					excludedShapeRands.emplace(2);
			}

			else if (randShape == 3)
			{
				randShape = shape3;
				shape3Frequency++;
				shape3Vector.push_back(iii);

				if (shape3Frequency >= 3)
					excludedShapeRands.emplace(3);
			}

			randColor = nrgFunctions::GetRandom(1, 3, excludedColorRands);

			if (randColor == 1)
			{
				randColor = color1;
				color1Frequency++;
				color1Vector.push_back(iii);

				if (color1Frequency >= 3)
					excludedColorRands.emplace(1);
			}

			else if (randColor == 2)
			{
				randColor = color2;
				color2Frequency++;
				color2Vector.push_back(iii);

				if (color2Frequency >= 3)
					excludedColorRands.emplace(2);
			}

			else if (randColor == 3)
			{
				randColor = color3;
				color3Frequency++;
				color3Vector.push_back(iii);

				if (color3Frequency >= 3)
					excludedColorRands.emplace(3);
			}

			colorKey.first = randColor;
			colorKey.second = randColor;
			colorPair = GetColorFromKey(colorKey);

			tiles[iii]->ChangeShape(randShape, colorPair);
		}

		//Decide which tile set will be matching criteria
		int rand = nrgFunctions::GetRandom(0, 2);

		switch (rand)
		{
		case 0: //Shape 1
			matchCriteria = SHAPES;
			for (int iii = 0; iii < shape1Vector.size(); iii++)
			{
				matchingTiles.emplace(shape1Vector[iii]);
				matchingTilesVector.push_back(shape1Vector[iii]);
			}

			switch (shape1)
			{
			case CIRCLE:
				nextHeaderString = "Circles";
				break;
			case HEXAGON:
				nextHeaderString = "Hexagons";
				break;
			case OCTAGON:
				nextHeaderString = "Octagons";
				break;
			}
			break;

		case 1: //Shape 2
			matchCriteria = SHAPES;
			for (int iii = 0; iii < shape2Vector.size(); iii++)
			{
				matchingTiles.emplace(shape2Vector[iii]);
				matchingTilesVector.push_back(shape2Vector[iii]);
			}

			switch (shape2)
			{
			case CIRCLE:
				nextHeaderString = "Circles";
				break;
			case HEXAGON:
				nextHeaderString = "Hexagons";
				break;
			case OCTAGON:
				nextHeaderString = "Octagons";
				break;
			}
			break;

		case 2: //Shape 3
			matchCriteria = SHAPES;
			for (int iii = 0; iii < shape3Vector.size(); iii++)
			{
				matchingTiles.emplace(shape3Vector[iii]);
				matchingTilesVector.push_back(shape3Vector[iii]);
			}

			switch (shape3)
			{
			case CIRCLE:
				nextHeaderString = "Circles";
				break;
			case HEXAGON:
				nextHeaderString = "Hexagons";
				break;
			case OCTAGON:
				nextHeaderString = "Octagons";
				break;
			}

			break;
		}
	}

	return;
}

void MemoryMode::ReRandomizeTiles()
{
	if (currentSelection == 1)
	{
		randomizeTiles = true;
		if (timerText->GetLabel()->getOpacity() != 255)
			timerText->GetLabel()->runAction(cocos2d::FadeIn::create(0.5));
		
		warningTime = 3000;
	}
		
	else if (currentSelection != 1)
		forcedGameOver = true;

	for (int iii = 0; iii < tiles.size(); iii++)
	{
		tiles[iii]->HideShape();
		tiles[iii]->SetFade(0);
	}

	disablePowerups = true;
	reuseBonuses = true;

	return;
}

void MemoryMode::ReuseBonuses()
{
	//Remember how many matched tiles were showing
	int tempShowMatchesUsed = matchedTilesShowing;
	matchedTilesShowing = 0;

	//Remember how many tiles were eliminated (matching tiles are always excluded, so total excluded tiles minus matching tiles)
	int tempEliminatesUsed = eliminateUsedOnCurrentCard;
	eliminateUsedOnCurrentCard = 0;
	excludedEliminateTiles.clear();

	//Lower bonuses used to balance out re-using bonuses below
	bonusesUsed -= tempEliminatesUsed;
	bonusesUsed -= tempShowMatchesUsed;

	//Re-use the same amount of bonuses as before re-randomize
	for (int iii = 0; iii < tempEliminatesUsed; iii++)
		UseBonusEliminate();
	for (int iii = 0; iii < tempShowMatchesUsed; iii++)
		UseShowMatch();

	return;
}

void MemoryMode::Reset()
{
	Game::Reset();

	//Intro animations
	int offset = 50;
	float duration = 0.5;
	float delay1 = duration / 2;
	float delay2 = (duration + delay1) / 2;

	baseTile->setPositionY(baseTile->getPositionY() + offset);
	sequenceText->setPositionY(sequenceText->getPositionY() + offset);
	baseTile->runAction(cocos2d::Sequence::create(
		cocos2d::DelayTime::create(delay2),
		cocos2d::Spawn::create(
			cocos2d::FadeIn::create(duration),
			cocos2d::MoveBy::create(duration, cocos2d::Vec2(0, -offset)),
			cocos2d::CallFunc::create([this]() {sequenceText->runAction(cocos2d::MoveBy::create(0.5, cocos2d::Vec2(0, -50)));}), NULL),
		cocos2d::CallFunc::create([this]()
	{
		//Turn off touch until game starts
		for (int iii = 0; iii < tiles.size(); iii++)
			tiles[iii]->ToggleTouch(false);

		baseTileShadow->runAction(cocos2d::FadeIn::create(0.5));
		scoreTile->runAction(cocos2d::FadeIn::create(0.5));
		scoreText->GetLabel()->runAction(cocos2d::FadeIn::create(0.5));
		headerTile->runAction(cocos2d::FadeIn::create(0.5));
		matchCriteriaText->GetLabel()->runAction(cocos2d::FadeIn::create(0.5));
		timerText->GetLabel()->runAction(cocos2d::FadeIn::create(0.5));
		optionsButton->addEvents();
		optionsButton->ToggleTouch(false);
		highScoresButton->addEvents();
		highScoresButton->ToggleTouch(false);
		buyBonusItemsButton->addEvents();
		buyBonusItemsButton->ToggleTouch(false);
		sceneTimer.Restart();
		countdownTimer.Restart();
		hideTiles = true;
		sceneTimer.SetMark(timelimit);
		for (int iii = 0; iii < tiles.size(); iii++)
			tiles[iii]->ToggleTouch(false);
	}), NULL));

	return;
}

void MemoryMode::SetMatchParameters(int excludedMatchCriteria, bool blind)
{
	//Standard start
	Game::SetMatchParameters();
	excludedShapes.erase(HEXAGON);

	difficulty = matches;

	//Repeat cycle after completing all matching rounds
	while (difficulty > 100)
		difficulty -= 101;

	//All time before 100 matches is calculated. All time after match 100 is 5 seconds
	if (matches <= 99)
		timelimit = 15000 - (difficulty % 10) * 1111;
	else
		timelimit = 5000;

	msLeft = timelimit;
	warningTime = 3000;

	if ((difficulty >= 40 && difficulty <= 49) || (difficulty >= 60 && difficulty <= 69) || (difficulty >= 80 && difficulty <= 89))
	{
		specialRestore = true;
		specialRestoreStart = true;
		specialRestoreFinished = false;
	}
	else
		specialRestore = false;

	//Determine match criteria based on which match we're on

	//4 Matching Tiles, 4 Nonmatching Tiles, 1 random
	if (difficulty < 20)
	{
		if (excludedMatchCriteria == -2)
			matchCriteria = matchCriteria;
		else if (excludedMatchCriteria == COLORS)
			matchCriteria = SHAPES;
		else if (excludedMatchCriteria == SHAPES)
			matchCriteria = COLORS;
		else
			matchCriteria = nrgFunctions::GetRandomChoice(COLORS, SHAPES);

		numberOfMatchingTiles = 4;

		if (matchCriteria == SHAPES)
			numberOfSameNonmatchingColors = 4;
		else if (matchCriteria == COLORS)
			numberOfSameNonmatchingShapes = 4;
	
		useDesignatedMatchStyle = true;
		useDesignatedNonmatchStyle = true;
		designatedMatchStyle.first = 0;
		designatedMatchStyle.second = 0;
		designatedNonmatchStyle.first = 0;
		designatedNonmatchStyle.second = 0;
	}

	//3 Shapes, 3 Colors, 3 Styles. Special Restore starts at 30-59. Match rule revealed at start with special restore.
	else if ( (difficulty >= 20 && difficulty <= 29) || (difficulty >= 40 && difficulty <= 49) || (difficulty >= 60 && difficulty <= 69) || (difficulty >= 80 && difficulty <= 89) )
	{
		numberOfMatchingTiles = 3;

		if (difficulty >= 40 && difficulty <= 49)
			matchCriteria = COLORS;
		else if (difficulty >= 60 && difficulty <= 69)
			matchCriteria = SHAPES;
		else if (difficulty >= 80 && difficulty <= 89)
			matchCriteria = STYLES;
		else
			matchCriteria = nrgFunctions::GetRandom(COLORS, STYLES);

		if (excludedMatchCriteria == -2)
			matchCriteria = matchCriteria;
		else if (excludedMatchCriteria == STYLES)
			matchCriteria = nrgFunctions::GetRandomChoice(SHAPES, COLORS);
		else if (excludedMatchCriteria == COLORS)
			matchCriteria = nrgFunctions::GetRandomChoice(SHAPES, STYLES);
		else if (excludedMatchCriteria == SHAPES)
			matchCriteria = nrgFunctions::GetRandomChoice(COLORS, STYLES);
			
		switch (matchCriteria)
		{
		case COLORS:
			numberOfSameNonmatchingShapes = 3;
			numberOfSameNonmatchingStyles = 3;
			nonmatchedShapesCanExceedTwo = false;
			nonmatchedStylesCanExceedTwo = false;
			break;
		case SHAPES:
			numberOfSameNonmatchingColors = 3;
			numberOfSameNonmatchingStyles = 3;
			nonmatchedColorsCanExceedTwo = false;
			nonmatchedStylesCanExceedTwo = false;
			break;
		case STYLES:
			numberOfSameNonmatchingColors = 3;
			numberOfSameNonmatchingStyles = 3;
			nonmatchedShapesCanExceedTwo = false;
			nonmatchedColorsCanExceedTwo = false;
			break;
		}
	}

	//3 of one Shape, different colors. 3 of one Color, different shapes. 3 of same Color, same Shape.
	else if (difficulty >= 30 && difficulty <= 39)
	{
		numberOfMatchingTiles = 3;

		if (excludedMatchCriteria == -2)
			matchCriteria = matchCriteria;
		else if (excludedMatchCriteria == COLORS)
			matchCriteria = nrgFunctions::GetRandomChoice(SHAPES, COLORS_SHAPES);
		else if (excludedMatchCriteria == SHAPES)
			matchCriteria = nrgFunctions::GetRandomChoice(COLORS, COLORS_SHAPES);
		else
		{
			int rand = nrgFunctions::GetRandom(0, 2);
			switch (rand)
			{
			case 0:
				matchCriteria = COLORS;
				matchingColors = true;
				break;
			case 1:
				matchCriteria = SHAPES;
				matchingShapes = true;
				break;
			case 2:
				matchCriteria = COLORS_SHAPES;
				matchingColors = true;
				matchingShapes = true;
				break;
			}
		}
	}

	//Everything is determined in RandomizeTiles. 4 of one Color. 5 of  one Color. 4 of one Shape. 5 of one Shape.
	else if(difficulty >= 50 && difficulty <= 59)
		;

	//Determined in RandomizeTiles. 4 pairs of different shapes. One different shape. Specific shape match, random order--different shape each time.
	else if (difficulty >= 70 && difficulty <= 79)
		matchCriteria = SHAPES;

	//Disable powerups for difficulty 100 match
	else if (difficulty == 100)
		matchCriteria = SHAPES;

	//Introduce possibility of Octagons and Hexagons after Match 15
	if (difficulty >= 15)
		excludedShapes.erase(OCTAGON);

	//Introduce blinds style to allow 9 possible tiles after Style Practice
	if (difficulty >= 20)
	{
		excludedStyles.erase(BLINDS);
		excludedStyles.erase(BLINDS_QUAD);
		excludedStyles.erase(OUTLINE_BLINDS);
		excludedStyles.erase(OUTLINE_QUAD);
		excludedStyles.erase(CONCENTRIC_QUAD);
		excludedStyles.erase(CONCENTRIC_BLINDS);
	}

	//Introduce more difficult styles at Match 100
	if (matches > 100)
	{
		excludedStyles.erase(OUTLINE_BLINDS_QUAD);
		excludedStyles.erase(CONCENTRIC_BLINDS_QUAD);
	}

	switch (matchCriteria)
	{
	case COLORS:
		matchingColors = true;
		break;
	case SHAPES:
		matchingShapes = true;
		break;
	case STYLES:
		matchingStyles = true;
		break;
	}

	return;
}

void MemoryMode::onEnter()
{
	mode = MEMORY;
    
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    sdkbox::PluginSdkboxPlay::setListener(this);
#endif
    
	Game::onEnter();
	return;
}

void MemoryMode::onExit()
{
    showingAd = false;
    
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    sdkbox::PluginSdkboxPlay::removeListener();
#endif
    
	Game::onExit();
	return;
}

MemoryMode::~MemoryMode()
{
	timers.clear();
}

#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
void MemoryMode::onConnectionStatusChanged(int status)
{
}

void MemoryMode::onScoreSubmitted(const std::string & leaderboard_name, int score, bool maxScoreAllTime, bool maxScoreWeek, bool maxScoreToday)
{
}

void MemoryMode::onIncrementalAchievementUnlocked(const std::string & achievement_name)
{
}

void MemoryMode::onIncrementalAchievementStep(const std::string & achievement_name, int step)
{
}

void MemoryMode::onAchievementUnlocked(const std::string & achievement_name, bool newlyUnlocked)
{
}
#endif