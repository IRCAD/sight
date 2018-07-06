/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwDataTools/helper/MedicalImage.hpp"

#include "fwDataTools/fieldHelper/Image.hpp"
#include "fwDataTools/fieldHelper/MedicalImageHelpers.hpp"
#include "fwDataTools/helper/Composite.hpp"
#include "fwDataTools/helper/Image.hpp"

#include <fwData/Image.hpp>

namespace fwDataTools
{

namespace helper
{

//------------------------------------------------------------------------------

MedicalImage::MedicalImage() :
    m_orientation(Z_AXIS)
{
}

//------------------------------------------------------------------------------

MedicalImage::~MedicalImage()
{
}

//------------------------------------------------------------------------------

void MedicalImage::getImageSpacing(double spacing[3]) const
{
    ::fwData::Image::sptr image = this->getImage();

    const ::fwData::Image::SpacingType& imSpacing = image->getSpacing();
    std::copy(imSpacing.begin(), imSpacing.end(), spacing);
}

//------------------------------------------------------------------------------

void MedicalImage::getImageOrigin(double origin[3]) const
{
    ::fwData::Image::sptr image = this->getImage();

    std::copy(image->getOrigin().begin(), image->getOrigin().end(), origin);
}

//------------------------------------------------------------------------------

void MedicalImage::getImageDataSize(int size[3]) const
{
    ::fwData::Image::sptr image = this->getImage();

    const ::fwData::Image::SizeType& imSize = image->getSize();
    std::copy(imSize.begin(), imSize.end(), size);
}

//------------------------------------------------------------------------------

void MedicalImage::getImageSize(double size[3]) const
{
    ::fwData::Image::sptr image = this->getImage();
    double spacing[3];

    const ::fwData::Image::SizeType& imSize = image->getSize();
    std::copy(imSize.begin(), imSize.end(), size);
    this->getImageSpacing(spacing);

    size[0] *= spacing[0];
    size[1] *= spacing[1];
    size[2] *= spacing[2];
}

//------------------------------------------------------------------------------

void MedicalImage::getCurrentSliceCenter(double center[3])
{
    ::fwData::Image::sptr image = this->getImage();
    double imageSize[3];
    this->getImageSize(imageSize);
    double origin[3];
    this->getImageOrigin(origin);

    ::fwData::Integer::sptr sliceIndex[3];
    this->getSliceIndex(sliceIndex);
    double index[3] = {
        static_cast<double>(sliceIndex[0]->value()),
        static_cast<double>(sliceIndex[1]->value()),
        static_cast<double>(sliceIndex[2]->value())
    };

    center[0] = origin[0] + (imageSize[0]-1.)/ 2.;
    center[1] = origin[1] + (imageSize[1]-1.)/ 2.;
    center[2] = origin[2] + (imageSize[2]-1.)/ 2.;

    double spacing[3];
    this->getImageSpacing(spacing);
    center[m_orientation] = origin[m_orientation] + index[m_orientation]*spacing[m_orientation];
}

//------------------------------------------------------------------------------

void MedicalImage::setOrientation( MedicalImage::Orientation orientation )
{
    m_orientation = orientation;
}

//------------------------------------------------------------------------------

void MedicalImage::setOrientation( int orientation )
{
    OSLM_ASSERT("orientation value must be  0,1 or 2 (value = " << orientation << ")",
                orientation == 0 || orientation == 1 || orientation == 2);
    this->setOrientation(static_cast< ::fwDataTools::helper::MedicalImage::Orientation >(orientation));
}

//------------------------------------------------------------------------------

static const int indexZ[12]   = { 0, 2, 4, 1, 2, 4,  1, 3, 4, 0, 3, 4 };
static const int indexY[12]   = { 0, 2, 4, 1, 2, 4,  1, 2, 5, 0, 2, 5 };
static const int indexX[12]   = { 0, 2, 4, 0, 2, 5,  0, 3, 5, 0, 3, 4 };
static const int* indexSet[3] = { indexX, indexY, indexZ  };
//------------------------------------------------------------------------------

void MedicalImage::getPlane( double points[4][3], int sliceNumber)
{
    ::fwData::Image::sptr image = this->getImage();
    double extent[6];
    for (unsigned char i = 0; i < 3; ++i )
    {
        extent[2*i]   = 0;
        extent[2*i+1] = image->getSize()[i]*image->getSpacing()[i];
    }
    extent[2*m_orientation]   = sliceNumber*image->getSpacing()[m_orientation];
    extent[2*m_orientation+1] = sliceNumber*image->getSpacing()[m_orientation];

    const int* extentIndex = indexSet[ m_orientation ];
    for (int p = 0; p < 4; ++p)
    {
        for (int i = 0; i < 3; ++i)
        {
            points[p][i] = extent[ *(extentIndex++) ];
        }
    }
}

//------------------------------------------------------------------------------

void MedicalImage::sliceIndexToWorld(const int index[3], double world[3] )
{
    double spacing[3];
    this->getImageSpacing(spacing);
    double origin[3];
    this->getImageOrigin(origin);
    for ( int i = 0; i < 3; ++i )
    {
        world[i] = static_cast<int>( (index[i]*spacing[i]) + 0.5*spacing[i] + origin[i] );
    }
}

//------------------------------------------------------------------------------

void MedicalImage::worldToSliceIndex(const double world[3], int index[3] )
{
    double spacing[3];
    this->getImageSpacing(spacing);
    double origin[3];
    this->getImageOrigin(origin);
    for ( int i = 0; i < 3; ++i )
    {
        // nearest integer
        index[i] =
            static_cast<int>( ( (world[i]-origin[i])/spacing[i] ) +
                              ( ( (world[i]-origin[i])/spacing[i] ) >= 0 ? 0.5 : -0.5 ) );
    }
}

//------------------------------------------------------------------------------

void MedicalImage::worldToImageSliceIndex(const double world[3], int index[3] )
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

void MedicalImage::getSliceIndex(::fwData::Integer::sptr index[3])
{
    index[0] = m_sagittalIndex;
    index[1] = m_frontalIndex;
    index[2] = m_axialIndex;
}

//------------------------------------------------------------------------------

bool MedicalImage::setSliceIndex(const int index[3])
{
    bool isModified = false;

    ::fwData::Integer::sptr sliceIndex[3];

    this->getSliceIndex(sliceIndex);

    if(    index[0] != m_sagittalIndex->value()
           || index[1] != m_frontalIndex->value()
           || index[2] != m_axialIndex->value() )
    {
        m_sagittalIndex->value() = index[0];
        m_frontalIndex->value()  = index[1];
        m_axialIndex->value()    = index[2];
        isModified               = true;
    }
    return isModified;
}

//------------------------------------------------------------------------------

void MedicalImage::updateImageInfos( ::fwData::Image::sptr image )
{
    m_weakImage  = image;
    m_axialIndex = image->setDefaultField(::fwDataTools::fieldHelper::Image::m_axialSliceIndexId,
                                          ::fwData::Integer::New(0));
    m_frontalIndex = image->setDefaultField(::fwDataTools::fieldHelper::Image::m_frontalSliceIndexId,
                                            ::fwData::Integer::New(0));
    m_sagittalIndex = image->setDefaultField(::fwDataTools::fieldHelper::Image::m_sagittalSliceIndexId,
                                             ::fwData::Integer::New(0));
}

//------------------------------------------------------------------------------

::fwData::Image::sptr MedicalImage::getImage() const
{
    SLM_ASSERT("Image weak pointer empty !", !m_weakImage.expired());
    return m_weakImage.lock();
}

//------------------------------------------------------------------------------

} //namespace helper

} //namespace fwDataTools
