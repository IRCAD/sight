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

#include "viz/scene3d/adaptor.hpp"
#include "viz/scene3d/config.hpp"

#include <data/mesh.hpp>

#include <OGRE/OgreEntity.h>
#include <OGRE/OgreManualObject.h>
#include <OGRE/OgreNode.h>
#include <OGRE/OgreRenderToVertexBuffer.h>
#include <OGRE/OgreSceneManager.h>
#include <OGRE/OgreSimpleRenderable.h>
#include <OGRE/OgreSubEntity.h>

namespace sight::viz::scene3d
{

/**
 * @brief This object is used to implement a render-to-vertex-buffer (r2vb) process (GL_TRANSFORM_FEEDBACK).
 *
 * This objects holds a reference to the object used as input for the render-to-vertex-buffer process.
 * It also contains the output vertex buffer, that is used to be displayed like a "regular" object.
 */
class VIZ_SCENE3D_CLASS_API R2VBRenderable : public Ogre::SimpleRenderable
{
public:

    /// Create a new instance of R2VBRenderable
    VIZ_SCENE3D_API static R2VBRenderable* make(
        const std::string& _name,
        Ogre::SubEntity* _sourceObject,
        Ogre::SceneManager* _sceneManager,
        data::mesh::CellType _primitiveType,
        const std::string& _mtlName
    );

    VIZ_SCENE3D_API R2VBRenderable(const Ogre::String& name);
    VIZ_SCENE3D_API ~R2VBRenderable() override;

    /// Set the maximum number of vertices in output, and adjust the size of the output buffer accordingly.
    /// It also updates the vertex declaration of the output buffer
    VIZ_SCENE3D_API void setOutputSettings(
        std::size_t _vertexCount,
        bool _hasColor,
        bool _hasTexCoord,
        bool _hasNormals = true
    );

    /** @copydoc SimpleRenderable::_updateRenderQueue. */
    VIZ_SCENE3D_API void _updateRenderQueue(Ogre::RenderQueue* _queue) override;

    /** @copydoc SimpleRenderable::getMovableType. */
    VIZ_SCENE3D_API const Ogre::String& getMovableType() const override;

    /** @copydoc SimpleRenderable::getRenderOperation. */
    VIZ_SCENE3D_API void getRenderOperation(Ogre::RenderOperation& _op) override;

    /// Delegate to the subentity.
    VIZ_SCENE3D_API Ogre::Real getBoundingRadius() const override;

    /// @copydoc Renderable::getSquaredViewDepth
    VIZ_SCENE3D_API Ogre::Real getSquaredViewDepth(const Ogre::Camera* _cam) const override;

    /// Return the parent mesh.
    VIZ_SCENE3D_API virtual const Ogre::MeshPtr& getMesh() const;

    /// Mark the output vertex buffer as dirty, the r2vb process will be run on next update
    VIZ_SCENE3D_API void setDirty();

    /// Runs the R2VB process.
    VIZ_SCENE3D_API void manualUpdate();

    /// Set the material used to process the geometry pass.
    VIZ_SCENE3D_API void setRenderToBufferMaterial(const std::string& _mtlName);

    viz::scene3d::adaptor::wptr m_materialAdaptor;

    data::mesh::CellType getInputPrimitiveType() const;

protected:

    /// Source object of the r2vb process
    Ogre::SubEntity* m_srcObject {};

    /// Buffer used as output
    Ogre::RenderToVertexBufferSharedPtr m_r2vbBuffer;

    /// Tells if the r2vb must be run on next update - typically we want this to be done, at worst, only once per frame.
    /// Thus we use this flag, depending on the technique to enable the r2vb only on the first rendering pass.
    bool m_dirty {false};

    /// Primitive type used as input
    data::mesh::CellType m_inputPrimitiveType {data::mesh::CellType::TRIANGLE};

    /// Maximum number of vertices in output buffer
    std::size_t m_maxOutputVertexCount {0};
};

//-----------------------------------------------------------------------------
// Inline functions

//-----------------------------------------------------------------------------

inline R2VBRenderable::~R2VBRenderable()
{
    m_srcObject = nullptr;
}

//-----------------------------------------------------------------------------

inline Ogre::Real R2VBRenderable::getBoundingRadius() const
{
    return m_srcObject->getParent()->getBoundingRadius();
}

//-----------------------------------------------------------------------------

inline Ogre::Real R2VBRenderable::getSquaredViewDepth(const Ogre::Camera* _cam) const
{
    return this->getParentNode()->getSquaredViewDepth(_cam);
}

//-----------------------------------------------------------------------------

inline void R2VBRenderable::setDirty()
{
    m_dirty = true;
}

//-----------------------------------------------------------------------------

inline void R2VBRenderable::manualUpdate()
{
    m_r2vbBuffer->update(mParentSceneManager);
}

//-----------------------------------------------------------------------------

inline void R2VBRenderable::setRenderToBufferMaterial(const std::string& _mtlName)
{
    m_r2vbBuffer->setRenderToBufferMaterialName(_mtlName);
}

//------------------------------------------------------------------------------

inline data::mesh::CellType R2VBRenderable::getInputPrimitiveType() const
{
    return m_inputPrimitiveType;
}

} // namespace sight::viz::scene3d
