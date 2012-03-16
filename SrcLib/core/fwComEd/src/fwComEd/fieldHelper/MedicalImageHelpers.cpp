/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwData/Integer.hpp>
#include <fwData/Composite.hpp>
#include <fwData/String.hpp>
#include <fwData/PointList.hpp>
#include <fwData/StandardBuffer.hpp>
#include <fwData/ResectionDB.hpp>

#include <fwMath/MeshFunctions.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/IEditionService.hpp>

#include "fwComEd/PatientDBMsg.hpp"
#include "fwComEd/Dictionary.hpp"
#include "fwComEd/fieldHelper/MedicalImageHelpers.hpp"

namespace fwComEd
{

namespace fieldHelper
{

//------------------------------------------------------------------------------

bool MedicalImageHelpers::checkLandmarks( ::fwData::Image::sptr _pImg )
{
    bool fieldIsModified = false;

    // Manage image landmarks
    if ( ! _pImg->getField_NEWAPI( ::fwComEd::Dictionary::m_imageLandmarksId ) )
    {
        ::fwData::PointList::NewSptr pl;
        _pImg->setField_NEWAPI( ::fwComEd::Dictionary::m_imageLandmarksId, pl );
        fieldIsModified = true;
    }

    return fieldIsModified;
}

//------------------------------------------------------------------------------

bool MedicalImageHelpers::checkImageValidity( ::fwData::Image::sptr _pImg )
{
    SLM_TRACE_FUNC();

    // Test if the image is allocated
    bool dataImageIsAllocated = (_pImg != ::fwData::Image::sptr());

    if (dataImageIsAllocated)
    {
        size_t nbDim = _pImg->getNumberOfDimensions();
        dataImageIsAllocated &= nbDim > 1;

        for ( size_t k = 0; dataImageIsAllocated && k < nbDim; ++k )
        {
            if(k == 2 && nbDim == 3) // special test for 2D jpeg image (size[2] == 1)
            {
                dataImageIsAllocated = dataImageIsAllocated && ( _pImg->getSize()[k] >= 1 );
            }
            else
            {
                dataImageIsAllocated = dataImageIsAllocated && ( _pImg->getSize()[k] != 0 && ( _pImg->getSize()[k] != 1 ) );
            }
        }
    }

    return dataImageIsAllocated;
}

//------------------------------------------------------------------------------

bool MedicalImageHelpers::checkImageSliceIndex( ::fwData::Image::sptr _pImg )
{
    SLM_ASSERT("_pImg pointer null", _pImg);

    bool fieldIsModified = false;

    const ::fwData::Image::SizeType &imageSize = _pImg->getSize();

    ::fwData::Integer::sptr axialIdx    = _pImg->getField_NEWAPI< ::fwData::Integer >( ::fwComEd::Dictionary::m_axialSliceIndexId );
    ::fwData::Integer::sptr frontalIdx  = _pImg->getField_NEWAPI< ::fwData::Integer >( ::fwComEd::Dictionary::m_frontalSliceIndexId);
    ::fwData::Integer::sptr sagittalIdx = _pImg->getField_NEWAPI< ::fwData::Integer >( ::fwComEd::Dictionary::m_sagittalSliceIndexId );

    // Manage image landmarks
    if ( ! (axialIdx && frontalIdx && sagittalIdx) )
    {
        // Set value
        axialIdx = ::fwData::Integer::New(-1);
        _pImg->setField_NEWAPI( ::fwComEd::Dictionary::m_axialSliceIndexId, axialIdx );

        frontalIdx = ::fwData::Integer::New(-1);
        _pImg->setField_NEWAPI( ::fwComEd::Dictionary::m_frontalSliceIndexId, frontalIdx );

        sagittalIdx = ::fwData::Integer::New(-1);
        _pImg->setField_NEWAPI( ::fwComEd::Dictionary::m_sagittalSliceIndexId, sagittalIdx );

        fieldIsModified = true;
    }


    SLM_ASSERT (
            "Information on image slice index is not correct, miss one of these fields : m_axialSliceIndexId, m_frontalSliceIndexId, m_sagittalSliceIndexId.",
            axialIdx && frontalIdx && sagittalIdx
            );

    // Get value
    if( axialIdx->value() < 0 ||  imageSize[2] < axialIdx->value() )
    {
        axialIdx->value() = imageSize[2] / 2;
        fieldIsModified = true;
    }

    if( frontalIdx->value() < 0 ||  imageSize[1] < frontalIdx->value() )
    {
        frontalIdx->value() = imageSize[1] / 2;
        fieldIsModified = true;
    }

    if( sagittalIdx->value() < 0 ||  imageSize[0] < sagittalIdx->value() )
    {
        sagittalIdx->value() = imageSize[0] / 2;
        fieldIsModified = true;
    }


    return fieldIsModified;
}

//------------------------------------------------------------------------------

::fwData::Point::sptr   MedicalImageHelpers::getImageSliceIndices( ::fwData::Image::sptr _pImg )
{
    SLM_ASSERT("_pImg pointer null", _pImg);

    ::fwData::Point::NewSptr point;

    MedicalImageHelpers::checkImageSliceIndex(_pImg);

    point->getRefCoord()[0] = _pImg->getField_NEWAPI< ::fwData::Integer >( ::fwComEd::Dictionary::m_sagittalSliceIndexId )->value();
    point->getRefCoord()[1] = _pImg->getField_NEWAPI< ::fwData::Integer >( ::fwComEd::Dictionary::m_frontalSliceIndexId  )->value();
    point->getRefCoord()[2] = _pImg->getField_NEWAPI< ::fwData::Integer >( ::fwComEd::Dictionary::m_axialSliceIndexId    )->value();

    return point;
}

//------------------------------------------------------------------------------

bool MedicalImageHelpers::checkComment( ::fwData::Image::sptr _pImg )
{
    SLM_ASSERT("_pImg pointer null", _pImg);

    bool fieldIsModified = false;

    if ( ! _pImg->getField_NEWAPI( ::fwComEd::Dictionary::m_commentId ) )
    {
        // Set value
        ::fwData::String::NewSptr param("Original image");
        _pImg->setField_NEWAPI( ::fwComEd::Dictionary::m_commentId, param );
        fieldIsModified = true;
    }

    return fieldIsModified;
}

//------------------------------------------------------------------------------

void MedicalImageHelpers::setImageLabel( ::fwData::Patient::sptr pPatient, ::fwData::Image::sptr pImage)
{
    SLM_ASSERT("pPatient pointer null", pPatient);
    SLM_ASSERT("pImage pointer null", pImage);

    std::stringstream label;
    ::fwData::Integer::sptr intField = pPatient->setDefaultField_NEWAPI( ::fwComEd::Dictionary::m_acquisitionCountId , ::fwData::Integer::New(0));
    label << "I" << intField->value();
    ++(intField->value());

    ::fwData::String::NewSptr labelField(label.str());
    pImage->setField_NEWAPI(::fwComEd::Dictionary::m_imageLabelId, labelField);
}

//------------------------------------------------------------------------------

::fwData::Image::sptr MedicalImageHelpers::initialize( ::fwData::Image::sptr imgSrc, ::fwData::Image::sptr imgToInitialize)
{
    SLM_ASSERT("Image source must be initialized", imgSrc);
    SLM_ASSERT("Image source must be valid", MedicalImageHelpers::checkImageValidity(imgSrc));

    if(!imgToInitialize)
    {
        imgToInitialize = ::fwData::Image::New();
    }
    ::fwData::Array::sptr imgData = imgSrc->getDataArray();
    imgSrc->setDataArray(::fwData::Array::sptr());

    imgToInitialize->deepCopy(imgSrc);

    imgSrc->setDataArray(imgData);

    imgToInitialize->allocate();

    return imgToInitialize;
}

//------------------------------------------------------------------------------

void MedicalImageHelpers::mergePatientDBInfo( ::fwData::PatientDB::sptr _patientDBFrom, ::fwData::PatientDB::sptr _patientDBTo, ::fwServices::IService::sptr _msgSender )
{
    // Add new patient DB to patient DB container

    // Test if the patient db contain patient
    bool hasOldPatients = ! _patientDBTo->getPatients().empty();
    bool hasNewPatients = ! _patientDBFrom->getPatients().empty();

    int index = 0;

    BOOST_FOREACH( ::fwData::Patient::sptr patient, _patientDBFrom->getPatients() )
    {
        BOOST_FOREACH( ::fwData::Study::sptr study, patient->getStudies() )
        {
            BOOST_FOREACH( ::fwData::Acquisition::sptr acq, study->getAcquisitions() )
            {
                BOOST_FOREACH( ::fwData::Reconstruction::sptr reconstruction, acq->getReconstructions() )
                {
                    ::fwData::Image::sptr image;
                    reconstruction->setImage( image );
                }
            }
        }

        bool patientExist = false;
        index = 0;
        BOOST_FOREACH( ::fwData::Patient::sptr oldPatient, _patientDBTo->getPatients() )
        {
            if (    patient->getName()      == oldPatient->getName() &&
                    patient->getFirstname() == oldPatient->getFirstname() &&
                    patient->getIsMale()    == oldPatient->getIsMale() )
            {
                patientExist = true;
                MedicalImageHelpers::mergeInformation(oldPatient,patient);
                break;
            }
            ++index;
        }
        if ( !patientExist )
        {
            _patientDBTo->addPatient( patient );
        }

    }


    if( hasNewPatients )
    {
        // Notify modifications
        ::fwComEd::PatientDBMsg::NewSptr msg;
        if( hasOldPatients )
        {
            msg->addEvent(::fwComEd::PatientDBMsg::ADD_PATIENT, ::fwData::Integer::New(index));
            msg->addEvent(::fwComEd::PatientDBMsg::NEW_LOADED_PATIENT);
        }
        else
        {
            msg->addEvent(::fwComEd::PatientDBMsg::NEW_PATIENT);
            msg->addEvent(::fwComEd::PatientDBMsg::NEW_LOADED_PATIENT);
        }
        ::fwServices::IEditionService::notify( _msgSender, _patientDBTo, msg);
    }
}

//------------------------------------------------------------------------------

void MedicalImageHelpers::mergeInformation(::fwData::Patient::sptr currentPatient, ::fwData::Patient::sptr importedPatient)
{

    BOOST_FOREACH( ::fwData::Study::sptr study, importedPatient->getStudies() )
    {
        currentPatient->addStudy(study);
    }

    ::fwData::Composite::sptr itool = ::fwData::Composite::dynamicCast(importedPatient->getTool(::fwComEd::Dictionary::m_resectionId));
    ::fwData::Composite::sptr ctool = ::fwData::Composite::dynamicCast(currentPatient->getTool(::fwComEd::Dictionary::m_resectionId) );

    //merge toolbox

    // dictionary to maintain OperationName in case of duplicate name.
    // these names are used in IDToolsBox::composite[m_resectionId] and in ID_SCENARIO::composite["ResectionAcquisitionPair"]
    std::map< std::string , std::string > ResctionOpNameTranslate;

    if ( itool )
    {
        if (!ctool )
        {
            currentPatient->addTool(::fwComEd::Dictionary::m_resectionId, itool );
        }
        else
        {
            // copy data of itool inside ctool map
            ::fwData::Composite::Container::iterator itoolIter;
            for (  itoolIter = itool->getRefMap().begin() ;  itoolIter != itool->getRefMap().end();  ++itoolIter )
            {
                std::string opName = itoolIter->first;
                ::fwData::ResectionDB::sptr resectionDB = ::fwData::ResectionDB::dynamicCast( itoolIter->second );
                SLM_ASSERT(" in Composite of Resection DB invalid Data Type", resectionDB );

                std::string newOpName = opName;
                while (  ctool->getRefMap().find( newOpName ) !=  ctool->getRefMap().end()  )
                {
                    newOpName += "BIS";
                }
                ctool->getRefMap()[newOpName] = resectionDB;
                ResctionOpNameTranslate[ opName ] = newOpName;
            }
        }
    }
    // merge scenario
    ::fwData::Composite::sptr iscenario = ::fwData::Composite::dynamicCast( importedPatient->getScenario("ResectionAcquisitionPair"));
    ::fwData::Composite::sptr cscenario = ::fwData::Composite::dynamicCast( currentPatient->getScenario("ResectionAcquisitionPair"));

    if ( iscenario )
    {
        if ( !cscenario )
        {
            currentPatient->addScenario("ResectionAcquisitionPair", iscenario );
        }
        else
        {
            // copy data of iscenario inside cscenario map
            ::fwData::Composite::Container::iterator iscenarioIter;
            for (  iscenarioIter = iscenario->getRefMap().begin() ;  iscenarioIter != iscenario->getRefMap().end();  ++iscenarioIter )
            {
                std::string opName = iscenarioIter->first;
                ::fwData::Acquisition::sptr acquisition = ::fwData::Acquisition::dynamicCast( iscenarioIter->second );
                SLM_ASSERT(" in Composite of Acquisition invalid Data Type", acquisition );

                std::string newOpName = ResctionOpNameTranslate[ opName ];
                cscenario->getRefMap()[newOpName] = acquisition;
            }
        }
    }
}

//------------------------------------------------------------------------------

bool MedicalImageHelpers::isBufNull(const ::fwData::Image::BufferType *buf, const unsigned int len)
{
    bool isNull;
    const ::fwData::Image::BufferType *ucbuf = static_cast< const ::fwData::Image::BufferType *> (buf);
    isNull = 0 == std::accumulate(
            ucbuf,
            ucbuf+len,
            0,
            bitwise_or< ::fwData::Image::BufferType >()
            );
    return isNull;
}

//------------------------------------------------------------------------------

} // fieldHelper
} // fwComEd
