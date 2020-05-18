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
#include "fwGdcmIO/writer/ie/InformationEntity.hpp"

#include <fwData/Image.hpp>

namespace fwGdcmIO
{
namespace writer
{
namespace ie
{

/**
 * @brief Document Information Entity class
 */
class FWGDCMIO_CLASS_API Document : public ::fwGdcmIO::writer::ie::InformationEntity< ::fwData::Image >
{

public:
    /**
     * @brief Constructor
     * @param[in] writer GDCM writer that must be enriched
     * @param[in] instance DICOM instance used to share information between modules
     * @param[in] image Image data
     * @param[in] logger Logger
     * @param[in] progress Progress callback
     * @param[in] cancel Cancel requested callback
     */
    FWGDCMIO_API Document(const SPTR(::gdcm::Writer)& writer,
                          const SPTR(::fwGdcmIO::container::DicomInstance)& instance,
                          const ::fwData::Image::csptr& image,
                          bool use3DSR                        = false,
                          const ::fwLog::Logger::sptr& logger = nullptr,
                          ProgressCallback progress           = nullptr,
                          CancelRequestedCallback cancel      = nullptr);

    /// Destructor
    FWGDCMIO_API virtual ~Document();

    /**
     * @brief Write SR Document General Module tags
     * @see PS 3.3 C.17.2
     */
    FWGDCMIO_API virtual void writeSRDocumentGeneralModule();

    /**
     * @brief Write SR Document Content Module tags
     * @see PS 3.3 C.17.3
     */
    FWGDCMIO_API virtual void writeSRDocumentContentModule();

    /**
     * @brief Write SOP Common Module tags
     * @see PS 3.3 C.12.1
     */
    FWGDCMIO_API void writeSOPCommonModule();

protected:

    /**
     * @brief Write Pertinent Other Evidence Sequence (0040,A385)
     */
    void writePertinentOtherEvidenceSequence();

    /// True if we must use 3DSR
    bool m_use3DSR;

};

} // namespace ie
} // namespace writer
} // namespace fwGdcmIO
