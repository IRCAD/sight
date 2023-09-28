/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "Tuto13MeshGeneratorCpp/config.hpp"

#include <core/tools/failed.hpp>

#include <data/Array.hpp>
#include <data/Mesh.hpp>

namespace Tuto13MeshGeneratorCpp
{

class TUTO13MESHGENERATORCPP_CLASS_API AlgoMeshDeformation final
{
public:

    /// Creates an instance.
    TUTO13MESHGENERATORCPP_API AlgoMeshDeformation() noexcept;

    /// Destroys the instance.
    TUTO13MESHGENERATORCPP_API ~AlgoMeshDeformation() noexcept;

    /// Computes the deformation or sets and initializes the algorithm if necessary.
    TUTO13MESHGENERATORCPP_API void computeDeformation(
        sight::data::Mesh::sptr _mesh,
        unsigned int _nbStep,
        unsigned int _amplitude
    );

private:

    /**
     * @brief Sets the algorithm parameters.
     * @param _mesh the mesh to deform.
     * @param _nbStep the number of simulated acquisitions to compute between inspiration and expiration.
     * @param _amplitude
     */
    void setParam(
        sight::data::Mesh::sptr _mesh,
        unsigned int _nbStep,
        unsigned int _amplitude
    );

    /// Initializes the simulated acquisition.
    void initSimu();

    /// Computes the simulated acquisition for the given step between inspiration and expiration.
    void computeSimu();

    /// Defines the number of simulated acquisitions to compute between inspiration and expiration.
    unsigned int m_nbStep {0};

    unsigned int m_amplitude {40};

    int m_step {0};

    int m_direction {1};

    std::size_t m_nbPoints {0};

    std::size_t m_nbCells {0};

    float m_yCenter {0};

    sight::data::Mesh::wptr m_mesh;

    sight::data::Mesh::sptr m_originMesh;
};

} // namespace Tuto13MeshGeneratorCpp.
