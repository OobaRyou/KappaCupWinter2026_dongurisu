#include "stdafx.h"
#include "Game.h"
#include "Player.h"

bool Game::Start()
{	//プレイヤーのオブジェクトを作る。
	player = NewGO<Player>(0, "player");

	m_modelRender.Init("Assets/modelData/Squirrel.tkm");
	
	return true;
}

void Game::Update()
{
	// g_renderingEngine->DisableRaytracing();
	m_modelRender.Update();
}

void Game::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
}