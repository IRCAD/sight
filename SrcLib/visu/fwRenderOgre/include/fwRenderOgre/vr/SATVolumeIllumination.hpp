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
                                           float satSizeRatio = 0.25f, bool ao = false, bool shadows = false,
                                           int nbShells = 3, int shellRadius = 3, float coneAngle = 0.1f,
                                           int samplesAlongCone = 50);

    /// Destructor, does nothing.
    FWRENDEROGRE_API ~SATVolumeIllumination();

    /// Computes a new SAT with a different resolution given by the size ratio.
    FWRENDEROGRE_API void updateSatFromRatio(float _satSizeRatio);

    /// Recomputes the illumination volume using the current SAT.
    FWRENDEROGRE_API void updateVolIllum();

    /// Recomputes the SAT and the illumination volume when the image or the TF changed.
    FWRENDEROGRE_API void SATUpdate(Ogre::TexturePtr _img, Ogre::TexturePtr _tf);

    /// Illumination volume texture getter.
    FWRENDEROGRE_API ::Ogre::TexturePtr getIlluminationVolume();

    /// Ambient occlusion / color bleeding usage setter
    FWRENDEROGRE_API void setAO(bool _ao);

    /// Soft shadows usage setter.
    FWRENDEROGRE_API void setShadows(bool _shadows);

    /// Shells number setter.
    FWRENDEROGRE_API void setNbShells(int _nbShells);

    /// Shell radius setter.
    FWRENDEROGRE_API void setShellRadius(int _shellRadius);

    /// Cone angle setter.
    FWRENDEROGRE_API void setConeAngle(float _coneAngle);

    /// Cone samples number setter.
    FWRENDEROGRE_API void setSamplesAlongCone(int _samplesAlongCone);

private:

    /// Allocates or resize the texture used to store the illumination volume.
    void updateTexture();

    /// SAT used to compute the illumination volume.
    ::fwRenderOgre::vr::SummedAreaTable m_sat;

    /// Texture holding the illumination volume.
    ::Ogre::TexturePtr m_illuminationVolume;

    /// Sets ambient occlusion / color bleeding usage.
    bool m_ao;

    /// Sets soft shadows usage.
    bool m_shadows;

    /// The index of the slice to which we currently render.
    int m_currentSliceIndex;

    /// Number of shells used to compute the volume illumination from the SAT.
    int m_nbShells;

    /// Radius of the shells used to compute the volume illumination from the SAT.
    int m_shellRadius;

    /// Angle used to define the soft shadows cones.
    float m_coneAngle;

    /// Number of samples along the soft shadows cones.
    int m_samplesAlongCone;

    /// Prefix used to name the buffers.
    std::string m_parentId;

    /// Resource name of the source buffer.
    const std::string BUFFER_NAME = "__IlluminationVolume";

    /// Camera used
    ::Ogre::Camera* m_dummyCamera;

    /// Scene manager.
    ::Ogre::SceneManager* m_sceneManager;
};

//-----------------------------------------------------------------------------
// Inline method(s)

inline ::Ogre::TexturePtr SATVolumeIllumination::getIlluminationVolume()
{
    return m_illuminationVolume;
}

//-----------------------------------------------------------------------------

inline void SATVolumeIllumination::setAO(bool _ao)
{
    m_ao = _ao;
}

//-----------------------------------------------------------------------------

inline void SATVolumeIllumination::setShadows(bool _shadows)
{
    m_shadows = _shadows;
}

//-----------------------------------------------------------------------------

inline void SATVolumeIllumination::setNbShells(int _nbShells)
{
    m_nbShells = _nbShells;
}

//-----------------------------------------------------------------------------

inline void SATVolumeIllumination::setShellRadius(int _shellRadius)
{
    m_shellRadius = _shellRadius;
}

//-----------------------------------------------------------------------------

inline void SATVolumeIllumination::setConeAngle(float _coneAngle)
{
    m_coneAngle = _coneAngle;
}

//-----------------------------------------------------------------------------

inline void SATVolumeIllumination::setSamplesAlongCone(int _samplesAlongCone)
{
    m_samplesAlongCone = _samplesAlongCone;
}

//-----------------------------------------------------------------------------

} // namespace vr

} // namespace fwRenderOgre

#endif // __FWRENDEROGRE_VR_SATVOLUMEILLUMINATION_HPP__
