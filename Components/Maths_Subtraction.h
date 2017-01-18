#pragma once

#include "IoComponentBase.h"

class URHO3D_API Maths_Subtraction : public IoComponentBase {

	URHO3D_OBJECT(Maths_Subtraction, IoComponentBase)

public:
	Maths_Subtraction(Urho3D::Context* context);

	static Urho3D::String iconTexture;

	void SolveInstance(
		const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
		Urho3D::Vector<Urho3D::Variant>& outSolveInstance
		);

	void AddInputSlots() = delete;
	void AddOutputSlots() = delete;
	void DeleteInputSlot(int index) = delete;
	void DeleteOututSlot(int index) = delete;

};