/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <QApplication>
#include <QWidget>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QSlider>
#include <QIntValidator>

#include <boost/math/special_functions/fpclassify.hpp>

#include <fwData/Image.hpp>
#include <fwData/Composite.hpp>

#include <fwCore/base.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/registry/ObjectService.hpp>
#include <fwServices/IEditionService.hpp>

#include <fwComEd/ImageMsg.hpp>
#include <fwComEd/Dictionary.hpp>
#include <fwComEd/fieldHelper/MedicalImageHelpers.hpp>

#include <fwGuiQt/container/QtContainer.hpp>
#include <fwGuiQt/widget/QRangeSlider.hpp>


#include "uiImageQt/WindowLevel.hpp"

namespace uiImage
{

REGISTER_SERVICE( ::gui::editor::IEditor , ::uiImage::WindowLevel , ::fwData::Image ) ;

//------------------------------------------------------------------------------

WindowLevel::WindowLevel() throw()
{
    m_imageDynamicMin   = -200.;
    m_imageDynamicWidth =  500.;

    addNewHandledEvent(::fwComEd::ImageMsg::WINDOWING);
    addNewHandledEvent(::fwComEd::ImageMsg::TRANSFERTFUNCTION);
}

//------------------------------------------------------------------------------

WindowLevel::~WindowLevel() throw()
{}

//------------------------------------------------------------------------------

void WindowLevel::starting() throw(::fwTools::Failed)
{
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();

    ::fwData::Integer::sptr min = image->getFieldSingleElement< ::fwData::Integer >( ::fwComEd::Dictionary::m_windowMinId );
    ::fwData::Integer::sptr max = image->getFieldSingleElement< ::fwData::Integer >( ::fwComEd::Dictionary::m_windowMaxId );

    this->create();
    ::fwGuiQt::container::QtContainer::sptr qtContainer =  ::fwGuiQt::container::QtContainer::dynamicCast( this->getContainer() );
    QWidget * const container = qtContainer->getQtContainer();
    SLM_ASSERT("container not instanced", container);

    QHBoxLayout* layout = new QHBoxLayout();

    m_valueTextMin = new QLineEdit( container );
    QIntValidator *minValidator = new QIntValidator(m_valueTextMin);
    m_valueTextMin->setValidator(minValidator);
    //m_valueTextMin->setText(QString("%1").arg(min->value()));

    m_valueTextMax = new QLineEdit( container );
    QIntValidator*  maxValidator = new QIntValidator(m_valueTextMax);
    m_valueTextMax->setValidator(maxValidator);
    //m_valueTextMax->setText(QString("%1").arg(max->value()));

    m_rangeSlider = new ::fwGuiQt::widget::QRangeSlider(container);

    //m_sliceSelectorMin = new QSlider( Qt::Horizontal, container );
    //m_sliceSelectorMin->setSliderPosition(min->value());
    //m_sliceSelectorMin->setMinimumWidth(40);

    //m_sliceSelectorMax = new QSlider( Qt::Horizontal, container );
    //m_sliceSelectorMax->setSliderPosition(max->value());
    //m_sliceSelectorMax->setMinimumWidth(40);

    layout->addWidget( m_valueTextMin );
    //layout->addWidget( m_sliceSelectorMin );
    //layout->addWidget( m_sliceSelectorMax );
    layout->addWidget( m_rangeSlider, 1 );
    layout->addWidget( m_valueTextMax );


    container->setLayout( layout );
    this->updating();

    //onImageWindowLevelChanged(*min, *max);

    QObject::connect(m_valueTextMin, SIGNAL(textEdited( QString )), this, SLOT(onMinTextChanged( QString )));
    QObject::connect(m_valueTextMax, SIGNAL(textEdited( QString )), this, SLOT(onMaxTextChanged( QString )));
    QObject::connect(m_rangeSlider, SIGNAL(sliderRangeChanged( double, double )), this, SLOT(onWindowLevelWidgetChanged( double, double )));
}

//------------------------------------------------------------------------------

void WindowLevel::stopping() throw(::fwTools::Failed)
{

    QObject::disconnect(m_rangeSlider, SIGNAL(sliderRangeChanged( double, double )), this, SLOT(onWindowLevelWidgetChanged( double, double )));
    QObject::disconnect(m_valueTextMin, SIGNAL(textEdited( QString )), this, SLOT(onMinTextChanged( QString )));
    QObject::disconnect(m_valueTextMax, SIGNAL(textEdited( QString )), this, SLOT(onMaxTextChanged( QString )));

    this->getContainer()->clean();
    this->destroy();
}

//------------------------------------------------------------------------------

void WindowLevel::configuring() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->initialize();
}

//------------------------------------------------------------------------------

void WindowLevel::updating() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();

    bool imageIsValid = ::fwComEd::fieldHelper::MedicalImageHelpers::checkImageValidity( image );
    this->setEnabled(imageIsValid);

    if(imageIsValid)
    {

        ::fwTools::DynamicType pixelType = image->getPixelType();
        SLM_ASSERT("Sorry, unmanaged type.",  pixelType != ::fwTools::DynamicType());

        std::string imageType = pixelType.string();
        bool isSigned = pixelType.isSigned();
        bool isFixedPrecision = pixelType.isFixedPrecision();

        int minValue = 0;
        int maxValue = 0;

        if(isFixedPrecision)
        {
            // Type is long, int, short, char
            if(isSigned)
            {
                if(imageType == "signed int")
                {
                    minValue = std::numeric_limits<int>::min();
                    maxValue = std::numeric_limits<int>::max();
                }
                else if(imageType == "signed short")
                {
                    minValue = std::numeric_limits<short>::min();
                    maxValue = std::numeric_limits<short>::max();
                }
                else if (imageType == "signed char")
                {
                    minValue = std::numeric_limits<signed char>::min();
                    maxValue = std::numeric_limits<signed char>::max();
                }
                else
                {
                    SLM_FATAL("The pixel type is not managed.");
                }
            }
            else
            {
                if(imageType == "unsigned int")
                {
                    minValue = std::numeric_limits<unsigned int>::min();
                    maxValue = std::numeric_limits<int>::max(); // due to the QSlider which take int and due to the TF uses fwData::Integer
                }
                else if(imageType == "unsigned short")
                {
                    minValue = std::numeric_limits<unsigned short>::min();
                    maxValue = std::numeric_limits<unsigned short>::max();
                }
                else if (imageType == "unsigned char")
                {
                    minValue = std::numeric_limits<unsigned char>::min();
                    maxValue = std::numeric_limits<unsigned char>::max();
                }
                else
                {
                    SLM_FATAL("The pixel type is not managed.");
                }
            }

        }
        else
        {
            // Image in Floa, double format
            // due to the QSlider which take int and due to the TF uses fwData::Integer
            minValue = std::numeric_limits<int>::min();
            maxValue = std::numeric_limits<int>::max();
        }

        //m_sliceSelectorMin->setMinimum(minValue);
        //m_sliceSelectorMin->setMaximum(maxValue);

        //m_sliceSelectorMax->setMinimum(minValue);
        //m_sliceSelectorMax->setMaximum(maxValue);

        //m_imageDynamicMin   = minValue;
        //m_imageDynamicWidth = maxValue-minValue;
    }
    ::fwData::Integer::sptr min = image->getFieldSingleElement< ::fwData::Integer >( ::fwComEd::Dictionary::m_windowMinId );
    ::fwData::Integer::sptr max = image->getFieldSingleElement< ::fwData::Integer >( ::fwComEd::Dictionary::m_windowMaxId );

    onImageWindowLevelChanged( *min, *max );
}

//------------------------------------------------------------------------------

void WindowLevel::swapping() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->updating();
}
//------------------------------------------------------------------------------

void WindowLevel::updating( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    ::fwComEd::ImageMsg::csptr imageMessage = ::fwComEd::ImageMsg::dynamicConstCast( _msg );

    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();
    ::fwData::Integer::sptr min = image->getFieldSingleElement< ::fwData::Integer >( ::fwComEd::Dictionary::m_windowMinId );
    ::fwData::Integer::sptr max = image->getFieldSingleElement< ::fwData::Integer >( ::fwComEd::Dictionary::m_windowMaxId );
    //m_sliceSelectorMin->setSliderPosition(min->value());
    //m_sliceSelectorMax->setSliderPosition(max->value());
    //m_valueTextMin->setText(QString("%1").arg(min->value()));
    //m_valueTextMax->setText(QString("%1").arg(max->value()));
    onImageWindowLevelChanged(*min, *max);

}

//------------------------------------------------------------------------------

void WindowLevel::info( std::ostream & _sstream )
{
    _sstream << "Window level editor";
}

//------------------------------------------------------------------------------

void WindowLevel::setWidgetWindowMinMax(int _imageMin, int _imageMax)
{
    //double dynMin = m_imageDynamicMin;
    //double dynMax = dynMin + m_imageDynamicWidth;

    //if (_imageMin < dynMin)
    //{
        //dynMin = _imageMin;
    //}
    //if (dynMax < _imageMax)
    //{
        //dynMax = _imageMax;
    //}

    //m_imageDynamicMin   = dynMin;
    //m_imageDynamicWidth = dynMax - dynMin;

    double rangeMin = fromWindowLevel(_imageMin);
    double rangeMax = fromWindowLevel(_imageMax);


    //XXX : Hack because of f4s' TF management
    m_rangeSlider->setMinimumMinMaxDelta(10./m_imageDynamicWidth);

    m_rangeSlider->setPos(rangeMin, rangeMax);
}

//------------------------------------------------------------------------------
double WindowLevel::fromWindowLevel(int _val)
{
    double valMin = m_imageDynamicMin;
    double valMax = valMin + m_imageDynamicWidth;

    double val = _val;
    valMin = std::min(val, valMin);
    valMax = std::max(val, valMax);

    m_imageDynamicMin = valMin;
    m_imageDynamicWidth = valMax - valMin;

    return (_val - m_imageDynamicMin) / m_imageDynamicWidth;
}

//------------------------------------------------------------------------------
int WindowLevel::toWindowLevel(double _val)
{
    return m_imageDynamicMin + m_imageDynamicWidth * _val;
}

//------------------------------------------------------------------------------
void  WindowLevel::updateWindowLevel(double _min, double _max)
{

    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();

    ::fwData::Integer::sptr min = image->getFieldSingleElement< ::fwData::Integer >( ::fwComEd::Dictionary::m_windowMinId );
    ::fwData::Integer::sptr max = image->getFieldSingleElement< ::fwData::Integer >( ::fwComEd::Dictionary::m_windowMaxId );

    double wmin = *min;
    double wmax = *max;

    if(! ::boost::math::isnan(_min) )
    {
        //SLM_ERROR("MIN NOT NAN");
        wmin = toWindowLevel(_min);
        min->value() = wmin;
    }
    if(! ::boost::math::isnan(_max) )
    {
        //SLM_ERROR("MAX NOT NAN");
        wmax = toWindowLevel(_max);
        max->value() = wmax;
    }

    ::fwComEd::fieldHelper::MedicalImageHelpers::updateTFFromMinMax(image);

    setWidgetWindowMinMax( wmin, wmax );
    notifyWindowLevel(wmin, wmax);
}


//------------------------------------------------------------------------------
void  WindowLevel::onWindowLevelWidgetChanged(double _min, double _max)
{
    updateWindowLevel(_min, _max);
    updateWindowLevelText( toWindowLevel(_min), toWindowLevel(_max) );
}

//------------------------------------------------------------------------------
void  WindowLevel::onImageWindowLevelChanged(int _imageMin, int _imageMax)
{
    setWidgetWindowMinMax( _imageMin, _imageMax );
    updateWindowLevelText( _imageMin, _imageMax );
}

//------------------------------------------------------------------------------
void  WindowLevel::notifyWindowLevel(int _imageMin, int _imageMax)
{
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();
    ::fwData::Integer::NewSptr min(static_cast < ::fwData::Integer::ValueType >(_imageMin));
    ::fwData::Integer::NewSptr max(static_cast < ::fwData::Integer::ValueType >(_imageMax));

    ::fwComEd::ImageMsg::NewSptr imageMsg;
    imageMsg->setWindowMinMax(min, max, image);
    ::fwServices::IEditionService::notify(this->getSptr(), image, imageMsg);
 }

//------------------------------------------------------------------------------
void  WindowLevel::updateWindowLevelText(int _imageMin, int _imageMax)
{
    m_valueTextMin->setText(QString("%1").arg(_imageMin));
    m_valueTextMax->setText(QString("%1").arg(_imageMax));
}

//------------------------------------------------------------------------------

void  WindowLevel::onMinTextChanged(QString strVal)
{
    OSLM_ERROR("MIN CHANGED "<< strVal.toStdString());
    int min;
    if(this->onTextChanged(m_valueTextMin, strVal, min))
    {
        OSLM_ERROR("MIN CHANGED "<< min << " " << fromWindowLevel(min)<<" ");
        updateWindowLevel(fromWindowLevel(min), std::numeric_limits<double>::quiet_NaN());
    }
}

//------------------------------------------------------------------------------

void WindowLevel::onMaxTextChanged(QString strVal)
{
    int max;
    if(this->onTextChanged(m_valueTextMax, strVal, max))
    {
        updateWindowLevel(std::numeric_limits<double>::quiet_NaN(), fromWindowLevel(max));
    }
}

//------------------------------------------------------------------------------

bool WindowLevel::onTextChanged(QLineEdit *widget, QString &strVal, int &val)
{
    bool ok=false;
    // due to the validator on QLineEdit xe are sure to have an integer.
    val = strVal.toInt(&ok);

    QPalette palette;
    if (!ok)
    {
        palette.setBrush(QPalette::Base, QBrush(Qt::red));
    }
    else
    {
        palette.setBrush(QPalette::Base, QApplication::palette().brush(QPalette::Base));
    }
    widget->setPalette(palette);
    return ok;
}

//------------------------------------------------------------------------------

void WindowLevel::setEnabled(bool enable)
{
    //m_sliceSelectorMin->setEnabled(enable);
    m_valueTextMin->setEnabled(enable);
    m_rangeSlider->setEnabled(enable);

    //m_sliceSelectorMax->setEnabled(enable);
    m_valueTextMax->setEnabled(enable);
}

//------------------------------------------------------------------------------

}

