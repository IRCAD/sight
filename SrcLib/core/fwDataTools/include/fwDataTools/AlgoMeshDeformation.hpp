/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDATATOOLS_ALGOMESHDEFORMATION_HPP__
#define __FWDATATOOLS_ALGOMESHDEFORMATION_HPP__

#include "fwDataTools/config.hpp"
#include <fwDataTools/helper/Mesh.hpp>

#include <fwData/Array.hpp>
#include <fwData/Mesh.hpp>

#include <fwTools/Failed.hpp>


namespace fwDataTools
{

class FWDATATOOLS_CLASS_API AlgoMeshDeformation
{

public:

    /**
     * @brief   Constructor
     */
    FWDATATOOLS_API AlgoMeshDeformation() throw();

    /**
     * @brief   Destructor
     */
    FWDATATOOLS_API virtual ~AlgoMeshDeformation() throw();

    /**
     * @brief Initialize algorithm's parameters.
     */
    FWDATATOOLS_API void setParam( ::fwData::Mesh::sptr _mesh,
                                   const unsigned int _step,
                                   const unsigned int _amplitude);

    /**
     * @brief Initialize the simulated acquisition.
     */
    FWDATATOOLS_API void initSimu();

    /**
     * @brief Compute the simulated acquisition for the given step between inspiration and expiration.
     */
    FWDATATOOLS_API void computeSimu();

    /// Compute deformation or init algo if necessary
    FWDATATOOLS_API void computeDeformation( ::fwData::Mesh::sptr _mesh,
                                             const unsigned int _nbStep,
                                             const unsigned int _amplitude );

private:

    ::fwData::Mesh::wptr m_mesh;
    ::fwDataTools::helper::Mesh::sptr m_meshHelper;
    ::fwData::Array::sptr m_originPoints;

    unsigned int m_amplitude;
    unsigned int m_step;
    int m_direction;
    size_t m_nbPoints;
    size_t m_nbCells;
    float m_yCenter;

    /**
     * @brief Number of simulated acquisitions to compute between inspiration and expiration.
     */
    unsigned int m_nbStep;

    /**
     * @brief Number of the last computed step between 0 and m_step
     */
    unsigned int m_lastStep;

};

}   // namespace fwDataTools

#endif  // __FWDATATOOLS_ALGOMESHDEFORMATION_HPP__

