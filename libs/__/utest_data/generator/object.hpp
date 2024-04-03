/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include <sight/utest_data/config.hpp>

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
    SIGHT_UTEST_DATA_API static data::color::sptr randomize_color();

    /// Creates a TransferFunction with random values and specified nbPoints, window and level
    SIGHT_UTEST_DATA_API static data::transfer_function::sptr create_tf_color(
        unsigned char _nb_points,
        double _window,
        double _level
    );

    /// Creates a TransferFunction with few values
    SIGHT_UTEST_DATA_API static data::transfer_function::sptr create_tf_color();

    /// Creates a StructureTraitsDictionary with many organs
    SIGHT_UTEST_DATA_API static data::structure_traits_dictionary::sptr create_structure_traits_dictionary();

    /// Creates a structure_traits representing the liver
    SIGHT_UTEST_DATA_API static data::structure_traits::sptr create_structure_traits();

    /// Creates a Material with ambient and diffuse color.
    SIGHT_UTEST_DATA_API static data::material::sptr create_material();

    /// Creates a ResectionDB containing one Resection.
    SIGHT_UTEST_DATA_API static data::resection_db::sptr generate_resection_db();

    /// Creates a Resection with 2 planes, 1 input Reconstruction and 1 output Reconstruction.
    SIGHT_UTEST_DATA_API static data::resection::sptr generate_resection();

    /// Creates a Point with random coordinates.
    SIGHT_UTEST_DATA_API static data::point::sptr generate_point();

    /// Creates a Plane with random points.
    SIGHT_UTEST_DATA_API static data::plane::sptr generate_plane();
};

} // namespace sight::utest_data::generator
