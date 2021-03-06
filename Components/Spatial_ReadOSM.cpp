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


#include "Spatial_ReadOSM.h"

#include <assert.h>

#include <Urho3D/Math/Vector3.h>
#include <Urho3D/IO/FileSystem.h>
#include <Urho3D/Resource/ResourceCache.h>

#include <Eigen/Core>
#include <Urho3D/Resource/XMLFile.h>
#include "Geomlib_GeoConversions.h"
#include "Polyline.h"

using namespace Urho3D;

String Spatial_ReadOSM::iconTexture = "Textures/Icons/Spatial_ReadOSM.png";

Spatial_ReadOSM::Spatial_ReadOSM(Context* context) : IoComponentBase(context, 3, 3)
{
	SetName("ReadOSM");
	SetFullName("Read OSM File");
	SetDescription("Reads an OSM file");
	//SetGroup(IoComponentGroup::VECTOR);
	//SetSubgroup("Point");

	inputSlots_[0]->SetName("OSMFile");
	inputSlots_[0]->SetVariableName("P");
	inputSlots_[0]->SetDescription("Path to OSM File (XML)");
	inputSlots_[0]->SetVariantType(VariantType::VAR_STRING);
	inputSlots_[0]->SetDefaultValue("Scripts/map.osm");
	inputSlots_[0]->DefaultSet();

	inputSlots_[1]->SetName("Scale");
	inputSlots_[1]->SetVariableName("S");
	inputSlots_[1]->SetDescription("Scale");
	inputSlots_[1]->SetVariantType(VariantType::VAR_FLOAT);
	inputSlots_[1]->SetDefaultValue(0.0001f);
	inputSlots_[1]->DefaultSet();

	inputSlots_[2]->SetName("Options");
	inputSlots_[2]->SetVariableName("O");
	inputSlots_[2]->SetDescription("OSM Options");
	inputSlots_[2]->SetVariantType(VariantType::VAR_STRING);
	inputSlots_[2]->DefaultSet();

	outputSlots_[0]->SetName("Ways");
	outputSlots_[0]->SetVariableName("W");
	outputSlots_[0]->SetDescription("Ways");
	outputSlots_[0]->SetDataAccess(DataAccess::LIST);
	outputSlots_[0]->SetVariantType(VariantType::VAR_VARIANTMAP);

	outputSlots_[1]->SetName("Buildings");
	outputSlots_[1]->SetVariableName("B");
	outputSlots_[1]->SetDescription("Buildings");
	outputSlots_[1]->SetDataAccess(DataAccess::LIST);
	outputSlots_[1]->SetVariantType(VariantType::VAR_VARIANTMAP);

	outputSlots_[2]->SetName("BuildingHeight");
	outputSlots_[2]->SetVariableName("BH");
	outputSlots_[2]->SetDescription("Building Height");
	outputSlots_[2]->SetDataAccess(DataAccess::LIST);
	outputSlots_[2]->SetVariantType(VariantType::VAR_FLOAT);
}

void Spatial_ReadOSM::SolveInstance(
	const Vector<Variant>& inSolveInstance,
	Vector<Variant>& outSolveInstance
	)
{
	ResourceCache* rc = GetSubsystem<ResourceCache>();
	FileSystem* fs = GetSubsystem<FileSystem>();

	String path = inSolveInstance[0].GetString();
	float scale = inSolveInstance[1].GetFloat();
	XMLFile* xml = rc->GetResource<XMLFile>(path);
	if (xml)
	{
		XMLElement root = xml->GetRoot();

		XMLElement bounds = root.GetChild("bounds");
		double latMin, latMax, lonMin, lonMax;
		latMin = bounds.GetDouble("minlat");
		latMax = bounds.GetDouble("maxlat");
		lonMin = bounds.GetDouble("minlon");
		lonMax = bounds.GetDouble("maxlon");

		Vector3 geoCenter((latMax + latMin) / 2, (lonMax + lonMin) / 2, 0);

		//read in the node list and convert to XYZ
		HashMap<String, Vector3> nodePts;
		XMLElement currNode = root.GetChild("node");
		double lat, lon;
		String id;
		while (currNode)
		{
			lat = currNode.GetDouble("lat");
			lon = currNode.GetDouble("lon");
			id = currNode.GetAttribute("id");

			//convert to XYZ
			Vector3 pt = Geomlib::GeoToXYZ(
				geoCenter.x_,
				geoCenter.y_,
				lat,
				lon
				);

			pt *= scale;

			nodePts[id] = pt;
			currNode = currNode.GetNext("node");
		}

		//now create the ways
		XMLElement currWay = root.GetChild("way");
		VariantVector waysOut;
		VariantVector buildingsOut;
		VariantVector heightsOut;
		while (currWay)
		{
			int type = -1;

			//get the tags
			Vector<Pair<String, String>> tags;
			XMLElement currTag = currWay.GetChild("tag");
			float currHeight = 0.0f;
			while (currTag)
			{
				String k = currTag.GetAttribute("k");
				String v = currTag.GetAttribute("v");
				Pair<String, String> tagKV;
				tags.Push(tagKV);

				if (k == "building")
					type = 2;
				else if (k == "highway" )
					type = 1;

				if (k == "height" || k == "max_height" || k == "building:height")
				{
					currHeight = scale * Abs(ToFloat(v));
				}

				currTag = currTag.GetNext("tag");
			}

			//only let through roads and buildings for now
			if (type == -1)
			{
				currWay = currWay.GetNext("way");
				continue;
			}			

			//proceed with extraction
			VariantVector wayPts;
			VariantMap wayPolyline;
			XMLElement wayNode = currWay.GetChild("nd");
			while (wayNode)
			{
				String id = wayNode.GetAttribute("ref");
				Vector3 pt = nodePts[id];
				wayPts.Push(pt);
				wayNode = wayNode.GetNext("nd");
			}

			//sort
			//VariantMap poly;
			//poly["vertices"] = wayPts;
			if (type == 1)
				waysOut.Push(Polyline_Make(wayPts));
			else if (type == 2)
			{
				//get height
				heightsOut.Push(currHeight);				
				buildingsOut.Push(Polyline_Make(wayPts));
			}


			//iterate
			currWay = currWay.GetNext("way");
		}

		outSolveInstance[0] = waysOut;
		outSolveInstance[1] = buildingsOut;
		outSolveInstance[2] = heightsOut;
	}
	else
	{
		SetAllOutputsNull(outSolveInstance);
		return;
	}
}