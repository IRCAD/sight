/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwGdcmIO/helper/DicomData.hpp"

#include <fwCore/base.hpp>

#include <fwDataTools/helper/Array.hpp>

#include <fwTools/Dispatcher.hpp>
#include <fwTools/DynamicTypeKeyTypeMapping.hpp>
#include <fwTools/IntrinsicTypes.hpp>

#include <gdcmDicts.h>
#include <gdcmGlobal.h>
#include <gdcmPhotometricInterpretation.h>
#include <gdcmPixelFormat.h>
#include <gdcmRescaler.h>
#include <gdcmScanner.h>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/bimap/bimap.hpp>
#include <boost/foreach.hpp>

#include <cmath>

namespace fwGdcmIO
{
namespace helper
{

//------------------------------------------------------------------------------

struct TypeOrderer
{
    bool operator()(const ::fwTools::Type& t1, const ::fwTools::Type& t2) const
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

void DicomData::setSQ(::gdcm::SmartPointer< ::gdcm::SequenceOfItems > sequence, const ::gdcm::Tag& tag,
                      ::gdcm::DataSet& dataset)
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

const ::fwTools::Type DicomData::getPixelType(const ::gdcm::PixelFormat& pixFormat)
{
    SLM_ASSERT("ScalarType '" << pixFormat.GetScalarType() << "' not found in PixelTypeTranslation",
               PixelTypeTranslation.right.find( pixFormat.GetScalarType() )!= PixelTypeTranslation.right.end() );

    return PixelTypeTranslation.right.at(pixFormat.GetScalarType());
}

//------------------------------------------------------------------------------

const ::fwTools::Type DicomData::getPixelType(const ::gdcm::Image& image)
{
    const ::gdcm::PixelFormat& pixelFormat = image.GetPixelFormat();
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

const gdcm::PhotometricInterpretation DicomData::getPhotometricInterpretation(const ::fwData::Image& image)
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
            pi = ::gdcm::PhotometricInterpretation::UNKNOWN;
            break;
    }

    return pi;
}

//------------------------------------------------------------------------------

::gdcm::Surface::VIEWType DicomData::convertToPresentationType(
    ::fwData::Material::RepresentationType representationMode)
{
    switch (representationMode)
    {
        case ::fwData::Material::SURFACE:
            return ::gdcm::Surface::SURFACE;
            break;
        case ::fwData::Material::POINT:
            return ::gdcm::Surface::POINTS;
            break;
        case ::fwData::Material::WIREFRAME:
            return ::gdcm::Surface::WIREFRAME;
            break;
        default:
            SLM_WARN("Representation type not handle (changed to : SURFACE)");
            return ::gdcm::Surface::SURFACE;
    }
}

//------------------------------------------------------------------------------

::fwData::Material::RepresentationType DicomData::convertToRepresentationMode(
    ::gdcm::Surface::VIEWType presentationType)
{
    switch (presentationType)
    {
        case ::gdcm::Surface::SURFACE:
            return ::fwData::Material::SURFACE;
            break;
        case ::gdcm::Surface::WIREFRAME:
            return ::fwData::Material::WIREFRAME;
            break;
        case ::gdcm::Surface::POINTS:
            return ::fwData::Material::POINT;
            break;
        default:
            SLM_WARN("Presentation type not handle (changed to : SURFACE)");
            return ::fwData::Material::SURFACE;
    }
}

//------------------------------------------------------------------------------

const char* DicomData::convertToPresentationTypeString(::fwData::Material::RepresentationType representationMode)
{
    switch (representationMode)
    {
        case ::fwData::Material::SURFACE:
            return "SURFACE";
            break;
        case ::fwData::Material::POINT:
            return "POINTS";
            break;
        case ::fwData::Material::WIREFRAME:
            return "WIREFRAME";
            break;
        default:
            SLM_WARN( "Presentation type not handle (changed to : SURFACE)" );
            return "SURFACE";
    }
}

//------------------------------------------------------------------------------

int DicomData::convertPointToFrameNumber(const ::fwData::Image::csptr& image, const ::fwData::Point::sptr& point)
throw(::fwGdcmIO::exception::Failed)
{
    // Retrieve Z spacing
    double zSpacing = (image->getNumberOfDimensions() > 2) ? (image->getSpacing()[2]) : 1;

    // Retrieve Z coordinate of image origin
    double zOrigin = (image->getNumberOfDimensions() > 2) ? (image->getOrigin()[2]) : 0;

    // Retrieve Z coordinate
    double zCoordinate = static_cast<double>(point->getCoord()[2]);

    // Compute frame number
    unsigned int frameNumber = static_cast<int>(floor((zCoordinate - zOrigin) / zSpacing + 0.5)) + 1;
    FW_RAISE_EXCEPTION_IF(::fwGdcmIO::exception::Failed("Coordinates out of image bounds."),
                          frameNumber < 1 || frameNumber > image->getSize()[2]);

    return frameNumber;
}

//------------------------------------------------------------------------------

double DicomData::convertFrameNumberToZCoordinate(const ::fwData::Image::csptr& image, int frameNumber)
throw(::fwGdcmIO::exception::Failed)
{
    // Retrieve Z spacing
    double zSpacing = (image->getNumberOfDimensions() > 2) ? (image->getSpacing()[2]) : 1;

    // Retrieve Z coordinate of image origin
    double zOrigin = (image->getNumberOfDimensions() > 2) ? (image->getOrigin()[2]) : 0;

    // Compute coordinate
    unsigned int frameIndex = (frameNumber-1);
    FW_RAISE_EXCEPTION_IF(::fwGdcmIO::exception::Failed("Coordinates out of image bounds."),
                          frameIndex >= image->getSize()[2]);
    double zCoordinate = zOrigin + frameIndex * zSpacing;

    return zCoordinate;
}

} //namespace helper

} //namespace fwGdcmIO

