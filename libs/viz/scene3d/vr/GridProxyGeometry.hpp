/************************************************************************
 *
 * Copyright (C) 2017-2021 IRCAD France
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
#include "viz/scene3d/TransferFunction.hpp"

#include <data/Image.hpp>

#include <OGRE/OgreCamera.h>
#include <OGRE/OgreEntity.h>
#include <OGRE/OgreSceneManager.h>

#include <array>

namespace sight::viz::scene3d
{

namespace vr
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
class VIZ_SCENE3D_CLASS_API GridProxyGeometry : public R2VBRenderable
{
public:

    static VIZ_SCENE3D_API GridProxyGeometry* New(
        const std::string& _name,
        ::Ogre::SceneManager* _sceneManager,
        ::Ogre::TexturePtr _3DImageTexture,
        const TransferFunction::sptr& _tf,
        const std::string& _mtlName
    );

    /// Constructor, should never be called directly.
    VIZ_SCENE3D_API GridProxyGeometry(const ::Ogre::String& name);

    /// Destructor, frees resources if they have been allocated.
    VIZ_SCENE3D_API ~GridProxyGeometry() override;

    /// Function to be called when the volume changed and its size too. Recomputes texture and geometry.
    VIZ_SCENE3D_API void updateGridSize();

    /// Fills the grid texture and uses it to compute the grid geometry.
    VIZ_SCENE3D_API void computeGrid();

    /// Clip the proxy geometry. Recomputes the geometry.
    /// @pre Clipping box coordinates must be clamped to the volume boundaries.
    VIZ_SCENE3D_API void clipGrid(const ::Ogre::AxisAlignedBox& _clippingBox);

    /// Get the object's type as a string.
    VIZ_SCENE3D_API const ::Ogre::String& getMovableType(void) const override;

    /// Sets the texture for which the grid is computed. Doesn't recompute the grid.
    VIZ_SCENE3D_API void set3DImageTexture(const ::Ogre::TexturePtr& _texture);

private:

    /// Initializes the structures used for geometry generation.
    void initialize();

    /// Allocates the grid texture, updates the R2VB source geometry and sets shader parameters.
    void setupGrid();

    /// Allocates the source geometry structure for R2VB.
    void initializeR2VBSource();

    /// Sets shader parameters and sets up the grid.
    void initializeGridMaterials();

    /// Entity holding the source geometry used for proxy geometry rendering.
    ::Ogre::Entity* m_r2vbSource {nullptr};

    /// GPU pass generating the geometry from the grid.
    ::Ogre::Pass* m_geomGeneratorPass {nullptr};

    /// GPU pass filling the 3D texture grid.
    ::Ogre::Pass* m_gridComputingPass {nullptr};

    /// Render operation used to compute the brick grid.
    ::Ogre::RenderOperation m_gridRenderOp;

    /// Grid defining the volume bricks.
    ::Ogre::TexturePtr m_gridTexture;

    /// Grid volume dimensions. (i.e. the number of bricks along each dimension)
    std::array<int, 3> m_gridSize {{2, 2, 2}};

    /// Size of a volume brick.
    const std::array<int, 3> m_brickSize {{8, 8, 8}};

    /// Image from which we define a grid.
    ::Ogre::TexturePtr m_3DImageTexture;

    /// Transfer function to be applied to the image.
    TransferFunction::wptr m_gpuTF;

    /// Camera used to compute the grid volume image.
    ::Ogre::Camera* m_gridViewportCamera {nullptr};
};

//------------------------------------------------------------------------------
/**
 * @brief Factory class generating GridProxyGeometry objects.
 *
 * This is absolutely necessery for all ogre classes inheriting from ::Ogre::MovableObject.
 * The factory has to be registered at the ogre root. (@see Utils::getOgreRoot)
 */
class GridProxyGeometryFactory : public viz::scene3d::factory::R2VBRenderable
{
public:

    /// Constructor, does nothing.
    VIZ_SCENE3D_API GridProxyGeometryFactory()
    {
    }

    /// Destructor, does nothing.
    VIZ_SCENE3D_API ~GridProxyGeometryFactory() override
    {
    }

    /// Produced object type name. (i.e. "GridProxyGeometry")
    VIZ_SCENE3D_API static ::Ogre::String FACTORY_TYPE_NAME;

    /// Returns the object type name.
    VIZ_SCENE3D_API const ::Ogre::String& getType(void) const override
    {
        return FACTORY_TYPE_NAME;
    }

#if OGRE_VERSION_PATCH < 9
    //------------------------------------------------------------------------------

    VIZ_SCENE3D_API void destroyInstance(::Ogre::MovableObject* obj) override
    {
        delete obj;
    }
#endif

protected:

    //------------------------------------------------------------------------------

    /// Produces a "GridGeometryObject" object.
    VIZ_SCENE3D_API ::Ogre::MovableObject* createInstanceImpl(
        const ::Ogre::String& name,
        const ::Ogre::NameValuePairList*
    ) override
    {
        return new viz::scene3d::vr::GridProxyGeometry(name);
    }
};

} // namespace vr

} // namespace sight::viz::scene3d
