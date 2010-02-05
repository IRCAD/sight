/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <wx/wx.h>
#include <wx/version.h>

#if wxCHECK_VERSION(2, 9, 0)

#include <wx/panel.h>
#include <wx/aui/aui.h>
#include <wx/dataview.h>
#include <wx/sizer.h>
#include <wx/colour.h>

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
#include <fwData/Material.hpp>
#include <fwData/Boolean.hpp>

#include <fwComEd/Dictionary.hpp>
#include <fwComEd/ReconstructionMsg.hpp>
#include <fwComEd/MaterialMsg.hpp>
#include <fwComEd/PatientDBMsg.hpp>
#include <fwTools/dateAndTime.hpp>
#include <fwTools/Stringizer.hpp>

#include "fwWX/AcquisitionSelectorPanelDataView.hpp"
#include "fwWX/PatientEditor.hpp"


namespace fwWX
{

//------------------------------------------------------------------------------

AcquisitionSelectorPanelDataView::AcquisitionSelectorPanelDataView(wxWindow * _container, ::boost::shared_ptr< ::fwData::PatientDB > _associatedPatientDB, ::fwServices::IService::wptr _serviceParent ) :
                                wxPanel( _container, /*wxID_ANY*/ID_PANELDATAVIEW, wxDefaultPosition, wxDefaultSize, wxNO_BORDER|wxTAB_TRAVERSAL ),
                                m_associatedPatientDB(_associatedPatientDB),
                                m_vSelectedItem(3,-1),
                                m_serviceParent(_serviceParent)
{
        // Creation of wxAuiManager
        wxAuiManager * infoPatientAuiManager = new wxAuiManager( this );

        // Center panel
        wxPanel * center = new wxPanel( this , wxID_ANY,
                        wxDefaultPosition, wxDefaultSize,
                        wxNO_BORDER|wxTAB_TRAVERSAL ) ;
        wxAuiPaneInfo centerInfo;
        centerInfo.Center();
        centerInfo.MinSize(wxSize(400,-1));
        centerInfo.CloseButton(false);
        centerInfo.Floatable(false);
        centerInfo.MaximizeButton(true);
        centerInfo.Caption(_("Series selector"));

        // Right Panel
        wxPanel * rightPanel = new wxPanel( this , wxID_ANY,
                        wxDefaultPosition, wxDefaultSize,
                        wxNO_BORDER|wxTAB_TRAVERSAL ) ;
        wxAuiPaneInfo rightInfo;
        rightInfo.Right();
        rightInfo.MinSize(wxSize(300,-1));
        rightInfo.CloseButton(false);
        rightInfo.Floatable(false);
        rightInfo.MaximizeButton(true);
        rightInfo.Caption(_("Property"));
        rightInfo.Show(true);
        rightInfo.SetFlag(wxAuiPaneInfo::savedHiddenState, rightInfo.HasFlag(wxAuiPaneInfo::optionHidden));
        rightInfo.Hide();

        infoPatientAuiManager->AddPane( rightPanel, rightInfo );

        centerInfo.Maximize();
        infoPatientAuiManager->AddPane( center, centerInfo );

    if ( !wxPGGlobalVars )
        wxPGGlobalVars = new wxPGGlobalVarsClass();

    m_pg = new wxPropertyGrid(rightPanel,ID_PROPGRID,wxDefaultPosition,wxSize(500,350),
                        wxPG_SPLITTER_AUTO_CENTER |
                        wxPG_BOLD_MODIFIED );
    // Register all editors (SpinCtrl etc.)
    m_pg->RegisterAdditionalEditors();

    m_wxDataViewCtrl = new wxDataViewCtrl( center, ID_LIST, wxDefaultPosition, wxDefaultSize, wxDV_ROW_LINES );
        m_patientDbTree_model = new ::PatientDBTreeModel(m_associatedPatientDB);
        m_wxDataViewCtrl->AssociateModel( m_patientDbTree_model );

        int cellWidthT1 = 80;
        int cellWidthT2 = 160;
        int cellWidthT3 = 200;
        int cellWidthT4 = 400;
        int col = 0;


        wxDataViewIconTextRenderer *tir = new wxDataViewIconTextRenderer( "wxDataViewIconText", wxDATAVIEW_CELL_INERT );
        wxDataViewColumn *column0 = new wxDataViewColumn( "Name", tir, col++, cellWidthT3, wxALIGN_LEFT,
                        wxDATAVIEW_COL_RESIZABLE );
        m_wxDataViewCtrl->AppendColumn( column0 );

        wxDataViewTextRenderer *tr = new wxDataViewTextRenderer( "string", wxDATAVIEW_CELL_INERT );
        wxDataViewColumn *column2 = new wxDataViewColumn( "Modality", tr, col++, cellWidthT1, wxALIGN_LEFT,
                        wxDATAVIEW_COL_RESIZABLE );
        m_wxDataViewCtrl->AppendColumn( column2 );


        tr = new wxDataViewTextRenderer( "string", wxDATAVIEW_CELL_INERT );
        wxDataViewColumn *column6 = new wxDataViewColumn( "Acq. date", tr, col++, cellWidthT2, wxALIGN_LEFT,
                        wxDATAVIEW_COL_RESIZABLE );
        m_wxDataViewCtrl->AppendColumn( column6 );

        tr = new wxDataViewTextRenderer( "string", wxDATAVIEW_CELL_INERT );
        wxDataViewColumn *column7 = new wxDataViewColumn( "Image size", tr, col++, cellWidthT2, wxALIGN_LEFT,
                        wxDATAVIEW_COL_RESIZABLE );
        m_wxDataViewCtrl->AppendColumn( column7 );

        tr = new wxDataViewTextRenderer( "string", wxDATAVIEW_CELL_INERT );
        wxDataViewColumn *column9 = new wxDataViewColumn( "Voxel size", tr, col++, cellWidthT2, wxALIGN_LEFT,
                        wxDATAVIEW_COL_RESIZABLE );
        m_wxDataViewCtrl->AppendColumn( column9 );

        tr = new wxDataViewTextRenderer( "string", wxDATAVIEW_CELL_INERT );
        wxDataViewColumn *column10 = new wxDataViewColumn( "Comment", tr, col++, cellWidthT4, wxALIGN_LEFT,
                        wxDATAVIEW_COL_RESIZABLE );
        m_wxDataViewCtrl->AppendColumn( column10 );

        wxSizer *       testSizer       ( new wxBoxSizer( wxVERTICAL ) );
        testSizer->Add(m_wxDataViewCtrl, 1, wxALL|wxEXPAND);
        center->SetSizer(       testSizer );

        wxSizer *       testSizerRight  ( new wxBoxSizer( wxVERTICAL ) );
        testSizerRight->Add(m_pg, 1, wxALL|wxEXPAND);
        rightPanel->SetSizer(   testSizerRight );

        infoPatientAuiManager->Update();
}

//------------------------------------------------------------------------------

AcquisitionSelectorPanelDataView::~AcquisitionSelectorPanelDataView()
{
        if(m_pg)
        {
                delete m_pg;
                m_pg = 0;
        }
        if(m_wxDataViewCtrl)
        {
                delete m_wxDataViewCtrl;
                m_wxDataViewCtrl = 0;
        }
        if(m_patientDbTree_model)
        {
                delete m_patientDbTree_model;
                m_patientDbTree_model = 0;
        }
}

//------------------------------------------------------------------------------

void AcquisitionSelectorPanelDataView::itemSelectionNotification()
{
        SLM_TRACE("itemSelectionNotification");
        fwDataNode *node = static_cast< fwDataNode* >(m_wxDataViewCtrl->GetSelection ().GetID());
        if(node)
        {
                m_patientDbTree_model->SelectNode(node);
                m_wxDataViewCtrl->Refresh();
        }

        updatePropGrid();

        if(m_vSelectedItem[0] != getIndexOfSelectedItem()[0] ||
                        m_vSelectedItem[1] != getIndexOfSelectedItem()[1]||
                        m_vSelectedItem[2] != getIndexOfSelectedItem()[2])
        {
                m_vSelectedItem = getIndexOfSelectedItem();
                ::fwData::Object::NewSptr acqSelected;
                acqSelected->children().push_back( ::fwData::Integer::NewSptr(getIndexOfSelectedItem()[0]) );
                acqSelected->children().push_back( ::fwData::Integer::NewSptr(getIndexOfSelectedItem()[1]) );
                acqSelected->children().push_back( ::fwData::Integer::NewSptr(getIndexOfSelectedItem()[2]) );

                // Add Field
                m_associatedPatientDB.lock()->removeField( fwComEd::Dictionary::m_imageSelectedId );
                m_associatedPatientDB.lock()->addFieldElement( fwComEd::Dictionary::m_imageSelectedId,
                                ::fwData::Integer::NewSptr(getIndexOfSelectedItem()[0]) );
                m_associatedPatientDB.lock()->addFieldElement( fwComEd::Dictionary::m_imageSelectedId,
                                ::fwData::Integer::NewSptr(getIndexOfSelectedItem()[1]) );
                m_associatedPatientDB.lock()->addFieldElement( fwComEd::Dictionary::m_imageSelectedId,
                                ::fwData::Integer::NewSptr(getIndexOfSelectedItem()[2]) );

                // notification
                //::boost::shared_ptr< ::fwComEd::PatientDBMsg > msg ( new ::fwComEd::PatientDBMsg( m_associatedPatientDB.lock() ) ) ;
                //msg->addMessageInformation( ::fwComEd::PatientDBMsg::NEW_IMAGE_SELECTED, acqSelected );
                ::fwComEd::PatientDBMsg::NewSptr msg;
                msg->addEvent( ::fwComEd::PatientDBMsg::NEW_IMAGE_SELECTED, acqSelected );

//              ::boost::shared_ptr< ::fwServices::IEditionService > basicEditor = ::fwServices::get< ::fwServices::IEditionService >( m_associatedPatientDB.lock() ) ;
//              basicEditor->notify( msg );
                ::fwServices::IEditionService::notify(m_serviceParent.lock(), m_associatedPatientDB.lock(), msg);
        }
        else
        {
                SLM_TRACE("Item already selected");
        }
}
//------------------------------------------------------------------------------
void AcquisitionSelectorPanelDataView::SelectLastItem()
{
        SLM_TRACE("AcquisitionSelectorPanelDataView::SelectLastItem()");
        bool bfind = false;
        ::boost::shared_ptr<fwDataNode> root = m_patientDbTree_model->getRoot();
        if(root && root->GetChildCount()>0)
        {
                fwDataNode* patient = root->GetNthChild(root->GetChildCount()-1);
                if(patient && patient->GetChildCount()>0)
                {
                        fwDataNode* acq =  patient->GetNthChild(patient->GetChildCount()-1);
                        if(acq)
                        {
                                wxDataViewItem item( acq );
                                m_wxDataViewCtrl->Select( item );
                                bfind = true;
//                              m_patientDbTree_model->SelectNode(acq);
                        }
                }
        }
        if (bfind)
        {
                updatePropGrid();
                itemSelectionNotification();
        }
        else
        {
                m_pg->Clear();
        }
}
//------------------------------------------------------------------------------
void AcquisitionSelectorPanelDataView::SelectItem( std::vector<int> select)
{
        ::boost::shared_ptr<fwDataNode> root = m_patientDbTree_model->getRoot();
        if( root && (select.size() == 3) && (root->GetChildCount() > select[0]) )
        {
                fwPatientNode* patient = static_cast< fwPatientNode* > (root->GetNthChild(select[0]));
                OSLM_INFO("root.GetChildCount: " << root->GetChildCount());
                if(patient && ( patient->GetChildCount() > select[2]) )
                {
                        OSLM_INFO("patient.GetChildCount: " << patient->GetChildCount());
                        for(int i = 0; i < patient->GetChildCount(); i++)
                        {
                                fwDataNode* acq =  patient->GetNthChild(i);
                                OSLM_INFO("acq : " << i << " #:" << acq );
                        }
                        // get index of selected acquisition
                        int count = select[2];
                        if (select[1] > 0)
                        {
                                int i=0;
                                ::fwData::Patient::StudyIterator studyIter = patient->getPatient().lock()->getStudies().first;
                                ::fwData::Patient::StudyIterator end = patient->getPatient().lock()->getStudies().second;
                                while (i < select[1] && studyIter!=end)
                                {
                                        count += (*studyIter)->getAcquisitionSize();
                                        i++;
                                        ++studyIter;
                                }
                        }
                        fwDataNode* acq =  patient->GetNthChild(count);
                        if(acq)
                        {
                                OSLM_INFO("acq.GetChildCount: " << acq->GetChildCount());
                                wxDataViewItem item( acq );
                                m_wxDataViewCtrl->Select( item );
//                              m_patientDbTree_model->SelectNode(acq);
                                updatePropGrid();
                                itemSelectionNotification();
                                OSLM_INFO("select: "<< select[0]<<" - " << select[1]<<" - "<< select[2]<<" ")
                        }
                }
        }
}

//------------------------------------------------------------------------------
void AcquisitionSelectorPanelDataView::SelectItem(fwDataNode* select)
{
        if(select)
        {
                wxDataViewItem item( select );
                m_wxDataViewCtrl->Select( item );
                updatePropGrid();
                itemSelectionNotification();
        }
}
//------------------------------------------------------------------------------

void AcquisitionSelectorPanelDataView::updatePropGrid()
{
        m_pg->Clear();
        m_pg->Freeze();
//      fwDataNode *dataTmp = (fwDataNode*) m_wxDataViewCtrl->GetSelection ().GetID();
        fwDataNode *dataTmp = static_cast<fwDataNode*>(m_wxDataViewCtrl->GetSelection ().GetID());
        if(dataTmp)
        {
                fwPatientNode *patient = dynamic_cast<fwPatientNode*>(dataTmp);
                if (patient)
                {
                        m_pg->Append( new wxPropertyCategory(wxT("patient"),wxPG_LABEL) );

                        std::string date        = boost::posix_time::to_iso_extended_string( patient->getPatient().lock()->getBirthdate() );
                        date =  date.substr(0,10);
                        wxPGProperty *id = m_pg->Append( new wxStringProperty(("Birthdate"), wxPG_DATE_FORMAT, date.c_str()));
                        m_pg->DisableProperty(id);

                        id = m_pg->Append( new wxStringProperty("Sex", wxPG_LABEL,
                                        wxConvertMB2WX((patient->getPatient().lock()->getIsMale()?"Male":"Female"))));
                        m_pg->DisableProperty(id);
                        id = m_pg->Append( new wxStringProperty("DICOM ID", wxPG_LABEL,
                                        wxConvertMB2WX(patient->getPatient().lock()->getIDDicom().c_str())) );
                        m_pg->DisableProperty(id);
                }
                else
                {
                        fwAcqNode *acq = dynamic_cast<fwAcqNode*>(dataTmp);
                        if (acq)
                        {
                                // patient
                                fwPatientNode *patientParent = dynamic_cast<fwPatientNode*>(acq->GetParent());
                                if (patientParent)
                                {
                                        m_pg->Append( new wxPropertyCategory(wxT("patient"),wxPG_LABEL) );

                                        std::string date        = boost::posix_time::to_iso_extended_string( patientParent->getPatient().lock()->getBirthdate() );
                                        date =  date.substr(0,10);
                                        wxPGProperty *id = m_pg->Append( new wxStringProperty(("Birthdate"), wxPG_DATE_FORMAT, date.c_str()));
                                        m_pg->DisableProperty(id);

                                        id = m_pg->Append( new wxStringProperty("Sex", wxPG_LABEL,
                                                        wxConvertMB2WX((patientParent->getPatient().lock()->getIsMale()?"Male":"Female"))));
                                        m_pg->DisableProperty(id);
                                        id = m_pg->Append( new wxStringProperty("DICOM ID", wxPG_LABEL,
                                                        wxConvertMB2WX(patientParent->getPatient().lock()->getIDDicom().c_str())) );
                                        m_pg->DisableProperty(id);
                                }

                                //study
                                m_pg->Append( new wxPropertyCategory(wxT("Study"),wxPG_LABEL) );
                                wxPGProperty *id = m_pg->Append( new wxStringProperty("Hospital", wxPG_LABEL ,
                                                wxConvertMB2WX(acq->getStudy().lock()->getHospital().c_str())) );
                                m_pg->DisableProperty(id);

                                // serie
                                m_pg->Append( new wxPropertyCategory(wxT("Serie"),wxPG_LABEL));
                                id = m_pg->Append( new wxStringProperty("BitsPerPixel", wxPG_LABEL,
                                                wxConvertMB2WX(::fwTools::getString<int>(acq->getAcq().lock()->getBitsPerPixel()).c_str())) );
                                m_pg->DisableProperty(id);
                                id = m_pg->Append( new wxStringProperty("ImageType", wxPG_LABEL,
                                                wxConvertMB2WX(acq->getAcq().lock()->getImageType().c_str())) );
                                m_pg->DisableProperty(id);
#ifdef USE_INTERNAL_FEATURES
                                id = m_pg->Append( new wxIntProperty("Labo ID", wxPG_LABEL, acq->getAcq().lock()->getLaboID() ) );
                                m_pg->DisableProperty(id);
#endif
//                              // reconstruction
//                              m_pg->Append( new wxPropertyCategory(wxT("Reconstructions"),wxPG_LABEL));
//                              ::fwData::Acquisition::ReconstructionIterator reconstructionBegin = acq->getAcq()->getReconstructions().first;
//                              ::fwData::Acquisition::ReconstructionIterator reconstructionEnd = acq->getAcq()->getReconstructions().second;
//                              ::fwData::Acquisition::ReconstructionIterator reconstruction = reconstructionBegin;
//
//                              if ( reconstructionBegin != reconstructionEnd)
//                              {
//                                      bool showAllRec = acq->getAcq()->getFieldSize("ShowReconstructions") ? acq->getAcq()->getFieldSingleElement< ::fwData::Boolean >("ShowReconstructions")->value() : true;
//                                      acq->getAcq()->setFieldSingleElement("ShowReconstructions",  ::fwData::Boolean::NewSptr(showAllRec) );
//                                      m_showAllRecPGProperty.first = acq->getAcq();
//                                      m_showAllRecPGProperty.second = m_pg->Append(  new wxBoolProperty( "Show reconstructions", wxPG_LABEL, true ) );
//                                      m_pg->SetPropertyAttribute( m_showAllRecPGProperty.second , wxPG_BOOL_USE_CHECKBOX, showAllRec );
//                              }
//
//
//                              m_vObjectEditor.clear();
//                              while ( reconstruction != reconstructionEnd )
//                              {
//                                      wxString uid = wxConvertMB2WX((*reconstruction)->getUUID().c_str());
//                                      wxString name = wxConvertMB2WX((*reconstruction)->getOrganName().c_str());
//                                      wxPGProperty* pid = m_pg->Append( new wxStringProperty(name, uid ));
//                                      m_pg->DisableProperty(pid);
//                                      ::fwData::Material::sptr mat = (*reconstruction)->getMaterial();
//                                      wxColour wxMaterialColor = wxColour (
//                                                      mat->ambient().red()*255,
//                                                      mat->ambient().green()*255,
//                                                      mat->ambient().blue()*255,
//                                                      mat->ambient().alpha()*255
//                                                      );
//                                      wxPGProperty* pid2 = m_pg->AppendIn(pid, new wxIntProperty (wxT("Transparent"),
//                                                      wxPG_LABEL,
//                                                      mat->ambient().alpha()*255 ) );
//
//                                      std::vector<const wxPGProperty *> vEditors;
//
//                                      wxPGEditor *pEditor1 = wxPG_EDITOR(SpinCtrl);
//                                      vEditors.push_back(pid2);
//                                      OSLM_TRACE("Prop1 : " << pid2);
//
//                                      m_pg->SetPropertyEditor(pid2, pEditor1);
//                                      m_pg->SetPropertyAttribute(pid2, wxPG_ATTR_MIN, (long)0 );
//                                      m_pg->SetPropertyAttribute(pid2, wxPG_ATTR_MAX, (long)255 );
//                                      m_pg->SetPropertyAttribute(pid2, wxT("Step"), (long)2 );
//                                      m_pg->SetPropertyAttribute(pid2, wxT("MotionSpin"), true );
//
//                                      pid2 = m_pg->AppendIn( pid, new wxColourProperty(wxT("ColorProperty"),wxPG_LABEL, wxMaterialColor) );
//                                      vEditors.push_back(pid2);
//                                      OSLM_TRACE("Prop2 : " << pid2);
//
//                                      pid2 = m_pg->AppendIn(pid, new wxBoolProperty( "Visible", wxPG_LABEL, (*reconstruction)->getIsVisible() ) );
//                                      m_pg->SetPropertyAttribute(pid2, wxPG_BOOL_USE_CHECKBOX, (*reconstruction)->getIsVisible() );
//                                      vEditors.push_back(pid2);
//                                      OSLM_TRACE("Prop3 : " << pid2);
//
//                                      std::pair < ::fwData::Object::sptr, std::vector< const wxPGProperty*> > mObjEdit;
//                                      mObjEdit = std::make_pair(*reconstruction, vEditors);
//                                      m_vObjectEditor.push_back(mObjEdit);
//
//                                      ++reconstruction;
//                              }

                        }
                        else
                        {
                                m_pg->Append( new wxStringProperty("unknown", wxPG_LABEL) );
                        }
                }
        }
        m_pg->Thaw();
}
//------------------------------------------------------------------------------

void AcquisitionSelectorPanelDataView::onItemSelected( wxDataViewEvent & event )
{
        SLM_TRACE("onItemSelected");
//      fwDataNode *node = (fwDataNode*) event.GetItem().GetID();
        //fwDataNode* node = static_cast< fwDataNode * >( event.GetItem().GetID() );
        fwDataNode *node= static_cast< fwDataNode* >(m_wxDataViewCtrl->GetSelection ().GetID());

        // TODO if only image/acquisition without patient?
        if(node && (node->GetChildren().GetCount() > 0 || node->GetParent()))
        {
                wxBeginBusyCursor();
//              m_patientDbTree_model->SelectNode(node);
                itemSelectionNotification();
                wxEndBusyCursor();
        }
}

//------------------------------------------------------------------------------

void AcquisitionSelectorPanelDataView::updateData( ::fwData::PatientDB::sptr _associatedPatientDB, bool _bResetSelection )
{
        SLM_TRACE("AcquisitionSelectorPanelDataView::updateData()");

        std::vector<int> oldSelection = getIndexOfSelectedItem();
//      fwDataNode *oldSelectNode = (fwDataNode*) m_wxDataViewCtrl->GetSelection ().GetID();
        fwDataNode *oldSelectNode = static_cast< fwDataNode* >(m_wxDataViewCtrl->GetSelection ().GetID());

        if (_associatedPatientDB)
        {
                m_associatedPatientDB = _associatedPatientDB;
                m_wxDataViewCtrl->Freeze ();
                m_patientDbTree_model->setPatientDB(m_associatedPatientDB);
                m_wxDataViewCtrl->Thaw();
        }


        if ( _associatedPatientDB->getPatientSize() <= 0 )
        {
                // reinit selecteditem
                m_vSelectedItem[0] = -1;
                m_vSelectedItem[1] = -1;
                m_vSelectedItem[2] = -1;
        }

        if(_bResetSelection)
        {
                m_vSelectedItem[0] = -1;
                m_vSelectedItem[1] = -1;
                m_vSelectedItem[2] = -1;
                SelectLastItem();
        }
        else
        {
                SelectItem(oldSelection);
        }

}

//------------------------------------------------------------------------------

void AcquisitionSelectorPanelDataView::eraseItemSelected( const std::vector<int> & _vItemSelected )
{
        SLM_TRACE("AcquisitionSelectorPanelDataView::eraseItemSelected");

        // Patient selection
        ::fwData::PatientDB::PatientIterator patientIter = m_associatedPatientDB.lock()->getPatients().first;
        patientIter += _vItemSelected[0];
        OSLM_DEBUG("erase Patient: "<<_vItemSelected[0]);

        // Study selection
        ::fwData::Patient::StudyIterator studyIter = (*patientIter)->getStudies().first;
        int nbStudies = (*patientIter)->getStudySize();
        studyIter += _vItemSelected[1];
        OSLM_DEBUG("erase Study: "<<_vItemSelected[1]);

        // Acquisition selection
        ::fwData::Study::AcquisitionIterator acquisitionIter = (*studyIter)->getAcquisitions().first;
        int nbAcquisitions = (*studyIter)->getAcquisitionSize();
        acquisitionIter += _vItemSelected[2];
        OSLM_DEBUG("erase Acquisition: "<<_vItemSelected[2]);

        (*studyIter)->getField( ::fwData::Study::ID_ACQUISITIONS )->children().erase( acquisitionIter.base() );

        if( nbAcquisitions == 1 )
        {
                // Erase study
                (*patientIter)->getField( ::fwData::Patient::ID_STUDIES )->children().erase( studyIter.base() );

                if ( nbStudies == 1 )
                {
                        // Erase patient
                        m_associatedPatientDB.lock()->getField( ::fwData::PatientDB::ID_PATIENTS )->children().erase( patientIter.base() );
                }
        }
}

//------------------------------------------------------------------------------

const std::vector<int> AcquisitionSelectorPanelDataView::getIndexOfSelectedItem()
{
        std::vector< int > selection (3,0);

//      fwDataNode *node = (fwDataNode*) m_wxDataViewCtrl->GetSelection ().GetID();
        fwDataNode *node = static_cast< fwDataNode* >(m_wxDataViewCtrl->GetSelection ().GetID());
        if(node)
                selection = node->getPosition();

        return selection;
}

//------------------------------------------------------------------------------

void AcquisitionSelectorPanelDataView::UpdateProperty(wxPropertyGridEvent& event)
{
    wxPGProperty* property = event.GetProperty();

    if (property)
    {
                const wxString& name = property->GetName();
                wxVariant value = event.GetValue();
                //wxVariant value = property->GetValue();

                // Don't handle 'unspecified' values
                if ( value.IsNull() )
                        return;

                if ( m_showAllRecPGProperty.second == property  )
                {
                        bool showAllRecs = value.GetLong();
                        m_showAllRecPGProperty.first.lock()->setFieldSingleElement("ShowReconstructions", ::fwData::Boolean::NewSptr(showAllRecs) );
//                      ::boost::shared_ptr< ::fwServices::IEditionService > editor = ::fwServices::get< fwServices::IEditionService >(         m_showAllRecPGProperty.first.lock() ) ;

                        //::boost::shared_ptr< ::fwServices::ObjectMsg > msg( new ::fwServices::ObjectMsg(      m_showAllRecPGProperty.first.lock()) ) ;
                        //msg->addModif( "ShowReconstructions" ) ;
                        ::fwServices::ObjectMsg::NewSptr msg;
                        msg->addEvent("ShowReconstructions");

//                      editor->notify( msg );
                        ::fwServices::IEditionService::notify(m_serviceParent.lock(), m_showAllRecPGProperty.first.lock(), msg);
                        return;
                }


                std::vector< std::pair < ::fwData::Object::sptr, std::vector<const wxPGProperty *> > >::iterator iter =  m_vObjectEditor.begin();
                std::vector< std::pair < ::fwData::Object::sptr, std::vector<const wxPGProperty *> > >::iterator iterEnd =  m_vObjectEditor.end();

                while (iter < iterEnd)
                {
                        std::vector<const wxPGProperty *> vEditor = iter->second;
                        std::vector<const wxPGProperty *>::iterator vIter = vEditor.begin();
                        std::vector<const wxPGProperty *>::iterator vIterEnd = vEditor.end();
                        while ( vIter < vIterEnd )
                        {
                                if ( (*vIter) == property)
                                {
                                        ::fwData::Reconstruction::sptr rec = ::fwData::Reconstruction::dynamicCast(iter->first);

                                        if(rec)
                                        {
                                                OSLM_DEBUG("Reconstruction index event : " << (vIter - vEditor.begin()));
                                                ::fwData::Material::sptr mat = rec->getMaterial();

                                                if (( vIter - vEditor.begin() ) == 0 ) // Alpha
                                                {
                                                        int a = value.GetLong();
                                                        mat->ambient().alpha() = (float) (a / 255.0) ;
                                                        OSLM_DEBUG("wxProp : UpdateProperty Alpha" );
                                                }
                                                if (( vIter - vEditor.begin() ) == 1 ) // RGB
                                                {
                                                        wxColour col;
                                                        col << value;
                                                        OSLM_DEBUG("wxProp : UpdateProperty RGB: r=" << (int) col.Red() << " g=" <<  (int) col.Green() << " b=" << (int)  col.Blue() );

                                                        float red   = ((int) col.Red())/255.0;
                                                        float green = ((int) col.Green())/255.0;
                                                        float blue  = ((int) col.Blue())/255.0;
                                                        mat->ambient().red() = red;
                                                        mat->ambient().green() = green;
                                                        mat->ambient().blue() = blue;
                                                        OSLM_DEBUG("wxProp : UpdateProperty RGB: r=" << red << " g=" << green << " b=" << blue );

                                                }
                                                if (( vIter - vEditor.begin() ) == 2 ) // Visibility
                                                {

                                                        bool isVisible = value.GetLong();
                                                        OSLM_DEBUG("wxProp : UpdateProperty Visibility" <<  isVisible);
                                                        rec->setIsVisible(isVisible);
//                                                      ::boost::shared_ptr< ::fwServices::IEditionService > editor = ::fwServices::get< fwServices::IEditionService >( rec ) ;
                                                        ::fwComEd::ReconstructionMsg::NewSptr msg;
                                                        msg->addEvent( ::fwComEd::ReconstructionMsg::VISIBILITY ) ;
//                                                      editor->notify( msg );
                                                        ::fwServices::IEditionService::notify(m_serviceParent.lock(), rec, msg);
                                                }

//                                              ::boost::shared_ptr< ::fwServices::IEditionService > editor = ::fwServices::get< fwServices::IEditionService >( rec ) ;
                                                ::fwComEd::MaterialMsg::NewSptr msg;
                                                msg->addEvent( ::fwComEd::MaterialMsg::MATERIAL_IS_MODIFIED ) ;
//                                              editor->notify( msg );
                                                ::fwServices::IEditionService::notify(m_serviceParent.lock(), rec, msg);
                                        }

                                        OSLM_DEBUG("UpdateProperty UUID : " << iter->first->getUUID());
                                }
                                OSLM_DEBUG("Obj : " << iter->first->getUUID() << " editor : " << (void *)*vIter );
                                vIter++;
                        }
                        iter++;
                }
    }
}

//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE( AcquisitionSelectorPanelDataView, wxPanel )
        EVT_DATAVIEW_SELECTION_CHANGED  ( AcquisitionSelectorPanelDataView::ID_LIST, AcquisitionSelectorPanelDataView::onItemSelected )
        EVT_PG_CHANGING(ID_PROPGRID, AcquisitionSelectorPanelDataView::UpdateProperty)
END_EVENT_TABLE()

//------------------------------------------------------------------------------

} //namespace fwWX

#endif
