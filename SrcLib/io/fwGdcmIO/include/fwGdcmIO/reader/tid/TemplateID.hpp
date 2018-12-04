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

#include "fwGdcmIO/config.hpp"
#include "fwGdcmIO/container/DicomInstance.hpp"

#include <fwData/Object.hpp>

#include <fwLog/Logger.hpp>

#include <gdcmReader.h>

namespace fwGdcmIO
{
namespace reader
{
namespace tid
{

/**
 * @brief TemplateID base class used to write templates
 */
template< class DATATYPE >
class FWGDCMIO_CLASS_API TemplateID
{

public:
    /**
     * @brief Constructor
     * @param[in] dicomSeries DicomSeries used to access computed tag values.
     * @param[in] reader GDCM reader that must be used to read series.
     * @param[in] instance DICOM instance used to share informations between modules
     * @param[in] object FW4SPL data object
     * @param[in] logger Logger
     */
    FWGDCMIO_API TemplateID(const CSPTR(::fwMedData::DicomSeries)& dicomSeries,
                            const SPTR(::gdcm::Reader)& reader,
                            const SPTR(::fwGdcmIO::container::DicomInstance)& instance,
                            const SPTR(DATATYPE)& object,
                            const ::fwLog::Logger::sptr& logger);

    /// Destructor
    FWGDCMIO_API virtual ~TemplateID();

protected:

    /// Dicom Series
    CSPTR(::fwMedData::DicomSeries) m_dicomSeries;

    /// GDCM Reader
    SPTR(::gdcm::Reader) m_reader;

    /// DICOM Instance
    SPTR(::fwGdcmIO::container::DicomInstance) m_instance;

    /// FW4SPL Object
    SPTR(DATATYPE) m_object;

    ///Logger
    ::fwLog::Logger::sptr m_logger;
};

//------------------------------------------------------------------------------

template< class DATATYPE >
TemplateID<DATATYPE>::TemplateID(const CSPTR(::fwMedData::DicomSeries)& dicomSeries,
                                 const SPTR(::gdcm::Reader)& reader,
                                 const SPTR(::fwGdcmIO::container::DicomInstance)& instance,
                                 const SPTR(DATATYPE)& object,
                                 const ::fwLog::Logger::sptr& logger) :
    m_dicomSeries(dicomSeries),
    m_reader(reader),
    m_instance(instance),
    m_object(object),
    m_logger(logger)
{
    SLM_ASSERT("DicomSeries should not be null.", dicomSeries);
    SLM_ASSERT("Reader should not be null.", reader);
    SLM_ASSERT("Instance should not be null.", instance);
    SLM_ASSERT("Object should not be null.", object);
    SLM_ASSERT("Logger should not be null.", logger);
}

//------------------------------------------------------------------------------

template< class DATATYPE >
TemplateID<DATATYPE>::~TemplateID()
{
}

//------------------------------------------------------------------------------

} // namespace tid
} // namespace reader
} // namespace fwGdcmIO
