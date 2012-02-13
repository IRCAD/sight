/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <map>
#include <iomanip>
#include <cstdlib>
#include <ctime>

#include <boost/foreach.hpp>

#include "fwDataTools/Image.hpp"


namespace fwDataTools
{

//------------------------------------------------------------------------------

Image::Image()
{}

//------------------------------------------------------------------------------

Image::~Image()
{}

//------------------------------------------------------------------------------

void Image::initRand()
{
    std::srand(std::time(NULL));
}

//------------------------------------------------------------------------------

void Image::generateImage(::fwData::Image::sptr image,
                          ::fwData::Image::SizeType size,
                          std::vector<double> spacing,
                          std::vector<double> origin,
                          ::fwTools::Type type)
{
    image->setSpacing(spacing);
    image->setOrigin(origin);
    image->setSize(size);
    image->setType(type);

    image->allocate();

    ::fwData::Array::sptr array = image->getDataArray();
    std::fill(array->begin(), array->end(), 0);
}

//------------------------------------------------------------------------------

void Image::generateRandomImage(::fwData::Image::sptr image, ::fwTools::Type type)
{
    ::boost::uint8_t dim = 3;
    image->setType(type);

    ::fwData::Image::SizeType size(3);
    size[0] = rand()%100 +1;
    size[1] = rand()%100 +1;
    size[2] = rand()%100 +1;
    image->setSize(size);

    std::vector< double > spacing(3);
    spacing[0] = (rand()%200 +1) / 100.;
    spacing[1] = (rand()%200 +1) / 100.;
    spacing[2] = (rand()%200 +1) / 100.;
    image->setSpacing(spacing);

    std::vector< double > origin(3);
    origin[0] = (rand()%200 - 100) / 10.;
    origin[1] = (rand()%200 - 100) / 10.;
    origin[2] = (rand()%200 - 100) / 10.;
    image->setOrigin(origin);

    image->allocate();

    ::fwData::Array::sptr array = image->getDataArray();
    randomizeArray(array);
}

//------------------------------------------------------------------------------

void Image::randomizeArray(::fwData::Array::sptr array)
{
    char *iter = array->begin< char >();

    for (; iter != array->end< char >() ; ++iter)
    {
        *iter = rand()%256;
    }
}

//------------------------------------------------------------------------------

bool Image::compareImage(::fwData::Image::sptr image1, ::fwData::Image::sptr image2)
{
    bool compare = true;

    compare &= image1->getNumberOfDimensions() == image2->getNumberOfDimensions();
    OSLM_ERROR_IF("Images have not the same dimension : " << image1->getNumberOfDimensions() << " != " << image2->getNumberOfDimensions(),
            image1->getNumberOfDimensions() != image2->getNumberOfDimensions());

    compare &= image1->getSize() == image2->getSize();
    OSLM_ERROR_IF("Images have not the same size : ( " <<
            image1->getSize()[0] << ", " << image1->getSize()[1] << ", " << image1->getSize()[2] << ") != (" <<
            image2->getSize()[0] << ", " << image2->getSize()[1] << ", " << image2->getSize()[2] << " )",
            image1->getSize() != image2->getSize());

    compare &= image1->getSpacing() == image2->getSpacing();
    OSLM_ERROR_IF(std::setprecision(10) << "Images have not the same spacing : ( " <<
            image1->getSpacing()[0] << ", " << image1->getSpacing()[1] << ", " << image1->getSpacing()[2] << ") != (" <<
            image2->getSpacing()[0] << ", " << image2->getSpacing()[1] << ", " << image2->getSpacing()[2] << " )",
            image1->getSpacing() != image2->getSpacing());

    compare &= image1->getOrigin() == image2->getOrigin();
    OSLM_ERROR_IF("Images have not the same origin : ( " <<
            image1->getOrigin()[0] << ", " << image1->getOrigin()[1] << ", " << image1->getOrigin()[2] << ") != (" <<
            image2->getOrigin()[0] << ", " << image2->getOrigin()[1] << ", " << image2->getOrigin()[2] << " )",
            image1->getOrigin() != image2->getOrigin());

    compare &= image1->getType() == image2->getType();
    OSLM_ERROR_IF("Images have not the same type : " << image1->getType().string() << " != " << image2->getType().string(),
            image1->getType() != image2->getType());

    compare &= image1->getSizeInBytes() == image2->getSizeInBytes();
    OSLM_ERROR_IF("Images have not the same type : " << image1->getSizeInBytes() << " != " << image2->getSizeInBytes(),
            image1->getSizeInBytes() != image2->getSizeInBytes());


    compare &= ::fwDataTools::Image::compareArray(image1->getDataArray(), image2->getDataArray());

    return compare;
}

//------------------------------------------------------------------------------

bool Image::compareImage(::fwData::Image::sptr image1,
                         ::fwData::Image::SizeType size,
                         std::vector<double> spacing,
                         std::vector<double> origin,
                         ::fwTools::Type type)
{
    bool compare = true;

    compare &= image1->getNumberOfDimensions() == size.size();
    OSLM_ERROR_IF("Images have not the same dimension : " << image1->getNumberOfDimensions() << " != " << size.size(),
            image1->getNumberOfDimensions() != size.size());

    compare &= image1->getSize() == size;
    OSLM_ERROR_IF("Images have not the same size",image1->getSize() != size);

    compare &= image1->getSpacing() == spacing;
    OSLM_ERROR_IF("Images have not the same spacing", image1->getSpacing() != spacing);

    compare &= image1->getOrigin() == origin;
    OSLM_ERROR_IF("Images have not the same origin", image1->getOrigin() != origin);

    compare &= image1->getType() == type;
    OSLM_ERROR_IF("Images have not the same type : " << image1->getType().string() << " != " << type.string(),
            image1->getType() != type);

    return compare;
}

//------------------------------------------------------------------------------

bool Image::compareArray(::fwData::Array::sptr array1, ::fwData::Array::sptr array2)
{
    bool compare = true;

    compare &= (!array1 && !array2) || (array1 && array2);
    SLM_ERROR_IF("The two arrays are not initialized", (!array1 && array2) || (array1 && !array2));

    compare &= array1->getSizeInBytes() == array2->getSizeInBytes();
    SLM_ERROR_IF("Arrays have not same size in bytes", array1->getSizeInBytes() != array2->getSizeInBytes());

    compare &= array1->getElementSizeInBytes() ==  array2->getElementSizeInBytes();
    SLM_ERROR_IF("Arrays have not same element size in bytes", array1->getElementSizeInBytes() !=  array2->getElementSizeInBytes());

    compare &= array1->getNumberOfDimensions() ==  array2->getNumberOfDimensions();
    SLM_ERROR_IF("Arrays have not same number of dimensions", array1->getNumberOfDimensions() !=  array2->getNumberOfDimensions());

    compare &= array1->getSize() == array2->getSize();
    SLM_ERROR_IF("Arrays have not same size", array1->getSize() != array2->getSize());

    compare &= array1->getNumberOfComponents() ==  array2->getNumberOfComponents();
    SLM_ERROR_IF("Arrays have not same number of componnents", array1->getNumberOfComponents() !=  array2->getNumberOfComponents());

    compare &= array1->getIsBufferOwner() ==  array2->getIsBufferOwner();
    OSLM_ERROR_IF("Arrays have not same buffer owner : " << array1->getIsBufferOwner() << " != " << array2->getIsBufferOwner(),
            array1->getIsBufferOwner() !=  array2->getIsBufferOwner());

    compare &= array1->getStrides() == array2->getStrides();
    SLM_ERROR_IF("Arrays have not same strides", array1->getStrides() != array2->getStrides());

    compare &= array1->getType().string() == array2->getType().string();
    SLM_ERROR_IF("Arrays have not same type", array1->getType().string() != array2->getType().string());

    compare &= array1->getType().sizeOf() == array2->getType().sizeOf();
    SLM_ERROR_IF("Arrays have not same type", array1->getType().sizeOf() != array2->getType().sizeOf());

    if(array1)
    {
        char *iter1 = array1->begin<char>();
        char *iter2 = array2->begin<char>();

        for (; iter1 != array1->end<char>() ; ++iter1, ++iter2)
        {
            if ((*iter1 != *iter2))
            {
                compare = false;
                SLM_ERROR("Array values are different");
                break;
            }
        }
    }
    return compare;
}

//------------------------------------------------------------------------------

} // namespace fwDataTools
