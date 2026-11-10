#pragma once
//リスの巣がある木。見た目と当たり判定を持つ。
class Nest : public IGameObject
{
public:
	//モデルと当たり判定を準備する。
	bool Start();
	//更新処理。
	void Update();
	//描画処理。
	void Render(RenderContext& rc);
	//木を置く位置を決める。Startより前に呼ぶ。
	void SetPosition(const Vector3& position);
	//ダメージを受ける
	void Damage(int damage);
	//壊れているか
	bool IsBroken()const;
	const Vector3& GetPosition() const;

	Vector3 m_position;				//座標。
	
private:
	ModelRender m_modelRender;		//見た目のモデル。
	ModelRender m_brokenRender;		//壊れた木のモデル。
	ModelRender m_colliderRender;	//当たり判定の形を読むためのモデル。描画はしない。
	PhysicsStaticObject m_physics;	//当たり判定。
	Quaternion m_rotation;			//回転。
	int m_hp = 0;					//巣の体力
};

