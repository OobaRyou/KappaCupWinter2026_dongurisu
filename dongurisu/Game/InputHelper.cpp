#include "stdafx.h"
#include "InputHelper.h"
namespace {
	/** スティックの値の最大(sThumbLX などはこの範囲)。 */
	const float STICK_MAX = 32767.0f;
	/** これより小さい倒し具合は 0 とみなす。 */
	const float STICK_DEADZONE = XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE / STICK_MAX;
	/** 倒し具合がこれ以上ならダッシュ。 */
	const float DASH_THRESHOLD = 0.7f;
}
void InputHelper::Update()
{
	XINPUT_STATE state = {};
	DWORD result = XInputGetState(0, &state);
	if (result!=ERROR_SUCCESS)
	{
		m_isPadConnected = false;
		m_stickStrength = 0.0f;
		return;
	}
	m_isPadConnected = true;
	//スティックの値
	float x = state.Gamepad.sThumbLX / STICK_MAX;
	float y = state.Gamepad.sThumbLY/ STICK_MAX;
	//倒し具合
	float strength = sqrtf(x * x + y * y);
	// 斜めに倒すと 1 を少し超えることがあるので 1 に抑える。
	//   ほとんど倒していない(離しても残る小さな値)なら 0 にする。
	if (strength > 1.0f) {
		strength = 1.0f;
	}
	if (strength < STICK_DEADZONE) {
		strength = 0.0f;
	}
	m_stickStrength = strength;

}

bool InputHelper::IsMoving() const
{
	if (m_isPadConnected)
	{//パッド：スティックを少しでも倒していれば移動中。
		return m_stickStrength > 0.0f;
	}
	return g_pad[0]->GetLStickXF() != 0.0f || g_pad[0]->GetLStickYF() != 0.0f;

}
bool InputHelper::IsDash()const
{
	if (!m_isPadConnected)
	{
		//キーボードは倒し具合がないので常にダッシュ
		return true;
	}
	//大きく倒していればダッシュ少しなら歩き
	return m_stickStrength >= DASH_THRESHOLD;
}
bool InputHelper::IsJumpTrigger()const
{
//Bボタン（キーボードは	K)押しっぱなしで連続ジャンプしないよう、押した瞬間だけtrue。
	return g_pad[0]->IsTrigger(enButtonB);
}
float InputHelper::GetStickStrength()const
{
	return m_stickStrength;
}

bool InputHelper::IsFirePress()const
{
	if (m_isPadConnected)
	{
		return g_pad[0]->IsPress(enButtonRB1);
	}
	return g_pad[0]->IsPress(enButtonA);
}
