/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>

#include <fwData/PatientDB.hpp>

#include <fwTools/Factory.hpp>

#include <fwServices/Base.hpp>

#include <fwComEd/fieldHelper/BackupHelper.hpp>
#include <fwComEd/fieldHelper/MedicalImageHelpers.hpp>

#include <fwGui/dialog/MessageDialog.hpp>

#include "uiPatientDB/action/CloneImage.hpp"

namespace uiPatientDB
{
namespace action
{

REGISTER_SERVICE( ::fwGui::IActionSrv , ::uiPatientDB::action::CloneImage , ::fwData::PatientDB ) ;

//------------------------------------------------------------------------------

CloneImage::CloneImage( ) throw()
{}

//------------------------------------------------------------------------------

CloneImage::~CloneImage() throw()
{}

//------------------------------------------------------------------------------

void CloneImage::info(std::ostream &_sstream )
{
    _sstream << "Action to clone image" << std::endl;
}

//------------------------------------------------------------------------------

void CloneImage::configuring() throw( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();

    this->::fwGui::IActionSrv::initialize();
}

//------------------------------------------------------------------------------

void CloneImage::updating( ) throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    ::fwData::PatientDB::sptr patientDB = this->getObject< ::fwData::PatientDB> ();

    ::fwData::Image::sptr pImage = ::fwComEd::fieldHelper::BackupHelper::getSelectedImage(patientDB);
    if( !pImage )
    {
        ::fwGui::dialog::MessageDialog::showMessageDialog(
                "Clone Image",
                "Sorry, it is impossible to clone image. There are not selected image in the software.",
                ::fwGui::dialog::IMessageDialog::WARNING);
    }
    else
    {
        ::fwData::Image::sptr imageBackup = ::fwComEd::fieldHelper::BackupHelper::backupSelectedImage(patientDB, this->getSptr());

        // Set image label
        ::fwData::Patient::sptr patient = ::fwComEd::fieldHelper::BackupHelper::getSelectedPatient(patientDB);
        ::fwComEd::fieldHelper::MedicalImageHelpers::setImageLabel(patient, imageBackup);
    }
}

//------------------------------------------------------------------------------

void CloneImage::starting() throw (::fwTools::Failed)
{
    this->::fwGui::IActionSrv::actionServiceStarting();
}

//------------------------------------------------------------------------------

void CloneImage::updating( ::fwServices::ObjectMsg::csptr _msg ) throw (::fwTools::Failed)
{}

//------------------------------------------------------------------------------

void CloneImage::stopping() throw (::fwTools::Failed)
{
    this->::fwGui::IActionSrv::actionServiceStopping();
}

//------------------------------------------------------------------------------

} // namespace action
} // namespace uiPatientDB
