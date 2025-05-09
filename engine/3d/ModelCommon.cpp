#include "ModelCommon.h"

ModelCommon* ModelCommon::GetInstance()
{
	static ModelCommon instance;
	return &instance;
}



void ModelCommon::Initialize(DirectXCommon* dxCommon)
{
	//引数で受け取ってメンバ変数に記録する
	this->dxCommon_ = dxCommon;
}
