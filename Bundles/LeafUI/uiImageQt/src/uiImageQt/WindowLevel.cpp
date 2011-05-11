/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <QWidget>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QSlider>
#include <QIntValidator>

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

#include "uiImageQt/WindowLevel.hpp"

namespace uiImage
{

REGISTER_SERVICE( ::gui::editor::IEditor , ::uiImage::WindowLevel , ::fwData::Image ) ;

//------------------------------------------------------------------------------

WindowLevel::WindowLevel() throw()
{
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
    m_valueTextMin->setText(QString("%1").arg(min->value()));

    m_sliceSelectorMin = new QSlider( Qt::Horizontal, container );
    m_sliceSelectorMin->setSliderPosition(min->value());
    m_sliceSelectorMin->setMinimumWidth(40);

    m_sliceSelectorMax = new QSlider( Qt::Horizontal, container );
    m_sliceSelectorMax->setSliderPosition(max->value());
    m_sliceSelectorMax->setMinimumWidth(40);

    m_valueTextMax = new QLineEdit( container );
    QIntValidator*  maxValidator = new QIntValidator(m_valueTextMax);
    m_valueTextMax->setValidator(maxValidator);
    m_valueTextMax->setText(QString("%1").arg(max->value()));

    layout->addWidget( m_valueTextMin, 0);
    layout->addWidget( m_sliceSelectorMin );
    layout->addWidget( m_sliceSelectorMax );
    layout->addWidget( m_valueTextMax, 0 );

    QObject::connect(m_sliceSelectorMin, SIGNAL(valueChanged( int )), this, SLOT(onMinChanged( int )));
    QObject::connect(m_sliceSelectorMax, SIGNAL(valueChanged( int )), this, SLOT(onMaxChanged( int )));
    QObject::connect(m_valueTextMin, SIGNAL(textChanged( QString )), this, SLOT(onMinChanged( QString )));
    QObject::connect(m_valueTextMax, SIGNAL(textChanged( QString )), this, SLOT(onMaxChanged( QString )));

    container->setLayout( layout );
    this->updating();
}

//------------------------------------------------------------------------------

void WindowLevel::stopping() throw(::fwTools::Failed)
{

    QObject::disconnect(m_sliceSelectorMin, SIGNAL(valueChanged( int )), this, SLOT(onMinChanged( int )));
    QObject::disconnect(m_sliceSelectorMax, SIGNAL(valueChanged( int )), this, SLOT(onMaxChanged( int )));
    QObject::disconnect(m_valueTextMin, SIGNAL(valueChanged( int )), this, SLOT(onMinChanged( int )));
    QObject::disconnect(m_valueTextMax, SIGNAL(valueChanged( int )), this, SLOT(onMaxChanged( int )));

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

        m_sliceSelectorMin->setMinimum(minValue);
        m_sliceSelectorMin->setMaximum(maxValue);

        m_sliceSelectorMax->setMinimum(minValue);
        m_sliceSelectorMax->setMaximum(maxValue);
    }
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
    m_sliceSelectorMin->setSliderPosition(min->value());
    m_sliceSelectorMax->setSliderPosition(max->value());
    m_valueTextMin->setText(QString("%1").arg(min->value()));
    m_valueTextMax->setText(QString("%1").arg(max->value()));

}

//------------------------------------------------------------------------------

void WindowLevel::info( std::ostream & _sstream )
{
    _sstream << "Window level editor";
}

//------------------------------------------------------------------------------

void  WindowLevel::onMinChanged(int val)
{
    m_valueTextMin->setText(QString("%1").arg(val));

    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();
    ::fwData::Integer::sptr min = image->getFieldSingleElement< ::fwData::Integer >( ::fwComEd::Dictionary::m_windowMinId );
    ::fwData::Integer::sptr max = image->getFieldSingleElement< ::fwData::Integer >( ::fwComEd::Dictionary::m_windowMaxId );
    if( max->value() <= val )
    {
       max ->value() = val + 10;
       m_valueTextMax->setText(QString("%1").arg(max->value()));
       m_sliceSelectorMax->setSliderPosition(max->value());
    }
    min->value() = val;
    ::fwComEd::fieldHelper::MedicalImageHelpers::updateTFFromMinMax(image);

    ::fwData::Integer::NewSptr newMin(val);
    this->sendMsg(newMin,max);
}

//------------------------------------------------------------------------------

void  WindowLevel::onMinChanged(QString strVal)
{
    bool ok=false;
    int val = strVal.toInt(&ok);

    m_sliceSelectorMin->setSliderPosition(val);

    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();
    ::fwData::Integer::sptr min = image->getFieldSingleElement< ::fwData::Integer >( ::fwComEd::Dictionary::m_windowMinId );
    ::fwData::Integer::sptr max = image->getFieldSingleElement< ::fwData::Integer >( ::fwComEd::Dictionary::m_windowMaxId );

    if( max->value() <= val )
    {
        max ->value() = val + 10;
        m_valueTextMax->setText(QString("%1").arg(max->value()));
        m_sliceSelectorMax->setSliderPosition(max->value());
    }
    min->value() = val;
    ::fwComEd::fieldHelper::MedicalImageHelpers::updateTFFromMinMax(image);

    ::fwData::Integer::NewSptr newMin(val);
    this->sendMsg(newMin,max);
}

//------------------------------------------------------------------------------
void WindowLevel::onMaxChanged(int val )
{
    m_valueTextMax->setText(QString("%1").arg(val));

    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();

    ::fwData::Integer::sptr min = image->getFieldSingleElement< ::fwData::Integer >( ::fwComEd::Dictionary::m_windowMinId );
    ::fwData::Integer::sptr max = image->getFieldSingleElement< ::fwData::Integer >( ::fwComEd::Dictionary::m_windowMaxId );
    if( val <= min->value() )
    {
        min ->value() = val - 10;
        m_valueTextMin->setText(QString("%1").arg(min->value()));
        m_sliceSelectorMin->setSliderPosition(min->value());
    }
    max->value() = val;

    ::fwComEd::fieldHelper::MedicalImageHelpers::updateTFFromMinMax(image);

    ::fwData::Integer::NewSptr newMax(val);
    this->sendMsg(min,newMax);
}

//------------------------------------------------------------------------------

void WindowLevel::onMaxChanged(QString strVal )
{
    bool ok=false;
    // due to the validator on QLineEdit xe are sure to have an integer.
    int val = strVal.toInt(&ok);

    m_sliceSelectorMax->setSliderPosition(val);

    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();
    ::fwData::Integer::sptr min = image->getFieldSingleElement< ::fwData::Integer >( ::fwComEd::Dictionary::m_windowMinId );
    ::fwData::Integer::sptr max = image->getFieldSingleElement< ::fwData::Integer >( ::fwComEd::Dictionary::m_windowMaxId );

    if( val <= min->value() )
    {
        min ->value() = val - 10;
        m_valueTextMin->setText(QString("%1").arg(min->value()));
        m_sliceSelectorMin->setSliderPosition(min->value());
    }
    max->value() = val;

    ::fwComEd::fieldHelper::MedicalImageHelpers::updateTFFromMinMax(image);

    ::fwData::Integer::NewSptr newMax(val);
    this->sendMsg(min,newMax);
}

//------------------------------------------------------------------------------

void WindowLevel::sendMsg(::fwData::Integer::sptr min, ::fwData::Integer::sptr max)
{
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();
    ::fwComEd::ImageMsg::NewSptr imageMsg;
    imageMsg->setWindowMinMax(min, max, image);
    ::fwServices::IEditionService::notify(this->getSptr(), image, imageMsg);
}

//------------------------------------------------------------------------------

void WindowLevel::setEnabled(bool enable)
{
    m_sliceSelectorMin->setEnabled(enable);
    m_valueTextMin->setEnabled(enable);

    m_sliceSelectorMax->setEnabled(enable);
    m_valueTextMax->setEnabled(enable);

}

//------------------------------------------------------------------------------

}

