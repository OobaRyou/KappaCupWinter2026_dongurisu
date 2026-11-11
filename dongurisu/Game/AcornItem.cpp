#include "stdafx.h"
#include "AcornItem.h"
#include"player.h"
namespace
{
	const float PICKUP_RANGE = 30.0f;		//これより近づいたら拾う距離
	const int ACORN_AMMO = 3;				//どんぐり１個で増える弾薬数
}
bool AcornItem::Start()
{
	m_modelRender.Init("Assets/modelData/acorn.tkm", nullptr, 0, enModelUpAxisY);
	m_modelRender.SetPosition(m_position);
	m_modelRender.SetScale(5.0f, 5.0f, 5.0f);
	m_modelRender.Update();
	m_player = FindGO<Player>("player");
	return true;
}
void AcornItem::Update()
{
	Vector3 toPlayer = m_player->position - m_position;
	float distance = toPlayer.Length();
	if (distance <= PICKUP_RANGE)
	{
		m_player->AddAmmo(ACORN_AMMO);
		DeleteGO(this);
		return;
	}
	m_modelRender.Update();
}
void AcornItem::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
}
void AcornItem::SetPosition(const Vector3& position)
{
	m_position = position;
}
