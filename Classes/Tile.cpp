#pragma once
#include "Tile.h"

Tile::Tile(cocos2d::SpriteBatchNode * batch)
{
	shadow->initWithSpriteFrameName("tile_shadow.png");
	tile->initWithSpriteFrameName("tile.png");
	
	outlineTop->initWithSpriteFrameName("circle_outline_top.png");
	outlineBottom->initWithSpriteFrameName("circle_outline_bottom.png");

	shapeTop->initWithSpriteFrameName("circle_light.png");
	shapeBottom->initWithSpriteFrameName("circle_dark.png");
	highlight->initWithSpriteFrameName("highlight.png");

	styleTop->initWithSpriteFrameName("style_1.png");
	styleBottom->initWithSpriteFrameName("style_1.png");

	style3Top->initWithSpriteFrameName("style_1.png");
	style3Bottom->initWithSpriteFrameName("style_1.png");

	style5Top->initWithSpriteFrameName("style_1.png");
	style5Bottom->initWithSpriteFrameName("style_1.png");

	addChild(shadow);
	addChild(tile);
	addChild(outlineTop);
	addChild(outlineBottom);
	addChild(shapeTop);
	addChild(shapeBottom);
	addChild(styleTop);
	addChild(styleBottom);
	addChild(style3Top);
	addChild(style3Bottom);
	addChild(style5Top);
	addChild(style5Bottom);
	addChild(highlight);
	
	shape = 0;
	color.first = cocos2d::Color3B::GRAY;
	color.second = cocos2d::Color3B::GRAY;
	rotation = 0;
	style.first = SOLID;
	style.second = SOLID;

	return;
}

Tile * Tile::create(cocos2d::SpriteBatchNode * batch)
{
	Tile* sprite = new Tile(batch);
	
	if (sprite->initWithSpriteFrameName("tile.png"))
	{
		sprite->autorelease();
		sprite->addEvents();
		sprite->setOpacity(0);
		sprite->getTexture()->setAliasTexParameters();
		sprite->setAnchorPoint(cocos2d::Vec2(0, 0));
		return sprite;
	}
	CC_SAFE_DELETE(sprite);
	return NULL;
}

void Tile::addEvents()
{
	//Set flag for events added
	eventsAdded = true;
	touchEnabled = true;

	//Create listener for touch events
	touchListener = cocos2d::EventListenerTouchOneByOne::create();
	touchListener->setSwallowTouches(true);

	//On touch, check to see if sprite was touched
	touchListener->onTouchBegan = [&](cocos2d::Touch* touch, cocos2d::Event* event)
	{
		touchLocation = touch->getLocation();
		float scale = 1;
		float x = 0;
		float y = 0;
		float width = 0;
		float height = 0;

		//If Parent is scaled differently, it masks true scale of object and needs to be accounted for
		if (getParent()->getScale() != 1)
			scale = getParent()->getScale();

		x = getParent()->getPositionX() - getParent()->getContentSize().width * scale / 2;
		y = getParent()->getPositionY() - getParent()->getContentSize().height * scale / 2;
		width = getContentSize().width * scale;
		height = getContentSize().height * scale;
		cocos2d::Rect rect;
		
		//Create bounding box based on parent scale, parent position, and anchor point to parent to recreate real space
		rect.setRect(x + getPositionX() * scale - width / 2, y + getPositionY() * scale - height / 2, getContentSize().width * scale, getContentSize().height * scale);

		if (rect.containsPoint(touchLocation))
		{
			wasTouched = true;
			return true; // to indicate that we have consumed it.
		}
		return false; // we did not consume this event, pass thru.
	};

	//Code for what to do when touch is swiped across screen
	touchListener->onTouchMoved = [=](cocos2d::Touch* touch, cocos2d::Event* event)
	{
		TouchMove(touch);
		touchEvent(touch, touch->getLocation());
	};

	//Code for what to do when touch is released
	touchListener->onTouchEnded = [=](cocos2d::Touch* touch, cocos2d::Event* event)
	{
		TouchEnd();
		touchEvent(touch, touch->getLocation());
	};

	cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(touchListener, 2);
	
	return;
}

void Tile::ChangeShape(int shapeInput, std::pair<cocos2d::Color3B, cocos2d::Color3B> colorInput, int rotation, std::pair<int, int> styleInput)
{
	std::string type;

	//Set parameters based on input
	shape = shapeInput;
	color = colorInput;
	style = styleInput;

	//TOP OUTLINE

	//RED:
	if(color.first == cocos2d::Color3B(255, 100, 100))
		outlineColor.first = cocos2d::Color3B(200, 0, 0);

	//GREEN:
	if(color.first == cocos2d::Color3B(87, 225, 90))
		outlineColor.first = cocos2d::Color3B(26, 146, 28);

	//BLUE:
	if(color.first == cocos2d::Color3B(45, 54, 241))
		outlineColor.first = cocos2d::Color3B(2, 10, 160);

	//YELLOW:
	if(color.first == cocos2d::Color3B(250, 238, 46))
		outlineColor.first = cocos2d::Color3B(160, 151, 2);

	//PURPLE:
	if(color.first == cocos2d::Color3B(211, 101, 207))
		outlineColor.first = cocos2d::Color3B(170, 0, 255);

	//CYAN:
	if(color.first == cocos2d::Color3B(50, 255, 255))
		outlineColor.first = cocos2d::Color3B(0, 164, 164);

	//ORANGE:
	if(color.first == cocos2d::Color3B(255, 210, 140))
		outlineColor.first = cocos2d::Color3B(255, 130, 0);

		
	//OUTLINE BOTTOM
	
	//RED:
	if(color.second == cocos2d::Color3B(255, 50, 50))
		outlineColor.second = cocos2d::Color3B(200, 0, 0);
	
	//GREEN:
	if(color.second == cocos2d::Color3B(37, 211, 41))
		outlineColor.second = cocos2d::Color3B(26, 146, 28);

	//BLUE:
	if(color.second == cocos2d::Color3B(2, 13, 224))
		outlineColor.second = cocos2d::Color3B(2, 10, 160);

	//YELLOW:
	if(color.second == cocos2d::Color3B(234, 220, 2))
		outlineColor.second = cocos2d::Color3B(160, 151, 2);

	//PURPLE:
	if(color.second == cocos2d::Color3B(194, 54, 189))
		outlineColor.second = cocos2d::Color3B(170, 0, 255);

	//CYAN:
	if(color.second == cocos2d::Color3B(0, 242, 242))
		outlineColor.second = cocos2d::Color3B(0, 164, 164);

	//ORANGE:
	if(color.second == cocos2d::Color3B(255, 170, 50))
		outlineColor.second = cocos2d::Color3B(255, 130, 0);

	//Reset all outlines to blank (SOLID)
	styleTop->setSpriteFrame("style_1.png");
	styleBottom->setSpriteFrame("style_1.png");
	style3Top->setSpriteFrame("style_1.png");
	style3Bottom->setSpriteFrame("style_1.png");
	style5Top->setSpriteFrame("style_1.png");
	style5Bottom->setSpriteFrame("style_1.png");

	//Determine shape based on input
	switch (shape)
	{
	case CIRCLE:
		type = "circle_";
		break;
	case SQUARE:
		type = "square_";
		break;
	case DIAMOND:
		type = "diamond_";
		break;
	case TRIANGLE:
		type = "triangle_";
		break;
	case HEXAGON:
		type = "hexagon_";
		break;
	case OCTAGON:
		type = "octagon_";
		break;
	}
	
	//Establish color and load appropriately shaded texture
	outlineTop->setSpriteFrame(type + "outline_top.png");
	outlineBottom->setSpriteFrame(type + "outline_bottom.png");

	shapeTop->setSpriteFrame(type + "light.png");
	shapeBottom->setSpriteFrame(type + "dark.png");

	//Use non-modified color for outlines
	outlineColor = color;
	outlineTop->setColor(outlineColor.first);
	outlineBottom->setColor(outlineColor.second);

	shapeTop->setColor(color.first);
	shapeBottom->setColor(color.second);

	setRotation(90 * rotation);

	//Establish outline style for top half
	switch (style.first)
	{
	case SOLID:
		styleTop->setSpriteFrame("style_1.png");
		break;
	case OUTLINE:
		styleTop->setSpriteFrame(type + "style_2_top.png");
		break;
	case BLINDS:
		styleTop->setSpriteFrame("style_1.png");
		style3Top->setSpriteFrame("style_3_top.png");
		break;
	case CONCENTRIC:
		styleTop->setSpriteFrame(type + "style_4_top.png");
		break;
	case QUAD:
		styleTop->setSpriteFrame("style_1.png");
		style5Top->setSpriteFrame("style_5_top.png");
		break;
	case OUTLINE_BLINDS:
		styleTop->setSpriteFrame(type + "style_2_top.png");
		style3Top->setSpriteFrame("style_3_top.png");
		break;
	case OUTLINE_QUAD:
		styleTop->setSpriteFrame(type + "style_2_top.png");
		style5Top->setSpriteFrame("style_5_top.png");
		break;
	case CONCENTRIC_BLINDS:
		styleTop->setSpriteFrame(type + "style_4_top.png");
		style3Top->setSpriteFrame("style_3_top.png");
		break;
	case CONCENTRIC_QUAD:
		styleTop->setSpriteFrame(type + "style_4_top.png");
		style5Top->setSpriteFrame("style_5_top.png");
		break;
	case BLINDS_QUAD:
		styleTop->setSpriteFrame("style_1.png");
		style3Top->setSpriteFrame("style_3_top.png");
		style5Top->setSpriteFrame("style_5_top.png");
		break;
	case OUTLINE_BLINDS_QUAD:
		styleTop->setSpriteFrame(type + "style_2_top.png");
		style3Top->setSpriteFrame("style_3_top.png");
		style5Top->setSpriteFrame("style_5_top.png");
		break;
	case CONCENTRIC_BLINDS_QUAD:
		styleTop->setSpriteFrame(type + "style_4_top.png");
		style3Top->setSpriteFrame("style_3_top.png");
		style5Top->setSpriteFrame("style_5_top.png");
		break;
	}

	//Establish outline style for bottom half
	switch (style.second)
	{
	case SOLID:
		styleBottom->setSpriteFrame("style_1.png");
		break;
	case OUTLINE:
		styleBottom->setSpriteFrame(type + "style_2_bottom.png");
		break;
	case BLINDS:
		styleBottom->setSpriteFrame("style_1.png");
		style3Bottom->setSpriteFrame("style_3_bottom.png");
		break;
	case CONCENTRIC:
		styleBottom->setSpriteFrame(type + "style_4_bottom.png");
		break;
	case QUAD:
		styleBottom->setSpriteFrame("style_1.png");
		style5Bottom->setSpriteFrame("style_5_bottom.png");
		break;
	case OUTLINE_BLINDS:
		styleBottom->setSpriteFrame(type + "style_2_bottom.png");
		style3Bottom->setSpriteFrame("style_3_bottom.png");
		break;
	case OUTLINE_QUAD:
		styleBottom->setSpriteFrame(type + "style_2_bottom.png");
		style5Bottom->setSpriteFrame("style_5_bottom.png");
		break;
	case CONCENTRIC_BLINDS:
		styleBottom->setSpriteFrame(type + "style_4_bottom.png");
		style3Bottom->setSpriteFrame("style_3_bottom.png");
		break;
	case CONCENTRIC_QUAD:
		styleBottom->setSpriteFrame(type + "style_4_bottom.png");
		style5Bottom->setSpriteFrame("style_5_bottom.png");
		break;
	case BLINDS_QUAD:
		styleBottom->setSpriteFrame("style_1.png");
		style3Bottom->setSpriteFrame("style_3_bottom.png");
		style5Bottom->setSpriteFrame("style_5_bottom.png");
		break;
	case OUTLINE_BLINDS_QUAD:
		styleBottom->setSpriteFrame(type + "style_2_bottom.png");
		style3Bottom->setSpriteFrame("style_3_bottom.png");
		style5Bottom->setSpriteFrame("style_5_bottom.png");
		break;
	case CONCENTRIC_BLINDS_QUAD:
		styleBottom->setSpriteFrame(type + "style_4_bottom.png");
		style3Bottom->setSpriteFrame("style_3_bottom.png");
		style5Bottom->setSpriteFrame("style_5_bottom.png");
		break;
	}

	//Randomly decide when to flash highlight
	AnimateHighlight();

	return;
}

void Tile::SetFade(int value, bool highlightOn)
{
	//Prevent overlapping actions
	tile->stopAllActions();
	shadow->stopAllActions();

	outlineTop->stopAllActions();
	outlineBottom->stopAllActions();
	shapeTop->stopAllActions();
	shapeBottom->stopAllActions();
	styleTop->stopAllActions();
	styleBottom->stopAllActions();
	style3Top->stopAllActions();
	style3Bottom->stopAllActions();
	style5Top->stopAllActions();
	style5Bottom->stopAllActions();
	highlight->stopAllActions();

	tile->setOpacity(value);
	shadow->setOpacity(value);
	outlineTop->setOpacity(value);
	outlineBottom->setOpacity(value);
	shapeTop->setOpacity(value);
	shapeBottom->setOpacity(value);
	styleTop->setOpacity(value);
	styleBottom->setOpacity(value);
	style3Top->setOpacity(value);
	style3Bottom->setOpacity(value);
	style5Top->setOpacity(value);
	style5Bottom->setOpacity(value);

	//Turn off highlight
	if (!highlightOn)
		highlight->setOpacity(0);
	else
		highlight->setOpacity(value);
}

int Tile::GetShape()
{
	return shape;
}

int Tile::GetRotation()
{
	return rotation;
}

std::pair<int, int> Tile::GetStyle()
{
	return style;
}

std::pair<cocos2d::Color3B, cocos2d::Color3B> Tile::GetColor()
{
	return color;
}

void Tile::SetTilePosition(int value)
{
	tilePosition = value;
	return;
}

int Tile::GetTilePosition()
{
	return tilePosition;
}

bool Tile::WasTouched()
{
	if (wasTouched)
	{
		wasTouched = false;
		return true;
	}
	else
		return false;
}

void Tile::RestoreTile(float x, float y, float duration, float delay, bool stopActions)
{
	if (stopActions)
	{
		//Prevent overlapping actions
		tile->stopAllActions();
		shadow->stopAllActions();
		outlineTop->stopAllActions();
		outlineBottom->stopAllActions();
		shapeTop->stopAllActions();
		shapeBottom->stopAllActions();
		styleTop->stopAllActions();
		styleBottom->stopAllActions();
		style3Top->stopAllActions();
		style3Bottom->stopAllActions();
		style5Top->stopAllActions();
		style5Bottom->stopAllActions();
		highlight->stopAllActions();
	}

	tile->setOpacity(0);
	shadow->setOpacity(0);
	outlineTop->setOpacity(0);
	outlineBottom->setOpacity(0);
	shapeTop->setOpacity(0);
	shapeBottom->setOpacity(0);
	styleTop->setOpacity(0);
	styleBottom->setOpacity(0);
	style3Top->setOpacity(0);
	style3Bottom->setOpacity(0);
	style5Top->setOpacity(0);
	style5Bottom->setOpacity(0);
	highlight->setOpacity(0);

	float fadeTime = 0.5;

	ToggleTouch(true);

	//If no offset provided, keep tile at current position
	if (x == 0)
		x = getPositionX();
	if (y == 0)
		y = getPositionY();

	runAction(
		cocos2d::Sequence::create(
			cocos2d::DelayTime::create(delay),
			cocos2d::MoveTo::create(duration, cocos2d::Vec2(x, y)), NULL
		)
	);
	tile->runAction(
		cocos2d::Sequence::create(
			cocos2d::DelayTime::create(delay),
			cocos2d::FadeIn::create(fadeTime), NULL
		)
	);
	shadow->runAction(
		cocos2d::Sequence::create(
			cocos2d::DelayTime::create(delay),
			cocos2d::FadeIn::create(fadeTime), NULL
		)
	);
	outlineTop->runAction(
		cocos2d::Sequence::create(
			cocos2d::DelayTime::create(delay),
			cocos2d::FadeIn::create(fadeTime), NULL
		)
	);
	outlineBottom->runAction(
		cocos2d::Sequence::create(
			cocos2d::DelayTime::create(delay),
			cocos2d::FadeIn::create(fadeTime), NULL
		)
	);
	shapeTop->runAction(
		cocos2d::Sequence::create(
			cocos2d::DelayTime::create(delay),
			cocos2d::FadeIn::create(fadeTime), NULL
		)
	);
	shapeBottom->runAction(
		cocos2d::Sequence::create(
			cocos2d::DelayTime::create(delay),
			cocos2d::FadeIn::create(fadeTime), NULL
		)
	);
	styleTop->runAction(
		cocos2d::Sequence::create(
			cocos2d::DelayTime::create(delay),
			cocos2d::FadeIn::create(fadeTime), NULL
		)
	);
	styleBottom->runAction(
		cocos2d::Sequence::create(
			cocos2d::DelayTime::create(delay),
			cocos2d::FadeIn::create(fadeTime), NULL
		)
	);
	style3Top->runAction(
		cocos2d::Sequence::create(
			cocos2d::DelayTime::create(delay),
			cocos2d::FadeIn::create(fadeTime), NULL
		)
	);
	style3Bottom->runAction(
		cocos2d::Sequence::create(
			cocos2d::DelayTime::create(delay),
			cocos2d::FadeIn::create(fadeTime), NULL
		)
	);
	style5Top->runAction(
		cocos2d::Sequence::create(
			cocos2d::DelayTime::create(delay),
			cocos2d::FadeIn::create(fadeTime), NULL
		)
	);
	style5Bottom->runAction(
		cocos2d::Sequence::create(
			cocos2d::DelayTime::create(delay),
			cocos2d::FadeIn::create(fadeTime), NULL
		)
	);
	highlight->runAction(
		cocos2d::Sequence::create(
			cocos2d::DelayTime::create(delay + fadeTime),
			cocos2d::CallFunc::create([this]() { highlight->setOpacity(255); AnimateHighlight();}), NULL
		)
	);

	return;
}

void Tile::DismissTile(cocos2d::MoveBy * move, cocos2d::DelayTime* delay)
{
	//Prevent overlapping actions
	tile->stopAllActions();
	shadow->stopAllActions();
	outlineTop->stopAllActions();
	outlineBottom->stopAllActions();
	shapeTop->stopAllActions();
	shapeBottom->stopAllActions();
	styleTop->stopAllActions();
	styleBottom->stopAllActions();
	style3Top->stopAllActions();
	style3Bottom->stopAllActions();
	style5Top->stopAllActions();
	style5Bottom->stopAllActions();
	highlight->stopAllActions();
	highlight->setOpacity(0);

	if (move == NULL)
	{
		ToggleTouch(false);
		tile->runAction(cocos2d::FadeOut::create(0.25));
		shadow->runAction(cocos2d::FadeOut::create(0.25));
		outlineTop->runAction(cocos2d::FadeOut::create(0.25));
		outlineBottom->runAction(cocos2d::FadeOut::create(0.25));
		shapeTop->runAction(cocos2d::FadeOut::create(0.25));
		shapeBottom->runAction(cocos2d::FadeOut::create(0.25));
		styleTop->runAction(cocos2d::FadeOut::create(0.25));
		styleBottom->runAction(cocos2d::FadeOut::create(0.25));
		style3Top->runAction(cocos2d::FadeOut::create(0.25));
		style3Bottom->runAction(cocos2d::FadeOut::create(0.25));
		style5Top->runAction(cocos2d::FadeOut::create(0.25));
		style5Bottom->runAction(cocos2d::FadeOut::create(0.25));
		highlight->runAction(cocos2d::FadeOut::create(0.25));
	}

	else if (move)
	{
		ToggleTouch(false);
		tile->runAction(cocos2d::Sequence::create(delay, cocos2d::Spawn::create(cocos2d::FadeOut::create(0.25), move, NULL), NULL));
		shadow->runAction(cocos2d::Sequence::create(delay, cocos2d::Spawn::create(cocos2d::FadeOut::create(0.25), move, NULL), NULL));
		outlineTop->runAction(cocos2d::Sequence::create(delay, cocos2d::Spawn::create(cocos2d::FadeOut::create(0.25), move, NULL), NULL));
		outlineBottom->runAction(cocos2d::Sequence::create(delay, cocos2d::Spawn::create(cocos2d::FadeOut::create(0.25), move, NULL), NULL));
		shapeTop->runAction(cocos2d::Sequence::create(delay, cocos2d::Spawn::create(cocos2d::FadeOut::create(0.25), move, NULL), NULL));
		shapeBottom->runAction(cocos2d::Sequence::create(delay, cocos2d::Spawn::create(cocos2d::FadeOut::create(0.25), move, NULL), NULL));
		styleTop->runAction(cocos2d::Sequence::create(delay, cocos2d::Spawn::create(cocos2d::FadeOut::create(0.25), move, NULL), NULL));
		styleBottom->runAction(cocos2d::Sequence::create(delay, cocos2d::Spawn::create(cocos2d::FadeOut::create(0.25), move, NULL), NULL));
		style3Top->runAction(cocos2d::Sequence::create(delay, cocos2d::Spawn::create(cocos2d::FadeOut::create(0.25), move, NULL), NULL));
		style3Bottom->runAction(cocos2d::Sequence::create(delay, cocos2d::Spawn::create(cocos2d::FadeOut::create(0.25), move, NULL), NULL));
		style5Top->runAction(cocos2d::Sequence::create(delay, cocos2d::Spawn::create(cocos2d::FadeOut::create(0.25), move, NULL), NULL));
		style5Bottom->runAction(cocos2d::Sequence::create(delay, cocos2d::Spawn::create(cocos2d::FadeOut::create(0.25), move, NULL), NULL));
		highlight->runAction(cocos2d::Sequence::create(delay, cocos2d::Spawn::create(cocos2d::FadeOut::create(0.25), move, NULL), NULL));
	}

	return;
}

void Tile::StopActions()
{
	tile->stopAllActions();
	shadow->stopAllActions();
	outlineTop->stopAllActions();
	outlineBottom->stopAllActions();
	shapeTop->stopAllActions();
	shapeBottom->stopAllActions();
	styleTop->stopAllActions();
	styleBottom->stopAllActions();
	style3Top->stopAllActions();
	style3Bottom->stopAllActions();
	style5Top->stopAllActions();
	style5Bottom->stopAllActions();
	highlight->stopAllActions();

	return;
}

void Tile::HideShape()
{
	//Prevent overlapping actions
	outlineTop->stopAllActions();
	outlineBottom->stopAllActions();
	shapeTop->stopAllActions();
	shapeBottom->stopAllActions();
	styleTop->stopAllActions();
	styleBottom->stopAllActions();
	style3Top->stopAllActions();
	style3Bottom->stopAllActions();
	style5Top->stopAllActions();
	style5Bottom->stopAllActions();
	
	outlineTop->runAction(cocos2d::FadeOut::create(0.25));
	outlineBottom->runAction(cocos2d::FadeOut::create(0.25));
	shapeTop->runAction(cocos2d::FadeOut::create(0.25));
	shapeBottom->runAction(cocos2d::FadeOut::create(0.25));
	styleTop->runAction(cocos2d::FadeOut::create(0.25));
	styleBottom->runAction(cocos2d::FadeOut::create(0.25));
	style3Top->runAction(cocos2d::FadeOut::create(0.25));
	style3Bottom->runAction(cocos2d::FadeOut::create(0.25));
	style5Top->runAction(cocos2d::FadeOut::create(0.25));
	style5Bottom->runAction(cocos2d::FadeOut::create(0.25));
		
	return;
}


void Tile::UnhideShape()
{
	//Prevent overlapping actions
	outlineTop->stopAllActions();
	outlineBottom->stopAllActions();
	shapeTop->stopAllActions();
	shapeBottom->stopAllActions();
	styleTop->stopAllActions();
	styleBottom->stopAllActions();
	style3Top->stopAllActions();
	style3Bottom->stopAllActions();
	style5Top->stopAllActions();
	style5Bottom->stopAllActions();

	outlineTop->runAction(cocos2d::FadeIn::create(0.25));
	outlineBottom->runAction(cocos2d::FadeIn::create(0.25));
	shapeTop->runAction(cocos2d::FadeIn::create(0.25));
	shapeBottom->runAction(cocos2d::FadeIn::create(0.25));
	styleTop->runAction(cocos2d::FadeIn::create(0.25));
	styleBottom->runAction(cocos2d::FadeIn::create(0.25));
	style3Top->runAction(cocos2d::FadeIn::create(0.25));
	style3Bottom->runAction(cocos2d::FadeIn::create(0.25));
	style5Top->runAction(cocos2d::FadeIn::create(0.25));
	style5Bottom->runAction(cocos2d::FadeIn::create(0.25));

	return;
}

void Tile::AnimateHighlight()
{
	highlight->stopAllActions();

	//Randomly decide when to flash highlight
	float ms = float(nrgFunctions::GetRandom(0, 999)) / 1000.0f;
	int sec = nrgFunctions::GetRandom(0, 3);
	float delay = sec + ms;
	highlight->setPositionX(-GetWidth() / 5 * 2);
	highlight->runAction(
		cocos2d::Sequence::create(
			cocos2d::DelayTime::create(delay),
			cocos2d::MoveTo::create(0.4, cocos2d::Vec2(GetWidth() / 5 * 2, highlight->getPositionY())),
			cocos2d::CallFunc::create([this]() {AnimateHighlight();}), NULL)
		);
}

GLubyte Tile::getOpacity()
{
	return tile->getOpacity();
}

Tile::~Tile()
{
}
