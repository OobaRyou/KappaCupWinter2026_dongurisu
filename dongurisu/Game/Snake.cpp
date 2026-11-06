#include "stdafx.h"
#include "Snake.h"
Snake::Snake()
{
	m_scale = 2.0f;
	m_hp = 3;
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

}
void Snake::Move()
{

}
void Snake::OnDead()
{

}
