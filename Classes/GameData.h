#pragma once
#include "pch.h"

class GameData
{
public:

	//Create save file with default values
	static void CreateSaveFile();

	//Resets game settings and progress to defaults. Retains purchase variables
	static void ResetStats();

	//Resets unlocked/purchased content
	static void ResetPurchasedContent();

	//Delete save file
	static void DeleteSaveFile();

	~GameData();

	protected:
		GameData();
};

//Overall Stats
extern const char* times_played_today; //INT
extern const char* times_opened_today; //INT
extern const char* times_opened_yesterday; //INT
extern const char* times_opened_two_days_ago; //INT
extern const char* last_day_opened; //INT
extern const char* last_year_opened; //INT

extern const char* matches_made_today; //INT
extern const char* matches_made_today_for_review; //INT
extern const char* best_score_countdown; //INT
extern const char* best_match_countdown; //INT
extern const char* best_score_classic; //INT
extern const char* best_match_classic; //INT
extern const char* best_score_rushrelax; //INT
extern const char* best_match_rushrelax; //INT
extern const char* best_score_memory; //INT
extern const char* best_match_memory; //INT

extern const char* consecutive_near_misses; //INT
extern const char* total_color_matches; //INT
extern const char* total_shape_matches; //INT
extern const char* total_style_matches; //INT

extern const char* request_review; //BOOL
extern const char* app_rated; //BOOL
extern const char* music_on; //BOOL
extern const char* sound_on; //BOOL

//Progress
extern const char* tutorial_complete_how_to_play; //BOOL
extern const char* tutorial_complete_using_bonus_items; //BOOL
extern const char* tutorial_complete_countdown; //BOOL
extern const char* tutorial_complete_classic; //BOOL
extern const char* tutorial_complete_rushrelax; //BOOL
extern const char* tutorial_complete_memory; //BOOL
extern const char* total_matches; //INT
extern const char* total_points; //INT

//Purchases 
extern const char* ads_removed; //BOOL
extern const char* purchased_time_plus_left; //INT
extern const char* purchased_eliminate_left; //INT
extern const char* purchased_show_match_left; //INT
extern const char* purchased_new_card_left; //INT


//Achievements
extern const char* first_match; //BOOL
extern const char* earned_10000_total_matches; //BOOL
extern const char* earned_1000000_total_points; //BOOL
extern const char* first_5_matches_in_12_seconds; //BOOL
extern const char* first_10_matches_in_24_seconds; //BOOL
extern const char* first_20_matches_in_50_seconds; //BOOL
extern const char* near_miss; //BOOL
extern const char* two_hundred_matches_in_one_day; //BOOL
extern const char* five_hundred_shape_matches; //BOOL
extern const char* five_hundred_color_matches; //BOOL
extern const char* five_hundred_style_matches; //BOOL
extern const char* ten_consecutive_near_misses; //BOOL
extern const char* earned_all_achievements; //BOOL

//countdown
extern const char* reach_match_10_countdown; //BOOL
extern const char* reach_match_20_countdown; //BOOL
extern const char* reach_match_30_countdown; //BOOL
extern const char* reach_match_50_countdown; //BOOL
extern const char* reach_match_60_countdown; //BOOL
extern const char* reach_match_70_countdown; //BOOL
extern const char* reach_match_80_countdown; //BOOL
extern const char* reach_match_90_countdown; //BOOL
extern const char* reach_match_100_countdown; //BOOL
extern const char* no_bonus_items_used_first_50_matches_countdown; //BOOL
extern const char* remaining_time_90_seconds_countdown; //BOOL
extern const char* maintain_time_above_20; //BOOL

//classic
extern const char* reach_match_1_classic; //BOOL
extern const char* reach_match_10_classic; //BOOL
extern const char* reach_match_20_classic; //BOOL
extern const char* reach_match_30_classic; //BOOL
extern const char* reach_match_50_classic; //BOOL
extern const char* reach_match_60_classic; //BOOL
extern const char* reach_match_70_classic; //BOOL
extern const char* reach_match_80_classic; //BOOL
extern const char* reach_match_90_classic; //BOOL
extern const char* reach_match_100_classic; //BOOL
extern const char* no_bonus_items_used_first_50_matches_classic; //BOOL

//memory
extern const char* reach_match_10_memory; //BOOL
extern const char* reach_match_20_memory; //BOOL
extern const char* reach_match_30_memory; //BOOL
extern const char* reach_match_50_memory; //BOOL
extern const char* reach_match_60_memory; //BOOL
extern const char* reach_match_70_memory; //BOOL
extern const char* reach_match_80_memory; //BOOL
extern const char* reach_match_90_memory; //BOOL
extern const char* reach_match_100_memory; //BOOL
extern const char* no_bonus_items_used_first_50_matches_memory; //BOOL

//Practice
extern const char* slow_poke; //BOOL
extern const char* golf_rules; //BOOL
extern const char* beat_practice; //BOOL

//New
extern const char* all_the_time_in_the_world; //BOOL
extern const char* time_plus_used; //INT
extern const char* process_of_elimination; //BOOL
extern const char* eliminate_used; //INT
extern const char* seeing_is_believing; //BOOL
extern const char* show_match_used; //INT
extern const char* clean_slate; //BOOL
extern const char* new_card_used; //INT

extern const char* take_the_points; //BOOL
extern const char* trades_accepted; //INT
extern const char* take_the_bonus; //BOOL
extern const char* bonuses_accepted; //INT

extern const char* the_10000_club; //BOOL
extern const char* aspiring_star; //BOOL
extern const char* six_figures; //BOOL
extern const char* the_500000_club; //BOOL

extern const char* matchmaker_training; //BOOL
extern const char* matchmaker_junior; //BOOL
extern const char* matchmaker_seasoned; //BOOL

extern const char* classically_trained; //BOOL
extern const char* classic_matches; //INT
extern const char* time_management; //BOOL
extern const char* countdown_matches; //INT
extern const char* practice_makes_perfect; //BOOL
extern const char* practice_matches; //INT
extern const char* photographic_memory; //BOOL
extern const char* memory_matches; //INT

//ACHIEVEMENT SUBMISSION VARIABLES
extern const char* first_match_submitted; //BOOL
extern const char* earned_10000_total_matches_submitted; //BOOL
extern const char* earned_1000000_total_points_submitted; //BOOL
extern const char* first_5_matches_in_12_seconds_submitted; //BOOL
extern const char* first_10_matches_in_24_seconds_submitted; //BOOL
extern const char* first_20_matches_in_50_seconds_submitted; //BOOL
extern const char* near_miss_submitted; //BOOL
extern const char* two_hundred_matches_in_one_day_submitted; //BOOL
extern const char* five_hundred_shape_matches_submitted; //BOOL
extern const char* five_hundred_color_matches_submitted; //BOOL
extern const char* five_hundred_style_matches_submitted; //BOOL
extern const char* ten_consecutive_near_misses_submitted; //BOOL
extern const char* earned_all_achievements_submitted; //BOOL

//countdown
extern const char* reach_match_10_countdown_submitted; //BOOL
extern const char* reach_match_20_countdown_submitted; //BOOL
extern const char* reach_match_30_countdown_submitted; //BOOL
extern const char* reach_match_50_countdown_submitted; //BOOL
extern const char* reach_match_60_countdown_submitted; //BOOL
extern const char* reach_match_70_countdown_submitted; //BOOL
extern const char* reach_match_80_countdown_submitted; //BOOL
extern const char* reach_match_90_countdown_submitted; //BOOL
extern const char* reach_match_100_countdown_submitted; //BOOL
extern const char* no_bonus_items_used_first_50_matches_countdown_submitted; //BOOL
extern const char* remaining_time_90_seconds_countdown_submitted; //BOOL
extern const char* maintain_time_above_20_submitted; //BOOL

//classic
extern const char* reach_match_1_classic_submitted; //BOOL
extern const char* reach_match_10_classic_submitted; //BOOL
extern const char* reach_match_20_classic_submitted; //BOOL
extern const char* reach_match_30_classic_submitted; //BOOL
extern const char* reach_match_50_classic_submitted; //BOOL
extern const char* reach_match_60_classic_submitted; //BOOL
extern const char* reach_match_70_classic_submitted; //BOOL
extern const char* reach_match_80_classic_submitted; //BOOL
extern const char* reach_match_90_classic_submitted; //BOOL
extern const char* reach_match_100_classic_submitted; //BOOL
extern const char* no_bonus_items_used_first_50_matches_classic_submitted; //BOOL

//memory
extern const char* reach_match_10_memory_submitted; //BOOL
extern const char* reach_match_20_memory_submitted; //BOOL
extern const char* reach_match_30_memory_submitted; //BOOL
extern const char* reach_match_50_memory_submitted; //BOOL
extern const char* reach_match_60_memory_submitted; //BOOL
extern const char* reach_match_70_memory_submitted; //BOOL
extern const char* reach_match_80_memory_submitted; //BOOL
extern const char* reach_match_90_memory_submitted; //BOOL
extern const char* reach_match_100_memory_submitted; //BOOL
extern const char* no_bonus_items_used_first_50_matches_memory_submitted; //BOOL

//Practice
extern const char* slow_poke_submitted; //BOOL
extern const char* golf_rules_submitted; //BOOL

//New
extern const char* all_the_time_in_the_world_submitted; //BOOL
extern const char* process_of_elimination_submitted; //BOOL
extern const char* seeing_is_believing_submitted; //BOOL
extern const char* clean_slate_submitted; //BOOL

extern const char* take_the_points_submitted; //BOOL
extern const char* take_the_bonus_submitted; //BOOL

extern const char* the_10000_club_submitted; //BOOL
extern const char* aspiring_star_submitted; //BOOL
extern const char* six_figures_submitted; //BOOL
extern const char* the_500000_club_submitted; //BOOL

extern const char* matchmaker_training_submitted; //BOOL
extern const char* matchmaker_junior_submitted; //BOOL
extern const char* matchmaker_seasoned_submitted; //BOOL

extern const char* classically_trained_submitted; //BOOL
extern const char* time_management_submitted; //BOOL
extern const char* practice_makes_perfect_submitted; //BOOL
extern const char* photographic_memory_submitted; //BOOL

