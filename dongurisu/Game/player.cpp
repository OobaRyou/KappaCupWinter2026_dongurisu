#include "stdafx.h"
#include "player.h"

namespace {
	/** モデルの原点は足元より上にあるので、表示するときにこの分だけ上げて足を地面に合わせる。 */
	const float MODEL_FOOT_OFFSET = 9.4f;
	/** 歩きの速さ。 */
	const float WALK_SPEED = 60.0f;
	/** ダッシュの速さ。 */
	const float DASH_SPEED = 240.0f;
	/** ジャンプの初速(1秒あたり)。重力と合わせて、滞空が約0.45秒・高さが約30になる。 */
	const float JUMP_SPEED = 270.0f;
	/** 重力加速度(1秒あたり)。ジャンプアニメの空中の長さ(約0.45秒)に合わせた値。 */
	const float GRAVITY = 1200.0f;
	/** 1フレームの時間(秒)。 */
	const float DELTA_TIME = 1.0f / 60.0f;
	//ドングリの弾の速さ
	const float BULLET_SPEED = 600.0f;
	//発射位置高さ
	const float MOUTH_HEIGHT = 12.0f;
	//発射位置中心からの距離
	const float MOUTH_FORWARD = 10.0f;
	//撃つクールタイム
	const float FIRE_INTERVAL = 0.1f;
	//一回の発射で出る量
	const int FRAGMENT_COUNT = 10;
	//広がる角度
	const float SPREAD_ANGLE = 15.0f;
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
	animationClips[enAnimationClip_walk].Load("Assets/modelData/walk.tka");
	animationClips[enAnimationClip_walk].SetLoopFlag(true);
	//ジャンプは1回だけ再生する。
	animationClips[enAnimationClip_jump].Load("Assets/modelData/jump.tka");
	animationClips[enAnimationClip_jump].SetLoopFlag(false);
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
	//入力を読む。移動やジャンプより先に呼ぶ。
	m_input.Update();
	//ジャンプ処理。上下の速さを決めてから移動させるので、Move より先に呼ぶ。
	Jump();
	//移動処理。
	Move();

	//弾発射
	Fire();

	//回転処理。
	Rotation();

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

	//ダッシュか歩きかで速さを変える。
	float speed = WALK_SPEED;
	if (m_input.IsDash()) {
		speed = DASH_SPEED;
	}

	// スティックの入力に応じて移動速度を設定する
	right *= stickL.x * speed;
	forward *= stickL.y * speed;

	moveSpeed += right + forward;

	// キャラクターを移動させる
	position = characterController.Execute(moveSpeed, DELTA_TIME);

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
		if (m_input.IsJumpTrigger())
		{
			moveSpeed.y = JUMP_SPEED;
		}
	}
	else
	{
		// 空中では重力を加える。1フレームの時間を掛けて、FPS が変わっても同じ落ち方にする。
		moveSpeed.y -= GRAVITY * DELTA_TIME;
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
	//確認用：Aボタン(キーボードのJ)で 自動→idle→idle2→run→dying→walk→jump→自動… と切り替える。
	if (g_pad[0]->IsTrigger(enButtonY)) {
		previewAnimation++;
		if (previewAnimation >= enAnimationClip_Num) {
			previewAnimation = -1;
		}
	}

	int animNo = previewAnimation;
	if (animNo == -1) {
		//自動：空中ならjump、移動していたらrun、止まっていたらidle。
		if (!characterController.IsOnGround())
		{
			animNo = enAnimationClip_jump;
		}
		else if (fabsf(moveSpeed.x) >= 0.001f || fabsf(moveSpeed.z) >= 0.001f) {
			//移動中：ダッシュならrun、歩きならwalk。
			if (m_input.IsDash()) {
				animNo = enAnimationClip_run;
			}
			else {
				animNo = enAnimationClip_walk;
			}
		}
		else {

			animNo = enAnimationClip_idle;
		}
	}
	modelRender.PlayAnimation(animNo, 0.2f);

	//再生中のアニメーション名を表示する。
	const wchar_t* animNames[enAnimationClip_Num] = { L"idle", L"idle2", L"run", L"dying", L"walk", L"jump" };
	wchar_t text[128];
	swprintf_s(text, L"anim: %s%s\n[J / A] change", animNames[animNo], previewAnimation == -1 ? L" (auto)" : L"");
	fontRender.SetText(text);
	fontRender.SetPosition(-900.0f, 400.0f, 0.0f);
}

//描画処理。
void Player::Render(RenderContext& rc)
{
	//リス描画処理
	modelRender.Draw(rc);
	fontRender.Draw(rc);
}
void Player::Fire()
{
	m_fireTimer -= DELTA_TIME;
	//まだ撃てる時間でなかったら何もしない
	if (m_fireTimer > 0.0f)
	{
		return;
	}
	if (!m_input.IsFirePress()) {
		return;
	}
	//カメラの向いている方向に撃つ(照準とカメラの向きを合わせる仕様)。
	Vector3 direction = g_camera3D->GetForward();

	//発射位置はリスの口のあたり。足元から上へ、撃つ方向へ少し前に出す。
	Vector3 front = direction;
	front.y = 0.0f;
	front.Normalize();
	Vector3 firePosition = position + Vector3(0.0f, MOUTH_HEIGHT, 0.0f) + front * MOUTH_FORWARD;
	
	for (int i=0;i < FRAGMENT_COUNT;i++)
	{
		float yawt = rand() / (float)RAND_MAX;
		float yawAngle = (yawt * 2.0f - 1.0f) * SPREAD_ANGLE;
		float pitcht = rand() / (float)RAND_MAX;
		float pitchAngle = (pitcht * 2.0f - 1.0f) * SPREAD_ANGLE;
		Vector3 fragmentDirection = direction;
		Quaternion yaw;
		yaw.SetRotationDeg(Vector3::Up, yawAngle);                  //真上を軸に回す → 左右に振れる。
		yaw.Apply(fragmentDirection);
		Quaternion pitch;
		pitch.SetRotationDeg(g_camera3D->GetRight(), pitchAngle);   //カメラの右向きを軸に回す → 上下に振れる。
		pitch.Apply(fragmentDirection);
		Bullet* bullet = NewGO<Bullet>(0, "bullet");
		bullet->SetPosition(firePosition);
		bullet->SetVelocity(fragmentDirection* BULLET_SPEED);
	}
	//撃ったので、次に撃てる時間までの時間をセットする
	m_fireTimer = FIRE_INTERVAL;
}
