/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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
#include "io/dicom/container/DicomInstance.hpp"

#include <core/log/Logger.hpp>

#include <data/Object.hpp>

#include <gdcmReader.h>

namespace sight::io::dicom::reader::tid
{

/**
 * @brief TemplateID base class used to write templates
 */
template<class DATATYPE>
class IO_DICOM_CLASS_API TemplateID
{
public:

    /**
     * @brief Constructor
     * @param[in] dicomSeries DicomSeries used to access computed tag values.
     * @param[in] reader GDCM reader that must be used to read series.
     * @param[in] instance DICOM instance used to share informations between modules
     * @param[in] object Sight data object
     * @param[in] logger Logger
     */
    IO_DICOM_API TemplateID(
        CSPTR(data::DicomSeries)dicomSeries,
        SPTR(gdcm::Reader)reader,
        SPTR(io::dicom::container::DicomInstance)instance,
        SPTR(DATATYPE)object,
        core::log::Logger::sptr logger
    );

    /// Destructor
    IO_DICOM_API virtual ~TemplateID();

protected:

    /// Dicom Series
    CSPTR(data::DicomSeries) m_dicomSeries;

    /// GDCM Reader
    SPTR(gdcm::Reader) m_reader;

    /// DICOM Instance
    SPTR(io::dicom::container::DicomInstance) m_instance;

    /// Sight Object
    SPTR(DATATYPE) m_object;

    ///Logger
    core::log::Logger::sptr m_logger;
};

//------------------------------------------------------------------------------

template<class DATATYPE>
TemplateID<DATATYPE>::TemplateID(
    CSPTR(data::DicomSeries)dicomSeries,
    SPTR(gdcm::Reader)reader,
    SPTR(io::dicom::container::DicomInstance)instance,
    SPTR(DATATYPE)object,
    core::log::Logger::sptr logger
) :
    m_dicomSeries(std::move(dicomSeries)),
    m_reader(std::move(reader)),
    m_instance(std::move(instance)),
    m_object(std::move(object)),
    m_logger(std::move(logger))
{
    SIGHT_ASSERT("DicomSeries should not be null.", m_dicomSeries);
    SIGHT_ASSERT("Reader should not be null.", m_reader);
    SIGHT_ASSERT("Instance should not be null.", m_instance);
    SIGHT_ASSERT("Object should not be null.", m_object);
    SIGHT_ASSERT("Logger should not be null.", m_logger);
}

//------------------------------------------------------------------------------

template<class DATATYPE> TemplateID<DATATYPE>::~TemplateID()
= default;

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::reader::tid
