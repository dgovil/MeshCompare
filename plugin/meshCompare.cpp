//
// Created by Dhruv Govil on 2018-06-11.
//

#include "meshCompare.h"
#include <maya/MFnPlugin.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnMesh.h>
#include <maya/MColor.h>
#include <maya/MColorArray.h>
#include <maya/MIntArray.h>
#include <maya/MPointArray.h>
#include <maya/MPoint.h>
#include <maya/MItGeometry.h>
#include <math.h>
#include <maya/MGlobal.h>


// This is a registered node ID. Do not reuse for other nodes
// Reuse of this ID will cause a conflict.
MTypeId meshCompare::id(0x0012d5c0);
MObject meshCompare::target;
MObject meshCompare::clamp;
MObject meshCompare::saturation;


meshCompare::meshCompare() {};

meshCompare::~meshCompare() {};

void *meshCompare::creator() {
    return new meshCompare();
}

MStatus meshCompare::initialize() {
    MStatus status;
    MFnTypedAttribute tAttr;
    MFnNumericAttribute nAttr;

    target = tAttr.create("target", "target", MFnData::kMesh, &status);
    CHECK_MSTATUS_AND_RETURN_IT(status);
    status = addAttribute(target);
    CHECK_MSTATUS_AND_RETURN_IT(status);
    status = attributeAffects(target, meshCompare::outputGeom);
    CHECK_MSTATUS_AND_RETURN_IT(status);

    clamp = nAttr.create("clamp", "clamp", MFnNumericData::kFloat, 1, &status);
    CHECK_MSTATUS_AND_RETURN_IT(status);
    nAttr.setMin(0.01f);
    nAttr.setKeyable(true);
    status = addAttribute(clamp);
    CHECK_MSTATUS_AND_RETURN_IT(status);
    status = attributeAffects(clamp, meshCompare::outputGeom);
    CHECK_MSTATUS_AND_RETURN_IT(status);

    saturation = nAttr.create("saturation", "saturation", MFnNumericData::kFloat, 0.8, &status);
    CHECK_MSTATUS_AND_RETURN_IT(status);
    nAttr.setMin(0.0f);
    nAttr.setMax(1.0f);
    nAttr.setKeyable(true);
    status = addAttribute(saturation);
    CHECK_MSTATUS_AND_RETURN_IT(status);
    status = attributeAffects(saturation, meshCompare::outputGeom);
    CHECK_MSTATUS_AND_RETURN_IT(status);

    return MS::kSuccess;
}

MStatus meshCompare::deform(MDataBlock &block,
                            MItGeometry &iter,
                            const MMatrix &mat,
                            unsigned int multiIndex) {
    MStatus status;


    MArrayDataHandle inputHandle = block.outputArrayValue(meshCompare::input, &status);
    CHECK_MSTATUS_AND_RETURN_IT(status);

    status = inputHandle.jumpToElement(multiIndex);
    CHECK_MSTATUS_AND_RETURN_IT(status);

    MObject inputObj = inputHandle.outputValue().child(meshCompare::inputGeom).asMesh();
    MFnMesh inputMesh(inputObj, &status);
    CHECK_MSTATUS_AND_RETURN_IT(status);

    MDataHandle targetHandle = block.inputValue(target, &status);
    CHECK_MSTATUS_AND_RETURN_IT(status);
    MObject targetObj = targetHandle.asMesh();
    MFnMesh targetMesh(targetObj, &status);
    CHECK_MSTATUS_AND_RETURN_IT(status);

    MDataHandle clampHandle = block.inputValue(clamp, &status);
    CHECK_MSTATUS_AND_RETURN_IT(status);
    float clampValue = clampHandle.asFloat();

    MDataHandle saturationHandle = block.inputValue(saturation, &status);
    CHECK_MSTATUS_AND_RETURN_IT(status);
    float saturationValue = saturationHandle.asFloat();


    MColorArray colors;
    MIntArray indexes;
    MPointArray targetPoints;
    targetMesh.getPoints(targetPoints, MSpace::kObject);

    int numTargetPoints = targetPoints.length();

    for (; !iter.isDone(); iter.next()) {
        MPoint mpoint = iter.position();
        int idx = iter.index();
        if (idx >= numTargetPoints) {
            return MS::kFailure;
        }
        MPoint tpoint = targetPoints[idx];

        float dx = (mpoint.x - tpoint.x);
        float dy = (mpoint.y - tpoint.y);
        float dz = (mpoint.z - tpoint.z);

        float distance = sqrt((dx * dx) + (dy * dy) + (dz * dz));

        float hue = 180;
        float sat = 0.2;
        float val = 0.5;

        if (distance > 0.0f) {
            sat = saturationValue;
            val = 1;
            distance = (distance / clampValue);
            if (distance > 1.0) {
                distance = 1.0;
            }

            hue = 180.0f - ((360.0f * distance) / 2);

        }

        MColor color(MColor::MColorType::kHSV,hue, sat, val);

        colors.append(color);
        indexes.append(idx);
        inputMesh.setVertexColor(color, idx);

    }

    for (int x=0; x < colors.length(); x++) {
        auto color = colors[x];
        int idx = indexes[x];
        std::string msg = "Color is: " + std::to_string(color.r) + " " +  std::to_string(color.g) + " " + std::to_string(color.b);
        msg += "   for idx" + std::to_string(idx);
        MGlobal::displayInfo(msg.c_str());
    }


//    inputMesh.setVertexColors(colors, indexes);
    return MS::kSuccess;
}


MStatus initializePlugin(MObject obj) {
    MStatus status;

    MFnPlugin plugin(obj, "Dhruv Govil", "1.0", "Any", &status);
    CHECK_MSTATUS_AND_RETURN_IT(status);

    status = plugin.registerNode(
            "meshCompare",
            meshCompare::id,
            meshCompare::creator,
            meshCompare::initialize,
            MPxNode::kDeformerNode
    );

    CHECK_MSTATUS_AND_RETURN_IT(status)
    return MS::kSuccess;

}

MStatus uninitializePlugin(MObject obj) {
    MStatus status;
    MFnPlugin plugin(obj);

    status = plugin.deregisterNode(meshCompare::id);
    CHECK_MSTATUS_AND_RETURN_IT(status);

    return MS::kSuccess;
}