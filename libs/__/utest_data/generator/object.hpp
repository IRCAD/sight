/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "utest_data/config.hpp"

#include <core/base.hpp>

#include <data/array.hpp>
#include <data/color.hpp>
#include <data/composite.hpp>
#include <data/material.hpp>
#include <data/plane.hpp>
#include <data/point.hpp>
#include <data/resection.hpp>
#include <data/resection_db.hpp>
#include <data/structure_traits.hpp>
#include <data/structure_traits_dictionary.hpp>
#include <data/transfer_function.hpp>

namespace sight::utest_data::generator
{

/**
 * @brief   This class contains helper to generate data objects.
 */
class object
{
public:

    /// Creates a color with random values
    UTEST_DATA_API static data::color::sptr randomizeColor();

    /// Creates a TransferFunction with random values and specified nbPoints, window and level
    UTEST_DATA_API static data::transfer_function::sptr createTFColor(
        unsigned char _nb_points,
        double _window,
        double _level
    );

    /// Creates a TransferFunction with few values
    UTEST_DATA_API static data::transfer_function::sptr createTFColor();

    /// Creates a StructureTraitsDictionary with many organs
    UTEST_DATA_API static data::structure_traits_dictionary::sptr createStructureTraitsDictionary();

    /// Creates a structure_traits representing the liver
    UTEST_DATA_API static data::structure_traits::sptr createStructureTraits();

    /// Creates a Material with ambient and diffuse color.
    UTEST_DATA_API static data::material::sptr createMaterial();

    /// Creates a ResectionDB containing one Resection.
    UTEST_DATA_API static data::resection_db::sptr generateResectionDB();

    /// Creates a Resection with 2 planes, 1 input Reconstruction and 1 output Reconstruction.
    UTEST_DATA_API static data::resection::sptr generateResection();

    /// Creates a Point with random coordinates.
    UTEST_DATA_API static data::point::sptr generatePoint();

    /// Creates a Plane with random points.
    UTEST_DATA_API static data::plane::sptr generatePlane();
};

} // namespace sight::utest_data::generator
