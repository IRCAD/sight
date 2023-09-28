/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
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

#include "data/config.hpp"

#include <data/Color.hpp>
#include <data/Material.hpp>
#include <data/Mesh.hpp>
#include <data/ModelSeries.hpp>
#include <data/Reconstruction.hpp>

#include <string>

namespace sight::data::tools
{

/**
 * @brief   This class contains helper to manipulate data::ModelSeries.
 */
class ModelSeries
{
public:

    /**
     * @brief Add a mesh into a model series. A reconstruction for this mesh will be created.
     * @param _modelSeries Model series where the mesh must be added.
     * @param _rec Reconstruction that must be added
     */
    DATA_API static void addReconstruction(
        const data::ModelSeries::sptr& _modelSeries,
        const data::Reconstruction::sptr& _rec
    );

    /**
     * @brief Create a reconstruction from a mesh.
     * @param _mesh Mesh that must be added
     * @param _organName Organ name to associate with the mesh
     * @param _structureType Structure type to associate with the mesh
     * @param _color Color of the material of the reconstruction.
     * @param _mode Representation mode of the material of the reconstruction.
     * @param _visible Visibility flag of the reconstruction.
     * @return Output reconstruction.
     */
    DATA_API static data::Reconstruction::sptr createReconstructionFromMesh(
        const data::Mesh::sptr& _mesh,
        const std::string& _organName,
        const std::string& _structureType,
        const data::Color::sptr& _color          = std::make_shared<data::Color>(),
        data::Material::RepresentationType _mode = data::Material::SURFACE,
        bool _visible                            = true
    );

    /**
     * @brief Add a mesh into a model series. A reconstruction for this mesh will be automatically created.
     * @param _modelSeries Model series where the mesh must be added.
     * @param _mesh Mesh that must be added
     * @param _organName Organ name to associate with the mesh
     * @param _structureType Structure type to associate with the mesh
     * @param _color Color of the material of the reconstruction.
     * @param _mode Representation mode of the material of the reconstruction.
     * @param _visible Visibility flag of the reconstruction.
     */
    DATA_API static void addMesh(
        const data::ModelSeries::sptr& _modelSeries,
        const data::Mesh::sptr& _mesh,
        const std::string& _organName,
        const std::string& _structureType,
        const data::Color::sptr& _color          = std::make_shared<data::Color>(),
        data::Material::RepresentationType _mode = data::Material::SURFACE,
        bool _visible                            = true
    );
};

} // end namespace sight::data::tools
