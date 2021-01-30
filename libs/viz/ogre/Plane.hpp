/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
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

#include "core/tools/fwID.hpp"

#include "viz/ogre/config.hpp"

#include <data/tools/helper/MedicalImage.hpp>

#include <OGRE/OgreMaterial.h>
#include <OGRE/OgreMesh.h>
#include <OGRE/OgreNumerics.h>
#include <OGRE/OgreSceneManager.h>
#include <OGRE/OgreTechnique.h>
#include <OGRE/OgreTexture.h>

#include <viz/ogre/TransferFunction.hpp>

#include <string>

namespace Ogre
{
class SceneNode;
}

namespace sight::viz::ogre
{

/**
 * @brief Manages a plane mesh on which a slice texture will be applied.
 */
class VIZ_OGRE_CLASS_API Plane
{

public:

    typedef data::tools::helper::MedicalImage::Orientation OrientationMode;

    /// Defines the texture filtering mode.
    typedef enum FilteringEnum
    {
        NONE,
        LINEAR,
        ANISOTROPIC
    } FilteringEnumType;

    using sptr = std::shared_ptr<Plane>;

    /**
     * @brief Creates a plane, instantiates its material. Call @ref Plane::initializePlane() to create its geometry.
     * @param _negatoId unique identifier of the negato.
     * @param _parentSceneNode parent node where attach the plane.
     * @param _sceneManager the Ogre scene manager.
     * @param _orientation the direction of the plane.
     * @param _tex texture to apply on the plane.
     * @param _filtering filtering method used to apply the texture.
     * @param _entityOpacity opacity of the entity.
     */
    VIZ_OGRE_API Plane(const core::tools::fwID::IDType& _negatoId, ::Ogre::SceneNode* _parentSceneNode,
                       ::Ogre::SceneManager* _sceneManager, OrientationMode _orientation, ::Ogre::TexturePtr _tex,
                       FilteringEnumType _filtering, float _entityOpacity = 1.0f, bool _displayBorder = true);

    /// Cleans ogre resources.
    VIZ_OGRE_API virtual ~Plane();

    /// Instantiates the plane mesh and entity.
    VIZ_OGRE_API void initializePlane();

    /**
     * @brief Handles the slice plane move.
     *     - in 2D, it will convert the position in unit floating value and call the fragment shader.
     *     - in 3D, it will also move the scene node in space.
     * @param _sliceIndex the image slice index used to move the plane.
     */
    VIZ_OGRE_API void changeSlice(float _sliceIndex);

    /**
     * @brief Sets the image axis orthogonal to the plane.
     * @param _newMode the new orientation.
     */
    VIZ_OGRE_API void setOrientationMode(OrientationMode _newMode);

    /**
     * @brief Sets whether the negato's opacity is taken into account.
     * @param _enable used true to enable the opacity.
     */
    VIZ_OGRE_API void enableAlpha(bool _enable);

    /**
     * @brief Sets the real world image's origin.
     * @param _origPos the image origin.
     */
    VIZ_OGRE_API void setOriginPosition(const ::Ogre::Vector3& _origPos);

    /**
     * @brief Sets the real world size of a voxel.
     * @param _spacing the image spacing.
     */
    VIZ_OGRE_API void setVoxelSpacing(const ::Ogre::Vector3& _spacing);

    /**
     * @brief Sets the plane's opacity.
     * @param _f the opacity value
     *
     * @pre _f must fit between 0 and 1.
     */
    VIZ_OGRE_API void setEntityOpacity(float _f);

    /**
     * @brief Shows/hides the plane in the scene.
     * @param _visible use true to show the plane.
     */
    VIZ_OGRE_API void setVisible(bool _visible);

    /**
     * @brief Adds or updates the texture containing the transfer function data in the negato passes.
     * @param _tfTexture the TF texture.
     */
    VIZ_OGRE_API void setTFData(const viz::ogre::TransferFunction& _tfTexture);

    /**
     * @brief Sets whether or not the transfer function uses thresholding.
     * @param _threshold use true to enable the treshold.
     */
    VIZ_OGRE_API void switchThresholding(bool _threshold);

    /// Gets the plane's width in model space.
    VIZ_OGRE_API ::Ogre::Real getWidth() const;

    /// Gets the plane's height in model space.
    VIZ_OGRE_API ::Ogre::Real getHeight() const;

    /// Moves the scene node to m_originPosition point
    VIZ_OGRE_API void moveToOriginPosition();

    /// Gets the x, y or z world position of the plane scene node according to the current orientation mode
    VIZ_OGRE_API double getSliceWorldPosition() const;

    /// Gets the image axis orthogonal to the plane.
    VIZ_OGRE_API OrientationMode getOrientationMode() const;

    /// Gets the material used to render the plane.
    VIZ_OGRE_API ::Ogre::MaterialPtr getMaterial();

    /// Gets the movable object created by this class.
    VIZ_OGRE_API const ::Ogre::MovableObject* getMovableObject() const;

    /**
     * @brief Sets the picking flags.
     * @param _flags the flags use for the picking.
     */
    VIZ_OGRE_API void setQueryFlags(std::uint32_t _flags);

    /// Sets this object's render queue group and render priority.
    VIZ_OGRE_API void setRenderQueuerGroupAndPriority(std::uint8_t _groupId, std::uint16_t _priority);

private:

    /// Sets the plane's original position.
    void initializePosition();

    /// Creates a material for the mesh plane with the negato texture.
    void initializeMaterial();

    /// Sets the relativePosition.
    void setRelativePosition(float _relativePosition);

    /// Moves plane along its Normal.
    void moveAlongAxis();

    /// Sets the dimensions for the related members, and also creates a movable plane to instanciate the entity.
    ::Ogre::MovablePlane setDimensions();

    /// Indicates whether whe want to threshold instead of windowing.
    bool m_threshold { false };

    /// Defines the filtering type for this plane.
    FilteringEnumType m_filtering { FilteringEnum::ANISOTROPIC };

    /// Defines the orientation mode of the plane.
    OrientationMode m_orientation;

    /// Contains the plane on wich we will apply a texture.
    ::Ogre::MeshPtr m_slicePlane;

    /// Contains the manual object that represent borders.
    ::Ogre::ManualObject* m_border { nullptr };

    /// Defines the origin position of the slice plane according to the source image's origin.
    ::Ogre::Vector3 m_originPosition { ::Ogre::Vector3::ZERO };

    /// Contains the plane material.
    ::Ogre::MaterialPtr m_texMaterial { nullptr };

    /// Contains the material used to display borders.
    ::Ogre::MaterialPtr m_borderMaterial { nullptr };

    /// Contains the texture.
    ::Ogre::TexturePtr m_texture;

    /// Contains the scenemanager containing the plane.
    ::Ogre::SceneManager* m_sceneManager { nullptr };

    /// Defines a strings needed to initialize mesh, scenenode, etc.
    std::string m_slicePlaneName;

    /// Defines the entity name. used to recover this from the Ogre entityManager.
    std::string m_entityName;

    /// Defines the scene node name used to recover the scene node from it's name.
    std::string m_sceneNodeName;

    /// Contains the scene node on which we will attach the mesh.
    ::Ogre::SceneNode* m_planeSceneNode { nullptr };

    /// Contains the parent scene node.
    ::Ogre::SceneNode* m_parentSceneNode { nullptr };

    /// Defines the entity's width.
    ::Ogre::Real m_width { 0.f };

    /// Defines the entity's height.
    ::Ogre::Real m_height { 0.f };

    /// Defines the entity's depth.
    ::Ogre::Real m_depth { 0.f };

    /// Defines the spacing in the texture 3d image file.
    ::Ogre::Vector3 m_spacing { ::Ogre::Vector3::ZERO };

    /// Defines the depth range.
    float m_relativePosition { 0.8f };

    /// Defines the opacity applied to the entity.
    float m_entityOpacity { 1.f };

    /// Defines if the border is displayed.
    bool m_displayBorder { true };

};

//------------------------------------------------------------------------------

inline void Plane::setOriginPosition(const ::Ogre::Vector3& _origPos)
{
    m_originPosition = _origPos;
}

//------------------------------------------------------------------------------

inline Plane::OrientationMode Plane::getOrientationMode() const
{
    return m_orientation;
}

//------------------------------------------------------------------------------

inline ::Ogre::MaterialPtr Plane::getMaterial()
{
    return m_texMaterial;
}

//------------------------------------------------------------------------------

inline ::Ogre::Real Plane::getWidth() const
{
    return m_width;
}

//------------------------------------------------------------------------------

inline ::Ogre::Real Plane::getHeight() const
{
    return m_height;
}

//------------------------------------------------------------------------------

} // Namespace fwRenderOgre.
