/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWXML_TEST_TU_OBJECT_COMPARATOR_HPP_
#define _FWXML_TEST_TU_OBJECT_COMPARATOR_HPP_

#include <fwData/StructureTraits.hpp>
#include <fwData/ROITraits.hpp>
#include <fwData/ReconstructionTraits.hpp>
#include <fwData/Node.hpp>
#include <fwData/Port.hpp>

class ObjectComparator
{
public:
    static void compareStructureTraits(::fwData::StructureTraits::sptr structure1, ::fwData::StructureTraits::sptr structure2);
    static void compareROITraits(::fwData::ROITraits::sptr roi1, ::fwData::ROITraits::sptr roi2);
    static void compareReconstructionTraits(::fwData::ReconstructionTraits::sptr rec1, ::fwData::ReconstructionTraits::sptr rec2);
    static void compareNode(::fwData::Node::sptr node1, ::fwData::Node::sptr node2);
    static void comparePort(::fwData::Port::sptr port1, ::fwData::Port::sptr port2);

};

#endif // _FWXML_TEST_TU_OBJECT_COMPARATOR_HPP_
