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
	/** 照準（画面のまん中）が指している点を探す。敵・木・地面のうち一番近いもの。 */
	Vector3 FindAimPosition(const Vector3& cameraPosition, const Vector3& cameraForward, float minDistance);
	//ダメージ
	void Damage(int damage);
	//弾薬を増やす
	void AddAmmo(int amount);
	//弾薬を満タンにする
	void RefillAmmo();
	//死亡判定
	bool IsDead()const;
	//forward の向きに壁があるかを調べる。あったら wallNormal に壁の向きを入れて true を返す。
	bool FindWall(const Vector3& forward, Vector3& wallNormal);
	//登り中の移動。
	void Climb();
	//壁に近づきすぎていたら外へ押し出す。
	void PushOutFromWall();
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
		enAnimationClip_climbStop,	//登っている途中で止まったときの姿勢。
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
	float m_aimTimer = 0.0f;	//撃ったあと、照準の方を向いていられる残り時間（秒）。
	int m_ammo = 0;			//今の弾薬数
	int m_hp = 0;			//体力
	float m_invincibleTimer = 0.0f;		//被弾後の無敵時間
	int m_blinkCount = 0;	//点滅用のフレーム数
	bool m_isClimbing = false;	//壁に張りついているか。
	Vector3 m_wallNormal;	//張りついている壁の向き(壁からリスの方へ向かう向き)。
	float m_wallDistance = 0.0f;	//壁までの距離。
	float m_climbHeadAngle = 0.0f;	//壁の上での頭の向き(0度で上、90度で右、180度で下)。
};

