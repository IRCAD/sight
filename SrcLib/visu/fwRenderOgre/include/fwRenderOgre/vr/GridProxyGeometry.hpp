/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "fwRenderOgre/config.hpp"
#include "fwRenderOgre/factory/R2VBRenderable.hpp"
#include "fwRenderOgre/R2VBRenderable.hpp"
#include "fwRenderOgre/TransferFunction.hpp"

#include <fwData/Image.hpp>

#include <OGRE/OgreEntity.h>
#include <OGRE/OgreSceneManager.h>

#include <array>

namespace fwRenderOgre
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
class FWRENDEROGRE_CLASS_API GridProxyGeometry : public R2VBRenderable
{
public:

    static FWRENDEROGRE_API GridProxyGeometry* New(const std::string& _name, ::Ogre::SceneManager* _sceneManager,
                                                   ::Ogre::TexturePtr _3DImageTexture, TransferFunction& _tf,
                                                   const std::string& _mtlName);

    /// Constructor, should never be called directly.
    FWRENDEROGRE_API GridProxyGeometry(const ::Ogre::String& name);

    /// Destructor, frees resources if they have been allocated.
    FWRENDEROGRE_API virtual ~GridProxyGeometry();

    /// Function to be called when the volume changed and its size too. Recomputes texture and geometry.
    FWRENDEROGRE_API void updateGridSize(const ::Ogre::Vector2& _tfWindow);

    /// Fills the grid texture and uses it to compute the grid geometry.
    FWRENDEROGRE_API void computeGrid(const ::Ogre::Vector2& _tfWindow);

    /// Clip the proxy geometry. Recomputes the geometry.
    FWRENDEROGRE_API void clipGrid(const ::Ogre::AxisAlignedBox& _clippingBox);

    /// Get the object's type as a string.
    FWRENDEROGRE_API virtual const ::Ogre::String& getMovableType(void) const override;

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
    ::Ogre::Entity* m_r2vbSource;

    /// Render operation used to compute the brick grid.
    ::Ogre::RenderOperation m_gridRenderOp;

    /// Grid defining the volume bricks.
    ::Ogre::TexturePtr m_gridTexture;

    /// Grid volume dimensions. (i.e. the number of bricks along each dimension)
    std::array< int, 3 > m_gridSize;

    /// Size of a volume brick.
    std::array< int, 3 > m_brickSize;

    /// Image from which we define a grid.
    ::Ogre::TexturePtr m_3DImageTexture;

    /// Transfer function to be applied to the image.
    TransferFunction m_gpuTF;

};

//------------------------------------------------------------------------------
/**
 * @brief Factory class generating GridProxyGeometry objects.
 *
 * This is absolutely necessery for all ogre classes inheriting from ::Ogre::MovableObject.
 * The factory has to be registered at the ogre root. (@see Utils::getOgreRoot)
 */
class GridProxyGeometryFactory : public ::fwRenderOgre::factory::R2VBRenderable
{
public:

    /// Constructor, does nothing.
    FWRENDEROGRE_API GridProxyGeometryFactory()
    {
    }

    /// Destructor, does nothing.
    FWRENDEROGRE_API ~GridProxyGeometryFactory()
    {
    }

    /// Produced object type name. (i.e. "GridProxyGeometry")
    static ::Ogre::String FACTORY_TYPE_NAME;

    /// Returns the object type name.
    FWRENDEROGRE_API virtual const ::Ogre::String& getType(void) const override
    {
        return FACTORY_TYPE_NAME;
    }

    //------------------------------------------------------------------------------

    /// Deletes an instance of a produced object.
    FWRENDEROGRE_API virtual void destroyInstance( ::Ogre::MovableObject* obj) override
    {
        delete obj;
    }

protected:

    //------------------------------------------------------------------------------

    /// Produces a "GridGeometryObject" object.
    FWRENDEROGRE_API ::Ogre::MovableObject* createInstanceImpl(const ::Ogre::String& name,
                                                               const ::Ogre::NameValuePairList* ) override
    {
        return new ::fwRenderOgre::vr::GridProxyGeometry(name);
    }
};

} // namespace vr
} // namespace fwRenderOgre
