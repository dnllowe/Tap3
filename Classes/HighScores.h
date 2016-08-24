#pragma once
#include "pch.h"

class HighScores : public nrgScene
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
, public sdkbox::SdkboxPlayListener
#endif

{
public:
	HighScores();
	~HighScores();

	static cocos2d::Scene* CreateScene();

	bool init();

	void update(float dt);

	void onEnter();

	void onExit();

	CREATE_FUNC(HighScores);

	

protected:

	cocos2d::SpriteBatchNode* batch = NULL;
	cocos2d::SpriteFrameCache* cache = cocos2d::SpriteFrameCache::getInstance();

	nrgButton* backButton = NULL; //Back button

	nrgSprite* fillScreen = NULL;
	nrgButton* classicScoreTile = NULL;
	nrgButton* classicMatchTile = NULL;
	nrgButton* countdownScoreTile = NULL;
	nrgButton* countdownMatchTile = NULL;
	nrgButton* memoryScoreTile = NULL;
	nrgButton* memoryMatchTile = NULL;
	nrgButton* practiceScoreTile = NULL;
	nrgButton* scoreTile = NULL;
	nrgButton* matchTile = NULL;
	nrgButton* achievementsTile = NULL;

	nrgText* classicScoreText = NULL;
	nrgText* classicMatchText = NULL;
	nrgText* countdownScoreText = NULL;
	nrgText* countdownMatchText = NULL;
	nrgText* memoryScoreText = NULL;
	nrgText* memoryMatchText = NULL;
	nrgText* practiceScoreText = NULL;
	nrgText* scoreText = NULL;
	nrgText* matchText = NULL;
	nrgText* achievementsText = NULL;

	int tileTextSize;
    
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

