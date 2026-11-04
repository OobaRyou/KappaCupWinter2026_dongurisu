#pragma once
#include "camera/CameraCollisionSolver.h"
class Player;
//ゲーム中のカメラを制御する。
class GameCamera : public IGameObject
{
public:
	GameCamera();
	~GameCamera();
	bool Start();
	void Update();
	/////////////////////////////////////
	//メンバ変数
	/////////////////////////////////////
	Player* m_player;	//プレイヤー。
	Vector3 m_toCameraPos;	//注視点から視点に向かうベクトル。
	CameraCollisionSolver m_cameraCollisionSolver; //カメラが地形にめり込まないようにする。
};
