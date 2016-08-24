#pragma once
#include "pch.h"
#include "nrgSprite.h"

class Shape : public nrgSprite
{
public:
	
	//Return shape object.
	//top: whether to load top or bottom shape texture
	//type: 0 = CIRCLE, 1 = SQUARE, 2 = DIAMOND, 3 = TRIANGLE, 4 = HEXAGON, 5 = OCTAGON
	//color = cocos2d::Color3B
	//rotation: 0 = 0 degrees, 1 = 90 degrees, 2 = 180 degrees, 3 = 270 degrees
	//outline: 0 = SOLID, 1 = OUTLINE, 2 = DOTTED
	static Shape* create(cocos2d::SpriteBatchNode* batch, bool top = true, int typeInput = 0, cocos2d::Color3B colorInput = cocos2d::Color3B::GRAY, int rotationInput = 0, int outlineInput = 0);

	//Get shape type (0 = CIRCLE, 1 = SQUARE, 2 = DIAMOND, 3 = TRIANGLE, 4 = HEXAGON, 5 = OCTAGON)
	int GetType();

	//Get shape rotation 0 = 0 degrees, 1 = 90 degrees, 2 = 180 degrees, 3 = 270 degrees
	int GetRotation();

	//Get shape outline type (0 = SOLID, 1 = OUTLINE, 2 = DOTTED)
	int GetOutlineType();

	~Shape();

protected:

	Shape();

	enum SHAPE { CIRCLE, SQUARE, DIAMOND, TRIANGLE, HEXAGON, OCTAGON };
	enum OUTLINE { SOLID, OUTLINE, DOTTED };
	int shapeType;
	cocos2d::Color3B color;
	int rotation;
	int outline;
};

