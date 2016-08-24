#pragma once
#include "pch.h"
#include "nrgScene.h"

class MainMenu : public Game
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

	//Resets randomization styles back to default states
	void RunIntro();

	//Creates the "create" function
	CREATE_FUNC(MainMenu);

	void onEnter();

	void onExit();

	~MainMenu();

protected:
	
	//Batch and cache
	cocos2d::SpriteBatchNode* batch = NULL;
	cocos2d::SpriteFrameCache* cache = cocos2d::SpriteFrameCache::getInstance();
    
	bool gameModeSelected = false; //Whether play has chosen difficulty
	bool selectingGameMode = false;
	bool startMenuButtonOn = false;
	bool backMenuButtonOn = false;

	bool choseCountdown = false;
	bool choseClassic = false;
	bool chosePractice = false;
	bool choseMemory = false;

	int highestMatchCountdown = 0; //Highest match player has ever reached on Easy
	int highestScoreCountdown = 0; //Highest score player has ever reached Easy
	int highestMatchClassic = 0; //Highest match player has ever reached on Normal
	int highestScoreClassic = 0; //Highest score player has ever reached Normal
	int highestMatchPractice = 0; //Highest match player has ever reached on Hard
	int highestScorePractice = 0; //Highest score player has ever reached Hard
	int highestMatchMemory = 0; //Highest match player has ever reached on Hard
	int highestScoreMemory = 0; //Highest score player has ever reached Hard
	
	bool displayBestScore = false; //Only display best score if a score is made on either easy, normal, or hard
	bool displayingCountdown = false;
	bool displayingClassic = false;
	bool displayingPractice = false;
	bool displayingMemory = false;

	nrgText* title = NULL; //Title text for start screen
	nrgSprite* titleShadow = NULL;
	nrgText* eyebrow = NULL; //Eyebrow to title text for start screen
	nrgSprite* bestScoreTile = NULL;
	nrgSprite* bestScoreTileShadow = NULL;

	bool gameStarted = false; //Whether play has started game or not
	bool displayReviewRequest = false; //Whether to turn on review request forms for display
	
	nrgButton* countdownButton = NULL;
	nrgButton* classicButton = NULL;
	nrgButton* practiceButton = NULL;
	nrgButton* memoryButton = NULL;
	nrgText* countdownText = NULL;
	nrgText* classicText = NULL;
	nrgText* practiceText = NULL;
	nrgText* memoryText = NULL;
	nrgText* baseTileTitle = NULL;

	MainMenu();
	
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

