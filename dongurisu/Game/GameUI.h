#pragma once
/** 画面に重ねて出すUI（照準など）。 */
class GameUI : public IGameObject
{
public:
	/** 画像を読み込む。 */
	bool Start();
	/** 描画処理。 */
	void Render(RenderContext& rc);
private:
	SpriteRender m_crosshairRender;		//照準の画像。
};
