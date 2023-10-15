/************************************************************************
 *
 * Copyright (C) 2017-2023 IRCAD France
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

#include "viz/scene3d/config.hpp"
#include "viz/scene3d/factory/R2VBRenderable.hpp"
#include "viz/scene3d/R2VBRenderable.hpp"
#include "viz/scene3d/Texture.hpp"
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
class VIZ_SCENE3D_CLASS_API grid_proxy_geometry : public R2VBRenderable
{
public:

    /// Creates an instance of the objetc.
    //As the object must be created through Ogre::...::createMovableObject, the constructor cannot be called directly
    static VIZ_SCENE3D_API grid_proxy_geometry* make(
        const std::string& _name,
        Ogre::SceneManager* _sceneManager,
        const viz::scene3d::Texture::sptr& _3DImageTexture,
        const viz::scene3d::Texture::sptr& _maskTexture,
        const transfer_function::sptr& _tf,
        const std::string& _mtlName
    );

    /// Constructor, should never be called directly.
    grid_proxy_geometry(const Ogre::String& name);

    /// Destructor, frees resources if they have been allocated.
    VIZ_SCENE3D_API ~grid_proxy_geometry() override;

    /// Function to be called when the volume changed and its size too. Recomputes texture and geometry.
    VIZ_SCENE3D_API void updateGridSize();

    /// Fills the grid texture and uses it to compute the grid geometry.
    VIZ_SCENE3D_API void computeGrid();

    /// Clip the proxy geometry. Recomputes the geometry.
    /// @pre Clipping box coordinates must be clamped to the volume boundaries.
    VIZ_SCENE3D_API void clipGrid(const Ogre::AxisAlignedBox& _clippingBox);

    /// Get the object's type as a string.
    VIZ_SCENE3D_API const Ogre::String& getMovableType() const override;

    /// Sets the texture for which the grid is computed. Doesn't recompute the grid.
    VIZ_SCENE3D_API void set3DImageTexture(const viz::scene3d::Texture::sptr& _texture);

    /// Sets the mask texture for which the grid is computed. Doesn't recompute the grid.
    VIZ_SCENE3D_API void setMaskTexture(const viz::scene3d::Texture::sptr& _texture);

private:

    /// Initializes the structures used for geometry generation.
    void initialize();

    /// Allocates the grid texture, updates the R2VB source geometry and sets shader parameters.
    void setupGrid();

    /// Allocates the source geometry structure for R2VB.
    void initializeR2VBSource();

    /// Sets shader parameters and sets up the grid.
    void initializeGridMaterials();

    /// Name of the texture used to store the transfer function
    static inline const std::string s_TF_TEXUNIT_NAME = "transferFunction";

    /// Entity holding the source geometry used for proxy geometry rendering.
    Ogre::Entity* m_r2vbSource {nullptr};

    /// GPU pass generating the geometry from the grid.
    Ogre::Pass* m_geomGeneratorPass {nullptr};

    /// GPU pass filling the 3D texture grid.
    Ogre::Pass* m_gridComputingPass {nullptr};

    /// Render operation used to compute the brick grid.
    Ogre::RenderOperation m_gridRenderOp;

    /// Grid defining the volume bricks.
    Ogre::TexturePtr m_gridTexture {nullptr};

    /// Grid volume dimensions. (i.e. the number of bricks along each dimension)
    std::array<int, 3> m_gridSize {{2, 2, 2}};

    /// Size of a volume brick.
    static constexpr std::array<int, 3> s_brickSize {{8, 8, 8}};

    /// image from which we define a grid.
    viz::scene3d::Texture::sptr m_3DImageTexture {nullptr};

    /// image from which we define a grid.
    viz::scene3d::Texture::sptr m_maskTexture {nullptr};

    /// Transfer function to be applied to the image.
    transfer_function::wptr m_gpuTF;

    /// Camera used to compute the grid volume image.
    Ogre::Camera* m_gridViewportCamera {nullptr};
};

//------------------------------------------------------------------------------
/**
 * @brief factory class generating grid_proxy_geometry objects.
 *
 * This is absolutely necessary for all ogre classes inheriting from Ogre::MovableObject.
 * The factory has to be registered at the ogre root. (@see Utils::getOgreRoot)
 */
class grid_proxy_geometry_factory : public viz::scene3d::factory::R2VBRenderable
{
public:

    /// Constructor, does nothing.
    VIZ_SCENE3D_API grid_proxy_geometry_factory()
    = default;

    /// Destructor, does nothing.
    VIZ_SCENE3D_API ~grid_proxy_geometry_factory() override
    = default;

    /// Produced object type name. (i.e. "grid_proxy_geometry")
    VIZ_SCENE3D_API static inline const Ogre::String FACTORY_TYPE_NAME = "grid_proxy_geometry";

    /// Returns the object type name.
    [[nodiscard]] VIZ_SCENE3D_API const Ogre::String& getType() const override
    {
        return grid_proxy_geometry_factory::FACTORY_TYPE_NAME;
    }

#if OGRE_VERSION_PATCH < 9
    //------------------------------------------------------------------------------

    VIZ_SCENE3D_API void destroyInstance(Ogre::MovableObject* obj) override
    {
        delete obj;
    }
#endif

protected:

    //------------------------------------------------------------------------------

    /// Produces a "GridGeometryObject" object.
    VIZ_SCENE3D_API Ogre::MovableObject* createInstanceImpl(
        const Ogre::String& name,
        const Ogre::NameValuePairList*
        /*params*/
    ) override
    {
        return new viz::scene3d::vr::grid_proxy_geometry(name);
    }
};

//------------------------------------------------------------------------------
//Inline functions

inline const Ogre::String& grid_proxy_geometry::getMovableType() const
{
    return grid_proxy_geometry_factory::FACTORY_TYPE_NAME;
}

//------------------------------------------------------------------------------

inline void grid_proxy_geometry::set3DImageTexture(const viz::scene3d::Texture::sptr& _texture)
{
    m_3DImageTexture = _texture;
}

//------------------------------------------------------------------------------

inline void grid_proxy_geometry::setMaskTexture(const viz::scene3d::Texture::sptr& _texture)
{
    m_maskTexture = _texture;
}

} // namespace sight::viz::scene3d::vr
