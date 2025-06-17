#pragma once
#include "engine/3d/Object3D.h"
#include "BlockType.h"
class Block
{

public:
	/// <summary>
	/// ����������
	/// </summary>
	void Initialize(BlockType blockType,Vector3 position);

	/// <summary>
	/// �X�V����
	/// </summary>
	void Update();

	/// <summary>
	/// �`�揈��
	/// </summary>
	void Draw();

private:
	/// �u���b�N���f��
	std::unique_ptr<Object3D> blockModel;
	/// �u���b�N�̈ʒu
	Transform transform;
	/// �\������u���b�N�̎��
	BlockType blockType;
};

