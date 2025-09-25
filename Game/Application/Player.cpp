#include "Player.h"
#include "Input.h"
#include "engine/bace/ImGuiManager.h"
#include "SpriteCommon.h"
#include "Game/Particle/EffectManager.h"
#include "Map.h"
#include "engine/bace/Logger.h"

AABB Player::GetAABB() const
{
	return {
		transform.translate - transform.scale/2,
		transform.translate + transform.scale/2
	};
}

void Player::OnCollision(Collider* other) 
{
	if (other->GetType() == Collider::Type::Enemy) {
		hitThisFrame = true;     
		isAlive = false;  // プレイヤーが敵に当たったら死亡
		// 必要ならダメージ処理など
	}
}

AABB Player::CalcAABBAtPosition(const Vector3& pos)
{
	return  {
		pos - transform.scale * 0.5f,
		pos + transform.scale * 0.5f
	};
}



void Player::Initialize()
{
	// 初期化処理
	playerModel = std::make_unique<Object3D>();
	playerModel->Initialize();
	playerModel->SetModel("Player.obj");
	transform = {
		// Scale
		{1.0f, 1.0f, 1.0f},
		// Rotate
		{0.0f, 3.0f, 0.0f},
		// Translate
		{1.0f, -7.0f, 20.0f} };
	playerModel->SetTransform(transform);

	/// 一時的
	/*EffectManager::GetInstance()->CreateEffectGroup("ib", "resources/gradationLine.png");
	qux = std::make_unique<EffectEmitter>();

	emitterTransform = qux->GetTransform();
	emitterTransform = {
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{ 7.5f,-4.0f,1.5f}
	};
	qux->SetTransform(emitterTransform);
	qux->SetEffectName("ib");

	EffectManager::GetInstance()->CreateEffectGroup("id", "resources/gradationLine.png");

	quux = std::make_unique<EffectEmitter>();
	quuxTransform = quux->GetTransform();
	quuxTransform = {
		{0.5f,0.5f,0.5f },
		{ 0.0f,0.0f,0.0f },
		{ 6.5f,-4.0f,15.0f }
	};
	quux->SetTransform(quuxTransform);


	quux->SetEffectName("id");*/

	aabb = CalcAABBAtPosition(transform.translate);


}


void Player::Update()
{
	
	// 更新処理
	if(!isAlive) {
		// プレイヤーが死亡している場合は何もしない
		return;
	}
	prevTranslate = transform.translate;
	playerModel->Update();
	Move();
	Jump();
	playerModel->SetTransform(transform);


#ifdef _DEBUG
	DrawImgui();
#endif // DEBUG

}


void Player::Draw()
{
	if(!isAlive) {
		/// プレイヤーが死亡している場合は何もしない
		return;
	}

	// 描画処理
	/// スペースキーを押したら
	if (!Input::GetInstance()->PushKey(DIK_SPACE)) {
		playerModel->Draw();
	}
	
	if (dashInputRight == 2 || dashInputLeft == 2) {
		/// @ ダッシュ演出
		/*qux->EmitRing();
		quux->EmitCylinder();
		EffectManager::GetInstance()->DrawRing();*/
		//EffectManager::GetInstance()->DrawCylinder();

	}
}


void Player::Move()
{
	/// 右方向への移動入力処理
	MoveRight();
	/// 左方向への移動入力処理
	MoveLeft();

}

void Player::MoveRight()
{
	/// Dが押されたらダッシュ入力を増やす
	if (Input::GetInstance()->TriggerKey(DIK_D)) {
		dashInputRight++;
	}

	/// プレイヤーの移動処理
	if (Input::GetInstance()->PushKey(DIK_D)) {
		transform.translate.x += velocity.x;
	}
	/// ダッシュ入力が行われた場合
	if (dashInputRight == 1)
	{
		/// フレームのカウントスタート
		dashExtensionFrameRight++;
		/// カウントフレームが猶予フレームを超えたら受け付けた入力をリセットする
		if (dashExtensionFrameRight > dashInputMaxFrame) {
			dashInputRight = 0;
			dashExtensionFrameRight = 0;
		}
	}
	/// ダッシュ入力が２回行われダッシュ状態になった場合
	if (dashInputRight == 2) {

		/// ダッシュ速度を加算
		transform.translate.x += velocity.x + dashSpeed.x;
		if (Input::GetInstance()->RereseKey(DIK_D)) {
			/// ダッシュ入力が解除されたらダッシュ入力をリセット
			dashInputRight = 0;
			dashExtensionFrameRight = 0;
		}
	}

}

void Player::MoveLeft()
{
	/// Aが押されたらダッシュ入力を増やす
	if (Input::GetInstance()->TriggerKey(DIK_A)) {
		dashInputLeft++;
	}
	/// プレイヤーの左への移動処理
	if (Input::GetInstance()->PushKey(DIK_A)) {
		transform.translate.x -= velocity.x;
	}

	/// ダッシュ入力が行われた場合
	if (dashInputLeft == 1)
	{
		/// フレームのカウントスタート
		dashExtensionFrameLeft++;
		/// カウントフレームが猶予フレームを超えたら受け付けた入力をリセットする
		if (dashExtensionFrameLeft > dashInputMaxFrame) {
			dashInputLeft = 0;
			dashExtensionFrameLeft = 0;
		}
	}
	/// ダッシュ入力が２回行われダッシュ状態になった場合
	if (dashInputLeft == 2) {
		transform.translate.x -= velocity.x + dashSpeed.x;
		if (Input::GetInstance()->RereseKey(DIK_A)) {
			/// ダッシュ入力が解除されたらダッシュ入力をリセット
			dashInputLeft = 0;
			dashExtensionFrameLeft = 0;
		}
	}
}


void Player::Jump()
{
	// プレイヤーのジャンプ処理

}

void Player::DrawImgui()
{
#ifdef _DEBUG
	// Imguiの描画処理
	ImGui::Begin("Player Settings");
	ImGui::Text("ExtensionFrameRight : %.f", dashExtensionFrameRight);
	ImGui::Text("DashInputRight : %d", dashInputRight);
	ImGui::Separator();
	ImGui::Text("ExtensionFrameLeft : %.f", dashExtensionFrameLeft);
	ImGui::Text("DashInputLeft : %d", dashInputLeft);

	ImGui::DragFloat3("Translate", &transform.translate.x, 0.1f);
	ImGui::DragFloat3("Scale", &transform.scale.x, 0.1f);
	ImGui::DragFloat3("Rotate", &transform.rotate.x, 0.1f);
	ImGui::Separator();
	ImGui::Text("Hit: %s", hitThisFrame ? "True" : "False"); 
	ImGui::End();
#endif // DEBUG
}

void Player::CheckBlockCollision(Map& map)
{
	// 実際に動いた量（Move() で更新済み）
    Vector3 delta = {
        transform.translate.x - prevTranslate.x,
        transform.translate.y - prevTranslate.y,
        transform.translate.z - prevTranslate.z
    };

    // 衝突解決は前フレーム位置から始めて、軸ごとに積む
    Vector3 pos = prevTranslate;
	auto aabbAt = [&](const Vector3& p) {
		Vector3 half = transform.scale * 0.5f;
		half.z = 1000.0f;
		return AABB{ p - half, p + half };
		};

    auto moveAxis = [&](int axis){
        float step = (axis==0?delta.x : axis==1?delta.y : delta.z);
        if(step == 0.0f) return;

        // 仮移動
        if(axis==0) pos.x += step;
        if(axis==1) pos.y += step;
        if(axis==2) pos.z += step;

        AABB nextAABB = aabbAt(pos);

        // 近傍ブロックだけ取得
        std::vector<Block*> nearbyBlocks = map.GetNearbyBlocks(nextAABB);

        for(const Block* b : nearbyBlocks){
            // 静的ブロックのみ
            if (b->GetType() != Collider::Type::Static) continue;

            AABB blk = b->GetAABB();
            // 交差してなければ次へ
            if(!(nextAABB.Intersects(blk))) continue;

            // この軸だけ押し戻す（面へ吸着）
            const Vector3 half = transform.scale * 0.5f;
            if(axis==0){
                pos.x = (step>0) ? (blk.min.x - half.x) : (blk.max.x + half.x);
                delta.x = 0.0f;
            }else if(axis==1){
                pos.y = (step>0) ? (blk.min.y - half.y) : (blk.max.y + half.y);
                delta.y = 0.0f;
            }else{
                pos.z = (step>0) ? (blk.min.z - half.z) : (blk.max.z + half.z);
                delta.z = 0.0f;
            }
            nextAABB = aabbAt(pos); // 連鎖衝突に備えて更新
        }
    };

    // X→Y→Zの順で安定
    moveAxis(0);
    moveAxis(1);
    moveAxis(2);

    // 確定位置を反映
    transform.translate = pos;
	playerModel->SetTransform(transform);

}

void Player::DrawHitImgui()
{
#ifdef _DEBUG
	ImGui::Begin("Hit Info");
	ImGui::Separator();
	ImGui::Text("Hit: %s", hitThisFrame ? "True" : "False");
	ImGui::End();
#endif // DEBUG
}


void Player::Finalize()
{
	//// 終了処理
	//EffectManager::GetInstance()->DeleteEffectGroup("ib");
	//qux.reset();
	//EffectManager::GetInstance()->DeleteEffectGroup("id");
	//quux.reset();
}