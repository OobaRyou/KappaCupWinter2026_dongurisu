#include "stdafx.h"
#include "Bullet.h"
namespace
{
	//球が消えるまでの時間
	const float LIFE_TIME = 2.0f;
	//1フレームの時間
	const float DELTA_TIME = 1.0f / 60.0f;
}
bool Bullet::Start()
{
	m_modelRender.Init("Assets/modelData/acorn.tkm", nullptr, 0, enModelUpAxisY);
	m_modelRender.SetScale(2.0f, 2.0f, 2.0f);
	m_modelRender.SetPosition(m_position);
	m_modelRender.Update();
	return true;
}
void Bullet::Update()
{
	Vector3 oldPosition = m_position;
	Vector3 hitPos;
	//速度の分だけ進む
	m_position += m_velocity * DELTA_TIME;
	m_modelRender.SetPosition(m_position);
	m_modelRender.Update();


	if (PhysicsWorld::GetInstance()->RayTest(oldPosition, m_position, hitPos))
	{
		//何かに当たったら消える
		DeleteGO(this);
		return;
	}
	//一定時間たったら自分を消す
	m_timer += DELTA_TIME;
	if (m_timer >= LIFE_TIME)
	{
		DeleteGO(this);

	}
}
void Bullet::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
}
