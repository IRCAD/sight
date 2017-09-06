/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2016-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWRENDEROGRE_UI_VRWIDGET_HPP__
#define __FWRENDEROGRE_UI_VRWIDGET_HPP__

#include <fwRenderOgre/registry/macros.hpp>
#include <fwRenderOgre/SRender.hpp>
#include <fwRenderOgre/vr/IVolumeRenderer.hpp>

#include <fwData/TransformationMatrix3D.hpp>

#include <OGRE/OgreCamera.h>
#include <OGRE/OgreSceneNode.h>
#include <OGRE/OgreVector3.h>

namespace fwRenderOgre
{

namespace ui
{

/**
 * @brief Holds the clipping widgets, used to clip/move/scale the volume.
 */
class VRWidget
{
public:

    /// Shared pointer type.
    typedef std::shared_ptr< VRWidget > sptr;

    /**
     * @brief Constructor.
     *
     * @param id                   parent service id.
     * @param parentSceneNode      holds the volume object.
     * @param camera               render target.
     * @param renderService        service that renders this scene.
     * @param sceneManager         scene manager handling thos object.
     * @param cubeFaces            maps cube faces to vertex indices.
     * @param edges                all pairs of vertex indices forming edges.
     * @param imgPositions         image bounding box positions.
     * @param imgClippedPositions  image positions after clipping.
     */
    FWRENDEROGRE_API VRWidget(const std::string id,
                              ::Ogre::SceneNode* parentSceneNode,
                              ::Ogre::Camera* camera,
                              SRender::sptr renderService,
                              ::Ogre::SceneManager* sceneManager,
                              ::fwRenderOgre::vr::IVolumeRenderer* renderer,
                              ::fwData::TransformationMatrix3D::sptr clippingMatrix) noexcept;

    /// Destructor.
    FWRENDEROGRE_API virtual ~VRWidget() noexcept;

    /**
     * @brief Drags a widget toward a screen position.
     *
     * @param _pickedWidget picked widget.
     * @param _screenX cursor's horizontal position.
     * @param _screenY cursor's vertical position.
     */
    FWRENDEROGRE_API void widgetPicked(::Ogre::MovableObject* _pickedWidget, int _screenX, int _screenY);

    /// Drops the currently selected widget.
    FWRENDEROGRE_API void widgetReleased();

    /**
     * @brief Translates the clipping box along the screen's axes.
     *
     * @param x  cursor current horizontal position.
     * @param y  cursor current vertical position.
     * @param dx displacement along the horizontal axis.
     * @param dy displacement along the vertical axis.
     */
    FWRENDEROGRE_API void moveClippingBox(int x, int y, int dx, int dy);

    /**
     * @brief Scales the clipping box.
     *
     * @param x  cursor current horizontal position.
     * @param y  cursor current vertical position.
     * @param dy displacement along the vertical axis, used to compute scale factor.
     */
    FWRENDEROGRE_API void scaleClippingBox(int x, int y, int dy);

    /// Returns whether the widgets are visible or hidden.
    FWRENDEROGRE_API bool getVisibility() const;

    /// Hides or shows the widget.
    FWRENDEROGRE_API void setVisibility(bool visibility);

private:

    /// The current selection mode.
    enum
    {
        NONE,
        BOX,
        CAMERA
    } m_selectionMode;

    /// Get the face's image positions.
    std::array< ::Ogre::Vector3, 4 > getFacePositions(::fwRenderOgre::vr::IVolumeRenderer::CubeFace _faceName) const;

    /// Get the center of a clipping box face.
    ::Ogre::Vector3 getFaceCenter(::fwRenderOgre::vr::IVolumeRenderer::CubeFace _faceName) const;

    /// Returns the clipping box's image space positions.
    std::array< ::Ogre::Vector3, 8 > getClippingBoxPositions() const;

    /// Creates the widget objects and scene nodes.
    void initWidgets();

    /// Recomputes the intersection between the volume and it's clipping box.
    void updateClippingCube();

    /// Updates the widget's positions based on the clipping box.
    void updateWidgets();

    /// Highlight a clipping box face.
    void selectFace(::fwRenderOgre::vr::IVolumeRenderer::CubeFace _faceName);

    /// Unhighlight face.
    void deselectFace();

    /// ID of the service using this widget.
    const std::string m_id;

    /// This object's scene manager.
    ::Ogre::SceneManager* m_sceneManager;

    /// Camera too which the volume is rendered.
    ::Ogre::Camera* m_camera;

    /// Render service in charge of rendering these widgets.
    SRender::sptr m_renderService;

    /// Parent node containing the volume.
    ::Ogre::SceneNode* m_volumeSceneNode;

    /// Node holding widget objects.
    ::Ogre::SceneNode* m_widgetSceneNode;

    /// Maps widget objects to their scene node and to a cube face.
    std::map< ::Ogre::MovableObject*,
              std::pair< ::fwRenderOgre::vr::IVolumeRenderer::CubeFace, ::Ogre::SceneNode* > >  m_widgets;

    /// Renders the volume.
    ::fwRenderOgre::vr::IVolumeRenderer* m_renderer;

    /// Axis aligned clipping cube.
    std::array< ::Ogre::Vector3, 2> m_clippingCube;

    /// Object used to display the clipping box.
    ::Ogre::ManualObject* m_boundingBox;

    /// Oject holding the highlighted face geometry.
    ::Ogre::ManualObject* m_selectedFace;

    /// Widget currently being dragged.
    ::Ogre::Entity* m_selectedWidget;

    /// Picked box point.
    ::Ogre::Vector3 m_pickedBoxPoint;

    /// Materials
    ::Ogre::MaterialPtr m_sphereHighlightMtl, m_frameMtl, m_frameHighlightMtl, m_faceMtl;

    /// Pointer to the adaptor clipping matrix
    ::fwData::TransformationMatrix3D::sptr m_clippingMatrix;
};

} // namespace ui
} // namespace fwRenderOgre

#endif // __FWRENDEROGRE_UI_VRWIDGET_HPP__
