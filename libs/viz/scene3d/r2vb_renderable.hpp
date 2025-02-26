/************************************************************************
 *
 * Copyright (C) 2014-2025 IRCAD France
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

#include <sight/viz/scene3d/config.hpp>

#include "viz/scene3d/material/r2vb.hpp"

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
class SIGHT_VIZ_SCENE3D_CLASS_API r2vb_renderable : public Ogre::SimpleRenderable
{
public:

    /// Create a new instance of r2vb_renderable
    SIGHT_VIZ_SCENE3D_API static r2vb_renderable* make(
        const std::string& _name,
        Ogre::SubEntity* _source_object,
        Ogre::SceneManager* _scene_manager,
        data::mesh::cell_type_t _primitive_type,
        const std::string& _mtl_name
    );

    SIGHT_VIZ_SCENE3D_API r2vb_renderable(const Ogre::String& _name);
    SIGHT_VIZ_SCENE3D_API ~r2vb_renderable() override;

    /// Set the maximum number of vertices in output, and adjust the size of the output buffer accordingly.
    /// It also updates the vertex declaration of the output buffer
    SIGHT_VIZ_SCENE3D_API void set_output_settings(
        std::size_t _vertex_count,
        bool _has_color,
        bool _has_tex_coord,
        bool _has_normals = true
    );

    /** @copydoc SimpleRenderable::_updateRenderQueue. */
    SIGHT_VIZ_SCENE3D_API void _updateRenderQueue(Ogre::RenderQueue* _queue) override;

    /** @copydoc SimpleRenderable::getMovableType. */
    SIGHT_VIZ_SCENE3D_API const Ogre::String& getMovableType() const override;

    /** @copydoc SimpleRenderable::getRenderOperation. */
    SIGHT_VIZ_SCENE3D_API void getRenderOperation(Ogre::RenderOperation& _op) override;

    /// Delegate to the subentity.
    SIGHT_VIZ_SCENE3D_API Ogre::Real getBoundingRadius() const override;

    /// @copydoc Renderable::getSquaredViewDepth
    SIGHT_VIZ_SCENE3D_API Ogre::Real getSquaredViewDepth(const Ogre::Camera* _cam) const override;

    /// Return the parent mesh.
    SIGHT_VIZ_SCENE3D_API virtual const Ogre::MeshPtr& get_mesh() const;

    /// Mark the output vertex buffer as dirty, the r2vb process will be run on next update
    SIGHT_VIZ_SCENE3D_API void set_dirty();

    /// Runs the R2VB process.
    SIGHT_VIZ_SCENE3D_API void manual_update();

    /// Set the material used to process the geometry pass.
    SIGHT_VIZ_SCENE3D_API void set_render_to_buffer_material(const std::string& _mtl_name);

    viz::scene3d::material::r2vb::uptr material;

    data::mesh::cell_type_t get_input_primitive_type() const;

protected:

    /// Source object of the r2vb process
    Ogre::SubEntity* m_src_object {};

    /// Buffer used as output
    Ogre::RenderToVertexBufferSharedPtr m_r2vb_buffer;

    /// Tells if the r2vb must be run on next update - typically we want this to be done, at worst, only once per frame.
    /// Thus we use this flag, depending on the technique to enable the r2vb only on the first rendering pass.
    bool m_dirty {false};

    /// Primitive type used as input
    data::mesh::cell_type_t m_input_primitive_type {data::mesh::cell_type_t::triangle};

    /// Maximum number of vertices in output buffer
    std::size_t m_max_output_vertex_count {0};
};

//-----------------------------------------------------------------------------
// Inline functions

//-----------------------------------------------------------------------------

inline r2vb_renderable::~r2vb_renderable()
{
    m_src_object = nullptr;
}

//-----------------------------------------------------------------------------

inline Ogre::Real r2vb_renderable::getBoundingRadius() const
{
    return m_src_object->getParent()->getBoundingRadius();
}

//-----------------------------------------------------------------------------

inline Ogre::Real r2vb_renderable::getSquaredViewDepth(const Ogre::Camera* _cam) const
{
    return this->getParentNode()->getSquaredViewDepth(_cam);
}

//-----------------------------------------------------------------------------

inline void r2vb_renderable::set_dirty()
{
    m_dirty = true;
}

//-----------------------------------------------------------------------------

inline void r2vb_renderable::manual_update()
{
    m_r2vb_buffer->update(mManager);
}

//-----------------------------------------------------------------------------

inline void r2vb_renderable::set_render_to_buffer_material(const std::string& _mtl_name)
{
    m_r2vb_buffer->setRenderToBufferMaterialName(_mtl_name);
}

//------------------------------------------------------------------------------

inline data::mesh::cell_type_t r2vb_renderable::get_input_primitive_type() const
{
    return m_input_primitive_type;
}

} // namespace sight::viz::scene3d
