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

#include "viz/scene3d/factory/r2vb_renderable.hpp"
#include "viz/scene3d/r2vb_renderable.hpp"

#include <viz/scene3d/ogre.hpp>

#include <OGRE/OgreMaterialManager.h>
#include <OGRE/OgreRenderQueue.h>

namespace sight::viz::scene3d
{

//-----------------------------------------------------------------------------

viz::scene3d::r2vb_renderable* viz::scene3d::r2vb_renderable::make(
    const std::string& _name,
    Ogre::SubEntity* _source_object,
    Ogre::SceneManager* _scene_manager,
    data::mesh::cell_type_t _primitive_type,
    const std::string& _mtl_name
)
{
    const auto& factory_name = viz::scene3d::factory::r2vb_renderable::FACTORY_TYPE_NAME;
    auto* instance           = static_cast<viz::scene3d::r2vb_renderable*>
                               (_scene_manager->createMovableObject(_name, factory_name));

    instance->m_inputPrimitiveType = _primitive_type;
    instance->m_srcObject          = _source_object;
    instance->mManager             = _scene_manager;

    // Input material name

    Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().getByName(_mtl_name, RESOURCE_GROUP);
    instance->setMaterial(mat);
    return instance;
}

//-----------------------------------------------------------------------------

viz::scene3d::r2vb_renderable::r2vb_renderable(const Ogre::String& _name) :
    SimpleRenderable(_name)
{
}

//-----------------------------------------------------------------------------

void viz::scene3d::r2vb_renderable::setOutputSettings(
    std::size_t _vertex_count,
    bool _has_color,
    bool _has_tex_coord,
    bool _has_normals
)
{
    if(m_maxOutputVertexCount < _vertex_count)
    {
        const std::string r2vb_material = (m_r2vbBuffer) ? m_r2vbBuffer->getRenderToBufferMaterial()->getName() : "";
        m_r2vbBuffer = Ogre::HardwareBufferManager::getSingleton().createRenderToVertexBuffer();

        m_r2vbBuffer->setOperationType(Ogre::RenderOperation::OT_TRIANGLE_LIST);
        m_r2vbBuffer->setResetsEveryUpdate(true);

        const std::size_t num_vertices = m_inputPrimitiveType == data::mesh::cell_type_t::QUAD ? _vertex_count * 2
                                                                                               : m_inputPrimitiveType
                                         == data::mesh::cell_type_t::TETRA ? _vertex_count * 4
                                                                           :
                                         _vertex_count;
        m_r2vbBuffer->setMaxVertexCount(static_cast<unsigned int>(num_vertices));
        m_r2vbBuffer->setSourceRenderable(m_srcObject);
        if(!r2vb_material.empty())
        {
            m_r2vbBuffer->setRenderToBufferMaterialName(r2vb_material);
        }

        m_maxOutputVertexCount = _vertex_count;
    }

    // Define feedback vertex declarations
    Ogre::VertexDeclaration* vtx_decl = m_r2vbBuffer->getVertexDeclaration();
    vtx_decl->removeAllElements();

    std::size_t ofst = 0;
    ofst += vtx_decl->addElement(0, ofst, Ogre::VET_FLOAT3, Ogre::VES_POSITION).getSize();
    if(_has_normals)
    {
        ofst += vtx_decl->addElement(0, ofst, Ogre::VET_FLOAT3, Ogre::VES_NORMAL).getSize();
    }

    if(_has_color)
    {
        ofst += vtx_decl->addElement(0, ofst, Ogre::VET_COLOUR, Ogre::VES_DIFFUSE).getSize();
    }

    if(_has_tex_coord)
    {
        vtx_decl->addElement(0, ofst, Ogre::VET_FLOAT2, Ogre::VES_TEXTURE_COORDINATES).getSize();
    }

    // Set bounds.
    this->setBoundingBox(m_srcObject->getParent()->getBoundingBox());

    this->setDirty();
}

//-----------------------------------------------------------------------------

const Ogre::String& r2vb_renderable::getMovableType() const
{
    return factory::r2vb_renderable::FACTORY_TYPE_NAME;
}

//-----------------------------------------------------------------------------

void r2vb_renderable::_updateRenderQueue(Ogre::RenderQueue* _queue)
{
    // Don't do anything if the object is not visible
    if(m_srcObject->getParent()->isVisible())
    {
        // Update the ouput vertex buffer only if the dirty flag is set
        if(m_dirty)
        {
            m_r2vbBuffer->update(mManager);
            m_dirty = false;
        }

        // Add the output vertex buffer in the render queue
        _queue->addRenderable(this, mRenderQueueID, mRenderQueuePriority);
    }
}

//-----------------------------------------------------------------------------

void r2vb_renderable::getRenderOperation(Ogre::RenderOperation& _op)
{
    m_r2vbBuffer->getRenderOperation(_op);
}

//------------------------------------------------------------------------------

const Ogre::MeshPtr& r2vb_renderable::getMesh() const
{
    return m_srcObject->getParent()->getMesh();
}

//-----------------------------------------------------------------------------

} // namespace sight::viz::scene3d
