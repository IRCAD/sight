/************************************************************************
 *
 * Copyright (C) 2016-2022 IRCAD France
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

#include "viz/scene3d/config.hpp"
#include "viz/scene3d/Texture.hpp"
#include "viz/scene3d/vr/SummedAreaTable.hpp"

#include <optional>

namespace sight::viz::scene3d
{

namespace vr
{

/**
 * @brief Computes the illumination volume for extinction based shading.
 */
class VIZ_SCENE3D_CLASS_API IllumAmbientOcclusionSAT final
{
public:

    typedef std::shared_ptr<IllumAmbientOcclusionSAT> sptr;

    /// Internal wrapper holding the Summed Area Table (SAT) parameters
    struct sat_parameters_t
    {
        /// Ratio used to determine the size of the SAT regarding of the associated image size.
        float size_ratio = 0.25f;

        /// Number of shells used to compute the volume illumination from the SAT.
        unsigned shells = 4;

        /// Radius of the shells used to compute the volume illumination from the SAT.
        unsigned radius = 4;

        /// Angle used to define the soft shadows cones.
        float angle = 0.1f;

        /// Number of samples along the soft shadows cones.
        unsigned samples = 50;
    };

    /// Initializes the illumination volume with the give dimensions.
    VIZ_SCENE3D_API IllumAmbientOcclusionSAT(
        std::string parentId,
        Ogre::SceneManager* sceneManager,
        bool ao                                    = false,
        bool shadows                               = false,
        std::optional<sat_parameters_t> parameters = {});

    /// Destructor, does nothing.
    VIZ_SCENE3D_API ~IllumAmbientOcclusionSAT();

    /// Computes a new SAT with a different resolution given by the size ratio.
    VIZ_SCENE3D_API void updateSatFromRatio(float _satSizeRatio);

    /// Recomputes the illumination volume using the current SAT.
    VIZ_SCENE3D_API void updateVolumeIllumination();

    /// Recomputes the SAT and the illumination volume when the image or the TF changed.
    VIZ_SCENE3D_API void SATUpdate(
        const Texture::sptr& _img,
        const viz::scene3d::TransferFunction::sptr& _tf,
        float _sampleDistance
    );

    /// Ambient occlusion / color bleeding usage setter
    VIZ_SCENE3D_API void setAO(bool _ao);

    /// Soft shadows usage setter.
    VIZ_SCENE3D_API void setShadows(bool _shadows);

    /// Shells number setter.
    VIZ_SCENE3D_API void setNbShells(unsigned _nbShells);

    /// Shell radius setter.
    VIZ_SCENE3D_API void setShellRadius(unsigned _shellRadius);

    /// Cone angle setter.
    VIZ_SCENE3D_API void setConeAngle(float _coneAngle);

    /// Cone samples number setter.
    VIZ_SCENE3D_API void setSamplesAlongCone(unsigned _samplesAlongCone);

    ///@brief Parameters
    VIZ_SCENE3D_API const sat_parameters_t& parameters() const;

    ///@brief Returns the texture used to store the illumination volume.
    const Ogre::TexturePtr& getIlluminationVolume() const;

private:

    /// Allocates or resize the texture used to store the illumination volume.
    void updateTexture();

    /// Texture holding the illumination volume.
    Ogre::TexturePtr m_illuminationVolume;

    /// Sets ambient occlusion / color bleeding usage.
    bool m_ao;

    /// Sets soft shadows usage.
    bool m_shadows;

    /// The index of the slice to which we currently render.
    int m_currentSliceIndex;

    ///@brief Parameters
    sat_parameters_t m_parameters;

    /// Prefix used to name the buffers.
    std::string m_parentId;

    /// Resource name of the source buffer, i.e. "__IlluminationVolume".
    static inline const std::string BUFFER_NAME = "__IlluminationVolume";

    /// Camera used
    std::unique_ptr<Ogre::Camera> m_dummyCamera {nullptr};

    /// Scene manager.
    Ogre::SceneManager* m_sceneManager;

    /// SAT used to compute the illumination volume.
    viz::scene3d::vr::SummedAreaTable m_sat;
};

//-----------------------------------------------------------------------------
// Inline method(s)

inline const Ogre::TexturePtr& IllumAmbientOcclusionSAT::getIlluminationVolume() const
{
    return m_illuminationVolume;
}

//------------------------------------------------------------------------------

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

inline void IllumAmbientOcclusionSAT::setNbShells(unsigned _nbShells)
{
    m_parameters.shells = _nbShells;
}

//-----------------------------------------------------------------------------

inline void IllumAmbientOcclusionSAT::setShellRadius(unsigned _shellRadius)
{
    m_parameters.radius = _shellRadius;
}

//-----------------------------------------------------------------------------

inline void IllumAmbientOcclusionSAT::setConeAngle(float _coneAngle)
{
    m_parameters.angle = _coneAngle;
}

//-----------------------------------------------------------------------------

inline void IllumAmbientOcclusionSAT::setSamplesAlongCone(unsigned _samplesAlongCone)
{
    m_parameters.samples = _samplesAlongCone;
}

//------------------------------------------------------------------------------

inline auto IllumAmbientOcclusionSAT::parameters() const -> const sat_parameters_t&
{
    return m_parameters;
}

//-----------------------------------------------------------------------------

} // namespace vr

} // namespace sight::viz::scene3d
