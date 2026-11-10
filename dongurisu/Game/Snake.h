#pragma once
#include"Enemy.h"
class Player;
class Nest;
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
	//巣までの距離を返す
	float GetDistanceToNest();
	//狙う相手を決める
	void UpdateTarget();

	Player* m_player = nullptr;	//追いかけるリス。
	Nest* m_nest = nullptr;//攻撃する巣
	bool m_isTargetPlayer = false;//座標を取得する
};

