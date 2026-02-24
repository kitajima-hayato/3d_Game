#pragma once
class ITransitionEffect
{
public:
	/// <summary>
	/// 仮想デストラクタ
	/// </summary>
	virtual ~ITransitionEffect() = default;
	/// <summary>
	/// 遷移演出の開始
	/// </summary>
	virtual void Start() = 0;
	/// <summary>
	/// 更新処理
	/// </summary>
	virtual void Update(float deltaTime) = 0;
	/// <summary>
	/// 描画処理
	/// </summary>
	virtual void Draw() = 0;
	/// <summary>
	/// 演出が中間地点に到達したか
	/// </summary>
	virtual bool IsHalfway() = 0;
	/// <summary>
	///演出が完了したか
	/// </summary>
	virtual bool IsComplete() = 0;
	/// <summary>
	/// 演出の進行度を取得する
	/// </summary>
	/// <returns>(0.0 ~ 1.0)</returns>
	virtual float GetProgress() = 0;
};

