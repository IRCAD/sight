/************************************************************************
 *
 * Copyright (C) 2014-2024 IRCAD France
 * Copyright (C) 2014-2019 IHU Strasbourg
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

#include <sight/data/config.hpp>

#include <data/color.hpp>
#include <data/material.hpp>
#include <data/mesh.hpp>
#include <data/model_series.hpp>
#include <data/reconstruction.hpp>

#include <string>

namespace sight::data::tools
{

/**
 * @brief   This class contains helper to manipulate data::model_series.
 */
class model_series
{
public:

    /**
     * @brief Add a mesh into a model series. A reconstruction for this mesh will be created.
     * @param _model_series Model series where the mesh must be added.
     * @param _rec Reconstruction that must be added
     */
    SIGHT_DATA_API static void add_reconstruction(
        const data::model_series::sptr& _model_series,
        const data::reconstruction::sptr& _rec
    );

    /**
     * @brief Create a reconstruction from a mesh.
     * @param _mesh Mesh that must be added
     * @param _organ_name Organ name to associate with the mesh
     * @param _structure_type Structure type to associate with the mesh
     * @param _color Color of the material of the reconstruction.
     * @param _mode Representation mode of the material of the reconstruction.
     * @param _visible Visibility flag of the reconstruction.
     * @return Output reconstruction.
     */
    SIGHT_DATA_API static data::reconstruction::sptr create_reconstruction_from_mesh(
        const data::mesh::sptr& _mesh,
        const std::string& _organ_name,
        const std::string& _structure_type,
        const data::color::sptr& _color        = std::make_shared<data::color>(),
        data::material::representation_t _mode = data::material::surface,
        bool _visible                          = true
    );

    /**
     * @brief Add a mesh into a model series. A reconstruction for this mesh will be automatically created.
     * @param _model_series Model series where the mesh must be added.
     * @param _mesh Mesh that must be added
     * @param _organ_name Organ name to associate with the mesh
     * @param _structure_type Structure type to associate with the mesh
     * @param _color Color of the material of the reconstruction.
     * @param _mode Representation mode of the material of the reconstruction.
     * @param _visible Visibility flag of the reconstruction.
     */
    SIGHT_DATA_API static void add_mesh(
        const data::model_series::sptr& _model_series,
        const data::mesh::sptr& _mesh,
        const std::string& _organ_name,
        const std::string& _structure_type,
        const data::color::sptr& _color        = std::make_shared<data::color>(),
        data::material::representation_t _mode = data::material::surface,
        bool _visible                          = true
    );
};

} // end namespace sight::data::tools
