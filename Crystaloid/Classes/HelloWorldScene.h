/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 
 http://www.cocos2d-x.org
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include <iostream>
#include <math.h>
#include <algorithm>

using namespace std;
using namespace cocos2d;
using namespace ui;

enum PSCENE {START, GAME, OVER};

struct Crystal
{
	Sprite *sprite;	
	int type;
};

struct Electron
{
	Sprite *sprite;
	float velX;
	float velY;
};

struct Player
{
	Sprite *sprite;
	int life;
	int score;
};

struct Bonus
{
	Sprite *sprite;
	int myType;
	string fn[3] = { "bonus_1.png", "bonus_2.png", "bonus_3.png" };
	float velX = 0;
	float velY = -10;
};

class HelloWorld : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();
	virtual void update(float dt);
	bool onTouchStart(Touch *touch, Event *event);
	bool onTouchMove(Touch *touch, Event *event);
	bool onTouchEnd(Touch *touch, Event *event);
	void updateCrystal(Sprite *sprite, int num);
	void showWin();
    void createBorder();
	void createLevel();
	void gameOver();
	void startMenu();
	void continueGame();
	void resetVelocity(float dt);
	void startedPosition();
	void startGame();
	void clearLevel();

	template <class T1, class T2>
	bool checkPaddleCollision(T1 &_A, T2 _B);

	// variables
	vector<Crystal> crystals;
	vector<Bonus> bonuses;
	Player player;
	Label *score;
	Label *life;
	Electron electron;
	Size visibleSize;
	float touchPosX;
	string maps[10];
	float oY;
	float velocity;
	bool isStarted;
	Size borderSize;
	int pseudoScene;

    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);
};

#endif // __HELLOWORLD_SCENE_H__
