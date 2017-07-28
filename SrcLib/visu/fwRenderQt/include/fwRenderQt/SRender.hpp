/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWRENDERQT_SRENDER_HPP__
#define __FWRENDERQT_SRENDER_HPP__

#include "fwRenderQt/config.hpp"
#include "fwRenderQt/data/Axis.hpp"
#include "fwRenderQt/data/Coord.hpp"
#include "fwRenderQt/data/Event.hpp"
#include "fwRenderQt/data/Viewport.hpp"

#include <fwCom/helper/SigSlotConnection.hpp>

#include <fwRender/IRender.hpp>

#include <fwServices/helper/Config.hpp>

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QResizeEvent>
#include <Qt>

#include <map>

namespace fwRenderQt
{
// Not declared in the #include because of mutual inclusion.
class Scene2DGraphicsView;

// Not declared in the #include because of mutual inclusion.
class IAdaptor;

/**
 * @brief Scene2D rendering class
 */
class FWRENDERQT_CLASS_API SRender : public ::fwRender::IRender
{

public:

    fwCoreServiceClassDefinitionsMacro( (SRender)(::fwRender::IRender) );

    /// A string type representing adaptors id.
    typedef std::string AdaptorIDType;

    /// A string type representing objects id.
    typedef std::string ObjectIDType;

    /// A float type representing adaptors zValue.
    typedef float AdaptorZValueType;

    /// Constructor, set coordinates m_sceneStart to (-100, -100), m_sceneWidth to (200, 200) and add new handle events
    ///  ADDED_KEYS, REMOVED_KEYS and CHANGED_KEYS.
    FWRENDERQT_API SRender() noexcept;

    /// Basic destructor, do nothing.
    FWRENDERQT_API virtual ~SRender() noexcept;

    /// Get the scene.
    FWRENDERQT_API QGraphicsScene* getScene() const;

    /// Get the view.
    FWRENDERQT_API Scene2DGraphicsView* getView() const;

    /// Get the viewport.
    FWRENDERQT_API ::fwRenderQt::data::Viewport::sptr SRender::getViewport() const;

    /// Get the axis.
    FWRENDERQT_API ::fwRenderQt::data::Axis::sptr getAxis(const std::string& id) const;

    /// Iterate on the m_zValue2AdaptorID map and, if the event hasn't been accepted yet, call the adaptor
    /// processInteraction function.
    FWRENDERQT_API void dispatchInteraction(::fwRenderQt::data::Event& _event );

    /// Returns the viewport coordinate point mapped to scene coordinates.
    FWRENDERQT_API ::fwRenderQt::data::Coord mapToScene( const ::fwRenderQt::data::Coord& coord ) const;

    /// Returns what happens to scene's aspect ratio on view resize events
    FWRENDERQT_API Qt::AspectRatioMode getAspectRatioMode() const;

    /// Update scene size from items bounding rect, this bounding can be enlarged with ratioPercent parameter
    FWRENDERQT_API void updateSceneSize( float ratioPercent = 0 );

protected:

    /**
     * @brief Configuring the SRender service.
     *
     * Example of configuration
     * @code{.xml}
       <service uid="GENERIC_UID_Scene2D" impl="::fwRenderQt::SRender" type="::fwRender::IRender" autoConnect="yes">
        <scene>
            <scene x="-1100" y="-1.1" width="2500" height="1.2" />
            <viewport x="-500" y="-1.1" width="500" height="1.2" />
            <axis id="xAxis" origin="0.0" scale="1.0" scaleType="LINEAR" />
            <axis id="yAxis" origin="0.0" scale="-1.0" scaleType="LINEAR" />
            <axis id="axeHistogramY" origin="0.0" scale="-0.000005" scaleType="LINEAR" />

            <adaptor uid="grid" />

        </scene>

       </service>
       @endcode
     *
     * - \<scene x="-1100" y="-1.1" width="2500" height="1.2" /\> : Set the scene coordinates
     *
     * \b x : mandatory : Set the x coordinate of the top left scene corner
     *
     * \b y : mandatory : Set the y coordinate of the top left scene corner
     *
     * \b width : mandatory : Set the width of the scene
     *
     * \b height : mandatory : Set the height of the scene
     *
     * \b antialiasing :  not mandatory : activate scene antialiasing if attribute's value set to 'true'
     *
     * - \<viewport id="view1" x="-500" y="-1.1" width="500" height="1.2" /\> : Set a viewport coordinates
     *
     * \b id : mandatory : Set the viewport id
     *
     * \b x : mandatory : Set the x coordinate of the top left viewport corner
     *
     * \b y : mandatory : Set the y coordinate of the top left viewport corner
     *
     * \b width : mandatory : Set the width of the viewport
     *
     * \b height : mandatory : Set the height of the viewport
     *
     * - \<axis id="xAxis" origin="0.0" scale="1.0" scaleType="LINEAR" /\> : Set an axis specifications
     *
     * \b id : mandatory : Set the axis id
     *
     * \b origin : mandatory : Set the axis origin
     *
     * \b scale : mandatory : Set the axis scale
     *
     * \b scaleType : mandatory : Set the axis scaleType
     *
     * - \<adaptor id="grid" class="::fwRenderQt::adaptor::GridFromFloat" objectId="myData"\> : Set an adaptor
     *
     * \b id : mandatory : Set the adaptor id
     *
     * \b class : mandatory : Set the adaptor type
     *
     * \b objectId : mandatory : Set the adaptor related object id
     *
     * \b uid : no mandatory : Set the adaptor uid
     *
     *  - \b connect : not mandatory, connects signal to slot
     *   - \b waitForKey : not mandatory, defines the required object key for the signal/slot connection
     *   - \b signal : mandatory, must be signal holder UID, followed by '/', followed by signal name. To use the
     *        object (defined by waitForKey) signal, you don't have to write object uid, only the signal name.
     *   - \b slot : mandatory, must be slot holder UID, followed by '/', followed by slot name
     */
    FWRENDERQT_API void configuring();

    /// Call startContext to set the scene, the viewport and the view,
    /// and add'em to the QtContainer and start the adaptors
    /// contained in the adaptors id vector of the ObjectsID2AdaptorIDVector map.
    FWRENDERQT_API void starting();

    /// Do nothing.
    FWRENDERQT_API void updating();

    FWRENDERQT_API void swapping();

    /// Start/stop adaptors
    FWRENDERQT_API void swapping(const KeyType& key);

    /// Stop all the adaptors attached to the render related composite,
    /// stop all those attached to the objects contained
    /// by the render related composite, clear the maps and call stopContext().
    FWRENDERQT_API void stopping();

private:

    /// A ConfigurationElement type representing a configuration.
    typedef SPTR (::fwRuntime::ConfigurationElement) ConfigurationType;

    /// Create the QtContainer, the scene, the viewport, the view.
    void startContext();

    /// Delete m_scene and m_view;
    void stopContext();

    /// Get the axis configuration specifications
    void configureAxis ( ConfigurationType _conf );

    /// Get the viewport configuration specifications, create a new viewport.
    void configureViewport ( ConfigurationType _conf );

    /// Get the scene configuration specifications and set them to m_sceneStart and m_sceneWidth.
    void configureScene( ConfigurationType _conf );

    /// Get the adaptor configuration specifications, push back the id attribute in the m_objectsID2AdaptorIDVector map,
    /// create a SceneAdaptor2D,
    /// set its attributes and push it back in the m_adaptorID2SceneAdaptor2D map.
    void configureAdaptor ( ConfigurationType _conf );

    typedef std::map< AdaptorZValueType, AdaptorIDType > ZValue2AdaptorID;

    /// Map of AdaptorIDType referenced by AdaptorZValueType;
    ZValue2AdaptorID m_zValue2AdaptorID;

    std::map<std::string, ::fwRenderQt::data::Axis::sptr > m_axisMap;

    /// The render configuration.
    ConfigurationType m_sceneConfiguration;

    /// Coordinates of the scene top left corner.
    ::fwRenderQt::data::Coord m_sceneStart;

    /// Width and height of the scene.
    ::fwRenderQt::data::Coord m_sceneWidth;

    /// The scene.
    QGraphicsScene* m_scene;

    /// The view.
    Scene2DGraphicsView* m_view;

    ::fwRenderQt::data::Viewport::sptr m_viewport;

    /// If antialiasing is requested (deactivated by default because of a potential lack of performance)
    bool m_antialiasing;

    /// How the scene should behave on view resize events
    Qt::AspectRatioMode m_aspectRatioMode;
};

} // namespace fwRenderQt

#endif // __FWRENDERQT_SRENDER_HPP__

