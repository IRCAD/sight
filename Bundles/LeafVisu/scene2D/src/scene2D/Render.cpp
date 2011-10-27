/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <QVBoxLayout>

#include <QGraphicsRectItem>

#include <fwData/Composite.hpp>
#include <fwServices/Base.hpp>
#include <fwComEd/CompositeMsg.hpp>
#include <scene2D/adaptor/IAdaptor.hpp>
#include <fwGuiQt/container/QtContainer.hpp>

#include "scene2D/Render.hpp"
#include "scene2D/Scene2DGraphicsView.hpp"

REGISTER_SERVICE( ::fwRender::IRender , ::scene2D::Render , ::fwData::Composite ) ;

namespace scene2D
{


Render::Render() throw()
        : m_sceneStart (-100,-100),
          m_sceneWidth (200,200),
          m_antialiasing(false)
{
    addNewHandledEvent( ::fwComEd::CompositeMsg::ADDED_FIELDS );
    addNewHandledEvent( ::fwComEd::CompositeMsg::REMOVED_FIELDS );
    addNewHandledEvent( ::fwComEd::CompositeMsg::SWAPPED_FIELDS );
}

//-----------------------------------------------------------------------------

Render::~Render() throw()
{
}

//-----------------------------------------------------------------------------

QGraphicsScene* Render::getScene()
{
    SLM_TRACE_FUNC();

    return m_scene;
}

//-----------------------------------------------------------------------------

Scene2DGraphicsView* Render::getView()
{
    SLM_TRACE_FUNC();

    return m_view;
}

//-----------------------------------------------------------------------------

SPTR(::scene2D::data::Viewport) Render::getViewport()
{
    SLM_TRACE_FUNC();

    return ::scene2D::data::Viewport::dynamicCast( m_objectID2Object["view1"] );
}

//-----------------------------------------------------------------------------

SPTR(::fwData::Object) Render::getRegisteredObject(ObjectIDType _objectID)
{
    SLM_TRACE_FUNC();

    OSLM_ASSERT("Sorry, the object id '"<< _objectID <<"' does not exist in the registered objects map.", m_objectID2Object.find(_objectID) != m_objectID2Object.end() );
    return m_objectID2Object[_objectID];
}

//-----------------------------------------------------------------------------

void Render::dispatchInteraction( SPTR(::scene2D::data::Event) _event)
{
    SLM_TRACE_FUNC();

    /* std::map are sorted regarding to key values. Keys of m_zValue2AdaptorID are z-values (float).
     *
     * The dispatching is performed to the highest z-valued adaptors first, so we start iterating
     * from the end of m_zValue2AdaptorID (with reverse_iterator).
     */
    for(ZValue2AdaptorID::reverse_iterator rit = m_zValue2AdaptorID.rbegin(); rit != m_zValue2AdaptorID.rend(); ++rit )
    {
        if ( ! _event->getAccepted() && ! m_adaptorID2SceneAdaptor2D[ rit->second ].m_service.expired() )
        {
            m_adaptorID2SceneAdaptor2D[ rit->second ].m_service.lock()->processInteraction( _event );
        }
    }
}

//-----------------------------------------------------------------------------

::scene2D::data::Coord Render::mapToScene( const ::scene2D::data::Coord & coord ) const
{
    SLM_TRACE_FUNC();

    /// Returns the viewport coordinate point mapped to scene coordinates.
    QPoint qp ( coord.getX(), coord.getY() );
    QPointF qps = m_view->mapToScene(qp);
    return ::scene2D::data::Coord(qps.x(),qps.y());
}

//-----------------------------------------------------------------------------

void Render::configuring() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();

    this->initialize();

    std::vector < SPTR(::fwRuntime::ConfigurationElement) > vectConfig = m_configuration->find("scene");
    SLM_ASSERT("There is no implementation between \"scene\" tags", !vectConfig.empty());
    m_sceneConfiguration = vectConfig.at(0);

    ::fwRuntime::ConfigurationElementContainer::Iterator iter;
    for (iter = m_sceneConfiguration->begin() ; iter != m_sceneConfiguration->end() ; ++iter)
    {
        if ((*iter)->getName() == "axis")
        {
            this->configureAxis(*iter);
        }
        else if ((*iter)->getName() == "viewport")
        {
            this->configureViewport(*iter);
        }
        else if ((*iter)->getName() == "scene")
        {
            this->configureScene(*iter);
        }
        else if ((*iter)->getName() == "adaptor")
        {
            this->configureAdaptor(*iter);
        }
        else
        {
            OSLM_ASSERT("Bad scene configurationType, unknown xml node : " << (*iter)->getName(), false);
        }
    }
}

//-----------------------------------------------------------------------------

void Render::starting() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();

    this->create();

    this->startContext();

    SPTR(::fwData::Composite) composite = this->getObject< ::fwData::Composite >();

    ObjectsID2AdaptorIDVector::iterator objectIter = m_objectsID2AdaptorIDVector.find( "self" );
    if ( objectIter != m_objectsID2AdaptorIDVector.end() )
    {
        BOOST_FOREACH( AdaptorIDType adaptorId,  objectIter->second )
        {
            this->startAdaptor( adaptorId, composite );
            SLM_ASSERT("Service is not started", m_adaptorID2SceneAdaptor2D[adaptorId].getService()->isStarted());
        }
    }

    this->startAdaptorsFromComposite(composite);
}

//-----------------------------------------------------------------------------

void Render::updating() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void Render::updating( fwServices::ObjectMsg::csptr _msg) throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();

    ::fwComEd::CompositeMsg::csptr compositeMsg = ::fwComEd::CompositeMsg::dynamicConstCast(_msg);

    if(compositeMsg && compositeMsg->hasEvent( ::fwComEd::CompositeMsg::ADDED_FIELDS ) )
    {
        SPTR(::fwData::Composite) field = compositeMsg->getAddedFields();
        this->startAdaptorsFromComposite(field);
    }
    else if(compositeMsg && compositeMsg->hasEvent( ::fwComEd::CompositeMsg::REMOVED_FIELDS ) )
    {
        SPTR(::fwData::Composite) field = compositeMsg->getAddedFields();
        this->stopAdaptorsFromComposite(field);
    }
    else if(compositeMsg && compositeMsg->hasEvent( ::fwComEd::CompositeMsg::SWAPPED_FIELDS ) )
    {
        //SLM_FATAL("ToDo IM");
    }
}

//-----------------------------------------------------------------------------

void Render::swapping() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();

    //SLM_FATAL("ToDo IM");
}

//-----------------------------------------------------------------------------

void Render::stopping() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();

    SPTR(::fwData::Composite) composite = this->getObject< ::fwData::Composite >();

    ObjectsID2AdaptorIDVector::iterator objectIter = m_objectsID2AdaptorIDVector.find( "self" );
    if ( objectIter != m_objectsID2AdaptorIDVector.end() )
    {
        BOOST_FOREACH( AdaptorIDType adaptorId,  objectIter->second )
        {
            this->stopAdaptor( adaptorId );
        }
    }

    this->stopAdaptorsFromComposite(composite);

    m_adaptorID2SceneAdaptor2D.clear();
    m_objectID2Object.clear();
    m_objectsID2AdaptorIDVector.clear();

    this->stopContext();

    this->getContainer()->clean();
    this->destroy();
}

//-----------------------------------------------------------------------------

void Render::startContext()
{
    SLM_TRACE_FUNC();

    SPTR(::fwGuiQt::container::QtContainer) qtContainer =  ::fwGuiQt::container::QtContainer::dynamicCast(this->getContainer());

    m_scene = new QGraphicsScene( m_sceneStart.getX(), m_sceneStart.getY(), m_sceneWidth.getX(), m_sceneWidth.getY());
    m_scene->setBackgroundBrush(QBrush(QColor(0,0,0)));
    m_scene->setFocus( Qt::MouseFocusReason );

    SPTR(::scene2D::data::Viewport) viewport = ::scene2D::data::Viewport::dynamicCast( m_objectID2Object["view1"] );

    m_view = new Scene2DGraphicsView( m_scene, qtContainer->getQtContainer() );
    m_view->setViewport( viewport );
    m_view->setSceneRender( ::scene2D::Render::dynamicCast( this->getSptr() ) );
    m_view->setRenderHint( QPainter::Antialiasing, m_antialiasing );

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(m_view);
    qtContainer->getQtContainer()->setLayout(layout);
}

//-----------------------------------------------------------------------------

void Render::stopContext()
{
    SLM_TRACE_FUNC();

    delete m_scene;
    delete m_view;
}

//-----------------------------------------------------------------------------

void Render::ensureUniqueZValue( SceneAdaptor2D _adaptee )
{
    SLM_TRACE_FUNC();

    const float zValue = _adaptee.m_service.lock()->getZValue();

    if( m_zValue2AdaptorID.find( zValue ) != m_zValue2AdaptorID.end() )
    {
        _adaptee.m_service.lock()->setZValue( zValue + 1 );
        ensureUniqueZValue( _adaptee );
    }
}

//-----------------------------------------------------------------------------

void Render::configureAxis( ConfigurationType _conf )
{
    SLM_TRACE_FUNC();

    SLM_ASSERT("\"axis\" tag required", _conf->getName() == "axis");

    std::string id      = _conf->getAttributeValue("id");
    std::string origin  = _conf->getAttributeValue("origin");
    std::string scale   = _conf->getAttributeValue("scale");
    std::string scaleType    = _conf->getAttributeValue("scaleType");


    if(m_objectID2Object.count(id) == 0)
    {
        m_objectID2Object[id] = ::scene2D::data::Axis::New();
        SLM_ASSERT( "Sorry, axis ptr is null",  m_objectID2Object[id] );
        ::scene2D::data::Axis::dynamicCast(m_objectID2Object[id])->setOrigin (::boost::lexical_cast< float >( origin ));
        ::scene2D::data::Axis::dynamicCast(m_objectID2Object[id])->setScale (::boost::lexical_cast< float >( scale ));
        ::scene2D::data::Axis::dynamicCast(m_objectID2Object[id])->setScaleType ( scaleType );
    }
}

//-----------------------------------------------------------------------------

void Render::configureViewport( ConfigurationType _conf )
{
    SLM_TRACE_FUNC();

    SLM_ASSERT("\"viewport\" tag required", _conf->getName() == "viewport");

    std::string id  = _conf->getAttributeValue("id");
    std::string x  = _conf->getAttributeValue("x");
    std::string y   = _conf->getAttributeValue("y");
    std::string width    = _conf->getAttributeValue("width");
    std::string height    = _conf->getAttributeValue("height");

    m_objectID2Object[id] = ::scene2D::data::Viewport::New();
    SLM_ASSERT( "Sorry, viewport ptr is null",  m_objectID2Object[id] );
    ::scene2D::data::Viewport::dynamicCast(m_objectID2Object[id])->setX (::boost::lexical_cast< float >( x ));
    ::scene2D::data::Viewport::dynamicCast(m_objectID2Object[id])->setY (::boost::lexical_cast< float >( y ));
    ::scene2D::data::Viewport::dynamicCast(m_objectID2Object[id])->setWidth (::boost::lexical_cast< float >( width ));
    ::scene2D::data::Viewport::dynamicCast(m_objectID2Object[id])->setHeight (::boost::lexical_cast< float >( height ));
}

//-----------------------------------------------------------------------------

void Render::configureScene( ConfigurationType _conf )
{
    SLM_TRACE_FUNC();

    SLM_ASSERT("\"viewport\" tag required", _conf->getName() == "scene");

    std::string x = _conf->getAttributeValue("x");
    std::string y = _conf->getAttributeValue("y");
    std::string width = _conf->getAttributeValue("width");
    std::string height = _conf->getAttributeValue("height");

    m_sceneStart.setX( ::boost::lexical_cast< float >( x ) );
    m_sceneStart.setY( ::boost::lexical_cast< float >( y ) );
    m_sceneWidth.setX( ::boost::lexical_cast< float >( width ) );
    m_sceneWidth.setY( ::boost::lexical_cast< float >( height ) );

    if( _conf->hasAttribute("antialiasing"))
    {
        if( _conf->getAttributeValue("antialiasing") == "true")
        {
            m_antialiasing = true;
        }
    }
}

//-----------------------------------------------------------------------------

void Render::configureAdaptor( ConfigurationType _conf )
{
    SLM_TRACE_FUNC();

    SLM_ASSERT("\"adaptor\" tag required", _conf->getName() == "adaptor");

    const std::string id            = _conf->getAttributeValue("id");
    const std::string objectId      = _conf->getAttributeValue("objectId");
    const std::string type          = _conf->getAttributeValue("class");
    const std::string uid           = _conf->getAttributeValue("uid");

    SLM_ASSERT( "'id' required attribute missing or empty"      , !id.empty() );
    SLM_ASSERT( "'objectId' required attribute missing or empty", !objectId.empty() );
    SLM_ASSERT( "'class' required attribute missing or empty" , !type.empty() );

    m_objectsID2AdaptorIDVector[objectId].push_back(id);

    SceneAdaptor2D adaptee;
    adaptee.m_uid = uid;
    adaptee.m_type = type;
    adaptee.m_config = * (_conf->begin());

    m_adaptorID2SceneAdaptor2D[id] = adaptee;
}

//-----------------------------------------------------------------------------

void Render::startAdaptorsFromComposite( SPTR(::fwData::Composite) _composite)
{
    SLM_TRACE_FUNC();

    BOOST_FOREACH( ::fwData::Composite::value_type elem, (*_composite) )
    {
        std::string compositeKey = elem.first;
        ObjectsID2AdaptorIDVector::iterator objectIter = m_objectsID2AdaptorIDVector.find( compositeKey );
        if ( objectIter != m_objectsID2AdaptorIDVector.end() )
        {
            BOOST_FOREACH( AdaptorIDType adaptorId,  objectIter->second )
            {
                this->startAdaptor( adaptorId, elem.second );
                OSLM_ASSERT("Service "<<adaptorId<<" is not started", m_adaptorID2SceneAdaptor2D[adaptorId].getService()->isStarted());
            }
        }
    }
}

//-----------------------------------------------------------------------------

void Render::stopAdaptorsFromComposite( SPTR(::fwData::Composite) _composite)
{
    SLM_TRACE_FUNC();
    BOOST_FOREACH( ::fwData::Composite::value_type elem, (*_composite) )
    {
        std::string compositeKey = elem.first;
        ObjectsID2AdaptorIDVector::iterator objectIter = m_objectsID2AdaptorIDVector.find( compositeKey );
        if ( objectIter != m_objectsID2AdaptorIDVector.end() )
        {
            BOOST_FOREACH( AdaptorIDType adaptorId,  objectIter->second )
            {
                this->stopAdaptor( adaptorId );
            }
        }
    }
}

//-----------------------------------------------------------------------------

void Render::startAdaptor(AdaptorIDType _adaptorID, SPTR(::fwData::Object) _object)
{
    SLM_TRACE_FUNC();

    if (!m_adaptorID2SceneAdaptor2D[_adaptorID].m_uid.empty())
    {
        m_adaptorID2SceneAdaptor2D[_adaptorID].m_service = ::fwServices::add< ::scene2D::adaptor::IAdaptor >( _object , m_adaptorID2SceneAdaptor2D[_adaptorID].m_type, m_adaptorID2SceneAdaptor2D[_adaptorID].m_uid);
    }
    else
    {
        m_adaptorID2SceneAdaptor2D[_adaptorID].m_service = ::fwServices::add< ::scene2D::adaptor::IAdaptor >( _object , m_adaptorID2SceneAdaptor2D[_adaptorID].m_type);
    }

    SLM_ASSERT("\"config\" tag required", m_adaptorID2SceneAdaptor2D[_adaptorID].m_config->getName() == "config");
    SLM_ASSERT("Adaptor service expired", m_adaptorID2SceneAdaptor2D[_adaptorID].getService());

    m_adaptorID2SceneAdaptor2D[_adaptorID].getService()->setScene2DRender(Render::dynamicCast(this->shared_from_this()));
    m_adaptorID2SceneAdaptor2D[_adaptorID].getService()->setConfiguration(m_adaptorID2SceneAdaptor2D[_adaptorID].m_config);
    m_adaptorID2SceneAdaptor2D[_adaptorID].getService()->configure();
    m_adaptorID2SceneAdaptor2D[_adaptorID].getService()->start();

    // Fill the map of unique zValues
    ensureUniqueZValue( m_adaptorID2SceneAdaptor2D[_adaptorID] );
    m_zValue2AdaptorID[ m_adaptorID2SceneAdaptor2D[_adaptorID].m_service.lock()->getZValue() ] = _adaptorID;

    m_adaptorID2SceneAdaptor2D[_adaptorID].m_comChannel = ::fwServices::registerCommunicationChannel( _object , m_adaptorID2SceneAdaptor2D[_adaptorID].getService()->getSptr() );
    m_adaptorID2SceneAdaptor2D[_adaptorID].m_comChannel.lock()->start();
}

//-----------------------------------------------------------------------------

void Render::swapAdaptor(AdaptorIDType _adaptorID, SPTR(::fwData::Object) _object)
{
    SLM_TRACE_FUNC();

     m_adaptorID2SceneAdaptor2D[ _adaptorID ].getService()->swap( _object );
}

//-----------------------------------------------------------------------------

void Render::stopAdaptor(AdaptorIDType _adaptorID)
{
    SLM_TRACE_FUNC();

    m_adaptorID2SceneAdaptor2D[_adaptorID].m_comChannel.lock()->stop();
    ::fwServices::registry::ObjectService::unregisterService( m_adaptorID2SceneAdaptor2D[_adaptorID].m_comChannel.lock() );

    m_adaptorID2SceneAdaptor2D[_adaptorID].getService()->stop();
    SLM_ASSERT("Service is not stopped", m_adaptorID2SceneAdaptor2D[_adaptorID].getService()->isStopped());
    ::fwServices::OSR::unregisterService(m_adaptorID2SceneAdaptor2D[_adaptorID].getService());
    m_adaptorID2SceneAdaptor2D[_adaptorID].m_service.reset();
    m_adaptorID2SceneAdaptor2D.erase(_adaptorID);
}

} // namespace scene2D



