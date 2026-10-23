#pragma once

#include "Level3DRender/LevelRender.h"

class Player;

class Game : public IGameObject
{
public:
	Game() {}
	~Game() {}
	bool Start();
	void Update();
	void Render(RenderContext& rc);
	Player* player;
private:
	ModelRender m_modelRender;
	Vector3 m_pos;
	ModelRender m_groundRender;				//仮の地面の表示。本物のステージができたら置き換える。
	PhysicsStaticObject m_groundPhysics;	//仮の地面の当たり判定。
};

