#include "EffectEmitter.h"
EffectEmitter::EffectEmitter()
{
}

EffectEmitter::~EffectEmitter()
{

}

void EffectEmitter::Update()
{
	Emit();
}

void EffectEmitter::Emit() 
{
	EffectManager::GetInstance()->Emit(effectName, emitter.transform, 1);
}
