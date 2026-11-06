#pragma once
#include"Enemy.h"
class Player;
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
	bool CanAttack()override;
private:
	/** リスまでの距離を返す。上下の差は無視する。 */
	float GetDistanceToPlayer();

	Player* m_player = nullptr;	//追いかけるリス。
};

