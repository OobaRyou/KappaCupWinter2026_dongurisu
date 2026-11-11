#pragma once
class Player;
class AcornItem :public IGameObject
{
public:
	//モデルを準備する。
	bool Start();
	//更新処理。
	void Update();
	//描画処理。
	void Render(RenderContext& rc);
	//ドロップの位置を決める
	void SetPosition(const Vector3& position);
private:
	ModelRender m_modelRender;		//見た目のモデル。
	Vector3 m_position;				//位置
	Player* m_player = nullptr;

};
