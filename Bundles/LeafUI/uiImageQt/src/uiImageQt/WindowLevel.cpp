/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <QApplication>
#include <QComboBox>
#include <QGridLayout>
#include <QIntValidator>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QSignalMapper>
#include <QToolButton>
#include <QWidget>

#include <boost/math/special_functions/fpclassify.hpp>

#include <fwData/Image.hpp>
#include <fwData/Composite.hpp>

#include <fwComEd/ImageMsg.hpp>

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
    m_widgetDynamicRangeMin   = -1024.;
    m_widgetDynamicRangeWidth =  4000.;

    addNewHandledEvent(::fwComEd::ImageMsg::BUFFER);
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

    QGridLayout* layout = new QGridLayout();

    m_valueTextMin = new QLineEdit( container );
    QIntValidator *minValidator = new QIntValidator(m_valueTextMin);
    m_valueTextMin->setValidator(minValidator);

    m_valueTextMax = new QLineEdit( container );
    QIntValidator*  maxValidator = new QIntValidator(m_valueTextMax);
    m_valueTextMax->setValidator(maxValidator);

    m_rangeSlider = new ::fwGuiQt::widget::QRangeSlider(container);

    m_toggleTFButton = new QToolButton(container);
    QIcon *ico = new QIcon();
    QString squareIcon("Bundles/uiImageQt_" UIIMAGEQT_VER "/square.png");
    QString rampIcon("Bundles/uiImageQt_" UIIMAGEQT_VER "/ramp.png");
    ico->addPixmap(QPixmap(squareIcon), QIcon::Normal,QIcon::On);
    ico->addPixmap(QPixmap(rampIcon), QIcon::Normal,QIcon::Off);
    m_toggleTFButton->setIcon(*ico);
    m_toggleTFButton->setCheckable(true);


    m_dynamicRangeSelection = new QToolButton(container);
    m_dynamicRangeSelection->setPopupMode(QToolButton::InstantPopup);

    m_dynamicRangeMenu = new QMenu(m_dynamicRangeSelection);
    QAction *action1 = m_dynamicRangeMenu->addAction( "-1024; 1023" );
    QAction *action2 = m_dynamicRangeMenu->addAction( "-100; 300" );
    QAction *action3 = m_dynamicRangeMenu->addAction( "Fit W/L" );
    QAction *action4 = m_dynamicRangeMenu->addAction( "Fit Data" ); // TODO
    //QAction *action5 = m_dynamicRangeMenu->addAction( "Custom ..." ); // TODO
    m_dynamicRangeSelection->setMenu(m_dynamicRangeMenu);

    action1->setData(QVariant(1));
    action2->setData(QVariant(2));
    action3->setData(QVariant(3));
    action4->setData(QVariant(4));
    //action5->setData(QVariant(5));


    layout->addWidget( m_rangeSlider,  0, 0, 1, -1 );
    layout->addWidget( m_valueTextMin, 1, 0 );
    layout->addWidget( m_toggleTFButton, 1, 1 );
    layout->addWidget( m_dynamicRangeSelection, 1, 2 );
    layout->addWidget( m_valueTextMax, 1, 3 );


    container->setLayout( layout );
    this->updating();

    m_dynamicRangeSignalMapper = new QSignalMapper(this);

    QObject::connect(m_valueTextMin, SIGNAL(editingFinished()), this, SLOT(onTextEditingFinished()));
    QObject::connect(m_valueTextMax, SIGNAL(editingFinished()), this, SLOT(onTextEditingFinished()));
    QObject::connect(m_rangeSlider, SIGNAL(sliderRangeEdited( double, double )) , this, SLOT(onWindowLevelWidgetChanged( double, double )));
    QObject::connect(m_toggleTFButton, SIGNAL(toggled( bool )), this, SLOT(onToggleTF( bool )));
    QObject::connect(m_dynamicRangeSelection, SIGNAL(triggered( QAction * )), this, SLOT(onDynamicRangeSelectionChanged( QAction * )));

}

//------------------------------------------------------------------------------

void WindowLevel::stopping() throw(::fwTools::Failed)
{

    QObject::disconnect(m_dynamicRangeSelection, SIGNAL(triggered( QAction * )), this, SLOT(onDynamicRangeSelectionChanged( QAction * )));
    QObject::disconnect(m_toggleTFButton, SIGNAL(toggled( bool )), this, SLOT(onToggleTF( bool )));
    QObject::disconnect(m_rangeSlider, SIGNAL(sliderRangeEdited( double, double )), this, SLOT(onWindowLevelWidgetChanged( double, double )));
    QObject::disconnect(m_valueTextMin, SIGNAL(editingFinished( QString )), this, SLOT(onTextEditingFinished( QString )));
    QObject::disconnect(m_valueTextMax, SIGNAL(editingFinished( QString )), this, SLOT(onTextEditingFinished( QString )));


    m_valueTextMin->deleteLater();
    m_valueTextMax->deleteLater();
    m_toggleTFButton->deleteLater();
    m_dynamicRangeSelection->deleteLater();
    m_dynamicRangeSelection->deleteLater();
    m_dynamicRangeMenu->deleteLater();
    m_rangeSlider->deleteLater();
    m_dynamicRangeSignalMapper->deleteLater();

    QPointer< ::fwGuiQt::widget::QRangeSlider > m_rangeSlider;
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
    onImageWindowLevelChanged(*min, *max);

    bool imageIsValid = ::fwComEd::fieldHelper::MedicalImageHelpers::checkImageValidity( image );
    this->setEnabled(imageIsValid);

}

//------------------------------------------------------------------------------

void WindowLevel::info( std::ostream & _sstream )
{
    _sstream << "Window level editor";
}

//------------------------------------------------------------------------------
WindowLevel::WindowLevelMinMaxType WindowLevel::getImageWindowMinMax()
{
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();
    ::fwData::Integer::sptr min = image->getFieldSingleElement< ::fwData::Integer >( ::fwComEd::Dictionary::m_windowMinId );
    ::fwData::Integer::sptr max = image->getFieldSingleElement< ::fwData::Integer >( ::fwComEd::Dictionary::m_windowMaxId );

    WindowLevelMinMaxType res(*min, *max);
    return res;
}


//------------------------------------------------------------------------------

void WindowLevel::updateWidgetMinMax(int _imageMin, int _imageMax)
{
    double rangeMin = fromWindowLevel(_imageMin);
    double rangeMax = fromWindowLevel(_imageMax);


    //XXX : Hack because of f4s' TF management
    m_rangeSlider->setMinimumMinMaxDelta(10./m_widgetDynamicRangeWidth);

    m_rangeSlider->setPos(rangeMin, rangeMax);
}

//------------------------------------------------------------------------------
double WindowLevel::fromWindowLevel(int _val)
{
    double valMin = m_widgetDynamicRangeMin;
    double valMax = valMin + m_widgetDynamicRangeWidth;

    double val = _val;
    valMin = std::min(val, valMin);
    valMax = std::max(val, valMax);

    setWidgetDynamicRange(valMin, valMax);

    double res = (_val - m_widgetDynamicRangeMin) / m_widgetDynamicRangeWidth;
    return res;
}

//------------------------------------------------------------------------------
int WindowLevel::toWindowLevel(double _val)
{
    return m_widgetDynamicRangeMin + m_widgetDynamicRangeWidth * _val;
}

//------------------------------------------------------------------------------
void  WindowLevel::updateImageWindowLevel(int _imageMin, int _imageMax)
{

    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();

    ::fwData::Integer::sptr min = image->getFieldSingleElement< ::fwData::Integer >( ::fwComEd::Dictionary::m_windowMinId );
    ::fwData::Integer::sptr max = image->getFieldSingleElement< ::fwData::Integer >( ::fwComEd::Dictionary::m_windowMaxId );

    min->value() = _imageMin;
    max->value() = _imageMax;

    ::fwComEd::fieldHelper::MedicalImageHelpers::updateTFFromMinMax(image);

    notifyWindowLevel(_imageMin, _imageMax);
}


//------------------------------------------------------------------------------
void  WindowLevel::onWindowLevelWidgetChanged(double _min, double _max)
{
    int imageMin = toWindowLevel(_min);
    int imageMax = toWindowLevel(_max);
    updateImageWindowLevel(imageMin, imageMax);
    updateTextWindowLevel (imageMin, imageMax);
}

//------------------------------------------------------------------------------
void WindowLevel::onDynamicRangeSelectionChanged(QAction *action)
{
    WindowLevelMinMaxType wl    = getImageWindowMinMax();
    int min = static_cast <int> (m_widgetDynamicRangeMin);
    int max = static_cast <int> (m_widgetDynamicRangeWidth) + min;
    int index = action->data().toInt();
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();

    switch (index)
    {
        case 0:
            break;
        case 1: // -1024; 1023
            min = -1024;
            max =  1023;
            break;
        case 2: // -100; 300
            min = -100;
            max =  300;
            break;
        case 3: // Fit Window/Level
            min = wl.first;
            max = wl.second;
            break;
        case 4: // Fit Image Range
            ::fwComEd::fieldHelper::MedicalImageHelpers::getMinMax(image, min, max);
            break;
        case 5: // Custom : TODO
            break;
        default:
            SLM_ASSERT("Unknown range selector index", 0);
    }

    this->setWidgetDynamicRange(min, max);
    this->updateWidgetMinMax(wl.first, wl.second);
}

//------------------------------------------------------------------------------
void  WindowLevel::onImageWindowLevelChanged(int _imageMin, int _imageMax)
{
    updateWidgetMinMax( _imageMin, _imageMax );
    updateTextWindowLevel( _imageMin, _imageMax );
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
void  WindowLevel::updateTextWindowLevel(int _imageMin, int _imageMax)
{

    m_valueTextMin->setText(QString("%1").arg(_imageMin));
    m_valueTextMax->setText(QString("%1").arg(_imageMax));
}

//------------------------------------------------------------------------------
void  WindowLevel::onToggleTF(bool squareTF)
{
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();

    if( squareTF )
    {
        ::fwComEd::fieldHelper::MedicalImageHelpers::setSquareTF(image);
    }
    else
    {
        ::fwComEd::fieldHelper::MedicalImageHelpers::setBWTF(image);
    }

    WindowLevelMinMaxType wl = getImageWindowMinMax();
    this->updateWidgetMinMax(wl.first, wl.second);

    ::fwComEd::fieldHelper::MedicalImageHelpers::updateTFFromMinMax(image);

    ::fwComEd::ImageMsg::NewSptr imageMsg;
    imageMsg->addEvent(::fwComEd::ImageMsg::TRANSFERTFUNCTION);
    ::fwServices::IEditionService::notify(this->getSptr(), image, imageMsg);
}

//------------------------------------------------------------------------------
void  WindowLevel::onTextEditingFinished()
{
    int min, max;
    if(this->getWidgetIntValue(m_valueTextMin, min) && this->getWidgetIntValue(m_valueTextMax, max))
    {
        updateWidgetMinMax( min, max );
        updateImageWindowLevel(min, max);
    }
}

//------------------------------------------------------------------------------
bool WindowLevel::getWidgetIntValue(QLineEdit *widget, int &val)
{
    bool ok=false;
    val = widget->text().toInt(&ok);

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
    m_valueTextMin->setEnabled(enable);
    m_rangeSlider->setEnabled(enable);

    m_valueTextMax->setEnabled(enable);
}

//------------------------------------------------------------------------------
void WindowLevel::setWidgetDynamicRange(double min, double max)
{
    m_widgetDynamicRangeMin = min;
    m_widgetDynamicRangeWidth = max - min;

    m_dynamicRangeSelection->setText(QString("%1, %2 ").arg(min).arg(max));
}



}

