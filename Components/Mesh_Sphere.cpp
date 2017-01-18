#include "Mesh_Sphere.h"

#include <assert.h>

#include <Urho3D/Core/Context.h>
#include <Urho3D/Core/Object.h>
#include <Urho3D/Core/Variant.h>
#include <Urho3D/Container/Vector.h>
#include <Urho3D/Container/Str.h>

#include "ConversionUtilities.h"
#include "StockGeometries.h"
#include "Geomlib_TransformVertexList.h"
#include "Geomlib_TriMeshThicken.h"
#include "TriMesh.h"

using namespace Urho3D;

String Mesh_Sphere::iconTexture = "Textures/Icons/Mesh_Sphere.png";

Mesh_Sphere::Mesh_Sphere(Context* context) : IoComponentBase(context, 1, 1)
{
	SetName("Sphere");
	SetFullName("ConstructSphere");
	SetDescription("Construct a sphere mesh from scale");
	SetGroup(IoComponentGroup::MESH);
	SetSubgroup("Primitive");

	inputSlots_[0]->SetName("Transformation");
	inputSlots_[0]->SetVariableName("T");
	inputSlots_[0]->SetDescription("Transformation to apply to cube");
	inputSlots_[0]->SetVariantType(VariantType::VAR_MATRIX3X4);
	inputSlots_[0]->SetDefaultValue(Matrix3x4::IDENTITY);
	inputSlots_[0]->DefaultSet();

	outputSlots_[0]->SetName("Mesh");
	outputSlots_[0]->SetVariableName("M");
	outputSlots_[0]->SetDescription("Icosahedron Mesh");
	outputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}

void Mesh_Sphere::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	assert(inSolveInstance.Size() == inputSlots_.Size());
	assert(outSolveInstance.Size() == outputSlots_.Size());

	///////////////////
	// VERIFY & EXTRACT

	// Verify input slot 1
	VariantType type1 = inSolveInstance[0].GetType();
	if (type1 != VariantType::VAR_MATRIX3X4) {
		URHO3D_LOGWARNING("T must be a valid transform.");
		outSolveInstance[0] = Variant();
		return;
	}
	Matrix3x4 tr = inSolveInstance[0].GetMatrix3x4();

	///////////////////
	// COMPONENT'S WORK

	Variant baseSphere = MakeSphere();
	Variant sphere = TriMesh_ApplyTransform(baseSphere, tr);

	/////////////////
	// ASSIGN OUTPUTS

	outSolveInstance[0] = sphere;
}