#pragma once

#include <Urho3D/Core/Variant.h>
#include "IoComponentBase.h"

class URHO3D_API Physics_PhysicsWorld : public IoComponentBase {

	URHO3D_OBJECT(Physics_PhysicsWorld, IoComponentBase)

public:
	Physics_PhysicsWorld(Urho3D::Context* context);

	static Urho3D::String iconTexture;

	virtual void PreLocalSolve();

	void SolveInstance(
		const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
		Urho3D::Vector<Urho3D::Variant>& outSolveInstance
		);

	Urho3D::Vector<int> trackedItems;

};