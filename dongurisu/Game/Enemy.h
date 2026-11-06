#pragma once

/**
 * 敵の基底クラス。
 * ヘビ・鳥・クマなど、どの敵にも共通する処理(HP、状態、アニメーション、描画)をまとめる。
 * 敵ごとに違う部分は純粋仮想関数にして、子クラスで中身を書く。
 */
class Enemy : public IGameObject
{
public:
	/** 敵の状態。アニメーションクリップの番号としても使う。 */
	enum class EnState : int
	{
		Idle,	//待機。
		Move,	//移動。
		Attack,	//攻撃。
		Damage,	//被弾。
		Dead,	//撃破。
		Num,	//状態の数。
	};

	bool Start() override;
	void Update() override;
	void Render(RenderContext& rc) override;

	//
	// ダメージを受ける。
	// damage 受けるダメージ量。
	//  return このダメージで倒れたら true。
	bool Damage(int damage);
	// 倒れているか。 
	bool IsDead() const;
	// 座標を設定する。 
	void SetPosition(const Vector3& position);
	//座標を取得する。 
	const Vector3& GetPosition() const;

protected:
	// モデル(tkm)のファイルパスを返す。 
	virtual const char* GetModelFilePath() = 0;
	// 状態ごとのアニメーション(tka)のファイルパスを返す。 
	virtual const char* GetAnimationFilePath(EnState state) = 0;
	//移動処理。m_position と m_rotation を動かす。 
	virtual void Move() = 0;
	// 攻撃が当たる瞬間の処理。攻撃アニメーションの「Hit」イベントで呼ばれる。
	virtual void Attack() = 0;
	// 倒れた瞬間の処理。 
	virtual void OnDead() = 0;

	ModelRender m_modelRender;										//モデルレンダー。
	AnimationClip m_animationClips[static_cast<int>(EnState::Num)];	//アニメーションクリップ。状態の番号で使う。
	Vector3 m_position;												//座標。
	Quaternion m_rotation;											//回転。
	float m_scale = 1.0f;											//大きさ。子クラスのコンストラクタで変える。
	int m_hp = 1;													//体力。
	float m_attackInterval = 2.0f;									//攻撃の間隔(秒)。
	float m_attackTimer = 0.0f;										//前の攻撃からの時間(秒)。
	EnState m_state = EnState::Idle;								//今の状態。

private:
	// 待機・移動中の処理。移動して、時間が来たら攻撃を始める。 
	void UpdateIdleAndMove();
	// 今の状態のアニメーションを再生する。 
	void PlayAnimation();
	// アニメーションイベントが起きたときの処理。 
	void OnAnimationEvent(const wchar_t* eventName);
};
