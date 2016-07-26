#ifndef __FWRENDEROGRE_UI_VRWIDGET_HPP__
#define __FWRENDEROGRE_UI_VRWIDGET_HPP__

#include <fwCom/HasSlots.hpp>
#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>

#include <fwRenderOgre/registry/macros.hpp>

#include <fwRenderOgre/IVolumeRenderer.hpp>
#include <fwRenderOgre/SRender.hpp>

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
class VRWidget : public ::fwCom::HasSlots
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
                              ::Ogre::SceneNode *parentSceneNode,
                              ::Ogre::Camera    *camera,
                              SRender::sptr renderService,
                              ::Ogre::SceneManager* sceneManager,
                              IVolumeRenderer *renderer) throw();

    /// Destructor.
    FWRENDEROGRE_API virtual ~VRWidget() throw();

    /**
     * @brief Drags a widget toward a screen position.
     *
     * @param _pickedWidget picked widget.
     * @param _screenX cursor's horizontal position.
     * @param _screenY cursor's vertical position.
     */
    FWRENDEROGRE_API void widgetPicked(::Ogre::MovableObject *_pickedWidget, int _screenX, int _screenY);

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

    /// Hides the clipping box and widgets.
    FWRENDEROGRE_API void hide();

    /// Shows the clipping box and widgets.
    FWRENDEROGRE_API void show();

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
    std::array< ::Ogre::Vector3, 4 > getFacePositions(IVolumeRenderer::CubeFace _faceName) const;

    /// Get the center of a clipping box face.
    ::Ogre::Vector3 getFaceCenter(IVolumeRenderer::CubeFace _faceName) const;

    /// Returns the clipping box's image space positions.
    std::array< ::Ogre::Vector3, 8 > getClippingBoxPositions() const;

    /// Creates the widget objects and scene nodes.
    void initWidgets();

    /// Recomputes the intersection between the volume and it's clipping box.
    void updateClippingCube();

    /// Updates the widget's positions based on the clipping box.
    void updateWidgets();

    /// Highlight a clipping box face.
    void selectFace(IVolumeRenderer::CubeFace _faceName);

    /// Unhighlight face.
    void deselectFace();

    /// ID of the service using this widget.
    const std::string m_id;

    /// This object's scene manager.
    ::Ogre::SceneManager *m_sceneManager;

    /// Camera too which the volume is rendered.
    ::Ogre::Camera *m_camera;

    /// Render service in charge of rendering these widgets.
    SRender::sptr m_renderService;

    /// Parent node containing the volume.
    ::Ogre::SceneNode *m_volumeSceneNode;

    /// Node holding widget objects.
    ::Ogre::SceneNode *m_widgetSceneNode;

    /// Maps widget objects to their scene node and to a cube face.
    std::map< ::Ogre::MovableObject *, std::pair< IVolumeRenderer::CubeFace, ::Ogre::SceneNode * > >  m_widgets;

    /// Renders the volume.
    IVolumeRenderer *m_renderer;

    /// Axis aligned clipping cube.
    ::Ogre::Vector3 m_clippingCube[2] = {
        ::Ogre::Vector3(0, 0, 0),
        ::Ogre::Vector3(1, 1, 1)
    };

    /// Object used to display the clipping box.
    ::Ogre::ManualObject *m_boundingBox;

    /// Oject holding the highlighted face geometry.
    ::Ogre::ManualObject *m_selectedFace;

    /// Widget currently being dragged.
    ::Ogre::Entity *m_selectedWidget;

    /// Picked box point.
    ::Ogre::Vector3 m_pickedBoxPoint;

};

} // namespace ui
} // namespace fwRenderOgre

#endif // __FWRENDEROGRE_UI_VRWIDGET_HPP__
