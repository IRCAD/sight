/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <cmath>

#include <gdcmGlobal.h>
#include <gdcmDicts.h>
#include <gdcmPhotometricInterpretation.h>
#include <gdcmPixelFormat.h>
#include <gdcmScanner.h>
#include <gdcmRescaler.h>

#include <boost/foreach.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/classification.hpp>

#include <boost/bimap/bimap.hpp>
#include <boost/assign/list_of.hpp>

#include <fwCore/base.hpp>

#include <fwTools/Dispatcher.hpp>
#include <fwTools/IntrinsicTypes.hpp>
#include <fwTools/DynamicTypeKeyTypeMapping.hpp>

#include <fwComEd/helper/Array.hpp>

#include "gdcmIO/helper/DicomData.hpp"

namespace gdcmIO
{
namespace helper
{

//------------------------------------------------------------------------------

struct TypeOrderer
{
    bool operator()(const ::fwTools::Type &t1, const ::fwTools::Type &t2) const
    {
        return t1.string() < t2.string();
    }
};

//------------------------------------------------------------------------------

typedef ::boost::bimaps::bimap< ::boost::bimaps::set_of< ::fwTools::Type, TypeOrderer >,
        ::boost::bimaps::set_of< gdcm::PixelFormat::ScalarType > > TypeTranslator;

//------------------------------------------------------------------------------

TypeTranslator PixelTypeTranslation = ::boost::assign::list_of< TypeTranslator::relation >
        (::fwTools::Type::create("uint8"),  ::gdcm::PixelFormat::UINT8)
        (::fwTools::Type::create("int8"),   ::gdcm::PixelFormat::INT8)
//        ( ::fwTools::Type::create("XXX"), ::gdcm::PixelFormat::UINT12 )   // Unsupported by VTK Render
//        ( ::fwTools::Type::create("XXX"), ::gdcm::PixelFormat::INT12 )    // Unsupported by VTK Render
        (::fwTools::Type::create("uint16"), ::gdcm::PixelFormat::UINT16)
        (::fwTools::Type::create("int16"),  ::gdcm::PixelFormat::INT16)
        (::fwTools::Type::create("uint32"), ::gdcm::PixelFormat::UINT32)
        (::fwTools::Type::create("int32"),  ::gdcm::PixelFormat::INT32)
//        ( ::fwTools::Type::create("XXX"), ::gdcm::PixelFormat::FLOAT16 )  // Unsupported by VTK Render
        (::fwTools::Type::create("float"),  ::gdcm::PixelFormat::FLOAT32)
        (::fwTools::Type::create("double"), ::gdcm::PixelFormat::FLOAT64);

//------------------------------------------------------------------------------

void DicomData::setSQ(::gdcm::SmartPointer< ::gdcm::SequenceOfItems > sequence, const ::gdcm::Tag &tag,
        ::gdcm::DataSet &dataset)
{
    if (sequence->GetNumberOfItems() > 0)
    {
        // Create the sequence of items
        ::gdcm::DataElement dataElement(tag);
        dataElement.SetVR(::gdcm::VR::SQ);
        dataElement.SetValue(*sequence);
        dataElement.SetVL(sequence->GetLength());

        // Insert or replace the sequence of items
        dataset.Insert(dataElement);
    }
}

//------------------------------------------------------------------------------

const ::fwTools::Type DicomData::getPixelType(const ::gdcm::PixelFormat &pixFormat)
{
    SLM_ASSERT("ScalarType '" << pixFormat.GetScalarType() << "' not found in PixelTypeTranslation",
            PixelTypeTranslation.right.find( pixFormat.GetScalarType() )!= PixelTypeTranslation.right.end() );

    return PixelTypeTranslation.right.at(pixFormat.GetScalarType());
}

//------------------------------------------------------------------------------

const ::fwTools::Type DicomData::getPixelType(const ::gdcm::Image &image)
{
    const ::gdcm::PixelFormat &pixelFormat = image.GetPixelFormat();
    SLM_ASSERT("ScalarType '" << pixelFormat.GetScalarType() << "' not found in PixelTypeTranslation",
            PixelTypeTranslation.right.find( pixelFormat.GetScalarType() )!= PixelTypeTranslation.right.end() );

    return PixelTypeTranslation.right.at(pixelFormat.GetScalarType());
}

//------------------------------------------------------------------------------

const gdcm::PixelFormat DicomData::getPixelType(const ::fwData::Image::csptr image)
{
    const ::gdcm::PixelFormat::ScalarType scalarType = PixelTypeTranslation.left.at(image->getType());
    SLM_ASSERT("ScalarType '" << scalarType << "' not found in PixelTypeTranslation",
            PixelTypeTranslation.right.find( scalarType )!= PixelTypeTranslation.right.end() );

    return gdcm::PixelFormat(scalarType);
}

//------------------------------------------------------------------------------

const gdcm::PhotometricInterpretation DicomData::getPhotometricInterpretation(const ::fwData::Image &image)
{
    ::gdcm::PhotometricInterpretation pi;
    size_t components = image.getNumberOfComponents();

    // Attempt a guess (VTK do the same choice)
    switch (components)
    {
    case 1: // It could well be MONOCHROME1
        pi = ::gdcm::PhotometricInterpretation::MONOCHROME2;
        break;
    case 3: // It could well be YBR
        pi = ::gdcm::PhotometricInterpretation::RGB;
        break;
    case 4: // It could well be CMYK
        pi = ::gdcm::PhotometricInterpretation::ARGB;
        break;
    default:
        SLM_ERROR("Photometric interpretation not found");
        pi = ::gdcm::PhotometricInterpretation::UNKNOW;
        break;
    }

    return pi;
}

//------------------------------------------------------------------------------

::gdcm::Surface::VIEWType DicomData::convertToPresentationType(
        ::fwData::Material::REPRESENTATION_MODE representationMode)
{
    switch (representationMode)
    {
    case ::fwData::Material::MODE_SURFACE:
        return ::gdcm::Surface::SURFACE;
        break;
    case ::fwData::Material::MODE_POINT:
        return ::gdcm::Surface::POINTS;
        break;
    case ::fwData::Material::MODE_WIREFRAME:
        return ::gdcm::Surface::WIREFRAME;
        break;
    default:
        SLM_WARN("Representation type not handle (changed to : SURFACE)");
        return ::gdcm::Surface::SURFACE;
    }
}

//------------------------------------------------------------------------------

::fwData::Material::REPRESENTATION_MODE DicomData::convertToRepresentationMode(
        ::gdcm::Surface::VIEWType presentationType)
{
    switch (presentationType)
    {
    case ::gdcm::Surface::SURFACE:
        return ::fwData::Material::MODE_SURFACE;
        break;
    case ::gdcm::Surface::WIREFRAME:
        return ::fwData::Material::MODE_WIREFRAME;
        break;
    case ::gdcm::Surface::POINTS:
        return ::fwData::Material::MODE_POINT;
        break;
    default:
        SLM_WARN("Presentation type not handle (changed to : SURFACE)");
        return ::fwData::Material::MODE_SURFACE;
    }
}

//------------------------------------------------------------------------------

const char* DicomData::convertToPresentationTypeString(::fwData::Material::REPRESENTATION_MODE representationMode)
{
    switch (representationMode)
    {
    case ::fwData::Material::MODE_SURFACE:
        return "SURFACE";
        break;
    case ::fwData::Material::MODE_POINT:
        return "POINTS";
        break;
    case ::fwData::Material::MODE_WIREFRAME:
        return "WIREFRAME";
        break;
    default:
        SLM_WARN( "Presentation type not handle (changed to : SURFACE)" );
        return "SURFACE";
    }
}

//------------------------------------------------------------------------------

int DicomData::convertPointToFrameNumber(const ::fwData::Image::csptr& image, const ::fwData::Point::sptr& point)
throw(::gdcmIO::exception::Failed)
{
    // Retrieve Z spacing
    float zSpacing = (image->getNumberOfDimensions() > 2)?(image->getSpacing()[2]):1;

    // Retrieve Z coordinate of image origin
    float zOrigin = (image->getNumberOfDimensions() > 2)?(image->getOrigin()[2]):0;

    // Retrieve Z coordinate
    float zCoordinate = static_cast<float>(point->getCoord()[2]);

    // Compute frame number
    int frameNumber = floor((zCoordinate - zOrigin) / zSpacing + 0.5) + 1;
    FW_RAISE_EXCEPTION_IF(::gdcmIO::exception::Failed("Coordinates out of image bounds."),
            frameNumber < 1 || frameNumber > image->getSize()[2]);

    return frameNumber;
}

//------------------------------------------------------------------------------

float DicomData::convertFrameNumberToZCoordinate(const ::fwData::Image::csptr& image, int frameNumber)
throw(::gdcmIO::exception::Failed)
{
    // Retrieve Z spacing
    float zSpacing = (image->getNumberOfDimensions() > 2)?(image->getSpacing()[2]):1;

    // Retrieve Z coordinate of image origin
    float zOrigin = (image->getNumberOfDimensions() > 2)?(image->getOrigin()[2]):0;

    // Compute coordinate
    int frameIndex = (frameNumber-1);
    FW_RAISE_EXCEPTION_IF(::gdcmIO::exception::Failed("Coordinates out of image bounds."),
            frameIndex < 0 || frameIndex >= image->getSize()[2]);
    float zCoordinate = zOrigin + frameIndex * zSpacing;

    return zCoordinate;
}

} //namespace helper

} //namespace gdcmIO

