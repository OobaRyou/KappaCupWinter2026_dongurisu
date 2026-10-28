#pragma once
class Bullet:public IGameObject
{
public:
	bool Start();
	void Update();
	void Render(RenderContext& rc);
	//発射位置の設定
	void SetPosition(const Vector3& position) { m_position = position; }
	//速度を設定するNewGO前に呼ぶ
	void SetVelocity(const Vector3& velocity) { m_velocity = velocity; }
private:
	ModelRender m_modelRender;
	Vector3 m_position;
	Vector3 m_velocity;
	float m_timer = 0.0f;
};

