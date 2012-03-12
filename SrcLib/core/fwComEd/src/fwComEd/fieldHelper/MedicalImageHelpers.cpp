/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwData/Integer.hpp>
#include <fwData/TransfertFunction.hpp>
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

std::pair<bool,bool> MedicalImageHelpers::checkMinMaxTF( ::fwData::Image::sptr _pImg )
{
    std::pair<bool,bool> fieldsAreModified (false,false);

    // Windowing
    int windowMin;
    int windowMax;
    if (    ! _pImg->getFieldSize( ::fwComEd::Dictionary::m_windowMinId ) ||
            ! _pImg->getFieldSize( ::fwComEd::Dictionary::m_windowMaxId ) )
    {
        ::fwData::Integer::NewSptr minField;
        ::fwData::Integer::NewSptr maxField;

        if ( _pImg->getWindowWidth() >= 10 ) // because of TF management
        {
            minField->value() = _pImg->getWindowCenter() - _pImg->getWindowWidth() / 2.0;
            maxField->value() = _pImg->getWindowCenter() + _pImg->getWindowWidth() / 2.0;
        }
        else
        {
            minField->value() = -200;
            maxField->value() = 300;
            _pImg->setWindowCenter(50);
            _pImg->setWindowWidth(500);
        }
        windowMin = minField->value();
        windowMax = maxField->value();

        // Set in selected image
        _pImg->setFieldSingleElement(::fwComEd::Dictionary::m_windowMinId, minField);
        _pImg->setFieldSingleElement(::fwComEd::Dictionary::m_windowMaxId, maxField);

        // Min / Max is modified
        fieldsAreModified.first = true;
    }
    else
    {
        ::fwData::Integer::sptr min = _pImg->getFieldSingleElement< ::fwData::Integer >( fwComEd::Dictionary::m_windowMinId );
        ::fwData::Integer::sptr max = _pImg->getFieldSingleElement< ::fwData::Integer >( fwComEd::Dictionary::m_windowMaxId );
        windowMin = min->value();
        windowMax = max->value();
    }


    // Transfert functions
    if ( ! _pImg->getFieldSize( ::fwComEd::Dictionary::m_transfertFunctionCompositeId ) )
    {
        // ACH => Normaly, if min max does not exist, TF must does not exist.
        SLM_ASSERT("Min is not modified", fieldsAreModified.first );

        ::fwData::TransfertFunction::sptr tf = ::fwData::TransfertFunction::createDefaultTransfertFunction(_pImg);
        tf->setMinMax(windowMin, windowMax);

        ::fwData::String::NewSptr tfId;
        tfId->value() = ::fwData::TransfertFunction::defaultTransfertFunctionName;
        ::fwData::Composite::NewSptr cTF;
        cTF->getRefMap()[tfId->value()] = tf;

        // Set in selected image
        _pImg->setFieldSingleElement(::fwComEd::Dictionary::m_transfertFunctionCompositeId, cTF);
        _pImg->setFieldSingleElement(::fwComEd::Dictionary::m_transfertFunctionId, tfId);

        // TF is modified
        fieldsAreModified.second = true;
    }
    else
    {
        // ACH => Normaly, if min max does not exist, TF must does not exist.
        SLM_ASSERT("Min is modified", ! fieldsAreModified.first );

        ::fwData::Composite::sptr cTransfertFunction = _pImg->getFieldSingleElement< ::fwData::Composite >( ::fwComEd::Dictionary::m_transfertFunctionCompositeId );
        ::fwData::String::sptr sTransfertFunction = _pImg->getFieldSingleElement< ::fwData::String >( ::fwComEd::Dictionary::m_transfertFunctionId );
        ::fwData::TransfertFunction::sptr pTF = ::fwData::TransfertFunction::dynamicCast( cTransfertFunction->getRefMap()[sTransfertFunction->value()] );

        std::pair< ::boost::int32_t, ::boost::int32_t > currentMinMax = pTF->getMinMax();

        // ACH => Normaly this case does not exit in the current framework
        OSLM_ASSERT("Current TransfertFunction Min Max are out of bounds ["<<windowMin<<", "<<windowMax<<"] != [" <<currentMinMax.first<<", "<<currentMinMax.second<<"]",
                windowMin == currentMinMax.first && windowMax == currentMinMax.second );
        if ( windowMin != currentMinMax.first || windowMax != currentMinMax.second )
        {
            pTF->setMinMax(windowMin, windowMax);

            // TF is modified
            fieldsAreModified.second = true;
        }
    }

    return fieldsAreModified;
}

//------------------------------------------------------------------------------

void MedicalImageHelpers::updateMinMaxFromTF( ::fwData::Integer::sptr min, ::fwData::Integer::sptr max, ::fwData::TransfertFunction::sptr tF)
{
    ::fwData::TransfertFunction::MinMaxType currentMinMax = tF->getMinMax();
    min->value() = currentMinMax.first;
    max->value() = currentMinMax.second;
}

//------------------------------------------------------------------------------

void MedicalImageHelpers::updateMinMaxFromTF( ::fwData::Image::sptr _pImg )
{
    assert( _pImg->getFieldSize( ::fwComEd::Dictionary::m_windowMinId ) &&
            _pImg->getFieldSize( ::fwComEd::Dictionary::m_windowMaxId ) &&
            _pImg->getFieldSize( ::fwComEd::Dictionary::m_transfertFunctionCompositeId ) );

    ::fwData::Integer::sptr minField = _pImg->getFieldSingleElement< ::fwData::Integer >( ::fwComEd::Dictionary::m_windowMinId );
    ::fwData::Integer::sptr maxField = _pImg->getFieldSingleElement< ::fwData::Integer >( ::fwComEd::Dictionary::m_windowMaxId );

    ::fwData::Composite::sptr cTransfertFunction = _pImg->getFieldSingleElement< ::fwData::Composite >( ::fwComEd::Dictionary::m_transfertFunctionCompositeId );
    ::fwData::String::sptr sTransfertFunction = _pImg->getFieldSingleElement< ::fwData::String >( ::fwComEd::Dictionary::m_transfertFunctionId );
    ::fwData::TransfertFunction::sptr pTF = ::fwData::TransfertFunction::dynamicCast( cTransfertFunction->getRefMap()[sTransfertFunction->value()] );

    updateMinMaxFromTF(minField, maxField, pTF);
}

//------------------------------------------------------------------------------

void MedicalImageHelpers::updateTFFromMinMax( ::fwData::Integer::sptr min, ::fwData::Integer::sptr max, ::fwData::TransfertFunction::sptr tF)
{
    tF->setMinMax( min->value(), max->value() );
}

//------------------------------------------------------------------------------

void MedicalImageHelpers::updateTFFromMinMax( ::fwData::Image::sptr _pImg )
{
    SLM_ASSERT("Image must have a TF composite field", _pImg->getFieldSize( ::fwComEd::Dictionary::m_transfertFunctionCompositeId ) );

    ::fwData::Composite::sptr cTransfertFunction = _pImg->getFieldSingleElement< ::fwData::Composite >( ::fwComEd::Dictionary::m_transfertFunctionCompositeId );
    ::fwData::String::sptr sTransfertFunction = _pImg->getFieldSingleElement< ::fwData::String >( ::fwComEd::Dictionary::m_transfertFunctionId );
    ::fwData::TransfertFunction::sptr pTF = ::fwData::TransfertFunction::dynamicCast( cTransfertFunction->getRefMap()[sTransfertFunction->value()] );

    updateTFFromMinMax(_pImg, pTF);
}

//------------------------------------------------------------------------------

void MedicalImageHelpers::updateTFFromMinMax( ::fwData::Image::sptr _pImg, ::fwData::TransfertFunction::sptr _pTF )
{
    assert( _pImg->getFieldSize( ::fwComEd::Dictionary::m_windowMinId ) &&
            _pImg->getFieldSize( ::fwComEd::Dictionary::m_windowMaxId ) );

    ::fwData::Integer::sptr minField = _pImg->getFieldSingleElement< ::fwData::Integer >( ::fwComEd::Dictionary::m_windowMinId );
    ::fwData::Integer::sptr maxField = _pImg->getFieldSingleElement< ::fwData::Integer >( ::fwComEd::Dictionary::m_windowMaxId );

    updateTFFromMinMax(minField, maxField, _pTF);
}

//------------------------------------------------------------------------------

void MedicalImageHelpers::setBWTF( ::fwData::Image::sptr _pImg, std::string tfSelectionFieldId )
{
    checkMinMaxTF( _pImg );

    ::fwData::Composite::sptr cTransfertFunction = _pImg->getFieldSingleElement< ::fwData::Composite >( ::fwComEd::Dictionary::m_transfertFunctionCompositeId );

    if (tfSelectionFieldId.empty())
    {
        tfSelectionFieldId = ::fwComEd::Dictionary::m_transfertFunctionId;
    }
    else if (!_pImg->getFieldSize(tfSelectionFieldId))
    {
        _pImg->setFieldSingleElement(tfSelectionFieldId, ::fwData::String::New(::fwData::TransfertFunction::defaultTransfertFunctionName));
    }
    ::fwData::String::sptr sTransfertFunction = _pImg->getFieldSingleElement< ::fwData::String >( tfSelectionFieldId );

    // If have not default Transfert Function (BW)
    if( cTransfertFunction->getRefMap().find( ::fwData::TransfertFunction::defaultTransfertFunctionName ) == cTransfertFunction->getRefMap().end() )
    {
        ::fwData::TransfertFunction::sptr tf = ::fwData::TransfertFunction::createDefaultTransfertFunction(_pImg);

        updateTFFromMinMax(_pImg, tf);

        cTransfertFunction->getRefMap()[ ::fwData::TransfertFunction::defaultTransfertFunctionName ] = tf;
        sTransfertFunction->value() = ::fwData::TransfertFunction::defaultTransfertFunctionName;
    }
    else
    {
        sTransfertFunction->value() = ::fwData::TransfertFunction::defaultTransfertFunctionName;
        updateMinMaxFromTF(_pImg);
    }
}


//------------------------------------------------------------------------------

void MedicalImageHelpers::setSquareTF( ::fwData::Image::sptr _pImg, std::string tfSelectionFieldId )
{
    checkMinMaxTF( _pImg );

    ::fwData::Composite::sptr cTransfertFunction = _pImg->getFieldSingleElement< ::fwData::Composite >( ::fwComEd::Dictionary::m_transfertFunctionCompositeId );

    if (tfSelectionFieldId.empty())
    {
        tfSelectionFieldId = ::fwComEd::Dictionary::m_transfertFunctionId;
    }
    else if (!_pImg->getFieldSize(tfSelectionFieldId))
    {
        _pImg->setFieldSingleElement(tfSelectionFieldId, ::fwData::String::New(::fwData::TransfertFunction::defaultTransfertFunctionName));
    }
    ::fwData::String::sptr sTransfertFunction = _pImg->getFieldSingleElement< ::fwData::String >( tfSelectionFieldId );

    ::fwData::TransfertFunction::sptr pTF;
    ::fwData::Composite::iterator iter = (*cTransfertFunction).find( ::fwData::TransfertFunction::squareTransfertFunctionName );

    // If have not quare Transfert Function (BW)
    if( iter == cTransfertFunction->getRefMap().end() )
    {
        pTF = ::fwData::TransfertFunction::New();
        pTF->setCRefName(::fwData::TransfertFunction::squareTransfertFunctionName);

        cTransfertFunction->getRefMap()[ ::fwData::TransfertFunction::squareTransfertFunctionName ] = pTF;
        sTransfertFunction->value() = ::fwData::TransfertFunction::squareTransfertFunctionName;
    }
    else
    {
        pTF = ::fwData::TransfertFunction::dynamicCast(iter->second);
        sTransfertFunction->value() = ::fwData::TransfertFunction::squareTransfertFunctionName;
    }

    pTF->clear();

    ::fwData::Integer::sptr minField = _pImg->getFieldSingleElement< ::fwData::Integer >( ::fwComEd::Dictionary::m_windowMinId );
    ::fwData::Integer::sptr maxField = _pImg->getFieldSingleElement< ::fwData::Integer >( ::fwComEd::Dictionary::m_windowMaxId );

    int min = minField->value();
    int max = maxField->value();
    ::fwData::Color::sptr color = pTF->getColor( min );
    color->getRefRGBA()[0] = 0;
    color->getRefRGBA()[1] = 0;
    color->getRefRGBA()[2] = 0;
    color->getRefRGBA()[3] = 0;

    color = pTF->getColor( min +1 );
    color->getRefRGBA()[0] = 1;
    color->getRefRGBA()[1] = 1;
    color->getRefRGBA()[2] = 1;
    color->getRefRGBA()[3] = 1;

    color = pTF->getColor( max -1 );
    color->getRefRGBA()[0] = 1;
    color->getRefRGBA()[1] = 1;
    color->getRefRGBA()[2] = 1;
    color->getRefRGBA()[3] = 1;

    color = pTF->getColor( max );
    color->getRefRGBA()[0] = 0;
    color->getRefRGBA()[1] = 0;
    color->getRefRGBA()[2] = 0;
    color->getRefRGBA()[3] = 0;
}

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

std::pair<bool, bool> MedicalImageHelpers::checkMinMaxTFAndSetBWTF( ::fwData::Image::sptr _pImg )
{
    std::pair<bool, bool> minmaxIsModified = ::fwComEd::fieldHelper::MedicalImageHelpers::checkMinMaxTF(_pImg);
    ::fwComEd::fieldHelper::MedicalImageHelpers::setBWTF(_pImg);

    return minmaxIsModified;
}

//------------------------------------------------------------------------------

std::pair<bool, bool> MedicalImageHelpers::checkMinMaxTFAndSetBWTF( ::fwData::PatientDB::sptr _pPatientDB )
{
    std::pair<bool,bool> fieldsAreModified (false,false);

    BOOST_FOREACH( ::fwData::Patient::sptr patient, _pPatientDB->getPatients() )
    {
        BOOST_FOREACH( ::fwData::Study::sptr study, patient->getStudies() )
        {
            BOOST_FOREACH( ::fwData::Acquisition::sptr acq, study->getAcquisitions() )
            {

                ::fwData::Image::sptr pImage = acq->getImage();
                if ( pImage )
                {
                    std::pair<bool, bool> minmaxIsModified = checkMinMaxTFAndSetBWTF(pImage);
                    fieldsAreModified.first = fieldsAreModified.first && minmaxIsModified.first;
                    fieldsAreModified.second = fieldsAreModified.second && minmaxIsModified.second;
                }

            }
        }
    }

    return fieldsAreModified;
}

//------------------------------------------------------------------------------

void MedicalImageHelpers::setImageLabel( ::fwData::Patient::sptr pPatient, ::fwData::Image::sptr pImage)
{
    SLM_ASSERT("pPatient pointer null", pPatient);
    SLM_ASSERT("pImage pointer null", pImage);

    std::stringstream label;
    ::fwData::Integer::sptr intField = pPatient->getField_NEWAPI< ::fwData::Integer >( ::fwComEd::Dictionary::m_acquisitionCountId );
    if(!intField)
    {
        intField = ::fwData::Integer::New(0);
        pPatient->setField_NEWAPI( ::fwComEd::Dictionary::m_acquisitionCountId, intField  );
    }
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
    ::fwComEd::fieldHelper::MedicalImageHelpers::checkMinMaxTFAndSetBWTF(_patientDBFrom);

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
