/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <QGraphicsItemGroup>
#include <QPoint>
#include <QBitmap>
#include <QPixmap>

#include <fwData/Image.hpp>
#include <fwData/Composite.hpp>
#include <fwData/Integer.hpp>
#include <fwData/TransferFunction.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/IEditionService.hpp>

#include <fwComEd/ImageMsg.hpp>
#include <fwComEd/TransferFunctionMsg.hpp>
#include <fwComEd/Dictionary.hpp>

#include "scene2D/adaptor/Negato.hpp"

REGISTER_SERVICE( ::scene2D::adaptor::IAdaptor , ::scene2D::adaptor::Negato  , ::fwData::Image ) ;


namespace scene2D
{
namespace adaptor
{

typedef ::fwComEd::helper::MedicalImageAdaptor MedicalImageAdaptor;

//-----------------------------------------------------------------------------

Negato::Negato() throw()
: m_pointIsCaptured (false), m_scaleRatio(1.1f), m_negatoIsBeingMoved(false),
    m_orientation(MedicalImageAdaptor::Z_AXIS), m_pos(0, 0), m_changeSliceTypeAllowed(true)
{
    this->installTFPoolEventHandler(this);
    this->addNewHandledEvent( ::fwComEd::ImageMsg::SLICE_INDEX );
    this->addNewHandledEvent( ::fwComEd::TransferFunctionMsg::WINDOWING );
    this->addNewHandledEvent( ::fwComEd::ImageMsg::CHANGE_SLICE_TYPE );
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

    if( !m_configuration->getAttributeValue("scaleRatio").empty() )
    {
        m_scaleRatio = ::boost::lexical_cast< float >( m_configuration->getAttributeValue("scaleRatio") );
    }

    if( !m_configuration->getAttributeValue("orientation").empty() )
    {
        const std::string orientationValue = m_configuration->getAttributeValue("orientation");

        if ( orientationValue == "axial" )
        {
            m_orientation = MedicalImageAdaptor::Z_AXIS;
        }
        else if ( orientationValue == "sagittal" )
        {
            m_orientation = MedicalImageAdaptor::X_AXIS;
        }
        else if ( orientationValue == "frontal" )
        {
            m_orientation = MedicalImageAdaptor::Y_AXIS;
        }
    }

    if(!m_configuration->getAttributeValue("changeSliceType").empty())
    {
        const std::string changeValue = m_configuration->getAttributeValue(("changeSliceType"));

        if(changeValue == "true" || changeValue == "yes")
        {
            m_changeSliceTypeAllowed = true;
        }
        else if(changeValue == "no" || changeValue == "false")
        {
            m_changeSliceTypeAllowed = false;
        }
    }
    this->parseTFConfig( m_configuration );
}

//-----------------------------------------------------------------------------

void Negato::updateFromImage( QImage * qimg )
{
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();
    this->updateImageInfos(image);
    const ::fwData::Image::SizeType size = image->getSize();

    const std::vector< double > spacing = image->getSpacing();
    double qImageSpacing[2];

    SLM_ASSERT("Negato orientation must be MedicalImageAdaptor::X_AXIS, MedicalImageAdaptor::Y_AXIS, or MedicalImageAdaptor::Z_AXIS",
        m_orientation == MedicalImageAdaptor::X_AXIS
        || m_orientation == MedicalImageAdaptor::Y_AXIS
        || m_orientation == MedicalImageAdaptor::Z_AXIS);

    ::fwData::Integer::sptr index;
    switch (m_orientation)
    {
        case MedicalImageAdaptor::X_AXIS:
            index = image->setDefaultField(::fwComEd::Dictionary::m_sagittalSliceIndexId, ::fwData::Integer::New(0));
            qImageSpacing[0] = spacing[1];
            qImageSpacing[1] = spacing[2];
            SLM_TRACE("Orientation = X");
            break;

        case MedicalImageAdaptor::Y_AXIS:
            index = image->setDefaultField(::fwComEd::Dictionary::m_frontalSliceIndexId, ::fwData::Integer::New(0));
            qImageSpacing[0] = spacing[0];
            qImageSpacing[1] = spacing[2];
            SLM_TRACE("Orientation = Y");
            break;

        case MedicalImageAdaptor::Z_AXIS:
            index = image->setDefaultField(::fwComEd::Dictionary::m_axialSliceIndexId, ::fwData::Integer::New(0));
            qImageSpacing[0] = spacing[0];
            qImageSpacing[1] = spacing[1];
            SLM_TRACE("Orientation = Z");
            break;

        default:
            SLM_FATAL("Unsupported value for m_orientation");
            break;
    }

    OSLM_TRACE("Updating from slice index " << index->value());

    ::fwData::TransferFunction::sptr tf = this->getTransferFunction();

    // Window min
    ::fwData::Integer::sptr minInt;
    const double min = tf->getMinMaxTFValues().first;

    // Window max
    const double max = tf->getMinMaxTFValues().second;

    signed short * imgBuff = (signed short *) ( image->getBuffer() );
    const double window = max - min;
    const unsigned int imageZOffset = size[0] * size[1];


    // Fill image according to current slice type:
    if(m_orientation == MedicalImageAdaptor::X_AXIS)
    {
        for( ::boost::int32_t z = 0; z < size[2]; ++z)
        {
            const unsigned int zOffset = z * imageZOffset;
            const unsigned int zPos = size[2] - 1 - z;
            const unsigned int zxOffset = zOffset + index->value();

            for( ::boost::int32_t y = 0; y < size[1]; y++ )
            {
                const unsigned char val = getQImageVal(zxOffset + y * size[0], imgBuff, max, min, window);
                qimg->setPixel(y, zPos, qRgb(val,val,val));
            }
        }
    }
    else if(m_orientation == MedicalImageAdaptor::Y_AXIS)
    {
        const double yOffset = index->value() * size[0];

        for( ::boost::int32_t z = 0; z < size[2]; ++z)
        {
            const double zOffset = z * imageZOffset;
            const double zPos = size[2] - 1 - z;
            const double zyOffset = zOffset + yOffset;

            for( ::boost::int32_t x = 0; x < size[0]; x++ )
            {
                const unsigned char val = getQImageVal(zyOffset + x, imgBuff, max, min, window);
                qimg->setPixel(x, zPos, qRgb(val,val,val));
            }
        }
    }
    else if(m_orientation == MedicalImageAdaptor::Z_AXIS)
    {
        const double zOffset = index->value() * imageZOffset;

        for( ::boost::int32_t y = 0; y < size[1]; y++ )
        {
            const unsigned int yOffset = y * size[0];
            const unsigned int zyOffset = zOffset + yOffset;

            for( ::boost::int32_t x = 0; x < size[0]; x++ )
            {
                const unsigned char val = getQImageVal(x + zyOffset, imgBuff, max, min, window);
                qimg->setPixel(x,y,qRgb(val,val,val));
            }
        }
    }

    m_pixmapItem->scale(qImageSpacing[0] / m_scale.first, qImageSpacing[1] / m_scale.second);

    m_scale.first = qImageSpacing[0];
    m_scale.second = qImageSpacing[1];
}

//-----------------------------------------------------------------------------

unsigned char Negato::getQImageVal(
    const unsigned int _index, signed short* _buffer, const double _max, const double _min, const double _window)
{
    signed short val16 = _buffer[_index];
    unsigned char val;

    if ( val16 <= _min )
    {
        val = 0;
    }
    else if ( val16 >= _max )
    {
        val = 255;
    }
    else
    {
        val = ( ( val16 - _min ) / _window ) * 255;
    }

    return val;
}

//---------------------------------------------------------------------------

QImage * Negato::createQImage()
{
    SLM_TRACE_FUNC();

    ::fwData::Image::sptr img = this->getObject< ::fwData::Image >();
    const ::fwData::Image::SizeType size = img->getSize();

    ::boost::int32_t qImageSize[2];
    ::fwData::Image::SizeType::value_type width;
    ::fwData::Image::SizeType::value_type height;

    switch(m_orientation)
    {
        case MedicalImageAdaptor::X_AXIS:
            width  = size[1];
            height = size[2];
            break;

        case MedicalImageAdaptor::Y_AXIS:
            width   = size[0];
            height  = size[2];
            break;

        case MedicalImageAdaptor::Z_AXIS:
            width   = size[0];
            height  = size[1];
            break;

        default:
            SLM_FATAL("Unsupported value for m_orientation");
            break;
    }

    qImageSize[0] = static_cast< ::boost::int32_t >(width);
    qImageSize[1] = static_cast< ::boost::int32_t >(height);
    QImage * qimage = new QImage(qImageSize[0], qImageSize[1], QImage::Format_RGB888);
    return qimage;
}

//-----------------------------------------------------------------------------

void Negato::doStart() throw ( ::fwTools::Failed )
{
    m_pixmapItem = new QGraphicsPixmapItem();
    m_qimg = this->createQImage();

    m_scale.first = 1;
    m_scale.second = 1;

    this->updateFromImage( m_qimg );

    QPixmap m_pixmap = QPixmap::fromImage( *m_qimg );
    m_pixmapItem->setPixmap(m_pixmap);

    m_layer = new QGraphicsItemGroup();
    m_layer->addToGroup(m_pixmapItem);
    m_layer->setPos(m_xAxis->getOrigin(), m_yAxis->getOrigin());

    this->getScene2DRender()->getScene()->addItem( m_layer );

    this->installTFObserver( this->getSptr() );
}

//-----------------------------------------------------------------------------

void Negato::doUpdate() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();

    this->updateFromImage( m_qimg );
    QPixmap m_pixmap = QPixmap::fromImage( *m_qimg );
    m_pixmapItem->setPixmap( m_pixmap );

    m_layer->setZValue(m_zValue);
}

//-----------------------------------------------------------------------------

void Negato::doUpdate( fwServices::ObjectMsg::csptr msg) throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    if(msg->hasEvent( ::fwComEd::ImageMsg::CHANGE_SLICE_TYPE) && m_changeSliceTypeAllowed)
    {
        ::fwData::Object::csptr cObjInfo = msg->getDataInfo( ::fwComEd::ImageMsg::CHANGE_SLICE_TYPE );
        ::fwData::Object::sptr objInfo = ::boost::const_pointer_cast< ::fwData::Object > ( cObjInfo );
        ::fwData::Composite::sptr info = ::fwData::Composite::dynamicCast ( objInfo );

        ::fwData::Integer::sptr fromSliceType = ::fwData::Integer::dynamicCast( info->getRefMap()["fromSliceType"] );
        ::fwData::Integer::sptr toSliceType = ::fwData::Integer::dynamicCast( info->getRefMap()["toSliceType"] );

        if( toSliceType->value() == static_cast< int > ( m_orientation ) )
        {
            m_orientation = static_cast< MedicalImageAdaptor::Orientation > ( fromSliceType->value() );
        }
        else if(fromSliceType->value() == static_cast<int>(m_orientation))
        {
            m_orientation = static_cast< MedicalImageAdaptor::Orientation >( toSliceType->value() );
        }

        m_qimg = this->createQImage();  // create image according to new orientation (handle size change)
        this->doUpdate();
    }

    if(this->upadteTFObserver(msg) ||
            msg->hasEvent( ::fwComEd::ImageMsg::SLICE_INDEX ) ||
            msg->hasEvent( ::fwComEd::TransferFunctionMsg::WINDOWING ) )
    {
        this->doUpdate();
    }
}

//-----------------------------------------------------------------------------

void Negato::doSwap() throw ( ::fwTools::Failed )
{
    const QPointF oldPos = m_pixmapItem->pos();
    const float scale = m_layer->scale();

    this->doStop();
    this->doStart();
    this->doUpdate();

    m_pixmapItem->setPos(oldPos);
    m_layer->setScale(scale);
    m_pos = oldPos;
}

//-----------------------------------------------------------------------------

void Negato::doStop() throw ( ::fwTools::Failed )
{
    this->removeTFObserver();
    this->getScene2DRender()->getScene()->removeItem(m_layer);

    delete m_qimg;
    delete m_pixmapItem;
    delete m_layer;
}

//-----------------------------------------------------------------------------

void Negato::processInteraction( ::scene2D::data::Event::sptr _event )
{
    SLM_TRACE_FUNC();

    ::scene2D::data::Coord coord = this->getScene2DRender()->mapToScene( _event->getCoord() );
    coord.setX( coord.getX() / m_layer->scale());
    coord.setY( coord.getY() / m_layer->scale());

    if ( _event->getType() == ::scene2D::data::Event::MouseButtonPress
            && _event->getButton() == ::scene2D::data::Event::RightButton )
    {
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
    else if(_event->getButton() == ::scene2D::data::Event::MidButton
            && _event->getModifier() == ::scene2D::data::Event::NoModifier)
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
    ::fwData::TransferFunction::sptr tf = this->getTransferFunction();

    double min = tf->getMinMaxTFValues().first;
    double max = tf->getMinMaxTFValues().second;

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

        this->doUpdate();

        // Fire the message
        this->setWindowLevel(newMin, newMax);
        this->notifyTFWindowing(this->getSptr());
    }
}

//-----------------------------------------------------------------------------

} // namespace adaptor
} // namespace scene2D



