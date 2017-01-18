#pragma once

#include "IoComponentBase.h"

class URHO3D_API Sets_LogisticGrowthSeries : public IoComponentBase {

	URHO3D_OBJECT(Sets_LogisticGrowthSeries, IoComponentBase)

public:
	Sets_LogisticGrowthSeries(Urho3D::Context* context);

	void SolveInstance(
		const Urho3D::Vector<Urho3D::Variant>& inSolveInstance,
		Urho3D::Vector<Urho3D::Variant>& outSolveInstance
		);

	void AddInputSlot() = delete;
	void AddOutputSlot() = delete;
	void DeleteInputSlot(int index) = delete;
	void DeleteOutputSlot(int index) = delete;

	static Urho3D::String iconTexture;
};