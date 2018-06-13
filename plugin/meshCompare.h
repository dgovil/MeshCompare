//
// Created by Dhruv Govil on 2018-06-11.
//

#ifndef PROJECT_MESHCOMPARE_H
#define PROJECT_MESHCOMPARE_H

#include <maya/MPxDeformerNode.h>


class meshCompare : public MPxDeformerNode
{
public:
    meshCompare() ;
    ~meshCompare() override ;

    MStatus deform(MDataBlock &block, MItGeometry &iter, const MMatrix &mat, unsigned int multiIndex) override ;

    static MTypeId id;
    static void* creator();
    static MStatus initialize();

    // Define plugs
    static MObject target;
    static MObject clamp;
    static MObject saturation;

    void postConstructor() override;
};

#endif //PROJECT_MESHCOMPARE_H
