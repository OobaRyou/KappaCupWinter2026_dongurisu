#include "stdafx.h"
#include "Snake.h"
#include"player.h"
namespace
{
	const float SNAKE_SPEED = 40.0f;
	const float STOP_DISTANCE = 40.0f;
	const float ATTACK_RANGE = 50.0f;
	const int SNAKE_ATTACK_DAMAGE = 10;
}
Snake::Snake()
{
	m_scale = 2.0f;
	m_hp = 10;
	m_attackInterval = 2.0f;
}
const char* Snake::GetModelFilePath()
{
	return"Assets/modelData/snake.tkm";
}
const char* Snake::GetAnimationFilePath(EnState state)
{
	switch (state)
	{
	case EnState::Idle:
		return"Assets/modelData/snake_idle.tka";
		break;
	case EnState::Move:
		return"Assets/modelData/snake_move.tka";
		break;
	case EnState::Attack:
		return"Assets/modelData/snake_attack.tka";
		break;
	case EnState::Damage:
		return"Assets/modelData/snake_damage.tka";
		break;
	case EnState::Dead:
		return"Assets/modelData/snake_death.tka";
		break;
	}
	return nullptr;
}
void Snake:: Attack()
{
	if (m_player == nullptr)
	{
		return;
	}
	float distance = GetDistanceToPlayer();
	if (distance <= ATTACK_RANGE)
	{
		m_player->Damage(SNAKE_ATTACK_DAMAGE);
	}
}
void Snake::Move()
{
	//リスをまだ見つけていなければ探す。毎フレーム探すと重いので、最初の1回だけ。
	if (m_player == nullptr)
	{
		m_player = FindGO<Player>("player");
	}
	//ヘビからリスへの向きと距離を求める。上下は無視する。
	//Move は距離だけでなく向きも使うので、ここで計算する。
	Vector3 toPlayer = m_player->position - m_position;
	toPlayer.y = 0.0f;
	float distance = toPlayer.Length();
	toPlayer.Normalize();
	//近づきすぎていなければ、リスの方へ進む。
	if (distance > STOP_DISTANCE)
	{
		m_position += toPlayer * SNAKE_SPEED * g_gameTime->GetFrameDeltaTime();
	}
	//止まっていても、頭をリスの方へ向ける。
	m_rotation.SetRotationYFromDirectionXZ(toPlayer);
}
void Snake::OnDead()
{

}
bool Snake::CanAttack()
{
	if (m_player == nullptr)
	{
		return false;
	}
	//噛みつきが届く距離にリスがいれば、攻撃を始めてよい。
	float distance = GetDistanceToPlayer();
	return distance <= ATTACK_RANGE;
}
float Snake::GetDistanceToPlayer()
{
	Vector3 toPlayer = m_player->position - m_position;
	toPlayer.y = 0.0f;
	return toPlayer.Length();
}
