/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <wx/wx.h>
#include <wx/window.h>
#include <wx/bmpbuttn.h>
#include <wx/button.h>
#include <wx/event.h>
#include <wx/filedlg.h>

#include <fwCore/base.hpp>

#include <boost/filesystem.hpp>
#include <boost/filesystem/convenience.hpp>

#include <fwTools/Object.hpp>
#include <fwTools/fwID.hpp>

#include <fwData/Image.hpp>
#include <fwData/Integer.hpp>
#include <fwData/String.hpp>
#include <fwData/Boolean.hpp>

#include <fwComEd/ImageMsg.hpp>
#include <fwComEd/Dictionary.hpp>

#include <fwRuntime/ConfigurationElement.hpp>
#include <fwRuntime/operations.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/registry/ObjectService.hpp>
#include <fwServices/IService.hpp>
#include <fwServices/op/Get.hpp>
#include <fwServices/IEditionService.hpp>

#include <fwWX/convert.hpp>

#include <fwGuiWx/container/WxContainer.hpp>

#include "uiImageWx/SliceListEditor.hpp"

namespace uiImage
{

REGISTER_SERVICE( ::gui::editor::IEditor , ::uiImage::SliceListEditor , ::fwData::Image ) ;


SliceListEditor::SliceListEditor() throw()
{
    m_idDropDown = wxNewId();
    m_nbSlice = 1;
    addNewHandledEvent( "SCAN_SHOW" );
}

//------------------------------------------------------------------------------

SliceListEditor::~SliceListEditor() throw()
{}

//------------------------------------------------------------------------------

void SliceListEditor::starting() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->create();

    ::fwGuiWx::container::WxContainer::sptr wxContainer =  ::fwGuiWx::container::WxContainer::dynamicCast( this->getContainer() );
    wxWindow* const container = wxContainer->getWxContainer();
    SLM_ASSERT("container not instanced", container);

    m_dropDownButton = new wxButton( container, m_idDropDown, _T(">"), wxDefaultPosition, wxSize(25,-1) );
    m_dropDownButton->SetToolTip(_("Manage slice visibility"));

    m_pDropDownMenu = new wxMenu();
    m_oneSliceItem = new wxMenuItem(m_pDropDownMenu, wxNewId(), _("One slice"), wxEmptyString, wxITEM_RADIO);
    m_threeSlicesItem = new wxMenuItem(m_pDropDownMenu, wxNewId(), _("three slices"), wxEmptyString, wxITEM_RADIO);
//  m_obliqueSliceItem = new wxMenuItem(m_pDropDownMenu, wxNewId(), _("Oblique slice") , wxEmptyString, wxITEM_RADIO);
    m_pDropDownMenu->Append(m_oneSliceItem);
    m_pDropDownMenu->Append(m_threeSlicesItem);
//    m_pDropDownMenu->Append(m_obliqueSliceItem);

    m_threeSlicesItem->Check(true);
    m_pDropDownMenu->Bind(wxEVT_COMMAND_MENU_SELECTED, &SliceListEditor::onChangeSliceMode, this, m_oneSliceItem->GetId());
    m_pDropDownMenu->Bind(wxEVT_COMMAND_MENU_SELECTED, &SliceListEditor::onChangeSliceMode, this, m_threeSlicesItem->GetId());
//    m_pDropDownMenu->Bind(wxEVT_COMMAND_MENU_SELECTED, &SliceListEditor::onChangeSliceMode, this, m_obliqueSliceItem->GetId());

    wxSizer* sizer = new wxBoxSizer( wxVERTICAL );
    sizer->Add( m_dropDownButton, 1, wxALL|wxEXPAND, 1 );

    m_oneSliceItem->Check(m_nbSlice == 1);
    m_threeSlicesItem->Check(m_nbSlice == 3);
//  m_obliqueSliceItem->Check(m_nbSlice == -1);

    container->Bind( wxEVT_COMMAND_BUTTON_CLICKED, &SliceListEditor::onDropDownButton, this,  m_idDropDown);

    container->SetSizer( sizer );
    container->Layout();
}

//------------------------------------------------------------------------------

void SliceListEditor::stopping() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    ::fwGuiWx::container::WxContainer::sptr wxContainer =  ::fwGuiWx::container::WxContainer::dynamicCast( this->getContainer() );
    wxWindow* const container = wxContainer->getWxContainer();
    SLM_ASSERT("container not instanced", container);

    m_pDropDownMenu->Unbind(wxEVT_COMMAND_MENU_SELECTED, &SliceListEditor::onChangeSliceMode, this, m_oneSliceItem->GetId());
    m_pDropDownMenu->Unbind(wxEVT_COMMAND_MENU_SELECTED, &SliceListEditor::onChangeSliceMode, this, m_threeSlicesItem->GetId());
    container->Unbind( wxEVT_COMMAND_BUTTON_CLICKED, &SliceListEditor::onDropDownButton, this, m_idDropDown);

    wxContainer->clean();
    this->destroy();
}

//------------------------------------------------------------------------------

void SliceListEditor::configuring() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();

    this->initialize();

    std::vector < Configuration > placeInSceneConfig = m_configuration->find("negatoAdaptor");
    SLM_ASSERT("Tag negatoAdaptor required!", !placeInSceneConfig.empty());
    SLM_ASSERT("UID attribute is missing", placeInSceneConfig.at(0)->hasAttribute("uid"));
    m_adaptorUID = placeInSceneConfig.at(0)->getAttributeValue("uid");

    if (placeInSceneConfig.at(0)->hasAttribute("slices"))
    {
        std::string value(placeInSceneConfig.at(0)->getAttributeValue("slices"));
        m_nbSlice = ::boost::lexical_cast<int >(value.c_str());
    }
}

//------------------------------------------------------------------------------

void SliceListEditor::updating() throw(::fwTools::Failed)
{
}

//------------------------------------------------------------------------------

void SliceListEditor::swapping() throw(::fwTools::Failed)
{

}
//------------------------------------------------------------------------------

void SliceListEditor::updating( ::fwServices::ObjectMsg::csptr msg ) throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    ::fwComEd::ImageMsg::csptr imageMsg = ::fwComEd::ImageMsg::dynamicConstCast( msg );
    if( imageMsg && imageMsg->hasEvent( "SCAN_SHOW"))
    {
        ::fwData::Object::csptr dataInfo = imageMsg->getDataInfo("SCAN_SHOW");
        SLM_ASSERT("dataInfo is missing", dataInfo);
        SLM_ASSERT("m_relatedServiceId is missing", dataInfo->getFieldSize( ::fwComEd::Dictionary::m_relatedServiceId ) );
        std::string servId = dataInfo->getFieldSingleElement< ::fwData::String >(::fwComEd::Dictionary::m_relatedServiceId)->value();
        if( servId ==  m_adaptorUID )
        {
            ::fwData::Boolean::csptr isShowScan = ::fwData::Boolean::dynamicConstCast(dataInfo);
            m_dropDownButton->Enable(isShowScan->value());
        }
    }
}

//------------------------------------------------------------------------------

void SliceListEditor::info( std::ostream &_sstream )
{
}

//------------------------------------------------------------------------------

void SliceListEditor::onDropDownButton( wxCommandEvent& event )
{
    SLM_TRACE_FUNC();
    if(::fwTools::fwID::exist(m_adaptorUID ))
    {
        m_dropDownButton->PopupMenu(m_pDropDownMenu, m_dropDownButton->GetSize().GetWidth(), 0);
    }
    else
    {
        OSLM_TRACE("Service "<< m_adaptorUID << " is not yet present.");
    }
}

//------------------------------------------------------------------------------

void SliceListEditor::onChangeSliceMode(  wxCommandEvent& event )
{
    if(::fwTools::fwID::exist(m_adaptorUID ))
    {
        ::fwServices::IService::sptr service = ::fwServices::get(m_adaptorUID);
        ::fwData::Image::sptr image = service->getObject< ::fwData::Image >();
        SLM_ASSERT("SliceListEditor adaptorUID " << m_adaptorUID <<" isn't an Adaptor on an Image?" , image);

        ::fwData::Integer::NewSptr dataInfo;

        if(event.GetId() == m_oneSliceItem->GetId())
        {
            dataInfo->value() = 1;
            m_nbSlice = 1;
        }
        else if(event.GetId() == m_threeSlicesItem->GetId())
        {
            dataInfo->value() = 3;
            m_nbSlice = 3;
        }
        else if(event.GetId() == m_obliqueSliceItem->GetId())
        {
            dataInfo->value() = -1;
            m_nbSlice = -1;
        }
        else
        {
            OSLM_FATAL("Unknown slice mode: "<<event.GetId());
        }
        dataInfo->setFieldSingleElement(::fwComEd::Dictionary::m_relatedServiceId ,  ::fwData::String::NewSptr( m_adaptorUID ) );
        ::fwComEd::ImageMsg::NewSptr imageMsg;
        imageMsg->addEvent( "SLICE_MODE", dataInfo );
        ::fwServices::IEditionService::notify(this->getSptr(), image, imageMsg);
    }
    else
    {
        OSLM_TRACE("Service "<< m_adaptorUID << " is not yet present.");
    }
}

}

