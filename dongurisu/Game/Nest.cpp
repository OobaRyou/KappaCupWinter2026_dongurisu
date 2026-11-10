#include "stdafx.h"
#include "Nest.h"
namespace
{
	const int NEST_MAX_HP = 100;
}
bool Nest::Start()
{
	//穴は木の+Z側にある。リスの方へ向けるため、180度回す。
	m_rotation.SetRotationDeg(Vector3::Up, 180.0f);
	m_modelRender.Init("Assets/modelData/nestTree.tkm", nullptr, 0, enModelUpAxisY);
	m_brokenRender.Init("Assets/modelData/nestTree_broken.tkm", nullptr, 0, enModelUpAxisY);
	m_modelRender.SetPosition(m_position);
	m_modelRender.SetRotation(m_rotation);
	m_modelRender.Update();
	m_brokenRender.SetPosition(m_position);
	m_brokenRender.SetRotation(m_rotation);
	m_brokenRender.Update();
	//当たり判定の形を読む。描画はしないが、見た目と同じ位置と向きにする。
	m_colliderRender.Init("Assets/modelData/nestTree_collider.tkm", nullptr, 0, enModelUpAxisY);
	m_colliderRender.SetPosition(m_position);
	m_colliderRender.SetRotation(m_rotation);
	m_colliderRender.Update();
	//読んだ形から当たり判定を作る。
	m_physics.CreateFromModel(m_colliderRender.GetModel(), m_colliderRender.GetModel().GetWorldMatrix());
	m_hp = NEST_MAX_HP;
	return true;
}
void Nest::Update()
{
	m_modelRender.Update();
	m_brokenRender.Update();
}
void Nest::Render(RenderContext& rc)
{
	if (!IsBroken())
	{
		m_modelRender.Draw(rc);
	}
	else
	{
		m_brokenRender.Draw(rc);
	}
}
void Nest::SetPosition(const Vector3& position)
{
	m_position = position;
}
void Nest::Damage(int damage)
{
	if (IsBroken())
	{
		
		return;
	}
	
	m_hp -= damage;
	if (m_hp < 0)
	{
		m_hp = 0;
	}
}
bool Nest::IsBroken()const
{
	return m_hp <= 0;
}
const Vector3& Nest::GetPosition() const
{
	return m_position;
}
