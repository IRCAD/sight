/************************************************************************
 *
 * Copyright (C) 2016-2021 IRCAD France
 * Copyright (C) 2016-2021 IHU Strasbourg
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

#include "viz/ogre/config.hpp"
#include "viz/ogre/vr/SummedAreaTable.hpp"

namespace sight::viz::ogre
{

namespace vr
{

/**
 * @brief Computes the illumination volume for extinction based shading.
 */
class VIZ_OGRE_CLASS_API IllumAmbientOcclusionSAT final
{

public:

    typedef std::shared_ptr<IllumAmbientOcclusionSAT> sptr;

    /// Initializes the illumination volume with the give dimensions.
    VIZ_OGRE_API IllumAmbientOcclusionSAT(std::string parentId, ::Ogre::SceneManager* sceneManager,
                                          float satSizeRatio = 0.25f, bool ao = false, bool shadows = false,
                                          int nbShells = 3, int shellRadius = 3, float coneAngle = 0.1f,
                                          int samplesAlongCone = 50);

    /// Destructor, does nothing.
    VIZ_OGRE_API virtual ~IllumAmbientOcclusionSAT();

    /// Computes a new SAT with a different resolution given by the size ratio.
    VIZ_OGRE_API void updateSatFromRatio(float _satSizeRatio);

    /// Recomputes the illumination volume using the current SAT.
    VIZ_OGRE_API void updateVolIllum();

    /// Recomputes the SAT and the illumination volume when the image or the TF changed.
    VIZ_OGRE_API void SATUpdate(::Ogre::TexturePtr _img, const viz::ogre::TransferFunction::sptr& _tf,
                                float _sampleDistance);

    /// Illumination volume texture getter.
    VIZ_OGRE_API ::Ogre::TexturePtr getIlluminationVolume();

    /// Ambient occlusion / color bleeding usage setter
    VIZ_OGRE_API void setAO(bool _ao);

    /// Soft shadows usage setter.
    VIZ_OGRE_API void setShadows(bool _shadows);

    /// Shells number setter.
    VIZ_OGRE_API void setNbShells(int _nbShells);

    /// Shell radius setter.
    VIZ_OGRE_API void setShellRadius(int _shellRadius);

    /// Cone angle setter.
    VIZ_OGRE_API void setConeAngle(float _coneAngle);

    /// Cone samples number setter.
    VIZ_OGRE_API void setSamplesAlongCone(int _samplesAlongCone);

private:

    /// Allocates or resize the texture used to store the illumination volume.
    void updateTexture();

    /// SAT used to compute the illumination volume.
    viz::ogre::vr::SummedAreaTable m_sat;

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

inline ::Ogre::TexturePtr IllumAmbientOcclusionSAT::getIlluminationVolume()
{
    return m_illuminationVolume;
}

//-----------------------------------------------------------------------------

inline void IllumAmbientOcclusionSAT::setAO(bool _ao)
{
    m_ao = _ao;
}

//-----------------------------------------------------------------------------

inline void IllumAmbientOcclusionSAT::setShadows(bool _shadows)
{
    m_shadows = _shadows;
}

//-----------------------------------------------------------------------------

inline void IllumAmbientOcclusionSAT::setNbShells(int _nbShells)
{
    m_nbShells = _nbShells;
}

//-----------------------------------------------------------------------------

inline void IllumAmbientOcclusionSAT::setShellRadius(int _shellRadius)
{
    m_shellRadius = _shellRadius;
}

//-----------------------------------------------------------------------------

inline void IllumAmbientOcclusionSAT::setConeAngle(float _coneAngle)
{
    m_coneAngle = _coneAngle;
}

//-----------------------------------------------------------------------------

inline void IllumAmbientOcclusionSAT::setSamplesAlongCone(int _samplesAlongCone)
{
    m_samplesAlongCone = _samplesAlongCone;
}

//-----------------------------------------------------------------------------

} // namespace vr

} // namespace sight::viz::ogre
