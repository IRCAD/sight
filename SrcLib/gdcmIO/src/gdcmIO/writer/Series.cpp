/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/make_shared.hpp>

#include <fwComEd/Dictionary.hpp>
#include <fwData/Image.hpp>
#include <fwData/Vector.hpp>
#include <fwMedData/Series.hpp>
#include <fwMedData/ImageSeries.hpp>
#include <fwMedData/ModelSeries.hpp>
#include <fwData/PointList.hpp>
#include <fwDataIO/writer/registry/macros.hpp>

#include "gdcmIO/writer/iod/CTMRImageIOD.hpp"
#include "gdcmIO/writer/iod/ComprehensiveSRIOD.hpp"
#include "gdcmIO/writer/iod/SpatialFiducialsIOD.hpp"
#include "gdcmIO/writer/iod/SurfaceSegmentationIOD.hpp"
#include "gdcmIO/writer/Series.hpp"

fwDataIOWriterRegisterMacro(::gdcmIO::writer::Series);

namespace gdcmIO
{
namespace writer
{

//------------------------------------------------------------------------------

Series::Series(::fwDataIO::writer::IObjectWriter::Key key) :
        ::fwData::location::enableFolder< ::fwDataIO::writer::IObjectWriter >(this),
         m_fiducialsExportMode(SPATIAL_FIDUCIALS)
{
}

//------------------------------------------------------------------------------

Series::~Series()
{
}

//------------------------------------------------------------------------------

void Series::write() throw (::gdcmIO::exception::Failed)
{
    ::fwMedData::Series::sptr series = this->getConcreteObject();
    SLM_ASSERT("::fwMedData::Series not instanced", series);

    // TODO: Make the user choose this value and implement EnhancedCTImageIOD/EnhancedMRImageIOD
    bool multiFiles = true;

    // Initialization shared object
    SPTR(::gdcmIO::container::DicomInstance) instance =
            ::boost::make_shared< ::gdcmIO::container::DicomInstance >(series, multiFiles);

    // Retrieve series SOPClassUID
    const std::string& sopClassUID = instance->getCRefSOPClassUID();

    if(sopClassUID == ::gdcm::MediaStorage::GetMSString(::gdcm::MediaStorage::CTImageStorage) ||
            sopClassUID == ::gdcm::MediaStorage::GetMSString(::gdcm::MediaStorage::MRImageStorage))
    {
        ::fwMedData::ImageSeries::sptr imageSeries = ::fwMedData::ImageSeries::dynamicCast(series);
        SLM_ASSERT("::fwMedData::ImageSeries not instanced", imageSeries);
        ::fwData::Image::sptr image = imageSeries->getImage();
        SLM_ASSERT("::fwData::Image not instanced", image);

        // Write image
        ::gdcmIO::writer::iod::CTMRImageIOD imageIOD(instance, this->getFolder());
        imageIOD.write(series);

        ::fwData::PointList::sptr landmarks =
                image->getField< ::fwData::PointList >(::fwComEd::Dictionary::m_imageLandmarksId);
        ::fwData::Vector::sptr distances =
                image->getField< ::fwData::Vector >(::fwComEd::Dictionary::m_imageDistancesId);
        if((landmarks && !landmarks->getCRefPoints().empty()) || (distances && !distances->empty()))
        {
            // Write Landmarks and Distances
            if(m_fiducialsExportMode == SPATIAL_FIDUCIALS)
            {
                ::gdcmIO::writer::iod::SpatialFiducialsIOD spatialFiducialsIOD(instance, this->getFolder());
                spatialFiducialsIOD.write(series);
            }
            else
            {
                ::gdcmIO::writer::iod::ComprehensiveSRIOD documentIOD(instance, this->getFolder(),
                        m_fiducialsExportMode == COMPREHENSIVE_3D_SR);
                documentIOD.write(series);
            }
        }
    }
    else if(sopClassUID == ::gdcm::MediaStorage::GetMSString(::gdcm::MediaStorage::SurfaceSegmentationStorage))
    {
        SPTR(::gdcmIO::container::DicomInstance) imageInstance = this->getImageInstance();
        ::gdcmIO::writer::iod::SurfaceSegmentationIOD iod(instance, imageInstance, this->getFolder());
        iod.write(series);
    }
    else
    {
        SLM_WARN("Series type not supported.");
    }

    // Push instance into container
    m_dicomInstanceMap[series->getInstanceUID()] = instance;

}

//------------------------------------------------------------------------------

bool Series::hasDocumentSR(::fwMedData::ImageSeries::csptr imageSeries) const
{
    ::fwData::Image::csptr image = imageSeries->getImage();
    SLM_ASSERT("Image not instanced", image);

    ::fwData::PointList::sptr pl;
    pl = image->getField< ::fwData::PointList >(::fwComEd::Dictionary::m_imageLandmarksId);
    // Check if image has landmark and distance
    return ((pl && pl->getPoints().size() > 0) || image->getField(::fwComEd::Dictionary::m_imageDistancesId));
}

//------------------------------------------------------------------------------

SPTR(::gdcmIO::container::DicomInstance) Series::getImageInstance()
{
    SLM_ASSERT("You must have created an image instance before trying to access it.", !m_dicomInstanceMap.empty());
    return m_dicomInstanceMap.begin()->second;
}

//------------------------------------------------------------------------------

std::string Series::extension()
{
    return std::string("");
}

//------------------------------------------------------------------------------

}// namespace writer

} // namespace gdcmIO
