#pragma once
#include "pch.h"
#include "nrgSprite.h"

class Tile : public nrgSprite
{
public:

	//Return Tile object
	static Tile* create(cocos2d::SpriteBatchNode* batch);

	//Add touch functionality with respect to baseTile parent and scaling
	void addEvents();

	//Change parameters of entire shape
	void ChangeShape(int typeInput = CIRCLE, std::pair<cocos2d::Color3B, cocos2d::Color3B> colorInput = std::pair<cocos2d::Color3B, cocos2d::Color3B>{ cocos2d::Color3B::GRAY, cocos2d::Color3B::GRAY }, int rotation = 0, std::pair<int, int> outlineInput = std::pair<int, int>{ 0, 0 });
	
	//Sets tile assets' opacity
	void SetFade(int value, bool highlightOn = false);

	//Get shape type (0 = CIRCLE, 1 = SQUARE, 2 = DIAMOND, 3 = TRIANGLE, 4 = HEXAGON, 5 = OCTAGON)
	int GetShape();

	//Get shape rotation 0 = 0 degrees, 1 = 90 degrees, 2 = 180 degrees, 3 = 270 degrees
	int GetRotation();

	//Get shape outline type (0 = SOLID, 1 = OUTLINE, 2 = BLINDS, 3 = CONCENTRIC, 4 = QUAD, 5 = OUTLINE_BLINDS, 6 = OUTLINE_QUAD, 7 = CONCENTRIC_BLINDS, 8 = CONCENTRIC_QUAD, 9 = BLINDS_QUAD, 10 = OUTLINE_BLINDS_QUAD, 11 = CONCENTRIC_BLINDS_QUAD)
	std::pair<int, int> GetStyle();

	//Get color of top and bottom shape
	std::pair<cocos2d::Color3B, cocos2d::Color3B> GetColor();

	//Set tile position label
	void SetTilePosition(int value);

	//Get tiles position on base tile
	int GetTilePosition();

	//Check if tile was touched
	bool WasTouched();

	//Re-enable tile and fade in sprites
	void RestoreTile(float x = 0, float y = 0, float duration = 0, float delay = 0, bool stopActions = true);

	//Disable tile and fade out sprites
	void DismissTile(cocos2d::MoveBy* move = NULL, cocos2d::DelayTime* delay = NULL);

	//Stop all tile asset actions
	void StopActions();

	//Fades shape sprites to 0 opacity
	void HideShape();

	//Fades shape sprite back to 255 opacity
	void UnhideShape();

	//Animates highlight sprite over image
	void AnimateHighlight();

	//Override "getOpacity" so that it returns child opacity, not parent's
	GLubyte getOpacity();

	~Tile();

protected:

	Tile(cocos2d::SpriteBatchNode * batch);

	cocos2d::Sprite* tile = cocos2d::Sprite::create(); //Sprite for tile
	cocos2d::Sprite* shadow = cocos2d::Sprite::create(); //Sprite for tile drop shadow
	cocos2d::Sprite* shapeTop = cocos2d::Sprite::create(); //Shape for the tile (top half)
	cocos2d::Sprite* shapeBottom = cocos2d::Sprite::create(); //Shape for the tile (bottom half)
	cocos2d::Sprite* styleTop = cocos2d::Sprite::create(); //Style for shape tile (top half)
	cocos2d::Sprite* styleBottom = cocos2d::Sprite::create(); //Style for shape tile (bottom half)
	cocos2d::Sprite* style3Top = cocos2d::Sprite::create(); //Style 3 for shape tile (top half)
	cocos2d::Sprite* style5Top = cocos2d::Sprite::create(); //Style 5 for shape tile (top half)
	cocos2d::Sprite* style3Bottom = cocos2d::Sprite::create(); //Style 3 for shape tile (bottom half)
	cocos2d::Sprite* style5Bottom = cocos2d::Sprite::create(); //Style 5 for shape tile (bottom half)
	cocos2d::Sprite* highlight = cocos2d::Sprite::create(); //Highlight bar
	cocos2d::Sprite* outlineTop = cocos2d::Sprite::create(); //Top outline of shape
	cocos2d::Sprite* outlineBottom = cocos2d::Sprite::create(); //Bottom outline of shape
	cocos2d::Size size;

	enum SHAPE { CIRCLE, SQUARE, DIAMOND, TRIANGLE, HEXAGON, OCTAGON };
	enum OUTLINE { SOLID, OUTLINE, BLINDS, CONCENTRIC, QUAD, OUTLINE_BLINDS, OUTLINE_QUAD, CONCENTRIC_BLINDS, CONCENTRIC_QUAD, BLINDS_QUAD, OUTLINE_BLINDS_QUAD, CONCENTRIC_BLINDS_QUAD };

	int shape = 0;
	int alphaMod = 0;
	int rotation = 0; //Rotation angle for shapes
	std::pair<int, int> style; //Pair for top and bottom outline styles
	std::pair<cocos2d::Color3B, cocos2d::Color3B> color; //Pair for top and bottom colors
	std::pair<cocos2d::Color3B, cocos2d::Color3B> outlineColor; //Pair for top and bottom outline colors
	int tilePosition; //Position of tile on base tile (0 = TOP LEFT, 1 = TOP CENTER, 2 = TOP RIGHT, 3 = MIDDLE LEFT, 4 = MIDDLE CENTER, 5 = MIDDLE RIGHT, 6 = BOTTOM LEFT, 7 = BOTTOM CENTER, 8 = BOTTOM RIGHT)
	enum TILE { TOP_LEFT, TOP_CENTER, TOP_RIGHT, MIDDLE_LEFT, MIDDLE_CENTER, MIDDLE_RIGHT, BOTTOM_LEFT, BOTTOM_CENTER, BOTTOM_RIGHT };

	bool wasTouched = false;
};

