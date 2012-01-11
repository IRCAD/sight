/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <cppunit/extensions/HelperMacros.h>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/convenience.hpp>

#include <fwData/Color.hpp>
#include <fwData/StructureTraits.hpp>

#include "ObjectComparator.hpp"

using namespace fwData;

//------------------------------------------------------------------------------

void ObjectComparator::compareStructureTraitsDictionary(::fwData::StructureTraitsDictionary::sptr structureDico1, ::fwData::StructureTraitsDictionary::sptr structureDico2)
{
    CPPUNIT_ASSERT(structureDico1);
    CPPUNIT_ASSERT(structureDico2);
    std::vector<std::string> structureTypes1 = structureDico1->getStructureTypeNames();
    std::vector<std::string> structureTypes2 = structureDico2->getStructureTypeNames();
    CPPUNIT_ASSERT_EQUAL(structureTypes1.size(), structureTypes2.size());
    CPPUNIT_ASSERT(std::equal(structureTypes1.begin(), structureTypes1.end(), structureTypes2.begin()));

    BOOST_FOREACH(std::string type, structureTypes1)
    {
        ::fwData::StructureTraits::sptr structure1 = structureDico1->getStructure(type);
        ::fwData::StructureTraits::sptr structure2 = structureDico2->getStructure(type);
        ObjectComparator::compareStructureTraits(structure1, structure2);
    }
}

//------------------------------------------------------------------------------

void ObjectComparator::compareStructureTraits(::fwData::StructureTraits::sptr structure1, ::fwData::StructureTraits::sptr structure2)
{
    // check structure
    CPPUNIT_ASSERT(structure1);
    CPPUNIT_ASSERT(structure2);
    CPPUNIT_ASSERT_EQUAL(structure1->getType(), structure2->getType());
    CPPUNIT_ASSERT_EQUAL(structure1->getClass(), structure2->getClass());
    CPPUNIT_ASSERT_EQUAL(structure1->getNativeExp(), structure2->getNativeExp());
    CPPUNIT_ASSERT_EQUAL(structure1->getNativeGeometricExp(), structure2->getNativeGeometricExp());
    CPPUNIT_ASSERT_EQUAL(structure1->getAttachmentType(), structure2->getAttachmentType());

    ::fwData::Color::sptr color1 = structure1->getColor();
    ::fwData::Color::sptr color2 = structure2->getColor();
    CPPUNIT_ASSERT_DOUBLES_EQUAL(color1->red(), color2->red(), 0.000001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(color1->green(), color2->green(), 0.000001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(color1->blue(), color2->blue(), 0.000001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(color1->alpha(), color2->alpha(), 0.000001);

    ::fwData::StructureTraits::CategoryContainer categories1 = structure1->getCategories();
    ::fwData::StructureTraits::CategoryContainer categories2 = structure2->getCategories();
    CPPUNIT_ASSERT_EQUAL(categories1.size(), categories2.size());
    CPPUNIT_ASSERT(std::equal(categories1.begin(), categories1.end(), categories2.begin()));
}

//------------------------------------------------------------------------------

void ObjectComparator::compareROITraits(::fwData::ROITraits::sptr roi1, ::fwData::ROITraits::sptr roi2)
{
    CPPUNIT_ASSERT(roi1);
    CPPUNIT_ASSERT(roi2);
    CPPUNIT_ASSERT_EQUAL(roi1->getIdentifier(), roi2->getIdentifier());
    CPPUNIT_ASSERT_EQUAL(roi1->getEvaluatedExp(), roi2->getEvaluatedExp());
    ObjectComparator::compareStructureTraits(roi1->getStructureTraits(), roi2->getStructureTraits());

    CPPUNIT_ASSERT((roi1->getMaskOpNode() && roi2->getMaskOpNode()) || (!roi1->getMaskOpNode() && !roi2->getMaskOpNode()));
    if (roi1->getMaskOpNode() && roi2->getMaskOpNode())
    {
        ObjectComparator::compareNode(roi1->getMaskOpNode(), roi2->getMaskOpNode());
    }

}

//------------------------------------------------------------------------------

void ObjectComparator::compareReconstructionTraits(::fwData::ReconstructionTraits::sptr rec1, ::fwData::ReconstructionTraits::sptr rec2)
{
    CPPUNIT_ASSERT(rec1);
    CPPUNIT_ASSERT(rec2);
    CPPUNIT_ASSERT_EQUAL(rec1->getIdentifier(), rec2->getIdentifier());
    ObjectComparator::compareStructureTraits(rec1->getStructureTraits(), rec2->getStructureTraits());

    CPPUNIT_ASSERT((rec1->getMaskOpNode() && rec2->getMaskOpNode()) || (!rec1->getMaskOpNode() && !rec2->getMaskOpNode()));
    if (rec1->getMaskOpNode() && rec2->getMaskOpNode())
    {
        ObjectComparator::compareNode(rec1->getMaskOpNode(), rec2->getMaskOpNode());
    }

    CPPUNIT_ASSERT((rec1->getMeshOpNode() && rec2->getMeshOpNode()) || (!rec1->getMeshOpNode() && !rec2->getMeshOpNode()));
    if (rec1->getMeshOpNode() && rec2->getMeshOpNode())
    {
        ObjectComparator::compareNode(rec1->getMeshOpNode(), rec2->getMeshOpNode());
    }
}

//------------------------------------------------------------------------------

void ObjectComparator::compareNode(::fwData::Node::sptr node1, ::fwData::Node::sptr node2)
{
    CPPUNIT_ASSERT(node1);
    CPPUNIT_ASSERT(node2);
    CPPUNIT_ASSERT(node1->getObject() == node2->getObject());

    ::fwData::Node::PortContainer inputs1 = node1->getInputPorts();
    ::fwData::Node::PortContainer inputs2 = node2->getInputPorts();
    CPPUNIT_ASSERT_EQUAL(inputs1.size(), inputs2.size());
    ::fwData::Node::PortContainer::iterator inputsIter1 = inputs1.begin();
    ::fwData::Node::PortContainer::iterator inputsIter2 = inputs2.begin();
    for (;inputsIter1 != inputs1.end(); ++inputsIter1, ++inputsIter2)
    {
        ObjectComparator::comparePort(*inputsIter1, *inputsIter2);
    }

    ::fwData::Node::PortContainer outputs1 = node1->getOutputPorts();
    ::fwData::Node::PortContainer outputs2 = node2->getOutputPorts();
    CPPUNIT_ASSERT_EQUAL(outputs1.size(), outputs2.size());
    ::fwData::Node::PortContainer::iterator outputsIter1 = outputs1.begin();
    ::fwData::Node::PortContainer::iterator outputsIter2 = outputs2.begin();
    for (;outputsIter1 != outputs1.end(); ++outputsIter1, ++outputsIter2)
    {
        ObjectComparator::comparePort(*outputsIter1, *outputsIter2);
    }
}

//------------------------------------------------------------------------------

void ObjectComparator::comparePort(::fwData::Port::sptr port1, ::fwData::Port::sptr port2)
{
    CPPUNIT_ASSERT(port1);
    CPPUNIT_ASSERT(port2);
    CPPUNIT_ASSERT_EQUAL(port1->getIdentifier(), port2->getIdentifier());
    CPPUNIT_ASSERT_EQUAL(port1->getType(), port2->getType());
}

//------------------------------------------------------------------------------
