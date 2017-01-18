#include "Scene_AddNode.h"

#include "IoGraph.h"

#include "Geomlib_ConstructTransform.h"

using namespace Urho3D;

String Scene_AddNode::iconTexture = "Textures/Icons/Scene_AddNode.png";

Scene_AddNode::Scene_AddNode(Urho3D::Context* context) : IoComponentBase(context, 3, 1)
{
	SetName("Add Node");
	SetFullName("Add Node To Scene");
	SetDescription("Adds a node with optional name and parent");
	SetGroup(IoComponentGroup::SCENE);
	SetSubgroup("");

	inputSlots_[0]->SetName("Transform");
	inputSlots_[0]->SetVariableName("T");
	inputSlots_[0]->SetDescription("Transform of node");
	inputSlots_[0]->SetVariantType(VariantType::VAR_MATRIX3X4);
	inputSlots_[0]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[0]->SetDefaultValue(Matrix3x4::IDENTITY);
	inputSlots_[0]->DefaultSet();

	inputSlots_[1]->SetName("Name");
	inputSlots_[1]->SetVariableName("N");
	inputSlots_[1]->SetDescription("Optional node name");
	inputSlots_[1]->SetVariantType(VariantType::VAR_STRING);
	inputSlots_[1]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[1]->SetDefaultValue("NewGeneratedNode");
	inputSlots_[1]->DefaultSet();

	inputSlots_[2]->SetName("Parent");
	inputSlots_[2]->SetVariableName("P");
	inputSlots_[2]->SetDescription("Optional node parent");
	inputSlots_[2]->SetVariantType(VariantType::VAR_INT);
	inputSlots_[2]->SetDataAccess(DataAccess::ITEM);
	inputSlots_[2]->SetDefaultValue(-1);
	inputSlots_[2]->DefaultSet();


	outputSlots_[0]->SetName("Node ID");
	outputSlots_[0]->SetVariableName("ID");
	outputSlots_[0]->SetDescription("ID of node");
	outputSlots_[0]->SetVariantType(VariantType::VAR_INT);
	outputSlots_[0]->SetDataAccess(DataAccess::ITEM);
}

Scene_AddNode::~Scene_AddNode()
{
	PreLocalSolve();
}

void Scene_AddNode::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
)
{
	Scene* scene = (Scene*)GetContext()->GetGlobalVar("Scene").GetPtr();

	if (scene == NULL)
	{
		URHO3D_LOGERROR("Null scene encountered.");
		return;
	}


	//add new node
	String name = inSolveInstance[1].GetString();

	Node* newNode = scene->CreateChild(name);

	//set parent if applicable
	int parentID = inSolveInstance[2].GetInt();
	Node* parentNode = scene->GetNode(parentID);
	if (parentNode)
	{
		newNode->SetParent(parentNode);
	}

	// construct transform:
	Matrix3x4 xform = Geomlib::ConstructTransform(inSolveInstance[0]);

	//apply transform	
	newNode->SetPosition(xform.Translation());
	newNode->SetRotation(xform.Rotation());
	newNode->SetScale(xform.Scale());

	//track
	trackedNodes.Push(newNode->GetID());

	outSolveInstance[0] = newNode->GetID();
}

void Scene_AddNode::PreLocalSolve()
{
	//delete old nodes
	Scene* scene = GetSubsystem<IoGraph>()->scene;
	for (int i = 0; i < trackedNodes.Size(); i++)
	{
		Node* oldNode = scene->GetNode(trackedNodes[i]);
		if (oldNode)
		{
			oldNode->Remove();
		}

	}

	trackedNodes.Clear();
}

