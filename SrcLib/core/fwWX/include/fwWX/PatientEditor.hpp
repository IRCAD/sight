/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWWX_PATIENTEDITOR_HPP_
#define _FWWX_PATIENTEDITOR_HPP_

#include <string>

#include <wx/datectrl.h>

#include "fwWX/config.hpp"

class wxWindow;

namespace fwData
{
    class PatientDB;
    class Patient;
    class Study;
    class Acquisition;
}

namespace fwWX
{

/**
 * @brief   Implements a class providing a dialog box to edit patient properties.
 * @class   PatientEditorDialog.
 * @author  IRCAD (Research and Development Team)
 * @date    2009.
 * @note This class is not commented, because it is not used in the open framework tutorial.
 * @todo PatientEditorDialog is not commented.
 */
class FWWX_CLASS_API PatientEditorDialog : public wxDialog
{
    public :

        FWWX_API PatientEditorDialog(   wxWindow * parent,
                                ::boost::shared_ptr< ::fwData::Patient > _pPatient,
                                ::boost::shared_ptr< ::fwData::Study > _pStudy,
                                ::boost::shared_ptr< ::fwData::Acquisition > _pAcquisition );

        FWWX_API ~PatientEditorDialog();

    private :

        /**
        * @name  wxWidget creation
        */
        //@{

        void createContent();

        wxSizer * createPatientSizer();

        wxSizer * createStudySizer();

        wxSizer * createAcquisitionSizer();

        //@}

        /**
        * @name  Data import
        */
        //@{

        void importData();

        void importPatientData();

        void importStudyData();

        void importAcquisitionData();

        void disableFieldEdition();

        //@}

        /**
        * @name  Data export
        */
        //@{

        void exportData();

        void exportPatientData();

        void exportStudyData();

        void exportAcquisitionData();

        //@}

        /**
        * @name  Event managment
        */
        //@{

        void onOk( wxCommandEvent & event );

        //@}

        /**
        * @name  Convertion method
        */
        //@{

        std::string wxStringToString(const wxString&);

        wxString intToWxString( int value );

        wxString stringToWxString( const std::string & value );

        wxString boolToWxString( bool value );

        wxDateTime::Month convertMonth(int month);

        //@}

    private :

        /**
        * @name  Patient data
        */
        //@{

        wxTextCtrl *        m_Name;
        wxTextCtrl *        m_Firstname;
        wxDatePickerCtrl *  m_Birthdate;
        wxChoice *          m_Sex;
        wxTextCtrl *        m_IDDicom;
        wxTextCtrl *        m_DbID;

        //@}

        /**
        * @name  Study data
        */
        //@{

        wxTextCtrl *    m_Hospital;
        wxChoice *      m_Modality;
        //wxTextCtrl *  m_AcquisitionZone;
        wxChoice *      m_AcquisitionZone_zone;
        wxChoice *      m_AcquisitionZone_organ;
        wxChoice *      m_AcquisitionZone_injection;
        wxTextCtrl *    m_UID;
        wxTextCtrl *    m_DbIDS;
        wxTextCtrl *    m_RISID;

        /**
        * @name  Acqusition data
        */
        //@{

        wxTextCtrl *        m_LaboID;
        wxDatePickerCtrl *  m_Date;
        wxTextCtrl *        m_Time;
        wxTextCtrl *        m_Index;
        wxTextCtrl *        m_ImageType;
        wxTextCtrl *        m_ImageFormat;
        wxTextCtrl *        m_Width;
        wxTextCtrl *        m_Height;
        wxTextCtrl *        m_SliceThickness;
        wxTextCtrl *        m_VoxelSize;
        wxTextCtrl *        m_Axe;
        wxTextCtrl *        m_FrameNumber;
        wxTextCtrl *        m_BitsPerPixel;
        wxTextCtrl *        m_DbIDA;

        //@}

        ::boost::shared_ptr< ::fwData::Patient > m_pPatient;
        ::boost::shared_ptr< ::fwData::Study > m_pStudy;
        ::boost::shared_ptr< ::fwData::Acquisition > m_pAcquisition;

        DECLARE_EVENT_TABLE()
};



struct PatientEditor
{

    /**
     * @brief   Edits patient properties using a dialog box.
     *
     * @param   parent  a pointer to a window that will be the parent of the dialog
     *
     * @return  a bool telling if patient properties have been updated
     */
    static const bool showModalDialog(  wxWindow * parent,
                                        ::boost::shared_ptr< ::fwData::Patient > _pPatient,
                                        ::boost::shared_ptr< ::fwData::Study > _pStudy,
                                        ::boost::shared_ptr< ::fwData::Acquisition > _pAcquisition );

};

} // namespace fwWX



#endif /*_FWWX_PATIENTEDITOR_HPP_*/
