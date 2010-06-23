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
#include <fwData/String.hpp>
#include <fwData/Composite.hpp>

#include <fwComEd/CompositeMsg.hpp>

#include <fwRuntime/ConfigurationElement.hpp>
#include <fwRuntime/operations.hpp>

#include <fwServices/helper.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/ObjectServiceRegistry.hpp>
#include <fwServices/IService.hpp>
#include <fwServices/op/Get.hpp>

#include <fwWX/convert.hpp>

#include "uiVisu/SnapshotEditor.hpp"

namespace uiVisu
{

REGISTER_SERVICE( ::gui::editor::IEditor , ::uiVisu::SnapshotEditor , ::fwTools::Object ) ;


SnapshotEditor::SnapshotEditor() throw()
{
    m_idSnapButton = wxNewId();
}

//------------------------------------------------------------------------------

SnapshotEditor::~SnapshotEditor() throw()
{}

//------------------------------------------------------------------------------

void SnapshotEditor::starting() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->initGuiParentContainer();

    namespace fs = ::boost::filesystem;
    fs::path pathImageSnap ("Bundles/uiVisu_" + std::string(UIVISU_VER) + "/camera-photo.png");
    OSLM_ASSERT("Image "<< pathImageSnap << "is missing", fs::exists(pathImageSnap));
    wxString filenameSnap ( ::fwWX::std2wx(pathImageSnap.string() ) );
    wxImage imageSnap;
    imageSnap.LoadFile(filenameSnap);
    wxBitmapButton* snapButton = new wxBitmapButton( m_container, m_idSnapButton, imageSnap, wxDefaultPosition, wxSize(25,-1) ) ;
    snapButton->SetToolTip(_("Snapshot"));

    wxSizer* sizer = new wxBoxSizer( wxVERTICAL );
    m_container->SetSizer( sizer );
    sizer->Fit( m_container );
    sizer->Add( snapButton, 1, wxALL|wxEXPAND, 1 );
    m_container->Bind( wxEVT_COMMAND_BUTTON_CLICKED, &SnapshotEditor::onSnapButton, this,  m_idSnapButton);
}

//------------------------------------------------------------------------------

void SnapshotEditor::stopping() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    m_container->Unbind( wxEVT_COMMAND_BUTTON_CLICKED, &SnapshotEditor::onSnapButton, this, m_idSnapButton);
    this->resetGuiParentContainer();

}

//------------------------------------------------------------------------------

void SnapshotEditor::configuring() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();

    std::vector < Configuration > snapConfig = m_configuration->find("snap");
    if(!snapConfig.empty())
    {
        std::vector < Configuration > sceneConfig = snapConfig.at(0)->find("scene");
        for(unsigned int i =0; i < sceneConfig.size(); i++)
        {
            OSLM_ASSERT("Wrong tag name: " << sceneConfig.at(i)->getName(), sceneConfig.at(i)->getName() == "scene");
            SLM_ASSERT("UID attribute is missing", sceneConfig.at(i)->hasAttribute("uid"));

            std::string value(sceneConfig.at(i)->getAttributeValue("uid"));
            m_scenesUID.push_back(value);
        }
    }
}

//------------------------------------------------------------------------------

void SnapshotEditor::updating() throw(::fwTools::Failed)
{
}

//------------------------------------------------------------------------------

void SnapshotEditor::swapping() throw(::fwTools::Failed)
{

}
//------------------------------------------------------------------------------

void SnapshotEditor::updating( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed)
{
}

//------------------------------------------------------------------------------

void SnapshotEditor::info( std::ostream &_sstream )
{
}

//------------------------------------------------------------------------------

void SnapshotEditor::onSnapButton( wxCommandEvent& event )
{
    SLM_TRACE_FUNC();
    if( m_container->IsShownOnScreen() )
    {
        for(unsigned int i=0; i < m_scenesUID.size(); i++)
        {
            ::fwServices::IService::sptr service = ::fwServices::get(m_scenesUID.at(i));
            ::fwData::Composite::sptr composite = service->getObject< ::fwData::Composite >();
            SLM_ASSERT("SnapshotEditor sceneUID " << m_scenesUID.at(i) <<" isn't a GenericScene?" , composite);

            ::fwData::Object::NewSptr dataInfo;

            ::fwData::String::NewSptr sceneID;
            sceneID->value() = m_scenesUID.at(i);
            ::fwData::String::NewSptr filename;

            filename->value() = this->requestFileName();
            if(!filename->value().empty())
            {
                dataInfo->setFieldSingleElement("sceneID", sceneID);
                dataInfo->setFieldSingleElement("filename", filename);
                ::fwComEd::CompositeMsg::NewSptr compositeMsg;
                compositeMsg->addEvent( "SNAP", dataInfo );
                ::fwServices::IEditionService::notify(this->getSptr(), composite, compositeMsg);
            }
        }
    }
    else
    {
        wxMessageBox(
                _("Sorry, it is not possible to snapshot the negato view. This view is not shown on screen."),
                _("Negato view snapshot"),
                wxOK | wxICON_WARNING,
                m_container );
    }
}

//------------------------------------------------------------------------------

std::string SnapshotEditor::requestFileName()
{
    std::string fileName = "";

    wxFileDialog* myFileDialog = new wxFileDialog(wxTheApp->GetTopWindow(),
            wxT("Save snapshot as"),
            wxT(""), wxT(""),
            wxT("Image file (*.jpg;*.jpeg;*.bmp;*.png;*.tiff)|*.jpg;*.jpeg;*.bmp;*.png;*.tiff|")
            wxT("jpeg (*.jpg;*.jpeg)|*.jpg;*.jpeg|")
            wxT("bmp (*.bmp)|*.bmp|")
            wxT("png (*.png)|*.png|")
            wxT("tiff (*.tiff)|*.tiff|")
            wxT("all (*.*)|*.*"),
            wxFD_SAVE|wxFD_CHANGE_DIR|wxFD_OVERWRITE_PROMPT, wxDefaultPosition);
    myFileDialog->Centre(wxBOTH);
    int OK = myFileDialog->ShowModal();

    if( OK==wxID_OK )
    {
        fileName = ::fwWX::wx2std(myFileDialog->GetPath());
    }
    myFileDialog->Destroy();

    return fileName;
}
//------------------------------------------------------------------------------

}

