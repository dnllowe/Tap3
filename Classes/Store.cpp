#include "pch.h"
#include "Store.h"


cocos2d::Scene * Store::CreateScene()
{
	cocos2d::Scene* scene = cocos2d::Scene::createWithPhysics();

	cocos2d::Layer* layer = Store::create();
	
	scene->addChild(layer);

	return scene;
}

bool Store::init()
{
    if(!gameData)
        gameData = cocos2d::UserDefault::getInstance();

	//Initialize origin
	SCREEN_WIDTH = glview->getDesignResolutionSize().width;
	SCREEN_HEIGHT = glview->getDesignResolutionSize().height;
	center.x = SCREEN_WIDTH / 2;
	center.y = SCREEN_HEIGHT / 2;

	//Enable native UI scrolling
	addChild(scrollView); //Add scrollView as main child. Add all scrolling assets to scrollView
	scrollView->setInertiaScrollEnabled(true);
	scrollView->setAnchorPoint(cocos2d::Vec2(0, 0));
	scrollView->setPosition(cocos2d::Vec2(0, 0));
	scrollView->setDirection(cocos2d::ui::ScrollView::Direction::VERTICAL);
	scrollView->setBounceEnabled(true);
	scrollView->setTouchEnabled(true);
	scrollView->setSwallowTouches(false);

	//Set clear color to white
	director->setClearColor(cocos2d::Color4F(1, 1, 1, 1));

	nrgFunctions::CreateSpriteBatch("spritebatch", batch, cache);
	scrollView->addChild(batch);

	int titleTextSize;
	int priceTextSize;
	int buyTextSize;
	int descriptionTextSize;
	int subtitleTextSize;
	int separatorTextSize;
	currentTimePlus= gameData->getIntegerForKey(purchased_time_plus_left);
	currentEliminate = gameData->getIntegerForKey(purchased_eliminate_left);
	currentShowMatch = gameData->getIntegerForKey(purchased_show_match_left);
	currentNewCard = gameData->getIntegerForKey(purchased_new_card_left);

	std::string separatorText = "";

//#if(CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	separatorText = "______________________________";
//#endif

//#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
//	separatorText = "_________________________";
//#endif

	std::string timePlusDescriptionString = "Adds 10 seconds to your remaining time. Crucial when you need an extra moment in a pinch.";
	std::string eliminateDescriptionString = "Each use removes 2 non-matching tiles from the card. Helps you hone in on the matches.";
	std::string showMatchDescriptionString = "Each use partially fades one of the matching tiles. Use to get a clue for the matching pattern.";
	std::string newCardDescriptionString = "Resets the card with new tiles. Use if you've made a mistake or want to try a different matching rule.";
	std::string comboDescription_1_String = "$1.00 Savings. You Get:\n\n10x Time +\n15x Eliminate Tiles\n10x Show Match\n10x Reset Tile";
	std::string comboDescription_2_String = "$3.00 Savings. You Get:\n\n20x Time +\n30x Eliminate Tiles\n20x Show Match\n20x Reset Tile";
	std::string comboDescription_3_String = "BEST VALUE $20.00 Savings\nYou're in this for the long-haul:\n\n100x Time +\n150x Eliminate Tiles\n100x Show Match\n100x Reset Tile.";
	
	sprintf(sz, "(%d left)", currentTimePlus);
	std::string timePlusQuantityString(sz);
	sprintf(sz, "(%d left)", currentEliminate);
	std::string eliminateQuantityString(sz);
	sprintf(sz, "(%d left)", currentShowMatch);
	std::string showMatchQuantityString(sz);
	sprintf(sz, "(%d left)", currentNewCard);
	std::string newCardQuantityString(sz);
	sprintf(sz, "\n\n(%d left)\n(%d left)\n(%d left)\n(%d left)", currentTimePlus, currentEliminate, currentShowMatch, currentNewCard);
	std::string comboQuantityString(sz);
	sprintf(sz, "");

    titleTextSize = 37;
    priceTextSize = 18;
    buyTextSize = 20;
    descriptionTextSize = 15;
    subtitleTextSize = 30;
    separatorTextSize = 12;
 
    backButton = nrgButton::create("back_button.png", "back_button_pressed.png", false);
	addChild(backButton);
	title = nrgText::create("Bonus Items", "fonts/alba.super.ttf", titleTextSize, SCREEN_WIDTH, 0, 0, cocos2d::Color4B(20, 175, 255, 128));
	separator_1 = nrgText::create(separatorText, "fonts/alba.super.ttf", separatorTextSize, SCREEN_WIDTH, 0, 0, cocos2d::Color4B(20, 175, 255, 128));
	
	buy_1 = nrgButton::create(batch, "menu_button.png", "menu_button_pressed.png", false);
	buy_2 = nrgButton::create(batch, "menu_button.png", "menu_button_pressed.png", false);
	buy_3 = nrgButton::create(batch, "menu_button.png", "menu_button_pressed.png", false);
	buy_4 = nrgButton::create(batch, "menu_button.png", "menu_button_pressed.png", false);
	buy_5 = nrgButton::create(batch, "menu_button.png", "menu_button_pressed.png", false);
	buy_6 = nrgButton::create(batch, "menu_button.png", "menu_button_pressed.png", false);
	buy_7 = nrgButton::create(batch, "menu_button.png", "menu_button_pressed.png", false);

	buy_1_text = nrgText::create("Buy", "fonts/POE.ttf", buyTextSize, SCREEN_WIDTH, 0, 0, cocos2d::Color4B::WHITE);
	buy_2_text = nrgText::create("Buy", "fonts/POE.ttf", buyTextSize, SCREEN_WIDTH, 0, 0, cocos2d::Color4B::WHITE);
	buy_3_text = nrgText::create("Buy", "fonts/POE.ttf", buyTextSize, SCREEN_WIDTH, 0, 0, cocos2d::Color4B::WHITE);
	buy_4_text = nrgText::create("Buy", "fonts/POE.ttf", buyTextSize, SCREEN_WIDTH, 0, 0, cocos2d::Color4B::WHITE);
	buy_5_text = nrgText::create("Buy", "fonts/POE.ttf", buyTextSize, SCREEN_WIDTH, 0, 0, cocos2d::Color4B::WHITE);
	buy_6_text = nrgText::create("Buy", "fonts/POE.ttf", buyTextSize, SCREEN_WIDTH, 0, 0, cocos2d::Color4B::WHITE);
	buy_7_text = nrgText::create("Buy", "fonts/POE.ttf", buyTextSize, SCREEN_WIDTH, 0, 0, cocos2d::Color4B::WHITE);

	price_1 = nrgText::create("10x for $0.99", "fonts/POE.ttf", priceTextSize, SCREEN_WIDTH, 0, 0, cocos2d::Color4B::GRAY);
	price_2 = nrgText::create("15x for $0.99", "fonts/POE.ttf", priceTextSize, SCREEN_WIDTH, 0, 0, cocos2d::Color4B::GRAY);
	price_3 = nrgText::create("10x for $0.99", "fonts/POE.ttf", priceTextSize, SCREEN_WIDTH, 0, 0, cocos2d::Color4B::GRAY);
	price_4 = nrgText::create("10x for $0.99", "fonts/POE.ttf", priceTextSize, SCREEN_WIDTH, 0, 0, cocos2d::Color4B::GRAY);
	price_5 = nrgText::create("1x for $2.99", "fonts/POE.ttf", priceTextSize, SCREEN_WIDTH, 0, 0, cocos2d::Color4B::GRAY);
	price_6 = nrgText::create("1x for $4.99", "fonts/POE.ttf", priceTextSize, SCREEN_WIDTH, 0, 0, cocos2d::Color4B::GRAY);
	price_7 = nrgText::create("1x for $19.99", "fonts/POE.ttf", priceTextSize, SCREEN_WIDTH, 0, 0, cocos2d::Color4B::GRAY);

	timePlusQuantity = nrgText::create(timePlusQuantityString, "fonts/POE.ttf", descriptionTextSize, SCREEN_WIDTH * 3 / 4, 0, 0, cocos2d::Color4B::GRAY);
	eliminateQuantity = nrgText::create(eliminateQuantityString, "fonts/POE.ttf", descriptionTextSize, SCREEN_WIDTH * 3 / 4, 0, 0, cocos2d::Color4B::GRAY);
	showMatchQuantity = nrgText::create(showMatchQuantityString, "fonts/POE.ttf", descriptionTextSize, SCREEN_WIDTH * 3 / 4, 0, 0, cocos2d::Color4B::GRAY);
	newCardQuantity = nrgText::create(newCardQuantityString, "fonts/POE.ttf", descriptionTextSize, SCREEN_WIDTH * 3 / 4, 0, 0, cocos2d::Color4B::GRAY);

	comboQuantity_1 = nrgText::create(comboQuantityString, "fonts/POE.ttf", descriptionTextSize, SCREEN_WIDTH * 3 / 4, 0, 0, cocos2d::Color4B::GRAY);
	comboQuantity_2 = nrgText::create(comboQuantityString, "fonts/POE.ttf", descriptionTextSize, SCREEN_WIDTH * 3 / 4, 0, 0, cocos2d::Color4B::GRAY);
	comboQuantityString.insert(comboQuantityString.begin(), '\n');
	comboQuantity_3 = nrgText::create(comboQuantityString, "fonts/POE.ttf", descriptionTextSize, SCREEN_WIDTH * 3 / 4, 0, 0, cocos2d::Color4B::GRAY);


	timePlusMain = nrgSprite::create(batch, "time_tile.png", false);
	timePlusSmallThumbnail_1 = nrgSprite::create(batch, "time_tile.png", false);
	timePlusSmallThumbnail_2 = nrgSprite::create(batch, "time_tile.png", false);
	timePlusSmallThumbnail_3 = nrgSprite::create(batch, "time_tile.png", false);
	timePlusDescription = nrgText::create(timePlusDescriptionString, "fonts/POE.ttf", descriptionTextSize, SCREEN_WIDTH * 3 / 4, 0, 0, cocos2d::Color4B::GRAY);
	timePlusTitle = nrgText::create("Time +", "fonts/POE.ttf", subtitleTextSize, SCREEN_WIDTH * 3 / 4, 0, 0, cocos2d::Color4B::GRAY);

	eliminateMain = nrgSprite::create(batch, "eliminate_tile.png", false);
	eliminateSmallThumbnail_1 = nrgSprite::create(batch, "eliminate_tile.png", false);
	eliminateSmallThumbnail_2 = nrgSprite::create(batch, "eliminate_tile.png", false);
	eliminateSmallThumbnail_3 = nrgSprite::create(batch, "eliminate_tile.png", false);
	eliminateDescription = nrgText::create(eliminateDescriptionString, "fonts/POE.ttf", descriptionTextSize, SCREEN_WIDTH * 3 / 4, 0, 0, cocos2d::Color4B::GRAY);
	eliminateTitle = nrgText::create("Eliminate Tiles", "fonts/POE.ttf", subtitleTextSize, SCREEN_WIDTH * 3 / 4, 0, 0, cocos2d::Color4B::GRAY);

	showMatchMain = nrgSprite::create(batch, "show_match_tile.png", false);
	showMatchSmallThumbnail_1 = nrgSprite::create(batch, "show_match_tile.png", false);
	showMatchSmallThumbnail_2 = nrgSprite::create(batch, "show_match_tile.png", false);
	showMatchSmallThumbnail_3 = nrgSprite::create(batch, "show_match_tile.png", false);
	showMatchDescription = nrgText::create(showMatchDescriptionString, "fonts/POE.ttf", descriptionTextSize, SCREEN_WIDTH * 3 / 4, 0, 0, cocos2d::Color4B::GRAY);
	showMatchTitle = nrgText::create("Show Match", "fonts/POE.ttf", subtitleTextSize, SCREEN_WIDTH * 3 / 4, 0, 0, cocos2d::Color4B::GRAY);

	newCardMain = nrgSprite::create(batch, "new_card_tile.png", false);
	newCardSmallThumbnail_1 = nrgSprite::create(batch, "new_card_tile.png", false);
	newCardSmallThumbnail_2 = nrgSprite::create(batch, "new_card_tile.png", false);
	newCardSmallThumbnail_3 = nrgSprite::create(batch, "new_card_tile.png", false);
	newCardDescription = nrgText::create(newCardDescriptionString, "fonts/POE.ttf", descriptionTextSize, SCREEN_WIDTH * 3 / 4, 0, 0, cocos2d::Color4B::GRAY);
	newCardTitle = nrgText::create("Reset Tiles", "fonts/POE.ttf", subtitleTextSize, SCREEN_WIDTH * 3 / 4, 0, 0, cocos2d::Color4B::GRAY);

	comboDescription_1 = nrgText::create(comboDescription_1_String, "fonts/POE.ttf", descriptionTextSize, SCREEN_WIDTH * 3 / 4, 0, 0, cocos2d::Color4B::GRAY);
	comboTitle_1 = nrgText::create("Combo Pack", "fonts/POE.ttf", subtitleTextSize, SCREEN_WIDTH * 3 / 4, 0, 0, cocos2d::Color4B::GRAY);

	comboDescription_2 = nrgText::create(comboDescription_2_String, "fonts/POE.ttf", descriptionTextSize, SCREEN_WIDTH * 3 / 4, 0, 0, cocos2d::Color4B::GRAY);
	comboTitle_2 = nrgText::create("Double Pack", "fonts/POE.ttf", subtitleTextSize, SCREEN_WIDTH * 3 / 4, 0, 0, cocos2d::Color4B::GRAY);
	
	comboDescription_3 = nrgText::create(comboDescription_3_String, "fonts/POE.ttf", descriptionTextSize, SCREEN_WIDTH * 3 / 4, 0, 0, cocos2d::Color4B::GRAY);
	comboTitle_3 = nrgText::create("The All Nighter", "fonts/POE.ttf", subtitleTextSize, SCREEN_WIDTH * 3 / 4, 0, 0, cocos2d::Color4B::GRAY);

	addChild(title);
	addChild(separator_1);

	
	buy_1->AddToScrollView(scrollView);
	buy_2->AddToScrollView(scrollView);
	buy_3->AddToScrollView(scrollView);
	buy_4->AddToScrollView(scrollView);
	buy_5->AddToScrollView(scrollView);
	buy_6->AddToScrollView(scrollView);
	buy_7->AddToScrollView(scrollView);
	
	scrollView->addChild(buy_1_text);
	scrollView->addChild(buy_2_text);
	scrollView->addChild(buy_3_text);
	scrollView->addChild(buy_4_text);
	scrollView->addChild(buy_5_text);
	scrollView->addChild(buy_6_text);
	scrollView->addChild(buy_7_text);

	scrollView->addChild(price_1);
	scrollView->addChild(price_2);
	scrollView->addChild(price_3);
	scrollView->addChild(price_4);
	scrollView->addChild(price_5);
	scrollView->addChild(price_6);
	scrollView->addChild(price_7);

	scrollView->addChild(timePlusQuantity);
	scrollView->addChild(eliminateQuantity);
	scrollView->addChild(showMatchQuantity);
	scrollView->addChild(newCardQuantity);

	scrollView->addChild(comboQuantity_1);
	scrollView->addChild(comboQuantity_2);
	scrollView->addChild(comboQuantity_3);

	scrollView->addChild(eliminateTitle);
	scrollView->addChild(timePlusTitle);
	scrollView->addChild(showMatchTitle);
	scrollView->addChild(newCardTitle);

	scrollView->addChild(eliminateDescription);
	scrollView->addChild(timePlusDescription);
	scrollView->addChild(showMatchDescription);
	scrollView->addChild(newCardDescription);

	scrollView->addChild(comboTitle_1);
	scrollView->addChild(comboDescription_1);
	scrollView->addChild(comboTitle_2);
	scrollView->addChild(comboDescription_2);
	scrollView->addChild(comboTitle_3);
	scrollView->addChild(comboDescription_3);
	

	nrgFunctions::ResizeForDevice(backButton);
	
	nrgFunctions::ResizeForDevice(timePlusMain);
	nrgFunctions::ResizeForDevice(timePlusSmallThumbnail_1);
	nrgFunctions::ResizeForDevice(timePlusSmallThumbnail_2);
	nrgFunctions::ResizeForDevice(timePlusSmallThumbnail_3);

	nrgFunctions::ResizeForDevice(eliminateMain);
	nrgFunctions::ResizeForDevice(eliminateSmallThumbnail_1);
	nrgFunctions::ResizeForDevice(eliminateSmallThumbnail_2);
	nrgFunctions::ResizeForDevice(eliminateSmallThumbnail_3);

	nrgFunctions::ResizeForDevice(showMatchMain);
	nrgFunctions::ResizeForDevice(showMatchSmallThumbnail_1);
	nrgFunctions::ResizeForDevice(showMatchSmallThumbnail_2);
	nrgFunctions::ResizeForDevice(showMatchSmallThumbnail_3);

	nrgFunctions::ResizeForDevice(newCardMain);
	nrgFunctions::ResizeForDevice(newCardSmallThumbnail_1);
	nrgFunctions::ResizeForDevice(newCardSmallThumbnail_2);
	nrgFunctions::ResizeForDevice(newCardSmallThumbnail_3);

	nrgFunctions::ResizeForDevice(buy_1);
	nrgFunctions::ResizeForDevice(buy_2);
	nrgFunctions::ResizeForDevice(buy_3);
	nrgFunctions::ResizeForDevice(buy_4);
	nrgFunctions::ResizeForDevice(buy_5);
	nrgFunctions::ResizeForDevice(buy_6);
	nrgFunctions::ResizeForDevice(buy_7);

	//Shrink thumbnails
	float scale = eliminateSmallThumbnail_1->getScale();

	eliminateSmallThumbnail_1->setScale(0.5 * scale);
	eliminateSmallThumbnail_2->setScale(0.5 * scale);
	eliminateSmallThumbnail_3->setScale(0.5 * scale);
	timePlusSmallThumbnail_1->setScale(0.5 * scale);
	timePlusSmallThumbnail_2->setScale(0.5 * scale);
	timePlusSmallThumbnail_3->setScale(0.5 * scale);
	showMatchSmallThumbnail_1->setScale(0.5 * scale);
	showMatchSmallThumbnail_2->setScale(0.5 * scale);
	showMatchSmallThumbnail_3->setScale(0.5 * scale);
	newCardSmallThumbnail_1->setScale(0.5 * scale);
	newCardSmallThumbnail_2->setScale(0.5 * scale);
	newCardSmallThumbnail_3->setScale(0.5 * scale);

	//Add touch listeners, disable until onEnter
	backButton->addEvents();
	backButton->ToggleEnabled(false);
	buy_1->addEvents();
	buy_1->ToggleEnabled(false);
	buy_2->addEvents();
	buy_2->ToggleEnabled(false);
	buy_3->addEvents();
	buy_3->ToggleEnabled(false);
	buy_4->addEvents();
	buy_4->ToggleEnabled(false);
	buy_5->addEvents();
	buy_5->ToggleEnabled(false);
	buy_6->addEvents();
	buy_6->ToggleEnabled(false);
	buy_7->addEvents();
	buy_7->ToggleEnabled(false);

	//Position elements
	backButton->setPositionX(10);
	backButton->SetTop(SCREEN_HEIGHT - 20);

	title->Center();
	title->setPositionY(backButton->getPositionY() - 50);
	
	separator_1->Center();
	separator_1->setPositionY(title->GetBottom() + 10);

	float distanceToBelowSeparator = SCREEN_HEIGHT - separator_1->GetBottom();
	float fullHeight = timePlusTitle->GetScaledHeight() + timePlusDescription->GetScaledHeight() + 220 
					 + eliminateTitle->GetScaledHeight() + eliminateDescription->GetScaledHeight() + 220
					 + showMatchTitle->GetScaledHeight()  + showMatchDescription->GetScaledHeight() + 220
					 + newCardTitle->GetScaledHeight() + newCardDescription->GetScaledHeight() + 220
					 + comboTitle_1->GetScaledHeight() + comboDescription_1->GetScaledHeight() + 220
					 + comboTitle_2->GetScaledHeight() + comboDescription_2->GetScaledHeight() + 210 
					 + comboTitle_3->GetScaledHeight() + comboDescription_3->GetScaledHeight() + 50;

	//Create ScrollView container
	scrollView->setSize(cocos2d::Size(SCREEN_WIDTH, SCREEN_HEIGHT - distanceToBelowSeparator)); //This is the scrollView window size
	scrollView->setInnerContainerSize(cocos2d::Size(SCREEN_WIDTH, fullHeight)); //This is the container size
	cocos2d::Size size = scrollView->getInnerContainerSize();

	timePlusMain->setPositionX(20);
	timePlusMain->setPositionY(size.height - 60);
	timePlusTitle->setPositionY(timePlusMain->GetTop() - timePlusMain->GetHeight() / 2 + timePlusTitle->GetScaledHeight() / 2);
	timePlusTitle->setPositionX(timePlusMain->GetRightSide() + 10);
	timePlusDescription->Center();
	timePlusDescription->setPositionY(timePlusTitle->GetBottom() - 10);
	price_1->Center();
	price_1->setPositionY(timePlusDescription->GetBottom() - 25);
	timePlusQuantity->Center();
	timePlusQuantity->setPositionY(price_1->GetBottom());
	buy_1->Center();
	buy_1->SetTop(timePlusQuantity->GetBottom() - 25);
	nrgFunctions::Center(buy_1_text, buy_1);
	buy_1_text->setPositionY(buy_1_text->getPositionY() + 2);

	eliminateMain->setPositionX(20);
	eliminateMain->setPositionY(timePlusDescription->GetBottom() - 210);
	eliminateTitle->setPositionY(eliminateMain->GetTop() - eliminateMain->GetHeight() / 2 + eliminateTitle->GetScaledHeight() / 2);
	eliminateTitle->setPositionX(eliminateMain->GetRightSide() + 10);
	eliminateDescription->Center();
	eliminateDescription->setPositionY(eliminateTitle->GetBottom() - 10);
	price_2->Center();
	price_2->setPositionY(eliminateDescription->GetBottom() - 25);
	eliminateQuantity->Center();
	eliminateQuantity->setPositionY(price_2->GetBottom());
	buy_2->Center();
	buy_2->SetTop(eliminateQuantity->GetBottom() - 25);
	nrgFunctions::Center(buy_2_text, buy_2);
	buy_2_text->setPositionY(buy_2_text->getPositionY() + 2);

	showMatchMain->setPositionX(20);
	showMatchMain->setPositionY(eliminateDescription->GetBottom() - 210);
	showMatchTitle->setPositionY(showMatchMain->GetTop() - showMatchMain->GetHeight() / 2 + showMatchTitle->GetScaledHeight() / 2);
	showMatchTitle->setPositionX(showMatchMain->GetRightSide() + 10);
	showMatchDescription->Center();
	showMatchDescription->setPositionY(showMatchTitle->GetBottom() - 10);
	price_3->Center();
	price_3->setPositionY(showMatchDescription->GetBottom() - 25);
	showMatchQuantity->Center();
	showMatchQuantity->setPositionY(price_3->GetBottom());
	buy_3->Center();
	buy_3->SetTop(showMatchQuantity->GetBottom() - 25);
	nrgFunctions::Center(buy_3_text, buy_3);
	buy_3_text->setPositionY(buy_3_text->getPositionY() + 2);

	newCardMain->setPositionX(20);
	newCardMain->setPositionY(showMatchDescription->GetBottom() - 210);
	newCardTitle->setPositionY(newCardMain->GetTop() - newCardMain->GetHeight() / 2 + newCardTitle->GetScaledHeight() / 2);
	newCardTitle->setPositionX(newCardMain->GetRightSide() + 10);
	newCardDescription->Center();
	newCardDescription->setPositionY(newCardTitle->GetBottom() - 10);
	price_4->Center();
	price_4->setPositionY(newCardDescription->GetBottom() - 25);
	newCardQuantity->Center();
	newCardQuantity->setPositionY(price_4->GetBottom());
	buy_4->Center();
	buy_4->SetTop(newCardQuantity->GetBottom() - 25);
	nrgFunctions::Center(buy_4_text, buy_4);
	buy_4_text->setPositionY(buy_4_text->getPositionY() + 2);

	//Combo 1 Thumbnail Cluster
	float thumbnailHeight = timePlusSmallThumbnail_1->GetHeight() * 2 + 2;

	timePlusSmallThumbnail_1->setPositionX(20);
	timePlusSmallThumbnail_1->setPositionY(newCardDescription->GetBottom() - 210);
	eliminateSmallThumbnail_1->setPositionX(timePlusSmallThumbnail_1->GetRightSide() + 2);
	eliminateSmallThumbnail_1->setPositionY(timePlusSmallThumbnail_1->getPositionY());
	showMatchSmallThumbnail_1->setPositionX(20);
	showMatchSmallThumbnail_1->SetTop(eliminateSmallThumbnail_1->getPositionY() - 2);
	newCardSmallThumbnail_1->setPositionX(showMatchSmallThumbnail_1->GetRightSide() + 2);
	newCardSmallThumbnail_1->setPositionY(showMatchSmallThumbnail_1->getPositionY());
	
	comboTitle_1->setPositionY(showMatchSmallThumbnail_1->getPositionY() + thumbnailHeight / 2 + comboTitle_1->GetScaledHeight() / 2);
	comboTitle_1->setPositionX(eliminateSmallThumbnail_1->GetRightSide() + 10);
	comboDescription_1->Center();
	comboDescription_1->setPositionY(comboTitle_1->GetBottom() - 10);
	comboQuantity_1->setPositionY(comboDescription_1->getPositionY());
	comboQuantity_1->setPositionX(SCREEN_WIDTH * 3  / 4);
	price_5->Center();
	price_5->setPositionY(comboDescription_1->GetBottom() - 25);
	buy_5->Center();
	buy_5->SetTop(price_5->GetBottom() - 25);
	nrgFunctions::Center(buy_5_text, buy_5);
	buy_5_text->setPositionY(buy_5_text->getPositionY() + 2);

	timePlusSmallThumbnail_2->setPositionX(20);
	timePlusSmallThumbnail_2->setPositionY(comboDescription_1->GetBottom() - 200);
	eliminateSmallThumbnail_2->setPositionX(timePlusSmallThumbnail_2->GetRightSide() + 2);
	eliminateSmallThumbnail_2->setPositionY(timePlusSmallThumbnail_2->getPositionY());
	showMatchSmallThumbnail_2->setPositionX(20);
	showMatchSmallThumbnail_2->SetTop(eliminateSmallThumbnail_2->getPositionY() - 2);
	newCardSmallThumbnail_2->setPositionX(showMatchSmallThumbnail_2->GetRightSide() + 2);
	newCardSmallThumbnail_2->setPositionY(showMatchSmallThumbnail_2->getPositionY());

	comboTitle_2->setPositionY(showMatchSmallThumbnail_2->getPositionY() + thumbnailHeight / 2 + comboTitle_2->GetScaledHeight() / 2);
	comboTitle_2->setPositionX(eliminateSmallThumbnail_2->GetRightSide() + 10);
	comboDescription_2->Center();
	comboDescription_2->setPositionY(comboTitle_2->GetBottom() - 10);
	comboQuantity_2->setPositionY(comboDescription_2->getPositionY());
	comboQuantity_2->setPositionX(SCREEN_WIDTH * 3 / 4);
	price_6->Center();
	price_6->setPositionY(comboDescription_2->GetBottom() - 25);
	buy_6->Center();
	buy_6->SetTop(price_6->GetBottom() - 25);
	nrgFunctions::Center(buy_6_text, buy_6);
	buy_6_text->setPositionY(buy_6_text->getPositionY() + 2);

	timePlusSmallThumbnail_3->setPositionX(20);
	timePlusSmallThumbnail_3->setPositionY(comboDescription_2->GetBottom() - 200);
	eliminateSmallThumbnail_3->setPositionX(timePlusSmallThumbnail_3->GetRightSide() + 2);
	eliminateSmallThumbnail_3->setPositionY(timePlusSmallThumbnail_3->getPositionY());
	showMatchSmallThumbnail_3->setPositionX(20);
	showMatchSmallThumbnail_3->SetTop(eliminateSmallThumbnail_3->getPositionY() - 2);
	newCardSmallThumbnail_3->setPositionX(showMatchSmallThumbnail_3->GetRightSide() + 2);
	newCardSmallThumbnail_3->setPositionY(showMatchSmallThumbnail_3->getPositionY());

	comboTitle_3->setPositionY(showMatchSmallThumbnail_3->getPositionY() + thumbnailHeight / 2 + comboTitle_3->GetScaledHeight() / 2);
	comboTitle_3->setPositionX(eliminateSmallThumbnail_3->GetRightSide() + 10);
	comboDescription_3->Center();
	comboDescription_3->setPositionY(comboTitle_3->GetBottom() - 10);
	comboQuantity_3->setPositionY(comboDescription_3->getPositionY());
	comboQuantity_3->setPositionX(SCREEN_WIDTH * 3 / 4);
	price_7->Center();
	price_7->setPositionY(comboDescription_3->GetBottom() - 25);
	buy_7->Center();
	buy_7->SetTop(price_7->GetBottom() - 25);
	nrgFunctions::Center(buy_7_text, buy_7);
	buy_7_text->setPositionY(buy_7_text->getPositionY() + 2);

	scheduleUpdate();
	return true;
}

void Store::update(float dt)
{
	if (backButton->WasPressed())
	{
		audio->PlayClip("low_tone");
		director->popScene();
	}

#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	if (buy_1->WasPressed())
		sdkbox::IAP::purchase("time_plus");

	if (buy_2->WasPressed())
		sdkbox::IAP::purchase("eliminate");

	if (buy_3->WasPressed())
		sdkbox::IAP::purchase("show_match");

	if (buy_4->WasPressed())
		sdkbox::IAP::purchase("reset");

	if (buy_5->WasPressed())
		sdkbox::IAP::purchase("combo");

	if (buy_6->WasPressed())
		sdkbox::IAP::purchase("double");

	if (buy_7->WasPressed())
		sdkbox::IAP::purchase("all_nighter");
#endif

	if (updateQuantity)
	{
		updateQuantity = false;

		std::string timePlusDescriptionString = "Adds 10 seconds to your remaining time. Crucial when you need an extra moment in a pinch.";
		std::string eliminateDescriptionString = "Each use removes 2 non-matching tiles from the card. Helps you hone in on the matches.";
		std::string showMatchDescriptionString = "Each use partially fades one of the matching tiles. Use to get a clue for the matching pattern.";
		std::string newCardDescriptionString = "Resets the card with new tiles. Use if you've made a mistake or want to try a different matching pattern.";
		std::string comboDescription_1_String = "$1.00 Savings. You Get:\n\n10x Time +\n15x Eliminate Tiles\n10x Show Match\n10x Reset Tile";
		std::string comboDescription_2_String = "$3.00 Savings. You Get:\n\n20x Time +\n30x Eliminate Tiles\n20x Show Match\n20x Reset Tile";
		std::string comboDescription_3_String = "BEST VALUE $20.00 Savings\nYou're in this for the long-haul:\n\n100x Time +\n150x Eliminate Tiles\n100x Show Match\n100x Reset Tile.";

		sprintf(sz, "(%d left)", currentTimePlus);
		std::string timePlusQuantityString(sz);
		sprintf(sz, "(%d left)", currentEliminate);
		std::string eliminateQuantityString(sz);
		sprintf(sz, "(%d left)", currentShowMatch);
		std::string showMatchQuantityString(sz);
		sprintf(sz, "(%d left)", currentNewCard);
		std::string newCardQuantityString(sz);
		sprintf(sz, "\n\n(%d left)\n(%d left)\n(%d left)\n(%d left)", currentTimePlus, currentEliminate, currentShowMatch, currentNewCard);
		std::string comboQuantityString(sz);
		sprintf(sz, "");

		timePlusDescription->setString(timePlusDescriptionString);
		eliminateDescription->setString(eliminateDescriptionString);
		showMatchDescription->setString(showMatchDescriptionString);
		newCardDescription->setString(newCardDescriptionString);
		comboDescription_1->setString(comboDescription_1_String);
		comboDescription_2->setString(comboDescription_2_String);
		comboDescription_3->setString(comboDescription_3_String);
		timePlusQuantity->setString(timePlusQuantityString);
		eliminateQuantity->setString(eliminateQuantityString);
		showMatchQuantity->setString(showMatchQuantityString);
		newCardQuantity->setString(newCardQuantityString);
		comboQuantity_1->setString(comboQuantityString);
		comboQuantity_2->setString(comboQuantityString);
		comboQuantityString.insert(comboQuantityString.begin(), '\n');
		comboQuantity_3->setString(comboQuantityString);
	}

	return;
}

void Store::onEnter()
{
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    audio->SetMusicVolume(0.35);
#endif
    
#if(CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    audio->SetMusicVolume(0.05);
#endif
    
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    sdkbox::PluginAdMob::hide("home");
    
    //Set IAP listener
    sdkbox::IAP::setListener(this);
#endif
    
	backButton->ToggleEnabled(true);
	buy_1->ToggleEnabled(true);
	buy_2->ToggleEnabled(true);
	buy_3->ToggleEnabled(true);
	buy_4->ToggleEnabled(true);
	buy_5->ToggleEnabled(true);
	buy_6->ToggleEnabled(true);
	buy_7->ToggleEnabled(true);
	cocos2d::Layer::onEnter();
	return;
}

void Store::onExit()
{
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    audio->SetMusicVolume(1);
#endif
    
#if(CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    audio->SetMusicVolume(0.10);
#endif
    
    //Set IAP listener
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    sdkbox::IAP::removeListener();
#endif
    
    backButton->ToggleEnabled(false);
	buy_1->ToggleEnabled(false);
	buy_2->ToggleEnabled(false);
	buy_3->ToggleEnabled(false);
	buy_4->ToggleEnabled(false);
	buy_5->ToggleEnabled(false);
	buy_6->ToggleEnabled(false);
	buy_7->ToggleEnabled(false);
	cocos2d::Layer::onExit();
	return;
}

Store::Store()
{
}


Store::~Store()
{
}

#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
void Store::onSuccess(sdkbox::Product const & p)
{
	if (p.name == "time_plus")
	{
		currentTimePlus += 10;
		gameData->setIntegerForKey(purchased_time_plus_left, currentTimePlus);
	}
	else if (p.name == "eliminate")
	{
		currentEliminate += 15;
		gameData->setIntegerForKey(purchased_eliminate_left, currentEliminate);
	}
	else if (p.name == "show_match")
	{
		currentShowMatch += 10;
		gameData->setIntegerForKey(purchased_show_match_left, currentShowMatch);
	}
	else if (p.name == "reset")
	{
		currentNewCard += 10;
		gameData->setIntegerForKey(purchased_new_card_left, currentNewCard);
	}
	else if (p.name == "combo")
	{
		currentTimePlus += 10;
		currentEliminate += 15;
		currentShowMatch += 10;
		currentNewCard += 10;

		gameData->setIntegerForKey(purchased_time_plus_left, currentTimePlus);
		gameData->setIntegerForKey(purchased_eliminate_left, currentEliminate);
		gameData->setIntegerForKey(purchased_show_match_left, currentShowMatch);
		gameData->setIntegerForKey(purchased_new_card_left, currentNewCard);
	}
	else if (p.name == "double")
	{
		currentTimePlus += 20;
		currentEliminate += 30;
		currentShowMatch += 20;
		currentNewCard += 20;

		gameData->setIntegerForKey(purchased_time_plus_left, currentTimePlus);
		gameData->setIntegerForKey(purchased_eliminate_left, currentEliminate);
		gameData->setIntegerForKey(purchased_show_match_left, currentShowMatch);
		gameData->setIntegerForKey(purchased_new_card_left, currentNewCard);
	}
	else if (p.name == "all_nighter")
	{
		currentTimePlus += 100;
		currentEliminate += 150;
		currentShowMatch += 100;
		currentNewCard += 100;

		gameData->setIntegerForKey(purchased_time_plus_left, currentTimePlus);
		gameData->setIntegerForKey(purchased_eliminate_left, currentEliminate);
		gameData->setIntegerForKey(purchased_show_match_left, currentShowMatch);
		gameData->setIntegerForKey(purchased_new_card_left, currentNewCard);
	}

	updateQuantity = true;
	audio->PlayClip("triple_tone_high");
	CCLOG("Purchase Success: %s", p.id.c_str());
	return;
}

void Store::onFailure(sdkbox::Product const & p, const std::string & msg)
{
	CCLOG("Purchase Failed: %s", msg.c_str());
	audio->PlayClip("low_tone");
	return;
}

void Store::onCanceled(sdkbox::Product const & p)
{
	CCLOG("Purchase Canceled: %s", p.id.c_str());
	audio->PlayClip("low_tone");
	return;
}

void Store::onRestored(sdkbox::Product const & p)
{
	CCLOG("Purchase Restored: %s", p.name.c_str());
	audio->PlayClip("triple_tone_high");
	return;
}

void Store::onProductRequestFailure(const std::string & msg)
{
	CCLOG("Fail to load products");
	return;
}

void Store::onInitialized(bool ok)
{
	CCLOG("%s : %d", __func__, ok);
	return;
}

void Store::onRestoreComplete(bool ok, const std::string & msg)
{
	CCLOG("%s:%d:%s", __func__, ok, msg.data());
	return;
}

void Store::onProductRequestSuccess(std::vector<sdkbox::Product> const & products)
{
	return;
}
#endif
