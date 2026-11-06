#pragma once
class DamageNumber:public IGameObject
{
public:
	void Init(int damage, const Vector3& position);
	void Update();
	void Render(RenderContext& rc);
private:
	FontRender m_fontRender;
	Vector3 m_position;
	float m_timer=0.0f;


};

