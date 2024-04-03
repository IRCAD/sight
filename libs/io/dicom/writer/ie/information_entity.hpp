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

#include <sight/io/dicom/config.hpp>

#include "io/dicom/container/dicom_instance.hpp"

#include <core/log/logger.hpp>

#include <data/object.hpp>

#include <gdcmWriter.h>

#include <cstdint>

namespace sight::io::dicom::writer::ie
{

/**
 * @brief InformationEntity base class used to write modules
 */
template<class DATATYPE>
class SIGHT_IO_DICOM_CLASS_API information_entity
{
public:

    using progress_callback         = std::function<void (std::uint64_t)>;
    using cancel_requested_callback = std::function<bool ()>;

    /**
     * @brief Constructor
     * @param[in] _writer GDCM writer that must be enriched
     * @param[in] _instance DICOM instance used to share information between modules
     * @param[in] _object Sight data object
     * @param[in] _logger Logger
     * @param[in] _progress Progress callback
     * @param[in] _cancel Cancel requested callback
     */
    SIGHT_IO_DICOM_API information_entity(
        SPTR(gdcm::Writer)_writer,
        SPTR(io::dicom::container::dicom_instance)_instance,
        CSPTR(DATATYPE)_object,
        core::log::logger::sptr _logger   = nullptr,
        progress_callback _progress       = nullptr,
        cancel_requested_callback _cancel = nullptr
    );

    /// Destructor
    SIGHT_IO_DICOM_API virtual ~information_entity();

protected:

    /// GDCM Writer
    SPTR(gdcm::Writer) m_writer;

    /// DICOM Instance
    SPTR(io::dicom::container::dicom_instance) m_instance;

    /// Sight Object
    CSPTR(DATATYPE) m_object;

    /// Logger
    core::log::logger::sptr m_logger;

    /// Progress callback for jobs
    progress_callback m_progress_callback;

    /// Cancel information for jobs
    cancel_requested_callback m_cancel_requested_callback;
};

//------------------------------------------------------------------------------

template<class DATATYPE>
information_entity<DATATYPE>::information_entity(
    SPTR(gdcm::Writer)_writer,
    SPTR(io::dicom::container::dicom_instance)_instance,
    CSPTR(DATATYPE)_object,
    core::log::logger::sptr _logger,
    progress_callback _progress,
    cancel_requested_callback _cancel
) :
    m_writer(std::move(_writer)),
    m_instance(std::move(_instance)),
    m_object(std::move(_object)),
    m_logger(std::move(_logger)),
    m_progress_callback(std::move(_progress)),
    m_cancel_requested_callback(std::move(_cancel))
{
    SIGHT_ASSERT("Writer should not be null.", m_writer);
    SIGHT_ASSERT("Instance should not be null.", m_instance);
    SIGHT_ASSERT("Object should not be null.", m_object);
}

//------------------------------------------------------------------------------

template<class DATATYPE> information_entity<DATATYPE>::~information_entity()
= default;

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::writer::ie
