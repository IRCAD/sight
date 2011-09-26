/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _SCENE2D_RENDER_HPP_
#define _SCENE2D_RENDER_HPP_

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QResizeEvent>

#include <fwRender/IRender.hpp>
#include <fwServices/ComChannelService.hpp>
#include <scene2D/data/Axis.hpp>
#include <scene2D/data/Viewport.hpp>
#include <fwData/Composite.hpp>

#include "scene2D/config.hpp"
#include "scene2D/data/Event.hpp"
#include "scene2D/data/Coord.hpp"

namespace scene2D
{
    // Not declared in the #include because of mutual inclusion.
    class Scene2DGraphicsView;

namespace adaptor
{
    // Not declared in the #include because of mutual inclusion.
    class IAdaptor;
}

class SCENE2D_CLASS_API Render : public ::fwRender::IRender
{

public:

    fwCoreServiceClassDefinitionsMacro ( (Render)(::fwRender::IRender) ) ;

    /// A string type representing adaptors id.
    typedef std::string AdaptorIDType;

    /// A string type representing objects id.
    typedef std::string ObjectIDType;

    /// A float type representing adaptors zValue.
    typedef float AdaptorZValueType;

    /// Constructor, set coordinates m_sceneStart to (-100, -100), m_sceneWidth to (200, 200) and add new handle events
    //  ADDED_FIELDS, REMOVED_FIELDS and SWAPPED_FIELDS.
    SCENE2D_API Render() throw();

    /// Basic destructor, do nothing.
    SCENE2D_API virtual ~Render() throw();

    /// Get the scene.
    SCENE2D_API QGraphicsScene* getScene();

    /// Get the view.
    SCENE2D_API Scene2DGraphicsView* getView();

    /// Get the viewport.
    SCENE2D_API SPTR(::scene2D::data::Viewport) getViewport();

    /// Get the object referenced in m_objectID2Object by the key _objectID.
    SCENE2D_API SPTR(::fwData::Object) getRegisteredObject(ObjectIDType _objectID);

    /// Iterate on the m_zValue2AdaptorID map and, if the event hasn't been accepted yet, call the adaptor processInteraction function.
    SCENE2D_API void dispatchInteraction( SPTR(::scene2D::data::Event) _event );

    /// Returns the viewport coordinate point mapped to scene coordinates.
    SCENE2D_API ::scene2D::data::Coord mapToScene( const ::scene2D::data::Coord & coord ) const;

protected:
    /**
    * @brief Configuring the Render service.
    *
    * Example of configuration
    * @verbatim
    <service uid="GENERIC_UID_Scene2D" implementation="::scene2D::Render" type="::fwRender::IRender" autoComChannel="yes">

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

        </scene>

    </service>
    @endverbatim
    *
    * - <scene x="-1100" y="-1.1" width="2500" height="1.2" /> : Set the scene coordinates
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
    * - <viewport id="view1" x="-500" y="-1.1" width="500" height="1.2" /> : Set a viewport coordinates
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
    * - <axis id="xAxis" origin="0.0" scale="1.0" scaleType="LINEAR" /> : Set an axis specifications
    *
    * \b id : mandatory : Set the axis id
    *
    * \b origin : mandatory : Set the axis origin
    *
    * \b scale : mandatory : Set the axis scale
    *
    * \b scaleType : mandatory : Set the axis scaleType
    *
    * - <adaptor id="grid" class="::scene2D::adaptor::GridFromFloat" objectId="myData"> : Set an adaptor
    *
    * \b id : mandatory : Set the adaptor id
    *
    * \b class : mandatory : Set the adaptor type
    *
    * \b objectId : mandatory : Set the adaptor related object id
    *
    * \b uid : no mandatory : Set the adaptor uid
    */
    SCENE2D_API void configuring() throw ( ::fwTools::Failed );

    /// Call startContext to set the scene, the viewport and the view, and add'em to the QtContainer and start the adaptors
    //  contained in the adaptors id vector of the ObjectsID2AdaptorIDVector map.
    SCENE2D_API void starting()    throw ( ::fwTools::Failed );

    /// Do nothing.
    SCENE2D_API void updating()    throw ( ::fwTools::Failed );

    /// If the message is ADDED_FIELDS, call the startAdaptorsFromComposite function to start all the adaptors contained
    //  in the message composite.
    SCENE2D_API void updating( fwServices::ObjectMsg::csptr _msg ) throw ( ::fwTools::Failed );

    /// ToDo IM
    SCENE2D_API void swapping()    throw ( ::fwTools::Failed );

    /// Stop all the adaptors attached to the render related composite, stop all those rattached to the objects contained
    //  by the render related composite, clear the maps and call stopContext().
    SCENE2D_API void stopping()    throw ( ::fwTools::Failed );

private:

    /// A ConfigurationElement type representing a configuration.
    typedef SPTR(::fwRuntime::ConfigurationElement) ConfigurationType;

    /// An internal class to store adaptors representations.
    class SceneAdaptor2D {

        public:

        /// Basic construtor, do nothing.
        SceneAdaptor2D()
        { }

        /// Basic destructor, do nothing.
        ~SceneAdaptor2D()
        { }

        /// Get the related service.
        SPTR (adaptor::IAdaptor) getService()
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

        /// The adaptor comChannel.
        WPTR(::fwServices::ComChannelService) m_comChannel;
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
    void startAdaptorsFromComposite( SPTR(::fwData::Composite) _composite);

    /// Get all the objects of the render related composite, and stop all their related adaptors.
    void stopAdaptorsFromComposite( SPTR(::fwData::Composite) _composite);

    /// Get the SceneAdaptor2D related to the _adaptorID key in the m_adaptorID2SceneAdaptor2D map, add a service corresponding to _object,
    //  set its render, its configuration, configure it, star it, check if its zValue is unique, store it in the m_zValue2AdaptorID map,
    //  create the comChannel and start it.
    void startAdaptor(AdaptorIDType _adaptorID, SPTR(::fwData::Object) _object);

    /// Swap the SceneAdaptor2D to _object.
    void swapAdaptor(AdaptorIDType _adaptorID, SPTR(::fwData::Object) _object);

    /// Stop the associated comChannel, unregister it, stop the adaptor service, unregister it, reset it and erase the SceneAdaptor2D in
    //  the m_adaptorID2SceneAdaptor2D map.
    void stopAdaptor(AdaptorIDType _adaptorID);

    typedef std::map< ObjectIDType, std::vector<AdaptorIDType> > ObjectsID2AdaptorIDVector;

    /// Map of std::vector<AdaptorIDType> referenced by ObjectIDType;
    ObjectsID2AdaptorIDVector m_objectsID2AdaptorIDVector;
    typedef std::map< AdaptorIDType, SceneAdaptor2D > AdaptorID2SceneAdaptor2D ;

    /// Map of SceneAdaptor2D referenced by AdaptorIDType;
    AdaptorID2SceneAdaptor2D m_adaptorID2SceneAdaptor2D;
    typedef std::map< ObjectIDType, SPTR(::fwData::Object) > ObjectID2Object ;

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
    QGraphicsScene * m_scene;

    /// The view.
    Scene2DGraphicsView * m_view;

    /// If antialiasing is requested (deactivated by default because of a potential lack of performance)
    bool m_antialiasing;

};



} // namespace scene2D


#endif // _SCENE2D_RENDER_HPP_

