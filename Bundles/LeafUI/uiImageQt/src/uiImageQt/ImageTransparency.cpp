/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>

#include <QHBoxLayout>
#include <QLabel>
#include <QWidget>

#include <fwCore/base.hpp>

#include <fwTools/Object.hpp>

#include <fwData/Image.hpp>
#include <fwData/Boolean.hpp>

#include <fwMath/IntrasecTypes.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/IEditionService.hpp>
#include <fwServices/IService.hpp>
#include <fwServices/registry/ObjectService.hpp>

#include <fwComEd/ImageMsg.hpp>
#include <fwComEd/fieldHelper/MedicalImageHelpers.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include "uiImageQt/ImageTransparency.hpp"

namespace uiImage
{

REGISTER_SERVICE( ::gui::editor::IEditor , ::uiImage::ImageTransparency , ::fwData::Image ) ;


ImageTransparency::ImageTransparency() throw()
{
    addNewHandledEvent(::fwComEd::ImageMsg::TRANSPARENCY);
    addNewHandledEvent(::fwComEd::ImageMsg::VISIBILITY);
    addNewHandledEvent(::fwComEd::ImageMsg::BUFFER);
}

//------------------------------------------------------------------------------

ImageTransparency::~ImageTransparency() throw()
{}

//------------------------------------------------------------------------------

void ImageTransparency::starting() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->::fwGui::IGuiContainerSrv::create();
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();

    ::fwGuiQt::container::QtContainer::sptr qtContainer =  ::fwGuiQt::container::QtContainer::dynamicCast( this->getContainer() );
    QWidget* const container = qtContainer->getQtContainer();
    SLM_ASSERT("container not instanced", container);

    QHBoxLayout* hLayout = new QHBoxLayout();

    QLabel* staticText = new QLabel( tr("Transparency: "), container);
    hLayout->addWidget( staticText, 0, Qt::AlignVCenter );

    m_valueSlider = new QSlider( Qt::Horizontal, container );
    hLayout->addWidget( m_valueSlider, 1, Qt::AlignVCenter );
    m_valueSlider->setRange(0, 100);
    m_valueSlider->setMinimumWidth(100);

    m_valueCheckBox = new QCheckBox( tr("visible"), container);
    if (!m_shortcut.empty())
    {
        m_valueCheckBox->setShortcut(QKeySequence(QString::fromStdString(m_shortcut)));
    }
    hLayout->addWidget( m_valueCheckBox, 0, Qt::AlignVCenter );

    container->setLayout( hLayout );

    QObject::connect(m_valueSlider, SIGNAL(valueChanged(int)), this, SLOT(onModifyTransparency(int)));
    QObject::connect(m_valueCheckBox, SIGNAL(stateChanged(int)), this, SLOT(onModifyVisibility(int)));

    this->updating();
}

//------------------------------------------------------------------------------

void ImageTransparency::stopping() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    QObject::disconnect(m_valueSlider, SIGNAL(valueChanged(int)), this, SLOT(onModifyTransparency(int)));
    QObject::disconnect(m_valueCheckBox, SIGNAL(stateChanged(int)), this, SLOT(onModifyVisibility(int)));

    this->getContainer()->clean();
    this->::fwGui::IGuiContainerSrv::destroy();
}

//------------------------------------------------------------------------------

void ImageTransparency::configuring() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->::fwGui::IGuiContainerSrv::initialize();

    //<shortcut value="X"/>
    std::vector < ConfigurationType > vectCfg = m_configuration->find("shortcut");
    if(!vectCfg.empty())
    {
        ConfigurationType config = vectCfg.at(0);
        SLM_ASSERT("Missing attribute value", config->hasAttribute("value"));
        m_shortcut = config->getAttributeValue("value");
    }
}

//------------------------------------------------------------------------------

void ImageTransparency::updating() throw(::fwTools::Failed)
{
    ::fwData::Image::sptr img = this->getObject< ::fwData::Image >();

    bool imageIsValid = ::fwComEd::fieldHelper::MedicalImageHelpers::checkImageValidity( img );
    m_valueSlider->setEnabled(imageIsValid);
    m_valueCheckBox->setEnabled(imageIsValid);
    if (imageIsValid)
    {
        QObject::disconnect(m_valueSlider, SIGNAL(valueChanged(int)), this, SLOT(onModifyTransparency(int)));
        QObject::disconnect(m_valueCheckBox, SIGNAL(stateChanged(int)), this, SLOT(onModifyVisibility(int)));


        if(img->getField( "TRANSPARENCY" ) )
        {
            ::fwData::Integer::sptr transparency = img->getField< ::fwData::Integer >( "TRANSPARENCY" );
            m_valueSlider->setValue( *transparency );
        }
        else
        {
            img->setField( "TRANSPARENCY", ::fwData::Integer::New(0) );
            m_valueSlider->setValue( 0 );
        }
        if(img->getField( "VISIBILITY" ) )
        {
            ::fwData::Boolean::sptr visible = img->getField< ::fwData::Boolean >( "VISIBILITY" );
            m_valueCheckBox->setChecked( *visible );
        }
        else
        {
            img->setField( "VISIBILITY", ::fwData::Boolean::New(true) );
            m_valueCheckBox->setChecked( true );
        }
        QObject::connect(m_valueSlider, SIGNAL(valueChanged(int)), this, SLOT(onModifyTransparency(int)));
        QObject::connect(m_valueCheckBox, SIGNAL(stateChanged(int)), this, SLOT(onModifyVisibility(int)));
    }
}

//------------------------------------------------------------------------------

void ImageTransparency::swapping() throw(::fwTools::Failed)
{
    this->updating();
}

//------------------------------------------------------------------------------

void ImageTransparency::updating( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    ::fwComEd::ImageMsg::csptr imageMsg = ::fwComEd::ImageMsg::dynamicConstCast(_msg);
    if(imageMsg)
    {
        if ( imageMsg->hasEvent( ::fwComEd::ImageMsg::TRANSPARENCY ) ||
             imageMsg->hasEvent( ::fwComEd::ImageMsg::VISIBILITY )   ||
             imageMsg->hasEvent( ::fwComEd::ImageMsg::BUFFER ) )
        {
            this->updating();
        }
    }
}

//------------------------------------------------------------------------------

void ImageTransparency::info( std::ostream &_sstream )
{
    _sstream << "Image Features Editor";
}

//------------------------------------------------------------------------------

void ImageTransparency::onModifyTransparency(int value)
{
    SLM_TRACE_FUNC();
    ::fwData::Image::sptr img = this->getObject< ::fwData::Image >();
    img->setField( "TRANSPARENCY",  ::fwData::Integer::New(value) );
    ::fwComEd::ImageMsg::NewSptr imageMsg;
    imageMsg->addEvent( "TRANSPARENCY" );
    ::fwServices::IEditionService::notify(this->getSptr(), img, imageMsg);
}

//------------------------------------------------------------------------------

void ImageTransparency::onModifyVisibility(int value)
{
    SLM_TRACE_FUNC();
    ::fwData::Image::sptr img = this->getObject< ::fwData::Image >();
    img->setField( "VISIBILITY",  ::fwData::Boolean::New(value != Qt::Unchecked) );
    ::fwComEd::ImageMsg::NewSptr imageMsg;
    imageMsg->addEvent( "VISIBILITY" );
    ::fwServices::IEditionService::notify(this->getSptr(), img, imageMsg);
}

//------------------------------------------------------------------------------

}

