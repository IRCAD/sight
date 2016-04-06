#ifndef __VRWIDGET_HPP__
#define __VRWIDGET_HPP__

#include <fwCom/HasSlots.hpp>
#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>

#include <fwRenderOgre/registry/macros.hpp>
#include <fwRenderOgre/SRender.hpp>

#include <OGRE/OgreCamera.h>
#include <OGRE/OgreSceneNode.h>
#include <OGRE/OgreVector3.h>

namespace fwRenderOgre
{
namespace ui
{

class VRWidget : public ::fwCom::HasSlots
{
public:

    /// Shared pointer type.
    typedef std::shared_ptr< VRWidget > sptr;

    /// Clipping cube faces.
    enum CubeFace
    {
        X_NEGATIVE = 0,
        X_POSITIVE = 1,
        Y_NEGATIVE = 2,
        Y_POSITIVE = 3,
        Z_NEGATIVE = 4,
        Z_POSITIVE = 5
    };

    /// Array of 4 vertex indices.
    typedef std::array< unsigned, 4 > CubeFacePositionList;

    /// Maps a face name to an array of 4 vertex indices.
    typedef std::map< CubeFace, CubeFacePositionList > CubeFacePositionsMap;

    /// Lists vertex indices pairs that form edges.
    typedef std::array< std::pair<unsigned, unsigned>, 12 > CubeEdgeList;

    /// Constructor.
    FWRENDEROGRE_API VRWidget(const std::string id,
                              ::Ogre::SceneNode *parentSceneNode,
                              ::Ogre::Camera    *camera,
                              SRender::sptr renderService,
                              const CubeFacePositionsMap& cubeFaces,
                              const CubeEdgeList& edges,
                              const ::Ogre::Vector3 *imgPositions,
                              ::Ogre::Vector3 *imgClippedPositions) throw();

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
     * @param x  cursor horizontal position before move
     * @param y  cursor vertical position before move
     * @param dx displacement along the horizontal axis.
     * @param dy displacement along the vertical axis.
     */
    FWRENDEROGRE_API void moveClippingBox(int x, int y, int dx, int dy);

    FWRENDEROGRE_API void scaleClippingBox(int x, int y, int dx, int dy);

private:

    /// The current selection mode.
    enum
    {
        NONE,
        BOX,
        CAMERA
    } m_selectionMode;

    /// Get the face's image positions.
    std::array< ::Ogre::Vector3, 4 > getFacePositions(CubeFace _faceName) const;

    /// Get the center of a clipping box face.
    ::Ogre::Vector3 getFaceCenter(CubeFace _faceName) const;

    /// Returns the clipping box's image space positions.
    std::array< ::Ogre::Vector3, 8 > getClippingBoxPositions() const;

    /// Creates the widget objects and scene nodes.
    void initWidgets();

    /// Recomputes the intersection between the volume and it's clipping box.
    void updateClippingCube();

    /// Updates the widget's positions based on the clipping box.
    void updateWidgets();

    /// Highlight a clipping box face.
    void selectFace(CubeFace _faceName);

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

    /// Maps widget objects to their scene node and to a cube face.
    std::map< ::Ogre::MovableObject *, std::pair< CubeFace, ::Ogre::SceneNode * > >  m_widgets;

    /// Maps face name to index positions.
    const CubeFacePositionsMap& m_cubeFaces;

    /// Cube edges.
    const CubeEdgeList& m_edges;

    /// Image positions (in image space).
    const ::Ogre::Vector3 *m_imagePositions;

    /// Image positions after clipping (in image space).
    ::Ogre::Vector3 *m_clippedImagePositions;

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
    ::Ogre::Vector3 pickedBoxPoint;
};

} // namespace ui
} // namespace fwRenderOgre

#endif // __VRWIDGET_HPP__
