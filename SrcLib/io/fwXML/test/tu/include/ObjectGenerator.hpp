/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWXML_TEST_TU_OBJECT_GENERATOR_HPP_
#define _FWXML_TEST_TU_OBJECT_GENERATOR_HPP_

#include <vector>

#include <fwTools/Object.hpp>

#include <fwData/StructureTraits.hpp>
#include <fwData/ROITraits.hpp>
#include <fwData/ReconstructionTraits.hpp>
#include <fwData/StructureTraitsDictionary.hpp>
#include <fwData/Node.hpp>
#include <fwData/Port.hpp>
#include <fwData/Composite.hpp>

class ObjectGenerator
{
public:


    static ::fwData::StructureTraitsDictionary::sptr createStructureTraitsDictionary();
    static ::fwData::StructureTraits::sptr createStructureTraits();

    /// Return a composite containing the ROITraits and a reference on its StructureTraits and mask node (because of weak ptr)
    static ::fwData::Composite::sptr createROITraits();

    /// Return a composite containing the ReconstructionTraits and a reference on its StructureTraits and mask/mesh node (because of weak ptr)
    static ::fwData::Composite::sptr createReconstructionTraits();
    static ::fwData::Node::sptr createNode();
    static ::fwData::Port::sptr createPort();

};

#endif // _FWXML_TEST_TU_OBJECT_GENERATOR_HPP_
