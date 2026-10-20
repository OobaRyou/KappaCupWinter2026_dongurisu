#include "stdafx.h"
#include "player.h"
Player::Player()
{
	//アニメーションクリップをロードする。
	animationClips[enAnimationClip_run].Load("Assets/animData/run.tka");
	animationClips[enAnimationClip_run].SetLoopFlag(true);
	//ユニティちゃんのモデルを読み込む。
	modelRender.Init("Assets/modelData/Squirrel.tkm", animationClips, enAnimationClip_Num, enModelUpAxisY);

	//キャラコンを初期化する。
	characterController.Init(25.0f, 75.0f, position);
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


	//アニメーションの再生。
	PlayAnimation();

	//絵描きさんの更新処理。
	modelRender.Update();
}
void Player::Move()
{
	//xzの移動速度を0.0fにする。
	moveSpeed.x = 0.0f;
	moveSpeed.z = 0.0f;

	//左スティックの入力量を取得。
	Vector3 stickL;
	stickL.x = g_pad[0]->GetLStickXF();
	stickL.y = g_pad[0]->GetLStickYF();

	//カメラの前方向と右方向のベクトルを持ってくる。
	Vector3 forward = g_camera3D->GetForward();
	Vector3 right = g_camera3D->GetRight();
	//y方向には移動させない。
	forward.y = 0.0f;
	right.y = 0.0f;

	//左スティックの入力量と120.0fを乗算。
	right *= stickL.x * 120.0f;
	forward *= stickL.y * 120.0f;

	//移動速度にスティックの入力量を加算する。
	moveSpeed += right + forward;
	//キャラクターコントローラーを使って座標を移動させる。
	position = characterController.Execute(moveSpeed, 1.0f / 60.0f);

	//絵描きさんに座標を教える。
	modelRender.SetPosition(position);
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
	
		//待機アニメーションを再生する。
		modelRender.PlayAnimation(enAnimationClip_run);
		
		//プレイヤーステートが1(ジャンプ中)だったら。
	
	

	

}

//描画処理。
void Player::Render(RenderContext& rc)
{
	//ユニティちゃんを描画する。
	modelRender.Draw(rc);
}