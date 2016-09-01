#pragma once
#include "pch.h"
#include "Countdown.h"

Countdown::Countdown()
{
}

cocos2d::Scene * Countdown::CreateScene()
{
	//Create Scene
	cocos2d::Scene* scene = cocos2d::Scene::create();

	//Create layer
	cocos2d::Layer* layer = Countdown::create();
	
	scene->addChild(layer);

	return scene;
}

bool Countdown::init()
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
	startingMs = 25000;
	matchMilestone = 5;
	nextMilestone = 5;
	gameStarted = true;
	mode = COUNTDOWN;

	InitGameAssets();
	Reset();
	
	highestMatch = gameData->getIntegerForKey(best_match_countdown, 0);
	highestScore = gameData->getIntegerForKey(best_score_countdown, 0);

	//Add timers to vector so they can be controlled outside of class methods
	timers.push_back(&sceneTimer);
	timers.push_back(&countdownTimer);
	timers.push_back(&speedTimer_5_matches);
	timers.push_back(&speedTimer_10_matches);
	timers.push_back(&speedTimer_20_matches);
	timers.push_back(&bonusTimer);
	timers.push_back(&sequenceTimer);

	if (testing)
		startingMs = 120000;

	msLeft = startingMs;

    warningTime = 3000;
    
	speedTimer_5_matches.Start();
	speedTimer_10_matches.Start();
	speedTimer_20_matches.Start();

    //Begin Game Loop
	scheduleUpdate();
	return true;
}

void Countdown::update(float dt)
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
		GenerateBonus();

	//Display bonus item chosen
	if (bonusTimer.IsRunning() && bonusTimer.GetElapsedTime() >= bonusTimer.GetMark())
		DisplayBonus();

	//KEEP BONUS ITEM
	if (confirmingBonus && menuRight->WasPressed())
		AcceptBonus();

	//EXCHANGE BONUS FOR POINTS
	if (confirmingBonus && menuLeft->WasPressed())
		TradeBonus();

	//Don't perform any actions if generating bonus items
	if (confirmingBonus || (bonusTimer.IsRunning() && bonusTimer.GetElapsedTime() <= bonusTimer.GetMark()))
		return;

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
		allowRerandomize = true;
		randomizeTiles = true;
		sceneTimer.SetMark(0);
		speedTimer_5_matches.Unpause();
		speedTimer_10_matches.Unpause();
		speedTimer_20_matches.Unpause();
	}

	//Leave loop if displaying sequence
	if (displaySequence)
		return;

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
		eliminateTile->ToggleTouch(false);
		timePlusTile->ToggleTouch(false);
		showMatchTile->ToggleTouch(false);
		newCardTile->ToggleTouch(false);
	}

	//Check if eliminate tile bonus is used
	if (eliminateTile->WasTouched() && bonusEliminateLeft > 0)
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
	if (showMatchTile->WasTouched() && bonusShowMatchLeft > 0)
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
	if (newCardTile->WasTouched() && bonusNewCardLeft > 0)
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
				tile->DismissTile();
				selection1 = tile->GetTilePosition();
				tile->ToggleTouch(false);
				currentSelection = 2;
				audio->PlayClip("tone1");

				//Keep eliminate tile and show match tile off if currentselection isn't 1 (can only use when player hasn't already selected a tile
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

	//Check for match success or failure once 3 tiles have been touched
	if (currentSelection == 4)
	{
		currentSelection = 1;

		if (matchingTiles.find(selection1) != matchingTiles.end() && matchingTiles.find(selection2) != matchingTiles.end() && matchingTiles.find(selection3) != matchingTiles.end())
			matchMade = true;
		else
			gameOver = true;

		if (matchMade)
		{
			matches++;
			gameData->setIntegerForKey(total_matches, gameData->getIntegerForKey(total_matches, 0) + 1);
			gameData->setIntegerForKey(countdown_matches, gameData->getIntegerForKey(countdown_matches, 0) + 1);
			CheckSequence();

			//Check for speed match achievements
			if (matches == 5)
			{
				if (speedTimer_5_matches.GetElapsedTime() <= 12000)
				{
					speedTimer_5_matches.Stop();
					earnedSpeedDemon1 = true;
				}
			}

			else if (matches == 10)
			{
				if (speedTimer_10_matches.GetElapsedTime() <= 24000)
				{
					speedTimer_10_matches.Stop();
					earnedSpeedDemon2 = true;
				}
			}

			else if (matches == 20)
			{
				if (speedTimer_20_matches.GetElapsedTime() <= 50000)
				{
					speedTimer_20_matches.Stop();
					earnedSpeedDemon3 = true;
				}
			}

			//Check for no bonuses used achievement
			if (matches == 50 && bonusesUsed == 0)
				noBonusesUsedBefore50Matches = true;

			//Check for near miss achievements
			if (msLeft <= 0)
			{
				lastMatchWasNearMiss = true;
				nearMiss = true;
			}

			else
			{
				gameData->setIntegerForKey(consecutive_near_misses, 0);
				lastMatchWasNearMiss = false;
			}

			if (lastMatchWasNearMiss)
				gameData->setIntegerForKey(consecutive_near_misses, gameData->getIntegerForKey(consecutive_near_misses, 0) + 1);

			if (matchCriteria == COLORS)
				gameData->setIntegerForKey(total_color_matches, gameData->getIntegerForKey(total_color_matches, 0) + 1);
			if (matchCriteria == SHAPES)
				gameData->setIntegerForKey(total_shape_matches, gameData->getIntegerForKey(total_shape_matches, 0) + 1);
			if (matchCriteria == STYLES)
				gameData->setIntegerForKey(total_style_matches, gameData->getIntegerForKey(total_style_matches, 0) + 1);

			if (matches != matchMilestone)
				audio->PlayClip("double_tone_high");

			msAdded = matchTimeIncrease;

			if (matches == matchMilestone)
			{
				audio->PlayClip("triple_tone_high");
				msAdded += milestoneTimeIncrease;
				generateBonus = true;
				matchMilestone += nextMilestone;
			}

			//Check to see if we're still above 50 seconds remaining for first 50 matches
            if (msLeft < 20000 && matches <= 50)
                msWentBelow20 = true;
			if (!msWentBelow20 && matches > 50)
				remainingTimeAbove20 = true;
			

			msLeft += msAdded;

			//Reset warning time to correct threshold 
			if (msLeft >= 3000)
				warningTime = 3000;
			else if (msLeft >= 2000)
				warningTime = 2000;
			else if (msLeft >= 1000)
				warningTime = 1000;

			CalculateScore();
			UpdateScore();
			DismissHeaderText();
			dismissTiles = true;
			disablePowerups = true;
			sceneTimer.SetMark(100);
			matchMade = false;
			timePlusUsedOnCurrentCard = 0;

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
			CalculateScore();
			UpdateScore();

			//Reveal match type for blind match
			blindMatch = false;
			if (scoreText->getString() == "???")
				UpdateHeaderText(matchCriteria);

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

	//Update time until game over. Update AFTER checking for any user input (to allow for a match first)
	if (!gameOver)
		UpdateTimer();

	//Leave loop and begin generate bonus on next loop
	if (generateBonus)
		return;

	if (displaySequence && !sequenceTimer.IsMarkSet())
		DisplaySequence();

	if (displaySequence)
		return;

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
		UpdateHeaderText(matchCriteria);
		RandomizeTiles();
		randomizeTiles = false;
		restoreTiles = true;
		sceneTimer.SetMark(500);
	}

	//Restore all tiles
	if (restoreTiles && sceneTimer.GetElapsedTime() >= sceneTimer.GetMark())
	{
		if (gameOver)
		{
			for (int iii = 0; iii < tiles.size(); iii++)
			{
				tiles[iii]->RestoreTile();

				//If game over, look for non matching tiles (int matchingTiles does not contain) and set to half opacity
				if (matchingTiles.find(iii) == matchingTiles.end())
					tiles[iii]->SetFade(128);
			}
		}

		else
			RestoreTiles();

		restoreTiles = false;

		if (!gameOver)
		{
			enablePowerups = true;
			sceneTimer.SetMark(500);
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

		disablePowerups = false;
	}

	if (enablePowerups && sceneTimer.GetElapsedTime() >= sceneTimer.GetMark())
	{
		powerupsAreOn = true;

		timePlusTile->setVisible(true);
		eliminateTile->setVisible(true);
		showMatchTile->setVisible(true);
		newCardTile->setVisible(true);

		if (bonusEliminateLeft > 0)
		{
			eliminateTile->runAction(cocos2d::FadeTo::create(0.5, 255));
			eliminateTile->ToggleTouch(true);
			eliminateText->GetLabel()->runAction(cocos2d::FadeTo::create(0.5, 255));
		}

		if (bonusTimePlusLeft > 0)
		{
			timePlusTile->runAction(cocos2d::FadeTo::create(0.5, 255));
			timePlusTile->ToggleTouch(true);
			timePlusText->GetLabel()->runAction(cocos2d::FadeTo::create(0.5, 255));
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
		retry->ToggleOn(false);
		gameOver = false;
		gameWon = false;

#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
        audio->SetMusicVolume(1);
#endif
        
#if(CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
        audio->SetMusicVolume(Game::GetiOSMaxVolumeBG());
#endif
        
        earnedSpeedDemon1 = false;
		earnedSpeedDemon2 = false;
		earnedSpeedDemon3 = false;

		//For testing purposes
		matches = startingMatches;

		startingMs = 30000;
		matchMilestone = 5;
		nextMilestone = 5;
		msLeft = startingMs;
		sprintf(sz, "%d.00", startingMs / 1000);
		timerText->setString(sz);
		timerText->Center();
		timerText->SetBottom(timeRemainingBottomPosition);
        timerText->GetLabel()->setTextColor(cocos2d::Color4B::BLACK);
        
		//Turn menu buttons off
		optionsButton->runAction(cocos2d::Sequence::create(cocos2d::FadeOut::create(0.25), cocos2d::CallFunc::create([this]() {optionsButton->setVisible(false);}), NULL));
		optionsText->GetLabel()->runAction(cocos2d::Sequence::create(cocos2d::FadeOut::create(0.25), cocos2d::CallFunc::create([this]() {optionsText->GetLabel()->setVisible(false);}), NULL));
		highScoresButton->runAction(cocos2d::Sequence::create(cocos2d::FadeOut::create(0.25), cocos2d::CallFunc::create([this](){highScoresButton->setVisible(false);}), NULL));
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

		disablePowerups = false;
		powerupsAreOn = true;
		bestMatchIcon->setOpacity(0);
		bestMatchIcon->setVisible(false);
		bestScoreIcon->setOpacity(0);
		bestScoreIcon->setVisible(false);
		ResetScore();
		finalScore = 0;
		displayedScore = 0;
		UpdateScore();
		currentSelection = 1;
		bonusesUsed = 0;
		timerReached90 = false;
		remainingTimeAbove20 = false;
		lastMatchWasNearMiss = false;
		nearMiss = false;
		noBonusesUsedBefore50Matches = false;
		gameData->setIntegerForKey(consecutive_near_misses, 0);
		achievementsUnlocked = 0;
		sequence = 1;
		displaySequence = true;

		generateBonus = false;
		confirmingBonus = false;

		//Get ride of achievements text
		achievementUnlockedHeader->GetLabel()->runAction(cocos2d::Sequence::create(cocos2d::FadeOut::create(0.25), cocos2d::CallFunc::create([this]() {achievementUnlockedHeader->GetLabel()->setVisible(false);}), NULL));
		for (int iii = 0; iii < achievementsText.size(); iii++)
		{
			achievementsText[0]->removeFromParent();
			achievementsText.erase(achievementsText.begin());
		}

		achievementsText.clear();

		SetMatchParameters();
		UpdateHeaderText(matchCriteria);
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

		timePlusUsedOnCurrentCard = 0;

		timePlusTile->setVisible(true);
		eliminateTile->setVisible(true);
		showMatchTile->setVisible(true);
		newCardTile->setVisible(true);

		if (bonusEliminateLeft == 0)
		{
			eliminateTile->setOpacity(0);
			eliminateText->GetLabel()->setOpacity(0);
		}
		else
			eliminateTile->ToggleTouch(true);

		if (bonusTimePlusLeft == 0)
		{
			timePlusTile->setOpacity(0);
			timePlusText->GetLabel()->setOpacity(0);
		}
		else
			timePlusTile->ToggleTouch(true);

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
		showMatchText->setPositionX(showMatchTile->getPositionX() + showMatchTile->GetWidth() / 2 - showMatchText->GetScaledWidth() / 2 );

		sprintf(sz, "%d / %d", earnedNewCardBonuses, purchasedNewCardBonuses);
		newCardText->setString(sz);
		newCardText->setPositionX(newCardTile->getPositionX() + newCardTile->GetWidth() / 2 - newCardText->GetScaledWidth() / 2);

		if (bonusTimePlusLeft > 0)
		{
			timePlusTile->ToggleEnabled(true);
			timePlusTile->setOpacity(255);
			timePlusText->GetLabel()->setOpacity(255);
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

		sceneTimer.Restart();
		countdownTimer.Restart();
		speedTimer_5_matches.Restart();
		speedTimer_10_matches.Restart();
		speedTimer_20_matches.Restart();
		warningTime = 3000;
	}

	return;
}

void Countdown::CalculateScore()
{
	finalScore = (matches * matchMultiplier) + speedBonus + extraPoints;
	displayedScore = finalScore;
	return;
}

void Countdown::ResetScore()
{
	matches = startingMatches;
	speedBonus = 0;
	extraPoints = 0;
	previousSpeedBonus = 0;
	earnedEliminateBonuses = 0;
	earnedTimePlusBonuses = 0;
	earnedShowMatchBonuses = 0;
	earnedNewCardBonuses = 0;
	msAdded = 0;
	return;
}

void Countdown::UseBonusTimePlus()
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

void Countdown::UseNewCard()
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

void Countdown::CheckForNewBests()
{
	CalculateScore();
	//Check for new best score
	if (finalScore > highestScore)
	{
		bestScoreIcon->setVisible(true);
		bestScoreIcon->runAction(cocos2d::FadeIn::create(0.25));
		highestScore = finalScore;
		gameData->setIntegerForKey(best_score_countdown, highestScore);
	}

	//Check for new best match
	if (matches > highestMatch)
	{
		bestMatchIcon->setVisible(true);
		bestMatchIcon->runAction(cocos2d::FadeIn::create(0.25));
		highestMatch = matches;
		gameData->setIntegerForKey(best_match_countdown, highestMatch);
	}

	gameData->setIntegerForKey(total_points, gameData->getIntegerForKey(total_points, 0) + finalScore);
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	sdkbox::PluginSdkboxPlay::submitScore("high_score_countdown", gameData->getIntegerForKey(best_score_countdown, highestScore));
	sdkbox::PluginSdkboxPlay::submitScore("highest_match_countdown", gameData->getIntegerForKey(best_match_countdown, highestMatch));
	sdkbox::PluginSdkboxPlay::submitScore("total_score", gameData->getIntegerForKey(total_points, 0));
	sdkbox::PluginSdkboxPlay::submitScore("total_matches", gameData->getIntegerForKey(total_matches, 0));
#endif

	return;
}

void Countdown::UpdateTimer()
{
	//Get how much time has passed in ms
	msLeft -= countdownTimer.GetElapsedTime();
	countdownTimer.Restart();

	if (msLeft >= 90000)
		timerReached90 = true;

	if (msLeft <= warningTime)
	{
		audio->PlayClip("warning_tone");
		warningTime -= 1000;
	}

	//Set clock display to 0 after time is up
	if (msLeft <= 0)
	{
		if (msLeft > -500)
		{
			timerText->setString("00.00");
			timerText->Center();
			timerText->SetBottom(timeRemainingBottomPosition);

			//Disable all powerups
			if (bonusEliminateLeft > 0)
			{
				eliminateTile->setOpacity(64);
				eliminateText->GetLabel()->setOpacity(64);
			}

			if (bonusShowMatchLeft > 0)
			{
				showMatchTile->setOpacity(64);
				showMatchText->GetLabel()->setOpacity(64);
			}
		}

		//Give player an extra 500ms to account for human delayed perception. In other words, they don't think the game is cheating them.
		else if (msLeft <= -500)
		{
			gameOver = true;
            
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
            audio->SetMusicVolume(0.35);
#endif
            
#if(CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
            audio->SetMusicVolume(Game::GetiOSMinVolumeBG());
#endif
    
            if (bonusTimePlusLeft > 0)
            {
                timePlusTile->setOpacity(64);
                timePlusText->GetLabel()->setOpacity(64);
            }
            
            if (bonusNewCardLeft > 0)
            {
                newCardTile->setOpacity(64);
                newCardText->GetLabel()->setOpacity(64);
            }

			currentSelection = 1;
			audio->PlayClip("double_tone_low");

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

			//Reveal match type for blind match
			blindMatch = false;
			if (scoreText->getString() == "???")
				UpdateHeaderText(matchCriteria);

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
}

void Countdown::DisplayMatchSummary()
{
	std::vector<std::string> matchMadeListStrings;
	std::string matchPoints;
	std::string speedPoints;
	std::string timeAdded;
	std::string nextBonus;

	sprintf(sz, "Match +100");
	matchPoints.append(sz);
	matchMadeListStrings.push_back(matchPoints);

	if (previousSpeedBonus > 0)
	{
		sprintf(sz, "Speed +%d", previousSpeedBonus);
		speedPoints.append(sz);
		matchMadeListStrings.push_back(speedPoints);
	}

	float decimal = (float(msAdded) / 1000 - int(msAdded / 1000)) * 10;
	int whole = msAdded / 1000;

	sprintf(sz, "+%d.%d seconds", whole, int(decimal));
	timeAdded.append(sz);
	matchMadeListStrings.push_back(timeAdded);

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

bool Countdown::CheckForAchievements()
{
	bool earnedAllAchievements = Game::CheckForAchievements();
    
    //If testing, ignore checking other achievements
    if(testing && testingAchievements)
        return true;

	if (!gameData->getBoolForKey(reach_match_10_countdown, false))
	{
		if (matches >= 10)
		{
			gameData->setBoolForKey(reach_match_10_countdown, true);
			achievementsUnlocked++;
			achievementsText.push_back(nrgText::create("Reach 10 Matches (Countdown)\n\nHey, you're getting the hang of this!", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));
		}
		else
			earnedAllAchievements = false;
	}

    if (!gameData->getBoolForKey(reach_match_20_countdown, false))
	{
		if (matches >= 20)
		{
			gameData->setBoolForKey(reach_match_20_countdown, true);
			achievementsUnlocked++;
			achievementsText.push_back(nrgText::create("Reach 20 Matches (Countdown)\n\nThat wasn't too hard, right?", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));
		}
		else
			earnedAllAchievements = false;
	}
    
    if (!gameData->getBoolForKey(reach_match_30_countdown, false))
	{
		if (matches >= 30)
		{
			gameData->setBoolForKey(reach_match_30_countdown, true);
			achievementsUnlocked++;
			achievementsText.push_back(nrgText::create("Reach 30 Matches (Countdown)\n\nOk then, we'll have to crank up the difficulty...", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));
		}
		else
			earnedAllAchievements = false;
	}

	if (!gameData->getBoolForKey(reach_match_50_countdown, false))
	{
		if (matches >= 50)
		{
			gameData->setBoolForKey(reach_match_50_countdown, true);
			achievementsUnlocked++;
			achievementsText.push_back(nrgText::create("Reach 50 Matches (Countdown)\n\nYou've got 50 matches under your belt. You know the drill. You can do this.", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));
		}
		else
			earnedAllAchievements = false;
	}

	if (!gameData->getBoolForKey(reach_match_60_countdown, false))
	{
		if (matches >= 60)
		{
			gameData->setBoolForKey(reach_match_60_countdown, true);
			achievementsUnlocked++;
			achievementsText.push_back(nrgText::create("Reach 60 Matches (Countdown)\n\nI think we have a natural-born matcher over here!", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));
		}
		else
			earnedAllAchievements = false;
	}

	if (!gameData->getBoolForKey(reach_match_70_countdown, false))
	{
		if (matches >= 70)
		{
			gameData->setBoolForKey(reach_match_70_countdown, true);
			achievementsUnlocked++;
			achievementsText.push_back(nrgText::create("Reach 70 Matches (Countdown)\n\nSuch precision. Such efficiency. You are a true Tap 3 Prodigy.", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));
		}
		else
			earnedAllAchievements = false;
	}

	if (!gameData->getBoolForKey(reach_match_80_countdown, false))
	{
		if (matches >= 80)
		{
			gameData->setBoolForKey(reach_match_80_countdown, true);
			achievementsUnlocked++;
			achievementsText.push_back(nrgText::create("Reach 80 Matches (Countdown)\n\nYou just earned some bragging rights.", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));
		}
		else
			earnedAllAchievements = false;
	}

	if (!gameData->getBoolForKey(reach_match_90_countdown, false))
	{
		if (matches >= 90)
		{
			gameData->setBoolForKey(reach_match_90_countdown, true);
			achievementsUnlocked++;
			achievementsText.push_back(nrgText::create("Reach 90 Matches (Countdown)\n\nIs there nothing we can do to stop you???", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));
		}
		else
			earnedAllAchievements = false;
	}

	if (!gameData->getBoolForKey(no_bonus_items_used_first_50_matches_countdown, false))
	{
		if (noBonusesUsedBefore50Matches)
		{
			gameData->setBoolForKey(no_bonus_items_used_first_50_matches_countdown, true);
			achievementsUnlocked++;
			achievementsText.push_back(nrgText::create("No Assistance Necessary (Countdown): Get to 50 Matches in Countdown Without Using Bonus Items\n\nImpressive. That kind of restraint doesn't go unnoticed!", "fonts/POE.ttf", achievementTextSize - 1, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));
		}
		else
			earnedAllAchievements = false;
	}

	if (!gameData->getBoolForKey(remaining_time_90_seconds_countdown, false))
	{
		if (timerReached90)
		{
			gameData->setBoolForKey(remaining_time_90_seconds_countdown, true);
			achievementsUnlocked++;
			achievementsText.push_back(nrgText::create("\nTime to Spare: Increase Remaining Time to 90 Seconds or More in Countdown\n\nYou're either very fast, or used some bonus items. Either way, nice work.", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));
		}
		else
			earnedAllAchievements = false;
	}

	if (!gameData->getBoolForKey(maintain_time_above_20, false))
	{
		if (remainingTimeAbove20)
		{
			gameData->setBoolForKey(maintain_time_above_20, true);
			achievementsUnlocked++;
			achievementsText.push_back(nrgText::create("\nTime Keeper: Keep Your Remaining Time Above 20 Seconds for 50 Matches in Countdown\n\nLook at those thumbs go! You waste no time making matches.", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));
		}
		else
			earnedAllAchievements = false;
	}

	if (!gameData->getBoolForKey(first_5_matches_in_12_seconds, false))
	{
		if (earnedSpeedDemon1)
		{
			gameData->setBoolForKey(first_5_matches_in_12_seconds, true);
			achievementsUnlocked++;
			achievementsText.push_back(nrgText::create("Speed Demon: Get First 5 Matches in 12 Seconds in Countdown\n\nThat's some quick thinking!", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));
		}
		else
			earnedAllAchievements = false;
	}

	if (!gameData->getBoolForKey(first_10_matches_in_24_seconds, false))
	{
		if (earnedSpeedDemon2)
		{
			gameData->setBoolForKey(first_10_matches_in_24_seconds, true);
			achievementsUnlocked++;
			achievementsText.push_back(nrgText::create("Speed Demon II: Get First 10 Matches in 24 Seconds in Countdown\n\nYou are lightning fast!", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));
		}
		else
			earnedAllAchievements = false;
	}

	if (!gameData->getBoolForKey(first_20_matches_in_50_seconds, false))
	{
		if (earnedSpeedDemon3)
		{
			gameData->setBoolForKey(first_20_matches_in_50_seconds, true);
			achievementsUnlocked++;
			achievementsText.push_back(nrgText::create("Speed Demon III: Get First 20 Matches in 50 Seconds in Countdown\n\nCongrats. That's actually faster than we thought humanly possible!", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));
		}
		else
			earnedAllAchievements = false;
	}

    if (!gameData->getBoolForKey(reach_match_100_countdown, false))
    {
        if (matches >= 100)
        {
			gameData->setBoolForKey(reach_match_100_countdown, true);
            achievementsUnlocked++;
            achievementsUnlocked++; //Need extra one for second page of text
            achievementsText.push_back(nrgText::create("Mastered Countdown: Reach 100 Matches\n\nCongratulations! You've completed 100 Countdown Matches! You've got some fast thumbs and a keen eye!", "fonts/POE.ttf", achievementTextSize - 1, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));
            achievementsText.push_back(nrgText::create("Mastered Countdown: Reach 100 Matches\n\nThank you soooo-oooo much for enjoying Tap 3!!! We hope you still have time for a few more rounds!", "fonts/POE.ttf", achievementTextSize - 1, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));
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

void Countdown::RandomizeTiles()
{
	Game::RandomizeTiles();
	
    //If over match 60, create a "tricky" round for color
    if (matches >= 60 && matchCriteria == COLORS)
    {
        //Reset tiles and matches
        excludedTiles.clear();
        matchingTiles.clear();
        matchingTilesVector.clear();
        excludedColorPairs.clear();
        excludedStylePairs.clear();
        excludedColors.clear();
        excludedShapes.clear();
        excludedStyles.clear();
        
        std::vector<int> nonmatchingTiles;
        
        std::pair<int, int> matchingColorKey; //The matching color key
        std::pair<int, int> reverseMatchingColorKey; //The oppositie (used to excluded from random generation)
        std::pair<cocos2d::Color3B, cocos2d::Color3B> matchingColor; //The matching color pair
        std::pair<int, int> nonmatchingColorKey; //The first non-matching key, which has 2 of the same colors
        std::pair<cocos2d::Color3B, cocos2d::Color3B> nonmatchingColor; //The first nonmatching color pair, which has 2 of the same color
        
        //Exclude all same color pairs
        excludedColorPairs.emplace(0, 0);
        excludedColorPairs.emplace(1, 1);
        excludedColorPairs.emplace(2, 2);
        excludedColorPairs.emplace(3, 3);
        excludedColorPairs.emplace(4, 4);
        excludedColorPairs.emplace(5, 5);
        excludedColorPairs.emplace(6, 6);
        
        //Set matching tile color
        matchingColorKey = nrgFunctions::GetRandomPair(0, 6, 0, 6, excludedColorPairs);
        matchingColor = GetColorFromKey(matchingColorKey);
        
        //Excluded the colors chosen for matching tile
        std::unordered_set<int> excludedColors;
        excludedColors.emplace(matchingColorKey.first);
        excludedColors.emplace(matchingColorKey.second);
        
        //Set colors for set 1 of nonmatching tiles, which will be completely different than matching colors. 2 same, one reverse
        nonmatchingColorKey.first = nrgFunctions::GetRandom(0, 6, excludedColors); //Get any color not used in matching color
        excludedColors.emplace(nonmatchingColorKey.first); //Excluded first color to prevent solid color
        nonmatchingColorKey.second = nrgFunctions::GetRandom(0, 6, excludedColors); //Get any color not used in matching color or nonmatching first color
        nonmatchingColor = GetColorFromKey(nonmatchingColorKey);
        
        //Create color for the reverse part of set
        std::pair<cocos2d::Color3B, cocos2d::Color3B> nonmatchingColor2; //The reverse of nonmatchingColor
        nonmatchingColor2.first = nonmatchingColor.second;
        nonmatchingColor2.second = nonmatchingColor.first;
        
        //Third set of nonmatching tiles, created from top of matching and top of reverse nonmatch. 2 same, one reverse
        std::pair<cocos2d::Color3B, cocos2d::Color3B> nonmatchingColor3;
        nonmatchingColor3.first = nonmatchingColor2.first;
        nonmatchingColor3.second = matchingColor.first;
        
        //Create color for the reverse part of set
        std::pair<cocos2d::Color3B, cocos2d::Color3B> nonmatchingColor4;
        nonmatchingColor4.first = nonmatchingColor3.second;
        nonmatchingColor4.second = nonmatchingColor3.first;
        
        for (int iii = 0; iii < 3; iii++)
        {
            matchingTilesVector.push_back(nrgFunctions::GetRandom(TOP_LEFT, BOTTOM_RIGHT, excludedTiles));
            matchingTiles.emplace(matchingTilesVector[iii]);
            excludedTiles.emplace(matchingTilesVector[iii]);
        }
        
        for (int iii = 0; iii < 6; iii++)
        {
            nonmatchingTiles.push_back(nrgFunctions::GetRandom(TOP_LEFT, BOTTOM_RIGHT, excludedTiles));
            excludedTiles.emplace(nonmatchingTiles[iii]);
        }
        
        //Matching Tile 1
        tiles[matchingTilesVector[0]]->ChangeShape(
                                                   tiles[matchingTilesVector[0]]->GetShape(),
                                                   matchingColor, 0, tiles[matchingTilesVector[0]]->GetStyle());
        
        //Matching Tile 2
        tiles[matchingTilesVector[1]]->ChangeShape(
                                                   tiles[matchingTilesVector[1]]->GetShape(),
                                                   matchingColor, 0, tiles[matchingTilesVector[1]]->GetStyle());
        
        //Matching Tile 3
        tiles[matchingTilesVector[2]]->ChangeShape(
                                                   tiles[matchingTilesVector[2]]->GetShape(),
                                                   matchingColor, 0, tiles[matchingTilesVector[2]]->GetStyle());
        
        
        //Nonmatching Set 1 (2 tiles with same non-matching colors, different from matching tiles, one reverse tile)
        
        //Nonmatching Tile 1
        tiles[nonmatchingTiles[0]]->ChangeShape(
                                                tiles[nonmatchingTiles[0]]->GetShape(),
                                                nonmatchingColor, 0, tiles[nonmatchingTiles[0]]->GetStyle());
        
        //Nonmatching Tile 2
        tiles[nonmatchingTiles[1]]->ChangeShape(
                                                tiles[nonmatchingTiles[1]]->GetShape(),
                                                nonmatchingColor, 0, tiles[nonmatchingTiles[1]]->GetStyle());
        
        //Nonmatching Tile 3 (reverse)
        tiles[nonmatchingTiles[2]]->ChangeShape(
                                                tiles[nonmatchingTiles[2]]->GetShape(),
                                                nonmatchingColor2, 0, tiles[nonmatchingTiles[2]]->GetStyle());
        
        
        //Nonmatching Set 2 (Top color of non-matching tile and top color of matching tile, 2 same, one reverse)
        
        //Nonmatching Tile 1
        tiles[nonmatchingTiles[3]]->ChangeShape(
                                                tiles[nonmatchingTiles[3]]->GetShape(),
                                                nonmatchingColor3, 0, tiles[nonmatchingTiles[3]]->GetStyle());
        
        //Nonmatching Tile 2
        tiles[nonmatchingTiles[4]]->ChangeShape(
                                                tiles[nonmatchingTiles[4]]->GetShape(),
                                                nonmatchingColor3, 0, tiles[nonmatchingTiles[4]]->GetStyle());
        
        //Nonmatching Tile 3 (reverse)
        tiles[nonmatchingTiles[5]]->ChangeShape(tiles[nonmatchingTiles[5]]->GetShape(),
        nonmatchingColor4, 0, tiles[nonmatchingTiles[5]]->GetStyle());
    }
    
    //If over match 90, create a "tricky" round for styles
    
    if (matches >= 90 && matchCriteria == STYLES)
    {
        //Reset tiles and matches
		excludedTiles.clear();
		matchingTiles.clear();
		matchingTilesVector.clear();
		excludedColorPairs.clear();
		excludedStylePairs.clear();
		excludedColors.clear();
		excludedShapes.clear();
		excludedStyles.clear();

		//Exclude all same style pairs
		excludedStylePairs.emplace(0, 0);
		excludedStylePairs.emplace(1, 1);
		excludedStylePairs.emplace(2, 2);
		excludedStylePairs.emplace(3, 3);
		excludedStylePairs.emplace(4, 4);
		excludedStylePairs.emplace(5, 5);
		excludedStylePairs.emplace(6, 6);
		excludedStylePairs.emplace(7, 7);
		excludedStylePairs.emplace(8, 8);
		excludedStylePairs.emplace(9, 9);
		excludedStylePairs.emplace(10, 10);
		excludedStylePairs.emplace(11, 11);

		std::vector<int> nonmatchingTiles;

		for (int iii = 0; iii < 3; iii++)
		{
			matchingTilesVector.push_back(nrgFunctions::GetRandom(TOP_LEFT, BOTTOM_RIGHT, excludedTiles));
			matchingTiles.emplace(matchingTilesVector[iii]);
			excludedTiles.emplace(matchingTilesVector[iii]);
		}

		for (int iii = 0; iii < 6; iii++)
		{
			nonmatchingTiles.push_back(nrgFunctions::GetRandom(TOP_LEFT, BOTTOM_RIGHT, excludedTiles));
			excludedTiles.emplace(nonmatchingTiles[iii]);
		}

		std::pair<int, int> matchingStyle;
		std::pair<int, int> nonmatchingStyle1;
		std::pair<int, int> nonmatchingStyle1B;
		std::pair<int, int> nonmatchingStyle2;
		std::pair<int, int> nonmatchingStyle2B;

		//Set matching tile style
		matchingStyle = nrgFunctions::GetRandomPair(4, 11, 4, 11, excludedStylePairs);
		excludedStylePairs.emplace(matchingStyle);

		//Matching Tile 1
		tiles[matchingTilesVector[0]]->ChangeShape(
			tiles[matchingTilesVector[0]]->GetShape(),
			tiles[matchingTilesVector[0]]->GetColor(), 0, matchingStyle);

		//Matching Tile 2
		tiles[matchingTilesVector[1]]->ChangeShape(
			tiles[matchingTilesVector[1]]->GetShape(),
			tiles[matchingTilesVector[1]]->GetColor(), 0, matchingStyle);

		//Matching Tile 3
		tiles[matchingTilesVector[2]]->ChangeShape(
			tiles[matchingTilesVector[2]]->GetShape(),
			tiles[matchingTilesVector[2]]->GetColor(), 0, matchingStyle);


		//Nonmatching Set 1
		nonmatchingStyle1 = nrgFunctions::GetRandomPair(4, 11, 4, 11, excludedStylePairs);
		nonmatchingStyle1B.first = nonmatchingStyle1.second;
		nonmatchingStyle1B.second = nonmatchingStyle1.first;
		excludedStylePairs.emplace(nonmatchingStyle1);
		excludedStylePairs.emplace(nonmatchingStyle1B);

		//Nonmatching Tile 1
		tiles[nonmatchingTiles[0]]->ChangeShape(
			tiles[nonmatchingTiles[0]]->GetShape(),
			tiles[nonmatchingTiles[0]]->GetColor(), 0, nonmatchingStyle1);

		//Nonmatching Tile 2
		tiles[nonmatchingTiles[1]]->ChangeShape(
			tiles[nonmatchingTiles[1]]->GetShape(),
			tiles[nonmatchingTiles[1]]->GetColor(), 0, nonmatchingStyle1);

		//Nonmatching Tile 3
		tiles[nonmatchingTiles[2]]->ChangeShape(
			tiles[nonmatchingTiles[2]]->GetShape(),
			tiles[nonmatchingTiles[2]]->GetColor(), 0, nonmatchingStyle1B);


		//Nonmatching Set 2
		nonmatchingStyle2 = nrgFunctions::GetRandomPair(4, 11, 4, 11, excludedStylePairs);
		nonmatchingStyle2B.first = nonmatchingStyle2.second;
		nonmatchingStyle2B.second = nonmatchingStyle2.first;
		excludedStylePairs.emplace(nonmatchingStyle2);
		excludedStylePairs.emplace(nonmatchingStyle2B);

		//Nonmatching Tile 1
		tiles[nonmatchingTiles[3]]->ChangeShape(
			tiles[nonmatchingTiles[3]]->GetShape(),
			tiles[nonmatchingTiles[3]]->GetColor(), 0, nonmatchingStyle2);

		//Nonmatching Tile 2
		tiles[nonmatchingTiles[4]]->ChangeShape(
			tiles[nonmatchingTiles[4]]->GetShape(),
			tiles[nonmatchingTiles[4]]->GetColor(), 0, nonmatchingStyle2);

		//Nonmatching Tile 3
		tiles[nonmatchingTiles[5]]->ChangeShape(
			tiles[nonmatchingTiles[5]]->GetShape(),
			tiles[nonmatchingTiles[5]]->GetColor(), 0, nonmatchingStyle2B);
	}

	return;
}

void Countdown::Reset()
{	
	Game::Reset();

	timerReached90 = false;

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
		pointsText->GetLabel()->runAction(cocos2d::FadeIn::create(0.5));

		optionsButton->addEvents();
		optionsButton->ToggleTouch(false);
		highScoresButton->addEvents();
		highScoresButton->ToggleTouch(false);
		buyBonusItemsButton->addEvents();
		buyBonusItemsButton->ToggleTouch(false);
	}), NULL));
	
	return;
}

void Countdown::SetMatchParameters(int excludedMatchCriteria, bool blind)
{
	//Standard start
	Game::SetMatchParameters();

	//Determine match criteria based on which match we're on

	//SHAPES PRACTICE
	if (matches < 10)
	{
		difficulty = matches / 3;

		//Match colors and shapes and styles

		if (excludedMatchCriteria != SHAPES)
			matchCriteria = SHAPES;
		else
		{
			matchCriteria = COLORS;
			excludedShapes.erase(HEXAGON);
		}

		matchingStyles = true;
		useDesignatedMatchStyle = true;
		useDesignatedNonmatchStyle = true;
		designatedMatchStyle.first = 0;
		designatedMatchStyle.second = 0;
		designatedNonmatchStyle.first = 0;
		designatedNonmatchStyle.second = 0;

		if (matchCriteria == SHAPES)
		{
			nonmatchedColorsCanExceedTwo = true;
			nonmatchedStylesCanExceedTwo = true;
		}

		switch (difficulty)
		{

		case 0:
			numberOfMatchingTiles = 5;
			if (matchCriteria == SHAPES)
			{
				matchingColors = true;
				numberOfSameNonmatchingColors = 4;
			}
			else
			{
				matchingShapes = true;
				numberOfSameNonmatchingShapes = 4;
			}
			break;
		case 1:
			numberOfMatchingTiles = 4;
			if (matchCriteria == SHAPES)
			{
				matchingColors = false;
				numberOfSameNonmatchingColors = 0;
				nonmatchedColorsCanExceedTwo = false;
			}
			break;
		case 2:
			numberOfMatchingTiles = 3;
			if (matchCriteria == SHAPES)
			{
				matchingColors = false;
				numberOfSameNonmatchingColors = 0;
				nonmatchedColorsCanExceedTwo = false;
			}
			break;
		}
	}

	//SHAPES OR COLORS
	else if (matches < 20)
	{
		matchingStyles = true;

		//Unless excluded using new card
		if (excludedMatchCriteria == COLORS)
			matchCriteria = SHAPES;
		else if (excludedMatchCriteria == SHAPES)
			matchCriteria = COLORS;
		else if (excludedMatchCriteria != COLORS && excludedMatchCriteria != SHAPES)
			matchCriteria = nrgFunctions::GetRandomChoice(COLORS, SHAPES);

		//Always make match 10 colors
		if (matches == 10 && excludedMatchCriteria != COLORS)
			matchCriteria = COLORS;
		else if (matches == 10 && excludedMatchCriteria == COLORS)
			matchCriteria = SHAPES;

		useDesignatedMatchStyle = true;
		useDesignatedNonmatchStyle = true;
		designatedMatchStyle.first = 0;
		designatedMatchStyle.second = 0;
		designatedNonmatchStyle.first = 0;
		designatedNonmatchStyle.second = 0;
		excludedShapes.erase(HEXAGON);
	}

	//STYLES PRACTICE
	else if (matches < 26)
	{
		difficulty = (matches - 20) / 2;

		if (excludedMatchCriteria != STYLES)
		{
			matchCriteria = STYLES;

			//Match colors and shapes and styles
			matchingShapes = true;
			matchingColors = true;
		}

		else
		{
			matchCriteria = nrgFunctions::GetRandomChoice(SHAPES, COLORS);
			excludedStyles.erase(BLINDS);
			difficulty = 3;
		}

		switch (difficulty)
		{

		case 0:
			numberOfSameNonmatchingColors = 6;
			nonmatchedColorsCanExceedTwo = true;
			break;
		case 1:
			matchingShapes = false;
			numberOfSameNonmatchingColors = 4;
			nonmatchedColorsCanExceedTwo = true;
			break;
		case 2:
			matchingColors = false;
			matchingShapes = false;
			numberOfSameNonmatchingColors = 2;
			break;
		default:
			break;
		}
	}

	//
	//COLORS, SHAPES, or STYLES--possibility for Blind Match, exclude previous match criteria if player is using Pass
	//

	else
	{
		//Random match criteria after warm up rounds, or keep the same when rerandomizing
		if (excludedMatchCriteria >= 0)
		{ 
			matchCriteria = nrgFunctions::GetRandom(COLORS, STYLES, excludedMatchCriteria);
			excludedShapes.erase(HEXAGON);
			excludedStyles.erase(BLINDS);
		}
		
		//Randomize as usual
		else if (excludedMatchCriteria == -1)
			matchCriteria = nrgFunctions::GetRandom(COLORS, STYLES);
		
		//Keep same criteria as before
		else if (excludedMatchCriteria == -2)
			matchCriteria = matchCriteria;

			//1 in 10 chance of blind match
			int rand = nrgFunctions::GetRandom(1, 10);

			if (rand == 5 || blind)
				blindMatch = true;
	}

	//Don't allow split color matches to have different style until match 50
	if (matches >= 30 && matches < 50 && matchCriteria == COLORS)
	{
		useDesignatedMatchStyle = true;
		useDesignatedNonmatchStyle = true;
		designatedMatchStyle.first = SOLID;
		designatedMatchStyle.second = SOLID;
		designatedNonmatchStyle.first = SOLID;
		designatedNonmatchStyle.second = SOLID;
	}

	//Introduce possibility of Octagons and Hexagons after Match 15
	if (matches >= 15)
	{
		excludedShapes.erase(OCTAGON);
		excludedShapes.erase(HEXAGON);
	}

	//Introduce blinds style to allow 9 possible tiles after Style Practice
	if (matches >= 26)
		excludedStyles.erase(BLINDS);

	if (matches >= 30)
		splitColor = true;

	//Begin showing false positives after Match 40
	if (matches >= 40)
	{
		nonmatchedColorsCanExceedTwo = true;
		nonmatchedShapesCanExceedTwo = true;
		nonmatchedStylesCanExceedTwo = true;
	}

	//After match 50, Color matches can now have styles overlaid onto them (SEE ABOVE)

    //At match 60, colors have tricky set
    
	//Introduce more difficult styles at Match 70
	if (matches >= 70)
	{
		excludedStyles.erase(BLINDS_QUAD);
		excludedStyles.erase(OUTLINE_BLINDS);
		excludedStyles.erase(OUTLINE_QUAD);
		excludedStyles.erase(CONCENTRIC_BLINDS);
		excludedStyles.erase(CONCENTRIC_QUAD);
	}

	//Introduce final styles
	if (matches >= 80)
	{
		excludedStyles.erase(OUTLINE_BLINDS_QUAD);
		excludedStyles.erase(CONCENTRIC_BLINDS_QUAD);
	}

	//Allow split style patterns after 90
	if (matches >= 90)
	{
		int split = nrgFunctions::GetRandom(0, 2);
		if (split == 1)
			splitStyle = true;
	}

	//All rounds past Match 95 are blind
	if (matches >= 95)
		blindMatch = true;
		
	//If blind match, must make sure only one possible match exists, and must leave enough possible options to create 9 tiles
	if (blindMatch)
	{
		nonmatchedColorsCanExceedTwo = false;
		nonmatchedShapesCanExceedTwo = false;
		nonmatchedStylesCanExceedTwo = false;
		numberOfSameNonmatchingColors = 0;
		numberOfSameNonmatchingShapes = 0;
		numberOfSameNonmatchingStyles = 0;
		useDesignatedNonmatchColor = false;
		useDesignatedNonmatchShape = false;
		useDesignatedNonmatchStyle = false;
		numberOfMatchingTiles = 3;
		excludedShapes.erase(OCTAGON);
		excludedStyles.erase(QUAD);
		excludedStyles.erase(CONCENTRIC);
		excludedStyles.erase(BLINDS);
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

void Countdown::onEnter()
{
    mode = COUNTDOWN;
    
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    sdkbox::PluginSdkboxPlay::setListener(this);
#endif
    
	Game::onEnter();
	return;
}

void Countdown::onExit()
{
    showingAd = false;
    
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    sdkbox::PluginSdkboxPlay::removeListener();
#endif
    
	Game::onExit();
	return;
}

Countdown::~Countdown()
{
	timers.clear();
}

#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
void Countdown::onConnectionStatusChanged(int status)
{
}

void Countdown::onScoreSubmitted(const std::string & leaderboard_name, int score, bool maxScoreAllTime, bool maxScoreWeek, bool maxScoreToday)
{
}

void Countdown::onIncrementalAchievementUnlocked(const std::string & achievement_name)
{
}

void Countdown::onIncrementalAchievementStep(const std::string & achievement_name, int step)
{
}

void Countdown::onAchievementUnlocked(const std::string & achievement_name, bool newlyUnlocked)
{
}
#endif