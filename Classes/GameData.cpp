#include "pch.h"
#include "GameData.h"

GameData::GameData()
{
}

void GameData::ResetStats()
{
	//Get instance of user default singleton
	cocos2d::UserDefault* gameData = cocos2d::UserDefault::getInstance();

	std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
	time_t timeObject = std::chrono::system_clock::to_time_t(now);
	tm* currentTime = localtime(&timeObject);

	int today = currentTime->tm_yday;
	int year = currentTime->tm_year;

	//Overall stats
	gameData->setIntegerForKey(times_opened_today, 0);
	gameData->setIntegerForKey(times_opened_two_days_ago, 0);
	gameData->setIntegerForKey(last_day_opened, today);
	gameData->setIntegerForKey(last_year_opened, year);
	
	gameData->setBoolForKey(request_review, false);
	gameData->setBoolForKey(app_rated, false);
	gameData->setBoolForKey(music_on, true);
	gameData->setBoolForKey(sound_on, true);

	gameData->setIntegerForKey(matches_made_today, 0);
	gameData->setIntegerForKey(matches_made_today_for_review, 0);
	gameData->setIntegerForKey(best_score_countdown, 0);
	gameData->setIntegerForKey(best_match_countdown, 0);
	gameData->setIntegerForKey(best_score_classic, 0);
	gameData->setIntegerForKey(best_match_classic, 0);
	gameData->setIntegerForKey(best_score_rushrelax, 0);
	gameData->setIntegerForKey(best_match_rushrelax, 0);
	gameData->setIntegerForKey(best_score_memory, 0);
	gameData->setIntegerForKey(best_match_memory, 0);

	gameData->setIntegerForKey(consecutive_near_misses, 0);
	gameData->setIntegerForKey(total_color_matches, 0);
	gameData->setIntegerForKey(total_shape_matches, 0);
	gameData->setIntegerForKey(total_style_matches, 0);

	//Progress
	gameData->setIntegerForKey(total_matches, 0);
	gameData->setIntegerForKey(total_points, 0);

	//Purchases
	gameData->setBoolForKey(ads_removed, false);
	gameData->setIntegerForKey(purchased_eliminate_left, 0);
	gameData->setIntegerForKey(purchased_time_plus_left, 0);
	gameData->setIntegerForKey(purchased_show_match_left, 0);
	gameData->setIntegerForKey(purchased_new_card_left, 0);

	//Achievements
	gameData->setBoolForKey(first_match, false);
	gameData->setBoolForKey(earned_10000_total_matches, false);
	gameData->setBoolForKey(earned_1000000_total_points, false);
	gameData->setBoolForKey(first_5_matches_in_12_seconds, false);
	gameData->setBoolForKey(first_10_matches_in_24_seconds, false);
	gameData->setBoolForKey(first_20_matches_in_50_seconds, false);
	gameData->setBoolForKey(two_hundred_matches_in_one_day, false);
	gameData->setBoolForKey(five_hundred_color_matches, false);
	gameData->setBoolForKey(five_hundred_shape_matches, false);
	gameData->setBoolForKey(five_hundred_style_matches, false);
	gameData->setBoolForKey(near_miss, false);
	gameData->setBoolForKey(ten_consecutive_near_misses, false);
	gameData->setBoolForKey(earned_all_achievements, false);

	//countdown
	gameData->setBoolForKey(reach_match_10_countdown, false);
	gameData->setBoolForKey(reach_match_20_countdown, false);
	gameData->setBoolForKey(reach_match_30_countdown, false);
	gameData->setBoolForKey(reach_match_50_countdown, false);
	gameData->setBoolForKey(reach_match_60_countdown, false);
	gameData->setBoolForKey(reach_match_70_countdown, false);
	gameData->setBoolForKey(reach_match_80_countdown, false);
	gameData->setBoolForKey(reach_match_90_countdown, false);
	gameData->setBoolForKey(reach_match_100_countdown, false);
	gameData->setBoolForKey(no_bonus_items_used_first_50_matches_countdown, false);
	gameData->setBoolForKey(remaining_time_90_seconds_countdown, false);
	gameData->setBoolForKey(maintain_time_above_20, false);

	//classic
	gameData->setBoolForKey(reach_match_1_classic, false);
	gameData->setBoolForKey(reach_match_10_classic, false);
	gameData->setBoolForKey(reach_match_20_classic, false);
	gameData->setBoolForKey(reach_match_30_classic, false);
	gameData->setBoolForKey(reach_match_50_classic, false);
	gameData->setBoolForKey(reach_match_60_classic, false);
	gameData->setBoolForKey(reach_match_70_classic, false);
	gameData->setBoolForKey(reach_match_80_classic, false);
	gameData->setBoolForKey(reach_match_90_classic, false);
	gameData->setBoolForKey(reach_match_100_classic, false);
	gameData->setBoolForKey(no_bonus_items_used_first_50_matches_classic, false);

	//memory
	gameData->setBoolForKey(reach_match_10_memory, false);
	gameData->setBoolForKey(reach_match_20_memory, false);
	gameData->setBoolForKey(reach_match_30_memory, false);
	gameData->setBoolForKey(reach_match_50_memory, false);
	gameData->setBoolForKey(reach_match_60_memory, false);
	gameData->setBoolForKey(reach_match_70_memory, false);
	gameData->setBoolForKey(reach_match_80_memory, false);
	gameData->setBoolForKey(reach_match_90_memory, false);
	gameData->setBoolForKey(reach_match_100_memory, false);
	gameData->setBoolForKey(no_bonus_items_used_first_50_matches_memory, false);

	//Practice
	gameData->setBoolForKey(slow_poke, false);
	gameData->setBoolForKey(golf_rules, false);

	//New
	gameData->setBoolForKey(all_the_time_in_the_world, false);
	gameData->setIntegerForKey(time_plus_used, 0);
	gameData->setBoolForKey(process_of_elimination, false);
	gameData->setIntegerForKey(eliminate_used, 0);
	gameData->setBoolForKey(seeing_is_believing, false);
	gameData->setIntegerForKey(show_match_used, 0);
	gameData->setBoolForKey(clean_slate, false);
	gameData->setIntegerForKey(new_card_used, 0);

	gameData->setBoolForKey(take_the_points, false);
	gameData->setIntegerForKey(trades_accepted, 0);
	gameData->setBoolForKey(take_the_bonus, false);
	gameData->setIntegerForKey(bonuses_accepted, 0);

	gameData->setBoolForKey(the_10000_club, false);
	gameData->setBoolForKey(aspiring_star, false);
	gameData->setBoolForKey(six_figures, false);
	gameData->setBoolForKey(the_500000_club, false);

	gameData->setBoolForKey(matchmaker_training, false);
	gameData->setBoolForKey(matchmaker_junior, false);
	gameData->setBoolForKey(matchmaker_seasoned, false);

	gameData->setBoolForKey(classically_trained, false);
	gameData->setIntegerForKey(classic_matches, 0);
	gameData->setBoolForKey(time_management, false);
	gameData->setIntegerForKey(countdown_matches, 0);
	gameData->setBoolForKey(practice_makes_perfect, false);
	gameData->setIntegerForKey(practice_matches, 0);
	gameData->setBoolForKey(photographic_memory, false);
	gameData->setIntegerForKey(memory_matches, 0);


	//ACHIEVEMENTS SUBMITTED VARIABLES
	gameData->setBoolForKey(first_match_submitted, false);
	gameData->setBoolForKey(earned_10000_total_matches_submitted, false);
	gameData->setBoolForKey(earned_1000000_total_points_submitted, false);
	gameData->setBoolForKey(first_5_matches_in_12_seconds_submitted, false);
	gameData->setBoolForKey(first_10_matches_in_24_seconds_submitted, false);
	gameData->setBoolForKey(first_20_matches_in_50_seconds_submitted, false);
	gameData->setBoolForKey(two_hundred_matches_in_one_day_submitted, false);
	gameData->setBoolForKey(five_hundred_color_matches_submitted, false);
	gameData->setBoolForKey(five_hundred_shape_matches_submitted, false);
	gameData->setBoolForKey(five_hundred_style_matches_submitted, false);
	gameData->setBoolForKey(near_miss_submitted, false);
	gameData->setBoolForKey(ten_consecutive_near_misses_submitted, false);
	gameData->setBoolForKey(earned_all_achievements_submitted, false);

	//countdown
	gameData->setBoolForKey(reach_match_10_countdown_submitted, false);
	gameData->setBoolForKey(reach_match_20_countdown_submitted, false);
	gameData->setBoolForKey(reach_match_30_countdown_submitted, false);
	gameData->setBoolForKey(reach_match_50_countdown_submitted, false);
	gameData->setBoolForKey(reach_match_60_countdown_submitted, false);
	gameData->setBoolForKey(reach_match_70_countdown_submitted, false);
	gameData->setBoolForKey(reach_match_80_countdown_submitted, false);
	gameData->setBoolForKey(reach_match_90_countdown_submitted, false);
	gameData->setBoolForKey(reach_match_100_countdown_submitted, false);
	gameData->setBoolForKey(no_bonus_items_used_first_50_matches_countdown_submitted, false);
	gameData->setBoolForKey(remaining_time_90_seconds_countdown_submitted, false);
	gameData->setBoolForKey(maintain_time_above_20_submitted, false);

	//classic
	gameData->setBoolForKey(reach_match_1_classic_submitted, false);
	gameData->setBoolForKey(reach_match_10_classic_submitted, false);
	gameData->setBoolForKey(reach_match_20_classic_submitted, false);
	gameData->setBoolForKey(reach_match_30_classic_submitted, false);
	gameData->setBoolForKey(reach_match_50_classic_submitted, false);
	gameData->setBoolForKey(reach_match_60_classic_submitted, false);
	gameData->setBoolForKey(reach_match_70_classic_submitted, false);
	gameData->setBoolForKey(reach_match_80_classic_submitted, false);
	gameData->setBoolForKey(reach_match_90_classic_submitted, false);
	gameData->setBoolForKey(reach_match_100_classic_submitted, false);
	gameData->setBoolForKey(no_bonus_items_used_first_50_matches_classic_submitted, false);

	//memory
	gameData->setBoolForKey(reach_match_10_memory_submitted, false);
	gameData->setBoolForKey(reach_match_20_memory_submitted, false);
	gameData->setBoolForKey(reach_match_30_memory_submitted, false);
	gameData->setBoolForKey(reach_match_50_memory_submitted, false);
	gameData->setBoolForKey(reach_match_60_memory_submitted, false);
	gameData->setBoolForKey(reach_match_70_memory_submitted, false);
	gameData->setBoolForKey(reach_match_80_memory_submitted, false);
	gameData->setBoolForKey(reach_match_90_memory_submitted, false);
	gameData->setBoolForKey(reach_match_100_memory_submitted, false);
	gameData->setBoolForKey(no_bonus_items_used_first_50_matches_memory_submitted, false);

	//Practice
	gameData->setBoolForKey(slow_poke_submitted, false);
	gameData->setBoolForKey(golf_rules_submitted, false);

	//New
	gameData->setBoolForKey(all_the_time_in_the_world_submitted, false);
	gameData->setBoolForKey(process_of_elimination_submitted, false);
	gameData->setBoolForKey(seeing_is_believing_submitted, false);
	gameData->setBoolForKey(clean_slate_submitted, false);

	gameData->setBoolForKey(take_the_points_submitted, false);
	gameData->setBoolForKey(take_the_bonus_submitted, false);

	gameData->setBoolForKey(the_10000_club_submitted, false);
	gameData->setBoolForKey(aspiring_star_submitted, false);
	gameData->setBoolForKey(six_figures_submitted, false);
	gameData->setBoolForKey(the_500000_club_submitted, false);

	gameData->setBoolForKey(matchmaker_training_submitted, false);
	gameData->setBoolForKey(matchmaker_junior_submitted, false);
	gameData->setBoolForKey(matchmaker_seasoned_submitted, false);

	gameData->setBoolForKey(classically_trained_submitted, false);
	gameData->setBoolForKey(time_management_submitted, false);
	gameData->setBoolForKey(practice_makes_perfect_submitted, false);
	gameData->setBoolForKey(photographic_memory_submitted, false);

//	gameData->flush();
	return;
}

void GameData::ResetPurchasedContent()
{
	//Get instance of user default singleton
	cocos2d::UserDefault* gameData = cocos2d::UserDefault::getInstance();
	
	gameData->setBoolForKey(ads_removed, false);
	gameData->setIntegerForKey(purchased_eliminate_left, 0);
	gameData->setIntegerForKey(purchased_time_plus_left, 0);
	gameData->setIntegerForKey(purchased_show_match_left, 0);
	gameData->setIntegerForKey(purchased_new_card_left, 0);
	
//	gameData->flush();
	return;
}


void GameData::DeleteSaveFile()
{	
	//Get instance of user default singleton
	cocos2d::UserDefault* gameData = cocos2d::UserDefault::getInstance();
	
	gameData->deleteValueForKey(times_opened_today);
	gameData->deleteValueForKey(times_opened_two_days_ago);
	gameData->deleteValueForKey(last_day_opened);
	gameData->deleteValueForKey(last_year_opened);

	gameData->deleteValueForKey(request_review);
	gameData->deleteValueForKey(app_rated);
	gameData->deleteValueForKey(music_on);
	gameData->deleteValueForKey(sound_on);

	gameData->deleteValueForKey(matches_made_today);
	gameData->deleteValueForKey(matches_made_today_for_review);
	gameData->deleteValueForKey(best_score_countdown);
	gameData->deleteValueForKey(best_match_countdown);
	gameData->deleteValueForKey(best_score_classic);
	gameData->deleteValueForKey(best_match_classic);
	gameData->deleteValueForKey(best_score_rushrelax);
	gameData->deleteValueForKey(best_match_rushrelax);
	gameData->deleteValueForKey(best_score_memory);
	gameData->deleteValueForKey(best_match_memory);

	gameData->deleteValueForKey(consecutive_near_misses);
	gameData->deleteValueForKey(total_color_matches);
	gameData->deleteValueForKey(total_shape_matches);
	gameData->deleteValueForKey(total_style_matches);

	//Progress
	gameData->deleteValueForKey(tutorial_complete_countdown);
	gameData->deleteValueForKey(tutorial_complete_classic);
	gameData->deleteValueForKey(tutorial_complete_rushrelax);
	gameData->deleteValueForKey(tutorial_complete_memory);
	gameData->deleteValueForKey(total_matches);
	gameData->deleteValueForKey(total_points);

	//Purchases
	gameData->deleteValueForKey(ads_removed);
	gameData->deleteValueForKey(purchased_eliminate_left);
	gameData->deleteValueForKey(purchased_time_plus_left);
	gameData->deleteValueForKey(purchased_show_match_left);
	gameData->deleteValueForKey(purchased_new_card_left);

	//Achievements
	gameData->deleteValueForKey(first_match);
	gameData->deleteValueForKey(earned_10000_total_matches);
	gameData->deleteValueForKey(earned_1000000_total_points);
	gameData->deleteValueForKey(first_5_matches_in_12_seconds);
	gameData->deleteValueForKey(first_10_matches_in_24_seconds);
	gameData->deleteValueForKey(first_20_matches_in_50_seconds);
	gameData->deleteValueForKey(two_hundred_matches_in_one_day);
	gameData->deleteValueForKey(five_hundred_color_matches);
	gameData->deleteValueForKey(five_hundred_shape_matches);
	gameData->deleteValueForKey(five_hundred_style_matches);
	gameData->deleteValueForKey(near_miss);
	gameData->deleteValueForKey(ten_consecutive_near_misses);
	gameData->deleteValueForKey(earned_all_achievements);

	//countdown
	gameData->deleteValueForKey(reach_match_10_countdown);
	gameData->deleteValueForKey(reach_match_20_countdown);
	gameData->deleteValueForKey(reach_match_30_countdown);
	gameData->deleteValueForKey(reach_match_50_countdown);
	gameData->deleteValueForKey(reach_match_60_countdown);
	gameData->deleteValueForKey(reach_match_70_countdown);
	gameData->deleteValueForKey(reach_match_80_countdown);
	gameData->deleteValueForKey(reach_match_90_countdown);
	gameData->deleteValueForKey(reach_match_100_countdown);
	gameData->deleteValueForKey(no_bonus_items_used_first_50_matches_countdown);
	gameData->deleteValueForKey(remaining_time_90_seconds_countdown);
	gameData->deleteValueForKey(maintain_time_above_20);

	//classic
	gameData->deleteValueForKey(reach_match_1_classic);
	gameData->deleteValueForKey(reach_match_10_classic);
	gameData->deleteValueForKey(reach_match_20_classic);
	gameData->deleteValueForKey(reach_match_30_classic);
	gameData->deleteValueForKey(reach_match_50_classic);
	gameData->deleteValueForKey(reach_match_60_classic);
	gameData->deleteValueForKey(reach_match_70_classic);
	gameData->deleteValueForKey(reach_match_80_classic);
	gameData->deleteValueForKey(reach_match_90_classic);
	gameData->deleteValueForKey(reach_match_100_classic);
	gameData->deleteValueForKey(no_bonus_items_used_first_50_matches_classic);

	//memory
	gameData->deleteValueForKey(reach_match_10_memory);
	gameData->deleteValueForKey(reach_match_20_memory);
	gameData->deleteValueForKey(reach_match_30_memory);
	gameData->deleteValueForKey(reach_match_50_memory);
	gameData->deleteValueForKey(reach_match_60_memory);
	gameData->deleteValueForKey(reach_match_70_memory);
	gameData->deleteValueForKey(reach_match_80_memory);
	gameData->deleteValueForKey(reach_match_90_memory);
	gameData->deleteValueForKey(reach_match_100_memory);
	gameData->deleteValueForKey(no_bonus_items_used_first_50_matches_memory);

	//Practice
	gameData->deleteValueForKey(slow_poke);
	gameData->deleteValueForKey(golf_rules);

	//New
	gameData->deleteValueForKey(all_the_time_in_the_world); 
	gameData->deleteValueForKey(time_plus_used); 
	gameData->deleteValueForKey(process_of_elimination); 
	gameData->deleteValueForKey(eliminate_used); 
	gameData->deleteValueForKey(seeing_is_believing); 
	gameData->deleteValueForKey(show_match_used); 
	gameData->deleteValueForKey(clean_slate); 
	gameData->deleteValueForKey(new_card_used); 

	gameData->deleteValueForKey(take_the_points); 
	gameData->deleteValueForKey(trades_accepted); 
	gameData->deleteValueForKey(take_the_bonus); 
	gameData->deleteValueForKey(bonuses_accepted); 

	gameData->deleteValueForKey(the_10000_club); 
	gameData->deleteValueForKey(aspiring_star); 
	gameData->deleteValueForKey(six_figures);
	gameData->deleteValueForKey(the_500000_club); 

	gameData->deleteValueForKey(matchmaker_training); 
	gameData->deleteValueForKey(matchmaker_junior); 
	gameData->deleteValueForKey(matchmaker_seasoned); 

	gameData->deleteValueForKey(classically_trained); 
	gameData->deleteValueForKey(classic_matches); 
	gameData->deleteValueForKey(time_management);
	gameData->deleteValueForKey(countdown_matches); 
	gameData->deleteValueForKey(practice_makes_perfect); 
	gameData->deleteValueForKey(practice_matches); 
	gameData->deleteValueForKey(photographic_memory);
	gameData->deleteValueForKey(memory_matches);

	//ACHIEVEMENTS SUBMITTED VARIABLES
	gameData->deleteValueForKey(first_match_submitted);
	gameData->deleteValueForKey(earned_10000_total_matches_submitted);
	gameData->deleteValueForKey(earned_1000000_total_points_submitted);
	gameData->deleteValueForKey(first_5_matches_in_12_seconds_submitted);
	gameData->deleteValueForKey(first_10_matches_in_24_seconds_submitted);
	gameData->deleteValueForKey(first_20_matches_in_50_seconds_submitted);
	gameData->deleteValueForKey(two_hundred_matches_in_one_day_submitted);
	gameData->deleteValueForKey(five_hundred_color_matches_submitted);
	gameData->deleteValueForKey(five_hundred_shape_matches_submitted);
	gameData->deleteValueForKey(five_hundred_style_matches_submitted);
	gameData->deleteValueForKey(near_miss_submitted);
	gameData->deleteValueForKey(ten_consecutive_near_misses_submitted);
	gameData->deleteValueForKey(earned_all_achievements_submitted);

	//countdown
	gameData->deleteValueForKey(reach_match_10_countdown_submitted);
	gameData->deleteValueForKey(reach_match_20_countdown_submitted);
	gameData->deleteValueForKey(reach_match_30_countdown_submitted);
	gameData->deleteValueForKey(reach_match_50_countdown_submitted);
	gameData->deleteValueForKey(reach_match_60_countdown_submitted);
	gameData->deleteValueForKey(reach_match_70_countdown_submitted);
	gameData->deleteValueForKey(reach_match_80_countdown_submitted);
	gameData->deleteValueForKey(reach_match_90_countdown_submitted);
	gameData->deleteValueForKey(reach_match_100_countdown_submitted);
	gameData->deleteValueForKey(no_bonus_items_used_first_50_matches_countdown_submitted);
	gameData->deleteValueForKey(remaining_time_90_seconds_countdown_submitted);
	gameData->deleteValueForKey(maintain_time_above_20_submitted);

	//classic
	gameData->deleteValueForKey(reach_match_1_classic_submitted);
	gameData->deleteValueForKey(reach_match_10_classic_submitted);
	gameData->deleteValueForKey(reach_match_20_classic_submitted);
	gameData->deleteValueForKey(reach_match_30_classic_submitted);
	gameData->deleteValueForKey(reach_match_50_classic_submitted);
	gameData->deleteValueForKey(reach_match_60_classic_submitted);
	gameData->deleteValueForKey(reach_match_70_classic_submitted);
	gameData->deleteValueForKey(reach_match_80_classic_submitted);
	gameData->deleteValueForKey(reach_match_90_classic_submitted);
	gameData->deleteValueForKey(reach_match_100_classic_submitted);
	gameData->deleteValueForKey(no_bonus_items_used_first_50_matches_classic_submitted);

	//memory
	gameData->deleteValueForKey(reach_match_10_memory_submitted);
	gameData->deleteValueForKey(reach_match_20_memory_submitted);
	gameData->deleteValueForKey(reach_match_30_memory_submitted);
	gameData->deleteValueForKey(reach_match_50_memory_submitted);
	gameData->deleteValueForKey(reach_match_60_memory_submitted);
	gameData->deleteValueForKey(reach_match_70_memory_submitted);
	gameData->deleteValueForKey(reach_match_80_memory_submitted);
	gameData->deleteValueForKey(reach_match_90_memory_submitted);
	gameData->deleteValueForKey(reach_match_100_memory_submitted);
	gameData->deleteValueForKey(no_bonus_items_used_first_50_matches_memory_submitted);

	//Practice
	gameData->deleteValueForKey(slow_poke_submitted);
	gameData->deleteValueForKey(golf_rules_submitted);

	//New
	gameData->deleteValueForKey(all_the_time_in_the_world_submitted);
	gameData->deleteValueForKey(process_of_elimination_submitted);
	gameData->deleteValueForKey(seeing_is_believing_submitted);
	gameData->deleteValueForKey(clean_slate_submitted);

	gameData->deleteValueForKey(take_the_points_submitted);
	gameData->deleteValueForKey(take_the_bonus_submitted);

	gameData->deleteValueForKey(the_10000_club_submitted);
	gameData->deleteValueForKey(aspiring_star_submitted);
	gameData->deleteValueForKey(six_figures_submitted);
	gameData->deleteValueForKey(the_500000_club_submitted);

	gameData->deleteValueForKey(matchmaker_training_submitted);
	gameData->deleteValueForKey(matchmaker_junior_submitted);
	gameData->deleteValueForKey(matchmaker_seasoned_submitted);

	gameData->deleteValueForKey(classically_trained_submitted);
	gameData->deleteValueForKey(time_management_submitted);
	gameData->deleteValueForKey(practice_makes_perfect_submitted);
	gameData->deleteValueForKey(photographic_memory_submitted);

	gameData->flush();
	return;
}

GameData::~GameData()
{
}

//Overall Stats
const char* times_played_today = "times_played_today"; //INT
const char* times_opened_today = "times_opened_today"; //INT
const char* times_opened_yesterday = "times_opened_yesterday"; //INT
const char* times_opened_two_days_ago = "times_opened_two_days_ago"; //INT
const char* last_day_opened = "last_day_opened"; //INT
const char* last_year_opened = "last_year_opened"; //INT

const char* request_review = "request_review"; //BOOL
const char* app_rated = "app_rated"; //BOOL
const char* music_on = "music_on"; //BOOL
const char* sound_on = "sound_on"; //BOOL

const char* matches_made_today = "matches_made_today"; //INT
const char* matches_made_today_for_review = "matches_made_today_for_review"; //INT
const char* best_score_countdown = "best_score_countdown"; //INT
const char* best_match_countdown = "best_match_countdown"; //INT
const char* best_score_classic = "best_score_classic"; //INT
const char* best_match_classic = "best_match_classic"; //INT
const char* best_score_rushrelax = "best_score_rushrelax"; //INT
const char* best_match_rushrelax = "best_match_rushrelax"; //INT
const char* best_score_memory = "best_score_memory"; //INT
const char* best_match_memory = "best_match_memory"; //INT

const char* consecutive_near_misses = "consecutive_near_misses"; //INT
const char* total_color_matches = "total_color_matches"; //INT
const char* total_shape_matches = "total_shape_matches"; //INT
const char* total_style_matches = "total_style_matches"; //INT

 //Progress
const char* tutorial_complete_how_to_play = "tutorial_complete_how_to_play"; //BOOL
const char* tutorial_complete_using_bonus_items = "tutorial_complete_using_bonus_items"; //BOOL
const char* tutorial_complete_countdown = "tutorial_complete_countdown"; //BOOL
const char* tutorial_complete_classic = "tutorial_complete_classic"; //BOOL
const char* tutorial_complete_rushrelax = "tutorial_complete_rushrelax"; //BOOL
const char* tutorial_complete_memory = "tutorial_complete_memory"; //BOOL
const char* total_matches = "total_matches"; //INT
const char* total_points = "total_points"; //INT

//Purchases 
const char* ads_removed = "ads_removed"; //BOOL
const char* purchased_time_plus_left = "purchased_time_plus_left"; //INT
const char* purchased_eliminate_left = "purchased_eliminate_left"; //INT
const char* purchased_show_match_left = "purchased_show_match_left"; //INT
const char* purchased_new_card_left = "purchased_new_card_left"; //INT

//Achievements
const char* first_match = "first_match"; //BOOL
const char* earned_10000_total_matches = "earned_10000_total_matches"; //BOOL
const char* earned_1000000_total_points = "earned_1000000_total_points"; //BOOL
const char* first_5_matches_in_12_seconds = "first_5_matches_in_12_seconds"; //BOOL
const char* first_10_matches_in_24_seconds = "first_10_matches_in_24_seconds"; //BOOL
const char* first_20_matches_in_50_seconds = "first_20_matches_in_50_seconds"; //BOOL
const char* two_hundred_matches_in_one_day = "two_hundred_matches_in_one_day"; //BOOL
const char* five_hundred_shape_matches = "five_hundred_shape_matches"; //BOOL
const char* five_hundred_color_matches = "five_hundred_color_matches"; //BOOL
const char* five_hundred_style_matches = "five_hundred_style_matches"; //BOOL
const char* ten_consecutive_near_misses = "ten_consecutive_near_misses"; //BOOL
const char* near_miss = "near_miss"; //BOOL
const char* earned_all_achievements = "earned_all_achievements"; //BOOL

//countdown
const char* reach_match_10_countdown = "reach_match_10_countdown"; //BOOL
const char* reach_match_20_countdown = "reach_match_20_countdown"; //BOOL
const char* reach_match_30_countdown = "reach_match_30_countdown"; //BOOL
const char* reach_match_50_countdown = "reach_match_50_countdown"; //BOOL
const char* reach_match_60_countdown = "reach_match_60_countdown"; //BOOL
const char* reach_match_70_countdown = "reach_match_70_countdown"; //BOOL
const char* reach_match_80_countdown = "reach_match_80_countdown"; //BOOL
const char* reach_match_90_countdown = "reach_match_90_countdown"; //BOOL
const char* reach_match_100_countdown = "reach_match_100_countdown"; //BOOL
const char* no_bonus_items_used_first_50_matches_countdown = "no_bonus_items_used_first_50_matches_countdown"; //BOOL
const char* remaining_time_90_seconds_countdown = "remaining_time_90_seconds_countdown"; //BOOL
const char* maintain_time_above_20 = "maintain_time_above_20"; //BOOL

//classic
const char* reach_match_1_classic = "reach_match_1_classic"; //BOOL
const char* reach_match_10_classic = "reach_match_10_classic"; //BOOL
const char* reach_match_20_classic = "reach_match_20_classic"; //BOOL
const char* reach_match_30_classic = "reach_match_30_classic"; //BOOL
const char* reach_match_50_classic = "reach_match_50_classic"; //BOOL
const char* reach_match_60_classic = "reach_match_60_classic"; //BOOL
const char* reach_match_70_classic = "reach_match_70_classic"; //BOOL
const char* reach_match_80_classic = "reach_match_80_classic"; //BOOL
const char* reach_match_90_classic = "reach_match_90_classic"; //BOOL
const char* reach_match_100_classic = "reach_match_100_classic"; //BOOL
const char* no_bonus_items_used_first_50_matches_classic = "no_bonus_items_used_first_50_matches_classic"; //BOOL

//memory
const char* reach_match_10_memory = "reach_match_10_memory"; //BOOL
const char* reach_match_20_memory = "reach_match_20_memory"; //BOOL
const char* reach_match_30_memory = "reach_match_30_memory"; //BOOL
const char* reach_match_50_memory = "reach_match_50_memory"; //BOOL
const char* reach_match_60_memory = "reach_match_60_memory"; //BOOL
const char* reach_match_70_memory = "reach_match_70_memory"; //BOOL
const char* reach_match_80_memory = "reach_match_80_memory"; //BOOL
const char* reach_match_90_memory = "reach_match_90_memory"; //BOOL
const char* reach_match_100_memory = "reach_match_100_memory"; //BOOL
const char* no_bonus_items_used_first_50_matches_memory = "no_bonus_items_used_first_50_matches_memory"; //BOOL

//Practice
const char* slow_poke = "slow_poke"; //BOOL
const char* golf_rules = "golf_rules"; //BOOL

//New
const char* all_the_time_in_the_world = "all_the_time_in_the_world"; //BOOL
const char* time_plus_used = "time_plus_used"; //INT
const char* process_of_elimination = "process_of_elimination"; //BOOL
const char* eliminate_used = "eliminate_used"; //INT
const char* seeing_is_believing = "seeing_is_believing"; //BOOL
const char* show_match_used = "show_match_used"; //INT
const char* clean_slate = "clean_slate"; //BOOL
const char* new_card_used = "new_card_used"; //INT

const char* take_the_points = "take_the_points"; //BOOL
const char* trades_accepted = "trades_accepted"; //INT
const char* take_the_bonus = "take_the_bonus"; //BOOL
const char* bonuses_accepted = "bonuses_accepted"; //INT

const char* the_10000_club = "the_10000_club"; //BOOL
const char* aspiring_star = "aspiring_star"; //BOOL
const char* six_figures = "six_figures"; //BOOL
const char* the_500000_club = "the_500000_club"; //BOOL

const char* matchmaker_training = "matchmaker_training"; //BOOL
const char* matchmaker_junior = "matchmaker_junior"; //BOOL
const char* matchmaker_seasoned = "matchmaker_seasoned"; //BOOL

const char* classically_trained = "classically_trained"; //BOOL
const char* classic_matches = "classic_matches"; //INT
const char* time_management = "time_management"; //BOOL
const char* countdown_matches = "countdown_matches"; //INT
const char* practice_makes_perfect = "practice_makes_perfect"; //BOOL
const char* practice_matches = "practice_matches"; //INT
const char* photographic_memory = "photographic_memory"; //BOOL
const char* memory_matches = "memory_matches"; //INT


//ACHIEVEMENT SUBMITTED VARIABLES
const char* first_match_submitted = "first_match_submitted"; //BOOL
const char* earned_10000_total_matches_submitted = "earned_10000_total_matches_submitted"; //BOOL
const char* earned_1000000_total_points_submitted = "earned_1000000_total_points_submitted"; //BOOL
const char* first_5_matches_in_12_seconds_submitted = "first_5_matches_in_12_seconds_submitted"; //BOOL
const char* first_10_matches_in_24_seconds_submitted = "first_10_matches_in_24_seconds_submitted"; //BOOL
const char* first_20_matches_in_50_seconds_submitted = "first_20_matches_in_50_seconds_submitted"; //BOOL
const char* two_hundred_matches_in_one_day_submitted = "two_hundred_matches_in_one_day_submitted"; //BOOL
const char* five_hundred_shape_matches_submitted = "five_hundred_shape_matches_submitted"; //BOOL
const char* five_hundred_color_matches_submitted = "five_hundred_color_matches_submitted"; //BOOL
const char* five_hundred_style_matches_submitted = "five_hundred_style_matches_submitted"; //BOOL
const char* ten_consecutive_near_misses_submitted = "ten_consecutive_near_misses_submitted"; //BOOL
const char* near_miss_submitted = "near_miss_submitted"; //BOOL
const char* earned_all_achievements_submitted = "earned_all_achievements_submitted"; //BOOL

//countdown
const char* reach_match_10_countdown_submitted = "reach_match_10_countdown_submitted"; //BOOL
const char* reach_match_20_countdown_submitted = "reach_match_20_countdown_submitted"; //BOOL
const char* reach_match_30_countdown_submitted = "reach_match_30_countdown_submitted"; //BOOL
const char* reach_match_50_countdown_submitted = "reach_match_50_countdown_submitted"; //BOOL
const char* reach_match_60_countdown_submitted = "reach_match_60_countdown_submitted"; //BOOL
const char* reach_match_70_countdown_submitted = "reach_match_70_countdown_submitted"; //BOOL
const char* reach_match_80_countdown_submitted = "reach_match_80_countdown_submitted"; //BOOL
const char* reach_match_90_countdown_submitted = "reach_match_90_countdown_submitted"; //BOOL
const char* reach_match_100_countdown_submitted = "reach_match_100_countdown_submitted"; //BOOL
const char* no_bonus_items_used_first_50_matches_countdown_submitted = "no_bonus_items_used_first_50_matches_countdown_submitted"; //BOOL
const char* remaining_time_90_seconds_countdown_submitted = "remaining_time_90_seconds_countdown_submitted"; //BOOL
const char* maintain_time_above_20_submitted = "maintain_time_above_20_submitted"; //BOOL

//classic
const char* reach_match_1_classic_submitted = "reach_match_1_classic_submitted"; //BOOL
const char* reach_match_10_classic_submitted = "reach_match_10_classic_submitted"; //BOOL
const char* reach_match_20_classic_submitted = "reach_match_20_classic_submitted"; //BOOL
const char* reach_match_30_classic_submitted = "reach_match_30_classic_submitted"; //BOOL
const char* reach_match_50_classic_submitted = "reach_match_50_classic_submitted"; //BOOL
const char* reach_match_60_classic_submitted = "reach_match_60_classic_submitted"; //BOOL
const char* reach_match_70_classic_submitted = "reach_match_70_classic_submitted"; //BOOL
const char* reach_match_80_classic_submitted = "reach_match_80_classic_submitted"; //BOOL
const char* reach_match_90_classic_submitted = "reach_match_90_classic_submitted"; //BOOL
const char* reach_match_100_classic_submitted = "reach_match_100_classic_submitted"; //BOOL
const char* no_bonus_items_used_first_50_matches_classic_submitted = "no_bonus_items_used_first_50_matches_classic_submitted"; //BOOL

//memory
const char* reach_match_10_memory_submitted = "reach_match_10_memory_submitted"; //BOOL
const char* reach_match_20_memory_submitted = "reach_match_20_memory_submitted"; //BOOL
const char* reach_match_30_memory_submitted = "reach_match_30_memory_submitted"; //BOOL
const char* reach_match_50_memory_submitted = "reach_match_50_memory_submitted"; //BOOL
const char* reach_match_60_memory_submitted = "reach_match_60_memory_submitted"; //BOOL
const char* reach_match_70_memory_submitted = "reach_match_70_memory_submitted"; //BOOL
const char* reach_match_80_memory_submitted = "reach_match_80_memory_submitted"; //BOOL
const char* reach_match_90_memory_submitted = "reach_match_90_memory_submitted"; //BOOL
const char* reach_match_100_memory_submitted = "reach_match_100_memory_submitted"; //BOOL
const char* no_bonus_items_used_first_50_matches_memory_submitted = "no_bonus_items_used_first_50_matches_memory_submitted"; //BOOL

//Practice
const char* slow_poke_submitted = "slow_poke_submitted"; //BOOL
const char* golf_rules_submitted = "golf_rules_submitted"; //BOOL

//New
const char* all_the_time_in_the_world_submitted = "all_the_time_in_the_world_submitted"; //BOOL
const char* process_of_elimination_submitted = "process_of_elimination_submitted"; //BOOL
const char* seeing_is_believing_submitted = "seeing_is_believing_submitted"; //BOOL
const char* clean_slate_submitted = "clean_slate_submitted"; //BOOL

const char* take_the_points_submitted = "take_the_points_submitted"; //BOOL
const char* take_the_bonus_submitted = "take_the_bonus_submitted"; //BOOL

const char* the_10000_club_submitted = "the_10000_club_submitted"; //BOOL
const char* aspiring_star_submitted = "aspiring_star_submitted"; //BOOL
const char* six_figures_submitted = "six_figures_submitted"; //BOOL
const char* the_500000_club_submitted = "the_500000_club_submitted"; //BOOL

const char* matchmaker_training_submitted = "matchmaker_training_submitted"; //BOOL
const char* matchmaker_junior_submitted = "matchmaker_junior_submitted"; //BOOL
const char* matchmaker_seasoned_submitted = "matchmaker_seasoned_submitted"; //BOOL

const char* classically_trained_submitted = "classically_trained_submitted"; //BOOL
const char* time_management_submitted = "time_management_submitted"; //BOOL
const char* practice_makes_perfect_submitted = "practice_makes_perfect_submitted"; //BOOL
const char* photographic_memory_submitted = "photographic_memory_submitted"; //BOOL