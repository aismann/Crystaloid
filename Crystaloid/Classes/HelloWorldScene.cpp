#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"

#define LINECOUNT 10

USING_NS_CC;

float minimum(float x, float y)
{
	return x > y ? y : x;
}

Scene* HelloWorld::createScene()
{
    return HelloWorld::create();
}


bool HelloWorld::init()
{
    if ( !Scene::init() )
        return false;

    visibleSize = Director::getInstance()->getVisibleSize();
	
	maps[0] = "0000000000000000000011001100111100110011110011001111001100111133113311000000000000000000000022202220";
	maps[1] = "00000000000000000000003020103003020102030000000000010203010000000000001111333111";
	maps[2] = "000000000000000000002220022200222002220022222222000222222000023232300002333330000233323000033333300";
	maps[3] = "000000000000002220000022111220023321233202333233320231333132023333333200023132000000222";
	maps[4] = "0000000000000000000000000000002222222222223322332222332233222211111122211111111222111111221111111111111111111101111111102211221122";
	maps[5] = "0000000000200000000222001100222220110222222200222222222222223333333333";
	maps[6] = "000000000000000000000000000000121212121212121212121212121212121212121212121212121212120300330030";
	maps[7] = "00000000000000000000000000000032144123001111110032144123222222222200330033001111111111";
	maps[8] = "000000000000000000000000000000001100110011001100110022002200333333333300220022000011001100";
	maps[9] = "00000000000000000000000000000000002220000022111220023321233202333233320231333132023333333200023132000000222";
	
	velocity = 8;

	isStarted = false;

	// loading			
	CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("collision.mp3");
	CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("explode.mp3");
	CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("gameover.mp3");
	CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("pickup.mp3");
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("resources.plist");
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("resources-2.plist");
			
	auto bg = Sprite::createWithSpriteFrameName("bg.png");
	bg->setAnchorPoint(Vec2(0, 0));
	bg->setPosition(Vec2(0, 0));
	this->addChild(bg, 0);

	// Score Label
	score = Label::createWithBMFont("font.fnt", "", TextHAlignment::LEFT);
	score->setAnchorPoint(Vec2(0.0f, 1.0f));
	score->setPosition(visibleSize.width * 0.08f, visibleSize.height * 0.98f);
	this->addChild(score, 5);

	// Life Label
	life = Label::createWithBMFont("font.fnt", "", TextHAlignment::RIGHT);
	life->setAnchorPoint(Vec2(1.0f, 1.0f));
	life->setPosition(visibleSize.width * 0.92f, visibleSize.height * 0.98f);
	this->addChild(life, 5);

	oY = visibleSize.height * 0.1f;

	createBorder();

	createLevel();

	// player
	player.sprite = Sprite::createWithSpriteFrameName("platform.png");
	player.sprite->setAnchorPoint(Vec2(0, 1));
	player.sprite->setPosition(visibleSize.width / 2, visibleSize.height * 0.15f);
	this->addChild(player.sprite, 3);
	player.life = 3;
	player.score = 0;

	// electron
	electron.sprite = Sprite::createWithSpriteFrameName("e.png");
	electron.sprite->setAnchorPoint(Vec2(0.5, 0.5));
	electron.sprite->setPosition(player.sprite->getPosition() + Vec2(player.sprite->getContentSize().width/2, player.sprite->getContentSize().height*0.2f));
	this->addChild(electron.sprite, 2);
	
	electron.velX = velocity;
	electron.velY = velocity;

	this->scheduleUpdate();

	// listener
	auto listenerT = EventListenerTouchOneByOne::create();
	listenerT->setSwallowTouches(true);
	listenerT->onTouchBegan = CC_CALLBACK_2(HelloWorld::onTouchStart, this);
	listenerT->onTouchMoved = CC_CALLBACK_2(HelloWorld::onTouchMove, this);
	listenerT->onTouchEnded = CC_CALLBACK_2(HelloWorld::onTouchEnd, this);
	this->_eventDispatcher->addEventListenerWithSceneGraphPriority(listenerT, this);
	
	startMenu();
			   	 
    return true;
}


bool HelloWorld::onTouchStart(Touch *touch, Event *event)
{
	if (player.life > 0 && pseudoScene == GAME)
	{
		if (touch->getLocation().x - player.sprite->getContentSize().width / 2 >= 0 && touch->getLocation().x + player.sprite->getContentSize().width / 2 <= visibleSize.width)
			player.sprite->setPosition(touch->getLocation().x - player.sprite->getContentSize().width / 2, player.sprite->getPosition().y);

		if (!isStarted)
			electron.sprite->setPosition(player.sprite->getPosition() + Vec2(player.sprite->getContentSize().width / 2, player.sprite->getContentSize().height*0.2f));

		!isStarted ? isStarted = true : isStarted = true;
	}

	return true;
}


bool HelloWorld::onTouchMove(Touch *touch, Event *event)
{
	if (player.life > 0 && pseudoScene == GAME)
	{
		if (touch->getLocation().x - player.sprite->getContentSize().width / 2 >= 0 && touch->getLocation().x + player.sprite->getContentSize().width / 2 <= visibleSize.width)
			player.sprite->setPosition(touch->getLocation().x - player.sprite->getContentSize().width / 2, player.sprite->getPosition().y);

		if (!isStarted)
			electron.sprite->setPosition(player.sprite->getPosition() + Vec2(player.sprite->getContentSize().width / 2, player.sprite->getContentSize().height*0.2f));
	}

	return true;
}


bool HelloWorld::onTouchEnd(Touch *touch, Event *event)
{
	return true;
}


void HelloWorld::updateCrystal(Sprite *sprite, int num)
{
	string name = "b";
	name += to_string(num);
	name.append(".png");	
	sprite->setSpriteFrame(SpriteFrameCache::getInstance()->spriteFrameByName(name));
}


void HelloWorld::update(float dt)
{
	if (player.life > 0 && isStarted)
	{
		electron.sprite->setPosition(electron.sprite->getPosition() + Vec2(electron.velX, electron.velY));

		if (bonuses.size() > 0)
		{
			for (auto &bonus : bonuses)
			{
				bonus.sprite->setPosition(bonus.sprite->getPosition() + Vec2(bonus.velX, bonus.velY));
				
				if (checkPaddleCollision(bonus, player))
				{
					switch (bonus.myType)
					{
					case 1:
						player.life++;
						life->setString("LIFE: " + to_string(player.life));
						break;
					case 2:
						electron.velX /= 2;
						electron.velY /= 2;
						velocity /= 2;
						this->schedule(schedule_selector(HelloWorld::resetVelocity), 25);
						CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("pickup.mp3");
						break;
					case 3:
						electron.velX *= 2;
						electron.velY *= 2;
						velocity *= 2;
						this->schedule(schedule_selector(HelloWorld::resetVelocity), 25);
						CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("pickup.mp3");
						break;
					}

					this->removeChild(bonus.sprite, true);
					bonus.myType = 0;
				}
				else {
					if (bonus.velY + bonus.sprite->getPosition().y + bonus.sprite->getContentSize().height <= 0)
					{
						this->removeChild(bonus.sprite, true);
						bonus.myType = 0;
					}
				}
			}
		}

		auto  &le = electron;

		if (!bonuses.empty())
			bonuses.erase(std::remove_if(bonuses.begin(), bonuses.end(), [](const Bonus &itr) { return itr.myType == 0;	}), std::end(bonuses));

		if (electron.velY + electron.sprite->getPosition().y + electron.sprite->getContentSize().height / 2 >= visibleSize.height - borderSize.height / 2 - oY)
			electron.velY *= -1;

		if (electron.velY + electron.sprite->getPosition().y + electron.sprite->getContentSize().height / 2 <= 0)
			continueGame();

		if (electron.velX + electron.sprite->getPosition().x + electron.sprite->getContentSize().width / 2 >= visibleSize.width - borderSize.height / 2 ||
			electron.velX + electron.sprite->getPosition().x - electron.sprite->getContentSize().width / 2 <= borderSize.height / 2)
			electron.velX *= -1;

		for (auto &crystal : crystals)
		{		
			if (checkPaddleCollision(le, crystal))
			{
				crystal.type -= 1;

				if (crystal.type > 0)
				{
					updateCrystal(crystal.sprite, crystal.type);
					CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("collision.mp3");
				}
				else {
					auto is_bonus = rand() % 2;
					is_bonus ? is_bonus = rand() % 2 : is_bonus = 0;
					if (is_bonus)
					{
						auto type_bonus = rand() % 3;
						Bonus bonus;
						bonus.sprite = Sprite::createWithSpriteFrameName(bonus.fn[type_bonus]);
						bonus.sprite->setPosition(crystal.sprite->getPosition());
						this->addChild(bonus.sprite);
						bonus.myType = type_bonus + 1;
						bonuses.push_back(bonus);
					}

					this->removeChild(crystal.sprite, true);
					crystal.type = 0;
					player.score += 1;
					score->setString("SCORE: " + to_string(player.score));			

					CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("explode.mp3");
				}
			}
		}
		
		if (checkPaddleCollision(le, player))
		{

		}
		
		if (!crystals.empty())
			crystals.erase(std::remove_if(crystals.begin(), crystals.end(), [](const Crystal &itr) { return itr.type == 0; }), std::end(crystals));
		else
			showWin();
	}
}

template <class T1, class T2>
bool HelloWorld::checkPaddleCollision(T1 &_A, T2 _B)
{
	auto leftN = _A.velX + _A.sprite->getPosition().x + _A.sprite->getContentSize().width / 2 - _B.sprite->getPosition().x;
	auto rightN = _B.sprite->getPosition().x + _B.sprite->getContentSize().width - (_A.velX + _A.sprite->getPosition().x - _A.sprite->getContentSize().width / 2);
	auto topN = _B.sprite->getPosition().y - (_A.velY + _A.sprite->getPosition().y - _A.sprite->getContentSize().height / 2);
	auto bottomN = _A.velY + _A.sprite->getPosition().y + _A.sprite->getContentSize().height / 2 - (_B.sprite->getPosition().y - _B.sprite->getContentSize().height);

	auto minX = minimum(leftN, rightN);
	auto minY = minimum(topN, bottomN);

	if (minX >= 0 && minY >= 0)
	{		
		_A.velX = velocity * (leftN - rightN) / (_B.sprite->getContentSize().width + _A.sprite->getContentSize().width);
		_A.velY = velocity * (bottomN - topN) / (_B.sprite->getContentSize().height + _A.sprite->getContentSize().height);
		
		return true;
	}

	return false;
}


void HelloWorld::startedPosition()
{
	isStarted = false;
	player.sprite->setPosition(visibleSize.width / 2 - player.sprite->getContentSize().width / 2, player.sprite->getPosition().y);
	electron.sprite->setPosition(player.sprite->getPosition() + Vec2(player.sprite->getContentSize().width / 2, player.sprite->getContentSize().height*0.2f));
	velocity = 8;
}

void HelloWorld::showWin()
{
	startedPosition();
	createLevel();
}


void HelloWorld::resetVelocity(float dt)
{
	this->unschedule(schedule_selector(HelloWorld::resetVelocity));
	velocity = 8;
}

void HelloWorld::createBorder()
{
	auto bh = 0.0f;

	for (auto a = 0; a < 46; a++)
	{
		auto sprite = Sprite::createWithSpriteFrameName("border.png");
		sprite->setAnchorPoint(Vec2(0, 1));
		sprite->setRotation(90);
		sprite->setPosition(a * sprite->getContentSize().height, visibleSize.height + sprite->getContentSize().height - oY);
		this->addChild(sprite, 8);
		borderSize = Size(sprite->getContentSize().width, sprite->getContentSize().height);
	}

	for (auto x = 0; x < 2; x++)
	{
		for (auto a = 0; a < 80; a++)
		{
			auto sprite = Sprite::createWithSpriteFrameName("border.png");
			bh = sprite->getContentSize().height;
			sprite->setPosition(x * visibleSize.width, visibleSize.height - a * bh - oY);
			this->addChild(sprite, 8);			
		}
	}

	for (auto a = 0; a < 8; a++)
	{
		auto sprite = Sprite::createWithSpriteFrameName("angle.png");
		sprite->setAnchorPoint(Vec2(0.5, 0.5));
		sprite->setPosition((a % 2) * visibleSize.width, visibleSize.height - oY - (a / 2)*20*bh);
		this->addChild(sprite, 9);
	}
}


void HelloWorld::createLevel()
{
	auto startPos = Vec2(borderSize.width * 0.6f, visibleSize.height - borderSize.width *0.62f - oY);

	int mc = rand() % (sizeof(maps) / sizeof(*maps)); 

	for (unsigned int i = 0; i < strlen(maps[mc].c_str()); i++)
	{
		if (maps[mc][i] != '0')
		{
			Crystal crystal;
			crystal.type = maps[mc][i] - '0';
			crystal.sprite = Sprite::create();

			updateCrystal(crystal.sprite, crystal.type);

			crystal.sprite->setAnchorPoint(Vec2(0, 1));
			crystal.sprite->setPosition(startPos + Vec2(crystal.sprite->getContentSize().width * 1.15f * (i - (i / LINECOUNT)*LINECOUNT), -1 * crystal.sprite->getContentSize().height * 1.2f * (i / LINECOUNT)));
			this->addChild(crystal.sprite);

			crystals.push_back(crystal);
		}
	}
}


void HelloWorld::gameOver()
{
	pseudoScene = OVER;
	auto *gameOverLayer = Layer::create();
	gameOverLayer->setTag(OVER);

	score->setString("");
	life->setString("");

	auto fadeBGNode = DrawNode::create();
	Vec2 fadeBG[] = { Vec2(0, 0), Vec2(0, visibleSize.height), Vec2(visibleSize.width, visibleSize.height), Vec2(visibleSize.width, 0) };
	Color4F aBlack(0.0f, 0.0f, 0.0f, 0.7f);
	fadeBGNode->drawPolygon(fadeBG, 4, aBlack, 0, aBlack);
	gameOverLayer->addChild(fadeBGNode);

	auto gLabel = Label::createWithBMFont("font.fnt", "GAME_OVER");
	gLabel->setPosition(visibleSize.width / 2, visibleSize.height * 0.96f);
	gameOverLayer->addChild(gLabel);

	auto ystLabel = Label::createWithBMFont("font.fnt", "YOUR_SCORE");
	ystLabel->setPosition(visibleSize.width / 2, visibleSize.height * 0.8f);
	gameOverLayer->addChild(ystLabel);

	string score_string = "_";
	score_string.append(to_string(player.score));
	score_string.append("_");
	auto ysLabel = Label::createWithBMFont("font.fnt", score_string);
	ysLabel->setPosition(visibleSize.width / 2, visibleSize.height * 0.72f);
	ysLabel->setScale(1.2f);
	gameOverLayer->addChild(ysLabel);
	
	auto button = Button::create("start.png", "start.png", "start.png", Widget::TextureResType::PLIST);
	button->setPosition(Vec2(visibleSize.width / 2, visibleSize.height * 0.5f));

	button->addTouchEventListener([&](Ref* sender, Widget::TouchEventType type) {
		switch (type)
		{
		case ui::Widget::TouchEventType::BEGAN:
			break;
		case ui::Widget::TouchEventType::ENDED:
			pseudoScene = GAME;
			startGame();
			clearLevel();
			createLevel();
			this->removeChildByTag(OVER, true);
			break;
		}
	});

	gameOverLayer->addChild(button);

	auto buttonQuit = Button::create("quit.png", "quit.png", "quit.png", Widget::TextureResType::PLIST);
	buttonQuit->setPosition(Vec2(visibleSize.width / 2, visibleSize.height * 0.3f));

	buttonQuit->addTouchEventListener([&](Ref* sender, Widget::TouchEventType type) {
		switch (type)
		{
		case ui::Widget::TouchEventType::BEGAN:
			break;
		case ui::Widget::TouchEventType::ENDED:
			Director::getInstance()->end();
			break;
		}
	});

	gameOverLayer->addChild(buttonQuit);

	this->addChild(gameOverLayer, 70);

	CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("gameover.mp3");
}

void HelloWorld::continueGame()
{	
	startedPosition();
	player.life--;	

	if (player.life > 0)
		life->setString("LIFE: " + to_string(player.life));
	else
		gameOver();
}

void HelloWorld::clearLevel()
{
	for (auto &crystal : crystals)
	{
		this->removeChild(crystal.sprite, true);
	}

	for (auto &bonus : bonuses)
	{
		this->removeChild(bonus.sprite, true);
	}

	crystals.clear();
	bonuses.clear();
}


void HelloWorld::startMenu()
{
	pseudoScene = START;
	auto startMenuLayer = Layer::create();
	startMenuLayer->setTag(START);

	auto fadeBGNode = DrawNode::create();
	Vec2 fadeBG[] = { Vec2(0, 0), Vec2(0, visibleSize.height), Vec2(visibleSize.width, visibleSize.height), Vec2(visibleSize.width, 0) };
	Color4F aBlack(0.0f, 0.0f, 0.0f, 0.7f);
	fadeBGNode->drawPolygon(fadeBG, 4, aBlack, 0, aBlack);
	startMenuLayer->addChild(fadeBGNode);

	auto ystLabel = Label::createWithBMFont("font.fnt", "CRYSTALOID");
	ystLabel->setPosition(visibleSize.width / 2, visibleSize.height * 0.96f);
	startMenuLayer->addChild(ystLabel);

	auto button = Button::create("start.png", "start.png", "start.png", Widget::TextureResType::PLIST);
	button->setPosition(Vec2(visibleSize.width / 2, visibleSize.height * 0.5f));

	button->addTouchEventListener([&](Ref* sender, Widget::TouchEventType type) {
		switch (type)
		{
		case ui::Widget::TouchEventType::BEGAN:
			break;
		case ui::Widget::TouchEventType::ENDED:
			pseudoScene = GAME;
			startGame();
			this->removeChildByTag(START, true);
			break;
		}
	});

	startMenuLayer->addChild(button);

	auto buttonQuit = Button::create("quit.png", "quit.png", "quit.png", Widget::TextureResType::PLIST);
	buttonQuit->setPosition(Vec2(visibleSize.width / 2, visibleSize.height * 0.3f));

	buttonQuit->addTouchEventListener([&](Ref* sender, Widget::TouchEventType type) {
		switch (type)
		{
		case ui::Widget::TouchEventType::BEGAN:
			break;
		case ui::Widget::TouchEventType::ENDED:
			Director::getInstance()->end();
			break;
		}
	});

	startMenuLayer->addChild(buttonQuit);

	this->addChild(startMenuLayer, 70);
}


void HelloWorld::startGame()
{
	score->setString("SCORE:0");
	life->setString("LIFE:3");
	player.score = 0;
	player.life = 3;
	startedPosition();
}