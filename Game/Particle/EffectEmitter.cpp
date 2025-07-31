#include "EffectEmitter.h"
EffectEmitter::EffectEmitter()
{
}

EffectEmitter::~EffectEmitter()
{

}

void EffectEmitter::Update()
{
	// EmitCylinder();
	//EmitRing();
}

void EffectEmitter::EmitCylinder() 
{
	EffectManager::GetInstance()->EmitCylinder(effectName, emitter.transform, 1);
}

void EffectEmitter::EmitRing()
{
	EffectManager::GetInstance()->EmitRing(effectName, emitter.transform, 2);
}
