/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>

#include <fwTools/helpers.hpp>

#include <fwData/Image.hpp>
#include <fwData/TransfertFunction.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/Factory.hpp>
#include <fwServices/registry/ObjectService.hpp>

#include <fwComEd/fieldHelper/MedicalImageHelpers.hpp>
#include <fwComEd/Dictionary.hpp>
#include <fwComEd/ImageMsg.hpp>

#include "fwComEd/helper/MedicalImageAdaptor.hpp"


namespace fwComEd
{

namespace helper
{

//------------------------------------------------------------------------------

MedicalImageAdaptor::MedicalImageAdaptor() : m_orientation(Z_AXIS)
{
    m_tfSelectionFieldId = ::fwComEd::Dictionary::m_transfertFunctionId;
}

//------------------------------------------------------------------------------

MedicalImageAdaptor::~MedicalImageAdaptor()
{}

//------------------------------------------------------------------------------

void MedicalImageAdaptor::getImageSpacing(double spacing[3])
{
    ::fwData::Image::sptr image = this->getImage();;

    const ::fwData::Image::SpacingType& imSpacing = image->getSpacing();
    std::copy(imSpacing.begin(), imSpacing.end(), spacing);
}

//------------------------------------------------------------------------------

void MedicalImageAdaptor::getImageOrigin(double origin[3])
{
    ::fwData::Image::sptr image = this->getImage();;

    std::copy(image->getOrigin().begin(), image->getOrigin().end(), origin);
}

//------------------------------------------------------------------------------

void MedicalImageAdaptor::getImageDataSize(int size[3])
{
    ::fwData::Image::sptr image = this->getImage();;

    const ::fwData::Image::SizeType& imSize = image->getSize();
    std::copy(imSize.begin(), imSize.end(), size);
}

//------------------------------------------------------------------------------

void MedicalImageAdaptor::getImageSize(double size[3])
{
    ::fwData::Image::sptr image = this->getImage();;
    double spacing[3];

    const ::fwData::Image::SizeType& imSize = image->getSize();
    std::copy(imSize.begin(), imSize.end(), size);
    this->getImageSpacing(spacing);

    size[0] *= spacing[0];
    size[1] *= spacing[1];
    size[2] *= spacing[2];
}

//------------------------------------------------------------------------------

void MedicalImageAdaptor::getCurrentSliceCenter(double center[3])
{
    ::fwData::Image::sptr image = this->getImage();;
    double imageSize[3];
    this->getImageSize(imageSize);
    double origin[3];
    this->getImageOrigin(origin);

    ::fwData::Integer::sptr sliceIndex[3];
    this->getSliceIndex(sliceIndex);
    double index[3] = {sliceIndex[0]->value(), sliceIndex[1]->value(), sliceIndex[2]->value()};

    center[0] = origin[0] + (imageSize[0]-1.)/ 2.;
    center[1] = origin[1] + (imageSize[1]-1.)/ 2.;
    center[2] = origin[2] + (imageSize[2]-1.)/ 2.;

    double spacing[3];
    this->getImageSpacing(spacing);
    center[m_orientation] = origin[m_orientation] + index[m_orientation]*spacing[m_orientation];
}

//------------------------------------------------------------------------------

void MedicalImageAdaptor::setOrientation( MedicalImageAdaptor::Orientation orientation )
{
    m_orientation = orientation;
}

//------------------------------------------------------------------------------

void MedicalImageAdaptor::setOrientation( int orientation )
{
    OSLM_FATAL_IF(" orientation value must be  0,1 or 2 (value=" << orientation << ")" , orientation<0 || orientation>3);
    m_orientation = static_cast< ::fwComEd::helper::MedicalImageAdaptor::Orientation >(orientation);
}

//------------------------------------------------------------------------------

static const int indexZ[12] = { 0,2,4, 1,2,4,  1,3,4 ,0,3,4 };
static const int indexY[12] = { 0,2,4, 1,2,4,  1,2,5 ,0,2,5 };
static const int indexX[12] = { 0,2,4, 0,2,5,  0,3,5 ,0,3,4 };
static const int *indexSet[3] = { indexX, indexY, indexZ  };
void MedicalImageAdaptor::getPlane( double points[4][3] , int sliceNumber)
{
    ::fwData::Image::sptr image = this->getImage();;
    double extent[6];
    for (char i=0;  i<3; ++i )
    {
        extent[2*i]   =  0;
        extent[2*i+1] = image->getSize()[i]*image->getSpacing()[i];
    }
    extent[2*m_orientation] = sliceNumber*image->getSpacing()[m_orientation];
    extent[2*m_orientation+1] = sliceNumber*image->getSpacing()[m_orientation];

    const int *extentIndex = indexSet[ m_orientation ];
    for (int p=0; p<4 ; ++p)
    {
        for (int i=0; i<3 ; ++i)
        {
            points[p][i]= extent[ *(extentIndex++) ];
        }
    }
}

//------------------------------------------------------------------------------

void MedicalImageAdaptor::sliceIndexToWorld(const int index[3], double world[3] )
{
    double spacing[3];
    this->getImageSpacing(spacing);
    double origin[3];
    this->getImageOrigin(origin);
    for ( int i=0 ; i<3 ; ++i )
    {
        world[i] = static_cast<int>( (index[i]*spacing[i]) + 0.5*spacing[i] + origin[i] );
    }
}

//------------------------------------------------------------------------------

void MedicalImageAdaptor::worldToSliceIndex(const double world[3], int index[3] )
{
    double spacing[3];
    this->getImageSpacing(spacing);
    double origin[3];
    this->getImageOrigin(origin);
    for ( int i=0 ; i<3 ; ++i )
    {
        // nearest integer
        index[i] = static_cast<int>( ( (world[i]-origin[i])/spacing[i] ) + ( ( (world[i]-origin[i])/spacing[i] ) >= 0 ? 0.5 : -0.5 ) );
    }
}

//------------------------------------------------------------------------------

void MedicalImageAdaptor::worldToImageSliceIndex(const double world[3], int index[3] )
{
    int imageSize[3];
    this->getImageDataSize(imageSize);
    this->worldToSliceIndex(world, index);

    int idval;
    for (int i = 0; i < 3; i++)
    {
        int max = imageSize[i]-1;
        idval = index[i];
        if (idval < 0)
        {
            index[i] = 0;
        }
        else if (idval > max)
        {
            index[i] = max;
        }
    }
}

//------------------------------------------------------------------------------

void MedicalImageAdaptor::getSliceIndex(::fwData::Integer::sptr index[3])
{
    index[0] = m_sagittalIndex;
    index[1] = m_frontalIndex;
    index[2] = m_axialIndex;
}

//------------------------------------------------------------------------------

bool MedicalImageAdaptor::setSliceIndex(const int index[3])
{
    bool isModified = false;
    ::fwData::Image::sptr image = this->getImage();;

    ::fwData::Integer::sptr sliceIndex[3];

    this->getSliceIndex(sliceIndex);

    if(    index[0] != sliceIndex[0]->value()
        || index[1] != sliceIndex[1]->value()
        || index[2] != sliceIndex[2]->value() )
    {
        sliceIndex[0]->value() = index[0];
        sliceIndex[1]->value() = index[1];
        sliceIndex[2]->value() = index[2];
        isModified = true;
    }
    return isModified;
}

//------------------------------------------------------------------------------

void MedicalImageAdaptor::updateImageInfos( ::fwData::Image::sptr image  )
{
    m_weakImage = image;
    m_axialIndex    = image->getDefaultField_NEWAPI(::fwComEd::Dictionary::m_axialSliceIndexId   , ::fwData::Integer::New(0));
    m_frontalIndex  = image->getDefaultField_NEWAPI(::fwComEd::Dictionary::m_frontalSliceIndexId , ::fwData::Integer::New(0));
    m_sagittalIndex = image->getDefaultField_NEWAPI(::fwComEd::Dictionary::m_sagittalSliceIndexId, ::fwData::Integer::New(0));
    m_windowMin     = image->getDefaultField_NEWAPI(::fwComEd::Dictionary::m_windowMinId         , ::fwData::Integer::New(-200));
    m_windowMax     = image->getDefaultField_NEWAPI(::fwComEd::Dictionary::m_windowMaxId         , ::fwData::Integer::New(300));

    m_transfertFunctionId = image->getDefaultField_NEWAPI(
                m_tfSelectionFieldId,
                ::fwData::String::New(::fwData::TransfertFunction::defaultTransfertFunctionName )
                );

    ::fwData::Composite::sptr cTF;
    if(!image->getField_NEWAPI(::fwComEd::Dictionary::m_transfertFunctionCompositeId))
    {
        ::fwData::TransfertFunction::sptr tf = ::fwData::TransfertFunction::createDefaultTransfertFunction(image);
        tf->setMinMax(m_windowMin->value(), m_windowMax->value());

        ::fwData::String::NewSptr tfId;
        tfId->value() = ::fwData::TransfertFunction::defaultTransfertFunctionName;
        cTF = ::fwData::Composite::New();

        (*cTF)[tfId->value()] = tf;
    }
    m_transfertFunctions = image->getDefaultField_NEWAPI(::fwComEd::Dictionary::m_transfertFunctionCompositeId, cTF);
}

//------------------------------------------------------------------------------

::fwData::TransfertFunction::sptr MedicalImageAdaptor::getCurrentTransfertFunction()
{
    return ::fwData::TransfertFunction::dynamicCast((*m_transfertFunctions)[m_transfertFunctionId->value()]);
}

//------------------------------------------------------------------------------

::fwData::Image::sptr MedicalImageAdaptor::getImage()
{
    SLM_ASSERT("Image weak pointer empty !", !m_weakImage.expired());
    return m_weakImage.lock();
}

//------------------------------------------------------------------------------

void MedicalImageAdaptor::setTFSelectionFieldId(std::string tfSelectionId)
{
    if (!tfSelectionId.empty())
    {
        m_tfSelectionFieldId = tfSelectionId;
    }
}

//------------------------------------------------------------------------------

} //namespace helper

} //namespace fwComEd
