/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "Tuto14MeshGenerator/config.hpp"

#include <fwData/Array.hpp>
#include <fwData/Mesh.hpp>

#include <fwDataTools/helper/Mesh.hpp>

#include <fwTools/Failed.hpp>

namespace Tuto14MeshGenerator
{

class TUTO14MESHGENERATOR_CLASS_API AlgoMeshDeformation
{
public:

    /// Constructor
    TUTO14MESHGENERATOR_API AlgoMeshDeformation() noexcept;

    /// Destructor
    TUTO14MESHGENERATOR_API virtual ~AlgoMeshDeformation() noexcept;

    /**
     * @brief Initialize algorithm's parameters.
     */
    TUTO14MESHGENERATOR_API void setParam( ::fwData::Mesh::sptr _mesh,
                                           const unsigned int _step,
                                           const unsigned int _amplitude);

    /**
     * @brief Initialize the simulated acquisition.
     */
    TUTO14MESHGENERATOR_API void initSimu();

    /**
     * @brief Compute the simulated acquisition for the given step between inspiration and expiration.
     */
    TUTO14MESHGENERATOR_API void computeSimu();

    /// Compute deformation or init algo if necessary
    TUTO14MESHGENERATOR_API void computeDeformation( ::fwData::Mesh::sptr _mesh,
                                                     const unsigned int _nbStep,
                                                     const unsigned int _amplitude );

private:

    /**
     * @brief Number of simulated acquisitions to compute between inspiration and expiration.
     */
    unsigned int m_nbStep;

    ::fwData::Mesh::wptr m_mesh;
    ::fwDataTools::helper::Mesh::sptr m_meshHelper;
    ::fwData::Array::sptr m_originPoints;

    unsigned int m_amplitude;
    unsigned int m_step;
    int m_direction;
    size_t m_nbPoints;
    size_t m_nbCells;
    float m_yCenter;
};

}   // namespace Tuto14MeshGenerator

