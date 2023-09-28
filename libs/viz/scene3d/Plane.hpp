/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
 * Copyright (C) 2014-2020 IHU Strasbourg
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

#include <data/helper/MedicalImage.hpp>

#include <viz/scene3d/Texture.hpp>
#include <viz/scene3d/TransferFunction.hpp>

#include <OGRE/OgreMaterial.h>
#include <OGRE/OgreMesh.h>
#include <OGRE/OgreNumerics.h>
#include <OGRE/OgreSceneManager.h>
#include <OGRE/OgreTechnique.h>
#include <OGRE/OgreTexture.h>

#include <string>

namespace Ogre
{

class SceneNode;

} // namespace Ogre

namespace sight::viz::scene3d
{

/**
 * @brief Manages a plane mesh on which a slice texture will be applied.
 */
class VIZ_SCENE3D_CLASS_API Plane
{
public:

    using OrientationMode = data::helper::MedicalImage::orientation_t;

    /// Defines the texture filtering mode.
    enum class filter_t : std::uint8_t
    {
        NONE,
        LINEAR,
        ANISOTROPIC
    };

    using sptr = std::shared_ptr<Plane>;

    /**
     * @brief Creates a plane, instantiates its material. Call @ref Plane::update() to create its geometry.
     * @param _negatoId unique identifier of the negato.
     * @param _parentSceneNode parent node where attach the plane.
     * @param _sceneManager the Ogre scene manager.
     * @param _tex texture to apply on the plane.
     * @param _filtering filtering method used to apply the texture.
     * @param _displayBorder display a border around the negato plane.
     * @param _displayOtherPlanes display a line indicating the location of the two other planes.
     * @param _entityOpacity opacity of the entity.
     */
    VIZ_SCENE3D_API Plane(
        const core::tools::id::type& _negatoId,
        Ogre::SceneNode* _parentSceneNode,
        Ogre::SceneManager* _sceneManager,
        viz::scene3d::Texture::sptr _tex,
        filter_t _filtering,
        bool _displayBorder      = true,
        bool _displayOtherPlanes = true,
        float _entityOpacity     = 1.0F
    );

    /// Cleans ogre resources.
    VIZ_SCENE3D_API ~Plane();

    /**
     * @brief Instantiates the plane mesh and entity.
     * @param _enableTransparency used true to enable the opacity.
     */
    VIZ_SCENE3D_API void update(
        OrientationMode _orientation,
        const Ogre::Vector3& _spacing,
        const Ogre::Vector3& _origin,
        bool _enableTransparency
    );

    /**
     * @brief Handles the slice plane move.
     *     - in 2D, it will convert the position in unit floating value and call the fragment shader.
     *     - in 3D, it will also move the scene node in space.
     * @param _slicesIndex the image slices indexes.
     */
    VIZ_SCENE3D_API void changeSlice(const std::array<float, 3>& _slicesIndex);

    /**
     * @brief Sets the plane's opacity.
     * @param _f the opacity value
     *
     * @pre _f must fit between 0 and 1.
     */
    VIZ_SCENE3D_API void setEntityOpacity(float _f);

    /**
     * @brief Shows/hides the plane in the scene.
     * @param _visible use true to show the plane.
     */
    VIZ_SCENE3D_API void setVisible(bool _visible);

    /**
     * @brief Adds or updates the texture containing the transfer function data in the negato passes.
     * @param _tfTexture the TF texture.
     */
    VIZ_SCENE3D_API void setTFData(const viz::scene3d::TransferFunction& _tfTexture);

    /// Gets the image axis orthogonal to the plane.
    [[nodiscard]] VIZ_SCENE3D_API OrientationMode getOrientationMode() const;

    /// Gets the movable object created by this class.
    [[nodiscard]] VIZ_SCENE3D_API const Ogre::MovableObject* getMovableObject() const;

    /**
     * @brief Sets the picking flags.
     * @param _flags the flags use for the picking.
     */
    VIZ_SCENE3D_API void setQueryFlags(std::uint32_t _flags);

    /// Sets this object's render queue group and render priority.
    VIZ_SCENE3D_API void setRenderQueuerGroupAndPriority(std::uint8_t _groupId, std::uint16_t _priority);

    /// Compute two cross lines that intersect at the given position, according to the plane orientation.
    VIZ_SCENE3D_API std::array<Ogre::Vector3, 4> computeCross(
        const Ogre::Vector3& _center,
        const Ogre::Vector3& _imageOrigin
    ) const;

private:

    /// Sets the plane's original position.
    void updatePosition();

    /// Sets the dimensions for the related members, and also creates a movable plane to instantiate the entity.
    Ogre::MovablePlane setDimensions(const Ogre::Vector3& _spacing);

    struct LineShape
    {
        /// Contains the manual object that represent borders.
        Ogre::ManualObject* shape {nullptr};

        /// Contains the material used to display borders.
        Ogre::MaterialPtr material {nullptr};

        /// Is it displayed?
        bool enabled {true};
    };

    /// Optional border
    LineShape m_border;

    /// Lines representing the slices of the two other orthogonal planes
    LineShape m_slicesCross;

    /// Defines the filtering type for this plane.
    filter_t m_filtering {filter_t::ANISOTROPIC};

    /// Defines the orientation mode of the plane.
    OrientationMode m_orientation {OrientationMode::X_AXIS};

    /// Contains the plane on which we will apply a texture.
    Ogre::MeshPtr m_slicePlane;

    /// Defines the origin position of the slice plane according to the source image's origin.
    Ogre::Vector3 m_origin {Ogre::Vector3::ZERO};

    /// Contains the plane material.
    Ogre::MaterialPtr m_texMaterial {nullptr};

    /// Contains the texture.
    viz::scene3d::Texture::sptr m_texture;

    /// Contains the scenemanager containing the plane.
    Ogre::SceneManager* m_sceneManager {nullptr};

    /// Defines a strings needed to initialize mesh, scenenode, etc.
    std::string m_slicePlaneName;

    /// Defines the entity name. used to recover this from the Ogre entityManager.
    std::string m_entityName;

    /// Defines the scene node name used to recover the scene node from it's name.
    std::string m_sceneNodeName;

    /// Contains the scene node on which we will attach the mesh.
    Ogre::SceneNode* m_planeSceneNode {nullptr};

    /// Contains the parent scene node.
    Ogre::SceneNode* m_parentSceneNode {nullptr};

    /// Defines the entity's size in the X,Y,Z axis.
    Ogre::Vector3 m_size {0.F};

    /// Defines the opacity applied to the entity.
    float m_entityOpacity {1.F};
};

//------------------------------------------------------------------------------

inline Plane::OrientationMode Plane::getOrientationMode() const
{
    return m_orientation;
}

//------------------------------------------------------------------------------

} // namespace sight::viz::scene3d
