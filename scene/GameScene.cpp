#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include "AxisIndicator.h"
#include "PrimitiveDrawer.h"

using namespace MathUtility;

GameScene::GameScene() {}

GameScene::~GameScene() {
	delete model_;
	delete debugCamera_;
}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	debugText_ = DebugText::GetInstance();

	textureHandle_ = TextureManager::Load("mario.jpg");
	model_ = Model::Create();


	worldTransform_.Initialize();
	viewProjection_.Initialize();
	debugCamera_ = new DebugCamera(1280, 720);

	//軸方向表示の表示を有効にする
	AxisIndicator::GetInstance()->SetVisible(true);
	//軸方向表示が参照するビュープロジェクションを指定する
	AxisIndicator::GetInstance()->SetTargetViewProjection(&debugCamera_->GetViewProjection());
	
	//ライン描画が参照するビュープロジェクションを指定する
	PrimitiveDrawer::GetInstance()->SetViewProjection(&debugCamera_->GetViewProjection());
	//
	worldTransform_.scale_ = {3,1,1};
	//
	Matrix4 matScale_;
	//
	matScale_ = MathUtility::Matrix4Identity();
	matScale_.m[0][0] = worldTransform_.scale_.x;
	matScale_.m[1][1] = worldTransform_.scale_.y;
	matScale_.m[2][2] = worldTransform_.scale_.z;

	worldTransform_.matWorld_ = MathUtility::Matrix4Identity();
	worldTransform_.matWorld_*= matScale_;
	worldTransform_.TransferMatrix();

	

	//XYZ軸
	worldTransform_.rotation_ = { 0.0f,(45.0f),1.0f };
	
	Matrix4 matRot;

	Matrix4 matRotX, matRotY, matRotZ;

	matRotZ = MathUtility::Matrix4Identity();
	matRotZ.m[0][0] = cos(worldTransform_.rotation_.z);
	matRotZ.m[0][1] = sin(worldTransform_.rotation_.z);
	matRotZ.m[1][0] = -sin(worldTransform_.rotation_.z);
	matRotZ.m[1][1] = cos(worldTransform_.rotation_.z);

	matRotX = MathUtility::Matrix4Identity();
	matRotX.m[1][1] = cos(worldTransform_.rotation_.x);
	matRotX.m[1][2] = sin(worldTransform_.rotation_.x);
	matRotX.m[2][1] = -sin(worldTransform_.rotation_.x);
	matRotX.m[2][2] = cos(worldTransform_.rotation_.x);

	matRotY = MathUtility::Matrix4Identity();
	matRotY.m[0][0] = cos(worldTransform_.rotation_.y);
	matRotY.m[0][2] = sin(worldTransform_.rotation_.y);
	matRotY.m[2][0] = -sin(worldTransform_.rotation_.y);
	matRotY.m[2][2] = cos(worldTransform_.rotation_.y);

	matRot = matRotZ * matRotX * matRotY;

	worldTransform_.matWorld_ = MathUtility::Matrix4Identity();
	worldTransform_.matWorld_ *= matRot;
	worldTransform_.TransferMatrix();

	worldTransform_.translation_ = { 0,10,0 };

	Matrix4 matTrans = MathUtility::Matrix4Identity();

	matTrans.m[3][0] = worldTransform_.translation_.x;
	matTrans.m[3][1] = worldTransform_.translation_.y;
	matTrans.m[3][2] = worldTransform_.translation_.z;

	worldTransform_.matWorld_ = MathUtility::Matrix4Identity();
	worldTransform_.matWorld_ *= matTrans;
	worldTransform_.TransferMatrix();

}

void GameScene::Update() {
	debugCamera_->Update();

}

void GameScene::Draw() {


	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();

	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);
	
	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>
	//
	model_->Draw(worldTransform_, debugCamera_->GetViewProjection(), textureHandle_);

	//ライン描画が参照するビュープロジェクションを指定する
	for (float i = 0; i < 10; i++) {
		PrimitiveDrawer::GetInstance()->DrawLine3d({ 0,i,0 }, { 10,i,0 }, { 1,0,0,1 });
		PrimitiveDrawer::GetInstance()->DrawLine3d({ i,0,0 }, { i,10,0 }, { 1,0,0,1 });
	}
	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion
	
#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);
	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	// デバッグテキストの描画
	debugText_->DrawAll(commandList);
	//
	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}
