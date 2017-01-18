#include "Tree_Flatten.h"

#include <assert.h>

using namespace Urho3D;

String Tree_Flatten::iconTexture = "Textures/Icons/Tree_Flatten.png";

Tree_Flatten::Tree_Flatten(Context* context) :
	IoComponentBase(context, 1, 1)
{
	SetName("Flatten");
	SetFullName("Flatten Tree");
	SetDescription("Perform flattening on a tree");
	SetGroup(IoComponentGroup::TREE);
	SetSubgroup("Operations");

	inputSlots_[0]->SetName("IoDataTree");
	inputSlots_[0]->SetVariableName("T");
	inputSlots_[0]->SetDescription("IoDataTree to flatten");
	inputSlots_[0]->SetVariantType(VariantType::VAR_NONE);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	outputSlots_[0]->SetName("IoDataTree");
	outputSlots_[0]->SetVariableName("F");
	outputSlots_[0]->SetDescription("Flattened IoDataTree");
	outputSlots_[0]->SetVariantType(VariantType::VAR_NONE);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}


int Tree_Flatten::LocalSolve()
{
	assert(inputSlots_.Size() >= 1);
	assert(outputSlots_.Size() == 1);

	if (inputSlots_[0]->HasNoData()) {
		solvedFlag_ = 0;
		return 0;
	}

	SharedPtr<IoDataTree> inputIoDataTree = SharedPtr<IoDataTree>(new IoDataTree(inputSlots_[0]->GetIoDataTree()));

	outputSlots_[0]->SetIoDataTree(inputIoDataTree->Flatten());

	solvedFlag_ = 1;
	return 1;
}