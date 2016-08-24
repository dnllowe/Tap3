#pragma once
#include "pch.h"
#include "nrgScene.h"

class MemoryMode : public Game
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

	void ShowMatchingTiles();

	//Restore tiles based on specific match number
	void SpecialRestore(int patternInput = -1);

	void UpdateTimer();

	void RestartTimers();

	//Calculate final score
	void CalculateScore();

	//Resets score variables
	void ResetScore();

	void UseBonusTimePlus();

	//Generates new, randomized card of tiles
	void UseNewCard();

	void UseBonusEliminate();

	void UseShowMatch();

	//Checks and sets new best score and best match
	void CheckForNewBests();

	//Displays match #, points gained, speed bonus, and time gained for each match
	void DisplayMatchSummary();

	//Check to see if any achievements were unlocked during the previous game
	bool CheckForAchievements();

	//Randomize tile shapes based on parameters
	void RandomizeTiles();

	void ReRandomizeTiles();

	void ReuseBonuses();

	//Resets randomization styles back to default states
	void Reset();

	//Set match criteria and parameters based on current match. Excluded Criteria == -1: randomize as usual. == -2: use same criteria as before. 0 or higher: excluded the passed criteria
	void SetMatchParameters(int excludedMatchCriteria = -1, bool blind = false);

	//Creates the "create" function
	CREATE_FUNC(MemoryMode);

	void onEnter();

	void onExit();

	~MemoryMode();

protected:
	
	MemoryMode();
	bool hideTiles = false; //Hides tile shapes
	bool specialRestore = false; //Whether we're using a special restore function
	bool specialRestoreStart = false;
	bool specialRestoreFinished = true;
	bool reuseBonuses = false;
	bool runGroup1a = false;
	bool runGroup1b = false;
	bool runGroup2a = false;
	bool runGroup2b = false;
	bool runGroup3a = false;
	bool runGroup3b = false;
	int readyOffset = 8;
	int offset = 35;
	bool exactShape = false;
	bool exactColor = false;
	std::vector<int> matchCriteriaOrder; //Use to set order of match criteria for order-based rounds where matching tile number may differ
	std::string nextHeaderString; //When header text is decided in SetMatchParameters or RandomizeTiles and must be dislpayed after tiles are hidden, set the string here.
	std::vector<std::string> nextHeaderStringVector;
	enum EXACT_SHAPE {};
	int losingSelection = 0; //The selection the user failed on

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

