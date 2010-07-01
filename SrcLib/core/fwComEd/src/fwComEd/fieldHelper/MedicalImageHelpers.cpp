/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */
/* ***** BEGIN CONTRIBUTORS BLOCK *****
 * Contributors:
 *  - Jean-Baptiste.Fasquel (LISA Laboratory, Angers University, France)
 * ****** END CONTRIBUTORS BLOCK ****** */

#include <fwData/Integer.hpp>
#include <fwData/TransfertFunction.hpp>
#include <fwData/Composite.hpp>
#include <fwData/String.hpp>
#include <fwData/PointList.hpp>
#include <fwData/Float.hpp>
#include <fwData/StandardBuffer.hpp>

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

        if ( _pImg->getWindowWidth() != 0 )
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
        assert( fieldsAreModified.first );

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
        assert( ! fieldsAreModified.first );

        ::fwData::Composite::sptr cTransfertFunction = _pImg->getFieldSingleElement< ::fwData::Composite >( ::fwComEd::Dictionary::m_transfertFunctionCompositeId );
        ::fwData::String::sptr sTransfertFunction = _pImg->getFieldSingleElement< ::fwData::String >( ::fwComEd::Dictionary::m_transfertFunctionId );
        ::fwData::TransfertFunction::sptr pTF = ::fwData::TransfertFunction::dynamicCast( cTransfertFunction->getRefMap()[sTransfertFunction->value()] );

        std::pair< ::boost::int32_t, ::boost::int32_t > currentMinMax = pTF->getMinMax();

        // ACH => Normaly this case does not exit in the current framework
        assert( windowMin == currentMinMax.first && windowMax == currentMinMax.second );
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
    assert( _pImg->getFieldSize( ::fwComEd::Dictionary::m_windowMinId ) &&
            _pImg->getFieldSize( ::fwComEd::Dictionary::m_windowMaxId ) &&
            _pImg->getFieldSize( ::fwComEd::Dictionary::m_transfertFunctionCompositeId ) );

    ::fwData::Integer::sptr minField = _pImg->getFieldSingleElement< ::fwData::Integer >( ::fwComEd::Dictionary::m_windowMinId );
    ::fwData::Integer::sptr maxField = _pImg->getFieldSingleElement< ::fwData::Integer >( ::fwComEd::Dictionary::m_windowMaxId );

    ::fwData::Composite::sptr cTransfertFunction = _pImg->getFieldSingleElement< ::fwData::Composite >( ::fwComEd::Dictionary::m_transfertFunctionCompositeId );
    ::fwData::String::sptr sTransfertFunction = _pImg->getFieldSingleElement< ::fwData::String >( ::fwComEd::Dictionary::m_transfertFunctionId );
    ::fwData::TransfertFunction::sptr pTF = ::fwData::TransfertFunction::dynamicCast( cTransfertFunction->getRefMap()[sTransfertFunction->value()] );

    updateTFFromMinMax(minField, maxField, pTF);
}

//------------------------------------------------------------------------------

void MedicalImageHelpers::setBWTF( ::fwData::Image::sptr _pImg )
{
    checkMinMaxTF( _pImg );

    ::fwData::Composite::sptr cTransfertFunction = _pImg->getFieldSingleElement< ::fwData::Composite >( ::fwComEd::Dictionary::m_transfertFunctionCompositeId );
    ::fwData::String::sptr sTransfertFunction = _pImg->getFieldSingleElement< ::fwData::String >( ::fwComEd::Dictionary::m_transfertFunctionId );

    // If have not default Transfert Function (BW)
    if( cTransfertFunction->getRefMap().find( ::fwData::TransfertFunction::defaultTransfertFunctionName ) == cTransfertFunction->getRefMap().end() )
    {
        ::fwData::Integer::sptr min = _pImg->getFieldSingleElement< ::fwData::Integer >( fwComEd::Dictionary::m_windowMinId );
        ::fwData::Integer::sptr max = _pImg->getFieldSingleElement< ::fwData::Integer >( fwComEd::Dictionary::m_windowMaxId );
        int windowMin = min->value();
        int windowMax = max->value();

        ::fwData::TransfertFunction::sptr tf = ::fwData::TransfertFunction::createDefaultTransfertFunction(_pImg);
        tf->setMinMax(windowMin, windowMax);

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

bool MedicalImageHelpers::checkLandmarks( ::fwData::Image::sptr _pImg )
{
    bool fieldIsModified = false;

    // Manage image landmarks
    if ( ! _pImg->getFieldSize( ::fwComEd::Dictionary::m_imageLandmarksId ) )
    {
        ::fwData::PointList::NewSptr pl;
        _pImg->setFieldSingleElement( ::fwComEd::Dictionary::m_imageLandmarksId, pl );
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

    for ( int k = 0; dataImageIsAllocated && k < _pImg->getDimension(); k++ )
    {
        if(k == 2 && _pImg->getDimension() == 3) // special test for 2D jpeg image (size[2] == 1)
            dataImageIsAllocated = dataImageIsAllocated && ( _pImg->getSize()[k] >= 1 );
        else
            dataImageIsAllocated = dataImageIsAllocated && ( _pImg->getSize()[k] != 0 && ( _pImg->getSize()[k] != 1 ) );
    }

    return dataImageIsAllocated;
}

//------------------------------------------------------------------------------

bool MedicalImageHelpers::checkImageSliceIndex( ::fwData::Image::sptr _pImg )
{
    SLM_TRACE_FUNC();

    bool fieldIsModified = false;

    // Manage image landmarks
    if (    ! _pImg->getFieldSize( ::fwComEd::Dictionary::m_axialSliceIndexId ) ||
            ! _pImg->getFieldSize( ::fwComEd::Dictionary::m_frontalSliceIndexId ) ||
            ! _pImg->getFieldSize( ::fwComEd::Dictionary::m_sagittalSliceIndexId ) )
    {

        // Get value
        std::vector< boost::int32_t > imageSize (3,0);
        for ( int k = 0; k < _pImg->getDimension(); k++ )
        {
            imageSize[k] = _pImg->getSize()[k];
        }

        // Set value
        ::fwData::Integer::NewSptr paramA;
        paramA->value() = imageSize[2] / 2;
        _pImg->setFieldSingleElement( ::fwComEd::Dictionary::m_axialSliceIndexId, paramA );

        ::fwData::Integer::NewSptr paramF;
        paramF->value() = imageSize[1] / 2;
        _pImg->setFieldSingleElement( ::fwComEd::Dictionary::m_frontalSliceIndexId, paramF );

        ::fwData::Integer::NewSptr paramS;
        paramS->value() = imageSize[0] / 2;
        _pImg->setFieldSingleElement( ::fwComEd::Dictionary::m_sagittalSliceIndexId, paramS );

        fieldIsModified = true;
    }
    else if (   _pImg->getFieldSize( ::fwComEd::Dictionary::m_axialSliceIndexId ) > 0 &&
                _pImg->getFieldSize( ::fwComEd::Dictionary::m_frontalSliceIndexId ) > 0 &&
                _pImg->getFieldSize( ::fwComEd::Dictionary::m_sagittalSliceIndexId ) > 0 )
    {
        // Get value
        std::vector< boost::int32_t > imageSize (3,0);
        for ( int k = 0; k < _pImg->getDimension(); k++ )
        {
            imageSize[k] = _pImg->getSize()[k];
        }

        // Get value
        ::fwData::Integer::sptr paramA = _pImg->getFieldSingleElement< ::fwData::Integer >( ::fwComEd::Dictionary::m_axialSliceIndexId );
        if( paramA->value() < 0 ||  imageSize[2] < paramA->value() )
        {
            paramA->value() = imageSize[2] / 2;
            _pImg->setFieldSingleElement( ::fwComEd::Dictionary::m_axialSliceIndexId, paramA );
            fieldIsModified = true;
        }

        ::fwData::Integer::sptr paramF = _pImg->getFieldSingleElement< ::fwData::Integer >( ::fwComEd::Dictionary::m_frontalSliceIndexId );
        if( paramF->value() < 0 ||  imageSize[1] < paramF->value() )
        {
            paramF->value() = imageSize[1] / 2;
            _pImg->setFieldSingleElement( ::fwComEd::Dictionary::m_frontalSliceIndexId, paramF );
            fieldIsModified = true;
        }

        ::fwData::Integer::sptr paramS = _pImg->getFieldSingleElement< ::fwData::Integer >( ::fwComEd::Dictionary::m_sagittalSliceIndexId );
        if( paramS->value() < 0 ||  imageSize[0] < paramS->value() )
        {
            paramS->value() = imageSize[0] / 2;
            _pImg->setFieldSingleElement( ::fwComEd::Dictionary::m_sagittalSliceIndexId, paramF );
            fieldIsModified = true;
        }
    }
    else
    {
        SLM_FATAL("Information on image slice index is not correct, miss one of these fields : m_axialSliceIndexId, m_frontalSliceIndexId, m_sagittalSliceIndexId.");
    }


    return fieldIsModified;
}

//------------------------------------------------------------------------------

::fwData::Point::sptr   MedicalImageHelpers::getImageSliceIndices( ::fwData::Image::sptr _pImg )
{
    assert( _pImg );

    ::fwData::Point::NewSptr point;

    MedicalImageHelpers::checkImageSliceIndex(_pImg);

    point->getRefCoord()[0] = _pImg->getFieldSingleElement< ::fwData::Integer >( ::fwComEd::Dictionary::m_sagittalSliceIndexId )->value();
    point->getRefCoord()[1] = _pImg->getFieldSingleElement< ::fwData::Integer >( ::fwComEd::Dictionary::m_frontalSliceIndexId  )->value();
    point->getRefCoord()[2] = _pImg->getFieldSingleElement< ::fwData::Integer >( ::fwComEd::Dictionary::m_axialSliceIndexId    )->value();

    return point;
}



//------------------------------------------------------------------------------

bool MedicalImageHelpers::checkComment( ::fwData::Image::sptr _pImg )
{
    SLM_TRACE_FUNC();

    bool fieldIsModified = false;

    if ( ! _pImg->getFieldSize( ::fwComEd::Dictionary::m_commentId ) )
    {
        // Set value
        ::fwData::String::NewSptr param;
        param->value() = "Original image";
        _pImg->setFieldSingleElement( ::fwComEd::Dictionary::m_commentId, param );

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

    ::fwData::PatientDB::PatientIterator patientIter;
    for (   patientIter = _pPatientDB->getPatients().first ;
            patientIter != _pPatientDB->getPatients().second ;
            ++patientIter)
    {
        // Study selection
        ::fwData::Patient::StudyIterator studyIter;
        for (   studyIter = (*patientIter)->getStudies().first ;
                studyIter != (*patientIter)->getStudies().second ;
                ++studyIter)
        {
            // Acquisition selection
            ::fwData::Study::AcquisitionIterator acquisitionIter;
            for (   acquisitionIter = (*studyIter)->getAcquisitions().first ;
                    acquisitionIter != (*studyIter)->getAcquisitions().second ;
                    ++acquisitionIter)
            {
                ::fwData::Image::sptr pImage = (*acquisitionIter)->getImage();
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
    std::stringstream label;
    if ( ! pPatient->getFieldSize( ::fwComEd::Dictionary::m_acquisitionCountId ) )
    {
        ::fwData::Integer::NewSptr intField;
        pPatient->setFieldSingleElement( ::fwComEd::Dictionary::m_acquisitionCountId, intField  );
        label << "I0";
        pPatient->getFieldSingleElement< ::fwData::Integer >(::fwComEd::Dictionary::m_acquisitionCountId)->value()++;
    }
    else
    {
        int count = pPatient->getFieldSingleElement< ::fwData::Integer >(::fwComEd::Dictionary::m_acquisitionCountId)->value();
        label << "I" << count;
        pPatient->getFieldSingleElement< ::fwData::Integer >(::fwComEd::Dictionary::m_acquisitionCountId)->value()++;
    }
    ::fwData::String::NewSptr labelField;
    labelField->value() = label.str();
    pImage->setFieldSingleElement(::fwComEd::Dictionary::m_imageLabelId, labelField);
}

//------------------------------------------------------------------------------

bool MedicalImageHelpers::checkOpacity( ::fwData::Image::sptr _pImg)
{
    bool fieldIsModified = false;

    // Manage image landmarks
    if ( ! _pImg->getFieldSize( ::fwComEd::Dictionary::m_opacityId ) )
    {
        ::fwData::Float::NewSptr opacity;
        opacity->value() = 1.0;
        _pImg->setFieldSingleElement( ::fwComEd::Dictionary::m_opacityId, opacity );
        fieldIsModified = true;
    }

    return fieldIsModified;
}

::fwData::Image::sptr MedicalImageHelpers::initialize( ::fwData::Image::sptr imgSrc, ::fwData::Image::sptr imgToInitialize)
{
    SLM_ASSERT("Image source must be initialized", imgSrc);
    SLM_ASSERT("Image source must be valid", MedicalImageHelpers::checkImageValidity(imgSrc));

    if(!imgToInitialize)
    {
        imgToInitialize = ::fwData::Image::New();
    }
    ::fwData::IBufferDelegate::sptr buffImgSrc = imgSrc->getBufferDelegate();
    imgSrc->setBufferDelegate( ::fwData::StandardBuffer::sptr() );

    imgToInitialize->deepCopy(imgSrc);
    imgSrc->setBufferDelegate( buffImgSrc );

    ::boost::int32_t size = ::fwData::imageSizeInBytes( *imgSrc );
    char * dest = new char[size];
    imgToInitialize->setBuffer( dest );

    return imgToInitialize;
}

//------------------------------------------------------------------------------

} // fieldHelper
} // fwComEd
