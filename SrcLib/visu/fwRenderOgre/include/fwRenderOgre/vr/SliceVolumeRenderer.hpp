/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWRENDEROGRE_VR_SLICEVOLUMERENDERER_HPP__
#define __FWRENDEROGRE_VR_SLICEVOLUMERENDERER_HPP__

#include "fwRenderOgre/vr/IVolumeRenderer.hpp"

#include <OGRE/OgreNode.h>

namespace fwRenderOgre
{

namespace vr
{

/**
 * @brief Implements a slice based volume renderer.
 */
class SliceVolumeRenderer : public IVolumeRenderer
{
public:

    /**
     * @brief Constructor.
     * @param parentId             ID of the service using this renderer
     * @param sceneManager         The scene manager being used.
     * @param volumeNode           This object's node.
     * @param imageTexture         Texture holding the 3D image to be rendered.
     * @param gpuTF                Texture holding the transfer function.
     * @param preintegrationTable  Texture holding the pre-integration table.
     */
    FWRENDEROGRE_API SliceVolumeRenderer(std::string parentId,
                                         ::Ogre::SceneManager* sceneManager,
                                         ::Ogre::SceneNode* parentNode,
                                         ::Ogre::TexturePtr imageTexture,
                                         TransferFunction& gpuTF,
                                         PreIntegrationTable& preintegrationTable);

    /// Destructor, does nothing.
    FWRENDEROGRE_API virtual ~SliceVolumeRenderer();

    /// Function called when a new image is being rendered.
    FWRENDEROGRE_API virtual void imageUpdate(::fwData::Image::sptr image, ::fwData::TransferFunction::sptr tf);

    /// Sets the number of slices.
    FWRENDEROGRE_API virtual void setSampling(uint16_t nbSamples);

    /// Sets pre-integrated mode.
    FWRENDEROGRE_API virtual void setPreIntegratedRendering(bool preIntegratedRendering);

private:

    /// Defines a polygon as a list of 3D points.
    typedef std::vector< ::Ogre::Vector3 > Polygon;

    /// Initializes the manual object containing the slices.
    void initSlices();

    /// Updates all the proxy geometry.
    void updateAllSlices();

    /// Updates a slice's mesh.
    void updateSlice(const Polygon& _polygon, const unsigned _sliceIndex);

    /// Adds the intersection (if it exists) between a plane and a segment in the _result polygon.
    bool planeEdgeIntersection(const ::Ogre::Vector3& _planeNormal, const ::Ogre::Vector3& _planeVertex,
                               const unsigned _edgeVertexIndex0, const unsigned _edgeVertexIndex1,
                               Polygon& _result) const;

    /// Returns the intersection between the bounding cube and a plane.
    Polygon cubePlaneIntersection(const ::Ogre::Vector3& _planeNormal, const ::Ogre::Vector3& _planeVertex,
                                  const unsigned _closestVertexIndex) const;

    /// The scene manager's render queue.
    ::Ogre::RenderQueue* m_sceneRenderQueue;

    /// Object containing the proxy geometry used for slice based VR.
    ::Ogre::ManualObject* m_intersectingPolygons;

    /// Listener used to update slices when the object is being rendered.
    struct RenderListener : public ::Ogre::MovableObject::Listener
    {
        SliceVolumeRenderer* m_renderer;

        RenderListener(SliceVolumeRenderer* renderer) :
            m_renderer(renderer)
        {
        }

        //------------------------------------------------------------------------------

        virtual bool objectRendering(const ::Ogre::MovableObject*, const ::Ogre::Camera*)
        {
            m_renderer->updateAllSlices();

            return true; // Always render object
        }

    } *m_renderListener;

};

} // namespace vr

} // namespace fwRenderOgre

#endif // __FWRENDEROGRE_VR_SLICEVOLUMERENDERER_HPP__
