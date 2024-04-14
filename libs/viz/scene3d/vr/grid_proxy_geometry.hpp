/************************************************************************
 *
 * Copyright (C) 2017-2024 IRCAD France
 * Copyright (C) 2017-2021 IHU Strasbourg
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

#include "viz/scene3d/factory/r2vb_renderable.hpp"
#include "viz/scene3d/r2vb_renderable.hpp"
#include "viz/scene3d/texture.hpp"
#include "viz/scene3d/transfer_function.hpp"

#include <data/image.hpp>

#include <OGRE/OgreCamera.h>
#include <OGRE/OgreEntity.h>
#include <OGRE/OgreSceneManager.h>

#include <array>

namespace sight::viz::scene3d::vr
{

/**
 * @brief Proxy geometry used for volume ray tracing.
 *
 * Instead of using a simple cube to define our ray intersection points we can define a tighter geometry depending on
 * the image and the transfer function.
 *
 * The geometry we compute here is quite simple though :
 * - first we divide the image volume in equally sized bricks (set to 8x8x8 empirically for now)
 * - we create a smaller volume where every voxel corresponds to a brick
 * - then for each brick we check if it is empty (contains at least one visible voxel after TF application) and store
 * this value in the smaller volume.
 * - finally we generate a small cube for every non-empty voxel in the grid volume.
 *
 * This is all done on the GPU using shaders and R2VB making it quite efficient.
 * The geometry needs to be recomputed when the TF changes or when the whole image changes.
 */
class SIGHT_VIZ_SCENE3D_CLASS_API grid_proxy_geometry : public r2vb_renderable
{
public:

    /// Creates an instance of the objetc.
    //As the object must be created through Ogre::...::createMovableObject, the constructor cannot be called directly
    static SIGHT_VIZ_SCENE3D_API grid_proxy_geometry* make(
        const std::string& _name,
        Ogre::SceneManager* _scene_manager,
        const viz::scene3d::texture::sptr& _3d_image_texture,
        const viz::scene3d::texture::sptr& _mask_texture,
        const transfer_function::sptr& _tf,
        const std::string& _mtl_name
    );

    /// Constructor, should never be called directly.
    grid_proxy_geometry(const Ogre::String& _name);

    /// Destructor, frees resources if they have been allocated.
    SIGHT_VIZ_SCENE3D_API ~grid_proxy_geometry() override;

    /// Function to be called when the volume changed and its size too. Recomputes texture and geometry.
    SIGHT_VIZ_SCENE3D_API void update_grid_size();

    /// Fills the grid texture and uses it to compute the grid geometry.
    SIGHT_VIZ_SCENE3D_API void compute_grid();

    /// Clip the proxy geometry. Recomputes the geometry.
    /// @pre Clipping box coordinates must be clamped to the volume boundaries.
    SIGHT_VIZ_SCENE3D_API void clip_grid(const Ogre::AxisAlignedBox& _clipping_box);

    /// Get the object's type as a string.
    SIGHT_VIZ_SCENE3D_API const Ogre::String& getMovableType() const override;

    /// Sets the texture for which the grid is computed. Doesn't recompute the grid.
    SIGHT_VIZ_SCENE3D_API void set_3d_image_texture(const viz::scene3d::texture::sptr& _texture);

    /// Sets the mask texture for which the grid is computed. Doesn't recompute the grid.
    SIGHT_VIZ_SCENE3D_API void set_mask_texture(const viz::scene3d::texture::sptr& _texture);

private:

    /// Initializes the structures used for geometry generation.
    void initialize();

    /// Allocates the grid texture, updates the R2VB source geometry and sets shader parameters.
    void setup_grid();

    /// Allocates the source geometry structure for R2VB.
    void initialize_r2_vb_source();

    /// Sets shader parameters and sets up the grid.
    void initialize_grid_materials();

    /// Name of the texture used to store the transfer function
    static inline const std::string TF_TEXUNIT_NAME = "transferFunction";

    /// Entity holding the source geometry used for proxy geometry rendering.
    Ogre::Entity* m_r2vb_source {nullptr};

    /// GPU pass generating the geometry from the grid.
    Ogre::Pass* m_geom_generator_pass {nullptr};

    /// GPU pass filling the 3D texture grid.
    Ogre::Pass* m_grid_computing_pass {nullptr};

    /// Render operation used to compute the brick grid.
    Ogre::RenderOperation m_grid_render_op;

    /// Grid defining the volume bricks.
    Ogre::TexturePtr m_grid_texture {nullptr};

    /// Grid volume dimensions. (i.e. the number of bricks along each dimension)
    std::array<int, 3> m_grid_size {{2, 2, 2}};

    /// Size of a volume brick.
    static constexpr std::array<int, 3> BRICK_SIZE {{8, 8, 8}};

    /// image from which we define a grid.
    viz::scene3d::texture::sptr m_3d_image_texture {nullptr};

    /// image from which we define a grid.
    viz::scene3d::texture::sptr m_mask_texture {nullptr};

    /// Transfer function to be applied to the image.
    transfer_function::wptr m_gpu_tf;

    /// Camera used to compute the grid volume image.
    Ogre::Camera* m_grid_viewport_camera {nullptr};
};

//------------------------------------------------------------------------------
/**
 * @brief factory class generating grid_proxy_geometry objects.
 *
 * This is absolutely necessary for all ogre classes inheriting from Ogre::MovableObject.
 * The factory has to be registered at the ogre root. (@see utils::getOgreRoot)
 */
class grid_proxy_geometry_factory : public viz::scene3d::factory::r2vb_renderable
{
public:

    /// Constructor, does nothing.
    SIGHT_VIZ_SCENE3D_API grid_proxy_geometry_factory()
    = default;

    /// Destructor, does nothing.
    SIGHT_VIZ_SCENE3D_API ~grid_proxy_geometry_factory() override
    = default;

    /// Produced object type name. (i.e. "grid_proxy_geometry")
    SIGHT_VIZ_SCENE3D_API static inline const Ogre::String FACTORY_TYPE_NAME = "grid_proxy_geometry";

    /// Returns the object type name.
    [[nodiscard]] SIGHT_VIZ_SCENE3D_API const Ogre::String& getType() const override
    {
        return grid_proxy_geometry_factory::FACTORY_TYPE_NAME;
    }

#if OGRE_VERSION_PATCH < 9
    //------------------------------------------------------------------------------

    SIGHT_VIZ_SCENE3D_API void destroyInstance(Ogre::MovableObject* obj) override
    {
        delete obj;
    }
#endif

protected:

    //------------------------------------------------------------------------------

    /// Produces a "GridGeometryObject" object.
    SIGHT_VIZ_SCENE3D_API Ogre::MovableObject* createInstanceImpl(
        const Ogre::String& _name,
        const Ogre::NameValuePairList*
        /*params*/
    ) override
    {
        return new viz::scene3d::vr::grid_proxy_geometry(_name);
    }
};

//------------------------------------------------------------------------------
//Inline functions

inline const Ogre::String& grid_proxy_geometry::getMovableType() const
{
    return grid_proxy_geometry_factory::FACTORY_TYPE_NAME;
}

//------------------------------------------------------------------------------

inline void grid_proxy_geometry::set_3d_image_texture(const viz::scene3d::texture::sptr& _texture)
{
    m_3d_image_texture = _texture;
}

//------------------------------------------------------------------------------

inline void grid_proxy_geometry::set_mask_texture(const viz::scene3d::texture::sptr& _texture)
{
    m_mask_texture = _texture;
}

} // namespace sight::viz::scene3d::vr
