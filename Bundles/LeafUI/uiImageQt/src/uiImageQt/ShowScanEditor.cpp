/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <QString>
#include <QVBoxLayout>
#include <QPushButton>

#include <fwCore/base.hpp>

#include <boost/filesystem.hpp>
#include <boost/filesystem/convenience.hpp>

#include <fwTools/Object.hpp>
#include <fwTools/fwID.hpp>

#include <fwData/Image.hpp>
#include <fwData/Boolean.hpp>
#include <fwData/String.hpp>

#include <fwComEd/ImageMsg.hpp>
#include <fwComEd/Dictionary.hpp>

#include <fwRuntime/ConfigurationElement.hpp>
#include <fwRuntime/operations.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/registry/ObjectService.hpp>
#include <fwServices/IService.hpp>
#include <fwServices/IEditionService.hpp>
#include <fwServices/op/Get.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include "uiImageQt/ShowScanEditor.hpp"

namespace uiImage
{

REGISTER_SERVICE( ::gui::editor::IEditor , ::uiImage::ShowScanEditor , ::fwData::Image ) ;


ShowScanEditor::ShowScanEditor() throw(): m_scanAreShown(true)
{
    handlingEventOff();
}

//------------------------------------------------------------------------------

ShowScanEditor::~ShowScanEditor() throw()
{}

//------------------------------------------------------------------------------

void ShowScanEditor::starting() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->create();

    ::fwGuiQt::container::QtContainer::sptr qtContainer =  ::fwGuiQt::container::QtContainer::dynamicCast( this->getContainer() );
    QWidget* const container = qtContainer->getQtContainer();
    SLM_ASSERT("container not instanced", container);

    ::boost::filesystem::path pathImageScan ("Bundles/uiImageQt_" + std::string(UIIMAGEQT_VER) + "/sliceShow.png");
    OSLM_ASSERT("Image "<< pathImageScan << "is missing", ::boost::filesystem::exists(pathImageScan));
    m_imageShowScan = QIcon(QString::fromStdString(pathImageScan.string()));

    pathImageScan  = "Bundles/uiImageQt_" + std::string(UIIMAGEQT_VER) + "/sliceHide.png";
    OSLM_ASSERT("Image "<< pathImageScan << "is missing", ::boost::filesystem::exists(pathImageScan));
    m_imageHideScan = QIcon(QString::fromStdString(pathImageScan.string()));


    m_showScanButton = new QPushButton( m_imageShowScan, "", container) ;
    m_showScanButton->setToolTip(tr("Show/Hide Scan"));
    m_showScanButton->setIconSize( QSize( 40, 16 ) );

    QVBoxLayout* layout = new QVBoxLayout( container );
    layout->addWidget( m_showScanButton );
    layout->setContentsMargins(0,0,0,0);

    QObject::connect(m_showScanButton, SIGNAL(clicked()), this, SLOT(onChangeScanMode()));

    container->setLayout( layout );
}

//------------------------------------------------------------------------------

void ShowScanEditor::stopping() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    QObject::disconnect(m_showScanButton, SIGNAL(clicked()), this, SLOT(onChangeScanMode()));

    this->getContainer()->clean();
    this->destroy();
}

//------------------------------------------------------------------------------

void ShowScanEditor::configuring() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->initialize();

    std::vector < Configuration > placeInSceneConfig = m_configuration->find("negatoAdaptor");
    SLM_ASSERT("Tag negatoAdaptor required!", !placeInSceneConfig.empty());
    SLM_ASSERT("UID attribute is missing", placeInSceneConfig.at(0)->hasAttribute("uid"));
    m_adaptorUID = placeInSceneConfig.at(0)->getAttributeValue("uid");
}

//------------------------------------------------------------------------------

void ShowScanEditor::updating() throw(::fwTools::Failed)
{
}

//------------------------------------------------------------------------------

void ShowScanEditor::swapping() throw(::fwTools::Failed)
{
}
//------------------------------------------------------------------------------

void ShowScanEditor::updating( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed)
{
}

//------------------------------------------------------------------------------

void ShowScanEditor::info( std::ostream &_sstream )
{
}

//------------------------------------------------------------------------------

void ShowScanEditor::onChangeScanMode()
{
    if(::fwTools::fwID::exist(m_adaptorUID))
    {
        m_scanAreShown = !m_scanAreShown;

        if (!m_scanAreShown)
        {
            m_showScanButton->setIcon(m_imageHideScan);
        }
        else
        {
            m_showScanButton->setIcon(m_imageShowScan);
        }

        ::fwServices::IService::sptr service = ::fwServices::get(m_adaptorUID);
        ::fwData::Image::sptr image = service->getObject< ::fwData::Image >();
        SLM_ASSERT("ShowScanEditor adaptorUID " << m_adaptorUID <<" isn't an Adaptor on an Image?" , image);

        ::fwData::Boolean::NewSptr dataInfo;
        dataInfo->value() = m_scanAreShown;

        dataInfo->setFieldSingleElement(::fwComEd::Dictionary::m_relatedServiceId ,  ::fwData::String::NewSptr( m_adaptorUID ) );
        ::fwComEd::ImageMsg::NewSptr imageMsg;
        imageMsg->addEvent( "SCAN_SHOW", dataInfo );
        ::fwServices::IEditionService::notify(this->getSptr(), image, imageMsg);
    }
    else
    {
        OSLM_TRACE("Service "<< m_adaptorUID << " is not yet present.");
    }
}

}

