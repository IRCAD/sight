/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWIGG_MESHER_HPP__
#define __FWIGG_MESHER_HPP__

#include "fwIGG/config.hpp"

#include <fwCore/base.hpp>
#include <fwCore/BaseObject.hpp>

#include <fwData/Image.hpp>
#include <fwData/Mesh.hpp>

#include <fwDataTools/Mesh.hpp>

#include <Mesher/simplifvoxmesh.h>


namespace fwIGG
{

struct PFP : public CGoGN::PFP_STANDARD
{
    typedef CGoGN::EmbeddedMap2 MAP;
};

/**
 * @brief   IGG Mesher: Class used to generate a mesh from an image.
 */
class FWIGG_CLASS_API Mesher : public ::fwCore::BaseObject
{
public:

    fwCoreClassDefinitionsWithFactoryMacro( (Mesher)(::fwCore::BaseObject), (()), new Mesher );

    /**
     * @brief Computes a simplified ::fwData::Mesh according to a given mask
     * @tparam DATATYPE pixel format
     * @param[in] img the image to mesh
     * @param[out] mesh the mesh
     * @param[in] valueMin the minimum value in the image to mesh
     * @param[in] valueMax the maximum value in the image to mesh
     * @param[in] faces the percentage or the number of faces needed for the mesh
     * @param[in] adapt the adaptivity factor used to simplify the mesh (0: does not adapt to 100: adaptivity max)
     * @param[in] radius the radius of the ball used to simplify the mesh (5 to 8 recommended)
     * @param[in] percent whether the "faces" parameter corresponds to percentage or number of faces
     * @param[in] closing whether the mesh is closed at the bounds of the image
     */
    template < typename DATATYPE >
    static void computeMeshMCS(::fwData::Image::csptr img, ::fwData::Mesh::sptr mesh, DATATYPE valueMin,
                               DATATYPE valueMax, unsigned int faces, unsigned int adapt,
                               unsigned int radius, bool percent, bool closing);

    /**
     * @brief Computes a simplified CGoGN mesh according to a given mask
     * @tparam DATATYPE pixel format
     * @param[in] img the image to mesh
     * @param[in] valueMin the minimum value in the image to mesh
     * @param[in] valueMax the maximum value in the image to mesh
     * @param[in] faces the percentage or the number of faces needed for the mesh
     * @param[in] adapt the adaptivity factor used to simplify the mesh (0: does not adapt to 100: adaptivity max)
     * @param[in] radius the radius of the ball used to simplify the mesh (5 to 8 recommended)
     * @param[in] percent whether the "faces" parameter corresponds to percentage or number of faces
     * @param[in] closing whether the mesh is closed at the bounds of the image
     * @return the computed CGoGN mesh
     */
    template < typename DATATYPE >
    static ::boost::shared_ptr<PFP::MAP> computeMeshMCSCGoGN(::fwData::Image::csptr img, DATATYPE valueMin,
                                                             DATATYPE valueMax, unsigned int faces,
                                                             unsigned int adapt, unsigned int radius, bool percent,
                                                             bool closing);

    /**
     * @brief Computes a ::fwData::Mesh marching cube of a given mask
     * @tparam DATATYPE pixel format
     * @param[in] img the image to mesh
     * @param[out] mesh the mesh
     * @param[in] valueMin the minimum value in the image to mesh
     * @param[in] valueMax the maximum value in the image to mesh
     */
    template < typename DATATYPE >
    static void computeMeshMC(::fwData::Image::csptr img, ::fwData::Mesh::sptr mesh,
                              DATATYPE valueMin, DATATYPE valueMax);

    /**
     * @brief Computes a CGoGN mesh marching cube of a given mask
     * @tparam DATATYPE pixel format
     * @param[in] img the image to mesh
     * @param[in] valueMin the minimum value in the image to mesh
     * @param[in] valueMax the maximum value in the image to mesh
     * @return the computed CGoGN mesh
     */
    template < typename DATATYPE >
    static ::boost::shared_ptr<PFP::MAP> computeMeshMCCGoGN(::fwData::Image::csptr img, DATATYPE valueMin,
                                                            DATATYPE valueMax);

private:

    /**
     * @brief Converts CGoGN mesh to ::fwData::Mesh
     * @param[in] pfpMap the CGoGN mesh to convert
     * @param[out] mesh the converted ::fwData::Mesh
     * @param[in] origin the origin of the image used to compute the mesh
     */
    FWIGG_API static void convertCgognMesh( ::boost::shared_ptr<PFP::MAP> pfpMap,
                                            ::fwData::Mesh::sptr mesh,
                                            ::fwData::Image::OriginType origin);

};

} // namespace fwIGG

#endif /*__FWIGG_MESHER_HPP__*/


