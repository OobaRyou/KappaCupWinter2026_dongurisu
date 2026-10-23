#include "stdafx.h"
#include "player.h"

namespace {
	/** モデルの原点は足元より上にあるので、表示するときにこの分だけ上げて足を地面に合わせる。 */
	const float MODEL_FOOT_OFFSET = 9.4f;
}

Player::Player()
{
	//アニメーションクリップをロードする。
	animationClips[enAnimationClip_idle].Load("Assets/modelData/idle.tka");
	animationClips[enAnimationClip_idle].SetLoopFlag(true);
	animationClips[enAnimationClip_idle2].Load("Assets/modelData/idle2.tka");
	animationClips[enAnimationClip_idle2].SetLoopFlag(true);
	animationClips[enAnimationClip_run].Load("Assets/modelData/run.tka");
	animationClips[enAnimationClip_run].SetLoopFlag(true);
	animationClips[enAnimationClip_dying].Load("Assets/modelData/dying.tka");
	animationClips[enAnimationClip_dying].SetLoopFlag(false);
	//ユニティちゃんのモデルを読み込む。
	modelRender.Init("Assets/modelData/Squirrel.tkm", animationClips, enAnimationClip_Num, enModelUpAxisY);

	//キャラコンを初期化する。
	characterController.Init(6.0f,8.0f, position);
}
Player::~Player()
{

}
void Player::Update()
{
	//移動処理。
	Move();

	//回転処理。
	Rotation();
	// ジャンプ処理
	Jump();

	//アニメーションの再生。
	PlayAnimation();

	//絵描きさんの更新処理。
	modelRender.Update();
}
void Player::Move()
{
	// 横方向の速度をリセットする
	moveSpeed.x = 0.0f;
	moveSpeed.z = 0.0f;

	// 左スティックの入力を取得する
	Vector3 stickL;
	stickL.x = g_pad[0]->GetLStickXF();
	stickL.y = g_pad[0]->GetLStickYF();

	// カメラの方向を取得する
	Vector3 forward = g_camera3D->GetForward();
	Vector3 right = g_camera3D->GetRight();

	// 高さ方向の移動を無効にする
	forward.y = 0.0f;
	right.y = 0.0f;

	// スティックの入力に応じて移動速度を設定する
	right *= stickL.x * 120.0f;
	forward *= stickL.y * 120.0f;

	moveSpeed += right + forward;

	// セレクトボタンを押している間は横方向の速度を2倍にする
	if (g_pad[0]->IsPress(enButtonSelect))
	{
		moveSpeed.x *= 2.0f;
		moveSpeed.z *= 2.0f;
	}

	// キャラクターを移動させる
	position = characterController.Execute(moveSpeed, 1.0f / 60.0f);

	// モデルの表示位置を更新する
	modelRender.SetPosition(
		position + Vector3(0.0f, MODEL_FOOT_OFFSET, 0.0f)
	);
}
void Player::Jump()
{
	// 地面にいるか確認する
	if (characterController.IsOnGround())
	{
		// 落下速度をリセットする
		moveSpeed.y = 0.0f;

		// Bボタンを押した瞬間にジャンプする
		if (g_pad[0]->IsTrigger(enButtonB))
		{
			moveSpeed.y = 300.0f;
		}
	}
	else
	{
		// 空中では重力を加える
		moveSpeed.y -= GRAVITY;
	}
}
void Player::Rotation()
{
	//xかzの移動速度があったら(スティックの入力があったら)。
	if (fabsf(moveSpeed.x) >= 0.001f || fabsf(moveSpeed.z) >= 0.001f)
	{
		//キャラクターの方向を変える。
		rotation.SetRotationYFromDirectionXZ(moveSpeed);
		//絵描きさんに回転を教える。
		modelRender.SetRotation(rotation);
	}
}
void Player::PlayAnimation()
{
	//確認用：Aボタン(キーボードのJ)で 自動→idle→idle2→run→dying→自動… と切り替える。
	if (g_pad[0]->IsTrigger(enButtonA)) {
		previewAnimation++;
		if (previewAnimation >= enAnimationClip_Num) {
			previewAnimation = -1;
		}
	}

	int animNo = previewAnimation;
	if (animNo == -1) {
		//自動：移動していたらrun、止まっていたらidle。
		if (fabsf(moveSpeed.x) >= 0.001f || fabsf(moveSpeed.z) >= 0.001f) {
			animNo = enAnimationClip_run;
		}
		else {
			animNo = enAnimationClip_idle;
		}
	}
	modelRender.PlayAnimation(animNo, 0.2f);

	//再生中のアニメーション名を表示する。
	const wchar_t* animNames[enAnimationClip_Num] = { L"idle", L"idle2", L"run", L"dying" };
	wchar_t text[128];
	swprintf_s(text, L"anim: %s%s\n[J / A] change", animNames[animNo], previewAnimation == -1 ? L" (auto)" : L"");
	fontRender.SetText(text);
	fontRender.SetPosition(-900.0f, 400.0f, 0.0f);
}

//描画処理。
void Player::Render(RenderContext& rc)
{
	//ユニティちゃんを描画する。
	modelRender.Draw(rc);
	fontRender.Draw(rc);
}
