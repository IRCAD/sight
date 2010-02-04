/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>

#include <fwData/Object.hpp>
#include <fwData/Integer.hpp>
#include <fwData/Composite.hpp>
#include <fwData/TransfertFunction.hpp>
#include <fwData/String.hpp>

#include <fwServices/helper.hpp>
#include <fwServices/IService.hpp>

#include "fwComEd/Dictionary.hpp"
#include "fwComEd/PatientDBMsg.hpp"
#include "fwComEd/fieldHelper/MedicalImageHelpers.hpp"
#include "fwComEd/fieldHelper/BackupHelper.hpp"


namespace fwComEd
{

namespace fieldHelper
{

//-----------------------------------------------------------------------------

::fwData::Image::sptr BackupHelper::getSelectedImage(::fwData::PatientDB::sptr pPatientDB)
{
	::fwData::Image::sptr pImage;

	if(pPatientDB->getFieldSize(fwComEd::Dictionary::m_imageSelectedId))
	{
		pImage = getSelectedAcquisition(pPatientDB)->getImage();
	}

	return pImage;
}

//-----------------------------------------------------------------------------

::fwData::Acquisition::sptr BackupHelper::getSelectedAcquisition(::fwData::PatientDB::sptr pPatientDB)
{
	::fwData::Acquisition::sptr pAcq;

	if(pPatientDB->getFieldSize(fwComEd::Dictionary::m_imageSelectedId))
	{
		// Get Selection

		::fwTools::Field::sptr pDataInfo = pPatientDB->getField( fwComEd::Dictionary::m_imageSelectedId );

		/*::fwData::Object::csptr const vSelection = mySpecificMsg->m_modifiedObject;*/
		::fwData::Integer::sptr myIntPat = ::boost::dynamic_pointer_cast< ::fwData::Integer > ( pDataInfo->children().at(0) );
		::fwData::Integer::sptr myIntStu = ::boost::dynamic_pointer_cast< ::fwData::Integer > ( pDataInfo->children().at(1) );
		::fwData::Integer::sptr myIntAcq = ::boost::dynamic_pointer_cast< ::fwData::Integer > ( pDataInfo->children().at(2) );

		// Print the selection (debug)
		OSLM_DEBUG( "myIntPat->value : " << myIntPat->value() );
		OSLM_DEBUG( "myIntStu->value : " << myIntStu->value() );
		OSLM_DEBUG( "myIntAcq->value : " << myIntAcq->value() );

		// Patient selection
		::fwData::PatientDB::PatientIterator patientIter = pPatientDB->getPatients().first;
		patientIter += myIntPat->value();

		// Study selection
		::fwData::Patient::StudyIterator studyIter = (*patientIter)->getStudies().first;
		studyIter += myIntStu->value();

		// Acquisition selection
		::fwData::Study::AcquisitionIterator acquisitionIter = (*studyIter)->getAcquisitions().first;
		acquisitionIter += myIntAcq->value();

		pAcq = (*acquisitionIter);
	}

	return pAcq;
}

//-----------------------------------------------------------------------------

::fwData::Patient::sptr BackupHelper::getSelectedPatient(::fwData::PatientDB::sptr pPatientDB)
{
	::fwData::Patient::sptr pPatient;

	if(pPatientDB->getFieldSize(fwComEd::Dictionary::m_imageSelectedId))
	{
		// Get Selection

		::fwTools::Field::sptr pDataInfo = pPatientDB->getField( fwComEd::Dictionary::m_imageSelectedId );

		/*::fwData::Object::csptr const vSelection = mySpecificMsg->m_modifiedObject;*/
		::fwData::Integer::sptr myIntPat = ::boost::dynamic_pointer_cast< ::fwData::Integer > ( pDataInfo->children().at(0) );

		// Print the selection (debug)
		OSLM_DEBUG( "myIntPat->value : " << myIntPat->value() );

		// Patient selection
		::fwData::PatientDB::PatientIterator patientIter = pPatientDB->getPatients().first;
		patientIter += myIntPat->value();

		pPatient = (*patientIter);
	}

	return pPatient;
}

//-----------------------------------------------------------------------------

bool BackupHelper::backupSelectedImage(::fwData::PatientDB::sptr _pPatientDB, ::fwServices::IService::sptr _MsgSource)
{
	bool bRes = false;
	if( _pPatientDB->getFieldSize(fwComEd::Dictionary::m_imageSelectedId) )
	{
		// Get Selection

		::fwTools::Field::sptr pDataInfo = _pPatientDB->getField( fwComEd::Dictionary::m_imageSelectedId );

		/*::fwData::Object::csptr const vSelection = mySpecificMsg->m_modifiedObject;*/
		::fwData::Integer::sptr myIntPat = ::boost::dynamic_pointer_cast< ::fwData::Integer > ( pDataInfo->children().at(0) );
		::fwData::Integer::sptr myIntStu = ::boost::dynamic_pointer_cast< ::fwData::Integer > ( pDataInfo->children().at(1) );
		::fwData::Integer::sptr myIntAcq = ::boost::dynamic_pointer_cast< ::fwData::Integer > ( pDataInfo->children().at(2) );

		// Print the selection (debug)
		OSLM_DEBUG( "myIntPat->value : " << myIntPat->value() );
		OSLM_DEBUG( "myIntStu->value : " << myIntStu->value() );
		OSLM_DEBUG( "myIntAcq->value : " << myIntAcq->value() );

		// Patient selection
		::fwData::PatientDB::PatientIterator patientIter = _pPatientDB->getPatients().first;
		patientIter += myIntPat->value();

		// Study selection
		::fwData::Patient::StudyIterator studyIter = (*patientIter)->getStudies().first;
		studyIter += myIntStu->value();

		// Acquisition selection
		::fwData::Study::AcquisitionIterator acquisitionIter = (*studyIter)->getAcquisitions().first;
		acquisitionIter += myIntAcq->value();

		// Image backup
		//assert(false && "TODO : clone need backup of all temp field (medical image)");
		::fwData::Acquisition::sptr pAquisitionBackup = (*acquisitionIter)->clone();
		::fwData::Image::sptr pImageBackup = pAquisitionBackup->getImage();

		if ( (*acquisitionIter)->getImage()->getFieldSize( ::fwComEd::Dictionary::m_transfertFunctionCompositeId ) )
		{
			::fwComEd::fieldHelper::MedicalImageHelpers::checkMinMaxTF( pImageBackup );
			::fwComEd::fieldHelper::MedicalImageHelpers::checkImageSliceIndex( pImageBackup );
			::fwComEd::fieldHelper::MedicalImageHelpers::checkLandmarks( pImageBackup );

			// clone TF
			::fwData::Composite::sptr cTF = (*acquisitionIter)->getImage()->getFieldSingleElement< ::fwData::Composite >( ::fwComEd::Dictionary::m_transfertFunctionCompositeId );
			::fwData::String::sptr sTF = (*acquisitionIter)->getImage()->getFieldSingleElement< ::fwData::String >( ::fwComEd::Dictionary::m_transfertFunctionId );

			::fwData::Composite::sptr cTFBackup = ::fwData::Composite::New();
			::fwData::String::sptr sTFBackup = ::fwData::String::New();

			::fwData::Composite::Container::iterator iterTF;
			for (iterTF = cTF->getRefMap().begin() ; iterTF != cTF->getRefMap().end() ; ++iterTF)
			{
				cTFBackup->getRefMap()[iterTF->first] = ::fwData::TransfertFunction::dynamicCast(iterTF->second)->clone();
			}
			sTFBackup->value() = sTF->value();

			pImageBackup->setFieldSingleElement(::fwComEd::Dictionary::m_transfertFunctionCompositeId, cTFBackup);
			pImageBackup->setFieldSingleElement(::fwComEd::Dictionary::m_transfertFunctionId, sTFBackup);

			// clone min/max
			int min = (*acquisitionIter)->getImage()->getFieldSingleElement< ::fwData::Integer >(::fwComEd::Dictionary::m_windowMinId)->value();
			pImageBackup->getFieldSingleElement< ::fwData::Integer >(::fwComEd::Dictionary::m_windowMinId)->value() = min;

			int max = (*acquisitionIter)->getImage()->getFieldSingleElement< ::fwData::Integer >(::fwComEd::Dictionary::m_windowMaxId)->value();
			pImageBackup->getFieldSingleElement< ::fwData::Integer >(::fwComEd::Dictionary::m_windowMaxId)->value() = max;

			// clone landmarks ???
			// m�me question pour les distances ...
		}
		// clone axial slice index
		if ((*acquisitionIter)->getImage()->getFieldSize( ::fwComEd::Dictionary::m_axialSliceIndexId ) )
		{
			int axialSliceIndex = (*acquisitionIter)->getImage()->getFieldSingleElement< ::fwData::Integer >(::fwComEd::Dictionary::m_axialSliceIndexId)->value();
			pImageBackup->getFieldSingleElement< ::fwData::Integer >(::fwComEd::Dictionary::m_axialSliceIndexId)->value() = axialSliceIndex;
		}
		// clone frontal slice inde
		if ((*acquisitionIter)->getImage()->getFieldSize( ::fwComEd::Dictionary::m_frontalSliceIndexId ) )
		{
			int frontalSliceIndex = (*acquisitionIter)->getImage()->getFieldSingleElement< ::fwData::Integer >(::fwComEd::Dictionary::m_frontalSliceIndexId)->value();
			pImageBackup->getFieldSingleElement< ::fwData::Integer >(::fwComEd::Dictionary::m_frontalSliceIndexId)->value() = frontalSliceIndex;
		}
		// clone sagittal slice inde
		if ((*acquisitionIter)->getImage()->getFieldSize( ::fwComEd::Dictionary::m_sagittalSliceIndexId ) )
		{
			int sagittalSliceIndex = (*acquisitionIter)->getImage()->getFieldSingleElement< ::fwData::Integer >(::fwComEd::Dictionary::m_sagittalSliceIndexId)->value();
			pImageBackup->getFieldSingleElement< ::fwData::Integer >(::fwComEd::Dictionary::m_sagittalSliceIndexId)->value() = sagittalSliceIndex;
		}
		// clone comment
		if ((*acquisitionIter)->getImage()->getFieldSize( ::fwComEd::Dictionary::m_commentId ) )
		{
			std::string comment = (*acquisitionIter)->getImage()->getFieldSingleElement< ::fwData::String >( ::fwComEd::Dictionary::m_commentId )->value();
			::fwData::String::NewSptr commentField;
			commentField->value() =	comment;
			pImageBackup->setFieldSingleElement( ::fwComEd::Dictionary::m_commentId, commentField);
		}
		// clone label
		if ((*acquisitionIter)->getImage()->getFieldSize( ::fwComEd::Dictionary::m_imageLabelId ) )
		{
			std::string label = (*acquisitionIter)->getImage()->getFieldSingleElement< ::fwData::String >( ::fwComEd::Dictionary::m_imageLabelId )->value();
			::fwData::String::NewSptr labelField;
			labelField->value() =	label;
			pImageBackup->setFieldSingleElement( ::fwComEd::Dictionary::m_imageLabelId, labelField);
		}

		(*studyIter)->addAcquisition( pAquisitionBackup );
		bRes = true;

		// Fire Event
		::fwComEd::PatientDBMsg::NewSptr msg;
		msg->addEvent( ::fwComEd::PatientDBMsg::ADD_PATIENT );

		::fwServices::IEditionService::notify(_MsgSource, _pPatientDB, msg);
	}
	return bRes;
}

//------------------------------------------------------------------------------


} // fieldHelper

} // fwComEd
