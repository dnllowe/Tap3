#pragma once
#include "pch.h"

Classic::Classic()
{
}

cocos2d::Scene * Classic::CreateScene()
{
	//Create Scene
	cocos2d::Scene* scene = cocos2d::Scene::create();

	//Create layer
	cocos2d::Layer* layer = Classic::create();

	scene->addChild(layer);

	return scene;
}

bool Classic::init()
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

	if (testing)
	{
		timelimit = 25000;
		initialTimelimit = 25000;
	}

	//Game Mode Specific variables
	startingMs = timelimit;
    msLeft = startingMs;
	matchMilestone = 5;
	nextMilestone = 5;
	gameStarted = true;
	mode = CLASSIC;

	InitGameAssets();
    timePlusTile->ToggleEnabled(false); //Not needed for this mode
    timePlusText->GetLabel()->setVisible(false); //Not needed for this mode

    Reset();

	highestMatch = gameData->getIntegerForKey(best_match_classic, 0);
	highestScore = gameData->getIntegerForKey(best_score_classic, 0);

	//Add timers to vector so they can be controlled outside of class methods
	timers.push_back(&sceneTimer);
	timers.push_back(&countdownTimer);
	timers.push_back(&bonusTimer);
	timers.push_back(&sequenceTimer);
    
    warningTime = 1000;
    
    //Begin Game Loop
	scheduleUpdate();
	return true;
}

void Classic::update(float dt)
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
			gameData->setIntegerForKey(classic_matches, gameData->getIntegerForKey(classic_matches, 0) + 1);
			CheckSequence();
			bonusTime = 0;
			timerText->GetLabel()->runAction(cocos2d::RepeatForever::create(
				cocos2d::Sequence::create(
					cocos2d::DelayTime::create(0.125),
					cocos2d::FadeTo::create(0.5, 64),
					cocos2d::FadeTo::create(0.5, 255),
					cocos2d::DelayTime::create(0.125), NULL)));

			//Check for no bonuses used achievement
			if (matches == 50 && bonusesUsed == 0)
				noBonusesUsedBefore50Matches = true;

			//Check for near miss achievements
			if (msLeft <= 0)
			{
				nearMiss = true;
				lastMatchWasNearMiss = true;
			}
			
			else
			{
				gameData->setIntegerForKey(consecutive_near_misses, 0);
				lastMatchWasNearMiss = false;
			}

			if (lastMatchWasNearMiss)
				gameData->setIntegerForKey(consecutive_near_misses, gameData->getIntegerForKey(consecutive_near_misses, 0) + 1);

			countdownTimer.Reset();
			msLeft = timelimit;

			if (matchCriteria == COLORS)
				gameData->setIntegerForKey(total_color_matches, gameData->getIntegerForKey(total_color_matches, 0) + 1);
			if (matchCriteria == SHAPES)
				gameData->setIntegerForKey(total_shape_matches, gameData->getIntegerForKey(total_shape_matches, 0) + 1);
			if (matchCriteria == STYLES)
				gameData->setIntegerForKey(total_style_matches, gameData->getIntegerForKey(total_style_matches, 0) + 1);

			if (matches != matchMilestone)
				audio->PlayClip("double_tone_high");

			if (matches == matchMilestone)
			{
				audio->PlayClip("triple_tone_high");
				generateBonus = true;
				matchMilestone += nextMilestone;
			}

			//Reset warning time to correct threshold 
			warningTime = 1000;

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
            bonusTime = 0;
            
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
			sceneTimer.SetMark(powerupEnableDelay);
		}

		countdownTimer.Restart();
		timerText->GetLabel()->stopAllActions();
		timerText->GetLabel()->setOpacity(255);
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
        
        //For testing purposes
		matches = startingMatches;
		timelimit = initialTimelimit;
		startingMs = timelimit;
        msLeft = startingMs;
		matchMilestone = 5;
		nextMilestone = 5;
		timerText->setString(startTimeString);
        timerText->Center();
        timerText->SetBottom(timeRemainingBottomPosition);
        timerText->GetLabel()->setTextColor(cocos2d::Color4B::BLACK);

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
        bonusTimePlusLeft = 0;
		bonusShowMatchLeft = purchasedShowMatchBonuses;
		bonusNewCardLeft = purchasedNewCardBonuses;

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
		sprintf(sz, "%d / %d", earnedEliminateBonuses, purchasedEliminateBonuses);
		eliminateText->setString(sz);
		eliminateText->setPositionX(eliminateTile->getPositionX() + eliminateTile->GetWidth() / 2 - eliminateText->GetScaledWidth() / 2);

		sprintf(sz, "%d / %d", earnedShowMatchBonuses, purchasedShowMatchBonuses);
		showMatchText->setString(sz);
		showMatchText->setPositionX(showMatchTile->getPositionX() + showMatchTile->GetWidth() / 2 - showMatchText->GetScaledWidth() / 2);

		sprintf(sz, "%d / %d", earnedNewCardBonuses, purchasedNewCardBonuses);
		newCardText->setString(sz);
		newCardText->setPositionX(newCardTile->getPositionX() + newCardTile->GetWidth() / 2 - newCardText->GetScaledWidth() / 2);

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

		warningTime = 1000;
	}

	return;
}

void Classic::CalculateScore()
{
	finalScore = (matches * matchMultiplier) + extraPoints;
	displayedScore = finalScore;

	return;
}

void Classic::ResetScore()
{
	matches = startingMatches;
	extraPoints = 0;
	earnedEliminateBonuses = 0;
	earnedShowMatchBonuses = 0;
	earnedNewCardBonuses = 0;
	return;
}

void Classic::UseNewCard()
{
	currentSelection = 1;
	SetMatchParameters(matchCriteria);
	UpdateHeaderText(matchCriteria);
	RandomizeTiles();
	RestoreTiles(0);
	msLeft = timelimit;
	bonusTime = 0;
	warningTime = 1000;
	countdownTimer.Restart();

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

    bonusesUsed++;
	gameData->setIntegerForKey(new_card_used, gameData->getIntegerForKey(new_card_used, 0) + 1);

	return;
}

void Classic::CheckForNewBests()
{
	CalculateScore();
	//Check for new best score
	if (finalScore > highestScore)
	{
		bestScoreIcon->setVisible(true);
		bestScoreIcon->runAction(cocos2d::FadeIn::create(0.25));
		highestScore = finalScore;
		gameData->setIntegerForKey(best_score_classic, highestScore);
	}

	//Check for new best match
	if (matches > highestMatch)
	{
		bestMatchIcon->setVisible(true);
		bestMatchIcon->runAction(cocos2d::FadeIn::create(0.25));
		highestMatch = matches;
		gameData->setIntegerForKey(best_match_classic, highestMatch);
	}

	gameData->setIntegerForKey(total_points, gameData->getIntegerForKey(total_points, 0) + finalScore);
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	sdkbox::PluginSdkboxPlay::submitScore("high_score_classic", gameData->getIntegerForKey(best_score_classic, highestScore));
	sdkbox::PluginSdkboxPlay::submitScore("highest_match_classic", gameData->getIntegerForKey(best_match_classic, highestMatch));
	sdkbox::PluginSdkboxPlay::submitScore("total_score", gameData->getIntegerForKey(total_points, 0));
	sdkbox::PluginSdkboxPlay::submitScore("total_matches", gameData->getIntegerForKey(total_matches, 0));
#endif
	return;
}

void Classic::UpdateTimer()
{
	//Get how much time has passed in ms
	msLeft = timelimit + bonusTime - countdownTimer.GetElapsedTime();

	if (msLeft <= warningTime)
	{
		audio->PlayClip("warning_tone");
		warningTime = -500;
	}

	//Set clock display to 0 after time is up
	if (msLeft <= 0)
	{
		if (msLeft > -500)
		{
			timerText->setString("0.00");
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
			bonusTime = 0;

#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
            audio->SetMusicVolume(0.35);
#endif
            
#if(CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
            audio->SetMusicVolume(Game::GetiOSMinVolumeBG());
#endif
    
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
            
			//Reveal match type for blind match
			blindMatch = false;
			if (scoreText->getString() == "???")
				UpdateHeaderText(matchCriteria);

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

void Classic::DisplayMatchSummary()
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

bool Classic::CheckForAchievements()
{
	bool earnedAllAchievements = Game::CheckForAchievements();

    //Ignore checking achievements if testing
    if(testing && testingAchievements)
        return true;
		
	if (!gameData->getBoolForKey(reach_match_1_classic, false))
	{
		if (matches >= 1)
		{
			gameData->setBoolForKey(reach_match_1_classic, true);
			achievementsUnlocked++;
			achievementsText.push_back(nrgText::create("Difficult. Not Impossible\n\nYou got your first Classic Match! Give yourself a pat on the back. Not everyone can do that.", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));
		}
		else
			earnedAllAchievements = false;
	}

	if (!gameData->getBoolForKey(reach_match_10_classic, false))
	{
		if (matches >= 10)
		{
			gameData->setBoolForKey(reach_match_10_classic, true);
			achievementsUnlocked++;
			achievementsText.push_back(nrgText::create("Reach 10 Matches (Classic)\n\nI see potential in you...", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));
		}
		else
			earnedAllAchievements = false;
	}

    if (!gameData->getBoolForKey(reach_match_20_classic, false))
	{
		if (matches >= 20)
		{
			gameData->setBoolForKey(reach_match_20_classic, true);
			achievementsUnlocked++;
			achievementsText.push_back(nrgText::create("Reach 20 Matches (Classic)\n\nSee? 3 seconds is plenty of time!", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));
		}
		else
			earnedAllAchievements = false;
	}

    if (!gameData->getBoolForKey(reach_match_30_classic, false))
	{
		if (matches >= 30)
		{
			gameData->setBoolForKey(reach_match_30_classic, true);
			achievementsUnlocked++;
			achievementsText.push_back(nrgText::create("Reach 30 Matches (Classic)\n\nSorry, it's only going to get harder from here...", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));
		}
		else
			earnedAllAchievements = false;
	}

	if (!gameData->getBoolForKey(reach_match_50_classic, false))
	{
		if (matches >= 50)
		{
			gameData->setBoolForKey(reach_match_50_classic, true);
			achievementsUnlocked++;
			achievementsText.push_back(nrgText::create("Reach 50 Matches (Classic)\n\nI knew it. When I saw you 20 or so matches ago, I knew you had something special.", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));
		}
		else
			earnedAllAchievements = false;
	}

    if (!gameData->getBoolForKey(reach_match_60_classic, false))
	{
		if (matches >= 60)
		{
			gameData->setBoolForKey(reach_match_60_classic, true);
			achievementsUnlocked++;
			achievementsText.push_back(nrgText::create("Reach 60 Matches (Classic)\n\nYou should be feeling good about yourself. Classic is no walk in the park. Keep it up!", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));
		}
		else
			earnedAllAchievements = false;
	}

	if (!gameData->getBoolForKey(reach_match_70_classic, false))
	{
		if (matches >= 70)
		{
			gameData->setBoolForKey(reach_match_70_classic, true);
			achievementsUnlocked++;
			achievementsText.push_back(nrgText::create("Reach 70 Matches (Classic)\n\nWhew. Give your thumbs the rest of the week off!", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));
		}
		else
			earnedAllAchievements = false;
	}

	if (!gameData->getBoolForKey(reach_match_80_classic, false))
	{
		if (matches >= 80)
		{
			gameData->setBoolForKey(reach_match_80_classic, true);
			achievementsUnlocked++;
			achievementsText.push_back(nrgText::create("Reach 80 Matches (Classic)\n\nLet's take a moment to remember when you thought the first 20 matches were hard...", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));
		}
		else
			earnedAllAchievements = false;
	}

	if (!gameData->getBoolForKey(reach_match_90_classic, false))
	{
		if (matches >= 90)
		{
			gameData->setBoolForKey(reach_match_90_classic, true);
			achievementsUnlocked++;
			achievementsText.push_back(nrgText::create("Reach 90 Matches (Classic)\n\nMan, we're running out of achievements for you!", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));
		}
		else
			earnedAllAchievements = false;
	}

	if (!gameData->getBoolForKey(no_bonus_items_used_first_50_matches_classic, false))
	{
		if (noBonusesUsedBefore50Matches)
		{
			gameData->setBoolForKey(no_bonus_items_used_first_50_matches_classic, true);
			achievementsUnlocked++;
			achievementsText.push_back(nrgText::create("No Assistance Necessary (Classic): Get to 50 Matches in Classic Without Using Bonus Items\n\nImpressive. That kind of restraint doesn't go unnoticed!", "fonts/POE.ttf", achievementTextSize - 1, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));
		}
		else
			earnedAllAchievements = false;
	}

    if (!gameData->getBoolForKey(reach_match_100_classic, false))
    {
        if (matches >= 100)
        {
			gameData->setBoolForKey(reach_match_100_classic, true);
            achievementsUnlocked++;
            achievementsUnlocked++; //Need extra one for second page of text
            achievementsText.push_back(nrgText::create("Mastered Classic: Reach 100 Matches\n\nCongratulations! You've completed 100 Classic Matches! You've officially been classically trained in the art of Tap 3!", "fonts/POE.ttf", achievementTextSize - 2, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));
            achievementsText.push_back(nrgText::create("Mastered Classic: Reach 100 Matches\n\nThank you soooo-oooo much for enjoying Tap 3!!!", "fonts/POE.ttf", achievementTextSize, baseTile->GetWidth() - 10, 0, 0, cocos2d::Color4B::WHITE, cocos2d::TextHAlignment::CENTER));
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

void Classic::RandomizeTiles()
{
	Game::RandomizeTiles();
	return;
}

void Classic::Reset()
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
		pointsText->GetLabel()->runAction(cocos2d::FadeIn::create(0.5));

		optionsButton->addEvents();
		optionsButton->ToggleTouch(false);
		highScoresButton->addEvents();
		highScoresButton->ToggleTouch(false);
		buyBonusItemsButton->addEvents();
		buyBonusItemsButton->ToggleTouch(false);
		sceneTimer.Restart();
		countdownTimer.Restart();
	}), NULL));

	return;
}

void Classic::SetMatchParameters(int excludedMatchCriteria, bool blind)
{
	//Standard start
	Game::SetMatchParameters();

	difficulty = matches;

	excludedShapes.erase(HEXAGON);
	excludedStyles.erase(BLINDS);

	//Begin showing false positives after Match 10
	if (difficulty >= 10)
	{
		nonmatchedColorsCanExceedTwo = true;
		nonmatchedShapesCanExceedTwo = true;
		nonmatchedStylesCanExceedTwo = true;
	}

	if (difficulty < 25)
	{
		useDesignatedMatchStyle = true;
		useDesignatedNonmatchStyle = true;
		designatedMatchStyle.first = SOLID;
		designatedMatchStyle.second = SOLID;
		designatedNonmatchStyle.first = SOLID;
		designatedNonmatchStyle.second = SOLID;
	}

	//Determine match criteria based on which match we're on

	//Random match criteria after warm up rounds, or keep the same when rerandomizing
	if (excludedMatchCriteria >= 0)
	{
		if (difficulty < 25)
		{
			if (excludedMatchCriteria == SHAPES)
				matchCriteria = COLORS;
			else if (excludedMatchCriteria == COLORS)
				matchCriteria = SHAPES;
		}

		else if (difficulty >= 25)
		{
			if (excludedMatchCriteria == SHAPES)
				matchCriteria = nrgFunctions::GetRandomChoice(COLORS, STYLES);
			else if (excludedMatchCriteria == COLORS)
				matchCriteria = nrgFunctions::GetRandomChoice(SHAPES, STYLES);
			else if(excludedMatchCriteria == STYLES)
				matchCriteria = nrgFunctions::GetRandomChoice(COLORS, SHAPES);
		}
	}

	//Randomize as usual
	else if (excludedMatchCriteria == -1)
	{
		if (difficulty < 25)
			matchCriteria = nrgFunctions::GetRandomChoice(COLORS, SHAPES);

		else if (difficulty >= 25)
			matchCriteria = nrgFunctions::GetRandom(COLORS, STYLES);
	}

	//Keep same criteria as before
	else if (excludedMatchCriteria == -2)
		matchCriteria = matchCriteria;

	//Introduce possibility of Octagons after Match 15
	if (difficulty >= 15)
		excludedShapes.erase(OCTAGON);
	
    //Don't allow shape matches to have different style until match 50
    if (difficulty < 50 && matchCriteria == SHAPES)
    {
        useDesignatedMatchStyle = true;
        useDesignatedNonmatchStyle = true;
        designatedMatchStyle.first = SOLID;
        designatedMatchStyle.second = SOLID;
        designatedNonmatchStyle.first = SOLID;
        designatedNonmatchStyle.second = SOLID;
    }
    
    //Don't allow color matches to have different style until match 75
	if (difficulty < 75 && matchCriteria == COLORS)
	{
		useDesignatedMatchStyle = true;
		useDesignatedNonmatchStyle = true;
		designatedMatchStyle.first = SOLID;
		designatedMatchStyle.second = SOLID;
		designatedNonmatchStyle.first = SOLID;
		designatedNonmatchStyle.second = SOLID;
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

void Classic::onEnter()
{
	mode = CLASSIC;
    
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    sdkbox::PluginSdkboxPlay::setListener(this);
#endif

    Game::onEnter();
	return;
}

void Classic::onExit()
{
    showingAd = false;
    
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    sdkbox::PluginSdkboxPlay::removeListener();
#endif

    Game::onExit();
	return;
}

Classic::~Classic()
{
	timers.clear();
}

#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
void Classic::onConnectionStatusChanged(int status)
{
}

void Classic::onScoreSubmitted(const std::string & leaderboard_name, int score, bool maxScoreAllTime, bool maxScoreWeek, bool maxScoreToday)
{
}

void Classic::onIncrementalAchievementUnlocked(const std::string & achievement_name)
{
}

void Classic::onIncrementalAchievementStep(const std::string & achievement_name, int step)
{
}

void Classic::onAchievementUnlocked(const std::string & achievement_name, bool newlyUnlocked)
{
}
#endif