/************************************************************************
 *
 * Copyright (C) 2014-2020 IRCAD France
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

//-----------------------------------------------------------------------------

Plane::Plane(const ::fwTools::fwID::IDType& _negatoId, ::Ogre::SceneNode* _parentSceneNode,
             ::Ogre::SceneManager* _sceneManager, OrientationMode _orientation, ::Ogre::TexturePtr _tex,
             FilteringEnumType _filtering, float _entityOpacity, bool _displayBorder) :
    m_filtering(_filtering),
    m_orientation(_orientation),
    m_texture(_tex),
    m_sceneManager(_sceneManager),
    m_parentSceneNode(_parentSceneNode),
    m_entityOpacity(_entityOpacity),
    m_displayBorder(_displayBorder)
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
    m_planeSceneNode->removeAndDestroyAllChildren();
    m_parentSceneNode->removeAndDestroyChild(m_planeSceneNode);

    if(m_sceneManager->hasEntity(m_entityName))
    {
        m_sceneManager->destroyEntity(m_entityName);
    }

    if(m_slicePlane)
    {
        ::Ogre::MeshManager::getSingleton().remove(m_slicePlane);
    }

    if(m_border)
    {
        m_sceneManager->destroyManualObject(m_border);
    }

    if(m_texMaterial)
    {
        ::Ogre::MaterialManager::getSingleton().remove(m_texMaterial);
    }

    if(m_borderMaterial)
    {
        ::Ogre::MaterialManager::getSingleton().remove(m_borderMaterial);
    }
}

//-----------------------------------------------------------------------------

void Plane::initializeMaterial()
{
    auto& materialMgr = ::Ogre::MaterialManager::getSingleton();
    ::Ogre::MaterialPtr defaultMat = materialMgr.getByName("Negato");

    // If the texture material exists, delete it.
    if(m_texMaterial)
    {
        materialMgr.remove(m_texMaterial);
        m_texMaterial.reset();
    }

    m_texMaterial = defaultMat->clone(m_slicePlaneName + "_TextMaterial");

    const ::Ogre::ColourValue diffuse(1.f, 1.f, 1.f, m_entityOpacity);
    m_texMaterial->setDiffuse(diffuse);

    const int orientationIndex = static_cast<int>(m_orientation);

    const ::Ogre::Material::Techniques& techniques = m_texMaterial->getTechniques();

    for(const auto* const tech : techniques)
    {
        SLM_ASSERT("Technique is not set", tech);

        if(::fwRenderOgre::helper::Shading::isColorTechnique(*tech))
        {
            ::Ogre::Pass* const pass = tech->getPass(0);

            ::Ogre::TextureUnitState* const texState = pass->getTextureUnitState("image");
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

    if(m_displayBorder)
    {
        // If the border material exist, delete it.
        if(m_borderMaterial)
        {
            materialMgr.remove(m_borderMaterial);
            m_borderMaterial.reset();
        }

        m_borderMaterial = ::Ogre::MaterialManager::getSingleton().getByName("BasicAmbient");
        m_borderMaterial = m_borderMaterial->clone(m_slicePlaneName + "_BorderMaterial");
        if(m_orientation == ::fwDataTools::helper::MedicalImage::Orientation::X_AXIS)
        {
            m_borderMaterial->setAmbient(::Ogre::ColourValue::Red);
            m_borderMaterial->setDiffuse(::Ogre::ColourValue::Red);
        }
        else if(m_orientation == ::fwDataTools::helper::MedicalImage::Orientation::Y_AXIS)
        {
            m_borderMaterial->setAmbient(::Ogre::ColourValue::Green);
            m_borderMaterial->setDiffuse(::Ogre::ColourValue::Green);
        }
        else
        {
            m_borderMaterial->setAmbient(::Ogre::ColourValue::Blue);
            m_borderMaterial->setDiffuse(::Ogre::ColourValue::Blue);
        }
        m_borderMaterial->setDepthCheckEnabled(false);
    }
}

//-----------------------------------------------------------------------------

void Plane::setVoxelSpacing(const ::Ogre::Vector3& _spacing)
{
    m_spacing = _spacing;
}

//-----------------------------------------------------------------------------

void Plane::initializePlane()
{
    ::Ogre::MeshManager& meshManager = ::Ogre::MeshManager::getSingleton();

    // First delete mesh if it already exists.
    if(meshManager.resourceExists(m_slicePlaneName))
    {
        meshManager.remove(m_slicePlaneName);
        m_slicePlane.reset();
    }

    if(m_sceneManager->hasEntity(m_entityName))
    {
        m_sceneManager->getEntity( m_entityName)->detachFromParent();
        m_sceneManager->destroyEntity(m_entityName);
    }

    ::Ogre::MovablePlane plane = this->setDimensions();

    // Mesh plane instanciation:
    // Y is the default upVector,
    // so if we want a plane which normal is the Y unit vector we have to create it differently.
    if(m_orientation == ::fwDataTools::helper::MedicalImage::Orientation::Y_AXIS)
    {
        m_slicePlane = meshManager.createPlane(
            m_slicePlaneName,
            ::Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
            plane,
            m_width, m_height,
            1, 1,
            true,
            1, 1.0, 1.0,
            ::Ogre::Vector3::UNIT_Z);
    }
    else
    {
        m_slicePlane = meshManager.createPlane(
            m_slicePlaneName,
            ::Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
            plane,
            m_width, m_height);
    }

    // Entity creation.
    ::Ogre::Entity* planeEntity = m_sceneManager->createEntity(m_entityName, m_slicePlane);
    planeEntity->setMaterial(m_texMaterial);
    m_planeSceneNode->attachObject(planeEntity);

    if(m_displayBorder)
    {
        if(m_border)
        {
            m_sceneManager->destroyManualObject(m_border);
            m_border = nullptr;
        }

        // Create the border.
        m_border = m_sceneManager->createManualObject(m_slicePlaneName + "_Border");
        m_border->estimateVertexCount(5);
        m_border->begin(m_borderMaterial, ::Ogre::RenderOperation::OT_LINE_STRIP);

        if(m_orientation == ::fwDataTools::helper::MedicalImage::Orientation::X_AXIS)
        {
            m_border->position(::Ogre::Vector3(0, -m_height/2.f, -m_width/2.f));
            m_border->position(::Ogre::Vector3(0, m_height/2.f, -m_width/2.f));
            m_border->position(::Ogre::Vector3(0, m_height/2.f, m_width/2.f));
            m_border->position(::Ogre::Vector3(0, -m_height/2.f, m_width/2.f));
            m_border->position(::Ogre::Vector3(0, -m_height/2.f, -m_width/2.f));
        }
        else if(m_orientation == ::fwDataTools::helper::MedicalImage::Orientation::Y_AXIS)
        {
            m_border->position(::Ogre::Vector3(-m_width/2.f, 0, -m_height/2.f));
            m_border->position(::Ogre::Vector3(m_width/2.f, 0, -m_height/2.f));
            m_border->position(::Ogre::Vector3(m_width/2.f, 0, m_height/2.f));
            m_border->position(::Ogre::Vector3(-m_width/2.f, 0, m_height/2.f));
            m_border->position(::Ogre::Vector3(-m_width/2.f, 0, -m_height/2.f));
        }
        else
        {
            m_border->position(::Ogre::Vector3(-m_height/2.f, -m_width/2.f, 0));
            m_border->position(::Ogre::Vector3(m_height/2.f, -m_width/2.f, 0));
            m_border->position(::Ogre::Vector3(m_height/2.f, m_width/2.f, 0));
            m_border->position(::Ogre::Vector3(-m_height/2.f, m_width/2.f, 0));
            m_border->position(::Ogre::Vector3(-m_height/2.f, -m_width/2.f, 0));
        }

        m_border->end();
        m_planeSceneNode->attachObject(m_border);
    }

    this->initializePosition();
}

//-----------------------------------------------------------------------------

void Plane::initializePosition()
{
    this->moveToOriginPosition();

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

//-----------------------------------------------------------------------------

void Plane::moveAlongAxis()
{
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
    m_orientation = _newMode;
    const ::Ogre::Material::Techniques& techniques = m_texMaterial->getTechniques();

    for(const auto* const tech : techniques)
    {
        SLM_ASSERT("Technique is not set", tech);

        if(::fwRenderOgre::helper::Shading::isColorTechnique(*tech))
        {
            const ::Ogre::Pass* const pass = tech->getPass(0);
            SLM_ASSERT("Material '" + m_texMaterial->getName() + "' does not define any pass.", pass);

            const int orientationIndex = static_cast<int>(_newMode);
            pass->getVertexProgramParameters()->setNamedConstant("u_orientation", orientationIndex);
            pass->getFragmentProgramParameters()->setNamedConstant("u_orientation", orientationIndex);
        }
    }

    if(m_displayBorder)
    {
        if(m_orientation == ::fwDataTools::helper::MedicalImage::Orientation::X_AXIS)
        {
            m_borderMaterial->setAmbient(::Ogre::ColourValue::Red);
            m_borderMaterial->setDiffuse(::Ogre::ColourValue::Red);
        }
        else if(m_orientation == ::fwDataTools::helper::MedicalImage::Orientation::Y_AXIS)
        {
            m_borderMaterial->setAmbient(::Ogre::ColourValue::Green);
            m_borderMaterial->setDiffuse(::Ogre::ColourValue::Green);
        }
        else
        {
            m_borderMaterial->setAmbient(::Ogre::ColourValue::Blue);
            m_borderMaterial->setDiffuse(::Ogre::ColourValue::Blue);
        }
    }

    this->initializePlane();
}

//------------------------------------------------------------------------------

void Plane::enableAlpha(bool _enable)
{
    const ::Ogre::Material::Techniques& techniques = m_texMaterial->getTechniques();

    for(const auto* const tech : techniques)
    {
        SLM_ASSERT("Technique is not set", tech);

        if(::fwRenderOgre::helper::Shading::isColorTechnique(*tech))
        {
            ::Ogre::Pass* const pass = tech->getPass(0);

            SLM_ASSERT("Can't find Ogre pass", pass);
            pass->getFragmentProgramParameters()->setNamedConstant("u_enableAlpha", _enable );
        }
    }

}

//------------------------------------------------------------------------------

void Plane::setEntityOpacity(float _f)
{
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

    this->setRelativePosition( sliceIndex );
    this->moveAlongAxis();
}

//-----------------------------------------------------------------------------

::Ogre::MovablePlane Plane::setDimensions()
{
    ::Ogre::Real tex_width  = static_cast< ::Ogre::Real >( m_texture->getWidth() );
    ::Ogre::Real tex_height = static_cast< ::Ogre::Real >( m_texture->getHeight() );
    ::Ogre::Real tex_depth  = static_cast< ::Ogre::Real >( m_texture->getDepth() );

    ::Ogre::MovablePlane plane(::Ogre::Vector3::ZERO, 0);
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

    switch(m_orientation)
    {
        case OrientationMode::X_AXIS:
            plane = ::Ogre::MovablePlane(::Ogre::Vector3::UNIT_X, 0);
            break;
        case OrientationMode::Y_AXIS:
            plane = ::Ogre::MovablePlane(::Ogre::Vector3::UNIT_Y, 0);
            break;
        case OrientationMode::Z_AXIS:
            plane = ::Ogre::MovablePlane(::Ogre::Vector3::UNIT_Z, 0);
            break;
    }

    return plane;
}

//-----------------------------------------------------------------------------

const ::Ogre::MovableObject* Plane::getMovableObject() const
{
    if(m_sceneManager->hasEntity(m_entityName))
    {
        return m_sceneManager->getEntity(m_entityName);
    }
    return nullptr;
}

//-----------------------------------------------------------------------------

void Plane::setQueryFlags(std::uint32_t _flags)
{
    m_sceneManager->getEntity(m_entityName)->setQueryFlags(_flags);
}

//-----------------------------------------------------------------------------

void Plane::setRenderQueuerGroupAndPriority(std::uint8_t _groupId, std::uint16_t _priority)
{
    m_sceneManager->getEntity(m_entityName)->setRenderQueueGroupAndPriority(_groupId, _priority);
}

//-----------------------------------------------------------------------------
} // namespace fwRenderOgre
