/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __CPR_COMPUTECPRFUNCTOR_HXX__
#define __CPR_COMPUTECPRFUNCTOR_HXX__

#include "cpr/functions.hpp"

#include <fwData/Image.hpp>
#include <fwData/Mesh.hpp>

#include <fwDataTools/helper/ImageGetter.hpp>
#include <fwDataTools/helper/Mesh.hpp>

namespace cpr
{


/// This method fills the color grid for the mesh.
template<typename IMAGE_TYPE>
void fillColorGrid(
    const std::vector<double>& pointGrid,
    unsigned int nbCol,
    unsigned int nbRow,
    ::fwData::Image::csptr imageSource,
    std::vector<IMAGE_TYPE>& colorGrid)
{
    ::fwDataTools::helper::ImageGetter helperImage (imageSource);
    // Allocate
    double* point               = new double [3];
    unsigned int* IndexPosition = new unsigned int[3];
    bool isPointInImage;
    unsigned int nbPoints = nbCol*nbRow;
    colorGrid.reserve(nbPoints);
    colorGrid.resize(nbPoints);
    for (unsigned int i = 0; i < nbCol; i++)
    {
        for (unsigned int j = 0; j < nbRow; j++)
        {
            unsigned int indexPoint = i + j * nbCol;
            point[0] = pointGrid[indexPoint*3];
            point[1] = pointGrid[indexPoint*3 + 1];
            point[2] = pointGrid[indexPoint*3 + 2];
            // Compute nearest index image if the point is int the image
            isPointInImage = ::cpr::computeImageIndexFromSpacePosition
                                 (imageSource, &point[0], &IndexPosition[0]);
            // Compute color
            if(isPointInImage)
            {
                void* buff    = helperImage.getPixelBuffer( IndexPosition[0], IndexPosition[1], IndexPosition[2]);
                IMAGE_TYPE hu = *( static_cast<IMAGE_TYPE*>( buff ) );
                colorGrid[indexPoint] = hu;
            }
            else
            {
                colorGrid[indexPoint] = -1024;
            }
        }
    }
    // Free memory
    delete[] point;
    delete[] IndexPosition;
}
/// Transfert Function
template<typename IMAGE_TYPE>
void computeGreyLevelFromHounsfield( IMAGE_TYPE hounsfield, ::fwData::Mesh::ColorValueType* color)
{
    double min[3] = {0, 0, 0};
    double max[3] = {255, 255, 255};
    double minFt  = 0;
    double maxFt  = 300;
    if (hounsfield < minFt)
    {
        std::copy(min, min + 3, color);
    }
    else if (hounsfield > maxFt)
    {
        std::copy(max, max + 3, color);
    }
    else
    {
        for (int i = 0; i < 3; i++)
        {
            color[i] = (hounsfield - minFt) * (255.0 / (maxFt - minFt));
        }
    }
}
// Fill Mesh
template<typename IMAGE_TYPE>
void fillMesh(
    const std::vector<double>& pointGrid,
    std::vector< IMAGE_TYPE >& colorGrid,
    unsigned int nbCol,
    unsigned int nbRow,
    ::fwData::Image::csptr image,
    ::fwData::Mesh::sptr mesh )
{
    fillColorGrid(pointGrid, nbCol, nbRow, image, colorGrid);
    unsigned int nbPoints = nbCol * nbRow;
    unsigned int nbCells  = (nbCol - 1) * (nbRow - 1);
    ::fwData::Mesh::ColorValueType color[3];
    mesh->setNumberOfPoints(nbPoints);
    mesh->setNumberOfCells(nbCells);
    mesh->setCellDataSize(nbCells * 4);
    mesh->allocate(nbPoints, nbCells, nbCells * 4);
    mesh->allocatePointColors(::fwData::Mesh::RGB);
    ::fwDataTools::helper::Mesh helperMesh(mesh);
    ::fwData::Mesh::PointsMultiArrayType points                   = helperMesh.getPoints();
    ::fwData::Mesh::CellTypesMultiArrayType cellTypes             = helperMesh.getCellTypes();
    ::fwData::Mesh::CellDataMultiArrayType cellData               = helperMesh.getCellData();
    ::fwData::Mesh::CellDataOffsetsMultiArrayType cellDataOffsets = helperMesh.getCellDataOffsets();
    ::fwData::Mesh::PointColorsMultiArrayType pointColors         = helperMesh.getPointColors();
    for (unsigned int k = 0; k < nbCells; k++)
    {
        cellTypes[k]       = ::fwData::Mesh::QUAD;
        cellDataOffsets[k] = k * 4;
    }
    for (unsigned int i = 0; i < nbCol; i++)
    {
        for (unsigned int j = 0; j < nbRow; j++)
        {
            unsigned int indexPoint = i + j * nbCol;
            points[indexPoint][0] = pointGrid[indexPoint * 3];
            points[indexPoint][1] = pointGrid[indexPoint * 3 + 1];
            points[indexPoint][2] = pointGrid[indexPoint * 3 + 2];
            /// compute color
            IMAGE_TYPE hu = colorGrid[indexPoint]; // convert hu to grey
            computeGreyLevelFromHounsfield<IMAGE_TYPE>(hu, color);
            pointColors[indexPoint][0] = color[0];
            pointColors[indexPoint][1] = color[1];
            pointColors[indexPoint][2] = color[2];
        }
    }
    unsigned int indexCell = 0;
    for (unsigned int i = 0; i < nbCol - 1; i++)
    {
        for (unsigned int j = 0; j < nbRow - 1; j++)
        {
            unsigned int indexPoint = i + j * nbCol;
            cellData[indexCell]     = indexPoint;
            cellData[indexCell + 1] = indexPoint + 1;
            cellData[indexCell + 2] = indexPoint + nbCol + 1;
            cellData[indexCell + 3] = indexPoint + nbCol;
            indexCell              += 4;
        }
    }
}
/// Fill Image
template<typename IMAGE_TYPE>
void fillImage(const std::vector< IMAGE_TYPE >& colorGrid,
               unsigned int nbCol, unsigned int nbRow, double spacing, ::fwData::Image::sptr image )
{
    // Initialize the image
    image->setType(::fwTools::Type::create< IMAGE_TYPE>());//("int16")
    // Set image size
    ::fwData::Image::SizeType imageSize(3);
    imageSize[0] = nbCol;
    imageSize[1] = nbRow;
    imageSize[2] = 1;
    image->setSize(imageSize);
    // Set image spacing
    std::vector<double> imageSpacing(3);
    imageSpacing[0] = spacing;
    imageSpacing[1] = spacing;
    imageSpacing[2] = spacing;
    image->setSpacing(imageSpacing);
    // Set image origin
    std::vector<double> origin(3);
    origin[0] = 0.0;
    origin[1] = 0.0;
    origin[2] = 0.0;
    image->setOrigin(origin);
    // Allocate the image
    image->allocate(imageSize[0], imageSize[1], imageSize[2], ::fwTools::Type::create< IMAGE_TYPE>());
    // Get image array
    ::fwData::Array::sptr array = image->getDataArray();
    OSLM_DEBUG(" arraySize " << array->getNumberOfElements());
    ::fwDataTools::helper::Array arrayHelper(array);
    IMAGE_TYPE* iter   = arrayHelper.begin<IMAGE_TYPE>();
    IMAGE_TYPE* itrEnd = arrayHelper.end<IMAGE_TYPE>();
    // Fill image aray with the color grid values
    int count = 0;
    for (; iter != itrEnd; ++iter)
    {
        *iter = colorGrid[count];
        count++;
    }
}

} // namespace cpr

#endif // __CPR_COMPUTECPRFUNCTOR_HXX__

