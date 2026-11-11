#pragma once
/** ウェーブ1回分の出現データ。 */
struct WaveData
{
	float waveTime;			//ウェーブの制限時間（秒）。
	int maxAlive;			//場に出てよい数の上限。
	float spawnInterval;	//1匹ずつ出す間隔（秒）。
};

/** 敵をウェーブごとに出して、次のウェーブへ進める。 */
class WaveManager : public IGameObject
{
public:
	bool Start();
	void Update();
private:
	enum class EnWaveState :int
	{
		spawning,	//敵を出している。
		waiting,	//時間切れで、全滅を待っている。
		Resting,	//次のウェーブまで休んでいる。
		Finished,	//全部のウェーブが終わった。
	};
	/** 場にいる（死んでいない）敵の数を数える。 */
	int CountEnemies();
	/** 次のウェーブへ進む。最後ならFinishedにする。 */
	void Nextwave();
	EnWaveState m_state = EnWaveState::spawning;		//今の状態。
	int m_waveNo = 0;				//今のウェーブの番号（0から数える）。
	int m_spawnCount = 0;			//今のウェーブで出した数。
	float m_spawnTimer = 0.0f;		//次の1匹までの時間（秒）。
	float m_restTimer = 0.0f;		//次のウェーブまでの休み時間（秒）。
	float m_waveTimer = 0.0f;		//ウェーブの残り時間（秒）。
};

