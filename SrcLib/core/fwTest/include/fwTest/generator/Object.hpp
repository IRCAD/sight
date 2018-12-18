/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#ifndef __FWTEST_GENERATOR_OBJECT_HPP__
#define __FWTEST_GENERATOR_OBJECT_HPP__

#include "fwTest/config.hpp"

#include <fwCore/base.hpp>

#include <fwData/Array.hpp>
#include <fwData/Color.hpp>
#include <fwData/Composite.hpp>
#include <fwData/Material.hpp>
#include <fwData/Node.hpp>
#include <fwData/Plane.hpp>
#include <fwData/Point.hpp>
#include <fwData/Port.hpp>
#include <fwData/ProcessObject.hpp>
#include <fwData/ReconstructionTraits.hpp>
#include <fwData/Resection.hpp>
#include <fwData/ResectionDB.hpp>
#include <fwData/ROITraits.hpp>
#include <fwData/StructureTraits.hpp>
#include <fwData/StructureTraitsDictionary.hpp>
#include <fwData/TransferFunction.hpp>


namespace fwTest
{
namespace generator
{
/**
 * @brief   This class contains helper to generate data objects.
 * @class   Object
 */
class Object
{
public:

    /// Creates a color with random values
    FWTEST_API static ::fwData::Color::sptr randomizeColor();

    /// Creates a TransferFunction with random values and specified nbPoints, window and level
    FWTEST_API static ::fwData::TransferFunction::sptr createTFColor(unsigned char nbPoints,
                                                                     double window,
                                                                     double level );

    /// Creates a TransferFunction with few values
    FWTEST_API static ::fwData::TransferFunction::sptr createTFColor();

    /// Creates a StructureTraitsDictionary with many organs
    FWTEST_API static ::fwData::StructureTraitsDictionary::sptr createStructureTraitsDictionary();

    /// Creates a StructureTraits representing the liver
    FWTEST_API static ::fwData::StructureTraits::sptr createStructureTraits();

    /// Return a composite containing the ROITraits and a reference on its StructureTraits and mask node (because of weak ptr)
    FWTEST_API static ::fwData::Composite::sptr createROITraits();

    /// Return a composite containing the ReconstructionTraits and a reference on its StructureTraits and mask/mesh node (because of weak ptr)
    FWTEST_API static ::fwData::Composite::sptr createReconstructionTraits();

    /// Creates a Node with random ports
    FWTEST_API static ::fwData::Node::sptr createNode();

    /// Creates a Port with random identifier and type
    FWTEST_API static ::fwData::Port::sptr createPort();

    /// Creates a Material with ambient and diffuse color.
    FWTEST_API static ::fwData::Material::sptr createMaterial( );

    /// Creates a ProcessObject with 3 inputs and 1 output
    FWTEST_API static ::fwData::ProcessObject::sptr createProcessObject();

    /// Creates a ResectionDB containing one Resection.
    FWTEST_API static ::fwData::ResectionDB::sptr generateResectionDB();

    /// Creates a Resection with 2 planes, 1 input Reconstruction and 1 output Reconstruction.
    FWTEST_API static ::fwData::Resection::sptr generateResection();

    /// Creates a Point with random coordinates.
    FWTEST_API static ::fwData::Point::sptr generatePoint();

    /// Creates a Plane with random points.
    FWTEST_API static ::fwData::Plane::sptr generatePlane();
};

} // namespace generator
} // namespace fwTest


#endif // __FWTEST_GENERATOR_OBJECT_HPP__
