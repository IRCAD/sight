/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "scene2D/adaptor/ScaleValues.hpp"

#include "scene2D/Scene2DGraphicsView.hpp"
#include "scene2D/data/InitQtPen.hpp"

#include <fwData/Composite.hpp>

#include <fwServices/macros.hpp>

#include <QGraphicsItemGroup>

fwServicesRegisterMacro( ::scene2D::adaptor::IAdaptor, ::scene2D::adaptor::ScaleValues, ::fwData::Composite );

namespace scene2D
{
namespace adaptor
{

ScaleValues::ScaleValues() throw() :
    m_min(0.f),
    m_max(0.f),
    m_interval(10.),
    m_step(1),
    m_fontSize(8.f),
    m_showUnit(true),
    m_unit(nullptr),
    m_layer(nullptr)
{
}

//---------------------------------------------------------------------------------------------------------------

ScaleValues::~ScaleValues() throw()
{
}

//---------------------------------------------------------------------------------------------------------------

void ScaleValues::configuring() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();

    SLM_ASSERT("\"config\" tag missing", m_configuration->getName() == "config");

    this->IAdaptor::configuring();  // xAxis, yAxis, zValue, opacity

    SLM_TRACE("IAdaptor configuring ok");

    // Set the min/max values
    const std::string min = m_configuration->getAttributeValue("min");
    const std::string max = m_configuration->getAttributeValue("max");
    SLM_ASSERT("'min' atttribute is missing.", !min.empty());
    SLM_ASSERT("'min' atttribute is missing.", !max.empty());

    m_min = std::stod( min );
    m_max = std::stod( max );

    // Interval configuration
    if (!m_configuration->getAttributeValue("interval").empty())
    {
        m_interval = std::stof( m_configuration->getAttributeValue("interval") );
    }

    // Color configuration
    const std::string color = m_configuration->getAttributeValue("color");
    if (!color.empty())
    {
        ::scene2D::data::InitQtPen::setPenColor(m_pen, color, m_opacity);
    }
    else
    {
        ::scene2D::data::InitQtPen::setPenColor(m_pen, "white", m_opacity);
    }

    // Font size configuratiion
    if (!m_configuration->getAttributeValue("fontSize").empty())
    {
        m_fontSize = std::stof( m_configuration->getAttributeValue("fontSize") );
    }

    // Viewport configuratiion
    SLM_ASSERT("A viewport attribute must be specified with 'viewportUID'.",
               !m_configuration->getAttributeValue("viewportUID").empty());

    if( !m_configuration->getAttributeValue("viewportUID").empty() )
    {
        m_viewportID = m_configuration->getAttributeValue("viewportUID");
    }

    // Show unit
    const std::string showUnit = m_configuration->getAttributeValue("showUnit");
    if(showUnit == "true" || showUnit == "false")
    {
        m_showUnit = (showUnit == "true");
    }

    // Unit text configuration
    m_displayedUnit = m_configuration->getAttributeValue("unit");

    // 'align' attribute configuration
    m_align = m_configuration->getAttributeValue("align");

    SLM_ASSERT(
        "'align' attribute is missing. Please add an 'align' attribute with value 'left', 'right', 'top' or 'bottom'",
        !m_align.empty());

    SLM_ASSERT("Unsupported value for 'align' attribute.",
               m_align == "left" || m_align == "right" || m_align == "top" || m_align == "bottom");
}

//---------------------------------------------------------------------------------------------------------------

void ScaleValues::buildValues()
{
    SLM_TRACE_FUNC();
    SLM_ASSERT("m_interval can not be equal to 0", m_interval != 0);

    m_values.clear();

    double val         = this->getStartVal();
    const int range    = (int) ceil(this->getEndVal() - val);
    const int nbValues = (int)(ceil(range/ m_interval)) + 1;

    std::string format;

    if(m_align == "left")
    {
        format = "  %1";
    }
    else if(m_align == "right")
    {
        format = "%1  ";
    }
    else
    {
        format = "%1";
    }

    // Build the values as graphic items
    for(int i = 0; i < nbValues; ++i, val += m_interval)
    {
        QGraphicsSimpleTextItem* text = new QGraphicsSimpleTextItem();
        text->setText( QString::fromStdString(format).arg(val) );
        text->setFont( m_font );
        text->setBrush( m_brush );
        text->setCacheMode( QGraphicsItem::DeviceCoordinateCache );

        m_values.push_back( text );
    }

    // Add the values to the item group
    for( QGraphicsItem* item : m_values )
    {
        m_layer->addToGroup( item );
    }

    // Build the unit graphic item
    m_unit = new QGraphicsSimpleTextItem();
    m_unit->setText( QString::fromStdString( m_displayedUnit ) );
    m_unit->setFont( m_font );
    m_unit->setBrush( m_brush );
    m_unit->setCacheMode( QGraphicsItem::DeviceCoordinateCache );

    // Add it to the items to be displayed if required
    if( m_showUnit )
    {
        m_layer->addToGroup( m_unit );
    }

    // Set the layer position (according to the related axis) and zValue
    m_layer->setPos( m_xAxis->getOrigin(), m_yAxis->getOrigin() );
    m_layer->setZValue( m_zValue );

    // Add the layer containing grid's lines to the scene
    this->getScene2DRender()->getScene()->addItem( m_layer );
}

//---------------------------------------------------------------------------------------

void ScaleValues::doStart() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();

    // Initialize the layer
    m_layer = new QGraphicsItemGroup();

    m_brush = QBrush( m_pen.color() );
    m_pen.setCosmetic(true);

    m_font.setPointSize( m_fontSize );
    m_font.setLetterSpacing( QFont::AbsoluteSpacing, 0.25 );
    m_font.setKerning( true );
    m_font.setFixedPitch( true );

    m_viewport = this->getSafeInOut< ::scene2D::data::Viewport>( m_viewportID );
    SLM_ASSERT("Viewport '" + m_viewportID + "' is not found", m_viewport);

    m_connection = m_viewport->signal(::fwData::Object::s_MODIFIED_SIG)->connect(
        this->slot(::fwServices::IService::s_UPDATE_SLOT));

    this->buildValues();
    this->doUpdate();
}

//---------------------------------------------------------------------------------------

double ScaleValues::getStartVal()
{
    return (int)( m_min / m_interval ) * m_interval;
}

//---------------------------------------------------------------------------------------
double ScaleValues::getEndVal()
{
    return (int)( m_max / m_interval ) * m_interval;
}

//---------------------------------------------------------------------------------------

void ScaleValues::doUpdate() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();

    this->initializeViewSize();
    this->initializeViewportSize();

    rescaleValues();
}

//---------------------------------------------------------------------------------------

void ScaleValues::rescaleValues()
{
    const double viewportX      = m_viewport->getX();
    const double viewportWidth  = m_viewport->getWidth();
    const double viewportHeight = m_viewport->getHeight();

    const double viewportSizeRatio    = viewportHeight / viewportWidth;
    const double viewInitialSizeRatio = m_viewInitialSize.first / m_viewInitialSize.second;

    Scene2DRatio ratio        = this->getRatio(); // Total ratio
    double viewportWidthRatio = this->getViewportSizeRatio().first;

    double scaleX = m_fontSize;
    double scaleY = m_fontSize * viewportSizeRatio;
    scaleY /= viewportWidthRatio;
    scaleY *= viewInitialSizeRatio;

    scaleX = scaleX * ratio.first;
    scaleY = scaleY * ratio.second;

    QTransform transform;
    transform.scale( scaleX, scaleY );

    int step = 0;
    double valueSize;                   // the size (width or height) of the current value
    std::pair<double, double> coord;    // coordinates of the current scale value item
    std::pair<double, double> size;     // size of the current scale value item
    bool suggestResampling = false;     /* scale value items resampling is suggested because of
                                           a lack of sufficient width to display all of them */

    const size_t valuesSize = m_values.size();
    float val               = getStartVal();

    if(m_align == "left" || m_align == "right")
    {
        const double valueSizeRatio = m_interval / scaleY;

        float coeff = 0;

        double textPosX;

        if(m_align == "left")
        {
            textPosX = viewportX;
        }
        else
        {
            coeff    = -1;
            textPosX = viewportX + viewportWidth;
        }

        for(int i = 0; i < valuesSize; ++i, val += m_interval)
        {
            valueSize = m_values[i]->boundingRect().height();

            size = this->mapAdaptorToScene(
                std::pair<double, double>(m_values[i]->boundingRect().width(), valueSize),
                m_xAxis, m_yAxis);

            step = (int)(valueSize / valueSizeRatio) + 1;

            if( step > m_step )
            {
                m_step            = step;
                suggestResampling = true;
            }

            coord = this->mapAdaptorToScene(
                std::pair<double, double>(textPosX, val), m_xAxis, m_yAxis);

            m_values[i]->setTransform( transform );

            m_values[i]->setPos(
                coord.first + coeff * size.first * scaleX,
                coord.second - (m_interval - size.second / 2) * scaleY );
        }

        m_unit->setTransform( transform );

        val = viewportHeight * 0.8;

        coord = this->mapAdaptorToScene(
            std::pair<double, double>(textPosX, val), m_xAxis, m_yAxis);

        coeff = (m_align == "left") ? 1 : -1.5;

        m_unit->setPos(
            coord.first + coeff * 2 * size.first * scaleX,
            coord.second + size.second * scaleY);
    }
    else    // axis centered on top or bottom
    {
        const double valueSizeRatio = m_interval / scaleX;

        float coeff = 0.5;

        double textPosY = (m_align == "bottom")
                          ? m_viewport->getY()
                          : viewportHeight * 0.9;

        for(int i = 0; i < valuesSize; ++i, val += m_interval)
        {
            valueSize = m_values[i]->boundingRect().width();

            size = this->mapAdaptorToScene(
                std::pair<double, double>(valueSize, m_values[i]->boundingRect().height()),
                m_xAxis, m_yAxis);

            step = (int)(valueSize / valueSizeRatio) + 1;

            if( step > m_step )
            {
                m_step            = step;
                suggestResampling = true;
            }

            coord = this->mapAdaptorToScene(std::pair<double, double>(val, textPosY),
                                            m_xAxis, m_yAxis);

            m_values[i]->setTransform( transform );

            m_values[i]->setPos(
                coord.first - size.first / 2 * scaleX,
                coord.second - coeff * size.second / 2 * scaleY );
        }

        m_unit->setTransform( transform );

        val = viewportHeight * 0.8;

        size = this->mapAdaptorToScene(
            std::pair<double, double>(m_unit->boundingRect().width(), m_unit->boundingRect().height()),
            m_xAxis, m_yAxis);

        coord = this->mapAdaptorToScene(
            std::pair<double, double>(
                viewportX + viewportWidth / 2, textPosY),
            m_xAxis, m_yAxis);

        coeff = (m_align == "left") ? 1 : -1.5;

        m_unit->setPos(
            coord.first - size.first * scaleX,
            coord.second - 1.5 * size.second * scaleY);
    }

    if( suggestResampling )
    {
        showHideScaleValues();
    }
    else if( !suggestResampling && step != m_step )
    {
        m_step = step;
        showHideScaleValues();
    }
}

//---------------------------------------------------------------------------------------

void ScaleValues::showHideScaleValues()
{
    double value;
    const int size        = (int)m_values.size();
    const double startVal = this->getStartVal();

    for(int i = 0; i < size; ++i)
    {
        value = i * m_interval + startVal;  // compute the value at index 'i'

        // Display this value or not according to the current step between two consecutive values
        // and in keeping the displaying of the value '0':
        m_values[i]->setVisible( fmod(value, (m_step * m_interval)) == 0.0 );
    }
}

//---------------------------------------------------------------------------------------

void ScaleValues::processInteraction( ::scene2D::data::Event::sptr _event)
{
    if( _event->getType() == ::scene2D::data::Event::Resize)
    {
        doUpdate();
    }
}

//----------------------------------------------------------------------------------------

void ScaleValues::doSwap() throw ( ::fwTools::Failed )
{
}

//---------------------------------------------------------------------------------------

void ScaleValues::doStop() throw ( ::fwTools::Failed )
{
    // Remove the layer (and therefore all its related items) from the scene
    this->getScene2DRender()->getScene()->removeItem(m_layer);

    m_connection.disconnect();
}

} // namespace adaptor

} // namespace scene2D

