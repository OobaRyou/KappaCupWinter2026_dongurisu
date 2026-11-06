#include "stdafx.h"
#include "DamageNumber.h"
namespace
{
	const float LIFE_TIME = 0.8f;
	const float RISE_SPEED = 30.0f;
}

void DamageNumber::Init(int damage, const Vector3& position)
{
	m_position = position;
	wchar_t text[16];
	swprintf_s(text, L"%d", damage);
	m_fontRender.SetText(text);
	m_fontRender.SetColor(1.0f, 1.0f, 0.0f, 1.0f);
	
}
void DamageNumber::Update()
{
	m_timer += g_gameTime->GetFrameDeltaTime();
	if (m_timer >= LIFE_TIME)
	{
		DeleteGO(this);
		return;
	}
	m_position.y += RISE_SPEED* g_gameTime->GetFrameDeltaTime();
	float alpha = 1.0f - m_timer / LIFE_TIME;
	Vector2 screenPos;
	g_camera3D->CalcScreenPositionFromWorldPosition(screenPos, m_position);
	m_fontRender.SetPosition(screenPos.x, screenPos.y, 0.0f);
	m_fontRender.SetColor(1.0f, 1.0f, 0.0f, alpha);
}
void DamageNumber::Render(RenderContext& rc)
{
	m_fontRender.Draw(rc);
}
