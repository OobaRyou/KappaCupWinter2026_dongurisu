#pragma once
#include"InputHelper.h"
#include"Bullet.h"
class Player:public IGameObject
{
public:
	Player();
	~Player();
	//更新処理。
	void Update();
	//描画処理。
	void Render(RenderContext& rc);
	//移動処理。
	void Move();
	//ジャンプの処理
	void Jump();
	//回転処理。
	void Rotation();
	//ステート管理。
	void ManageState();
	//アニメーションの再生。
	void PlayAnimation();
	//弾発射
	void Fire();
	//メンバ変数。
	ModelRender modelRender;	//モデルレンダ―。
	Vector3 position;			//座標。
	enum EnAnimationClip {		//アニメーション。
		enAnimationClip_idle,
		enAnimationClip_idle2,
		enAnimationClip_run,
		enAnimationClip_dying,
		enAnimationClip_walk,
		enAnimationClip_jump,
		enAnimationClip_Num,
	};
	AnimationClip animationClips[enAnimationClip_Num];		//アニメーションクリップ。
	//確認用：再生するアニメーションを固定する番号。-1なら自動(移動中はrun、止まっているとidle)。
	int previewAnimation = -1;
	FontRender fontRender;		//確認用の文字表示。
	CharacterController characterController;  //キャラクターコントローラー。
	Vector3 moveSpeed;		//移動速度。
	Quaternion rotation;  //クォータニオン。
	InputHelper m_input;	//操作の判定(パッドとキーボードの違いをまとめる)。
	float m_fireTimer = 0.0f;	//次に撃てるまでの時間
	int m_ammo = 0;			//今の弾薬数
};

