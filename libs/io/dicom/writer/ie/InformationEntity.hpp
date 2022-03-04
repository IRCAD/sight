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

#include <gdcmWriter.h>

#include <cstdint>

namespace sight::io::dicom
{

namespace writer
{

namespace ie
{

/**
 * @brief InformationEntity base class used to write modules
 */
template<class DATATYPE>
class IO_DICOM_CLASS_API InformationEntity
{
public:

    typedef std::function<void (std::uint64_t)> ProgressCallback;
    typedef std::function<bool ()> CancelRequestedCallback;

    /**
     * @brief Constructor
     * @param[in] writer GDCM writer that must be enriched
     * @param[in] instance DICOM instance used to share information between modules
     * @param[in] object Sight data object
     * @param[in] logger Logger
     * @param[in] progress Progress callback
     * @param[in] cancel Cancel requested callback
     */
    IO_DICOM_API InformationEntity(
        const SPTR(gdcm::Writer)& writer,
        const SPTR(io::dicom::container::DicomInstance)& instance,
        const CSPTR(DATATYPE)& object,
        const core::log::Logger::sptr& logger = nullptr,
        ProgressCallback progress             = nullptr,
        CancelRequestedCallback cancel        = nullptr
    );

    /// Destructor
    IO_DICOM_API virtual ~InformationEntity();

protected:

    /// GDCM Writer
    SPTR(gdcm::Writer) m_writer;

    /// DICOM Instance
    SPTR(io::dicom::container::DicomInstance) m_instance;

    /// Sight Object
    CSPTR(DATATYPE) m_object;

    /// Logger
    core::log::Logger::sptr m_logger;

    /// Progress callback for jobs
    ProgressCallback m_progressCallback;

    /// Cancel information for jobs
    CancelRequestedCallback m_cancelRequestedCallback;
};

//------------------------------------------------------------------------------

template<class DATATYPE>
InformationEntity<DATATYPE>::InformationEntity(
    const SPTR(gdcm::Writer)& writer,
    const SPTR(io::dicom::container::DicomInstance)& instance,
    const CSPTR(DATATYPE)& object,
    const core::log::Logger::sptr& logger,
    ProgressCallback progress,
    CancelRequestedCallback cancel
) :
    m_writer(writer),
    m_instance(instance),
    m_object(object),
    m_logger(logger),
    m_progressCallback(progress),
    m_cancelRequestedCallback(cancel)
{
    SIGHT_ASSERT("Writer should not be null.", writer);
    SIGHT_ASSERT("Instance should not be null.", instance);
    SIGHT_ASSERT("Object should not be null.", object);
}

//------------------------------------------------------------------------------

template<class DATATYPE>
InformationEntity<DATATYPE>::~InformationEntity()
{
}

//------------------------------------------------------------------------------

} // namespace ie

} // namespace writer

} // namespace sight::io::dicom
