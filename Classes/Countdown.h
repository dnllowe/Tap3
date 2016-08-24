#pragma once
#include "pch.h"

#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include <jni.h> //For Java code interface
#include "platform/android/jni/JniHelper.h"
#endif

class Countdown : public Game
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	, public sdkbox::SdkboxPlayListener
#endif
{
public:

	//Creates scene and initializes necessary variables using Init()
	static cocos2d::Scene* CreateScene();

	//Function that runs game loop using CREATE_FUNC from Cocos2dx
    bool init();

	//Game loop entry point
	void update(float dt);

	//Calculate final score
	void CalculateScore();

	//Resets score variables
	void ResetScore();

	//Adds increasing time remaining.
	void UseBonusTimePlus();

	//Generates new, randomized card of tiles
	void UseNewCard();

	//Checks and sets new best score and best match
	void CheckForNewBests();

	//Update text for timer
	void UpdateTimer();

	//Randomly generates a bonus item
	void RandomizeBonus();

	//Displays match #, points gained, speed bonus, and time gained for each match
	void DisplayMatchSummary();

	//Check to see if any achievements were unlocked during the previous game. Returns true if all achievements earned
	bool CheckForAchievements();

	//Randomize tile shapes based on parameters
	void RandomizeTiles();

	//Resets randomization styles back to default states
	void Reset();

	//Set match criteria and parameters based on current match. Excluded Criteria == -1: randomize as usual. == -2: use same criteria as before. 0 or higher: excluded the passed criteria
	void SetMatchParameters(int excludedMatchCriteria = -1, bool blind = false);

	//Creates the "create" function
	CREATE_FUNC(Countdown);

	void onEnter();

	void onExit();

	~Countdown();

protected:
	
	//Batch and cache
//	cocos2d::SpriteBatchNode* batch = NULL;
//	cocos2d::SpriteFrameCache* cache = cocos2d::SpriteFrameCache::getInstance();

	bool timerReached90 = false;
	bool remainingTimeAbove20 = false;
    bool msWentBelow20 = false;
	int matchTimeIncrease = 4000; //Amount to increase time for getting match
	int milestoneTimeIncrease = 0; //Amount to increase time for reaching milestone

	bool earnedSpeedDemon1 = false;
	bool earnedSpeedDemon2 = false;
	bool earnedSpeedDemon3 = false;

	Countdown();
	
private:
	
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	/**
	* Call method invoked when the Plugin connection changes its status.
	* Values are as follows:
	*   + GPS_CONNECTED:       successfully connected.
	*   + GPS_DISCONNECTED:    successfully disconnected.
	*   + GPS_CONNECTION_ERROR:error with google play services connection.
	*/
	void onConnectionStatusChanged(int status) override;

	/**
	* Callback method invoked when an score has been successfully submitted to a leaderboard.
	* It notifies back with the leaderboard_name (not id, see the sdkbox_config.json file) and the
	* subbmited score, as well as whether the score is the daily, weekly, or all time best score.
	* Since Game center can't determine if submitted score is maximum, it will send the max score flags as false.
	*/
	void onScoreSubmitted(const std::string& leaderboard_name, int score, bool maxScoreAllTime, bool maxScoreWeek, bool maxScoreToday) override;

	/**
	* Callback method invoked when the request call to increment an achievement is succeessful and
	* that achievement gets unlocked. This happens when the incremental step count reaches its maximum value.
	* Maximum step count for an incremental achievement is defined in the google play developer console.
	*/
	void onIncrementalAchievementUnlocked(const std::string& achievement_name) override;

	/**
	* Callback method invoked when the request call to increment an achievement is successful.
	* If possible (Google play only) it notifies back with the current achievement step count.
	*/
	void onIncrementalAchievementStep(const std::string& achievement_name, int step) override;

	/**
	* Call method invoked when the request call to unlock a non-incremental achievement is successful.
	* If this is the first time the achievement is unlocked, newUnlocked will be true.
	*/
	void onAchievementUnlocked(const std::string& achievement_name, bool newlyUnlocked) override;
#endif
};

