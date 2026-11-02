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
	m_modelRender.Init("Assets/modelData/acorn_fragment.tkm", nullptr, 0, enModelUpAxisY);
	m_modelRender.SetScale(2.0f, 2.0f, 2.0f);
	m_modelRender.SetPosition(m_position);
	//破片事に最初の向きをバラバラにする
	float angleY = rand() / (float)RAND_MAX * 360.0f;
	float angleX = rand() / (float)RAND_MAX * 360.0f;
	Quaternion rotY;
	rotY.SetRotationDeg(Vector3::AxisY, angleY);
	Quaternion rotX;
	rotX.SetRotationDeg(Vector3::AxisX, angleX);
	rotY.Multiply(rotX);
	m_rotation = rotY;
	m_modelRender.SetRotation(rotY);//Rotation()はこの向きから回り始める
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
	Rotation();
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
void Bullet::Rotation()
{
	//回転を加算する。
	m_rotation.AddRotationDegY(2.0f);

	//絵描きさんに回転を教える。
	m_modelRender.SetRotation(m_rotation);
}
