/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#pragma once

#include "io/dicom/container/sr/DicomSRNumNode.hpp"
#include "io/dicom/container/sr/DicomSRSCoordNode.hpp"
#include "io/dicom/container/sr/DicomSRTextNode.hpp"
#include "io/dicom/exception/Failed.hpp"
#include "io/dicom/reader/ie/InformationEntity.hpp"

#include <data/DicomSeries.hpp>
#include <data/Image.hpp>

namespace sight::data
{

class Point;

}

namespace sight::io::dicom
{

namespace reader
{

namespace ie
{

/**
 * @brief Document Information Entity class
 * It search and read a specific template from the root of the document SR.
 * Currently, the template is TID 1400 (taken from PS 3.16) for distances
 * and based on TID 320 (taken from PS 3.16) for landmarks.
 */
class IO_DICOM_CLASS_API Document : public io::dicom::reader::ie::InformationEntity<data::Image>
{
public:

    /**
     * @brief Constructor
     * @param[in] dicomSeries DicomSeries used to access computed tag values.
     * @param[in] reader GDCM reader that must be used to read landmarks and distances.
     * @param[in] instance DICOM instance used to share informations between modules
     * @param[in] image Image data
     * @param[in] logger Logger
     * @param[in] progress Progress callback
     * @param[in] cancel Cancel requested callback
     */
    IO_DICOM_API Document(
        const CSPTR(data::DicomSeries)& dicomSeries,
        const SPTR(::gdcm::Reader)& reader,
        const SPTR(io::dicom::container::DicomInstance)& instance,
        const data::Image::sptr& image,
        const core::log::Logger::sptr& logger = nullptr,
        ProgressCallback progress             = nullptr,
        CancelRequestedCallback cancel        = nullptr
    );

    /// Destructor
    IO_DICOM_API virtual ~Document();

    /**
     * @brief Read the Structured Report
     * @throw io::dicom::exception::Failed
     */
    IO_DICOM_API void readSR();
};

} // namespace ie

} // namespace reader

} // namespace sight::io::dicom
