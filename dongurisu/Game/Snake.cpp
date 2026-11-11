#include "stdafx.h"
#include "Snake.h"
#include"player.h"
#include"Nest.h"
#include"AcornItem.h"
namespace
{
	const float SNAKE_SPEED = 40.0f;
	const float STOP_DISTANCE = 40.0f;
	const float ATTACK_RANGE = 50.0f;
	const int SNAKE_ATTACK_DAMAGE = 10;
	const float CHASE_DISTANCE = 200.0f;
	const float NEST_STOP_DISTANCE = 150.0f;
	const float NEST_ATTACK_RANGE = 160.0f;
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
	//巣を狙っているときは、巣に届けば巣を噛む。
	if (!m_isTargetPlayer)			
	{
		if (GetDistanceToNest() <= NEST_ATTACK_RANGE)	
		{
			m_nest->Damage(SNAKE_ATTACK_DAMAGE);		
		}
		return;						
	}
	//噛みつきが届く距離にリスがいれば、噛む
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
	if (m_nest == nullptr)
	{
		m_nest = FindGO<Nest>("nest");
	}
	UpdateTarget();
	Vector3 targetPosition;	//目指す位置。
	float stopDistance;		//止まる距離。
	if (m_isTargetPlayer)
	{
		//リスを目指す。
		targetPosition = m_player->position;
		stopDistance = STOP_DISTANCE;
	}
	else
	{
		//巣を目指す。
		targetPosition = m_nest->GetPosition();
		stopDistance = NEST_STOP_DISTANCE;
	}
	//ヘビから目標への向きと距離を求める。上下は無視する。
//Move は距離だけでなく向きも使うので、ここで計算する。
	Vector3 toTarget = targetPosition - m_position;
	toTarget.y = 0.0f;
	float distance = toTarget.Length();
	toTarget.Normalize();
	//近づきすぎていなければ、目標の方へ進む。
	if (distance > stopDistance)
	{
		m_position += toTarget * SNAKE_SPEED * g_gameTime->GetFrameDeltaTime();
	}
	//止まっていても、頭を目標の方へ向ける。
	m_rotation.SetRotationYFromDirectionXZ(toTarget);
	
	
}
void Snake::OnDead()
{
	AcornItem* acornItem = NewGO<AcornItem>(0,"acornItem");
	acornItem->SetPosition(m_position);

}
bool Snake::CanAttack()
{
	if (m_player == nullptr)
	{
		return false;
	}
	if (!m_isTargetPlayer)
	{
		return GetDistanceToNest() <= NEST_ATTACK_RANGE;
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
float Snake::GetDistanceToNest()
{
	Vector3 toNest = m_nest->GetPosition() - m_position;
	toNest.y = 0.0f;
	return toNest.Length();
}
void Snake::UpdateTarget()
{
	//リスまでの距離を高さを含めて求める
	Vector3 toPlayer = m_player->position - m_position;
	float distance = toPlayer.Length();
	//巣が壊れているか。リスが近ければ、リスを狙う
	if (m_nest->IsBroken() || distance <= CHASE_DISTANCE)
	{
		m_isTargetPlayer = true;
	}
	//それ以外は巣を狙う
	else
	{
		m_isTargetPlayer = false;
	}
}
