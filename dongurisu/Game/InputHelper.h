#pragma once

/** パッドとキーボードの違いをまとめて、プレイヤーの操作を判定する。 */
class InputHelper
{
public:
	/** 毎フレームの最初に呼ぶ。パッドの状態を読み直す。 */
	void Update();
	/** 移動の入力があるか。 */
	bool IsMoving() const;
	/** ダッシュするか。 */
	bool IsDash() const;
	/** ジャンプボタンを押した瞬間か。 */
	bool IsJumpTrigger() const;
	/** 左スティックの倒し具合(0〜1)。確認の表示用。 */
	float GetStickStrength() const;
	//発射ボタンを押したかどうか
	bool IsFirePress()const;
private:
	bool m_isPadConnected = false;	//パッドがつながっているか。
	float m_stickStrength = 0.0f;	//左スティックの倒し具合(0〜1)。
};
