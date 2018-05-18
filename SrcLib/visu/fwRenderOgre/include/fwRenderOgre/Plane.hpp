/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "fwRenderOgre/config.hpp"

#include "fwTools/fwID.hpp"

#include <fwDataTools/helper/MedicalImageAdaptor.hpp>

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
}

namespace fwRenderOgre
{

/**
 * @brief Manages a plane mesh on which a slice texture will be applied
 */
class FWRENDEROGRE_CLASS_API Plane
{
public:

    typedef ::fwDataTools::helper::MedicalImageAdaptor::Orientation OrientationMode;

    typedef enum FilteringEnum
    {
        NONE,
        LINEAR,
        ANISOTROPIC
    } FilteringEnumType;

    FWRENDEROGRE_API Plane(const ::fwTools::fwID::IDType& _negatoId, ::Ogre::SceneNode* _parentSceneNode,
                           ::Ogre::SceneManager* _sceneManager, OrientationMode _orientation, bool _is3D,
                           ::Ogre::TexturePtr _tex, FilteringEnumType _filtering, float _entityOpacity = 1.0f);

    FWRENDEROGRE_API virtual ~Plane();

    FWRENDEROGRE_API void initialize3DPlane();
    FWRENDEROGRE_API void initialize2DPlane();

    /// Slot handling slice plane move:
    ///     - in 2D, it will convert the position in unit floating value and call the fragment shader
    ///     - in 3D, it will also move the scene node in space
    FWRENDEROGRE_API void changeSlice( float _sliceIndex );

    FWRENDEROGRE_API void setOrientationMode(OrientationMode _newMode);

    FWRENDEROGRE_API void setOriginPosition(const ::Ogre::Vector3& _origPos);

    FWRENDEROGRE_API std::vector< ::Ogre::Real > getDepthSpacing();
    FWRENDEROGRE_API void setDepthSpacing(std::vector<double> _spacing);

    FWRENDEROGRE_API void setEntityOpacity( float _f );

    /// Adds or updates the texture containing the transfer function data in the negato passes
    FWRENDEROGRE_API void setTFData(const ::Ogre::TexturePtr _tfTexture, ::Ogre::Vector2 _tfWindow);

    FWRENDEROGRE_API void switchThresholding(bool _threshold);

    FWRENDEROGRE_API ::Ogre::Real getWidth() const;
    FWRENDEROGRE_API ::Ogre::Real getHeight() const;

    /// Moves the scene node to m_originPosition point
    FWRENDEROGRE_API void moveToOriginPosition();

    /// Returns the x, y or z world position of the plane scene node according to the current orientation mode
    FWRENDEROGRE_API double getSliceWorldPosition() const;

    FWRENDEROGRE_API OrientationMode getOrientationMode() const;

    FWRENDEROGRE_API ::Ogre::MaterialPtr getMaterial() const;

private:

    /// Sets the plane's original position.
    void initializePosition();

    /// Creates a material for the mesh plane with the negato texture
    void initializeMaterial();

    /// Sets the relativePosition, then
    void setRelativePosition(float _relativePosition);

    /// Moves plane along its Normal.
    void moveAlongAxis();

    /// Sets the dimensions for the related members, and also creates a movable plane to instanciate the entity.
    ::Ogre::MovablePlane* setDimensions();

    /// Indicates whether the plane is used by a 3D negato or not
    bool m_is3D;

    /// Indicates whether whe want to threshold instead of windowing
    bool m_threshold;

    /// Defines the filtering type for this plane
    FilteringEnumType m_filtering;

    /// Orientation mode of the plane
    OrientationMode m_orientation;

    /// The plane on wich we will apply a texture
    ::Ogre::MeshPtr m_slicePlane;
    /// The origin position of the slice plane according to the source image's origin
    ::Ogre::Vector3 m_originPosition;
    /// Pointer to the material
    ::Ogre::MaterialPtr m_texMaterial;
    /// Pointer to the texture
    ::Ogre::TexturePtr m_texture;
    /// Points to the scenemanager containing the plane
    ::Ogre::SceneManager* m_sceneManager;

    /// Strings needed to initialize mesh, scenenode, etc.
    std::string m_slicePlaneName;
    /// Entity name. used to recover this from the Ogre entityManager
    std::string m_entityName;
    /// Used to recover the scenenode from it's name.
    std::string m_sceneNodeName;

    /// The scene node on which we will attach the mesh
    ::Ogre::SceneNode* m_planeSceneNode;
    /// The parent scene node.
    ::Ogre::SceneNode* m_parentSceneNode;

    /// Entity's width.
    ::Ogre::Real m_width;
    /// Entity's height.
    ::Ogre::Real m_height;
    /// Entity's depth.
    ::Ogre::Real m_depth;

    /// Spacing in the texture 3d image file.
    std::vector< ::Ogre::Real > m_spacing;

    /// Where are we insite the depth range?
    float m_relativePosition;

    /// Opacity applied to the entity.
    float m_entityOpacity;

    static unsigned int s_id;
};

//------------------------------------------------------------------------------
// Inline functions

inline void Plane::setOriginPosition(const ::Ogre::Vector3& _origPos)
{
    m_originPosition = _origPos;
}

//------------------------------------------------------------------------------

inline std::vector< ::Ogre::Real > Plane::getDepthSpacing()
{
    return m_spacing;
}

//------------------------------------------------------------------------------

inline Plane::OrientationMode Plane::getOrientationMode() const
{
    return m_orientation;
}

//------------------------------------------------------------------------------

inline ::Ogre::MaterialPtr Plane::getMaterial() const
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

} // Namespace fwRenderOgre
