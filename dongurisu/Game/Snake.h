#pragma once
#include"Enemy.h"
class Snake:public Enemy
{
public:
	Snake();
protected:
	const char* GetModelFilePath()override;
	const char* GetAnimationFilePath(EnState state)override;
	void Move()override;
	void Attack()override;
	void OnDead()override;
};

