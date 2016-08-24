#pragma once
#include "BaseTile.h"

BaseTile::BaseTile(cocos2d::SpriteBatchNode* batch)
{
	topLeftTile = Tile::create(batch);
	topCenterTile = Tile::create(batch);
	topRightTile = Tile::create(batch);
	middleLeftTile = Tile::create(batch);
	middleCenterTile = Tile::create(batch);
	middleRightTile = Tile::create(batch);
	bottomLeftTile = Tile::create(batch);
	bottomCenterTile = Tile::create(batch);
	bottomRightTile = Tile::create(batch);

	topLeftTile->setLocalZOrder(1);
	
	addChild(topLeftTile);
	addChild(topCenterTile);
	addChild(topRightTile);
	addChild(middleLeftTile);
	addChild(middleCenterTile);
	addChild(middleRightTile);
	addChild(bottomLeftTile);
	addChild(bottomCenterTile);
	addChild(bottomRightTile);

	topLeftTile->SetTilePosition(TOP_LEFT);
	topCenterTile->SetTilePosition(TOP_CENTER);
	topRightTile->SetTilePosition(TOP_RIGHT);
	middleLeftTile->SetTilePosition(MIDDLE_LEFT);
	middleCenterTile->SetTilePosition(MIDDLE_CENTER);
	middleRightTile->SetTilePosition(MIDDLE_RIGHT);
	bottomLeftTile->SetTilePosition(BOTTOM_LEFT);
	bottomCenterTile->SetTilePosition(BOTTOM_CENTER);
	bottomRightTile->SetTilePosition(BOTTOM_RIGHT);

	return;
}


BaseTile * BaseTile::create(cocos2d::SpriteBatchNode * batch)
{
	BaseTile* sprite = new BaseTile(batch);
	
	if (sprite->initWithSpriteFrameName("base_tile.png"))
	{
		sprite->autorelease();
		batch->addChild(sprite);
		sprite->getTexture()->setAliasTexParameters();
		sprite->setAnchorPoint(cocos2d::Vec2(0.5, 0.5));
		return sprite;
	}
	CC_SAFE_DELETE(sprite);
	return NULL;
}

Tile * BaseTile::GetTopLeftTile()
{
	return topLeftTile;
}

Tile * BaseTile::GetTopCenterTile()
{
	return topCenterTile;
}

Tile * BaseTile::GetTopRightTile()
{
	return topRightTile;
}

Tile * BaseTile::GetMiddleLeftTile()
{
	return middleLeftTile;
}

Tile * BaseTile::GetMiddleCenterTile()
{
	return middleCenterTile;
}

Tile * BaseTile::GetMiddleRightTile()
{
	return middleRightTile;
}

Tile * BaseTile::GetBottomLeftTile()
{
	return bottomLeftTile;
}

Tile * BaseTile::GetBottomCenterTile()
{
	return bottomCenterTile;
}

Tile * BaseTile::GetBottomRightTile()
{
	return bottomRightTile;
}

BaseTile::~BaseTile()
{
}
