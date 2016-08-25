/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWRENDEROGRE_VR_SATVOLUMEILLUMINATION_HPP__
#define __FWRENDEROGRE_VR_SATVOLUMEILLUMINATION_HPP__

#include "fwRenderOgre/config.hpp"
#include "fwRenderOgre/vr/SummedAreaTable.hpp"

namespace fwRenderOgre
{

namespace vr
{

/**
 * @brief Computes the illumination volume for extinction based shading.
 */
class FWRENDEROGRE_CLASS_API SATVolumeIllumination
{
public:

    /// Initializes the illumination volume with the give dimensions.
    FWRENDEROGRE_API SATVolumeIllumination(std::string parentId, ::Ogre::SceneManager* sceneManager,
                                           float satSizeRatio = 0.25f, int nbShells = 3, int shellRadius = 3);

    /// Destructor, does nothing.
    FWRENDEROGRE_API ~SATVolumeIllumination();

    /// Computes a new SAT with a different resolution given by the size ratio.
    FWRENDEROGRE_API void updateSAT(float _satSizeRatio);

    /// Recomputes the illumination volume using the current SAT.
    FWRENDEROGRE_API void updateVolIllum();

    /// Recomputes the SAT and the illumination volume when the image or the TF changed.
    FWRENDEROGRE_API void SATUpdate(Ogre::TexturePtr _img, Ogre::TexturePtr _tf);

    /// Illumination volume texture getter
    FWRENDEROGRE_API ::Ogre::TexturePtr getIlluminationVolume();

private:

    /// SAT used to compute the illumination volume.
    ::fwRenderOgre::vr::SummedAreaTable m_sat;

    /// Texture holding the illumination volume.
    ::Ogre::TexturePtr m_illuminationVolume;

    /// The index of the slice to which we currently render.
    int m_currentSliceIndex;

    /// Number of shells used to compute the volume illumination from the SAT.
    int m_nbShells;

    /// Radius of the shells used to compute the volume illumination from the SAT.
    int m_shellRadius;
};

//-----------------------------------------------------------------------------
// Inline method(s)

inline ::Ogre::TexturePtr SATVolumeIllumination::getIlluminationVolume()
{
    return m_illuminationVolume;
}

//-----------------------------------------------------------------------------

} // namespace vr

} // namespace fwRenderOgre

#endif // __FWRENDEROGRE_VR_SATVOLUMEILLUMINATION_HPP__
