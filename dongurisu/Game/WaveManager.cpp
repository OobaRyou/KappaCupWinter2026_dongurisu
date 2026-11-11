#include "stdafx.h"
#include "WaveManager.h"
#include"Snake.h"
#include"Enemy.h"
namespace
{
	//ウェーブごとの出現データ。{ 制限時間, 場に出てよい数, 出す間隔 }。上から1回目、2回目、3回目。
	const WaveData WAVE_TABLE[] =
	{
		{90.0f,10, 2.0f },		//1回目。90秒、同時に10匹まで。
		{90.0f,15, 2.0f },		//2回目。90秒、同時に15匹まで。
		{ 120.0f,20, 2.0f },		//3回目。120秒、同時に20匹まで。
	};
	//ウェーブの数。
	const int WAVE_NUM = sizeof(WAVE_TABLE) / sizeof(WAVE_TABLE[0]);
	//敵が出てくる場所。
	const Vector3 SPAWN_POSITIONS[] =
	{
		Vector3(-700.0f, 0.0f, -500.0f),//左手前。
		Vector3(700.0f, 0.0f, -500.0f),	//右手前。
		Vector3(-800.0f, 0.0f,  700.0f),//左奥。
		Vector3(800.0f, 0.0f,  700.0f),	//右奥。
	};
	//出てくる場所の数。
	const int SPAWN_POSITION_NUM = sizeof(SPAWN_POSITIONS) / sizeof(SPAWN_POSITIONS[0]);
	//次のウェーブまでの休み時間（秒）。
	const float REST_TIME = 10.0f;
}
bool WaveManager::Start()
{
	//1回目のウェーブの時間を入れる。
	m_waveTimer = WAVE_TABLE[0].waveTime;
	return true;
}
void WaveManager::Update()
{
	switch (m_state)
	{
	case EnWaveState::spawning:
	{
		const WaveData& wave = WAVE_TABLE[m_waveNo];
		//ウェーブの残り時間を減らす。
		m_waveTimer -= g_gameTime->GetFrameDeltaTime();
		//時間切れなら、全滅を待つ。
		if (m_waveTimer<=0.0f)
		{
			m_state = EnWaveState::waiting;
			break;
		}
		//場の数が上限なら、今は出さない。
		if (CountEnemies()>=wave.maxAlive)
		{
			break;
		}
		//間隔ごとに1匹出す。出す場所は順番に変える。
		m_spawnTimer -= g_gameTime->GetFrameDeltaTime();
		if (m_spawnTimer <= 0.0f)
		{
			Snake* snake = NewGO<Snake>(0, "enemy");
			snake->SetPosition(SPAWN_POSITIONS[m_spawnCount % SPAWN_POSITION_NUM]);
			m_spawnTimer = wave.spawnInterval;
			m_spawnCount += 1;
		}
		break;
	}
	case EnWaveState::waiting:
	{
		//場の敵がいなくなったら、休みに入る。
		if (CountEnemies() == 0)
		{
			m_restTimer = REST_TIME;
			m_state = EnWaveState::Resting;
		}
		break;
	}
	case EnWaveState::Resting:
	{
		//休みが終わったら、次のウェーブへ。
		m_restTimer -= g_gameTime->GetFrameDeltaTime();
		if (m_restTimer <= 0.0f)
		{
			Nextwave();
		}
		break;
	}
	case EnWaveState::Finished:
		//全部終わったので、何もしない。
		break;
	}
	
}
int WaveManager::CountEnemies()
{
	int count = 0;
	QueryGOs<Enemy>("enemy", [&](Enemy* enemy)
		{
			//死んでいない敵だけ数える。
			if (!enemy->IsDead())
			{
				count++;
			}
			//全部の敵を見たいので、いつも次へ進む。
			return true;
		});
	return count;
	
}
void WaveManager::Nextwave()
{
	m_waveNo++;
	//最後のウェーブが終わったら、Finishedにする。
	if (m_waveNo >= WAVE_NUM)
	{
		m_state = EnWaveState::Finished;
		return;
	}
	//次のウェーブの準備をする。
	m_spawnCount = 0;
	m_spawnTimer = 0.0f;
	m_waveTimer = WAVE_TABLE[m_waveNo].waveTime;
	m_state = EnWaveState::spawning;
}
