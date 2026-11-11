#include "stdafx.h"
#include "Game.h"
#include "Player.h"
#include "GameCamera.h"
#include "Nest.h"
#include"WaveManager.h"

bool Game::Start()
{
	//レイトレーシング(映り込みの計算)は重いのでオフにする。
	g_renderingEngine->DisableRaytracing();
	//プレイヤーのオブジェクトを作る。
	player = NewGO<Player>(0, "player");
	//敵の出現を管理するオブジェクトを作る。
	NewGO<WaveManager>(0, "waveManager");
	
	//ゲームカメラのオブジェクトを作る。
	gameCamera = NewGO<GameCamera>(0, "gamecamera");


	m_modelRender.Init("Assets/modelData/Squirrel.tkm");

	//仮の地面を作る。上面が y = 0 の平らな板で、見た目と同じ形の当たり判定を付ける。
	m_groundRender.Init("Assets/modelData/tempGround.tkm", nullptr, 0, enModelUpAxisY);
	m_groundRender.Update();
	m_groundPhysics.CreateFromModel(m_groundRender.GetModel(), m_groundRender.GetModel().GetWorldMatrix());

	//巣の木を作る。幹が太いので、リスと重ならないように離して置く。
	Nest* nest = NewGO<Nest>(0, "nest");
	nest->SetPosition(Vector3(0.0f, 0.0f, 400.0f));

	return true;
}

void Game::Update()
{
	m_groundRender.Update();
}

void Game::Render(RenderContext& rc)
{
	
	m_groundRender.Draw(rc);
}
