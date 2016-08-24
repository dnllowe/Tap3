#pragma once
#include "Shape.h"

Shape::Shape()
{
}

Shape * Shape::create(cocos2d::SpriteBatchNode * batch, bool top, int typeInput, cocos2d::Color3B colorInput, int rotationInput, int outlineInput)
{
	Shape* sprite = new Shape;
	std::string type;

	//Determine shape based on input
	switch (typeInput)
	{
	case CIRCLE:
		type = "circle";
		break;
	case SQUARE:
		type = "square";
		break;
	case DIAMOND:
		type = "diamond";
		break;
	case TRIANGLE:
		type = "triangle";
		break;
	case HEXAGON:
		type = "hexagon";
		break;
	case OCTAGON:
		type = "octagon";
		break;
	}

	if (top) //Top halves are "light" images
		type.append("_light.png");
	else if (!top) //Bottom halves are "dark" images
		type.append("_dark.png");

	if (sprite->initWithSpriteFrameName(type))
	{
		sprite->autorelease();
		batch->addChild(sprite);
		sprite->getTexture()->setAliasTexParameters();
		sprite->setAnchorPoint(cocos2d::Vec2(0, 0));
		return sprite;
	}
	CC_SAFE_DELETE(sprite);
	return NULL;
}

int Shape::GetType()
{
	return shapeType;
}

int Shape::GetRotation()
{
	return rotation;
}

int Shape::GetOutlineType()
{
	return outline;
}

Shape::~Shape()
{
}
