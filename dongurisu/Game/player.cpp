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
	//弾の最大保有数
	const int MAX_AMMO = 50;
	//最大HP
	const int MAX_HP = 50;
	//無敵時間
	const float INVINCIBLE_TIME = 1.0f;
	//確認用：Lキー(Xボタン)で受けるダメージ。
	const int DEBUG_DAMAGE = 10;
	//登る速さ。
	const float CLIMB_SPEED = 80.0f;
	//壁を探す距離。レイの始点(リスの中心)からこの長さだけ前を調べる。
	const float WALL_CHECK_DISTANCE = 15.0f;
	//レイを飛ばす高さ。足元からだと地面に当たるので体の真ん中くらいにする。
	const float WALL_RAY_HEIGHT = 10.0f;
	//左右のレイの間隔。
	const float WALL_RAYS_SIDE = 3.0f;
	//登っている間に保つ、壁までの距離。
	const float WALL_KEEP_DISTANCE = 7.0f;
	//距離のずれを直す強さ。
	const float WALL_KEEP_POWER = 10.0f;
	//登っている間、モデルを壁から離す量。
	const float MODEL_WALL_OFFSET = 2.4f;
	//壁の上で頭の向きを変える速さ(1秒あたりの角度)。
	const float CLIMB_TURN_SPEED = 540.0f;
	//登っている間、足元のこの距離に地面があったら下り終わる。
	const float CLIMB_GROUND_CHECK = 2.0f;
	//登っている間にキャラコンへ渡す、ごく小さい上向きの速さ。上向きの間は「空中」の扱いになり、段差に乗らない。
	const float CLIMB_AIR_SPEED = 0.001f;
	//壁までの距離がこれより近かったら、外へ押し出す。リスの当たり判定の半径(6)より少し小さくする。
	const float PUSH_OUT_DISTANCE = 5.5f;
	//地上にいるとき、足元のこの距離に地面がなければ、段差から降りたと判断する。
	const float GROUND_SNAP_CHECK = 5.0f;
	//押し出しで調べる向きの数。
	const int PUSH_OUT_DIRECTIONS = 8;
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
	//登っている途中で止まったときの姿勢。走るアニメの、手足が壁に一番近い瞬間を取り出したもの。
	animationClips[enAnimationClip_climbStop].Load("Assets/modelData/climb_stop.tka");
	animationClips[enAnimationClip_climbStop].SetLoopFlag(true);
	//ユニティちゃんのモデルを読み込む。
	modelRender.Init("Assets/modelData/Squirrel.tkm", animationClips, enAnimationClip_Num, enModelUpAxisY);

	//キャラコンを初期化する。
	characterController.Init(6.0f,8.0f, position);
	m_ammo = MAX_AMMO;
	m_hp = MAX_HP;
}
Player::~Player()
{

}
void Player::Update()
{

	if (IsDead())
	{	//アニメーションの再生。
		PlayAnimation();

		//絵描きさんの更新処理。
		modelRender.Update();
		return;
	}
	//入力を読む。移動やジャンプより先に呼ぶ。
	m_input.Update();
	//無敵時間を減らす。0より下は「無敵ではない」と同じ扱いなので、そのままでよい。
	m_invincibleTimer -= DELTA_TIME;
	//
	m_blinkCount += 1;
	//確認用：Lキー(Xボタン)でダメージを受ける。敵の攻撃ができたら消す。
	if (g_pad[0]->IsTrigger(enButtonX)) {
		Damage(DEBUG_DAMAGE);
	}
	if (m_isClimbing)
	{
		Climb();
	}
	else
	{
		//ジャンプ処理。上下の速さを決めてから移動させるので、Move より先に呼ぶ。
		Jump();
		//移動処理。
		Move();
		//落ちている間に壁へ入りこまないように押し出す。
		PushOutFromWall();
		//スティックを倒した向きに壁があったら張りつく。
		Vector3 moveDir = moveSpeed;
		moveDir.y = 0.0f;
		if (moveDir.Length() > 0.001f)
		{
			moveDir.Normalize();
			if (FindWall(moveDir, m_wallNormal))
			{
				m_isClimbing = true;
				//張りついたときは頭を上にする。
				m_climbHeadAngle = 0.0f;
			}
		}
		//回転処理。
		Rotation();
		
	}

	//弾発射
	Fire();

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

	//足元のすぐ下に地面がなければ、段差から降りたので空中の扱いにする。
	//地面の上のままだと、キャラコンが下の地面まで一瞬で移動させて、太い幹の中へ入ることがある。
	if (characterController.IsOnGround())
	{
		Vector3 groundHit;
		if (!PhysicsWorld::GetInstance()->RayTest(position + Vector3::Up, position - Vector3::Up * GROUND_SNAP_CHECK, groundHit))
		{
			moveSpeed.y = CLIMB_AIR_SPEED;
		}
	}

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
		if (IsDead() == true)
		{
			animNo = enAnimationClip_dying;
		}
		else if (m_isClimbing)
		{
			//登り専用のアニメは作らず、走るアニメで代用する。止まったら手足を壁に付けた姿勢にする。
			if (fabsf(g_pad[0]->GetLStickXF()) >= 0.001f || fabsf(g_pad[0]->GetLStickYF()) >= 0.001f)
			{
				animNo = enAnimationClip_run;
			}
			else
			{
				animNo = enAnimationClip_climbStop;
			}
		}
		//自動：空中ならjump、移動していたらrun、止まっていたらidle。
		else if (!characterController.IsOnGround())
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
	const wchar_t* animNames[enAnimationClip_Num] = { L"idle", L"idle2", L"run", L"dying", L"walk", L"jump", L"climbStop" };
	wchar_t text[128];
	//確認用：HPと弾薬数も表示する。
	swprintf_s(text, L"anim: %s%s\n[J / A] change\nHP: %d  ammo: %d", animNames[animNo], previewAnimation == -1 ? L" (auto)" : L"", m_hp, m_ammo);
	fontRender.SetText(text);
	fontRender.SetPosition(-900.0f, 400.0f, 0.0f);
}

//描画処理。
void Player::Render(RenderContext& rc)
{
	bool isBlinkHidden = m_invincibleTimer > 0.0f && !IsDead() && m_blinkCount % 6 < 3;
	
	
	if (!isBlinkHidden)
	{
	//リス描画処理
	modelRender.Draw(rc);
	}
	

	
	fontRender.Draw(rc);
}
void Player::Fire()
{
	m_fireTimer -= DELTA_TIME;
	if (m_ammo == 0)
	{
		return;
	}
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
	m_ammo -= 1;
	//撃ったので、次に撃てる時間までの時間をセットする
	m_fireTimer = FIRE_INTERVAL;
}
void Player::Damage(int damage)
{

	if (m_invincibleTimer > 0.0f)
	{
		return;
	}
	m_hp -= damage;
	m_invincibleTimer = INVINCIBLE_TIME;
	if (m_hp <= 0)
	{
		m_hp = 0;
	}
}
bool Player:: IsDead() const
{
	return m_hp <= 0;
}
//forwardの向きに壁があるか調べる。
//壁があったらwallNormalに壁の向き(壁からリスの方への向き)を入れてtrueを返す。
//RayTestは当たった位置しか返さないので、左右にずらしたレイを2本飛ばして、当たった2点から壁の向きを出す。
bool Player::FindWall(const Vector3& forward, Vector3& wallNormal)
{
	//上と前の外積でリスの右向きを出す。
	Vector3 right;
	right.Cross(Vector3::Up, forward);
	//レイの始点。体の真ん中から飛ばす。前に出すと、壁にくっついたときに始点が壁の中に入って当たらなくなる。
	Vector3 rayStart = position + Vector3::Up * WALL_RAY_HEIGHT;
	//左のレイ。始点を左にずらして前に伸ばす。
	Vector3 leftStart = rayStart - right * WALL_RAYS_SIDE;
	Vector3 leftEnd = leftStart + forward * WALL_CHECK_DISTANCE;
	//右のレイ。左と同じで右にずらす。
	Vector3 rightStart = rayStart +right * WALL_RAYS_SIDE;
	Vector3 rightEnd = rightStart + forward * WALL_CHECK_DISTANCE;
	//レイを飛ばす。当たったらtrueが返ってきて、当たった位置が入る。
	Vector3 leftHit;
	Vector3 rightHit;
	bool isLeftHit = PhysicsWorld::GetInstance()->RayTest(leftStart, leftEnd, leftHit);
	bool isRightHit=PhysicsWorld::GetInstance()->RayTest(rightStart,rightEnd,rightHit);
	//片方でも外れたら壁の端か登りきったとこなので、壁なしにする。
	if (!isLeftHit || !isRightHit)
	{
		return false;
	}
	//当たった2点の真ん中までの距離を、壁までの距離として覚えておく。
	Vector3 toHit = (leftHit + rightHit) * 0.5f - position;
	toHit.y = 0.0f;
	m_wallDistance = toHit.Dot(forward);
	//当たった2点を結ぶと壁に沿った向きになる。
	Vector3 side = rightHit - leftHit;
	//上と壁に沿った向きの外積で、壁からリスの方への向きになる。
	wallNormal.Cross(Vector3::Up, side);
	//縦の壁として扱うので上下の傾きは消して、長さを1にする。
	wallNormal.y = 0.0f;
	wallNormal.Normalize();
	//探した向きと同じ側を向いてたら逆なので反対にする(念のため)。
	if (wallNormal.Dot(forward) > 0.0f)
	{
		wallNormal *= -1.0f;
	}
	return true;
}
//周りの壁に近づきすぎていたら外へ押し出す。
//キャラコンは上下に動くときに横の壁を調べないので、下ほど太い幹の横を落ちると中へ入りこんでしまう。
void Player::PushOutFromWall()
{
	//地上を歩いているときは、キャラコンが壁を調べているので何もしない。
	if (characterController.IsOnGround())
	{
		return;
	}
	Vector3 center = position + Vector3::Up * WALL_RAY_HEIGHT;
	Vector3 pushed = position;
	for (int i = 0; i < PUSH_OUT_DIRECTIONS; i++)
	{
		float angle = Math::PI * 2.0f * i / PUSH_OUT_DIRECTIONS;
		Vector3 direction(cosf(angle), 0.0f, sinf(angle));
		Vector3 hit;
		if (PhysicsWorld::GetInstance()->RayTest(center, center + direction * PUSH_OUT_DISTANCE, hit))
		{
			Vector3 toHit = hit - center;
			pushed -= direction * (PUSH_OUT_DISTANCE - toHit.Length());
		}
	}
	position = pushed;
	characterController.SetPosition(position);
	modelRender.SetPosition(position + Vector3::Up * MODEL_FOOT_OFFSET);
}
//登り中の移動。壁を探し直して、見つからなければ登り終わる。
void Player::Climb()
{
	//壁の方向。
	Vector3 toWall = m_wallNormal * -1.0f;
	//壁を探し直す。丸い幹でも、今いる場所の壁の向きが分かる。
	Vector3 newNormal;
	//壁がなければ登りきったので、上に跳ねて地上の動きに戻す。
	if (!FindWall(toWall, newNormal))
	{
		m_isClimbing = false;
		moveSpeed.y = JUMP_SPEED;
		rotation.SetRotationYFromDirectionXZ(toWall);
		modelRender.SetRotation(rotation);
		return;
	}
	//ジャンプボタンで壁から離れる。
	if (m_input.IsJumpTrigger())
	{
		moveSpeed.y = JUMP_SPEED;
		m_isClimbing = false;
		rotation.SetRotationYFromDirectionXZ(toWall);
		modelRender.SetRotation(rotation);
		return;
	}
	//当たり判定が角ばっていて向きが毎フレーム振れるので、少しずつ新しい向きに近づけてガタつきを消す。
	m_wallNormal.Lerp(0.1f, m_wallNormal, newNormal);
	m_wallNormal.Normalize();
	//壁を見たときの右向きを出す。
	Vector3 side;
	side.Cross(m_wallNormal, Vector3::Up);
	Vector3 stickL;
	stickL.x = g_pad[0]->GetLStickXF();
	stickL.y = g_pad[0]->GetLStickYF();
	//スティックで上下左右に動く。壁までの距離がずれていたら、離れすぎなら近づき、近すぎなら離れる。
	//横の移動だけキャラコンに任せる。上下をキャラコンで動かすと、幹のでこぼこに「乗った」と判定されて止まってしまう。
	moveSpeed = side * stickL.x * CLIMB_SPEED + toWall * ((m_wallDistance - WALL_KEEP_DISTANCE) * WALL_KEEP_POWER);
	moveSpeed.y = CLIMB_AIR_SPEED;
	position = characterController.Execute(moveSpeed, DELTA_TIME);
	//上下は自分で動かす。
	position.y += stickL.y * CLIMB_SPEED * DELTA_TIME;
	characterController.SetPosition(position);
	//モデルの表示位置を更新する。体の真ん中の高さで、足が壁に付くように離す。
	modelRender.SetPosition(position + Vector3::Up * WALL_RAY_HEIGHT + m_wallNormal * MODEL_WALL_OFFSET);
	//下りていて足元に地面があったら、登り終わる。頭を下にして下りてきたので、壁と反対の方を向かせる。
	if (stickL.y < 0.0f)
	{
		Vector3 groundCheckEnd = position - Vector3::Up * CLIMB_GROUND_CHECK;
		Vector3 groundHit;
		if (PhysicsWorld::GetInstance()->RayTest(position + Vector3::Up, groundCheckEnd, groundHit))
		{
			position.y = groundHit.y;
			characterController.SetPosition(position);
			moveSpeed = Vector3::Zero;
			m_isClimbing = false;
			rotation.SetRotationYFromDirectionXZ(m_wallNormal);
			modelRender.SetRotation(rotation);
			return;
		}
	}
	//スティックを倒した向きに頭を向ける。0度で上、90度で右、180度で下。離しているときは今の向きのまま。
	float targetAngle = m_climbHeadAngle;
	if (fabsf(stickL.x) >= 0.001f || fabsf(stickL.y) >= 0.001f)
	{
		targetAngle = atan2f(stickL.x, stickL.y) * 180.0f / Math::PI;
	}
	//急に反対を向かないように、近い方の回り方で少しずつ回す。
	float angleDiff = targetAngle - m_climbHeadAngle;
	if (angleDiff > 180.0f)
	{
		angleDiff -= 360.0f;
	}
	else if (angleDiff < -180.0f)
	{
		angleDiff += 360.0f;
	}
	float maxTurn = CLIMB_TURN_SPEED * DELTA_TIME;
	if (fabsf(angleDiff) <= maxTurn)
	{
		m_climbHeadAngle = targetAngle;
	}
	else if (angleDiff > 0.0f)
	{
		m_climbHeadAngle += maxTurn;
	}
	else
	{
		m_climbHeadAngle -= maxTurn;
	}
	//壁の方を向いてから右向きを軸に倒して、頭を上・お腹を壁に向ける。
	rotation.SetRotationYFromDirectionXZ(toWall);
	Quaternion pitch;
	pitch.SetRotationDeg(side, -90.0f);
	rotation.Multiply(pitch);
	//お腹を壁に向けたまま、壁の向きを軸に回して頭の向きを変える。
	Quaternion headTurn;
	headTurn.SetRotationDeg(m_wallNormal, m_climbHeadAngle);
	rotation.Multiply(headTurn);
	modelRender.SetRotation(rotation);
}
