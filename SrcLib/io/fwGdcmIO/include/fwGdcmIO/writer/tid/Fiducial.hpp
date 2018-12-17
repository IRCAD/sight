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
#include "fwGdcmIO/writer/tid/TemplateID.hpp"

#include <fwData/Image.hpp>

namespace fwData
{
class Point;
}

namespace fwGdcmIO
{
namespace writer
{
namespace tid
{

/**
 * @brief Fiducial TID
 * @see TID t1tt3 FIXME: Set the final TID
 */
class FWGDCMIO_CLASS_API Fiducial : public ::fwGdcmIO::writer::tid::TemplateID< ::fwData::Image >
{

public:
    /**
     * @brief Constructor
     * @param[in] writer GDCM writer that must be enriched
     * @param[in] instance DICOM instance used to share informations between modules
     * @param[in] image Image data
     */
    FWGDCMIO_API Fiducial(const SPTR(::gdcm::Writer)& writer,
                          const SPTR(::fwGdcmIO::container::DicomInstance)& instance,
                          const ::fwData::Image::csptr& image);

    /// Destructor
    FWGDCMIO_API virtual ~Fiducial();

    /**
     * @brief Create nodes according to the template
     * @param[in] parent Parent node
     * @param[in] useSCoord3D True if we must write SCOORD3D, false if we must write SCOORD
     */
    FWGDCMIO_API virtual void createNodes(const SPTR(::fwGdcmIO::container::sr::DicomSRNode)& parent,
                                          bool useSCoord3D = true);

protected:

    /**
     * @brief Create a fiducial node
     * @param[in] parent Parent node
     * @param[in] point Fiducial (landmark) point
     * @param[in] id ID of the ficudial
     * @param[in] useSCoord3D True if we must use 3D coordinates
     */
    void createFiducial(const SPTR(::fwGdcmIO::container::sr::DicomSRNode)& parent,
                        const CSPTR(::fwData::Point)& point,
                        unsigned int id,
                        bool useSCoord3D);

};

} // namespace tid
} // namespace writer
} // namespace fwGdcmIO
