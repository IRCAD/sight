/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <string>

#include <boost/lexical_cast.hpp>

#include <wx/wx.h>
#include <wx/button.h>
#include <wx/listctrl.h>
#include <wx/sizer.h>
#include <wx/string.h>
#include <wx/settings.h>
#include <wx/textctrl.h>
#include <wx/menu.h>
#include <wx/encconv.h>

#include <boost/algorithm/string/case_conv.hpp>
#include <boost/regex.hpp>

#include <fwData/PatientDB.hpp>
#include <fwData/Patient.hpp>
#include <fwData/Study.hpp>
#include <fwData/Acquisition.hpp>
#include <fwData/Image.hpp>

#include <fwCore/base.hpp>

#include "fwWX/PatientEditor.hpp"
#include "fwWX/convert.hpp"

namespace fwWX
{

//==============================================================================
// class PatientEditorDialog
//==============================================================================

PatientEditorDialog::PatientEditorDialog(
            wxWindow * parent,
            ::boost::shared_ptr< ::fwData::Patient > _pPatient,
            ::boost::shared_ptr< ::fwData::Study > _pStudy,
            ::boost::shared_ptr< ::fwData::Acquisition > _pAcquisition ) :
            wxDialog( parent, wxID_ANY, wxGetTranslation("Patient properties"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER ),
            m_pPatient(_pPatient),
            m_pStudy(_pStudy),
            m_pAcquisition(_pAcquisition)
{
    createContent();
    importData();
    disableFieldEdition();
}

//------------------------------------------------------------------------------

PatientEditorDialog::~PatientEditorDialog()
{
}

//------------------------------------------------------------------------------

void PatientEditorDialog::createContent()
{
    SLM_DEBUG("PatientEditorDialog::createContent");

    wxStaticText * patientInfo          = new wxStaticText(this, wxID_ANY, wxGetTranslation("PATIENT PROPERTIES"));
    wxStaticText * studyInfo            = new wxStaticText(this, wxID_ANY, wxGetTranslation("STUDY PROPERTIES"));
    wxStaticText * acquisitionInfo      = new wxStaticText(this, wxID_ANY, wxGetTranslation("ACQUISITION PROPERTIES"));

    // Left sizer ( Patient & Study )
    wxSizer *   leftSizer   ( new wxBoxSizer( wxVERTICAL ) );
    leftSizer->AddSpacer( 10 );
    leftSizer->Add( patientInfo, 0 );
    leftSizer->AddSpacer( 5 );
    leftSizer->Add( createPatientSizer(), 0, wxALL|wxEXPAND);
    leftSizer->AddSpacer( 15 );
    leftSizer->Add( studyInfo );
    leftSizer->AddSpacer( 5 );
    leftSizer->Add( createStudySizer(), 0, wxALL|wxEXPAND);
    leftSizer->AddSpacer( 10 );

    // Right sizer ( Acquisition & Button )
    wxSizer *   rightSizer  ( new wxBoxSizer( wxVERTICAL ) );
    rightSizer->AddSpacer( 10 );
    rightSizer->Add( acquisitionInfo, 0 );
    rightSizer->AddSpacer( 5 );
    rightSizer->Add( createAcquisitionSizer(), 1, wxALL|wxEXPAND );
    rightSizer->AddSpacer( 20 );
    rightSizer->Add( CreateButtonSizer(wxOK|wxCANCEL|wxNO_DEFAULT), 0, wxHORIZONTAL|wxALIGN_RIGHT );
    rightSizer->AddSpacer( 10 );

    // Main sizer
    wxSizer *   rootSizer   ( new wxBoxSizer( wxHORIZONTAL ) );
    rootSizer->AddSpacer( 10 );
    rootSizer->Add( leftSizer, 1, wxALL|wxEXPAND );
    rootSizer->AddSpacer( 20 );
    rootSizer->Add( rightSizer, 1, wxALL|wxEXPAND );
    rootSizer->AddSpacer( 10 );

    SetSizer( rootSizer );
    SetSize( 600, 500 );
    CenterOnParent();
}

//------------------------------------------------------------------------------

wxSizer * PatientEditorDialog::createPatientSizer()
{

    // Ctrl
    wxStaticText * textName         = new wxStaticText(this, wxID_ANY, wxGetTranslation("Name"));
    m_Name                          = new wxTextCtrl(this,wxID_ANY,wxGetTranslation("Unknow"));

    wxStaticText * textFirstname    = new wxStaticText(this, wxID_ANY, wxGetTranslation("FirstName"));
    m_Firstname                     = new wxTextCtrl(this,wxID_ANY,wxGetTranslation("Unknow"));

    wxStaticText * textBirthdate    = new wxStaticText(this, wxID_ANY, wxGetTranslation("Birthdate"));
    wxDateTime dateInit = wxDateTime::Today();
    dateInit.SetCountry(wxDateTime::France);
    m_Birthdate                     = new wxDatePickerCtrl( this, wxID_ANY, dateInit) ;

    // Range start
    wxDateTime rangeStart = wxDateTime::Today();
    rangeStart.Set(1,wxDateTime::Jan,1900);

    // Range end
    wxDateTime rangeEnd = wxDateTime::Today();
    rangeEnd.Set(1,wxDateTime::Jan,2100);


    // set range
    m_Birthdate->SetRange(rangeStart, rangeEnd);


    wxStaticText * textSex          = new wxStaticText(this, wxID_ANY, wxGetTranslation("Sex"));
    wxArrayString sexTags;
    sexTags.Add(wxGetTranslation("M"));
    sexTags.Add(wxGetTranslation("F"));
    m_Sex                           = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, sexTags) ;
    m_Sex->SetSelection(0);

    wxStaticText * textIDDicom      = new wxStaticText(this, wxID_ANY, wxGetTranslation("IDDicom"));
    m_IDDicom                       = new wxTextCtrl(this,wxID_ANY,wxGetTranslation("Unknow"));

    wxStaticText * textDbID         = new wxStaticText(this, wxID_ANY, wxGetTranslation("DbID"));
    m_DbID                          = new wxTextCtrl(this,wxID_ANY,wxGetTranslation("Unknow"));


    // Sizer
    wxFlexGridSizer * patientSizer = new wxFlexGridSizer( 0, 2, 2, 10 );
    patientSizer->AddGrowableCol( 1 );

    patientSizer->Add( textName );
    patientSizer->Add( m_Name, 1, wxEXPAND | wxALL);

    patientSizer->Add( textFirstname );
    patientSizer->Add( m_Firstname, 1, wxEXPAND | wxALL);

    patientSizer->Add( textBirthdate );
    patientSizer->Add( m_Birthdate, 1, wxEXPAND | wxALL);

    patientSizer->Add( textSex );
    patientSizer->Add( m_Sex, 1, wxEXPAND | wxALL);

    patientSizer->Add( textIDDicom );
    patientSizer->Add( m_IDDicom, 1, wxEXPAND | wxALL);

    patientSizer->Add( textDbID );
    patientSizer->Add( m_DbID, 1, wxEXPAND | wxALL);

    return patientSizer;
}

//------------------------------------------------------------------------------

wxSizer * PatientEditorDialog::createStudySizer()
{
    // Ctrl
    wxStaticText * textHospital         = new wxStaticText(this, wxID_ANY, wxGetTranslation("Hospital"));
/*
    dbmfo::dbmfo db;
    wxArrayString hospitalTags;
    std::vector< dbmfo::Hopital *> * hospitalVec = db.getAllHospitals();
    for( std::vector< dbmfo::Hopital *>::const_iterator ho = hospitalVec->begin();
         ho != hospitalVec->end();
         ++ho )
    {
        hospitalTags.Add( stringToWxString((*ho)->getVille()) );
    }
    */


    m_Hospital                          = new wxTextCtrl( this, wxID_ANY, wxGetTranslation("Unknow") ) ;


    wxStaticText * textModality         = new wxStaticText(this, wxID_ANY, wxGetTranslation("Modality"));
    wxArrayString modalityTags;
    modalityTags.Add(wxGetTranslation("CT"));
    modalityTags.Add(wxGetTranslation("MRI"));
    m_Modality                          = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, modalityTags) ;
    m_Modality->SetSelection(0);

//  wxStaticText * textAcquisitionZone  = new wxStaticText(this, wxID_ANY, wxGetTranslation("AcquisitionZone"));
//  m_AcquisitionZone                   = new wxTextCtrl(this,wxID_ANY,wxGetTranslation("Unknow"));

    wxStaticText * textAcquisitionZone_zone         = new wxStaticText(this, wxID_ANY, wxGetTranslation("Acquisition zone"));
    wxStaticText * textAcquisitionZone_organ        = new wxStaticText(this, wxID_ANY, wxGetTranslation("Organ"));
    wxStaticText * textAcquisitionZone_injection    = new wxStaticText(this, wxID_ANY, wxGetTranslation("Injection type"));

    wxArrayString zoneTags;
    zoneTags.Add(wxGetTranslation("Head"));
    zoneTags.Add(wxGetTranslation("Thorax"));
    zoneTags.Add(wxGetTranslation("Abdomen"));
    zoneTags.Add(wxGetTranslation("Pelvis"));
    zoneTags.Add(wxGetTranslation("Abdo Thorax"));
    m_AcquisitionZone_zone = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, zoneTags) ;
    m_AcquisitionZone_zone->SetSelection(0);
    wxArrayString organTags;
//  organTags.Add(wxGetTranslation("Acquisition"));
    organTags.Add(wxGetTranslation("Artery"));
    organTags.Add(wxGetTranslation("BiliarySystem"));
    organTags.Add(wxGetTranslation("Bladder"));
    organTags.Add(wxGetTranslation("Bone"));
    organTags.Add(wxGetTranslation("Bronchus"));
    organTags.Add(wxGetTranslation("Calcium"));
    organTags.Add(wxGetTranslation("Colon"));
    organTags.Add(wxGetTranslation("Cyst"));
    organTags.Add(wxGetTranslation("Duodenum"));
    organTags.Add(wxGetTranslation("Gallbladder"));
    organTags.Add(wxGetTranslation("Heart"));
    organTags.Add(wxGetTranslation("Kidneys"));
//  organTags.Add(wxGetTranslation("Landmark"));
    organTags.Add(wxGetTranslation("Liver"));
    organTags.Add(wxGetTranslation("Lungs"));
    organTags.Add(wxGetTranslation("Muscles"));
//  organTags.Add(wxGetTranslation("Object"));
    organTags.Add(wxGetTranslation("Oesophagus"));
    organTags.Add(wxGetTranslation("Pancreas"));
    organTags.Add(wxGetTranslation("ParathyroidGland"));
    organTags.Add(wxGetTranslation("PortalVein"));
    organTags.Add(wxGetTranslation("Prostate"));
//  organTags.Add(wxGetTranslation("Skin with validity"));
    organTags.Add(wxGetTranslation("SmallIntestin"));
    organTags.Add(wxGetTranslation("Spleen"));
    organTags.Add(wxGetTranslation("Stomach"));
    organTags.Add(wxGetTranslation("SurrenalGland"));
    organTags.Add(wxGetTranslation("ThyroidGland"));
    organTags.Add(wxGetTranslation("Trachea"));
    organTags.Add(wxGetTranslation("Tumor"));
    organTags.Add(wxGetTranslation("Ureter"));
    organTags.Add(wxGetTranslation("VenaCava"));
    organTags.Add(wxGetTranslation("VenousSystem"));
    m_AcquisitionZone_organ = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, organTags) ;
    m_AcquisitionZone_organ->SetSelection(0);

    wxArrayString injectionTags;
    injectionTags.Add(wxGetTranslation("Vein injection"));
    injectionTags.Add(wxGetTranslation("Arterial injection"));
    injectionTags.Add(wxGetTranslation("No injection"));
    m_AcquisitionZone_injection = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, injectionTags) ;
    m_AcquisitionZone_injection->SetSelection(0);


    wxStaticText * textUID              = new wxStaticText(this, wxID_ANY, wxGetTranslation("UID"));
    m_UID                               = new wxTextCtrl(this,wxID_ANY,wxGetTranslation("Unknow"));

    wxStaticText * textDbIDS            = new wxStaticText(this, wxID_ANY, wxGetTranslation("DbID"));
    m_DbIDS                             = new wxTextCtrl(this,wxID_ANY,wxGetTranslation("Unknow"));

    wxStaticText * textRISID            = new wxStaticText(this, wxID_ANY, wxGetTranslation("RISID"));
    m_RISID                             = new wxTextCtrl(this,wxID_ANY,wxGetTranslation("Unknow"));

    // Sizer
    wxFlexGridSizer * studySizer = new wxFlexGridSizer( 0, 2, 2, 10 );
    studySizer->AddGrowableCol( 1 );

    studySizer->Add( textHospital );
    studySizer->Add( m_Hospital, 1, wxEXPAND | wxALL);

    studySizer->Add( textModality );
    studySizer->Add( m_Modality, 1, wxEXPAND | wxALL);

    studySizer->Add( textAcquisitionZone_zone );
    studySizer->Add( m_AcquisitionZone_zone, 1, wxEXPAND | wxALL);

    studySizer->Add( textAcquisitionZone_organ );
    studySizer->Add( m_AcquisitionZone_organ, 1, wxEXPAND | wxALL);

    studySizer->Add( textAcquisitionZone_injection );
    studySizer->Add( m_AcquisitionZone_injection, 1, wxEXPAND | wxALL);

    studySizer->Add( textUID );
    studySizer->Add( m_UID, 1, wxEXPAND | wxALL);

    studySizer->Add( textRISID );
    studySizer->Add( m_RISID, 1, wxEXPAND | wxALL);

    studySizer->Add( textDbIDS );
    studySizer->Add( m_DbIDS, 1, wxEXPAND | wxALL);

    return studySizer;
}

//------------------------------------------------------------------------------

wxSizer * PatientEditorDialog::createAcquisitionSizer()
{
    // Ctrl
    wxStaticText * textLaboID           = new wxStaticText(this, wxID_ANY, wxGetTranslation("LaboID"));
    m_LaboID                            = new wxTextCtrl(this,wxID_ANY,wxGetTranslation("Unknow"));

    wxStaticText * textDate             = new wxStaticText(this, wxID_ANY, wxGetTranslation("Examen date"));
    m_Date                              = new wxDatePickerCtrl( this, wxID_ANY, wxDateTime::Today());

    // Range start
    wxDateTime rangeStart = wxDateTime::Today();
    rangeStart.Set(1,wxDateTime::Jan,1900);

    // Range end
    wxDateTime rangeEnd = wxDateTime::Today();
    rangeEnd.Set(1,wxDateTime::Jan,2100);


    // Set range
    m_Date->SetRange(rangeStart, rangeEnd);

    wxStaticText * textTime             = new wxStaticText(this, wxID_ANY, wxGetTranslation("Examen time"));
    m_Time                              = new wxTextCtrl(this,wxID_ANY,wxGetTranslation("Unknow"));

    wxStaticText * textIndex            = new wxStaticText(this, wxID_ANY, wxGetTranslation("Index"));
    m_Index                             = new wxTextCtrl(this,wxID_ANY,wxGetTranslation("Unknow"));

    wxStaticText * textImageType        = new wxStaticText(this, wxID_ANY, wxGetTranslation("ImageType"));
    m_ImageType                         = new wxTextCtrl(this,wxID_ANY,wxGetTranslation("Unknow"));

    wxStaticText * textImageFormat      = new wxStaticText(this, wxID_ANY, wxGetTranslation("ImageFormat"));
    m_ImageFormat                       = new wxTextCtrl(this,wxID_ANY,wxGetTranslation("Unknow"));

    wxStaticText * textWidth            = new wxStaticText(this, wxID_ANY, wxGetTranslation("Width"));
    m_Width                             = new wxTextCtrl(this,wxID_ANY,wxGetTranslation("Unknow"));

    wxStaticText * textHeight           = new wxStaticText(this, wxID_ANY, wxGetTranslation("Height"));
    m_Height                            = new wxTextCtrl(this,wxID_ANY,wxGetTranslation("Unknow"));

    wxStaticText * textSliceThickness   = new wxStaticText(this, wxID_ANY, wxGetTranslation("SliceThickness"));
    m_SliceThickness                    = new wxTextCtrl(this,wxID_ANY,wxGetTranslation("Unknow"));

    wxStaticText * textVoxelSize        = new wxStaticText(this, wxID_ANY, wxGetTranslation("VoxelSize"));
    m_VoxelSize                         = new wxTextCtrl(this,wxID_ANY,wxGetTranslation("Unknow"));

    wxStaticText * textAxe              = new wxStaticText(this, wxID_ANY, wxGetTranslation("Axe"));
    m_Axe                               = new wxTextCtrl(this,wxID_ANY,wxGetTranslation("Unknow"));

    wxStaticText * textFrameNumber      = new wxStaticText(this, wxID_ANY, wxGetTranslation("FrameNumber"));
    m_FrameNumber                       = new wxTextCtrl(this,wxID_ANY,wxGetTranslation("Unknow"));

    wxStaticText * textBitsPerPixel     = new wxStaticText(this, wxID_ANY, wxGetTranslation("BitsPerPixel"));
    m_BitsPerPixel                      = new wxTextCtrl(this,wxID_ANY,wxGetTranslation("Unknow"));

    wxStaticText * textDbIDA            = new wxStaticText(this, wxID_ANY, wxGetTranslation("DbID"));
    m_DbIDA                             = new wxTextCtrl(this,wxID_ANY,wxGetTranslation("Unknow"));


    // Sizer
    wxFlexGridSizer * acqSizer = new wxFlexGridSizer( 0, 2, 2, 10 );
    acqSizer->AddGrowableCol( 1 );

    acqSizer->Add( textLaboID );
    acqSizer->Add( m_LaboID, 1, wxEXPAND | wxALL);

    acqSizer->Add( textDate );
    acqSizer->Add( m_Date, 1, wxEXPAND | wxALL);

    acqSizer->Add( textTime );
    acqSizer->Add( m_Time, 1, wxEXPAND | wxALL);

    acqSizer->Add( textIndex );
    acqSizer->Add( m_Index, 1, wxEXPAND | wxALL);

    acqSizer->Add( textImageType );
    acqSizer->Add( m_ImageType, 1, wxEXPAND | wxALL);

    acqSizer->Add( textImageFormat );
    acqSizer->Add( m_ImageFormat, 1, wxEXPAND | wxALL);

    acqSizer->Add( textWidth );
    acqSizer->Add( m_Width, 1, wxEXPAND | wxALL);

    acqSizer->Add( textHeight );
    acqSizer->Add( m_Height, 1, wxEXPAND | wxALL);

    acqSizer->Add( textSliceThickness );
    acqSizer->Add( m_SliceThickness, 1, wxEXPAND | wxALL);

    acqSizer->Add( textVoxelSize );
    acqSizer->Add( m_VoxelSize, 1, wxEXPAND | wxALL);

    acqSizer->Add( textAxe );
    acqSizer->Add( m_Axe, 1, wxEXPAND | wxALL);

    acqSizer->Add( textFrameNumber );
    acqSizer->Add( m_FrameNumber, 1, wxEXPAND | wxALL);

    acqSizer->Add( textBitsPerPixel );
    acqSizer->Add( m_BitsPerPixel, 1, wxEXPAND | wxALL);

    acqSizer->Add( textDbIDA );
    acqSizer->Add( m_DbIDA, 1, wxEXPAND | wxALL);

    return acqSizer;
}

//------------------------------------------------------------------------------

void PatientEditorDialog::disableFieldEdition()
{
    SLM_DEBUG("PatientEditorDialog::disableFieldEdition");

    // Fields always disable
    m_DbIDS->Enable(false);
    m_DbID->Enable(false);
    m_Index->Enable(false);
    m_ImageType->Enable(false);
    m_Width->Enable(false);
    m_Height->Enable(false);
    m_SliceThickness->Enable(false);
    m_VoxelSize->Enable(false);
    m_Axe->Enable(false);
    m_FrameNumber->Enable(false);
    m_BitsPerPixel->Enable(false);
    m_DbIDA->Enable(false);

    // Fields disable if the patient come from database
    bool acquisitionIsInDatabase = ( m_pPatient->getDbID() != -1 );

    if( acquisitionIsInDatabase )
    {
        m_Name->Enable(false);
        m_Firstname->Enable(false);
        m_Birthdate->Enable(false);
        m_Sex->Enable(false);
        m_IDDicom->Enable(false);

        m_Hospital->Enable(false);
        m_Modality->Enable(false);
        //m_AcquisitionZone->Enable(false);
        m_AcquisitionZone_zone->Enable(false);
        m_AcquisitionZone_organ->Enable(false);
        m_AcquisitionZone_injection->Enable(false);
        m_UID->Enable(false);
        m_RISID->Enable(false);

        m_LaboID->Enable(false);
    }



}

//------------------------------------------------------------------------------

void PatientEditorDialog::importData()
{
    SLM_DEBUG("PatientEditorDialog::importData");

    importPatientData();
    importStudyData();
    importAcquisitionData();
}

//------------------------------------------------------------------------------

void PatientEditorDialog::importPatientData()
{


    wxString value;

    value = std2wx(m_pPatient->getName());
    m_Name->SetValue(value);

    value = std2wx(m_pPatient->getFirstname());
    m_Firstname->SetValue(value);

    ::boost::gregorian::date birthdateDate = m_pPatient->getBirthdate().date();
    wxDateTime newDate = m_Birthdate->GetValue();
    newDate.Set( (int) birthdateDate.day(), convertMonth( (int) birthdateDate.month() ),(int) birthdateDate.year() );
    m_Birthdate->SetValue(newDate);


//  value = stringToWxString(boost::lexical_cast< std::string >(m_pPatient->getIsMale()));
//  if( m_Sex->FindString(value) >= 0 )
//  {
//      m_Sex->SetStringSelection(value);
//  }
//  else
//  {
//      OSLM_WARN("PatientEditorDialog::importStudyData : the Sex ( "<< wxStringToString(value) <<" ) is not reconized, and set it to default value.");
//  }

    if (m_pPatient->getIsMale())
    {
        m_Sex->SetStringSelection(_T("M"));
    }
    else
    {
        m_Sex->SetStringSelection(_T("F"));
    }


    value = std2wx(m_pPatient->getIDDicom());
    m_IDDicom->SetValue(value);

    value = intToWxString(m_pPatient->getDbID());
    m_DbID->SetValue(value);
}

//------------------------------------------------------------------------------

void PatientEditorDialog::importStudyData()
{

    wxString value;

    value = std2wx(m_pStudy->getHospital());
    m_Hospital->SetValue(value);


    //m_Modality->setStringSelection(value);
    //m_Modality->SetValue(value);
    value = std2wx(m_pStudy->getModality());
    if( m_Modality->FindString(value)  >= 0 )
    {
        m_Modality->SetStringSelection(value);
    }
    else
    {
        OSLM_WARN("PatientEditorDialog::importStudyData : the Modality ( "
                  << wxStringToString(value) <<
                  " ) is not recognized, and set it to default value.");
    }


    //value = stringToWxString(m_pStudy->getAcquisitionZone());
    //m_AcquisitionZone->SetValue(value);
    std::string zone = m_pStudy->getAcquisitionZone();
    boost::regex e ("([a-z A-Z]+)wxGetTranslation([a-z A-Z]+)wxGetTranslation([a-z A-Z]+)");
    if ( regex_match(zone, e) )
    {
        std::string formatKey1 = "\\1";
        std::string formatKey2 = "\\2";
        std::string formatKey3 = "\\3";

        std::string key1 = regex_replace(zone, e, formatKey1, boost::match_default | boost::format_sed);
        std::string key2 = regex_replace(zone, e, formatKey2, boost::match_default | boost::format_sed);
        std::string key3 = regex_replace(zone, e, formatKey3, boost::match_default | boost::format_sed);

        value = std2wx(key1);
        m_AcquisitionZone_zone->SetStringSelection(value);
        value = std2wx(key2);
        m_AcquisitionZone_organ->SetStringSelection(value);
        value = std2wx(key3);
        m_AcquisitionZone_injection->SetStringSelection(value);
    }
    else
    {
        OSLM_WARN("The field AcquisitionZone : \""<< zone <<"\" does not match with this regex : \""<< e << "\". set AcquisitionZone to defalut value");
    }


    value = std2wx(m_pStudy->getUID());
    m_UID->SetValue(value);

    value = intToWxString(m_pStudy->getDbID());
    m_DbIDS->SetValue(value);

    value = std2wx(m_pStudy->getRISId());
    m_RISID->SetValue(value);
}

//------------------------------------------------------------------------------

void PatientEditorDialog::importAcquisitionData()
{
    wxString value;

    value = intToWxString(m_pAcquisition->getLaboID());
    m_LaboID->SetValue(value);

    ::boost::gregorian::date date = m_pAcquisition->getCreationDate().date();
    wxDateTime newDate = m_Date->GetValue();
    newDate.Set( (int) date.day(), convertMonth( (int) date.month() ),(int) date.year() );
    m_Date->SetValue(newDate);

    //value = stringToWxString(m_pAcquisition->getTime());
    //m_Time->SetValue(value);

    //value = intToWxString(m_pAcquisition->getIndex());
    //m_Index->SetValue(value);

    value = std2wx(m_pAcquisition->getImageType());
    m_ImageType->SetValue(value);

    value = std2wx(m_pAcquisition->getImageFormat());
    m_ImageFormat->SetValue(value);

    //value = intToWxString(m_pAcquisition->getWidth());
    //m_Width->SetValue(value);

    //value = intToWxString(m_pAcquisition->getHeight());
    //m_Height->SetValue(value);

    //value = intToWxString(m_pAcquisition->getSliceThickness());
    value = wxString::Format( wxGetTranslation("%f"),m_pAcquisition->getSliceThickness());
    m_SliceThickness->SetValue(value);

    //value = intToWxString(m_pAcquisition->getVoxelSize());
    std::vector< double > voxelSize( m_pAcquisition->getImage()->getSpacing() );
    value = wxString::Format( wxGetTranslation("%f x %f x %f"), voxelSize[0], voxelSize[1], voxelSize[2] );
    m_VoxelSize->SetValue(value);

    value = intToWxString(m_pAcquisition->getAxe());
    m_Axe->SetValue(value);

    //value = intToWxString(m_pAcquisition->getFrameNumber());
    //m_FrameNumber->SetValue(value);

    value = intToWxString(m_pAcquisition->getBitsPerPixel());
    m_BitsPerPixel->SetValue(value);

    value = intToWxString(m_pAcquisition->getDbID());
    m_DbIDA->SetValue(value);
}

//------------------------------------------------------------------------------

void PatientEditorDialog::exportData()
{
    SLM_DEBUG("PatientEditorDialog::exportData");
    exportPatientData();
    exportStudyData();
    exportAcquisitionData();
}

//------------------------------------------------------------------------------

void PatientEditorDialog::exportPatientData()
{
    // Parameter
    std::string value;

    value = wx2std(m_Name->GetValue());
//  value = wxStringToString(m_Name->GetValue());
    m_pPatient->setCRefName(value);

    value = wx2std(m_Firstname->GetValue());
//  value = wxStringToString(m_Firstname->GetValue());
    m_pPatient->setCRefFirstname(value);

    wxDateTime date = m_Birthdate->GetValue();
    wxString dateStr = date.Format(wxGetTranslation("%Y%m%d"));
    value = (const char*)dateStr.mb_str(wxConvUTF8);
    ::boost::gregorian::date birthdate ( ::boost::gregorian::from_undelimited_string( value ) );
    ::boost::posix_time::time_duration td;
    m_pPatient->setCRefBirthdate( ::boost::posix_time::ptime( birthdate, td ) );

    value = m_Sex->GetStringSelection().mb_str();
    OSLM_DEBUG("Sex of patient = "<<boost::lexical_cast< char >(value));
    m_pPatient->setCRefIsMale(boost::lexical_cast< char >(value) == 'M');

    value = m_IDDicom->GetValue().mb_str();
    m_pPatient->setCRefIDDicom(value);

    //value = wxStringToString(m_DbID->GetValue());
    //m_pPatient->setDbID(boost::lexical_cast< int >(value));
}

//------------------------------------------------------------------------------

void PatientEditorDialog::exportStudyData()
{
    std::string value;

    value = wx2std(m_Hospital->GetValue());
    OSLM_DEBUG("m_Hospital export : " << value);
    m_pStudy->setCRefHospital(value);

    value = wx2std(m_Modality->GetStringSelection());
    OSLM_DEBUG("Modality export : " << value);
    m_pStudy->setCRefModality(value);

//  value = wxStringToString(m_AcquisitionZone->GetValue());
//  m_pStudy->setAcquisitionZone(value);
    std::string zone = "";
    value = wx2std(m_AcquisitionZone_zone->GetStringSelection());
    zone += value + "_";
    value = wx2std(m_AcquisitionZone_organ->GetStringSelection());
    zone += value + "_";
    value = wx2std(m_AcquisitionZone_injection->GetStringSelection());
    zone += value;
    OSLM_DEBUG("AcquisitionZone export : " << zone);
    m_pStudy->setCRefAcquisitionZone(zone);

    value = wx2std(m_UID->GetValue());

    m_pStudy->setCRefUID(value);

    //value = wxStringToString(m_DbIDS->GetValue());
    //m_pPatient->setDbIDS(boost::lexical_cast< int >(value));

    value = wx2std(m_RISID->GetValue());
    m_pStudy->setCRefRISId(value);
}

//------------------------------------------------------------------------------

void PatientEditorDialog::exportAcquisitionData()
{
    std::string value;

    value = m_LaboID->GetValue().mb_str();
    long laboId = 0;
    m_LaboID->GetValue().ToLong( &laboId );
    m_pAcquisition->setLaboID( laboId );

    wxDateTime date = m_Date->GetValue();
    wxString dateStr = date.Format(wxGetTranslation("%Y%m%d"));
//  value = (const char*)dateStr.mb_str(wxConvUTF8);
    value = wx2std(dateStr);
    ::boost::gregorian::date birthdate ( ::boost::gregorian::from_undelimited_string( value ) );
    ::boost::posix_time::time_duration td;
    m_pAcquisition->setCRefCreationDate( ::boost::posix_time::ptime( birthdate, td ) );

    //value = m_Time->GetValue().mb_str();
    //m_pAcquisition->setCRefTime(value);

    value = m_ImageFormat->GetValue().mb_str();
    m_pAcquisition->setCRefImageFormat(value);
}

//------------------------------------------------------------------------------

void PatientEditorDialog::onOk( wxCommandEvent & event )
{
    exportData();
    event.Skip();
}

//------------------------------------------------------------------------------

wxString PatientEditorDialog::intToWxString( int value )
{
    std::string val = boost::lexical_cast< std::string >(value);
    return std2wx(val);
}

//------------------------------------------------------------------------------

wxString PatientEditorDialog::stringToWxString( const std::string & value )
{
//  wxCSConv cust(_T("C"));
//  return wxString( value.c_str() , cust );
    return std2wx(value);
    //wxString( value.c_str() , wxConvUTF8 );
}

//------------------------------------------------------------------------------

std::string PatientEditorDialog::wxStringToString( const wxString & value )
{
//  wxCSConv cust(_T("C"));
//  return (const char*)value.mb_str(cust);
//  return (const char*)value.mb_str(wxConvUTF8);
    return wx2std(value);
    //(const char*)value.mb_str();
}

//------------------------------------------------------------------------------

wxString PatientEditorDialog::boolToWxString( bool value )
{
    std::string val = boost::lexical_cast< std::string >(value);
    return std2wx(val);
}

//------------------------------------------------------------------------------

wxDateTime::Month PatientEditorDialog::convertMonth(int month)
{
    switch(month) {
        case 2 : return wxDateTime::Feb;
        case 3 : return wxDateTime::Mar;
        case 4 : return wxDateTime::Apr;
        case 5 : return wxDateTime::May;
        case 6 : return wxDateTime::Jun;
        case 7 : return wxDateTime::Jul;
        case 8 : return wxDateTime::Aug;
        case 9 : return wxDateTime::Sep;
        case 10 : return wxDateTime::Oct;
        case 11 : return wxDateTime::Nov;
        case 12 : return wxDateTime::Dec;
        default :return wxDateTime::Jan;
    }
}

//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE( PatientEditorDialog, wxDialog )
EVT_BUTTON( wxID_OK, PatientEditorDialog::onOk )
END_EVENT_TABLE()

//==============================================================================
// PatientEditor::showModalDialog
//==============================================================================

const bool PatientEditor::showModalDialog(  wxWindow * parent,
                                            ::boost::shared_ptr< ::fwData::Patient > _pPatient,
                                            ::boost::shared_ptr< ::fwData::Study > _pStudy,
                                            ::boost::shared_ptr< ::fwData::Acquisition > _pAcquisition)
{

    PatientEditorDialog dialog(parent,_pPatient,_pStudy,_pAcquisition);

    // Shows the dialog and return the user's choice.
    if( dialog.ShowModal() == wxID_OK )
    {
        return true;
    }
    else
    {
        return false;
    }
}


} // namespace fwWX
