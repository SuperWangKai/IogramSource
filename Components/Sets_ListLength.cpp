//
// Copyright (c) 2016 - 2017 Mesh Consultants Inc.
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//


#include "Sets_ListLength.h"

#include <assert.h>

#include <iostream>

using namespace Urho3D;

String Sets_ListLength::iconTexture = "Textures/Icons/Sets_ListLength.png";

Sets_ListLength::Sets_ListLength(Context* context) :
	IoComponentBase(context, 1, 1)
{
	SetName("ListLength");
	SetFullName("List Length");
	SetDescription("Compute list lengths for tree nodes");
	SetGroup(IoComponentGroup::SETS);
	SetSubgroup("Operations");

	inputSlots_[0]->SetName("DataTree");
	inputSlots_[0]->SetVariableName("D");
	inputSlots_[0]->SetDescription("IoDataTree storing lists at nodes");
	inputSlots_[0]->SetVariantType(VariantType::VAR_NONE);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);

	outputSlots_[0]->SetName("LengthTree");
	outputSlots_[0]->SetVariableName("L");
	outputSlots_[0]->SetDescription("IoDataTree storing lengths at nodes");
	outputSlots_[0]->SetVariantType(VariantType::VAR_NONE);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}

// Always attempts to solve the node anew, based on current input values, regardless of value of solvedFlag_
// Returns:
//   1 if the node is successfully solved
//   0 otherwise
int Sets_ListLength::LocalSolve()
{
	if (inputSlots_[0]->HasNoData()) {
		solvedFlag_ = 0;
		return 0;
	}

	SharedPtr<IoDataTree> in_tree = SharedPtr<IoDataTree>(new IoDataTree(inputSlots_[0]->GetIoDataTree()));
	if (in_tree->GetNumBranches() == 0) {
		solvedFlag_ = 0;
		return 0;
	}

	//SharedPtr<IoDataTree> out_tree = SharedPtr<IoDataTree>(new IoDataTree(GetContext()));
	IoDataTree out_tree(GetContext());

	//Vector<int> cur_path = in_tree->Begin();
	Vector<int> cur_path = in_tree->Begin();
	int l = in_tree->GetNumItemsAtBranch(cur_path, DataAccess::ITEM);
	out_tree.Add(cur_path, Variant(l));

	for (int i = 1; i < in_tree->GetNumBranches(); ++i) {
		cur_path = in_tree->GetNextBranch();
		int l = in_tree->GetNumItemsAtBranch(cur_path, DataAccess::ITEM);
		out_tree.Add(cur_path, Variant(l));
	}

	outputSlots_[0]->SetIoDataTree(out_tree);

	solvedFlag_ = 1;
	return 1;
}