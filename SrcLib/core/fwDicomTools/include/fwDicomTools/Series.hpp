/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDICOMTOOLS_SERIES_HPP__
#define __FWDICOMTOOLS_SERIES_HPP__

#include "fwDicomTools/config.hpp"

#include <fwCore/macros.hpp>

namespace fwMedData
{
class DicomSeries;
}

namespace fwMedData
{
class ImageSeries;
class ModelSeries;
}

namespace fwDicomTools
{

/**
 * @brief DICOM Series Helper
 */
class FWDICOMTOOLS_CLASS_API Series
{
public:
    /**
     * @brief Convert a DicomSeries to an ImageSeries
     * @param[in] series DicomSeries that must be converted
     */
    FWDICOMTOOLS_API static SPTR(::fwMedData::ImageSeries) convertToImageSeries(
        const SPTR(::fwMedData::DicomSeries)& series);

    /**
     * @brief Convert a DicomSeries to a ModelSeries
     * @param[in] series DicomSeries that must be converted
     */
    FWDICOMTOOLS_API static SPTR(::fwMedData::ModelSeries) convertToModelSeries(
        const SPTR(::fwMedData::DicomSeries)& series);


};

} //fwDicomTools


#endif /* __FWDICOMTOOLS_SERIES_HPP__ */
