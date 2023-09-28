/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "io/dicom/container/sr/DicomSRNode.hpp"
#include "io/dicom/writer/ie/InformationEntity.hpp"

#include <data/Image.hpp>

namespace sight::io::dicom::writer::ie
{

/**
 * @brief Document Information Entity class
 */
class IO_DICOM_CLASS_API Document : public io::dicom::writer::ie::InformationEntity<data::Image>
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
    IO_DICOM_API Document(
        const SPTR(gdcm::Writer)& writer,
        const SPTR(io::dicom::container::DicomInstance)& instance,
        const data::Image::csptr& image,
        bool use3DSR                          = false,
        const core::log::logger::sptr& logger = nullptr,
        ProgressCallback progress             = nullptr,
        CancelRequestedCallback cancel        = nullptr
    );

    /// Destructor
    IO_DICOM_API ~Document() override;

    /**
     * @brief Write SR Document General Module tags
     * @see PS 3.3 C.17.2
     */
    IO_DICOM_API virtual void writeSRDocumentGeneralModule();

    /**
     * @brief Write SR Document Content Module tags
     * @see PS 3.3 C.17.3
     */
    IO_DICOM_API virtual void writeSRDocumentContentModule();

    /**
     * @brief Write SOP Common Module tags
     * @see PS 3.3 C.12.1
     */
    IO_DICOM_API void writeSOPCommonModule();

protected:

    /**
     * @brief Write Pertinent Other Evidence Sequence (0040,A385)
     */
    void writePertinentOtherEvidenceSequence();

    /// True if we must use 3DSR
    bool m_use3DSR;
};

} // namespace sight::io::dicom::writer::ie
