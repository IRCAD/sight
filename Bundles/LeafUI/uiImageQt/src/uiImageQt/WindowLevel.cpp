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
#include <fwData/TransfertFunction.hpp>

#include <fwComEd/ImageMsg.hpp>
#include <fwComEd/TransferFunctionMsg.hpp>
#include <fwComEd/helper/Composite.hpp>

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
    m_autoWindowing = false;
    m_imageMin = -200;
    m_imageMax = 300;
    m_isNotifying = false;

    this->installTFPoolEventHandler(this);
    this->addNewHandledEvent(::fwComEd::ImageMsg::BUFFER);
    this->addNewHandledEvent( ::fwComEd::TransferFunctionMsg::WINDOWING );
}

//------------------------------------------------------------------------------

WindowLevel::~WindowLevel() throw()
{}

//------------------------------------------------------------------------------

void WindowLevel::starting() throw(::fwTools::Failed)
{
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();

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

    m_toggleAutoButton = new QToolButton(container);
    QIcon *icon = new QIcon();
    QString windo("Bundles/uiImageQt_" UIIMAGEQT_VER "/windowing.svg");
    icon->addFile(windo, QSize(), QIcon::Normal,QIcon::On);
    QString nowindo("Bundles/uiImageQt_" UIIMAGEQT_VER "/nowindowing.svg");
    icon->addFile(nowindo, QSize(), QIcon::Normal,QIcon::Off);
    m_toggleAutoButton->setIcon(*icon);
    m_toggleAutoButton->setToolTip("Automatic Windowing");
    m_toggleAutoButton->setCheckable(true);
    m_toggleAutoButton->setChecked(m_autoWindowing);

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
    layout->addWidget( m_toggleAutoButton, 1, 2 );
    layout->addWidget( m_dynamicRangeSelection, 1, 3 );
    layout->addWidget( m_valueTextMax, 1, 4 );


    container->setLayout( layout );
    this->updating();

    m_dynamicRangeSignalMapper = new QSignalMapper(this);

    QObject::connect(m_valueTextMin, SIGNAL(editingFinished()), this, SLOT(onTextEditingFinished()));
    QObject::connect(m_valueTextMax, SIGNAL(editingFinished()), this, SLOT(onTextEditingFinished()));
    QObject::connect(m_rangeSlider, SIGNAL(sliderRangeEdited( double, double )) , this, SLOT(onWindowLevelWidgetChanged( double, double )));
    QObject::connect(m_toggleTFButton, SIGNAL(toggled( bool )), this, SLOT(onToggleTF( bool )));
    QObject::connect(m_toggleAutoButton, SIGNAL(toggled( bool )), this, SLOT(onToggleAutoWL( bool )));
    QObject::connect(m_dynamicRangeSelection, SIGNAL(triggered( QAction * )), this, SLOT(onDynamicRangeSelectionChanged( QAction * )));

    this->installTFObserver( this->getSptr() );
}

//------------------------------------------------------------------------------

void WindowLevel::stopping() throw(::fwTools::Failed)
{
    this->removeTFObserver();
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

    this->getContainer()->clean();
    this->destroy();
}

//------------------------------------------------------------------------------

void WindowLevel::configuring() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->initialize();

    std::vector < ::fwRuntime::ConfigurationElement::sptr > configs = m_configuration->find("config");
    SLM_ASSERT("WindowLevel config is empty.", configs.size() == 1);

    ::fwRuntime::ConfigurationElement::sptr config = configs.front();
    if (config->hasAttribute("autoWindowing"))
    {
        std::string autoWindowing = config->getExistingAttributeValue("autoWindowing");
        SLM_ASSERT("Bad value for 'autoWindowing' attribute. It must be 'yes' or 'no'!", autoWindowing == "yes" || autoWindowing == "no");
        m_autoWindowing = (autoWindowing == "yes");
    }

    this->parseTFConfig(config);
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
        this->updateImageInfos(image);
        if(m_autoWindowing)
        {
            double min, max;
            ::fwComEd::fieldHelper::MedicalImageHelpers::getMinMax(image, min, max);
            this->updateImageWindowLevel(min, max);
        }

        ::fwData::TransfertFunction_VERSION_II::sptr pTF = this->getTransferFunction();
        SLM_ASSERT("TransfertFunction_VERSION_II null pointer", pTF);
        ::fwData::TransfertFunction_VERSION_II::TFValuePairType minMax = pTF->getWLMinMax();
        this->onImageWindowLevelChanged( minMax.first, minMax.second );
    }
}

//------------------------------------------------------------------------------

void WindowLevel::swapping() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->removeTFObserver();
    this->updating();
    this->installTFObserver( this->getSptr() );
}
//------------------------------------------------------------------------------

void WindowLevel::updating( ::fwServices::ObjectMsg::csptr msg ) throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();

    bool imageIsValid = ::fwComEd::fieldHelper::MedicalImageHelpers::checkImageValidity( image );
    if (imageIsValid)
    {
        this->upadteTFObserver(msg);
        this->updateImageInfos(image);
        if(m_autoWindowing && msg->hasEvent( ::fwComEd::ImageMsg::BUFFER ))
        {
            double min, max;
            ::fwComEd::fieldHelper::MedicalImageHelpers::getMinMax(image, min, max);
            this->updateImageWindowLevel(min, max);
        }

        ::fwData::TransfertFunction_VERSION_II::sptr pTF = this->getTransferFunction();
        SLM_ASSERT("TransfertFunction_VERSION_II null pointer", pTF);
        ::fwData::TransfertFunction_VERSION_II::TFValuePairType minMax = pTF->getWLMinMax();
        this->onImageWindowLevelChanged( minMax.first, minMax.second );
    }
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
     ::fwData::TransfertFunction_VERSION_II::sptr pTF = this->getTransferFunction();
     SLM_ASSERT("TransfertFunction_VERSION_II null pointer", pTF);

    return pTF->getWLMinMax();
}

//------------------------------------------------------------------------------
void WindowLevel::updateWidgetMinMax(double _imageMin, double _imageMax)
{
    double rangeMin = this->fromWindowLevel(_imageMin);
    double rangeMax = this->fromWindowLevel(_imageMax);

    //XXX : Hack because of f4s' TF management
    m_rangeSlider->setMinimumMinMaxDelta(10./m_widgetDynamicRangeWidth);

    m_rangeSlider->setPos(rangeMin, rangeMax);
}

//------------------------------------------------------------------------------

double WindowLevel::fromWindowLevel(double _val)
{
    double valMin = m_widgetDynamicRangeMin;
    double valMax = valMin + m_widgetDynamicRangeWidth;

    double val = _val;
    valMin = std::min(val, valMin);
    valMax = std::max(val, valMax);

    this->setWidgetDynamicRange(valMin, valMax);

    double res = (_val - m_widgetDynamicRangeMin) / m_widgetDynamicRangeWidth;
    return res;
}

//------------------------------------------------------------------------------

double WindowLevel::toWindowLevel(double _val)
{
    return m_widgetDynamicRangeMin + m_widgetDynamicRangeWidth * _val;
}

//------------------------------------------------------------------------------

void  WindowLevel::updateImageWindowLevel(double _imageMin, double _imageMax)
{
    m_imageMin = _imageMin;
    m_imageMax = _imageMax;

    if (!m_isNotifying)
    {
        this->notifyWindowLevel(_imageMin, _imageMax);
    }
}

//------------------------------------------------------------------------------

void  WindowLevel::onWindowLevelWidgetChanged(double _min, double _max)
{
    double imageMin = this->toWindowLevel(_min);
    double imageMax = this->toWindowLevel(_max);
    this->updateImageWindowLevel(imageMin, imageMax);
    this->updateTextWindowLevel (imageMin, imageMax);
}

//------------------------------------------------------------------------------

void WindowLevel::onDynamicRangeSelectionChanged(QAction *action)
{
    WindowLevelMinMaxType wl    = getImageWindowMinMax();
    double min = m_widgetDynamicRangeMin;
    double max = m_widgetDynamicRangeWidth + min;
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

void  WindowLevel::onImageWindowLevelChanged(double _imageMin, double _imageMax)
{
    this->updateWidgetMinMax( _imageMin, _imageMax );
    this->updateTextWindowLevel( _imageMin, _imageMax );
}

//------------------------------------------------------------------------------

void  WindowLevel::notifyWindowLevel(double _imageMin, double _imageMax)
{
    m_notifiedImageMin = _imageMin;
    m_notifiedImageMax = _imageMax;

    this->setWindowLevel(m_imageMin, m_imageMax);
    ::fwComEd::TransferFunctionMsg::sptr msg = this->notifyTFWindowing(this->getSptr());
    msg->setMessageCallback(::boost::bind( &WindowLevel::notifyWindowLevelCallback, this ));

    m_isNotifying = true;
}

//------------------------------------------------------------------------------

void  WindowLevel::notifyWindowLevelCallback()
{
    m_isNotifying = false;

    if (m_notifiedImageMin != m_imageMin || m_notifiedImageMax != m_imageMax)
    {
        this->updateImageWindowLevel(m_imageMin, m_imageMax);
    }
}

//------------------------------------------------------------------------------

void  WindowLevel::updateTextWindowLevel(double _imageMin, double _imageMax)
{

    m_valueTextMin->setText(QString("%1").arg(_imageMin));
    m_valueTextMax->setText(QString("%1").arg(_imageMax));
}

//------------------------------------------------------------------------------

void  WindowLevel::onToggleTF(bool squareTF)
{
    ::fwData::TransfertFunction_VERSION_II::sptr oldTF = this->getTransferFunction();
    ::fwData::TransfertFunction_VERSION_II::sptr newTF;

    if( squareTF )
    {
        newTF = ::fwData::TransfertFunction_VERSION_II::New();
        ::fwData::TransfertFunction_VERSION_II::TFColor color(1.,1.,1.,1.);
        newTF->setName("SquareTF");
        newTF->addTFColor(0.0, color);
        newTF->addTFColor(1.0, color);
        newTF->setIsClamped(true);
    }
    else
    {
        newTF = ::fwData::TransfertFunction_VERSION_II::createDefaultTF();
    }

    newTF->setWindow(oldTF->getWindow());
    newTF->setLevel(oldTF->getLevel());

    std::string tfPoolFwID = this->getTFPoolFwID();
    ::fwData::Composite::sptr pool = ::fwData::Composite::dynamicCast( ::fwTools::fwID::getObject( tfPoolFwID ) );
    OSLM_ASSERT( "Sorry, object with fwID " << tfPoolFwID << " doesn't exist.", pool );
    ::fwComEd::helper::Composite compositeHelper( pool );

    compositeHelper.swap(this->getSelectedTFKey(), newTF);
    compositeHelper.notify(this->getSptr());
}

//------------------------------------------------------------------------------

void  WindowLevel::onToggleAutoWL(bool autoWL)
{
     m_autoWindowing = autoWL;

     if (m_autoWindowing)
     {
         ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();
         double min, max;
         ::fwComEd::fieldHelper::MedicalImageHelpers::getMinMax(image, min, max);
         this->updateImageWindowLevel(min, max);
         this->onImageWindowLevelChanged(min, max);
     }
}

//------------------------------------------------------------------------------

void  WindowLevel::onTextEditingFinished()
{
    double min, max;
    if(this->getWidgetDoubleValue(m_valueTextMin, min) && this->getWidgetDoubleValue(m_valueTextMax, max))
    {
        this->updateWidgetMinMax( min, max );
        this->updateImageWindowLevel(min, max);
    }
}

//------------------------------------------------------------------------------

bool WindowLevel::getWidgetDoubleValue(QLineEdit *widget, double &val)
{
    bool ok=false;
    val = widget->text().toDouble(&ok);

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
    ::fwGuiQt::container::QtContainer::sptr qtContainer =  ::fwGuiQt::container::QtContainer::dynamicCast( this->getContainer() );
    QWidget * const container = qtContainer->getQtContainer();
    SLM_ASSERT("container not instanced", container);
    container->setEnabled(enable);
}

//------------------------------------------------------------------------------

void WindowLevel::setWidgetDynamicRange(double min, double max)
{
    m_widgetDynamicRangeMin = min;
    m_widgetDynamicRangeWidth = max - min;

    m_dynamicRangeSelection->setText(QString("%1, %2 ").arg(min).arg(max));
}

//------------------------------------------------------------------------------

}

