/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <cmath>

#include <boost/assign/std/vector.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/convenience.hpp>

#include "ObjectGenerator.hpp"

using namespace boost::assign;

//------------------------------------------------------------------------------

::fwData::StructureTraits::sptr ObjectGenerator::createStructureTraits()
{
    ::fwData::StructureTraits::NewSptr pStructureTraits;

    const std::string type = "Liver";
    const ::fwData::StructureTraits::StructureClass structClass = ::fwData::StructureTraits::ORGAN;
    const ::fwData::StructureTraits::ROIExpression nativeExp = "inter(world(type(Skin)),not(class(Organ)))";
    const ::fwData::StructureTraits::ROIExpression nativeGeoExp = "halfSpace(world(Left_lung),Z+,true)";

    pStructureTraits->setType(type);
    pStructureTraits->setClass(structClass);
    pStructureTraits->setNativeExp(nativeExp);
    pStructureTraits->setNativeGeometricExp(nativeGeoExp);

    ::fwData::Color::NewSptr color;
    color->setRGBA(rand()%100 / 100., rand()%100 / 100., rand()%100 / 100., rand()%100 / 100.);
    pStructureTraits->setColor(color);

    ::fwData::StructureTraits::CategoryContainer categories(2);
    categories[0] = ::fwData::StructureTraits::ABDOMEN;
    categories[1] = ::fwData::StructureTraits::BODY;
    pStructureTraits->setCRefCategories(categories);

    return pStructureTraits;
}

//------------------------------------------------------------------------------

::fwData::Composite::sptr ObjectGenerator::createROITraits()
{
    ::fwData::Composite::NewSptr composite;

    ::fwData::ROITraits::NewSptr roi;
    std::stringstream ss;
    ss << "ID_" << rand()%100;
    roi->setIdentifier(ss.str());

    roi->setEvaluatedExp("inter(skin),not(liver)");
    ::fwData::StructureTraits::sptr structure = ObjectGenerator::createStructureTraits();
    roi->setStructureTraits(structure);

    ::fwData::Node::sptr maskNode = ObjectGenerator::createNode();
    roi->setMaskOpNode(maskNode);

    (*composite)["ROITraits"] = roi;
    (*composite)["StructureTraits"] = structure;
    (*composite)["MaskOpNode"] = maskNode;

    return composite;
}

//------------------------------------------------------------------------------

::fwData::Composite::sptr ObjectGenerator::createReconstructionTraits()
{
    ::fwData::Composite::NewSptr composite;

    ::fwData::ReconstructionTraits::NewSptr rec;

    std::stringstream ss;
    ss << "ID_" << rand()%100;
    rec->setIdentifier(ss.str());

    ::fwData::StructureTraits::sptr structure = ObjectGenerator::createStructureTraits();
    rec->setStructureTraits(structure);

    ::fwData::Node::sptr maskNode = ObjectGenerator::createNode();
    rec->setMaskOpNode(maskNode);

    ::fwData::Node::sptr meshNode = ObjectGenerator::createNode();
    rec->setMeshOpNode(meshNode);

    (*composite)["ReconstructionTraits"] = rec;
    (*composite)["StructureTraits"] = structure;
    (*composite)["MaskOpNode"] = maskNode;
    (*composite)["MeshOpNode"] = meshNode;

    return composite;
}

//------------------------------------------------------------------------------

::fwData::Node::sptr ObjectGenerator::createNode()
{
    ::fwData::Node::NewSptr node;

    unsigned int nbInputs = rand()%5;
    unsigned int nbOutputs = rand()%5;

    for (unsigned int i=0 ; i<nbInputs ; ++i)
    {
        node->addInputPort(ObjectGenerator::createPort());
    }

    for (unsigned int i=0 ; i<nbOutputs ; ++i)
    {
        node->addOutputPort(ObjectGenerator::createPort());
    }

    return node;
}

//------------------------------------------------------------------------------

::fwData::Port::sptr ObjectGenerator::createPort()
{
    ::fwData::Port::NewSptr port;

    std::stringstream ssId;
    ssId << "ID_" << rand()%100;
    port->setIdentifier(ssId.str());

    std::stringstream ssType;
    ssType << "Type_" << rand()%100;
    port->setType(ssType.str());

    return port;
}

//------------------------------------------------------------------------------
