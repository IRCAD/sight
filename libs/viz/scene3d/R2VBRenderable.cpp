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

#include "viz/scene3d/factory/R2VBRenderable.hpp"
#include "viz/scene3d/R2VBRenderable.hpp"

#include <viz/scene3d/ogre.hpp>

#include <OGRE/OgreMaterialManager.h>
#include <OGRE/OgreRenderQueue.h>

namespace sight::viz::scene3d
{

//-----------------------------------------------------------------------------

viz::scene3d::R2VBRenderable* viz::scene3d::R2VBRenderable::New(
    const std::string& _name,
    ::Ogre::SubEntity* _sourceObject,
    ::Ogre::SceneManager* _sceneManager,
    data::Mesh::CellTypesEnum _primitiveType,
    const std::string& _mtlName
)
{
    data::Mesh::CellType type = data::Mesh::CellType::NO_CELL;
    switch(_primitiveType)
    {
        case data::Mesh::POINT:
            type = data::Mesh::CellType::POINT;
            break;

        case data::Mesh::EDGE:
            type = data::Mesh::CellType::EDGE;
            break;

        case data::Mesh::TRIANGLE:
            type = data::Mesh::CellType::TRIANGLE;
            break;

        case data::Mesh::QUAD:
            type = data::Mesh::CellType::QUAD;
            break;

        case data::Mesh::TETRA:
            type = data::Mesh::CellType::TETRA;
            break;

        default:
            type = data::Mesh::CellType::NO_CELL;
    }

    return viz::scene3d::R2VBRenderable::New(_name, _sourceObject, _sceneManager, type, _mtlName);
}

//-----------------------------------------------------------------------------

viz::scene3d::R2VBRenderable* viz::scene3d::R2VBRenderable::New(
    const std::string& _name,
    ::Ogre::SubEntity* _sourceObject,
    ::Ogre::SceneManager* _sceneManager,
    data::Mesh::CellType _primitiveType,
    const std::string& _mtlName
)
{
    const auto& factoryName = viz::scene3d::factory::R2VBRenderable::FACTORY_TYPE_NAME;
    auto instance           = static_cast<viz::scene3d::R2VBRenderable*>
                              (_sceneManager->createMovableObject(_name, factoryName));

    instance->m_inputPrimitiveType = _primitiveType;
    instance->m_srcObject          = _sourceObject;
    instance->mParentSceneManager  = _sceneManager;

    // Input material name

    ::Ogre::MaterialPtr mat = ::Ogre::MaterialManager::getSingleton().getByName(_mtlName, RESOURCE_GROUP);
    instance->setMaterial(mat);
    return instance;
}

//-----------------------------------------------------------------------------

viz::scene3d::R2VBRenderable::R2VBRenderable(const ::Ogre::String& _name) :
    SimpleRenderable(_name),
    m_dirty(false),
    m_inputPrimitiveType(data::Mesh::CellType::TRIANGLE),
    m_maxOutputVertexCount(0)
{
}

//-----------------------------------------------------------------------------

void viz::scene3d::R2VBRenderable::setOutputSettings(
    size_t _vertexCount,
    bool _hasColor,
    bool _hasTexCoord,
    bool _hasNormals
)
{
    if(m_maxOutputVertexCount < _vertexCount)
    {
        const std::string r2vbMaterial = (m_r2vbBuffer) ? m_r2vbBuffer->getRenderToBufferMaterial()->getName() : "";
        m_r2vbBuffer = ::Ogre::HardwareBufferManager::getSingleton().createRenderToVertexBuffer();

        m_r2vbBuffer->setOperationType(::Ogre::RenderOperation::OT_TRIANGLE_LIST);
        m_r2vbBuffer->setResetsEveryUpdate(true);

        const size_t numVertices = m_inputPrimitiveType == data::Mesh::CellType::QUAD ? _vertexCount * 2
                                                                                      : m_inputPrimitiveType
                                   == data::Mesh::CellType::TETRA ? _vertexCount * 4
                                                                  :
                                   _vertexCount;
        m_r2vbBuffer->setMaxVertexCount(static_cast<unsigned int>(numVertices));
        m_r2vbBuffer->setSourceRenderable(m_srcObject);
        if(!r2vbMaterial.empty())
        {
            m_r2vbBuffer->setRenderToBufferMaterialName(r2vbMaterial);
        }

        m_maxOutputVertexCount = _vertexCount;
    }

    // Define feedback vertex declarations
    ::Ogre::VertexDeclaration* vtxDecl = m_r2vbBuffer->getVertexDeclaration();
    vtxDecl->removeAllElements();

    size_t ofst = 0;
    ofst += vtxDecl->addElement(0, ofst, ::Ogre::VET_FLOAT3, ::Ogre::VES_POSITION).getSize();
    if(_hasNormals)
    {
        ofst += vtxDecl->addElement(0, ofst, ::Ogre::VET_FLOAT3, ::Ogre::VES_NORMAL).getSize();
    }

    if(_hasColor)
    {
        ofst += vtxDecl->addElement(0, ofst, ::Ogre::VET_COLOUR, ::Ogre::VES_DIFFUSE).getSize();
    }

    if(_hasTexCoord)
    {
        ofst += vtxDecl->addElement(0, ofst, ::Ogre::VET_FLOAT2, ::Ogre::VES_TEXTURE_COORDINATES).getSize();
    }

    // Set bounds.
    this->setBoundingBox(m_srcObject->getParent()->getBoundingBox());

    this->setDirty();
}

//-----------------------------------------------------------------------------

const ::Ogre::String& R2VBRenderable::getMovableType(void) const
{
    return factory::R2VBRenderable::FACTORY_TYPE_NAME;
}

//-----------------------------------------------------------------------------

void R2VBRenderable::_updateRenderQueue(::Ogre::RenderQueue* _queue)
{
    // Don't do anything if the object is not visible
    if(m_srcObject->getParent()->isVisible())
    {
        // Update the ouput vertex buffer only if the dirty flag is set
        if(m_dirty)
        {
            m_r2vbBuffer->update(mParentSceneManager);
            m_dirty = false;
        }

        // Add the output vertex buffer in the render queue
        _queue->addRenderable(this, mRenderQueueID, mRenderQueuePriority);
    }
}

//-----------------------------------------------------------------------------

void R2VBRenderable::getRenderOperation(::Ogre::RenderOperation& _op)
{
    m_r2vbBuffer->getRenderOperation(_op);
}

//------------------------------------------------------------------------------

const Ogre::MeshPtr& R2VBRenderable::getMesh() const
{
    return m_srcObject->getParent()->getMesh();
}

//-----------------------------------------------------------------------------

} // namespace sight::viz::scene3d
