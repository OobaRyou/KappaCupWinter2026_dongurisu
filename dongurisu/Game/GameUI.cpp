#include "stdafx.h"
#include "GameUI.h"
namespace
{
	const float CROSSHAIR_SIZE = 128.0f;	//照準の画像の大きさ（縦横）。
}
bool GameUI::Start()
{
	//照準の画像を読む。位置は(0,0)のまま＝画面のまん中。
	m_crosshairRender.Init("Assets/sprite/crosshair.DDS", CROSSHAIR_SIZE, CROSSHAIR_SIZE);
	m_crosshairRender.Update();
	return true;
}
void GameUI::Render(RenderContext& rc)
{
	m_crosshairRender.Draw(rc);
}
