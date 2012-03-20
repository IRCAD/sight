/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <wx/wx.h>
#include <wx/sizer.h>
#include <wx/colour.h>

#include <fwCore/base.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/IEditionService.hpp>
#include <fwComEd/ImageMsg.hpp>

#include <fwData/Object.hpp>
#include <fwData/PatientDB.hpp>
#include <fwData/Patient.hpp>
#include <fwData/Study.hpp>
#include <fwData/Acquisition.hpp>
#include <fwData/Image.hpp>
#include <fwData/Integer.hpp>

#include <fwDataTools/Patient.hpp>

#include <fwComEd/Dictionary.hpp>
#include <fwComEd/PatientDBMsg.hpp>
#include <fwComEd/fieldHelper/BackupHelper.hpp>

#include "fwWX/AcquisitionSelectorPanel.hpp"
#include "fwWX/PatientEditor.hpp"

namespace fwWX
{

//------------------------------------------------------------------------------

AcquisitionSelectorPanel::AcquisitionSelectorPanel(wxWindow * _container, ::boost::shared_ptr< ::fwData::PatientDB > _associatedPatientDB, ::fwServices::IService::wptr _serviceParent ) :
wxPanel( _container, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER|wxTAB_TRAVERSAL ),
m_associatedPatientDB(_associatedPatientDB),
m_iSelectedItemIndex(0),
m_serviceParent(_serviceParent)
{
    m_wxList = new wxListCtrl(this, ID_LIST, wxDefaultPosition, wxDefaultSize, wxLC_REPORT|wxLC_HRULES|wxLC_VRULES);

    int cellWidthT1 = 100;
    int cellWidthT2 = 150;
    int cellWidthT3 = 200;

    m_wxList->InsertColumn(0, _("Name"), wxLIST_FORMAT_LEFT, cellWidthT3);
    m_wxList->InsertColumn(1, _("Modality"), wxLIST_FORMAT_CENTER, cellWidthT1);
    m_wxList->InsertColumn(2, _("Zone"), wxLIST_FORMAT_LEFT, cellWidthT3);
    m_wxList->InsertColumn(3, _("ID"), wxLIST_FORMAT_LEFT, cellWidthT1);
    m_wxList->InsertColumn(4, _("Birthdate"), wxLIST_FORMAT_CENTER, cellWidthT2);
    m_wxList->InsertColumn(5, _("Hospital"), wxLIST_FORMAT_LEFT, cellWidthT2);
    m_wxList->InsertColumn(6, _("Acq. date"), wxLIST_FORMAT_CENTER, cellWidthT2);
    m_wxList->InsertColumn(7, _("Nb images"), wxLIST_FORMAT_CENTER, cellWidthT1);
    m_wxList->InsertColumn(8, _("Voxel size"), wxLIST_FORMAT_CENTER, cellWidthT2);

    wxSizer *   testSizer   ( new wxBoxSizer( wxVERTICAL ) );
    testSizer->Add(m_wxList, 1, wxALL|wxEXPAND);
    this->SetSizer( testSizer );
}

//------------------------------------------------------------------------------

void AcquisitionSelectorPanel::itemSelectionNotification( long itemIndex )
{
    ::fwComEd::fieldHelper::BackupHelper::setSelection(m_associatedPatientDB,
            m_lineToAcq[itemIndex][0], m_lineToAcq[itemIndex][1], m_lineToAcq[itemIndex][2]);

    // notification
    ::fwComEd::PatientDBMsg::NewSptr msg;
    msg->addEvent( ::fwComEd::PatientDBMsg::NEW_IMAGE_SELECTED );
    ::fwServices::IEditionService::notify( m_serviceParent.lock(), m_associatedPatientDB, msg);
}

//------------------------------------------------------------------------------

void AcquisitionSelectorPanel::onItemSelected( wxListEvent & event )
{
    SLM_TRACE_FUNC();

    if ( m_iSelectedItemIndex != event.GetIndex() )
    {
        wxBeginBusyCursor();
        m_iSelectedItemIndex = event.GetIndex();
        itemSelectionNotification( m_iSelectedItemIndex );
        wxEndBusyCursor();
    }
}

//------------------------------------------------------------------------------

void AcquisitionSelectorPanel::onKeyDown( wxListEvent & event )
{
    SLM_TRACE_FUNC();
    if( WXK_BACK    == event.GetKeyCode()   ||
        WXK_DELETE  == event.GetKeyCode()   )
    {
        SLM_ASSERT("No patient in selected PatientDB", !m_associatedPatientDB->getNumberOfPatients() > 0);
        ::fwData::Patient::sptr patient = m_associatedPatientDB->getPatients().at(0);
        SLM_ASSERT("No patient in selected patient", !patient->getNumberOfStudies() > 0);
        ::fwData::Study::sptr study = patient->getStudies().at(0);

        if ( m_associatedPatientDB->getNumberOfPatients() > 1 || patient->getNumberOfStudies() > 1 || study->getNumberOfAcquisitions() > 1 )
        {
            int answer = wxMessageBox( _("Are you sure to remove the patient of the list ?"), _("Patient delete"), wxYES_NO | wxICON_WARNING, this );
            if (answer == wxYES)
            {
                // Save current selection
                std::vector< int > oldSelection (3);
                oldSelection[0] = m_lineToAcq[m_iSelectedItemIndex][0];
                oldSelection[1] = m_lineToAcq[m_iSelectedItemIndex][1];
                oldSelection[2] = m_lineToAcq[m_iSelectedItemIndex][2];

                // Change selection
                m_iSelectedItemIndex = ( m_iSelectedItemIndex == 0 ) ? 1 : 0;
                wxBeginBusyCursor();
                itemSelectionNotification(m_iSelectedItemIndex);
                wxEndBusyCursor();

                // Remove old selected item
                this->eraseItemSelected( oldSelection );

                // Update data
                m_iSelectedItemIndex = 0;
                this->updateData( m_associatedPatientDB, false );
            }
        }
        else
        {
            wxMessageBox( _("Sorry, it is not possible to remove the last patient of the list. This patient is used by the application."), _("Patient delete"), wxOK | wxICON_INFORMATION, this );
        }
    }
}

//------------------------------------------------------------------------------

void AcquisitionSelectorPanel::onItemActivated( wxListEvent & event )
{
    if ( m_iSelectedItemIndex != event.GetIndex() )
    {
        wxBeginBusyCursor();
        m_iSelectedItemIndex = event.GetIndex();
        itemSelectionNotification( m_iSelectedItemIndex );
        wxEndBusyCursor();
    }

    // Patient selection
    ::fwData::Patient::sptr patient = m_associatedPatientDB->getPatients().at(m_lineToAcq[m_iSelectedItemIndex][0]);
    ::fwData::Study::sptr study = patient->getStudies().at(m_lineToAcq[m_iSelectedItemIndex][1]);
    ::fwData::Acquisition::sptr acquisition = study->getAcquisitions().at(m_lineToAcq[m_iSelectedItemIndex][2]);

    wxWindow    * topWindow       ( wxTheApp->GetTopWindow()                      );
    const bool  patientPropChanged( PatientEditor::showModalDialog( topWindow, patient, study, acquisition));
    if( patientPropChanged )
    {
        this->updateData( m_associatedPatientDB, false );
    }
}

//------------------------------------------------------------------------------

void AcquisitionSelectorPanel::updateData( ::fwData::PatientDB::sptr _associatedPatientDB, bool _bResetSelection )
{
    if (_associatedPatientDB)
    {
        SLM_ERROR_IF("m_wxList is NULL", m_wxList==NULL);
        if (m_wxList != NULL)
        {
            m_wxList->DeleteAllItems();
        }

        m_lineToAcq.clear();

        // String value definition
        ::std::string   name        = "?";
        ::std::string   modality    = "?";
        ::std::string   zone        = "?";
        ::std::string   id          = "?";
        ::std::string   birthdate   = "?";
        ::std::string   hospital    = "?";
        ::std::string   acqDate     = "?";
        ::std::string   nbImages    = "?";
        ::std::string   voxelSize   = "?";

        // assign m_associatedPatientDB
        m_associatedPatientDB = _associatedPatientDB;

        BOOST_FOREACH(::fwData::Patient::sptr patient, m_associatedPatientDB->getPatients() )
        {
            int indexP=0;

            name        = patient->getName() + " " + patient->getFirstname();
            birthdate   = boost::posix_time::to_iso_extended_string( patient->getBirthdate() );
            birthdate   =  birthdate.substr(0,10);
            id          = patient->getIDDicom();

            BOOST_FOREACH(::fwData::Study::sptr study, patient->getStudies() )
            {
                int indexS=0;

                hospital    = study->getHospital();
                modality    = study->getModality();
                zone        = study->getAcquisitionZone();

                BOOST_FOREACH(::fwData::Acquisition::sptr acquisition, study->getAcquisitions() )
                {
                    int indexA=0;

                    acqDate = ::boost::posix_time::to_iso_extended_string( acquisition->getCreationDate() );
                    acqDate = acqDate.substr(0,10) + " " + acqDate.substr(11,5);

                    if ( acquisition->getImage()->getNumberOfDimensions() == 3 )
                    {
                        std::stringstream nbImagesStream;
                        nbImagesStream << acquisition->getImage()->getSize()[2];
                        nbImages    = nbImagesStream.str();

                        std::stringstream voxelSizeStream;
                        voxelSizeStream << acquisition->getImage()->getSpacing()[0] <<   " x " << acquisition->getImage()->getSpacing()[1] << " x " << acquisition->getImage()->getSpacing()[2];
                        voxelSize   = voxelSizeStream.str();
                    }

                    int itemCount = m_wxList->GetItemCount();

                    std::vector<int> vTriSelection;
                    vTriSelection.push_back(indexP);
                    vTriSelection.push_back(indexS);
                    vTriSelection.push_back(indexA);

                    m_lineToAcq[itemCount] = vTriSelection;

                    wxCSConv cust(_T("C"));
                    long newItem = m_wxList->InsertItem( itemCount, wxString( name.c_str(), cust ), 0 );
                    m_wxList->SetItem( itemCount, 1, wxString( modality.c_str(), cust ) );
                    m_wxList->SetItem( itemCount, 2, wxString( zone.c_str(), cust ) );
                    m_wxList->SetItem( itemCount, 3, wxString( id.c_str(), cust ) );
                    m_wxList->SetItem( itemCount, 4, wxString( birthdate.c_str(), cust ) );
                    m_wxList->SetItem( itemCount, 5, wxString( hospital.c_str(), cust ) );
                    m_wxList->SetItem( itemCount, 6, wxString( acqDate.c_str(), cust ) );
                    m_wxList->SetItem( itemCount, 7, wxString( nbImages.c_str(), cust ) );
                    m_wxList->SetItem( itemCount, 8, wxString( voxelSize.c_str(), cust ) );


                    OSLM_DEBUG("newItem = " << newItem);
                    OSLM_DEBUG("itemCount = " << itemCount);

                    if ( itemCount == 1 )
                    {
                        wxListItem listItem;
                        listItem.SetId(itemCount);
                        listItem.SetColumn(3);
                        listItem.SetBackgroundColour(wxColour(*wxRED));
                        m_wxList->SetItem(listItem);
                    }

                    if (itemCount%2 == 0)
                    {
                        m_wxList->SetItemBackgroundColour(itemCount, wxColour(*wxLIGHT_GREY));
                    }
                    ++indexA;
                } // end acquisition
                ++indexS;
            } // end study
            ++indexP;
        } // end patient
    }

    // Select the first item in list
    if ( _associatedPatientDB->getNumberOfPatients() > 0 )
    {
        if ( _bResetSelection )
        {
            m_iSelectedItemIndex = 0;
            itemSelectionNotification(m_iSelectedItemIndex);
        }
        m_wxList->SetItemState(m_iSelectedItemIndex, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED); // Normaly run  itemSelectionNotification(0) by the event mecanism
    }
}

//------------------------------------------------------------------------------

void AcquisitionSelectorPanel::eraseItemSelected( const std::vector<int> & _vItemSelected )
{
    // Patient selection
    ::fwData::Patient::sptr patient = m_associatedPatientDB->getPatients().at(_vItemSelected[0]);
    // Study selection
    int nbStudies = patient->getNumberOfStudies();
    ::fwData::Study::sptr study = patient->getStudies().at(_vItemSelected[1]);
    // Acquisition selection
    int nbAcquisitions = study->getNumberOfAcquisitions();
    ::fwData::Acquisition::sptr acquisition = study->getAcquisitions().at(_vItemSelected[2]);

    // Erase acquisition
    ::fwDataTools::Patient::removeAcquisition(study, acquisition);

    if( nbAcquisitions == 1 )
    {
        // Erase study
        ::fwDataTools::Patient::removeStudy(patient, study);

        if ( nbStudies == 1 )
        {
            // Erase patient
            ::fwDataTools::Patient::removePatient(m_associatedPatientDB, patient);
        }
    }

    ::fwComEd::PatientDBMsg::NewSptr msg;
    msg->addEvent(::fwComEd::PatientDBMsg::CLEAR_PATIENT);
    ::fwServices::IEditionService::notify(m_serviceParent.lock(), m_associatedPatientDB, msg);
}

//------------------------------------------------------------------------------

const std::vector<int> AcquisitionSelectorPanel::getIndexOfSelectedItem()
{
    std::vector< int > selection (3);

    selection[0] = m_lineToAcq[m_iSelectedItemIndex][0];
    selection[1] = m_lineToAcq[m_iSelectedItemIndex][1];
    selection[2] = m_lineToAcq[m_iSelectedItemIndex][2];

    return selection;
}

//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE( AcquisitionSelectorPanel, wxPanel )
EVT_LIST_ITEM_SELECTED  ( AcquisitionSelectorPanel::ID_LIST, AcquisitionSelectorPanel::onItemSelected )
EVT_LIST_KEY_DOWN       ( AcquisitionSelectorPanel::ID_LIST, AcquisitionSelectorPanel::onKeyDown )
EVT_LIST_ITEM_ACTIVATED ( AcquisitionSelectorPanel::ID_LIST, AcquisitionSelectorPanel::onItemActivated )
END_EVENT_TABLE()

//------------------------------------------------------------------------------

} //namespace io
