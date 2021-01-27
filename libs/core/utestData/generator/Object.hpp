/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#pragma once

#include "utestData/config.hpp"

#include <core/base.hpp>

#include <data/Array.hpp>
#include <data/Color.hpp>
#include <data/Composite.hpp>
#include <data/Material.hpp>
#include <data/Node.hpp>
#include <data/Plane.hpp>
#include <data/Point.hpp>
#include <data/Port.hpp>
#include <data/ProcessObject.hpp>
#include <data/ReconstructionTraits.hpp>
#include <data/Resection.hpp>
#include <data/ResectionDB.hpp>
#include <data/ROITraits.hpp>
#include <data/StructureTraits.hpp>
#include <data/StructureTraitsDictionary.hpp>
#include <data/TransferFunction.hpp>

namespace sight::utestData
{
namespace generator
{
/**
 * @brief   This class contains helper to generate data objects.
 */
class Object
{
public:

    /// Creates a color with random values
    UTESTDATA_API static data::Color::sptr randomizeColor();

    /// Creates a TransferFunction with random values and specified nbPoints, window and level
    UTESTDATA_API static data::TransferFunction::sptr createTFColor(unsigned char nbPoints,
                                                                    double window,
                                                                    double level );

    /// Creates a TransferFunction with few values
    UTESTDATA_API static data::TransferFunction::sptr createTFColor();

    /// Creates a StructureTraitsDictionary with many organs
    UTESTDATA_API static data::StructureTraitsDictionary::sptr createStructureTraitsDictionary();

    /// Creates a StructureTraits representing the liver
    UTESTDATA_API static data::StructureTraits::sptr createStructureTraits();

    /// Return a composite containing the ROITraits and a reference on its StructureTraits and mask node (because of
    // weak ptr)
    UTESTDATA_API static data::Composite::sptr createROITraits();

    /// Return a composite containing the ReconstructionTraits and a reference on its StructureTraits and mask/mesh node
    // (because of weak ptr)
    UTESTDATA_API static data::Composite::sptr createReconstructionTraits();

    /// Creates a Node with random ports
    UTESTDATA_API static data::Node::sptr createNode();

    /// Creates a Port with random identifier and type
    UTESTDATA_API static data::Port::sptr createPort();

    /// Creates a Material with ambient and diffuse color.
    UTESTDATA_API static data::Material::sptr createMaterial( );

    /// Creates a ProcessObject with 3 inputs and 1 output
    UTESTDATA_API static data::ProcessObject::sptr createProcessObject();

    /// Creates a ResectionDB containing one Resection.
    UTESTDATA_API static data::ResectionDB::sptr generateResectionDB();

    /// Creates a Resection with 2 planes, 1 input Reconstruction and 1 output Reconstruction.
    UTESTDATA_API static data::Resection::sptr generateResection();

    /// Creates a Point with random coordinates.
    UTESTDATA_API static data::Point::sptr generatePoint();

    /// Creates a Plane with random points.
    UTESTDATA_API static data::Plane::sptr generatePlane();
};

} // namespace generator
} // namespace sight::utestData
