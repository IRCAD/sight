/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include "fwGdcmIO/container/sr/DicomSRNode.hpp"
#include "fwGdcmIO/reader/tid/TemplateID.hpp"

#include <fwData/Image.hpp>

#include <fwMedData/DicomSeries.hpp>

namespace fwData
{
class Point;
}

namespace fwGdcmIO
{
namespace reader
{
namespace tid
{

/**
 * @brief Measurement TID
 * @see TID 300
 */
class FWGDCMIO_CLASS_API Measurement : public ::fwGdcmIO::reader::tid::TemplateID< ::fwData::Image >
{

public:
    /**
     * @brief Constructor
     * @param[in] dicomSeries DicomSeries used to access computed tag values.
     * @param[in] reader GDCM reader that must be used to read series.
     * @param[in] instance DICOM instance used to share informations between modules
     * @param[in] image Sight data object
     * @param[in] logger Logger
     */
    FWGDCMIO_API Measurement(const CSPTR(::fwMedData::DicomSeries)& dicomSeries,
                             const SPTR(::gdcm::Reader)& reader,
                             const SPTR(::fwGdcmIO::container::DicomInstance)& instance,
                             const ::fwData::Image::sptr& image,
                             const ::fwLog::Logger::sptr& logger);

    /// Destructor
    FWGDCMIO_API virtual ~Measurement();

    /**
     * @brief Read a measurement node
     * @param[in] node Node that must be read
     */
    FWGDCMIO_API virtual void readNode(const SPTR(::fwGdcmIO::container::sr::DicomSRNode)& node);

protected:

    /**
     * @brief Add a distance into the image
     * @param point1 First point
     * @param point2 Second point
     */
    void addDistance(const SPTR(::fwData::Point)& point1,
                     const SPTR(::fwData::Point)& point2);

};

} // namespace tid
} // namespace reader
} // namespace fwGdcmIO
