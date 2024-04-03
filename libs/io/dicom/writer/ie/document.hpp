/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include "io/dicom/container/sr/dicom_sr_node.hpp"
#include "io/dicom/writer/ie/information_entity.hpp"

#include <data/image.hpp>

namespace sight::io::dicom::writer::ie
{

/**
 * @brief Document Information Entity class
 */
class SIGHT_IO_DICOM_CLASS_API document : public io::dicom::writer::ie::information_entity<data::image>
{
public:

    /**
     * @brief Constructor
     * @param[in] _writer GDCM writer that must be enriched
     * @param[in] _instance DICOM instance used to share information between modules
     * @param[in] _image Image data
     * @param[in] _logger Logger
     * @param[in] _progress Progress callback
     * @param[in] _cancel Cancel requested callback
     */
    SIGHT_IO_DICOM_API document(
        const SPTR(gdcm::Writer)& _writer,
        const SPTR(io::dicom::container::dicom_instance)& _instance,
        const data::image::csptr& _image,
        bool _use_3d_sr                        = false,
        const core::log::logger::sptr& _logger = nullptr,
        progress_callback _progress            = nullptr,
        cancel_requested_callback _cancel      = nullptr
    );

    /// Destructor
    SIGHT_IO_DICOM_API ~document() override;

    /**
     * @brief Write SR Document General Module tags
     * @see PS 3.3 C.17.2
     */
    SIGHT_IO_DICOM_API virtual void write_sr_document_general_module();

    /**
     * @brief Write SR Document Content Module tags
     * @see PS 3.3 C.17.3
     */
    SIGHT_IO_DICOM_API virtual void write_sr_document_content_module();

    /**
     * @brief Write SOP Common Module tags
     * @see PS 3.3 C.12.1
     */
    SIGHT_IO_DICOM_API void write_sop_common_module();

protected:

    /**
     * @brief Write Pertinent Other Evidence Sequence (0040,A385)
     */
    void write_pertinent_other_evidence_sequence();

    /// True if we must use 3DSR
    bool m_use_3d_sr;
};

} // namespace sight::io::dicom::writer::ie
