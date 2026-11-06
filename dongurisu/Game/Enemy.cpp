#include "stdafx.h"
#include "Enemy.h"

namespace {
	// アニメーションを切り替えるときの補間時間(秒)。 
	const float ANIMATION_INTERPOLATE_TIME = 0.2f;
}

bool Enemy::Start()
{
	//アニメーションクリップを読み込む。

	for (int i = 0; i < static_cast<int>(EnState::Num); i++)
	{
		EnState state = static_cast<EnState>(i);
		m_animationClips[i].Load(GetAnimationFilePath(state));
		//待機と移動はくり返し、それ以外は1回だけ再生する。
		m_animationClips[i].SetLoopFlag(state == EnState::Idle || state == EnState::Move);
	}
	//モデルを読み込む。
	m_modelRender.Init(GetModelFilePath(), m_animationClips, static_cast<int>(EnState::Num), enModelUpAxisY);
	//アニメーションイベントが起きたら OnAnimationEvent を呼んでもらう。
	m_modelRender.AddAnimationEvent([this](const wchar_t* clipName, const wchar_t* eventName) {
		OnAnimationEvent(eventName);
	});
	m_modelRender.SetScale(m_scale, m_scale, m_scale);
	m_modelRender.SetPosition(m_position);
	m_modelRender.SetRotation(m_rotation);
	return true;
}

void Enemy::Update()
{
	switch (m_state)
	{
	case EnState::Idle:
	case EnState::Move:
		UpdateIdleAndMove();
		break;
	case EnState::Attack:
	case EnState::Damage:
		//アニメーションが終わったら待機に戻る。
		if (!m_modelRender.IsPlayingAnimation())
		{
			m_state = EnState::Idle;
		}
		break;
	case EnState::Dead:
		//撃破アニメーションが終わったら消す。
		if (!m_modelRender.IsPlayingAnimation())
		{
			DeleteGO(this);
		}
		break;
	}

	PlayAnimation();

	m_modelRender.SetPosition(m_position);
	m_modelRender.SetRotation(m_rotation);
	m_modelRender.Update();
}

void Enemy::UpdateIdleAndMove()
{
	//移動処理は子クラスに任せる。
	Vector3 oldPosition = m_position;
	Move();
	//位置が変わっていたら移動中、変わっていなければ待機。
	Vector3 moved = m_position - oldPosition;
	if (moved.LengthSq() > 0.0f)
	{
		m_state = EnState::Move;
	}
	else
	{
		m_state = EnState::Idle;
	}

	//攻撃の時間が来たら、攻撃を始める。当たる瞬間の処理はアニメーションイベントで呼ぶ。
	m_attackTimer += g_gameTime->GetFrameDeltaTime();
	if (m_attackTimer >= m_attackInterval)
	{
		m_attackTimer = 0.0f;
		m_state = EnState::Attack;
	}
}

void Enemy::PlayAnimation()
{
	//状態の番号が、そのままアニメーションクリップの番号になっている。
	m_modelRender.PlayAnimation(static_cast<int>(m_state), ANIMATION_INTERPOLATE_TIME);
}

void Enemy::OnAnimationEvent(const wchar_t* eventName)
{
	//攻撃アニメーションの「Hit」で、攻撃を当てる。
	if (m_state == EnState::Attack && wcscmp(eventName, L"Hit") == 0)
	{
		Attack();
	}
}

void Enemy::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
}

bool Enemy::Damage(int damage)
{
	//もう倒れていたら何もしない(ショットガンの破片が同じフレームに何個も当たることがあるため)。
	if (IsDead())
	{
		return false;
	}
	m_hp -= damage;
	if (m_hp <= 0)
	{
		m_hp = 0;
		m_state = EnState::Dead;
		OnDead();
		return true;
	}
	m_state = EnState::Damage;
	return false;
}

bool Enemy::IsDead() const
{
	return m_state == EnState::Dead;
}

void Enemy::SetPosition(const Vector3& position)
{
	m_position = position;
}

const Vector3& Enemy::GetPosition() const
{
	return m_position;
}
