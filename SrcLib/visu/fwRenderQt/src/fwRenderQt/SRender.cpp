/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRenderQt/SRender.hpp"

#include "fwRenderQt/IAdaptor.hpp"
#include "fwRenderQt/registry/Adaptor.hpp"
#include "fwRenderQt/Scene2DGraphicsView.hpp"

#include <fwCom/helper/SigSlotConnection.hpp>
#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwGuiQt/container/QtContainer.hpp>

#include <fwServices/helper/Config.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/op/Add.hpp>

#include <QGraphicsRectItem>
#include <QVBoxLayout>

namespace fwRenderQt
{

static const ::fwServices::IService::KeyType s_VIEWPORT_INOUT = "viewport";

//-----------------------------------------------------------------------------

SRender::SRender() noexcept :
    m_sceneStart(-100., -100.),
    m_sceneWidth(200., 200.),
    m_scene(nullptr),
    m_view(nullptr),
    m_antialiasing(false),
    m_aspectRatioMode(Qt::IgnoreAspectRatio)
{
}

//-----------------------------------------------------------------------------

SRender::~SRender() noexcept
{
}

//-----------------------------------------------------------------------------

QGraphicsScene* SRender::getScene() const
{
    return m_scene;
}

//-----------------------------------------------------------------------------

Scene2DGraphicsView* SRender::getView() const
{
    return m_view;
}

//-----------------------------------------------------------------------------

::fwRenderQt::data::Viewport::sptr SRender::getViewport() const
{
    ::fwRenderQt::data::Viewport::sptr viewport = this->getInOut< ::fwRenderQt::data::Viewport>(s_VIEWPORT_INOUT);
    return viewport;
}

//-----------------------------------------------------------------------------

::fwRenderQt::data::Axis::sptr SRender::getAxis(const std::string& id) const
{
    ::fwRenderQt::data::Axis::sptr axis;
    const auto iter = m_axisMap.find(id);
    if(iter != m_axisMap.end())
    {
        axis = iter->second;
    }
    return axis;
}

//-----------------------------------------------------------------------------

void SRender::dispatchInteraction(::fwRenderQt::data::Event& _event)
{
    /* std::map are sorted regarding to key values. Keys of m_zValue2AdaptorID are z-values (float).
     *
     * The dispatching is performed to the highest z-valued adaptors first, so we start iterating
     * from the end of m_zValue2AdaptorID (with reverse_iterator).
     */
    if ( !_event.isAccepted() )
    {
        const auto& registry = ::fwRenderQt::registry::getAdaptorRegistry();

        for( const auto& elt : registry)
        {
            ::fwRenderQt::IAdaptor::sptr adaptor =
                ::fwRenderQt::IAdaptor::dynamicCast(::fwTools::fwID::getObject(elt.first));
            if(adaptor != nullptr && adaptor->isStarted())
            {
                adaptor->processInteraction( _event );
            }
        }
    }
}

//-----------------------------------------------------------------------------

::fwRenderQt::data::Coord SRender::mapToScene( const ::fwRenderQt::data::Coord& coord ) const
{
    /// Returns the viewport coordinate point mapped to scene coordinates.
    const QPoint qp( static_cast<int>(coord.getX()), static_cast<int>(coord.getY()) );
    const QPointF qps = m_view->mapToScene(qp);
    return ::fwRenderQt::data::Coord(qps.x(), qps.y());
}

//-----------------------------------------------------------------------------

void SRender::configuring()
{
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
            SLM_ASSERT("Bad scene configurationType, unknown xml node : " + (*iter)->getName(), false);
        }
    }
}

//-----------------------------------------------------------------------------

void SRender::starting()
{
    this->create();

    this->startContext();
}

//-----------------------------------------------------------------------------

void SRender::updating()
{
}

//-----------------------------------------------------------------------------

void SRender::swapping()
{
}

//-----------------------------------------------------------------------------

void SRender::swapping(const IService::KeyType& key)
{
}

//-----------------------------------------------------------------------------

void SRender::stopping()
{
    m_axisMap.clear();

    this->stopContext();
    this->destroy();
}

//-----------------------------------------------------------------------------

void SRender::startContext()
{
    SPTR(::fwGuiQt::container::QtContainer) qtContainer
        = ::fwGuiQt::container::QtContainer::dynamicCast(this->getContainer());

    m_scene = new QGraphicsScene( m_sceneStart.getX(), m_sceneStart.getY(), m_sceneWidth.getX(), m_sceneWidth.getY());
    m_scene->setBackgroundBrush(QBrush(QColor(0, 0, 0)));
    m_scene->setFocus( Qt::MouseFocusReason );

    m_view = new Scene2DGraphicsView( m_scene, qtContainer->getQtContainer() );
    m_view->setViewport( this->getViewport() );
    m_view->setSceneRender( ::fwRenderQt::SRender::dynamicCast( this->getSptr() ) );
    m_view->setRenderHint( QPainter::Antialiasing, m_antialiasing );

    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget(m_view);
    qtContainer->setLayout(layout);

    m_view->updateFromViewport();
}

//-----------------------------------------------------------------------------

void SRender::stopContext()
{
    delete m_scene;
    delete m_view;
}

//-----------------------------------------------------------------------------

Qt::AspectRatioMode SRender::getAspectRatioMode() const
{
    return m_aspectRatioMode;
}

//-----------------------------------------------------------------------------

void SRender::configureAxis( ConfigurationType _conf )
{
    SLM_ASSERT("\"axis\" tag required", _conf->getName() == "axis");

    const std::string id        = _conf->getAttributeValue("id");
    const std::string origin    = _conf->getAttributeValue("origin");
    const std::string scale     = _conf->getAttributeValue("scale");
    const std::string scaleType = _conf->getAttributeValue("scaleType");

    ::fwRenderQt::data::Axis::sptr axis = std::make_shared< ::fwRenderQt::data::Axis >();
    axis->setOrigin(std::stof( origin ));
    axis->setScale(std::stof( scale ));
    axis->setScaleType( scaleType == "LINEAR" ? ::fwRenderQt::data::Axis::LINEAR : ::fwRenderQt::data::Axis::LOG);
    m_axisMap[id] = axis;
}

//-----------------------------------------------------------------------------

void SRender::configureScene( ConfigurationType _conf )
{
    SLM_ASSERT("\"viewport\" tag required", _conf->getName() == "scene");

    const std::string x      = _conf->getAttributeValue("x");
    const std::string y      = _conf->getAttributeValue("y");
    const std::string width  = _conf->getAttributeValue("width");
    const std::string height = _conf->getAttributeValue("height");

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

void SRender::configureAdaptor( ConfigurationType _conf )
{
    SLM_ASSERT("\"adaptor\" tag required", _conf->getName() == "adaptor");

    const std::string adaptorId = _conf->getAttributeValue("uid");

    auto& registry = ::fwRenderQt::registry::getAdaptorRegistry();
    registry[adaptorId] = this->getID();
}

//-----------------------------------------------------------------------------

void SRender::updateSceneSize( float ratioPercent )
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

//-----------------------------------------------------------------------------

} // namespace fwRenderQt

