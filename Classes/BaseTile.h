#pragma once
#include "pch.h"
#include "nrgSprite.h"

class Tile;

class BaseTile : public nrgSprite
{
public:
	
	//Return basetile object
	static BaseTile* create(cocos2d::SpriteBatchNode* batch);

	//Return top left tile object
	Tile* GetTopLeftTile(); 

	//Return top center tile object
	Tile* GetTopCenterTile();

	//Return top right tile object
	Tile* GetTopRightTile();

	//Return middle left tile object
	Tile* GetMiddleLeftTile();

	//Return middle center tile object
	Tile* GetMiddleCenterTile();

	//Return middle right tile object
	Tile* GetMiddleRightTile();

	//Return bottom left tile object
	Tile* GetBottomLeftTile();

	//Return bottom center tile object
	Tile* GetBottomCenterTile();

	//Return bottom right tile object
	Tile* GetBottomRightTile();

	~BaseTile();

protected:
	BaseTile(cocos2d::SpriteBatchNode* batch);

	Tile* topLeftTile = NULL; //Top left tile
	Tile* topCenterTile = NULL; //Top center tile
	Tile* topRightTile = NULL; //Top right tile
	Tile* middleLeftTile = NULL; //Middle Left tile
	Tile* middleCenterTile = NULL; //Middle center tile
	Tile* middleRightTile = NULL; //Middle right tile
	Tile* bottomLeftTile = NULL; //Bottom left tile
	Tile* bottomCenterTile = NULL; //Bottom center tile
	Tile* bottomRightTile = NULL; //Bottom right tile
	enum TILE { TOP_LEFT, TOP_CENTER, TOP_RIGHT, MIDDLE_LEFT, MIDDLE_CENTER, MIDDLE_RIGHT, BOTTOM_LEFT, BOTTOM_CENTER, BOTTOM_RIGHT };
};

