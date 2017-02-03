/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "scene2D/Render.hpp"

#include "scene2D/Scene2DGraphicsView.hpp"
#include "scene2D/adaptor/IAdaptor.hpp"

#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>
#include <fwCom/helper/SigSlotConnection.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include <fwServices/helper/Config.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/op/Add.hpp>

#include <QGraphicsRectItem>
#include <QVBoxLayout>

fwServicesRegisterMacro( ::fwRender::IRender, ::scene2D::Render, ::fwData::Composite );

namespace scene2D
{

static const ::fwCom::Slots::SlotKeyType s_ADD_OBJECTS_SLOT    = "addObject";
static const ::fwCom::Slots::SlotKeyType s_CHANGE_OBJECTS_SLOT = "changeObject";
static const ::fwCom::Slots::SlotKeyType s_REMOVE_OBJECTS_SLOT = "removeObjects";

//-----------------------------------------------------------------------------

Render::Render() throw() :
    m_sceneStart(-100., -100.),
    m_sceneWidth(200., 200.),
    m_scene(nullptr),
    m_view(nullptr),
    m_antialiasing(false),
    m_aspectRatioMode(Qt::IgnoreAspectRatio)
{
    newSlot(s_ADD_OBJECTS_SLOT, &Render::addObjects, this);
    newSlot(s_CHANGE_OBJECTS_SLOT, &Render::changeObjects, this);
    newSlot(s_REMOVE_OBJECTS_SLOT, &Render::removeObjects, this);
}

//-----------------------------------------------------------------------------

Render::~Render() throw()
{
}

//-----------------------------------------------------------------------------

QGraphicsScene* Render::getScene() const
{
    return m_scene;
}

//-----------------------------------------------------------------------------

Scene2DGraphicsView* Render::getView() const
{
    return m_view;
}

//-----------------------------------------------------------------------------

SPTR(::scene2D::data::Viewport) Render::getViewport()
{
    return ::scene2D::data::Viewport::dynamicCast( m_objectID2Object["view1"] );
}

//-----------------------------------------------------------------------------

SPTR(::fwData::Object) Render::getRegisteredObject(ObjectIDType _objectID) const
{
    ObjectID2Object::const_iterator iter = m_objectID2Object.find( _objectID );
    OSLM_ASSERT("Sorry, the object id '"<< _objectID <<"' does not exist in the registered objects map.",
                iter != m_objectID2Object.end() );
    return iter->second;
}

//-----------------------------------------------------------------------------

void Render::dispatchInteraction( SPTR(::scene2D::data::Event)_event)
{
    SLM_TRACE_FUNC();

    /* std::map are sorted regarding to key values. Keys of m_zValue2AdaptorID are z-values (float).
     *
     * The dispatching is performed to the highest z-valued adaptors first, so we start iterating
     * from the end of m_zValue2AdaptorID (with reverse_iterator).
     */
    for(ZValue2AdaptorID::reverse_iterator rit = m_zValue2AdaptorID.rbegin(); rit != m_zValue2AdaptorID.rend(); ++rit )
    {
        if ( !_event->isAccepted() )
        {
            auto adaptor = m_adaptorID2SceneAdaptor2D[ rit->second ].m_service.lock();
            if(adaptor != nullptr && adaptor->isStarted())
            {
                m_adaptorID2SceneAdaptor2D[ rit->second ].m_service.lock()->processInteraction( _event );
            }
        }
    }
}

//-----------------------------------------------------------------------------

::scene2D::data::Coord Render::mapToScene( const ::scene2D::data::Coord& coord ) const
{
    /// Returns the viewport coordinate point mapped to scene coordinates.
    QPoint qp( static_cast<int>(coord.getX()), static_cast<int>(coord.getY()) );
    QPointF qps = m_view->mapToScene(qp);
    return ::scene2D::data::Coord(qps.x(), qps.y());
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
    for (iter = m_sceneConfiguration->begin(); iter != m_sceneConfiguration->end(); ++iter)
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
        else if((*iter)->getName() == "connect")
        {
            m_connect.push_back(*iter);
        }
        else if((*iter)->getName() == "proxy")
        {
            m_proxies.push_back(*iter);
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

    ::fwData::Composite::sptr composite;
    if(this->isVersion2())
    {
        composite = this->getInOut< ::fwData::Composite >(s_DEFAULT_OBJECT);
    }
    else
    {
        composite = this->getObject< ::fwData::Composite >();
    }

    ObjectsID2AdaptorIDVector::iterator objectIter = m_objectsID2AdaptorIDVector.find( "self" );
    if ( objectIter != m_objectsID2AdaptorIDVector.end() )
    {
        for(const AdaptorIDType& adaptorId :  objectIter->second )
        {
            this->startAdaptor( adaptorId, composite );
            SLM_ASSERT("Service is not started", m_adaptorID2SceneAdaptor2D[adaptorId].getService()->isStarted());
        }
    }

    ConstObjectMapType container;
    if(this->isVersion2())
    {
        for(auto obj : this->getInputs())
        {
            if(obj.first != s_DEFAULT_OBJECT)
            {
                container[obj.first] = obj.second.lock();
            }
        }
        for(auto obj : this->getInOuts())
        {
            if(obj.first != s_DEFAULT_OBJECT)
            {
                container[obj.first] = obj.second.lock();
            }
        }
        this->startAdaptorsFromComposite(container);
    }
    else
    {
        auto objects = composite->getContainer();
        std::for_each(objects.begin(), objects.end(), [&container](
                          const ::fwData::Composite::ContainerType::value_type& it)
            {
                container[it.first] = it.second;
            });
        this->startAdaptorsFromComposite(container);
    }

    //Create connections when adaptors are started

    for(const ::fwRuntime::ConfigurationElement::sptr& connect : m_connect)
    {
        if(!connect->hasAttribute("waitForKey"))
        {
            ::fwServices::helper::Config::createConnections(connect, m_connections);
        }
    }
    for(const ::fwRuntime::ConfigurationElement::sptr& proxy : m_proxies)
    {
        if(!proxy->hasAttribute("waitForKey"))
        {
            ::fwServices::helper::Config::createProxy("self", proxy, m_proxyMap);
        }
    }
    this->connectAfterWait(composite->getContainer());
}

//-----------------------------------------------------------------------------

void Render::connectAfterWait(const ::fwData::Composite::ContainerType& objects)
{

    for(const ::fwData::Composite::value_type& element : objects)
    {
        std::string key = element.first;
        for(const ::fwRuntime::ConfigurationElement::sptr& connect : m_connect)
        {
            if(connect->hasAttribute("waitForKey"))
            {
                std::string waitForKey = connect->getAttributeValue("waitForKey");
                if(waitForKey == key)
                {
                    ::fwServices::helper::Config::createConnections(connect, m_objectConnections[key], element.second);
                }
            }
        }

        for(const ::fwRuntime::ConfigurationElement::sptr& proxy : m_proxies)
        {
            if(proxy->hasAttribute("waitForKey"))
            {
                std::string waitForKey = proxy->getAttributeValue("waitForKey");
                if(waitForKey == key)
                {
                    ::fwServices::helper::Config::createProxy(key, proxy, m_proxyMap, element.second);
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------

void Render::disconnect(const ::fwData::Composite::ContainerType& objects)
{

    for(const ::fwData::Composite::value_type& element : objects)
    {
        std::string key = element.first;
        if(m_objectConnections.find(key) != m_objectConnections.end())
        {
            m_objectConnections[key].disconnect();
            m_objectConnections.erase(key);
        }

        ::fwServices::helper::Config::disconnectProxies(key, m_proxyMap);
    }
}

//-----------------------------------------------------------------------------

void Render::updating() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void Render::swapping() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void Render::swapping(const IService::KeyType& key) throw(::fwTools::Failed)
{
    if (this->isVersion2())
    {
        // remove connections
        auto iter = m_objectConnections.find(key);
        if(iter != m_objectConnections.end())
        {
            iter->second.disconnect();
            m_objectConnections.erase(key);
        }
        ::fwServices::helper::Config::disconnectProxies(key, m_proxyMap);

        auto obj = this->getInput< ::fwData::Object>(key);
        obj = (obj == nullptr) ? this->getInOut< ::fwData::Object>(key) : obj;

        ObjectsID2AdaptorIDVector::iterator objectIter = m_objectsID2AdaptorIDVector.find(key);
        if (objectIter != m_objectsID2AdaptorIDVector.end())
        {
            for (const AdaptorIDType& adaptorId : objectIter->second)
            {
                SceneAdaptor2D& info = m_adaptorID2SceneAdaptor2D[adaptorId];

                if (info.getService() && info.getService()->isStarted())
                {
                    info.getService()->stop();
                }
            }
        }

        // create connections
        if(obj)
        {
            ObjectsID2AdaptorIDVector::iterator objectIter = m_objectsID2AdaptorIDVector.find(key);
            if (objectIter != m_objectsID2AdaptorIDVector.end())
            {
                for (const AdaptorIDType& adaptorId : objectIter->second)
                {
                    SceneAdaptor2D& info = m_adaptorID2SceneAdaptor2D[adaptorId];

                    if (info.getService())
                    {
                        if(obj != info.getService()->getObject())
                        {
                            ::fwServices::OSR::swapService(::fwData::Object::constCast(obj), info.getService());
                        }
                        info.getService()->start();
                    }
                    else
                    {
                        this->startAdaptor(adaptorId, obj);
                    }
                }
            }

            ::fwData::Composite::ContainerType mapConnect;
            mapConnect[key] = ::fwData::Object::constCast(obj);
            this->connectAfterWait(mapConnect);
        }
    }
}

//-----------------------------------------------------------------------------

void Render::stopping() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();

    m_connections.disconnect();

    ::fwData::Composite::sptr composite;
    if(this->isVersion2())
    {
        composite = this->getInOut< ::fwData::Composite >(s_DEFAULT_OBJECT);
    }
    else
    {
        composite = this->getObject< ::fwData::Composite >();
    }

    this->disconnect(composite->getContainer());

    ObjectsID2AdaptorIDVector::iterator objectIter = m_objectsID2AdaptorIDVector.find( "self" );
    if ( objectIter != m_objectsID2AdaptorIDVector.end() )
    {
        for(const AdaptorIDType& adaptorId : objectIter->second )
        {
            this->stopAdaptor( adaptorId );
        }
    }

    ConstObjectMapType container;
    if(this->isVersion2())
    {
        for(auto obj : this->getInOuts())
        {
            if(obj.first != s_DEFAULT_OBJECT)
            {
                container[obj.first] = obj.second.lock();
            }
        }
        for(auto obj : this->getInputs())
        {
            if(obj.first != s_DEFAULT_OBJECT)
            {
                container[obj.first] = obj.second.lock();
            }
        }
        this->stopAdaptorsFromComposite(container);
    }
    else
    {
        auto objects = composite->getContainer();
        std::for_each(objects.begin(), objects.end(), [&container](
                          ::fwData::Composite::ContainerType::value_type it){ container[it.first] = it.second; });
        this->stopAdaptorsFromComposite(container);
    }

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

    SPTR(::fwGuiQt::container::QtContainer) qtContainer = ::fwGuiQt::container::QtContainer::dynamicCast(
        this->getContainer());

    m_scene = new QGraphicsScene( m_sceneStart.getX(), m_sceneStart.getY(), m_sceneWidth.getX(), m_sceneWidth.getY());
    m_scene->setBackgroundBrush(QBrush(QColor(0, 0, 0)));
    m_scene->setFocus( Qt::MouseFocusReason );

    SPTR(::scene2D::data::Viewport) viewport = ::scene2D::data::Viewport::dynamicCast( m_objectID2Object["view1"] );

    m_view = new Scene2DGraphicsView( m_scene, qtContainer->getQtContainer() );
    m_view->setViewport( viewport );
    m_view->setSceneRender( ::scene2D::Render::dynamicCast( this->getSptr() ) );
    m_view->setRenderHint( QPainter::Antialiasing, m_antialiasing );

    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget(m_view);
    qtContainer->getQtContainer()->setLayout(layout);

    m_view->updateFromViewport();
}

//-----------------------------------------------------------------------------

void Render::stopContext()
{
    SLM_TRACE_FUNC();

    delete m_scene;
    delete m_view;
}

//-----------------------------------------------------------------------------

Qt::AspectRatioMode Render::getAspectRatioMode() const
{
    return m_aspectRatioMode;
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

    std::string id        = _conf->getAttributeValue("id");
    std::string origin    = _conf->getAttributeValue("origin");
    std::string scale     = _conf->getAttributeValue("scale");
    std::string scaleType = _conf->getAttributeValue("scaleType");

    if(m_objectID2Object.count(id) == 0)
    {
        m_objectID2Object[id] = ::scene2D::data::Axis::New();
        SLM_ASSERT( "Sorry, axis ptr is null",  m_objectID2Object[id] );
        ::scene2D::data::Axis::dynamicCast(m_objectID2Object[id])->setOrigin(std::stof( origin ));
        ::scene2D::data::Axis::dynamicCast(m_objectID2Object[id])->setScale(std::stof( scale ));
        ::scene2D::data::Axis::dynamicCast(m_objectID2Object[id])->setScaleType( scaleType );
    }
}

//-----------------------------------------------------------------------------

void Render::configureViewport( ConfigurationType _conf )
{
    SLM_TRACE_FUNC();

    SLM_ASSERT("\"viewport\" tag required", _conf->getName() == "viewport");

    std::string id     = _conf->getAttributeValue("id");
    std::string x      = _conf->getAttributeValue("x");
    std::string y      = _conf->getAttributeValue("y");
    std::string width  = _conf->getAttributeValue("width");
    std::string height = _conf->getAttributeValue("height");

    m_objectID2Object[id] = ::scene2D::data::Viewport::New();
    SLM_ASSERT( "Sorry, viewport ptr is null",  m_objectID2Object[id] );
    ::scene2D::data::Viewport::dynamicCast(m_objectID2Object[id])->setX(std::stof( x ));
    ::scene2D::data::Viewport::dynamicCast(m_objectID2Object[id])->setY(std::stof( y ));
    ::scene2D::data::Viewport::dynamicCast(m_objectID2Object[id])->setWidth(std::stof( width ));
    ::scene2D::data::Viewport::dynamicCast(m_objectID2Object[id])->setHeight(std::stof( height ));
}

//-----------------------------------------------------------------------------

void Render::configureScene( ConfigurationType _conf )
{
    SLM_TRACE_FUNC();

    SLM_ASSERT("\"viewport\" tag required", _conf->getName() == "scene");

    std::string x      = _conf->getAttributeValue("x");
    std::string y      = _conf->getAttributeValue("y");
    std::string width  = _conf->getAttributeValue("width");
    std::string height = _conf->getAttributeValue("height");

    m_sceneStart.setX( std::stof( x ) );
    m_sceneStart.setY( std::stof( y ) );
    m_sceneWidth.setX( std::stof( width ) );
    m_sceneWidth.setY( std::stof( height ) );

    if( _conf->hasAttribute("antialiasing"))
    {
        if( _conf->getAttributeValue("antialiasing") == "true")
        {
            m_antialiasing = true;
        }
    }

    if( _conf->hasAttribute(("aspectRatioMode")))
    {
        m_aspectRatioMode = (_conf->getAttributeValue("aspectRatioMode") == "KeepAspectRatioByExpanding")
                            ? Qt::KeepAspectRatioByExpanding : Qt::IgnoreAspectRatio;
    }
}

//-----------------------------------------------------------------------------

void Render::configureAdaptor( ConfigurationType _conf )
{
    SLM_TRACE_FUNC();

    SLM_ASSERT("\"adaptor\" tag required", _conf->getName() == "adaptor");

    const std::string id       = _conf->getAttributeValue("id");
    const std::string objectId = _conf->getAttributeValue("objectId");
    const std::string type     = _conf->getAttributeValue("class");
    const std::string uid      = _conf->getAttributeValue("uid");

    SLM_ASSERT( "'id' required attribute missing or empty", !id.empty() );
    SLM_ASSERT( "'objectId' required attribute missing or empty", !objectId.empty() );
    SLM_ASSERT( "'class' required attribute missing or empty", !type.empty() );

    m_objectsID2AdaptorIDVector[objectId].push_back(id);

    SceneAdaptor2D adaptee;
    adaptee.m_uid    = uid;
    adaptee.m_type   = type;
    adaptee.m_config = *(_conf->begin());

    m_adaptorID2SceneAdaptor2D[id] = adaptee;
}

//-----------------------------------------------------------------------------

void Render::startAdaptorsFromComposite(const ConstObjectMapType& objects)
{
    SLM_TRACE_FUNC();

    for(const auto& elem : objects )
    {
        std::string compositeKey                       = elem.first;
        ObjectsID2AdaptorIDVector::iterator objectIter = m_objectsID2AdaptorIDVector.find( compositeKey );
        if ( objectIter != m_objectsID2AdaptorIDVector.end() )
        {
            for(const AdaptorIDType& adaptorId :  objectIter->second )
            {
                this->startAdaptor( adaptorId, elem.second );
                SLM_ASSERT("Service "+adaptorId+" is not started",
                           m_adaptorID2SceneAdaptor2D[adaptorId].getService()->isStarted());
            }
        }
    }
}

//-----------------------------------------------------------------------------

void Render::swapAdaptorsFromComposite(const ::fwData::Composite::ContainerType& objects)
{
    for(const ::fwData::Composite::value_type& elem : objects)
    {
        std::string compositeKey                       = elem.first;
        ObjectsID2AdaptorIDVector::iterator objectIter = m_objectsID2AdaptorIDVector.find( compositeKey );
        if ( objectIter != m_objectsID2AdaptorIDVector.end() )
        {
            for(const AdaptorIDType& adaptorId : objectIter->second )
            {
                ::fwRuntime::ConfigurationElementContainer::Iterator iter;
                for (const auto& config : m_sceneConfiguration->getElements())
                {
                    if((config->getName() == "adaptor") && (config->getAttributeValue("id") == adaptorId))
                    {
                        this->swapAdaptor( adaptorId, elem.second );
                    }
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------

void Render::stopAdaptorsFromComposite(const ConstObjectMapType& objects)
{
    for(const auto& elem : objects)
    {
        std::string compositeKey                       = elem.first;
        ObjectsID2AdaptorIDVector::iterator objectIter = m_objectsID2AdaptorIDVector.find( compositeKey );
        if ( objectIter != m_objectsID2AdaptorIDVector.end() )
        {
            for(const AdaptorIDType& adaptorId : objectIter->second )
            {
                this->stopAdaptor( adaptorId );
            }
        }
    }
}

//-----------------------------------------------------------------------------

void Render::startAdaptor(const AdaptorIDType& _adaptorID, const CSPTR(::fwData::Object)& _object)
{
    if (!m_adaptorID2SceneAdaptor2D[_adaptorID].m_uid.empty())
    {
        m_adaptorID2SceneAdaptor2D[_adaptorID].m_service = ::fwServices::add< ::scene2D::adaptor::IAdaptor >(
            _object, m_adaptorID2SceneAdaptor2D[_adaptorID].m_type, m_adaptorID2SceneAdaptor2D[_adaptorID].m_uid);
    }
    else
    {
        m_adaptorID2SceneAdaptor2D[_adaptorID].m_service = ::fwServices::add< ::scene2D::adaptor::IAdaptor >(
            _object, m_adaptorID2SceneAdaptor2D[_adaptorID].m_type);
    }

    SLM_ASSERT("\"config\" tag required", m_adaptorID2SceneAdaptor2D[_adaptorID].m_config->getName() == "config");
    SLM_ASSERT("Adaptor service expired", m_adaptorID2SceneAdaptor2D[_adaptorID].getService());

    m_adaptorID2SceneAdaptor2D[_adaptorID].getService()->setScene2DRender(Render::dynamicCast(this->shared_from_this()));
    m_adaptorID2SceneAdaptor2D[_adaptorID].getService()->setConfiguration(
        m_adaptorID2SceneAdaptor2D[_adaptorID].m_config);
    m_adaptorID2SceneAdaptor2D[_adaptorID].getService()->configure();
    m_adaptorID2SceneAdaptor2D[_adaptorID].getService()->start();

    // Fill the map of unique zValues
    ensureUniqueZValue( m_adaptorID2SceneAdaptor2D[_adaptorID] );
    m_zValue2AdaptorID[ m_adaptorID2SceneAdaptor2D[_adaptorID].m_service.lock()->getZValue() ] = _adaptorID;

}

//-----------------------------------------------------------------------------

void Render::swapAdaptor(const AdaptorIDType& _adaptorID, const SPTR(::fwData::Object)& _object)
{
    SLM_TRACE_FUNC();

    m_adaptorID2SceneAdaptor2D[ _adaptorID ].getService()->swap( _object );
}

//-----------------------------------------------------------------------------

void Render::stopAdaptor(const AdaptorIDType& _adaptorID)
{
    SLM_TRACE_FUNC();

    SceneAdaptor2D& info = m_adaptorID2SceneAdaptor2D[_adaptorID];

    if(info.getService())
    {
        m_zValue2AdaptorID.erase( info.getService()->getZValue() );

        info.getService()->stop();
        SLM_ASSERT("Service is not stopped", info.getService()->isStopped());
        ::fwServices::OSR::unregisterService(info.getService());
        info.m_service.reset();
    }
}

//-----------------------------------------------------------------------------

void Render::updateSceneSize( float ratioPercent )
{
    QRectF rec = m_scene->itemsBoundingRect();
    qreal x, y, w, h;
    rec.getRect(&x, &y, &w, &h);

    if ( ratioPercent != 0 )
    {
        qreal centerX = x + w/2.0;
        qreal centerY = y + h/2.0;
        w = w + w * ratioPercent;
        h = h + h * ratioPercent;
        x = centerX - w/2.0;
        y = centerY - h/2.0;
        rec.setRect(x, y, w, h);
    }
    m_sceneStart.setX( x );
    m_sceneStart.setY( y );
    m_sceneWidth.setX( w );
    m_sceneWidth.setY( h );

    m_scene->setSceneRect( rec );
}

//------------------------------------------------------------------------------

void Render::addObjects(::fwData::Composite::ContainerType objects)
{
    ConstObjectMapType map;
    std::for_each(objects.begin(), objects.end(), [&map](
                      const ::fwData::Composite::ContainerType::value_type& it){ map[it.first] = it.second; });
    this->startAdaptorsFromComposite(map);
    this->connectAfterWait(objects);
}

//------------------------------------------------------------------------------

void Render::changeObjects(::fwData::Composite::ContainerType newObjects,
                           ::fwData::Composite::ContainerType oldObjects)
{
    this->disconnect(oldObjects);
    this->swapAdaptorsFromComposite(newObjects);
    this->connectAfterWait(newObjects);
}

//------------------------------------------------------------------------------

void Render::removeObjects(::fwData::Composite::ContainerType objects)
{
    this->disconnect(objects);
    ConstObjectMapType map;
    std::for_each(objects.begin(), objects.end(), [&map](
                      const ::fwData::Composite::ContainerType::value_type& it){ map[it.first] = it.second; });
    this->stopAdaptorsFromComposite(map);
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsType Render::getObjSrvConnections() const
{
    KeyConnectionsType connections;
    connections.push_back( std::make_pair( ::fwData::Composite::s_ADDED_OBJECTS_SIG, s_ADD_OBJECTS_SLOT ) );
    connections.push_back( std::make_pair( ::fwData::Composite::s_CHANGED_OBJECTS_SIG, s_CHANGE_OBJECTS_SLOT ) );
    connections.push_back( std::make_pair( ::fwData::Composite::s_REMOVED_OBJECTS_SIG, s_REMOVE_OBJECTS_SLOT ) );

    return connections;
}

//-----------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap Render::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push( s_DEFAULT_OBJECT, ::fwData::Composite::s_ADDED_OBJECTS_SIG, s_ADD_OBJECTS_SLOT );
    connections.push( s_DEFAULT_OBJECT, ::fwData::Composite::s_CHANGED_OBJECTS_SIG, s_CHANGE_OBJECTS_SLOT );
    connections.push( s_DEFAULT_OBJECT, ::fwData::Composite::s_REMOVED_OBJECTS_SIG, s_REMOVE_OBJECTS_SLOT );

    return connections;
}

//-----------------------------------------------------------------------------

} // namespace scene2D

