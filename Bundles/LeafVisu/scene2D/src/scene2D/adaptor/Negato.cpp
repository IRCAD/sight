/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <QGraphicsItemGroup>
#include <QPoint>
#include <QBitmap>
#include <QPixmap>

#include <fwTools/helpers.hpp>

#include <fwData/Image.hpp>
#include <fwData/Composite.hpp>
#include <fwData/String.hpp>
#include <fwData/TransfertFunction.hpp>
#include <fwData/TransfertFunctionPoint.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/IEditionService.hpp>

#include <fwComEd/ImageMsg.hpp>
#include <fwComEd/Dictionary.hpp>

#include "scene2D/adaptor/Negato.hpp"

REGISTER_SERVICE( ::scene2D::adaptor::IAdaptor , ::scene2D::adaptor::Negato  , ::fwData::Image ) ;


namespace scene2D
{
namespace adaptor
{

//-----------------------------------------------------------------------------

Negato::Negato() throw()
: m_pointIsCaptured (false), m_scaleRatio(1.1f), m_negatoIsBeingMoved(false)
{
    addNewHandledEvent( ::fwComEd::ImageMsg::SLICE_INDEX );
    addNewHandledEvent( ::fwComEd::ImageMsg::WINDOWING );
}

//-----------------------------------------------------------------------------

Negato::~Negato() throw()
{}

//-----------------------------------------------------------------------------

void Negato::configuring() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();

    SLM_ASSERT("\"config\" tag missing", m_configuration->getName() == "config");

    this->IAdaptor::configuring();

    SLM_TRACE("IAdaptor configuring ok");

    //if (!m_configuration->getAttributeValue("zValue").empty())
    //{
    //    m_zValue = ::boost::lexical_cast< float >( m_configuration->getAttributeValue("zValue"));
    //}
    
    if( !m_configuration->getAttributeValue("scaleRatio").empty() )
    {
        m_scaleRatio = ::boost::lexical_cast< float >( m_configuration->getAttributeValue("scaleRatio") );
    }
}

//-----------------------------------------------------------------------------

void Negato::updateFromImage( QImage * qimg )
{
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();
    std::vector< ::boost::int32_t > size = image->getSize();

    ::fwData::Integer::sptr axialIndex;
    ::fwTools::getFieldFromObject(axialIndex   , image, ::fwComEd::Dictionary::m_axialSliceIndexId   , ::fwData::Integer::New(0) );

    ::fwData::Integer::sptr minInt;
    ::fwTools::getFieldFromObject(minInt   , image, ::fwComEd::Dictionary::m_windowMinId   , ::fwData::Integer::New(0) );
    double min = minInt->value();

    ::fwData::Integer::sptr maxInt;
    ::fwTools::getFieldFromObject(maxInt   , image, ::fwComEd::Dictionary::m_windowMaxId   , ::fwData::Integer::New(100) );
    double max = maxInt->value();

    signed short * imgBuff = (signed short *) ( image->getBuffer() );
    double window = max - min;
    signed short val16;
    unsigned char val;
    unsigned int yoffset;
    unsigned int zoffset = axialIndex->value() * size[0] * size[1];

    for( ::boost::int32_t y = 0; y < size[1]; y++ )
    {
        yoffset = y * size[0];

        for( ::boost::int32_t x = 0; x < size[0]; x++ )
        {
            val16 = imgBuff[ x + yoffset + zoffset ];

            if ( val16 <= min )
            {
                val = 0;
            }
            else if ( val16 >= max )
            {
                val = 255;
            }
            else
            {
                val = ( ( val16 - min ) / window ) * 255;
            }

            qimg->setPixel(x,y,qRgb(val,val,val));
        }
    }
}

//-----------------------------------------------------------------------------

QImage * Negato::createQImage()
{
    ::fwData::Image::sptr img = this->getObject< ::fwData::Image >();
    std::vector< ::boost::int32_t > size = img->getSize();
    QImage * qimage = new QImage(size[0],size[1],QImage::Format_RGB888);
    return qimage;

}

//-----------------------------------------------------------------------------

void Negato::doStart() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    m_qimg = this->createQImage();
    this->updateFromImage( m_qimg );
    QPixmap m_pixmap = QPixmap::fromImage( *m_qimg );
    //m_pixmapItem =  this->getScene2DRender()->getScene()->addPixmap( m_pixmap );
    m_pixmapItem = new QGraphicsPixmapItem(m_pixmap);

    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();
    std::vector< double > spacing = image->getSpacing();
    m_pixmapItem->scale( spacing[0], spacing[1] );

    m_layer = new QGraphicsItemGroup();
    m_layer->addToGroup(m_pixmapItem);
    this->getScene2DRender()->getScene()->addItem( m_layer );

    m_pos = m_pixmapItem->pos();
}

//-----------------------------------------------------------------------------

void Negato::doUpdate() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();

    this->updateFromImage( m_qimg );
    QPixmap m_pixmap = QPixmap::fromImage( *m_qimg );
    m_pixmapItem->setPixmap( m_pixmap );

    // Adjust the layer's position and zValue depending on the associated axis
    m_layer->setPos(m_xAxis->getOrigin(), m_yAxis->getOrigin());
    m_layer->setZValue(m_zValue);
}

//-----------------------------------------------------------------------------

void Negato::doUpdate( fwServices::ObjectMsg::csptr _msg) throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();

    ::fwComEd::ImageMsg::csptr imageMsg = ::fwComEd::ImageMsg::dynamicConstCast(_msg);

    if(     imageMsg && imageMsg->hasEvent( ::fwComEd::ImageMsg::SLICE_INDEX ) ||
            imageMsg && imageMsg->hasEvent( ::fwComEd::ImageMsg::WINDOWING ) )
    {
        this->doUpdate();
    }
}

//-----------------------------------------------------------------------------

void Negato::doSwap() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void Negato::doStop() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    
    this->getScene2DRender()->getScene()->removeItem(m_layer);

    delete m_qimg;
    delete m_pixmapItem;
    delete  m_layer;
}

//-----------------------------------------------------------------------------

void Negato::processInteraction( ::scene2D::data::Event::sptr _event )
{
    SLM_TRACE_FUNC();

    std::vector< double > origin = ::fwData::Image::dynamicCast( this->getObject< fwData::Image>() )->getOrigin();
    OSLM_TRACE("Image origin = " << origin[0] << ", " << origin[1] << ", " << origin[2]);

    ::scene2D::data::Coord coord = this->getScene2DRender()->mapToScene( _event->getCoord() );
    coord.setX( coord.getX() / m_layer->scale() );
    coord.setY( coord.getY() / m_layer->scale() );

    if ( _event->getType() == ::scene2D::data::Event::MouseButtonPress
            && _event->getButton() == ::scene2D::data::Event::RightButton )
    {
        OSLM_TRACE("Point is captured");
        m_pointIsCaptured = true;
        m_oldCoord = _event->getCoord();
        _event->setAccepted(true);
    }
    else if ( m_pointIsCaptured )
    {
        if( _event->getType() == ::scene2D::data::Event::MouseMove )
        {
            ::scene2D::data::Coord newCoord = _event->getCoord();
            this->changeImageMinMaxFromCoord( m_oldCoord, newCoord );
            m_oldCoord = newCoord;
            _event->setAccepted(true);
        }
        else if( _event->getButton() == ::scene2D::data::Event::RightButton
                && _event->getType() == ::scene2D::data::Event::MouseButtonRelease )
        {
            m_pointIsCaptured = false;
            _event->setAccepted(true);
        }
    }
    else if(_event->getType() == ::scene2D::data::Event::MouseWheelUp)
    {
        m_layer->setScale(m_layer->scale() * m_scaleRatio);  
    }
    else if(_event->getType() == ::scene2D::data::Event::MouseWheelDown)
    {
        m_layer->setScale(m_layer->scale() / m_scaleRatio); 
    }
    else if(_event->getButton() == ::scene2D::data::Event::MidButton)
    {
        if(_event->getType() == ::scene2D::data::Event::MouseButtonPress)
        {
            m_negatoIsBeingMoved = true;
            m_pos.setX( coord.getX() );
            m_pos.setY( coord.getY() );
        }
        else if(_event->getType() == ::scene2D::data::Event::MouseButtonRelease)
        {
            m_negatoIsBeingMoved = false;
        }
    }
    else if(m_negatoIsBeingMoved)
    {
        if(_event->getType() == ::scene2D::data::Event::MouseMove)
        {
            m_pixmapItem->setPos(
                    m_pixmapItem->pos().x() + (coord.getX() - m_pos.x()),
                    m_pixmapItem->pos().y() + (coord.getY() - m_pos.y()) );

            m_pos.setX(coord.getX());
            m_pos.setY(coord.getY());
        }
    }
}

//-----------------------------------------------------------------------------

void Negato::changeImageMinMaxFromCoord( scene2D::data::Coord & oldCoord, scene2D::data::Coord & newCoord )
{
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();

    ::fwData::Integer::sptr minInt;
    ::fwTools::getFieldFromObject(minInt   , image, ::fwComEd::Dictionary::m_windowMinId   , ::fwData::Integer::New(0) );
    double min = minInt->value();

    ::fwData::Integer::sptr maxInt;
    ::fwTools::getFieldFromObject(maxInt   , image, ::fwComEd::Dictionary::m_windowMaxId   , ::fwData::Integer::New(100) );
    double max = maxInt->value();

    double window = newCoord.getX() - m_oldCoord.getX();
    double level = newCoord.getY() - m_oldCoord.getY();

    double imgWindow = max - min;
    double imgLevel = min + imgWindow/2.0;


    double newImgLevel = imgLevel + level;
    double newImgWindow = imgWindow + imgWindow * window/100.0;

    if ( newImgWindow > 10 ) // Hack ToDo
    {
        double newMin = newImgLevel - newImgWindow/2.0;
        double newMax = newImgLevel + newImgWindow/2.0;

        minInt->value() = newMin;
        maxInt->value() = newMax;

        this->doUpdate();

        // Fire the message
        ::fwComEd::ImageMsg::NewSptr msg;
        msg->setWindowMinMax(minInt, maxInt);
        ::fwServices::IEditionService::notify(this->getSptr(), image, msg);
    }
}

//-----------------------------------------------------------------------------

} // namespace adaptor
} // namespace scene2D



