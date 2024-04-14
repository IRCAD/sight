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

#include <gdcmReader.h>

namespace sight::io::dicom::reader::tid
{

/**
 * @brief TemplateID base class used to write templates
 */
template<class DATATYPE>
class SIGHT_IO_DICOM_CLASS_API template_id
{
public:

    /**
     * @brief Constructor
     * @param[in] _dicom_series DicomSeries used to access computed tag values.
     * @param[in] _reader GDCM reader that must be used to read series.
     * @param[in] _instance DICOM instance used to share informations between modules
     * @param[in] _object Sight data object
     * @param[in] _logger Logger
     */
    SIGHT_IO_DICOM_API template_id(
        CSPTR(data::dicom_series)_dicom_series,
        SPTR(gdcm::Reader)_reader,
        SPTR(io::dicom::container::dicom_instance)_instance,
        SPTR(DATATYPE)_object,
        core::log::logger::sptr _logger
    );

    /// Destructor
    SIGHT_IO_DICOM_API virtual ~template_id();

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
};

//------------------------------------------------------------------------------

template<class DATATYPE>
template_id<DATATYPE>::template_id(
    CSPTR(data::dicom_series)_dicom_series,
    SPTR(gdcm::Reader)_reader,
    SPTR(io::dicom::container::dicom_instance)_instance,
    SPTR(DATATYPE)_object,
    core::log::logger::sptr _logger
) :
    m_dicom_series(std::move(_dicom_series)),
    m_reader(std::move(_reader)),
    m_instance(std::move(_instance)),
    m_object(std::move(_object)),
    m_logger(std::move(_logger))
{
    SIGHT_ASSERT("DicomSeries should not be null.", m_dicom_series);
    SIGHT_ASSERT("Reader should not be null.", m_reader);
    SIGHT_ASSERT("Instance should not be null.", m_instance);
    SIGHT_ASSERT("Object should not be null.", m_object);
    SIGHT_ASSERT("Logger should not be null.", m_logger);
}

//------------------------------------------------------------------------------

template<class DATATYPE> template_id<DATATYPE>::~template_id()
= default;

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::reader::tid
