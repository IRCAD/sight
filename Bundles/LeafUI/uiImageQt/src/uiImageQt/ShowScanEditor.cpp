///* ***** BEGIN LICENSE BLOCK *****
// * FW4SPL - Copyright (C) IRCAD, 2009-2010.
// * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
// * published by the Free Software Foundation.
// * ****** END LICENSE BLOCK ****** */
//
//#include <wx/wx.h>
//#include <wx/window.h>
//#include <wx/bmpbuttn.h>
//#include <wx/button.h>
//#include <wx/event.h>
//#include <wx/filedlg.h>
//
//#include <fwCore/base.hpp>
//
//#include <boost/filesystem.hpp>
//#include <boost/filesystem/convenience.hpp>
//
//#include <fwTools/Object.hpp>
//#include <fwTools/UUID.hpp>
//
//#include <fwData/Image.hpp>
//#include <fwData/Boolean.hpp>
//#include <fwData/String.hpp>
//
//#include <fwComEd/ImageMsg.hpp>
//#include <fwComEd/Dictionary.hpp>
//
//#include <fwRuntime/ConfigurationElement.hpp>
//#include <fwRuntime/operations.hpp>
//
//#include <fwServices/helper.hpp>
//#include <fwServices/macros.hpp>
//#include <fwServices/ObjectServiceRegistry.hpp>
//#include <fwServices/IService.hpp>
//#include <fwServices/op/Get.hpp>
//
//#include <fwWX/convert.hpp>
//
//#include <fwGuiQt/container/Container.hpp>
//
//#include "uiImageQt/ShowScanEditor.hpp"
//
//namespace uiImage
//{
//
//REGISTER_SERVICE( ::gui::editor::IEditor , ::uiImage::ShowScanEditor , ::fwData::Image ) ;
//
//
//ShowScanEditor::ShowScanEditor() throw(): m_scanAreShown(true)
//{
//    handlingEventOff();
//}
//
////------------------------------------------------------------------------------
//
//ShowScanEditor::~ShowScanEditor() throw()
//{}
//
////------------------------------------------------------------------------------
//
//void ShowScanEditor::starting() throw(::fwTools::Failed)
//{
//    SLM_TRACE_FUNC();
//    this->create();
//
//    ::fwGuiWx::container::WxContainer::sptr wxContainer =  ::fwGuiWx::container::WxContainer::dynamicCast( this->getContainer() );
//    wxWindow* const container = wxContainer->getWxContainer();
//    assert( container ) ;
//
//    namespace fs = ::boost::filesystem;
//    fs::path pathImageScan ("Bundles/uiImageQt_" + std::string(UIIMAGEQT_VER) + "/sliceShow.png");
//    OSLM_ASSERT("Image "<< pathImageScan << "is missing", fs::exists(pathImageScan));
//    m_imageShowScan.LoadFile(::fwWX::std2wx(pathImageScan.string()));
//
//    pathImageScan  = "Bundles/uiImageQt_" + std::string(UIIMAGEQT_VER) + "/sliceHide.png";
//    OSLM_ASSERT("Image "<< pathImageScan << "is missing", fs::exists(pathImageScan));
//    m_imageHideScan.LoadFile(::fwWX::std2wx(pathImageScan.string()));
//
//
//    m_showScanButton = new wxBitmapButton( container, wxNewId(), m_imageShowScan, wxDefaultPosition, wxSize(40,-1) ) ;
//    m_showScanButton->SetToolTip(_("Show/Hide Scan"));
//
//    wxSizer* sizer = new wxBoxSizer( wxVERTICAL );
//    sizer->Add( m_showScanButton, 1, wxALL|wxEXPAND, 1 );
//    container->Bind( wxEVT_COMMAND_BUTTON_CLICKED, &ShowScanEditor::onChangeScanMode, this,  m_showScanButton->GetId());
//
//    container->SetSizer( sizer );
//    container->Layout();
//}
//
////------------------------------------------------------------------------------
//
//void ShowScanEditor::stopping() throw(::fwTools::Failed)
//{
//    SLM_TRACE_FUNC();
//    ::fwGuiWx::container::WxContainer::sptr wxContainer =  ::fwGuiWx::container::WxContainer::dynamicCast( this->getContainer() );
//    wxWindow* const container = wxContainer->getWxContainer();
//    assert( container ) ;
//
//    container->Unbind( wxEVT_COMMAND_BUTTON_CLICKED, &ShowScanEditor::onChangeScanMode, this, m_showScanButton->GetId());
//
//    wxContainer->clean();
//    this->destroy();
//}
//
////------------------------------------------------------------------------------
//
//void ShowScanEditor::configuring() throw(fwTools::Failed)
//{
//    SLM_TRACE_FUNC();
//    this->initialize();
//
//    std::vector < Configuration > placeInSceneConfig = m_configuration->find("negatoAdaptor");
//    SLM_ASSERT("Tag negatoAdaptor required!", !placeInSceneConfig.empty());
//    SLM_ASSERT("UID attribute is missing", placeInSceneConfig.at(0)->hasAttribute("uid"));
//    m_adaptorUID = placeInSceneConfig.at(0)->getAttributeValue("uid");
//}
//
////------------------------------------------------------------------------------
//
//void ShowScanEditor::updating() throw(::fwTools::Failed)
//{
//}
//
////------------------------------------------------------------------------------
//
//void ShowScanEditor::swapping() throw(::fwTools::Failed)
//{
//}
////------------------------------------------------------------------------------
//
//void ShowScanEditor::updating( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed)
//{
//}
//
////------------------------------------------------------------------------------
//
//void ShowScanEditor::info( std::ostream &_sstream )
//{
//}
//
////------------------------------------------------------------------------------
//
//void ShowScanEditor::onChangeScanMode(  wxCommandEvent& event )
//{
//    if(::fwTools::UUID::exist(m_adaptorUID, ::fwTools::UUID::SIMPLE ))
//    {
//        m_scanAreShown = !m_scanAreShown;
//
//        if (!m_scanAreShown)
//        {
//            m_showScanButton->SetBitmapLabel(m_imageHideScan);
//        }
//        else
//        {
//            m_showScanButton->SetBitmapLabel(m_imageShowScan);
//        }
//
//        ::fwServices::IService::sptr service = ::fwServices::get(m_adaptorUID);
//        ::fwData::Image::sptr image = service->getObject< ::fwData::Image >();
//        SLM_ASSERT("ShowScanEditor adaptorUID " << m_adaptorUID <<" isn't an Adaptor on an Image?" , image);
//
//        ::fwData::Boolean::NewSptr dataInfo;
//        dataInfo->value() = m_scanAreShown;
//
//        dataInfo->setFieldSingleElement(::fwComEd::Dictionary::m_relatedServiceId ,  ::fwData::String::NewSptr( m_adaptorUID ) );
//        ::fwComEd::ImageMsg::NewSptr imageMsg;
//        imageMsg->addEvent( "SCAN_SHOW", dataInfo );
//        ::fwServices::IEditionService::notify(this->getSptr(), image, imageMsg);
//    }
//    else
//    {
//        OSLM_TRACE("Service "<< m_adaptorUID << " is not yet present.");
//    }
//}
//
//}
//
//