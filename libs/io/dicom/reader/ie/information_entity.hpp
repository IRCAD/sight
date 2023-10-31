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

#include "io/dicom/config.hpp"
#include "io/dicom/container/dicom_instance.hpp"

#include <core/log/logger.hpp>

#include <data/object.hpp>

#include <gdcmReader.h>

#include <cstdint>

namespace sight::data
{

class dicom_series;

} // namespace sight::data

namespace sight::io::dicom::reader::ie
{

/**
 * @brief InformationEntity base class used to read modules
 */
template<class DATATYPE>
class IO_DICOM_CLASS_API information_entity
{
public:

    using progress_callback         = std::function<void (std::uint64_t)>;
    using cancel_requested_callback = std::function<bool ()>;

    /**
     * @brief Constructor
     * @param[in] dicomSeries DicomSeries used to access computed tag values.
     * @param[in] reader GDCM reader that must be used to read series.
     * @param[in] instance DICOM instance used to share informations between modules
     * @param[in] object Sight data object
     * @param[in] logger Logger
     * @param[in] progress Progress callback
     * @param[in] cancel Cancel requested callback
     */
    IO_DICOM_API information_entity(
        CSPTR(data::dicom_series)_dicom_series,
        SPTR(gdcm::Reader)_reader,
        SPTR(io::dicom::container::dicom_instance)_instance,
        SPTR(DATATYPE)_object,
        core::log::logger::sptr _logger   = nullptr,
        progress_callback _progress       = nullptr,
        cancel_requested_callback _cancel = nullptr
    );

    /// Destructor
    IO_DICOM_API virtual ~information_entity();

protected:

    /// Dicom Series
    CSPTR(data::dicom_series) m_dicom_series;

    /// GDCM Reader
    SPTR(gdcm::Reader) m_reader;

    /// DICOM Instance
    SPTR(io::dicom::container::dicom_instance) m_instance;

    /// Sight Object
    SPTR(DATATYPE) m_object;

    ///Logger
    core::log::logger::sptr m_logger;

    /// Progress callback for jobs
    progress_callback m_progress_callback;

    /// Cancel information for jobs
    cancel_requested_callback m_cancel_requested_callback;
};

//------------------------------------------------------------------------------

template<class DATATYPE>
information_entity<DATATYPE>::information_entity(
    CSPTR(data::dicom_series)_dicom_series,
    SPTR(gdcm::Reader)_reader,
    SPTR(io::dicom::container::dicom_instance)_instance,
    SPTR(DATATYPE)_object,
    core::log::logger::sptr _logger,
    progress_callback _progress,
    cancel_requested_callback _cancel
) :
    m_dicom_series(std::move(_dicom_series)),
    m_reader(std::move(_reader)),
    m_instance(std::move(_instance)),
    m_object(std::move(_object)),
    m_logger(std::move(_logger)),
    m_progress_callback(std::move(_progress)),
    m_cancel_requested_callback(std::move(_cancel))
{
    SIGHT_ASSERT("DicomSeries should not be null.", m_dicom_series);
    SIGHT_ASSERT("Reader should not be null.", m_reader);
    SIGHT_ASSERT("Instance should not be null.", m_instance);
    SIGHT_ASSERT("Object should not be null.", m_object);
    SIGHT_ASSERT("Logger should not be null.", m_logger);
}

//------------------------------------------------------------------------------

template<class DATATYPE> information_entity<DATATYPE>::~information_entity()
= default;

} // namespace sight::io::dicom::reader::ie
