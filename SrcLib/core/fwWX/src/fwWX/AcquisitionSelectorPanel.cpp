/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <wx/wx.h>
#include <wx/sizer.h>
#include <wx/colour.h>
//#include <wx/list.h>

#include <fwCore/base.hpp>

#include <fwServices/helper.hpp>
#include <fwServices/IEditionService.hpp>
#include <fwComEd/ImageMsg.hpp>

#include <fwData/Object.hpp>
#include <fwData/PatientDB.hpp>
#include <fwData/Patient.hpp>
#include <fwData/Study.hpp>
#include <fwData/Acquisition.hpp>
#include <fwData/Image.hpp>
#include <fwData/Integer.hpp>

#include <fwComEd/Dictionary.hpp>
#include <fwComEd/PatientDBMsg.hpp>

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

        wxSizer *       testSizer       ( new wxBoxSizer( wxVERTICAL ) );
        testSizer->Add(m_wxList, 1, wxALL|wxEXPAND);
        this->SetSizer( testSizer );
}

//------------------------------------------------------------------------------

void AcquisitionSelectorPanel::itemSelectionNotification( long itemIndex )
{
        SLM_TRACE("itemSelectionNotification")

        ::boost::shared_ptr< ::fwData::Object > acqSelected ( new ::fwData::Object() );
        acqSelected->children().clear();
        acqSelected->children().push_back( ::boost::shared_ptr< ::fwData::Integer > ( new ::fwData::Integer(m_lineToAcq[itemIndex][0]) ) );
        acqSelected->children().push_back( ::boost::shared_ptr< ::fwData::Integer > ( new ::fwData::Integer(m_lineToAcq[itemIndex][1]) ) );
        acqSelected->children().push_back( ::boost::shared_ptr< ::fwData::Integer > ( new ::fwData::Integer(m_lineToAcq[itemIndex][2]) ) );


        //::boost::shared_ptr< ::fwComEd::PatientDBMsg > msg ( new ::fwComEd::PatientDBMsg( m_associatedPatientDB ) ) ;
        //msg->addMessageInformation( ::fwComEd::PatientDBMsg::NEW_IMAGE_SELECTED, acqSelected );
        ::fwComEd::PatientDBMsg::NewSptr msg;
        msg->addEvent( ::fwComEd::PatientDBMsg::NEW_IMAGE_SELECTED, acqSelected );

//      ::boost::shared_ptr< ::fwServices::IEditionService > basicEditor = ::fwServices::get< ::fwServices::IEditionService >( m_associatedPatientDB ) ;
//      basicEditor->notify( msg );
        ::fwServices::IEditionService::notify( m_serviceParent.lock(), m_associatedPatientDB, msg);
}

//------------------------------------------------------------------------------

void AcquisitionSelectorPanel::onItemSelected( wxListEvent & event )
{
        SLM_TRACE("onItemSelected");

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
        SLM_TRACE("AcquisitionSelectorPanel::onKeyDown");
        if( WXK_BACK    == event.GetKeyCode()   ||
                WXK_DELETE      == event.GetKeyCode()   )
        {

                ::fwData::PatientDB::PatientIterator patient = m_associatedPatientDB->getPatients().first;
                ::fwData::Patient::StudyIterator study = (*patient)->getStudies().first;

                if ( m_associatedPatientDB->getPatientSize() > 1 || (*patient)->getStudySize() > 1 || (*study)->getAcquisitionSize() > 1 )
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
                                eraseItemSelected( oldSelection );

                                // Update data
                                m_iSelectedItemIndex = 0;
                                updateData( m_associatedPatientDB, false );
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
        SLM_TRACE("AcquisitionSelectorPanel::onItemActivated");

        if ( m_iSelectedItemIndex != event.GetIndex() )
        {
                wxBeginBusyCursor();
                m_iSelectedItemIndex = event.GetIndex();
                itemSelectionNotification( m_iSelectedItemIndex );
                wxEndBusyCursor();
        }

        // Patient selection
        ::fwData::PatientDB::PatientIterator patientIter = m_associatedPatientDB->getPatients().first;
        patientIter += m_lineToAcq[m_iSelectedItemIndex][0];

        // Study selection
        ::fwData::Patient::StudyIterator studyIter = (*patientIter)->getStudies().first;
        studyIter += m_lineToAcq[m_iSelectedItemIndex][1];

        // Acquisition selection
        ::fwData::Study::AcquisitionIterator acquisitionIter = (*studyIter)->getAcquisitions().first;
        acquisitionIter += m_lineToAcq[m_iSelectedItemIndex][2];

        wxWindow        * topWindow                     ( wxTheApp->GetTopWindow()                                              );
        const bool      patientPropChanged      ( PatientEditor::showModalDialog( topWindow, *patientIter, *studyIter, *acquisitionIter)        );
        if( patientPropChanged )
        {
                updateData( m_associatedPatientDB, false );
        }
}

//------------------------------------------------------------------------------

void AcquisitionSelectorPanel::updateData( ::boost::shared_ptr< ::fwData::PatientDB > _associatedPatientDB, bool _bResetSelection )
{
        SLM_TRACE("updateData()")
        if (            _associatedPatientDB)
        {
                if (m_wxList != NULL)
                {
                        m_wxList->DeleteAllItems();
                }
                else
                {
                        SLM_ERROR("m_wxList is NULL");
                }

                m_lineToAcq.clear();


                // String value definition
                ::std::string   name            = "?";
                ::std::string   modality        = "?";
                ::std::string   zone            = "?";
                ::std::string   id                      = "?";
                ::std::string   birthdate       = "?";
                ::std::string   hospital        = "?";
                ::std::string   acqDate         = "?";
                ::std::string   nbImages        = "?";
                ::std::string   voxelSize       = "?";

                // assign m_associatedPatientDB
                m_associatedPatientDB = _associatedPatientDB;

                ::fwData::PatientDB::PatientIterator patientBegin = m_associatedPatientDB->getPatients().first;
                ::fwData::PatientDB::PatientIterator patientEnd         = m_associatedPatientDB->getPatients().second;
                ::fwData::PatientDB::PatientIterator patient            = patientBegin;

                while ( patient != patientEnd )
                {

                        name            = (*patient)->getName() + " " + (*patient)->getFirstname();
                        birthdate       = boost::posix_time::to_iso_extended_string( (*patient)->getBirthdate() );
                        birthdate       =  birthdate.substr(0,10);
                        id                      = (*patient)->getIDDicom();

                        ::fwData::Patient::StudyIterator studyBegin = (*patient)->getStudies().first;
                        ::fwData::Patient::StudyIterator studyEnd = (*patient)->getStudies().second;
                        ::fwData::Patient::StudyIterator study = studyBegin;

                        while ( study != studyEnd )
                        {
                                hospital        = (*study)->getHospital();
                                modality        = (*study)->getModality();
                                zone            = (*study)->getAcquisitionZone();

                                ::fwData::Study::AcquisitionIterator acquisitionBegin = (*study)->getAcquisitions().first;
                                ::fwData::Study::AcquisitionIterator acquisitionEnd = (*study)->getAcquisitions().second;
                                ::fwData::Study::AcquisitionIterator acquisition = acquisitionBegin;

                                while ( acquisition != acquisitionEnd )
                                {

                                        acqDate         = boost::posix_time::to_iso_extended_string( (*acquisition)->getCreationDate() );
                                        acqDate         =  acqDate.substr(0,10) + " " + acqDate.substr(11,5);

                                        if ( (*acquisition)->getImage()->getDimension() == 3 )
                                        {
                                                std::stringstream nbImagesStream;
                                                nbImagesStream << (*acquisition)->getImage()->getSize()[2];
                                                nbImages        = nbImagesStream.str();

                                                std::stringstream voxelSizeStream;
                                                voxelSizeStream << (*acquisition)->getImage()->getSpacing()[0] <<       " x " << (*acquisition)->getImage()->getSpacing()[1] << " x " << (*acquisition)->getImage()->getSpacing()[2];
                                                voxelSize       = voxelSizeStream.str();
                                        }

                                        int itemCount = m_wxList->GetItemCount();

                                        std::vector<int> vTriSelection;
                                        vTriSelection.push_back(patient         - patientBegin);
                                        vTriSelection.push_back(study           - studyBegin);
                                        vTriSelection.push_back(acquisition     - acquisitionBegin);

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

                                        SLM_TRACE("acquisition++")
                                        ++acquisition;
                                } // end acquisition

                                SLM_TRACE("study++")
                                ++study;
                        } // end study

                        SLM_TRACE("patient++")
                        ++patient;
                } // end patient
        }


        // Select the first item in list
        if ( _associatedPatientDB->getPatientSize() > 0 )
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
        SLM_TRACE("AcquisitionSelectorPanel::eraseItemSelected");

        // Patient selection
        ::fwData::PatientDB::PatientIterator patientIter = m_associatedPatientDB->getPatients().first;
        patientIter += _vItemSelected[0];

        // Study selection
        ::fwData::Patient::StudyIterator studyIter = (*patientIter)->getStudies().first;
        int nbStudies = (*patientIter)->getStudySize();
        studyIter += _vItemSelected[1];

        // Acquisition selection
        ::fwData::Study::AcquisitionIterator acquisitionIter = (*studyIter)->getAcquisitions().first;
        int nbAcquisitions = (*studyIter)->getAcquisitionSize();
        acquisitionIter += _vItemSelected[2];

        (*studyIter)->getField( ::fwData::Study::ID_ACQUISITIONS )->children().erase( acquisitionIter.base() );

        if( nbAcquisitions == 1 )
        {
                // Erase study
                (*patientIter)->getField( ::fwData::Patient::ID_STUDIES )->children().erase( studyIter.base() );

                if ( nbStudies == 1 )
                {
                        // Erase patient
                        m_associatedPatientDB->getField( ::fwData::PatientDB::ID_PATIENTS )->children().erase( patientIter.base() );
                }
        }
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
EVT_LIST_KEY_DOWN               ( AcquisitionSelectorPanel::ID_LIST, AcquisitionSelectorPanel::onKeyDown )
EVT_LIST_ITEM_ACTIVATED ( AcquisitionSelectorPanel::ID_LIST, AcquisitionSelectorPanel::onItemActivated )
END_EVENT_TABLE()

//------------------------------------------------------------------------------

} //namespace io
