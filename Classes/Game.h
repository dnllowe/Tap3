#pragma once
#include "pch.h"

class Game : public nrgScene
{
public:

	//Creates scene and initializes necessary variables using Init()
	static cocos2d::Scene* CreateScene();

	//Function that runs game loop using CREATE_FUNC from Cocos2dx
	virtual bool init();

	virtual void InitGameAssets();

	virtual void DisplaySequence();
	
	virtual void CheckSequence();

	virtual void GenerateBonus();

	virtual void DisplayBonus();

	virtual void AcceptBonus();

	virtual void TradeBonus();

	virtual void ShowHighScores();

	//Checks earned achievements against GameCenter and GooglePlay if not already submitted
	virtual void SyncAchievements();

	//Submits latest high scores saved locally to GameCenter and GooglePlay
	virtual void SyncHighScores();

	//Reveals matching tiles (for testing purposes)
	virtual void ShowMatchingTiles();

	//Calculate final score
	virtual void CalculateScore();

	//Converts remaining earned bonus items quantity string into points
	virtual void ConvertRemainingBonusItemsToPoints();

	//Resets score variables
	virtual void ResetScore();

	//Adds increasing time remaining.
	virtual void UseBonusTimePlus();

	//Reduces remaining tiles by two non-matching tiles. Should only be used when selection == 1.
	virtual void UseBonusEliminate();

	//Reveals one of the matching tiles. Should only be used when selection == 1.
	virtual void UseShowMatch();

	//Generates new, randomized card of tiles
	virtual void UseNewCard();

	//Update text for score
	virtual void UpdateScore();

	//Checks and sets new best score and best match
	virtual void CheckForNewBests();

	//Update text for timer
	virtual void UpdateTimer();

	//Fades out header text sprites
	virtual void DismissHeaderText();

	//Update text for header tile based on matchCriteria
	virtual void UpdateHeaderText(int value);

	virtual void UpdateHeaderText(std::string input);

	//Displays match #, points gained, speed bonus, and time gained for each match
	virtual void DisplayMatchSummary();

	//Adds time plus bonus
	virtual void AddTimePlusBonus();

	//Adds eliminate bonus
	virtual void AddEliminateBonus();

	//Adds show match bonus
	virtual void AddShowMatchBonus();

	//Adds pass bonus
	virtual void AddNewCardBonus();

	//Lowers time plus bonus
	virtual void LowerTimePlusBonus();

	//Lowers eliminate bonus
	virtual void LowerEliminateBonus();

	//Lowers show match bonus
	virtual void LowerShowMatchBonus();

	//Lowers pass bonus
	virtual void LowerNewCardBonus();

	//Restores tiles in animated move/fade sequence
	virtual void RestoreTiles(int pattern = -1);

	//Check to see if any achievements were unlocked during the previous game. Returns true if all achievements earned
	virtual bool CheckForAchievements();

	//Display achievements unlocked at game over screen
	virtual void DisplayAchievements();

	//Randomize tile shapes based on parameters
	virtual void RandomizeTiles();

	//Remembers how many matched and nonmatched tiles were pressed and randomizes tiles again in similar state
	virtual void ReRandomizeTiles();

	//Whether to rerandomize tiles on app resume
	virtual bool AllowRerandomize();

	//Converts int into Cocos2d 3B Color pair
	virtual std::pair<cocos2d::Color3B, cocos2d::Color3B> GetColorFromKey(std::pair<int, int> color);

	//Encodes Cocos2d 3B Color pair into int pair
	virtual std::pair<int, int> DecodeColor(std::pair<cocos2d::Color3B, cocos2d::Color3B>);

	//Resets randomization styles back to default states
	virtual void Reset();

	//Set match criteria and parameters based on current match. Excluded Criteria == -1: randomize as usual. == -2: use same criteria as before. 0 or higher: excluded the passed criteria
	virtual void SetMatchParameters(int excludedMatchCriteria = -1, bool blind = false);

	//Creates the "create" function
	CREATE_FUNC(Game);

	//Returns value for match criteria
	virtual int GetMatchCriteria();

	//Check to see if game is in gameover start
	virtual bool IsGameOver();

	//Check to see if game has started
	virtual bool IsGameStarted();

	//Get game mode
	static int GetMode();

	static void SetForcedGameOver(bool isGameOver = true);

	//Set whether to request review
	static void RequestReview();

	//Returns instance of running game loop
	static Game* getInstance();

	//Use to restart game timers (use getInstance if called from AppDelegate)
	virtual void RestartTimers();

	//Check play frequency over past 3 days
	static void CheckPlayFrequency();

	//Sets last day game was played. 0 = January 1st. 364/365 = December 31st
	static void SetLastDayPlayed(int value);

	//Returns last day game was played. 0 = January 1st. 364/365 = December 31st
	static int GetLastDayPlayed();

	//Sets how many times app was played today
	static void SetTimesPlayedToday(int value);

	//Returns how many times app was played today
	static int GetTimesPlayedToday();

	//Sets how many times app was played today
	static void SetTimesOpenedToday(int value);

	//Returns how many times app was played today
	static int GetTimesOpenedToday();

	//Sets how many times app was opened yesterday
	static void SetTimesOpenedYesterday(int value);

	//Returns how many times app was opened yesterday
	static int GetTimesOpenedYesterday();

	//Sets how many times app was opened two days ago
	static void SetTimesOpenedTwoDaysAgo(int value);

	//Returns how many times app was opened two days ago
	static int GetTimesOpenedTwoDaysAgo();

	//Sets last year game was opened
	static void SetLastYearOpened(int value);

	//Returns last year game was played
	static int GetLastYearOpened();

    void CheckForReview();
    
	virtual void onEnter();

	virtual void onExit();

	enum GAME_MODE { COUNTDOWN, CLASSIC, RUSH_RELAX, MEMORY };

	~Game();

protected:
	
	//Batch and cache
	cocos2d::SpriteBatchNode* batch = NULL;
	cocos2d::SpriteFrameCache* cache = cocos2d::SpriteFrameCache::getInstance();
	static Game* instance;
	
	int matches = 0; //How many matches the player has made
	int startingMatches = 0; //For testing purposes. Sets starting matches in "Reset" function
	int startingMs = 0; //How much ms to put on timer at beginning of game
	int matchMilestone = 5; //Which match to increase time by an extra 5000 ms and add tier one bonus items
	int nextMilestone = 10; //How much to add to previous milestone for next milestone
	int timelimit = 15000; //Time limit for memorization in Memory Mode

	bool gameStarted = false;
	bool gameWon = false; //Whether play reached match 100
	static bool forcedGameOver;
	bool gameOver; //Whether player made incorrect match or time ran out
	bool matchMade = false; //Whether player made match on most recent turn
	static bool requestReview;
	static int mode;
	bool allowRerandomize = true;

	//TESTING VARIABLES
	bool testingReview = false;
    bool testingAchievements = false;
	bool testingBonuses = false;
	int testingBonus = -1;
	enum BONUS { TIME, ELIMINATE, SHOW, NEW };

	bool showingAd = false;
	int highestMatch = 0; //Highest match player has ever reached on Easy
	int highestScore = 0; //Highest score player has ever reached Easy

	int baseTileTextSize; //Varies based on platform
	bool displayReviewRequest = false; //Whether to turn on review request forms for display
	int earnedTimePlusBonuses = 0; //How many time plus bonus items were earned during the match
	int earnedEliminateBonuses = 0; //How many eliminate bonus items were earned during the match
	int earnedShowMatchBonuses = 0; //How many show match bonus items were earned during the match
	int earnedNewCardBonuses = 0; //How many new card bonus items were earned during the match
	int purchasedTimePlusBonuses = 0; //How many time plus bonus items were purchased before the game
	int purchasedEliminateBonuses = 0; //How many eliminate bonus items were purchased before the game
	int purchasedShowMatchBonuses = 0; //How many show match bonus items were purchased before the game
	int purchasedNewCardBonuses = 0; //How many new card bonus items were purchased before the game
	int earnedBonusesMultiplier = 50; //How many points received for remaining earned bonuses at end of game
	int matchMultiplier = 100; //How many points received for each match made by end of game
	int speedBonus = 0; //Points received depending on how quickly player solved match
	int previousSpeedBonus = 0; //Speed bonus from most recent match only
	int msAdded = 0; //How many ms were added to clock after match
	int finalScore = 0; //Calculated final score based on matches, time remaining, and earned bonsues not used
	int displayedScore = 0; //Score to show on game over screen as it counts up to final score
	int pattern = 0; //Which intro pattern tiles will restore with

	//Game objects / elements
	BaseTile* baseTile = NULL; //Base tile for game
	nrgButton* backButton = NULL; //Button to go back to previous mode/screen
	nrgButton* optionsButton = NULL; //Button to go to options menu
	nrgButton* highScoresButton = NULL; //Button to display high scores, achievements, and leaderboards
	nrgButton* buyBonusItemsButton = NULL; //Button to enter in-app store
	nrgSprite* baseTileShadow = NULL; //Shadow for base tile
	nrgSprite* headerTile = NULL; //Tile for header
	nrgSprite* scoreTile = NULL; //Tile for score
	nrgSprite* eliminateTile = NULL; //Tile for eliminate bonus
	nrgSprite* timePlusTile = NULL; //Tile for time plus bonus
	nrgSprite* showMatchTile = NULL; //Tile for show match bonus
	nrgSprite* newCardTile = NULL; //Tile for pass bonus
	nrgText* optionsText = NULL; //Image for "Options" text. Anchor point is (0, 1)
	nrgText* highScoresText = NULL; //Image for "High scores" text. Anchor point is (0, 1)
	nrgText* buyBonusItemsText = NULL; //Image for "Buy bonus items" text. Anchor point is (0, 1)
	nrgText* baseTileText = NULL; //Text for request for review
	nrgButton* menuLeft = NULL;  //Background for menu button left
	nrgButton* menuRight = NULL; //Background for menu button right
	nrgText* tradeText = NULL; //Text for "Trade (100 pts)"
	bool yesMenuButtonOn = false;
	bool notReallyMenuButtonOn = false;
	bool sureMenuButtonOn = false;
	bool okMenuButtonOn = false;
	bool noThanksMenuButtonOn = false;

	bool generateBonus = false; //True while bonus items is randomizing
	bool confirmingBonus = false; //Play must confirm getting bonus or exchanging for points before continuing game
	nrgText* generateQuestionMark = NULL;
	nrgText* generateText = NULL;
	
	bool displaySequence = true; //Whehter to display which sequence of matches the player is on
	int sequence = 1;
	nrgText* sequenceText = NULL;
	int sequenceTextSize;

	nrgTimer bonusTimer; //Controls timing for generating bonus item
	nrgTimer sequenceTimer; //Counts how long to display current sequence text
	int randomizedBonus;
	int extraPoints = 0;

	nrgTimer countdownTimer; //Keeps track of time left
	nrgTimer matchTimer; //Keeps track of how long player took to make match
	nrgTimer speedTimer_5_matches;
	nrgTimer speedTimer_10_matches;
	nrgTimer speedTimer_20_matches;

	nrgSprite* generateTimePlusThumbnail = NULL;
	nrgSprite* generateEliminateThumbnail = NULL;
	nrgSprite* generateShowMatchThumbnail = NULL;
	nrgSprite* generateNewCardThumbnail = NULL;

	nrgSprite* bestMatchIcon = NULL; //Image for "New Best" tile (matches)
	nrgSprite* bestScoreIcon = NULL; //Image for "New Best" tile (score)

	nrgMenu* retry = NULL; //Menu for retrying
	
	int generateQuestionMarkTextSize;
	int generateTextSize;
	int bonusTextSize; //Font size for bonus item quantity
	int retryTextSize; //Font size for "Try Again" "Tap Here To Start"
	int matchMadeStringsSize; //Fonts size for points displayed on matching tile
	int pointsTextSize; //Font size for points
	int displayTextSize; //Font size for score and match criteria
	int scoreTextSize;
	int titleTextSize; //Varies based on platform
	int eyebrowTextSize; //Varies based on platform
	int gameModeTextSize;
	int topMenuButtonTextSize;
	int baseTileMenuButtonTextSize;
	int timeRemainingTextSize; //Will vary depending on platform
	int achievementHeaderSize = 45;
	int achievementTextSize = 35;
	
	int pointsTextBottomPosition = 55;
	int retryBottomPosition = 20;
	int timeRemainingBottomPosition;
	float bonusTextOffsetX;
	float bonusTextOffsetY;
	float timeRemainingStartPositionY; //Bottom position for timerText. Changes based on platform
	float titleStartPositionY; //Bottom position for title text. Changes based on platform
	float eliminateStartingX;
	float eliminateStartingY;
	float timePlusStartingX;
	float timePlusStartingY;
	float showMatchStartingX;
	float showMatchStartingY;
	float newCardStartingX;
	float newCardStartingY;
	float titleBottomPosition;
	int achievementHeaderOffset;
	int headerTileOffset = 0; //How far above baseTile to place headerTile
	int bestMatchIconOffset = 0; //How far below match text bestMatchIcon appears

	int msLeft = 0; //How many ms are left on timer
	int secLeft = 0; //How many seconds are left on timer
	int msPassed = 0; //How many ms have passed since timer reset
	int secPassed = 0; //How many seconds have passed since timer reset
	int msWarning = 3000; //What time remaining amount to set text to red
	int warningTime; //Which ms to play warning. Reduce by 1000 after each tick. Raise to 3000 if msLeft are >= 3000
	int bonusEliminateLeft = 0; //How many eliminate bonus items are left for use
	int bonusTimePlusLeft = 0; //How many time plus bonus items are left for use
	int bonusShowMatchLeft = 0; //How many show match bonus items are left for use
	int bonusNewCardLeft = 0; //How many bonus pass items are left for use
	int matchedTilesShowing = 0; //How many times player has used show match tile on current card
	int timePlusUsedOnCurrentCard = 0; //Keeps track of how many time plus have been used on current set of tiles
	int eliminateUsedOnCurrentCard = 0; //Keeps track of how many eliminate bonuses have been used on current set of tiles
	int timePlusUseLimit = 2; //Limits how many time plus bonuses can be used on current set of tiles

	//Text objects
	nrgText* matchCriteriaText = NULL; //Displays match criteria
	nrgText* scoreText = NULL; //Displays "Score"
	nrgText* leftMenuText = NULL; //Text for left menu button
	nrgText* rightMenuText = NULL; //Text for right menu button
	nrgText* timerText = NULL; //Displays time remaining
	nrgText* pointsText = NULL; //Displays score
	nrgText* timePlusText = NULL; //Displays quantity of time plus bonuses left
	nrgText* eliminateText = NULL; //Displays quantity of elimnate bonuses left
	nrgText* showMatchText = NULL; //Displays quantity of show match bonuses left
	nrgText* newCardText = NULL; //Displays quantity of pass bonuses left
	nrgList* matchMadeList = NULL; //Display summary of points after each match or full summary after game completion

	//Menu buttons
	nrgButton* stats = NULL;
	nrgButton* options = NULL;

	Tile* topLeftTile = NULL; //Top left tile
	Tile* topCenterTile = NULL; //Top center tile
	Tile* topRightTile = NULL; //Top right tile
	Tile* middleLeftTile = NULL; //Middle Left tile
	Tile* middleCenterTile = NULL; //Middle center tile
	Tile* middleRightTile = NULL; //Middle right tile
	Tile* bottomLeftTile = NULL; //Bottom left tile
	Tile* bottomCenterTile = NULL; //Bottom center tile
	Tile* bottomRightTile = NULL; //Bottom right tile

	std::vector<Tile*> tiles; //Vector for tiles. (0 = TOP LEFT, 1 = TOP CENTER, 2 = TOP RIGHT, 3 = MIDDLE LEFT, 4 = MIDDLE CENTER, 5 = MIDDLE RIGHT, 6 = BOTTOM LEFT, 7 = BOTTOM CENTER, 8 = BOTTOM RIGHT)
	std::pair<float, float> topLeft; //Coordinate for top left tile
	std::pair<float, float> topCenter; //Coordinate for top center tile
	std::pair<float, float> topRight; //Coordinate for top right tile
	std::pair<float, float> middleLeft; //Coordinate for middle left tile
	std::pair<float, float> middleCenter; //Coordinate for middle center tile
	std::pair<float, float> middleRight; //Cordinate for middle right tile
	std::pair<float, float> bottomLeft; //Coordinate for bottom left tile
	std::pair<float, float> bottomCenter; //Coordinate for bottom center tile
	std::pair<float, float> bottomRight; //Coordinate for bottom right tile

	int difficulty = 0; //Determines parameters of generated tiles
	int selection1 = -1; //First selection player made (0 = TOP LEFT, 1 = TOP CENTER, 2 = TOP RIGHT, 3 = MIDDLE LEFT, 4 = MIDDLE CENTER, 5 = MIDDLE RIGHT, 6 = BOTTOM LEFT, 7 = BOTTOM CENTER, 8 = BOTTOM RIGHT)
	int selection2 = -1; //Second selection player made (0 = TOP LEFT, 1 = TOP CENTER, 2 = TOP RIGHT, 3 = MIDDLE LEFT, 4 = MIDDLE CENTER, 5 = MIDDLE RIGHT, 6 = BOTTOM LEFT, 7 = BOTTOM CENTER, 8 = BOTTOM RIGHT)
	int selection3 = -1; //Third selection player made (0 = TOP LEFT, 1 = TOP CENTER, 2 = TOP RIGHT, 3 = MIDDLE LEFT, 4 = MIDDLE CENTER, 5 = MIDDLE RIGHT, 6 = BOTTOM LEFT, 7 = BOTTOM CENTER, 8 = BOTTOM RIGHT)

	enum TILE_POSITION { TOP_LEFT, TOP_CENTER, TOP_RIGHT, MIDDLE_LEFT, MIDDLE_CENTER, MIDDLE_RIGHT, BOTTOM_LEFT, BOTTOM_CENTER, BOTTOM_RIGHT };
	enum MATCH_CRITERIA { COLORS, SHAPES, STYLES, COLORS_SHAPES, COLORS_STYLES, SHAPES_STYLES, COLORS_SHAPES_STYLES }; // 0 = COLORS, 1 = SHAPES, 2 = STYLES, 3 = COLORS_SHAPES
	enum SHAPE { CIRCLE, SQUARE, DIAMOND, TRIANGLE, HEXAGON, OCTAGON }; //0 = CIRCLE, 1 = SQUARE, 2 = DIAMOND, 3 = TRIANGLE, 4 = HEXAGON, 5 = OCTAGON
	enum OUTLINE { SOLID, OUTLINE, BLINDS, CONCENTRIC, QUAD, OUTLINE_BLINDS, OUTLINE_QUAD, CONCENTRIC_BLINDS, CONCENTRIC_QUAD, BLINDS_QUAD, OUTLINE_BLINDS_QUAD, CONCENTRIC_BLINDS_QUAD }; //(0 = SOLID, 1 = OUTLINE, 2 = BLINDS, 3 = CONCENTRIC, 4 = QUAD, 5 = OUTLINE_BLINDS, 6 = OUTLINE_QUAD, 7 = CONCENTRIC_BLINDS, 8 = CONCENTRIC_QUAD, 9 = BLINDS_QUAD, 10 = OUTLINE_BLINDS_QUAD, 11 = CONCENTRIC_BLINDS_QUAD)
	enum COLORS { RED, GREEN, BLUE, YELLOW, PURPLE, CYAN, ORANGE };
	int matchCriteria = SHAPES; //Type of match player needs to make
	bool matchCriteriaSet = false; //Whether new match type needs to be generated
	int currentTile = 0; //Which tile loop is running logic on
	int currentSelection = 1; //Which selection player is on
	Tile* tile = NULL; //Quick pointer to current tile

	std::unordered_set<int> matchingTiles; //Which tiles are randomly selected to match
	std::unordered_set<int> excludedEliminateTiles; //Which tiles to exclude from eliminate tile bonus (matching tiles and already eliminated tiles)
	std::vector<int> matchingTilesVector; //Vector containing matching tile numbers
	int numberOfMatchingTiles = 3; //How many tiles will match
	int shape = 0; //Shape to set for tile
	int color = 0; //Color to set for solid-color shapes
	std::pair<int, int> colorKey; //Use to translate int pair to and from Cocos2d Color 3B pairs
	std::pair<cocos2d::Color3B, cocos2d::Color3B> colorPair; //Color pair to set for tile shape
	int rotation = 0; //The rotation of the tile.
	int outline = 0; //Outline style to set for tile
	std::pair<int, int> outlinePair; //The outline style pair to set for tile

	int nonmatchCircleFrequency = 0; //Counts how many times a nonmatched circle has been generated. Add to exclusion list after two to prevent a 2nd possible match candidate.
	int nonmatchSquareFrequency = 0; //Counts how many times a nonmatched square has been generated. Add to exclusion list after two to prevent a 2nd possible match candidate.
	int nonmatchDiamondFrequency = 0; //Counts how many times a nonmatched diamond has been generated. Add to exclusion list after two to prevent a 2nd possible match candidate.
	int nonmatchTriangleFrequency = 0; //Counts how many times a nonmatched triangle has been generated. Add to exclusion list after two to prevent a 2nd possible match candidate.
	int nonmatchHexagonFrequency = 0; //Counts how many times a nonmatched hexagon has been generated. Add to exclusion list after two to prevent a 2nd possible match candidate.
	int nonmatchOctagonFrequency = 0; //Counts how many times a nonmatched octagon has been generated. Add to exclusion list after two to prevent a 2nd possible match candidate.
	
	std::vector<int> nonmatchedSolidColors;
	int nonmatchSolidColorsFrequency;
	std::vector<int> nonmatchedStyles;
	int nonmatchStyleFrequency;

	std::vector<std::pair<int, int>> nonmatchedColorPairs; //Keeps track of which color pairs have been generated
	int nonmatchColorPairsFrequency = 0; //Counts how many times a nonmatched color pair has been generated. Add to exclusion list after two to prevent a 2nd possible match candidate.
	std::vector<std::pair<int, int>> nonmatchedStylePairs; //Keeps track of which style pairs have been generated
	int nonmatchStylePairsFrequency = 0; //Counts how many times a nonmatched outline style has been generated. Add to exclusion list after two to prevent a 2nd possible match candidate.

	std::unordered_set<int> excludedTiles; //Which tiles to exclude when deciding matching tiles
	std::unordered_set<int> excludedShapes; //Which shapes to exclude from random generation
	std::unordered_set<std::pair<int, int>, pair_hash> excludedColorPairs; //Which color pairs to exclude from random generation
	std::unordered_set<std::pair<int, int>, pair_hash> excludedStylePairs; //Which outline styles to exclude from random generation
	std::unordered_set<int> excludedColors;
	std::unordered_set<int> excludedStyles;

	bool dismissTiles = false; //Whether to dismiss remaining tiles
	bool randomizeTiles = false; //Whether to randomize tile shapes and patterns
	bool restoreTiles = false; //Whether to restore all tiles
	bool enablePowerups = false; //Whether to re-enable power ups (only once all tiles have loaded)
	bool disablePowerups = false; //Whether to disable powerups (use while tiles are loading)
	bool powerupsAreOn = true; //Whether powerups are enabled
	bool splitColor = false; //Whether top and bottom half of shape can have different colors
	bool splitStyle = false; //Whether top and bottom of shape can have different outline styles

	bool matchingShapes = true; //Whether shapes are set as match criteria
	bool matchingColors = false; //Whether colors are set as match criteria
	bool matchingStyles = false; //Whether styles are set as match criteria
	bool blindMatch = false; //Whether match criteria displays "???" or not. Only use when nonmatching criteria does not exceed two
	bool nonmatchedShapesCanExceedTwo = true; //Whether tiles can have 3 or more same shapes generated
	bool nonmatchedColorsCanExceedTwo = true; //Whether tiles can have 3 or more same colors generated
	bool nonmatchedStylesCanExceedTwo = true;  //Whether tiles can have 3 or more same styles generated

	bool useDesignatedMatchColor = false; //Whether to use a predetermined color for matching tiles
	bool useDesignatedNonmatchColor = false; //Whether to use a predetermined color for nonmatching tiles
	bool useDesignatedMatchShape = false; //Whether to use a predetermined shape for matching tiles
	bool useDesignatedNonmatchShape = false; //Whether to use a predetermined shape for nonmatching tiles
	bool useDesignatedMatchStyle = false; //Whether to use a predetermined style for matching tiles
	bool useDesignatedNonmatchStyle = false; //Whether to use a predetermined style for nonmatching tiles

	std::pair<int, int> designatedMatchColor; //Predetermined color for matching tiles
	std::pair<int, int> designatedNonmatchColor; //Predetermined color for nonmatching tiles
	std::pair<int, int> designatedMatchStyle; //Predetermined style for matching tiles
	std::pair<int, int> designatedNonmatchStyle; //Predetermined style for nonmatching tiles
	int designatedMatchShape; //Predetermined shape for matching tiles
	int designatedNonmatchShape; //Predetermined shape for nonmatching tiles

	int numberOfSameNonmatchingColors = 0; //How many nonmatching tiles share the same color (overruled by designated color)
	int numberOfSameNonmatchingShapes = 0; //How many nonmatching tiles share the same shape (overruled by designated shape)
	int numberOfSameNonmatchingStyles = 0; //How many nonmatching tiles share the same style (overruled by designated style)

	int designatedNonmatchColorLimit = 0; //Limits how many nonmatching tiles share designated color
	int designatedNonmatchStyleLimit = 0; //Limits how many nonmatching tiles share designated style
	int designatedNonmatchShapeLimit = 0; //Limits how many nonmatching tiles share designated shape

	//Achievement variables

	nrgText* achievementUnlockedHeader = NULL; //Text for "Achievement Unlocked!" or "Achievements Unlocked!"
	std::vector<nrgText*> achievementsText; //Vector for all achievements currently unlocked
	
	int achievementsUnlocked = 0; //How many achievements unlocked after last game

	int bonusesUsed = 0;
	bool noBonusesUsedBefore50Matches = false;
	bool lastMatchWasNearMiss = false;
	bool nearMiss = false;

	int minStyle = 0;
	int maxStyle = 11;
	int minColor = 0;
	int maxColor = 6;
	int minShape = 0;
	int maxShape = 5;

	Game();
	
};

