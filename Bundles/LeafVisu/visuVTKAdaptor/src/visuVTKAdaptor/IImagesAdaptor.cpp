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
#include <fwServices/ObjectServiceRegistry.hpp>

#include <fwComEd/fieldHelper/MedicalImageHelpers.hpp>
#include <fwComEd/Dictionary.hpp>
#include <fwComEd/ImageMsg.hpp>

#include <vtkIO/vtk.hpp>

#include <vtkMath.h>

#include "visuVTKAdaptor/IImagesAdaptor.hpp"


namespace visuVTKAdaptor
{


IImagesAdaptor::IImagesAdaptor() throw() : m_orientation(Z_AXIS)
{

}

IImagesAdaptor::~IImagesAdaptor() throw()
{
}

void IImagesAdaptor::getImageSpacing(double spacing[3])
{
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();

    std::copy(image->getRefSpacing().begin(), image->getRefSpacing().end(), spacing);

}

void IImagesAdaptor::getImageDataSize(int size[3])
{
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();

    std::copy(image->getRefSize().begin(), image->getRefSize().end(), size);
}

void IImagesAdaptor::getImageSize(double size[3])
{
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();
    double spacing[3];

    std::copy(image->getRefSize().begin(), image->getRefSize().end(), size);
    this->getImageSpacing(spacing);

    size[0] *= spacing[0];
    size[1] *= spacing[1];
    size[2] *= spacing[2];

}

void IImagesAdaptor::getCurrentSliceCenter(double center[3])
{
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();
    double imageSize[3];
    this->getImageSize(imageSize);

    ::fwData::Integer::sptr sliceIndex[3];
    this->getSliceIndex(sliceIndex);
    double index[3] = {sliceIndex[0]->value(), sliceIndex[1]->value(), sliceIndex[2]->value()};

    center[0] = (imageSize[0]-1.) / 2. ;
    center[1] = (imageSize[1]-1.) / 2. ;
    center[2] = (imageSize[2]-1.) / 2. ;

    double spacing[3];
    this->getImageSpacing(spacing);
    center[m_orientation] = index[m_orientation]*spacing[m_orientation];
}

void IImagesAdaptor::setOrientation( IImagesAdaptor::Orientation orientation )
{
    m_orientation = orientation;
}

void IImagesAdaptor::setOrientation( int orientation )
{
    OSLM_FATAL_IF(" orientation value must be  0,1 or 2 (value=" << orientation << ")" , orientation<0 || orientation>3);
    m_orientation = static_cast< ::visuVTKAdaptor::IImagesAdaptor::Orientation >(orientation);
}


static const int indexZ[12] = { 0,2,4, 1,2,4,  1,3,4 ,0,3,4 };
static const int indexY[12] = { 0,2,4, 1,2,4,  1,2,5 ,0,2,5 };
static const int indexX[12] = { 0,2,4, 0,2,5,  0,3,5 ,0,3,4 };
static const int *indexSet[3] = { indexX, indexY, indexZ  };
void IImagesAdaptor::getPlane( double points[4][3] , int sliceNumber)
{
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();
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
        for (int i=0; i<3 ; ++i)
        {
            points[p][i]= extent[ *(extentIndex++) ];
        }
}

//float IImagesAdaptor::getPixelvalue( double worldPosition)
//{
//  assert(false); // TODO
//}


void IImagesAdaptor::worldToSliceIndex(const double world[3], int index[3] )
{

    double spacing[3];
    this->getImageSpacing(spacing);
    for ( int i=0 ; i<3 ; ++i )
    {
        // nearest unsigned integer
        index[i] =  vtkMath::Round( world[i]/spacing[i] );
    }
}


void IImagesAdaptor::worldToImageSliceIndex(const double world[3], int index[3] )
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


void IImagesAdaptor::getSliceIndex(::fwData::Integer::sptr index[3])
{
    index[0] = m_sagittalIndex;
    index[1] = m_frontalIndex;
    index[2] = m_axialIndex;
}

bool IImagesAdaptor::setSliceIndex(const int index[3])
{
    bool isModified = false;
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();

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


void IImagesAdaptor::updateImageInfos( ::fwData::Image::sptr image  )
{
    ::fwTools::getFieldFromObject(m_axialIndex   , image, ::fwComEd::Dictionary::m_axialSliceIndexId   , ::fwData::Integer::New(0));
    ::fwTools::getFieldFromObject(m_frontalIndex , image, ::fwComEd::Dictionary::m_frontalSliceIndexId , ::fwData::Integer::New(0));
    ::fwTools::getFieldFromObject(m_sagittalIndex, image, ::fwComEd::Dictionary::m_sagittalSliceIndexId, ::fwData::Integer::New(0));
    ::fwTools::getFieldFromObject(m_windowMin    , image, ::fwComEd::Dictionary::m_windowMinId         , ::fwData::Integer::New(-200));
    ::fwTools::getFieldFromObject(m_windowMax    , image, ::fwComEd::Dictionary::m_windowMaxId         , ::fwData::Integer::New(300));

    ::fwTools::getFieldFromObject(m_transfertFunctionId, image, ::fwComEd::Dictionary::m_transfertFunctionId, ::fwData::String::New(::fwData::TransfertFunction::defaultTransfertFunctionName));

    ::fwData::Composite::sptr cTF;
    if(!image->getField(::fwComEd::Dictionary::m_transfertFunctionCompositeId))
    {
        ::fwData::TransfertFunction::sptr tf = ::fwData::TransfertFunction::createDefaultTransfertFunction(image);
        tf->setMinMax(m_windowMin->value(), m_windowMax->value());

        ::fwData::String::NewSptr tfId;
        tfId->value() = ::fwData::TransfertFunction::defaultTransfertFunctionName;
        cTF = ::fwData::Composite::New();

        cTF->operator[](tfId->value()) = tf;
        (*cTF)[tfId->value()] = tf;

    }
    ::fwTools::getFieldFromObject(m_transfertFunctions, image, ::fwComEd::Dictionary::m_transfertFunctionCompositeId, cTF);
}

} //namespace visuVTKAdaptor
