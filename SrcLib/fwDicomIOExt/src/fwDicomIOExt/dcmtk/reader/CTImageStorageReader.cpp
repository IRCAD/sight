/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <algorithm>

#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmnet/diutil.h>
#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmdata/dcdeftag.h>
#include <dcmtk/dcmimgle/dcmimage.h>

#include <boost/assign/list_of.hpp>
#include <boost/assign/std/vector.hpp>

#include <fwComEd/helper/Array.hpp>
#include <fwData/Image.hpp>
#include <fwMath/VectorFunctions.hpp>
#include <fwMedData/ImageSeries.hpp>
#include <fwTools/Type.hpp>

#include "fwDicomIOExt/dcmtk/reader/CTImageStorageReader.hpp"

namespace fwDicomIOExt
{
namespace dcmtk
{
namespace reader
{

CTImageStorageReader::CTImageStorageReader(::fwMedData::Series::sptr series) : IObjectReader(series)
{
    ::fwMedData::ImageSeries::sptr imageSeries = ::fwMedData::ImageSeries::dynamicCast(series);
    SLM_ASSERT("The series must be an ::fwMedData::ImageSeries.", imageSeries);
}

//-----------------------------------------------------------------------------

CTImageStorageReader::~CTImageStorageReader()
{
}

//-----------------------------------------------------------------------------

void CTImageStorageReader::addInstance(DcmDataset* dataset, const std::string& filename)
{
    m_instanceFiles.push_back(filename);
}

//-----------------------------------------------------------------------------

SPTR(::fwMedData::Series) CTImageStorageReader::create()
{
    this->sortInstances();

    ::fwData::Image::sptr image = ::fwData::Image::New();
    DcmFileFormat fileFormat;
    OFCondition status;
    DcmDataset* dataset;

    //Get informations from the first instance
    status = fileFormat.loadFile(m_instanceFiles.begin()->c_str());
    DicomImage dicomImage(m_instanceFiles.begin()->c_str());
    FW_RAISE_IF("Unable to read the file: \""+*m_instanceFiles.begin()+"\"",
            status.bad() || dicomImage.getStatus() != EIS_Normal);
    dataset = fileFormat.getDataset();


    //Spacing
    double spacing[3];
    dataset->findAndGetFloat64(DCM_PixelSpacing,spacing[0],0);
    dataset->findAndGetFloat64(DCM_PixelSpacing,spacing[1],1);
    dataset->findAndGetFloat64(DCM_SpacingBetweenSlices,spacing[2]);
    image->setSpacing(std::vector< double >(spacing, spacing+3));

    //Origin
    //TODO: Compute the correct origin
    double imagePosition[3];
    dataset->findAndGetFloat64(DCM_ImagePositionPatient, imagePosition[0], 0);
    dataset->findAndGetFloat64(DCM_ImagePositionPatient, imagePosition[1], 1);
    dataset->findAndGetFloat64(DCM_ImagePositionPatient, imagePosition[2], 2);
    image->setOrigin(::boost::assign::list_of(imagePosition[0])(imagePosition[1])(imagePosition[2]));

    //Size
    unsigned short rows, columns, depth;
    dataset->findAndGetUint16(DCM_Rows,rows);
    dataset->findAndGetUint16(DCM_Columns,columns);
    depth = m_instanceFiles.size();
    image->setSize(::boost::assign::list_of(rows)(columns)(depth));

    //Window Center
    double windowCenter;
    dataset->findAndGetFloat64(DCM_WindowCenter,windowCenter);
    image->setWindowCenter(windowCenter);

    //Window Width
    double windowWidth;
    dataset->findAndGetFloat64(DCM_WindowWidth,windowWidth);
    image->setWindowWidth(windowWidth);

    //Number of components
    OFString photometricInterpretation;
    dataset->findAndGetOFString(DCM_PhotometricInterpretation, photometricInterpretation);
    std::string photometricInterpretationStr = photometricInterpretation.c_str();

    if(photometricInterpretationStr == "MONOCHROME2")
    {
        image->setNumberOfComponents(1);
    }
    else if(photometricInterpretationStr == "RGB" || photometricInterpretationStr == "YBR")
    {
        image->setNumberOfComponents(3);
    }
    else if(photometricInterpretationStr == "ARGB" || photometricInterpretationStr == "CMYK")
    {
        image->setNumberOfComponents(4);
    }
    else
    {
        FW_RAISE ( "The photometric interpretation \"" << photometricInterpretationStr << "\" is not supported.");
    }

    //Type
    unsigned short bitsAllocated;
    dataset->findAndGetUint16(DCM_BitsAllocated,bitsAllocated);
    switch(bitsAllocated)
    {
        case 8:
        {
            image->setType(::fwTools::Type::s_INT8);
            break;
        }
        case 16:
        {
            image->setType(::fwTools::Type::s_INT16);
            break;
        }
        case 32:
        {
            image->setType(::fwTools::Type::s_INT32);
            break;
        }
        case 64:
        {
            image->setType(::fwTools::Type::s_INT64);
            break;
        }
        default:
        {
            image->setType(::fwTools::Type::s_UNSPECIFIED_TYPE);
            break;
        }
    }

    //Allocate image
    image->allocate();
    ::fwData::Array::sptr array = image->getDataArray();
    ::fwComEd::helper::Array arrayHelper(array);

    // Read every instances
    unsigned short x, y, z;
    x = y = z = 0;
    unsigned int sliceSize = rows*columns*sizeof(Uint16);

    Uint16* tempoBuffer = new Uint16[rows*columns*depth];
    Uint16* arrayBuffer = static_cast< Uint16* >(arrayHelper.getBuffer());

    for(InstanceFileContainerType::iterator it=m_instanceFiles.begin(); it != m_instanceFiles.end(); ++it)
    {
        status = fileFormat.loadFile(it->c_str());
        FW_RAISE_IF("Unable to read the file: \""+*it+"\"", status.bad());

        dataset = fileFormat.getDataset();
        const Uint16* pixelData;
        dataset->findAndGetUint16Array(DCM_PixelData, pixelData);

        //Add the slice to the temporary buffer
        memcpy(tempoBuffer+z*(rows*columns), pixelData, sliceSize);

        ++z; //Next frame
    }

    //Copy the temporary buffer to the final buffer
    for(x=0; x < columns; ++x)
    {
        for(y=0; y < rows; ++y)
        {
            unsigned int yshift = y*rows;
            unsigned int zshift = columns*rows;
            for(z=0; z < depth; ++z)
            {
                arrayBuffer[x + yshift + z*zshift] = tempoBuffer[x + y*rows + z*columns*rows];
            }
        }
    }

    delete tempoBuffer;

    ::fwMedData::ImageSeries::sptr imageSeries = ::fwMedData::ImageSeries::dynamicCast(m_series);
    SLM_ASSERT("The series must be an ::fwMedData::ImageSeries.", imageSeries);
    imageSeries->setImage(image);


    return m_series;
}

//-----------------------------------------------------------------------------

void CTImageStorageReader::sortInstances()
{
    typedef std::map< double, std::string > SortedFileMapType;
    SortedFileMapType sortedFiles;

    DcmFileFormat fileFormat;
    OFCondition status;
    DcmDataset* dataset;
    for(InstanceFileContainerType::iterator it=m_instanceFiles.begin(); it != m_instanceFiles.end(); ++it)
    {
        status = fileFormat.loadFile(it->c_str());
        FW_RAISE_IF("Unable to read the file: \""+*it+"\"", status.bad());
        dataset = fileFormat.getDataset();

        fwVec3d imagePosition;
        for(unsigned int i=0; i < 3; ++i)
        {
            dataset->findAndGetFloat64(DCM_ImagePositionPatient, imagePosition[i], i);
        }

        fwVec3d imageOrientationU;
        fwVec3d imageOrientationV;
        for(unsigned int i=0; i < 3; ++i)
        {
            dataset->findAndGetFloat64(DCM_ImageOrientationPatient, imageOrientationU[i], i);
            dataset->findAndGetFloat64(DCM_ImageOrientationPatient, imageOrientationV[i], i+3);
        }

        //Compute Z direction (cross product)
        fwVec3d zVector = ::fwMath::cross(imageOrientationU, imageOrientationV);

        //Compute dot product to get the index
        double index = ::fwMath::dot(imagePosition, zVector);

        sortedFiles[index] = *it;
    }

    m_instanceFiles.clear();

    for(SortedFileMapType::iterator it = sortedFiles.begin(); it != sortedFiles.end(); ++it)
    {
        m_instanceFiles.push_back(it->second);
    }


}

} //dcmtk
} //reader
} //fwDicomIOExt
