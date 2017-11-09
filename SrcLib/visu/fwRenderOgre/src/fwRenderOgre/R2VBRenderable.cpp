/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRenderOgre/factory/R2VBRenderable.hpp"
#include "fwRenderOgre/R2VBRenderable.hpp"

#include <OGRE/OgreMaterialManager.h>
#include <OGRE/OgreRenderQueue.h>

namespace fwRenderOgre
{

//-----------------------------------------------------------------------------

fwRenderOgre::R2VBRenderable* fwRenderOgre::R2VBRenderable::New(const std::string& _name,
                                                                ::Ogre::SubEntity* _sourceObject,
                                                                ::Ogre::SceneManager* _sceneManager,
                                                                ::fwData::Mesh::CellTypesEnum _primitiveType,
                                                                const std::string& _mtlName)
{
    const auto& factoryName = ::fwRenderOgre::factory::R2VBRenderable::FACTORY_TYPE_NAME;
    auto instance           = static_cast< ::fwRenderOgre::R2VBRenderable*>
                              (_sceneManager->createMovableObject(_name, factoryName));

    instance->m_inputPrimitiveType = _primitiveType;
    instance->m_srcObject          = _sourceObject;
    instance->mParentSceneManager  = _sceneManager;

    // Input material name

    ::Ogre::MaterialPtr mat = ::Ogre::MaterialManager::getSingleton().getByName(_mtlName);
    instance->setMaterial(mat);
    return instance;
}

//-----------------------------------------------------------------------------

fwRenderOgre::R2VBRenderable::R2VBRenderable(const ::Ogre::String& _name) :
    SimpleRenderable(_name),
    m_dirty(false),
    m_inputPrimitiveType(::fwData::Mesh::TRIANGLE),
    m_maxOutputVertexCount(0)
{
}

//-----------------------------------------------------------------------------

void fwRenderOgre::R2VBRenderable::setOutputSettings(size_t _vertexCount, bool _hasColor, bool _hasTexCoord,
                                                     const std::string& _mtlName)
{
    if(!m_r2vbBuffer || m_maxOutputVertexCount < _vertexCount)
    {
        m_r2vbBuffer = ::Ogre::HardwareBufferManager::getSingleton().createRenderToVertexBuffer();

        m_r2vbBuffer->setRenderToBufferMaterialName( _mtlName );
        m_r2vbBuffer->setOperationType(::Ogre::RenderOperation::OT_TRIANGLE_LIST);
        m_r2vbBuffer->setResetsEveryUpdate(true);

        const size_t numVertices = m_inputPrimitiveType == ::fwData::Mesh::QUAD ? _vertexCount * 2 :
                                   m_inputPrimitiveType == ::fwData::Mesh::TETRA ? _vertexCount * 4 :
                                   _vertexCount;
        m_r2vbBuffer->setMaxVertexCount(static_cast<unsigned int>(numVertices));
        m_r2vbBuffer->setSourceRenderable(m_srcObject);

        m_maxOutputVertexCount = _vertexCount;
    }

    // Define feedback vertex declarations
    ::Ogre::VertexDeclaration* vtxDecl = m_r2vbBuffer->getVertexDeclaration();
    size_t ofst = 0;
    ofst += vtxDecl->addElement(0, ofst, ::Ogre::VET_FLOAT3, ::Ogre::VES_POSITION).getSize();
    ofst += vtxDecl->addElement(0, ofst, ::Ogre::VET_FLOAT3, ::Ogre::VES_NORMAL).getSize();
    if(_hasColor)
    {
        ofst += vtxDecl->addElement(0, ofst, ::Ogre::VET_UBYTE4, ::Ogre::VES_DIFFUSE).getSize();
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
        _queue->addRenderable(this);
    }
}

//-----------------------------------------------------------------------------

void R2VBRenderable::getRenderOperation(::Ogre::RenderOperation& _op)
{
    m_r2vbBuffer->getRenderOperation(_op);
}

//-----------------------------------------------------------------------------

} // namespace fwRenderOgre
