/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __SCENE2D_RENDER_HPP__
#define __SCENE2D_RENDER_HPP__

#include "scene2D/config.hpp"
#include "scene2D/data/Axis.hpp"
#include "scene2D/data/Coord.hpp"
#include "scene2D/data/Event.hpp"
#include "scene2D/data/Viewport.hpp"

#include <fwCom/helper/SigSlotConnection.hpp>

#include <fwData/Composite.hpp>

#include <fwRender/IRender.hpp>

#include <fwServices/helper/Config.hpp>

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QResizeEvent>
#include <Qt>

//namespace fwData

namespace scene2D
{
// Not declared in the #include because of mutual inclusion.
class Scene2DGraphicsView;

namespace adaptor
{
// Not declared in the #include because of mutual inclusion.
class IAdaptor;
}

/**
 * @brief Scene2D rendering class
 */
class SCENE2D_CLASS_API Render : public ::fwRender::IRender
{

public:

    fwCoreServiceClassDefinitionsMacro ( (Render)(::fwRender::IRender) );

    /// A string type representing adaptors id.
    typedef std::string AdaptorIDType;

    /// A string type representing objects id.
    typedef std::string ObjectIDType;

    /// A float type representing adaptors zValue.
    typedef float AdaptorZValueType;

    /// Constructor, set coordinates m_sceneStart to (-100, -100), m_sceneWidth to (200, 200) and add new handle events
    //  ADDED_KEYS, REMOVED_KEYS and CHANGED_KEYS.
    SCENE2D_API Render() throw();

    /// Basic destructor, do nothing.
    SCENE2D_API virtual ~Render() throw();

    /// Get the scene.
    SCENE2D_API QGraphicsScene* getScene() const;

    /// Get the view.
    SCENE2D_API Scene2DGraphicsView* getView() const;

    /// Get the viewport.
    SCENE2D_API SPTR(::scene2D::data::Viewport) getViewport();

    /// Get the object referenced in m_objectID2Object by the key _objectID.
    SCENE2D_API SPTR(::fwData::Object) getRegisteredObject(ObjectIDType _objectID) const;

    /// Iterate on the m_zValue2AdaptorID map and, if the event hasn't been accepted yet, call the adaptor processInteraction function.
    SCENE2D_API void dispatchInteraction( SPTR(::scene2D::data::Event) _event );

    /// Returns the viewport coordinate point mapped to scene coordinates.
    SCENE2D_API ::scene2D::data::Coord mapToScene( const ::scene2D::data::Coord& coord ) const;

    /// Returns what happens to scene's aspect ratio on view resize events
    SCENE2D_API Qt::AspectRatioMode getAspectRatioMode() const;

    /// Update scene size from items bounding rect, this bounding can be enlarged with ratioPercent parameter
    SCENE2D_API void updateSceneSize( float ratioPercent = 0 );

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Composite::s_ADDED_OBJECTS_SIG to this::s_ADD_OBJECTS_SLOT
     * Connect Composite::s_CHANGED_OBJECTS_SIG to this::s_CHANGE_OBJECTS_SLOT
     * Connect Composite::s_REMOVED_OBJECTS_SIG to this::s_REMOVE_OBJECTS_SLOT
     */
    SCENE2D_API virtual KeyConnectionsType getObjSrvConnections() const;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Composite::s_ADDED_OBJECTS_SIG to this::s_UPDATE_OBJECTS_SLOT
     * Connect Composite::s_CHANGED_OBJECTS_SIG to this::s_UPDATE_OBJECTS_SLOT
     * Connect Composite::s_REMOVED_OBJECTS_SIG to this::s_UPDATE_OBJECTS_SLOT
     */
    SCENE2D_API virtual KeyConnectionsMap  getAutoConnections() const;

protected:

    /**
     * @brief Configuring the Render service.
     *
     * Example of configuration
     * @code{.xml}
       <service uid="GENERIC_UID_Scene2D" impl="::scene2D::Render" type="::fwRender::IRender" autoConnect="yes">

        <scene>

            <scene x="-1100" y="-1.1" width="2500" height="1.2" />

            <viewport id="view1" x="-500" y="-1.1" width="500" height="1.2" />

            <axis id="xAxis" origin="0.0" scale="1.0" scaleType="LINEAR" />
            <axis id="yAxis" origin="0.0" scale="-1.0" scaleType="LINEAR" />

            <axis id="axeHistogramY" origin="0.0" scale="-0.000005" scaleType="LINEAR" />

            <adaptor id="grid" class="::scene2D::adaptor::GridFromFloat" objectId="myData">
                <config xMin="-1200" xMax="1500" yMin="-0.7" yMax="1.7" xSpacing="100" ySpacing="0.1"
                    color="darkGray" xAxis="xAxis" yAxis="yAxis" zValue="1"/>
            </adaptor>

            <connect>
                <signal>adaptorUID/modified</signal>
                <slot>serviceUid/updateTM</slot>
            </connect>

            <connect waitForKey="myData">
                <signal>adaptorUID/modified</signal>
                <slot>serviceUid/updateTM</slot>
            </connect>

            <connect waitForKey="myData">
                <signal>modified</signal><!-- signal for object "myData" -->
                <slot>serviceUid/updateTM</slot>
            </connect>

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
     * - \<adaptor id="grid" class="::scene2D::adaptor::GridFromFloat" objectId="myData"\> : Set an adaptor
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
    SCENE2D_API void configuring() throw ( ::fwTools::Failed );

    /// Call startContext to set the scene, the viewport and the view, and add'em to the QtContainer and start the adaptors
    //  contained in the adaptors id vector of the ObjectsID2AdaptorIDVector map.
    SCENE2D_API void starting()    throw ( ::fwTools::Failed );

    /// Do nothing.
    SCENE2D_API void updating()    throw ( ::fwTools::Failed );

    SCENE2D_API void swapping()    throw ( ::fwTools::Failed );

    /// Start/stop adaptors
    SCENE2D_API void swapping(const KeyType& key) throw(::fwTools::Failed);

    /// Stop all the adaptors attached to the render related composite, stop all those rattached to the objects contained
    //  by the render related composite, clear the maps and call stopContext().
    SCENE2D_API void stopping()    throw ( ::fwTools::Failed );

private:

    /// A ConfigurationElement type representing a configuration.
    typedef SPTR (::fwRuntime::ConfigurationElement) ConfigurationType;

    /// Map used to reference input objects
    typedef std::map< std::string, ::fwData::Object::csptr > ConstObjectMapType;

    /// An internal class to store adaptors representations.
    class SceneAdaptor2D
    {

    public:

        /// Basic constructor, do nothing.
        SceneAdaptor2D()
        {
        }

        /// Basic destructor, do nothing.
        ~SceneAdaptor2D()
        {
        }

        /// Get the related service.
        SPTR (adaptor::IAdaptor) getService() const
        {
            return m_service.lock();
        }

        /// The adaptor uid.
        std::string m_uid;

        /// The adaptor type.
        std::string m_type;

        /// The adaptor config.
        ConfigurationType m_config;

        /// The adaptor service.
        WPTR(::scene2D::adaptor::IAdaptor) m_service;

    };

    /// Create the QtContainer, the scene, the viewport, the view.
    void startContext();

    /// Delete m_scene and m_view;
    void stopContext();

    /// Ensure that the given adaptor has a unique value among all other adaptors.
    void ensureUniqueZValue( SceneAdaptor2D _adaptee );

    /// Get the axis configuration specifications, create a new axis, store it in the m_objectID2Object map and set its attributes.
    void configureAxis ( ConfigurationType _conf );

    /// Get the viewport configuration specifications, create a new viewport, store it in the m_objectID2Object map and set its attributes.
    void configureViewport ( ConfigurationType _conf );

    /// Get the scene configuration specifications and set them to m_sceneStart and m_sceneWidth.
    void configureScene( ConfigurationType _conf );

    /// Get the adaptor configuration specifications, push back the id attribute in the m_objectsID2AdaptorIDVector map, create a SceneAdaptor2D,
    //  set its attributes and push it back in the m_adaptorID2SceneAdaptor2D map.
    void configureAdaptor ( ConfigurationType _conf );

    /// Get all the objects of the render related composite, and start all their related adaptors.
    void startAdaptorsFromComposite(const ConstObjectMapType& objects);

    /// Get all the objects of the render related composite, and stop all their related adaptors.
    void stopAdaptorsFromComposite(const ConstObjectMapType& objects);

    /// Get all the objects of the render related composite, and swap all their related adaptors.
    void swapAdaptorsFromComposite(const ::fwData::Composite::ContainerType& objects);

    /// Get the SceneAdaptor2D related to the _adaptorID key in the m_adaptorID2SceneAdaptor2D map, add a service corresponding to _object,
    ///  set its render, its configuration, configure it, star it, check if its zValue is unique, store it in the m_zValue2AdaptorID map.
    void startAdaptor(const AdaptorIDType& _adaptorID, const CSPTR(::fwData::Object)& _object);

    /// Swap the SceneAdaptor2D to _object.
    void swapAdaptor(const AdaptorIDType& _adaptorID, const SPTR(::fwData::Object)& _object);

    /// Stops the adaptor service, unregister it, reset it and erase the SceneAdaptor2D in
    ////  the m_adaptorID2SceneAdaptor2D map.
    void stopAdaptor(const AdaptorIDType& _adaptorID);

    /// Creates the connection if the required key is contained in the composite
    void connectAfterWait(const ::fwData::Composite::ContainerType& objects);

    /// Disconnects the connection based on a object key
    void disconnect(const ::fwData::Composite::ContainerType& objects);

    /// Slot: add objects
    void addObjects(::fwData::Composite::ContainerType objects);

    /// Slot: change objects
    void changeObjects(::fwData::Composite::ContainerType newObjects,
                       ::fwData::Composite::ContainerType oldObjects);

    /// Slot: remove objects
    void removeObjects(::fwData::Composite::ContainerType objects);

    typedef std::map< ObjectIDType, std::vector<AdaptorIDType> > ObjectsID2AdaptorIDVector;

    /// Map of std::vector<AdaptorIDType> referenced by ObjectIDType;
    ObjectsID2AdaptorIDVector m_objectsID2AdaptorIDVector;
    typedef std::map< AdaptorIDType, SceneAdaptor2D > AdaptorID2SceneAdaptor2D;

    /// Map of SceneAdaptor2D referenced by AdaptorIDType;
    AdaptorID2SceneAdaptor2D m_adaptorID2SceneAdaptor2D;
    typedef std::map< ObjectIDType, SPTR(::fwData::Object) > ObjectID2Object;

    /// Map of SPTR(::fwData::Object) referenced by ObjectIDType;
    ObjectID2Object m_objectID2Object;
    typedef std::map< AdaptorZValueType, AdaptorIDType > ZValue2AdaptorID;

    /// Map of AdaptorIDType referenced by AdaptorZValueType;
    ZValue2AdaptorID m_zValue2AdaptorID;

    /// The render configuration.
    ConfigurationType m_sceneConfiguration;

    /// Coordinates of the scene top left corner.
    ::scene2D::data::Coord m_sceneStart;

    /// Width and height of the scene.
    ::scene2D::data::Coord m_sceneWidth;

    /// The scene.
    QGraphicsScene* m_scene;

    /// The view.
    Scene2DGraphicsView* m_view;

    /// If antialiasing is requested (deactivated by default because of a potential lack of performance)
    bool m_antialiasing;

    /// How the scene should behave on view resize events
    Qt::AspectRatioMode m_aspectRatioMode;

    /// Signal/ Slot connection
    ::fwCom::helper::SigSlotConnection m_connections;

    typedef std::vector< ::fwRuntime::ConfigurationElement::sptr > ConnectConfigType;
    /// vector containing all the connections configurations
    ConnectConfigType m_connect;
    /// vector containing all the proxy configurations
    ConnectConfigType m_proxies;

    typedef std::map< std::string, ::fwCom::helper::SigSlotConnection > ObjectConnectionsMapType;
    /// map containing the object key/connection relation
    ObjectConnectionsMapType m_objectConnections;

    /// Map to register proxy connections
    ::fwServices::helper::Config::ProxyConnectionsMapType m_proxyMap;
};

} // namespace scene2D


#endif // __SCENE2D_RENDER_HPP__

