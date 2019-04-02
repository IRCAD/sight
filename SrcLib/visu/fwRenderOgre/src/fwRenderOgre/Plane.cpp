/************************************************************************
 *
 * Copyright (C) 2014-2019 IRCAD France
 * Copyright (C) 2014-2019 IHU Strasbourg
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

#include "fwRenderOgre/Plane.hpp"

#include <fwRenderOgre/helper/Shading.hpp>
#include <fwRenderOgre/Utils.hpp>

#include <OGRE/OgreEntity.h>
#include <OGRE/OgreMaterialManager.h>
#include <OGRE/OgreMeshManager.h>
#include <OGRE/OgreMovablePlane.h>
#include <OGRE/OgreSceneManager.h>
#include <OGRE/OgreSceneNode.h>

namespace fwRenderOgre
{
unsigned int Plane::s_id = 0;

//-----------------------------------------------------------------------------

Plane::Plane( const ::fwTools::fwID::IDType& _negatoId, ::Ogre::SceneNode* _parentSceneNode,
              ::Ogre::SceneManager* _sceneManager, OrientationMode _orientation, bool _is3D,
              ::Ogre::TexturePtr _tex, FilteringEnumType _filtering, float _entityOpacity) :
    m_is3D( _is3D ),
    m_threshold(false),
    m_filtering( _filtering ),
    m_orientation( _orientation ),
    m_originPosition(0.f, 0.f, 0.f),
    m_texture( _tex ),
    m_sceneManager( _sceneManager ),
    m_parentSceneNode( _parentSceneNode ),
    m_width(0.f),
    m_height(0.f),
    m_depth(0.f),
    m_relativePosition(0.8f),
    m_entityOpacity( _entityOpacity )
{
    // names definition
    switch(m_orientation)
    {
        case OrientationMode::X_AXIS:
            m_slicePlaneName = _negatoId + "_Sagittal_Mesh";
            m_entityName     = _negatoId + "_Sagittal_Entity";
            m_sceneNodeName  = _negatoId + "_Sagittal_SceneNode";
            break;
        case OrientationMode::Y_AXIS:
            m_slicePlaneName = _negatoId + "_Frontal_Mesh";
            m_entityName     = _negatoId + "_Frontal_Entity";
            m_sceneNodeName  = _negatoId + "_Frontal_SceneNode";
            break;
        case OrientationMode::Z_AXIS:
            m_slicePlaneName = _negatoId + "_Axial_Mesh";
            m_entityName     = _negatoId + "_Axial_Entity";
            m_sceneNodeName  = _negatoId + "_Axial_SceneNode";
            break;
    }

    // Creates the parent's child scene node positionned at (0; 0; 0)
    m_planeSceneNode = m_parentSceneNode->createChildSceneNode( m_sceneNodeName );
    this->initializeMaterial();
}

//-----------------------------------------------------------------------------

Plane::~Plane()
{
    m_parentSceneNode->removeAndDestroyChild(m_planeSceneNode);
}

//-----------------------------------------------------------------------------

void Plane::initializeMaterial()
{
    ::Ogre::MaterialPtr defaultMat = m_is3D ? ::Ogre::MaterialManager::getSingleton().getByName("Negato3D")
                                     : ::Ogre::MaterialManager::getSingleton().getByName("Negato2D");

    m_texMaterial = ::Ogre::MaterialManager::getSingleton().create(
        "NegatoMat" + std::to_string(s_id),
        ::Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    s_id++;

    defaultMat->copyDetailsTo(m_texMaterial);

    ::Ogre::ColourValue diffuse(1.f, 1.f, 1.f, m_entityOpacity);
    m_texMaterial->setDiffuse(diffuse);

    int orientationIndex = 0;

    switch (m_orientation)
    {
        case OrientationMode::X_AXIS:
            orientationIndex = 0;
            break;
        case OrientationMode::Y_AXIS:
            orientationIndex = 1;
            break;
        case OrientationMode::Z_AXIS:
            orientationIndex = 2;
            break;
    }

    // This is necessary to load and compile the material, otherwise the following technique iterator
    // is null when we call this method on the first time (from doStart() for instance)
    m_texMaterial->touch();

    const ::Ogre::Material::Techniques& techniques = m_texMaterial->getTechniques();

    for(const auto tech : techniques)
    {
        SLM_ASSERT("Technique is not set", tech);

        if(::fwRenderOgre::helper::Shading::isColorTechnique(*tech))
        {
            ::Ogre::Pass* pass = tech->getPass(0);

            ::Ogre::TextureUnitState* texState = pass->getTextureUnitState("image");
            SLM_ASSERT("'image' texture unit is not found", texState);
            texState->setTexture(m_texture);

            ::Ogre::TextureFilterOptions filterType = ::Ogre::TFO_NONE;
            switch(m_filtering)
            {
                case FilteringEnumType::NONE:
                    filterType = ::Ogre::TFO_NONE;
                    break;
                case FilteringEnumType::LINEAR:
                    filterType = ::Ogre::TFO_BILINEAR;
                    break;
                case FilteringEnumType::ANISOTROPIC:
                    filterType = ::Ogre::TFO_ANISOTROPIC;
                    break;
            }

            // Sets the texture filtering in the current texture unit state according to the negato's interpolation flag
            texState->setTextureFiltering(filterType);

            pass->getVertexProgramParameters()->setNamedConstant("u_orientation", orientationIndex);
            pass->getFragmentProgramParameters()->setNamedConstant("u_orientation", orientationIndex);
        }
    }
}

//-----------------------------------------------------------------------------

void Plane::setDepthSpacing(std::vector<double> _spacing)
{
    m_spacing.clear();

    for (auto spacingValue : _spacing)
    {
        m_spacing.push_back(static_cast< ::Ogre::Real >(spacingValue));
    }
}

//-----------------------------------------------------------------------------

void Plane::initialize3DPlane()
{
    ::Ogre::MeshManager* meshManager = ::Ogre::MeshManager::getSingletonPtr();

    // First delete mesh if it already exists
    if(meshManager->resourceExists(m_slicePlaneName))
    {
        meshManager->remove(m_slicePlaneName);
    }

    if( m_sceneManager->hasEntity(m_entityName))
    {
        m_sceneManager->getEntity( m_entityName)->detachFromParent();
        m_sceneManager->destroyEntity(m_entityName);
    }

    ::Ogre::MovablePlane* plane = setDimensions();

    // Mesh plane instanciation:
    // Y is the default upVector,
    // so if we want a plane which normal is the Y unit vector we have to create it differently
    if(m_orientation == ::fwDataTools::helper::MedicalImage::Orientation::Y_AXIS)
    {
        m_slicePlane = ::Ogre::MeshManager::getSingletonPtr()->createPlane(
            m_slicePlaneName,
            ::Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
            *plane,
            m_width, m_height,
            1, 1,
            true,
            1, 1.0, 1.0,
            ::Ogre::Vector3::UNIT_Z);
    }
    else
    {
        m_slicePlane = ::Ogre::MeshManager::getSingletonPtr()->createPlane(
            m_slicePlaneName,
            ::Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
            *plane,
            m_width, m_height);
    }

    // Entity creation
    ::Ogre::Entity* planeEntity = m_sceneManager->createEntity(m_entityName, m_slicePlane);
    planeEntity->setMaterial(m_texMaterial);
    m_planeSceneNode->attachObject(planeEntity);

    this->initializePosition();
}

//-----------------------------------------------------------------------------

void Plane::initialize2DPlane()
{
    ::Ogre::MeshManager* meshManager = ::Ogre::MeshManager::getSingletonPtr();

    // First delete mesh if it already exists
    if(meshManager->resourceExists(m_slicePlaneName))
    {
        meshManager->remove(m_slicePlaneName);
    }

    if( m_sceneManager->hasEntity(m_entityName))
    {
        m_sceneManager->getEntity( m_entityName)->detachFromParent();
        m_sceneManager->destroyEntity(m_entityName);
    }

    ::Ogre::MovablePlane* plane = setDimensions();

    // Mesh plane instanciation
    m_slicePlane = ::Ogre::MeshManager::getSingletonPtr()->createPlane(
        m_slicePlaneName,
        ::Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
        *plane,
        m_width, m_height);

    // Entity creation
    ::Ogre::Entity* planeEntity = m_sceneManager->createEntity(m_entityName, m_slicePlane);
    planeEntity->setMaterial(m_texMaterial);
    m_planeSceneNode->attachObject(planeEntity);

    this->initializePosition();
}

//-----------------------------------------------------------------------------

void Plane::initializePosition()
{
    this->moveToOriginPosition();

    if(m_is3D)
    {
        switch(m_orientation)
        {
            case OrientationMode::X_AXIS:
                m_planeSceneNode->translate(0, m_height / 2, m_width / 2);
                break;
            case OrientationMode::Y_AXIS:
                m_planeSceneNode->translate(m_width / 2, 0, m_height / 2);
                break;
            case OrientationMode::Z_AXIS:
                m_planeSceneNode->translate(m_width / 2, m_height / 2, 0);
                break;
        }
    }
    else
    {
        m_planeSceneNode->resetOrientation();
        switch(m_orientation)
        {
            case OrientationMode::X_AXIS:
                m_planeSceneNode->rotate(::Ogre::Vector3(0, 1, 0), ::Ogre::Degree(::Ogre::Real(90)));
                m_planeSceneNode->rotate(::Ogre::Vector3(0, 0, 1), ::Ogre::Degree(::Ogre::Real(90)));
                m_planeSceneNode->translate(0, m_width/2, m_height/2);
                break;
            case OrientationMode::Y_AXIS:
                m_planeSceneNode->rotate(::Ogre::Vector3(1, 0, 0), ::Ogre::Degree(::Ogre::Real(270)));
                m_planeSceneNode->translate(m_width/2, 0, m_height/2);
                break;
            case OrientationMode::Z_AXIS:
                m_planeSceneNode->translate(m_width/2, m_height/2, 0);
                break;
        }
    }
}

//-----------------------------------------------------------------------------

void Plane::moveAlongAxis()
{
    SLM_ASSERT("2D Plane, cannot move along its normal.", m_is3D);

    this->initializePosition();
    ::Ogre::Real distance = m_relativePosition * m_depth;

    switch(m_orientation)
    {
        case OrientationMode::X_AXIS:
            m_planeSceneNode->translate(distance, 0, 0);
            break;
        case OrientationMode::Y_AXIS:
            m_planeSceneNode->translate(0, distance, 0);
            break;
        case OrientationMode::Z_AXIS:
            m_planeSceneNode->translate(0, 0, distance);
            break;
    }
}

//-----------------------------------------------------------------------------

void Plane::setRelativePosition(float _relativePosition)
{
    if( _relativePosition <= 0 )
    {
        m_relativePosition = 0;
    }
    else if ( _relativePosition >= 1)
    {
        // as close as possible from 1, but smaller.
        m_relativePosition = 0.999999999999999f;
    }
    else
    {
        m_relativePosition = _relativePosition;
    }
}

//-----------------------------------------------------------------------------

void Plane::setTFData(const ::fwRenderOgre::TransferFunction& _tfTexture)
{
    const ::Ogre::Material::Techniques& techniques = m_texMaterial->getTechniques();

    for(const auto tech : techniques)
    {
        SLM_ASSERT("Technique is not set", tech);

        if(::fwRenderOgre::helper::Shading::isColorTechnique(*tech))
        {
            ::Ogre::Pass* pass = tech->getPass(0);
            _tfTexture.bind(pass, "tfTexture", pass->getFragmentProgramParameters());
        }
    }
}

//-----------------------------------------------------------------------------

void Plane::switchThresholding(bool _threshold)
{
    const ::Ogre::Material::Techniques& techniques = m_texMaterial->getTechniques();

    for(const auto tech : techniques)
    {
        SLM_ASSERT("Technique is not set", tech);

        if(::fwRenderOgre::helper::Shading::isColorTechnique(*tech))
        {
            ::Ogre::Pass* pass = tech->getPass(0);

            SLM_ASSERT("Can't find Ogre pass", pass);

            m_threshold = _threshold;
            pass->getFragmentProgramParameters()->setNamedConstant("u_threshold", static_cast<int>(m_threshold));
        }
    }
}

//-----------------------------------------------------------------------------

void Plane::moveToOriginPosition()
{
    m_planeSceneNode->setPosition(m_originPosition);
}

//------------------------------------------------------------------------------

double Plane::getSliceWorldPosition() const
{
    ::Ogre::Real position = 0.f;

    switch(m_orientation)
    {
        case OrientationMode::X_AXIS:
            position = m_planeSceneNode->getPosition().x;
            break;
        case OrientationMode::Y_AXIS:
            position = m_planeSceneNode->getPosition().y;
            break;
        case OrientationMode::Z_AXIS:
            position = m_planeSceneNode->getPosition().z;
            break;
    }

    return static_cast<double>(position);
}

//------------------------------------------------------------------------------

void Plane::setOrientationMode(OrientationMode _newMode)
{
    SLM_ASSERT("3D negato Plane, cannot change orientation", !m_is3D);

    m_orientation = _newMode;
    this->initializeMaterial();
    this->initialize2DPlane();
}

//------------------------------------------------------------------------------

void Plane::enableAlpha(bool _enable)
{
    const ::Ogre::Material::Techniques& techniques = m_texMaterial->getTechniques();

    for(const auto tech : techniques)
    {
        SLM_ASSERT("Technique is not set", tech);

        if(::fwRenderOgre::helper::Shading::isColorTechnique(*tech))
        {
            ::Ogre::Pass* pass = tech->getPass(0);

            SLM_ASSERT("Can't find Ogre pass", pass);
            pass->getFragmentProgramParameters()->setNamedConstant("u_enableAlpha", _enable );
        }
    }

}

//------------------------------------------------------------------------------

void Plane::setEntityOpacity(float _f)
{
    SLM_ASSERT("2D negato Plane, cannot change opacity", m_is3D);

    m_entityOpacity = _f;

    ::Ogre::ColourValue diffuse(1.f, 1.f, 1.f, m_entityOpacity);
    m_texMaterial->setDiffuse(diffuse);

    ::Ogre::Technique* tech = m_texMaterial->getTechnique(0);
    SLM_ASSERT("Technique is not set", tech);

    if(::fwRenderOgre::helper::Shading::isColorTechnique(*tech) &&
       !::fwRenderOgre::helper::Shading::isPeelTechnique(*tech))
    {
        ::Ogre::Pass* pass = tech->getPass(0);

        // We don't want a depth check if we have non-OIT transparency
        const bool needDepthCheck = (m_entityOpacity == 1.f);
        pass->setDepthCheckEnabled(needDepthCheck);
    }
}

//------------------------------------------------------------------------------

void Plane::setVisible(bool _visible)
{
    m_planeSceneNode->setVisible(_visible, true);
}

//------------------------------------------------------------------------------

void Plane::changeSlice(float sliceIndex)
{
    const ::Ogre::Material::Techniques& techniques = m_texMaterial->getTechniques();

    for(const auto tech : techniques)
    {
        SLM_ASSERT("Technique is not set", tech);

        if(::fwRenderOgre::helper::Shading::isColorTechnique(*tech))
        {
            ::Ogre::Pass* pass = tech->getPass(0);

            SLM_ASSERT("Can't find Ogre pass", pass);

            pass->getFragmentProgramParameters()->setNamedConstant("u_slice", sliceIndex );
        }
    }

    if (m_is3D)
    {
        this->setRelativePosition( sliceIndex );
        this->moveAlongAxis();
    }
}

//-----------------------------------------------------------------------------

::Ogre::MovablePlane* Plane::setDimensions()
{
    ::Ogre::Real tex_width  = static_cast< ::Ogre::Real >( m_texture->getWidth() );
    ::Ogre::Real tex_height = static_cast< ::Ogre::Real >( m_texture->getHeight() );
    ::Ogre::Real tex_depth  = static_cast< ::Ogre::Real >( m_texture->getDepth() );

    ::Ogre::MovablePlane* plane = nullptr;
    switch(m_orientation)
    {
        case OrientationMode::X_AXIS:
            m_width  = tex_depth * m_spacing[2];
            m_height = tex_height * m_spacing[1];
            m_depth  = tex_width * m_spacing[0];
            break;
        case OrientationMode::Y_AXIS:
            m_width  = tex_width * m_spacing[0];
            m_height = tex_depth * m_spacing[2];
            m_depth  = tex_height * m_spacing[1];
            break;
        case OrientationMode::Z_AXIS:
            m_width  = tex_width * m_spacing[0];
            m_height = tex_height * m_spacing[1];
            m_depth  = tex_depth * m_spacing[2];
            break;
    }

    if(m_is3D)
    {
        switch(m_orientation)
        {
            case OrientationMode::X_AXIS:
                plane = new ::Ogre::MovablePlane(::Ogre::Vector3::UNIT_X, 0);
                break;
            case OrientationMode::Y_AXIS:
                plane = new ::Ogre::MovablePlane(::Ogre::Vector3::UNIT_Y, 0);
                break;
            case OrientationMode::Z_AXIS:
                plane = new ::Ogre::MovablePlane(::Ogre::Vector3::UNIT_Z, 0);
                break;
        }
    }
    else
    {
        plane = new ::Ogre::MovablePlane(::Ogre::Vector3::UNIT_Z, 0);
        // It's more convenient to display the Negato2D's sagittal plane horizontally.
        if (m_orientation == OrientationMode::X_AXIS)
        {
            m_height = (tex_depth -1) * m_spacing[2];
            m_width  = (tex_height -1) * m_spacing[1];
            m_depth  = (tex_width -1) * m_spacing[0];
        }
    }

    return plane;
}

//-----------------------------------------------------------------------------
} // namespace fwRenderOgre
