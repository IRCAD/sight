#include <wx/window.h>
#include <wx/event.h>
#include <wx/sizer.h>
#include <wx/statbox.h>

#include <fwCore/base.hpp>

#include <fwTools/Object.hpp>
#include <fwTools/fwID.hpp>

#include <fwData/Acquisition.hpp>
#include <fwData/Boolean.hpp>
#include <fwData/String.hpp>

#include <fwComEd/ReconstructionMsg.hpp>
#include <fwComEd/AcquisitionMsg.hpp>

#include <fwRuntime/ConfigurationElement.hpp>
#include <fwRuntime/operations.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/ObjectServiceRegistry.hpp>
#include <fwServices/IService.hpp>
#include <fwServices/op/Get.hpp>
#include <fwServices/IEditionService.hpp>

#include <fwWX/convert.hpp>

#include <fwGuiWx/container/WxContainer.hpp>

#include "uiAcquisitionWx/OrganListEditor.hpp"

namespace uiAcquisition
{

REGISTER_SERVICE( ::gui::editor::IEditor , ::uiAcquisition::OrganListEditor , ::fwData::Acquisition ) ;


OrganListEditor::OrganListEditor() throw()
{
    addNewHandledEvent("ShowReconstructions");
    addNewHandledEvent("SelectReconstruction");
    addNewHandledEvent(::fwComEd::AcquisitionMsg::ADD_RECONSTRUCTION);
}

//------------------------------------------------------------------------------

OrganListEditor::~OrganListEditor() throw()
{}

//------------------------------------------------------------------------------

void OrganListEditor::starting() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->create();
    ::fwGuiWx::container::WxContainer::sptr wxContainer =  ::fwGuiWx::container::WxContainer::dynamicCast( this->getContainer() );
    wxWindow* const container = wxContainer->getWxContainer();
    assert( container ) ;

    m_showCheckBox = new wxCheckBox( container, wxNewId(), _("Hide all organs"));
    m_showCheckBox->SetToolTip(_("Show or hide all organs"));
    m_organChoice = new wxCheckListBox( container, wxNewId(), wxDefaultPosition, wxDefaultSize) ;

#ifdef __MACOSX__
    wxSizer* sizer = new wxBoxSizer( wxVERTICAL );
#else
    wxSizer* sizer = new wxStaticBoxSizer( wxVERTICAL, container, wxT("Organs"));
#endif
    sizer->Add( m_showCheckBox, 0, wxEXPAND|wxALL);
    sizer->Add( m_organChoice, 1, wxEXPAND|wxALL);

    container->SetSizer( sizer );
    container->Layout();

    container->Bind( wxEVT_COMMAND_CHECKBOX_CLICKED, &OrganListEditor::onShowReconstructions, this,  m_showCheckBox->GetId());
    container->Bind( wxEVT_COMMAND_CHECKLISTBOX_TOGGLED, &OrganListEditor::onOrganChoiceVisibility, this,  m_organChoice->GetId());
    container->Bind( wxEVT_COMMAND_LISTBOX_SELECTED, &OrganListEditor::onOrganChoiceSelection, this,  m_organChoice->GetId());
    this->updating();
}

//------------------------------------------------------------------------------

void OrganListEditor::stopping() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    ::fwGuiWx::container::WxContainer::sptr wxContainer =  ::fwGuiWx::container::WxContainer::dynamicCast( this->getContainer() );
    wxWindow* const container = wxContainer->getWxContainer();
    assert( container ) ;
    container->Unbind( wxEVT_COMMAND_CHECKBOX_CLICKED, &OrganListEditor::onShowReconstructions, this, m_showCheckBox->GetId());
    container->Unbind( wxEVT_COMMAND_CHECKLISTBOX_TOGGLED, &OrganListEditor::onOrganChoiceVisibility, this,  m_organChoice->GetId());
    container->Unbind( wxEVT_COMMAND_LISTBOX_SELECTED, &OrganListEditor::onOrganChoiceSelection, this,  m_organChoice->GetId());

    wxContainer->clean();
    this->destroy();
}

//------------------------------------------------------------------------------

void OrganListEditor::configuring() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->initialize();
}

//------------------------------------------------------------------------------

void OrganListEditor::updating() throw(::fwTools::Failed)
{
    this->updateReconstructions();
    this->refreshVisibility();
}

//------------------------------------------------------------------------------

void OrganListEditor::swapping() throw(::fwTools::Failed)
{
    this->updating();
}
//------------------------------------------------------------------------------

void OrganListEditor::updating( ::fwServices::ObjectMsg::csptr msg ) throw(::fwTools::Failed)
{
    ::fwComEd::AcquisitionMsg::csptr acquisitionMsg = ::fwComEd::AcquisitionMsg::dynamicConstCast( msg ) ;
    if ( acquisitionMsg )
    {
        if ( acquisitionMsg->hasEvent("ShowReconstructions") ||
                acquisitionMsg->hasEvent("SelectReconstruction") ||
                acquisitionMsg->hasEvent(::fwComEd::AcquisitionMsg::ADD_RECONSTRUCTION) )
        {
            this->updating();
        }
    }
}

//------------------------------------------------------------------------------

void OrganListEditor::info( std::ostream &_sstream )
{
}

//------------------------------------------------------------------------------

void OrganListEditor::updateReconstructions()
{
    m_organChoice->Clear();
    m_map.clear();
    ::fwGuiWx::container::WxContainer::sptr wxContainer =  ::fwGuiWx::container::WxContainer::dynamicCast( this->getContainer() );
    wxWindow* const container = wxContainer->getWxContainer();
    assert( container ) ;
    ::fwData::Acquisition::sptr acq = this->getObject< ::fwData::Acquisition >();
    container->Enable(acq->getReconstructions().first != acq->getReconstructions().second);

    if(acq->getReconstructions().first != acq->getReconstructions().second)
    {
        ::fwData::Acquisition::ReconstructionIterator iter =  acq->getReconstructions().first;

        for (; iter!=  acq->getReconstructions().second ; ++iter )
        {
            m_map[ (*iter)->getOrganName() ] = (*iter);
        }

        wxArrayString organChoices ;
        for( OrganNameReconstruction::iterator iter = m_map.begin(); iter != m_map.end(); ++iter )
        {
            //Convert std::string to wxString
            organChoices.Add( ::fwWX::std2wx( (*iter).first.data() ) ) ;
        }

        m_organChoice->Append(organChoices);

        bool showAllRec = true;
        if (acq->getFieldSize("ShowReconstructions"))
        {
            showAllRec = acq->getFieldSingleElement< ::fwData::Boolean >("ShowReconstructions")->value();
        }
        m_showCheckBox->SetValue(!showAllRec);
        m_organChoice->Enable(!m_showCheckBox->IsChecked());
    }
}

//------------------------------------------------------------------------------

void OrganListEditor::onOrganChoiceSelection(wxCommandEvent & event )
{
    this->notifyOrganChoiceSelection();
}

//------------------------------------------------------------------------------

void OrganListEditor::notifyOrganChoiceSelection()
{
    std::string organSelected = ::fwWX::wx2std(m_organChoice->GetStringSelection() );
    if(m_map.find(organSelected) != m_map.end())
    {
        ::fwData::Acquisition::sptr acq = this->getObject< ::fwData::Acquisition >();
        ::fwData::Reconstruction::sptr rec = m_map[organSelected] ;

        ::fwComEd::AcquisitionMsg::NewSptr msg;
        msg->addEvent( ::fwComEd::AcquisitionMsg::NEW_RECONSTRUCTION_SELECTED, ::fwData::String::New( rec->getID() ) );
        ::fwServices::IEditionService::notify(this->getSptr(), acq, msg);
    }
}

//------------------------------------------------------------------------------

void OrganListEditor::onOrganChoiceVisibility(wxCommandEvent & event )
{
    int item = event.GetInt();
    std::string organSelected = ::fwWX::wx2std(m_organChoice->GetString( item ) );
    ::fwData::Reconstruction::sptr rec = m_map[organSelected] ;
    assert(rec) ;
    rec->setIsVisible(!rec->getIsVisible());

    ::fwComEd::ReconstructionMsg::NewSptr msg;
    msg->addEvent( ::fwComEd::ReconstructionMsg::VISIBILITY ) ;
    ::fwServices::IEditionService::notify(this->getSptr(), rec, msg);
}

//------------------------------------------------------------------------------

void OrganListEditor::onShowReconstructions(wxCommandEvent & event )
{
    ::fwData::Acquisition::sptr acq = this->getObject< ::fwData::Acquisition >();
    acq->setFieldSingleElement("ShowReconstructions",  ::fwData::Boolean::NewSptr(!m_showCheckBox->IsChecked()) );

    ::fwComEd::AcquisitionMsg::NewSptr msg;
    msg->addEvent( "ShowReconstructions" );
    ::fwServices::IEditionService::notify(this->getSptr(), acq, msg);

    m_organChoice->Enable(!m_showCheckBox->IsChecked());
}

//------------------------------------------------------------------------------

void OrganListEditor::refreshVisibility()
{
    int item=0;
    for( OrganNameReconstruction::iterator iter = m_map.begin(); iter != m_map.end(); ++iter, ++item )
    {
        m_organChoice->Check( item, iter->second->getIsVisible()  );
    }
}

}

