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

#include <fwLog/Logger.hpp>

#include <fwMedData/Series.hpp>

#include <boost/filesystem/path.hpp>

#include <cstdint>

namespace fwGdcmIO
{
namespace writer
{
namespace iod
{

/**
 * @brief InformationObjectDefinition base class used to write DICOM modules
 */
class FWGDCMIO_CLASS_API InformationObjectDefinition
{

public:

    typedef std::function< void (std::uint64_t) > ProgressCallback;
    typedef std::function< bool () > CancelRequestedCallback;

    /**
     * @brief Constructor
     * @param[in] instance DICOM instance used to share information between modules
     * @param[in] destinationPath Destination path
     * @param[in] logger Logger
     * @param[in] progress Progress callback
     * @param[in] cancel Cancel requested callback
     */
    FWGDCMIO_API InformationObjectDefinition(const SPTR(::fwGdcmIO::container::DicomInstance)& instance,
                                             const ::boost::filesystem::path& destinationPath,
                                             const ::fwLog::Logger::sptr& logger = nullptr,
                                             ProgressCallback progress           = nullptr,
                                             CancelRequestedCallback cancel      = nullptr);

    /// Destructor
    FWGDCMIO_API virtual ~InformationObjectDefinition();

    /// Write DICOM file
    FWGDCMIO_API virtual void write(const ::fwMedData::Series::csptr& series) = 0;

protected:

    /// DICOM Instance
    SPTR(::fwGdcmIO::container::DicomInstance) m_instance;

    /// Destination Path
    ::boost::filesystem::path m_destinationPath;

    ///Logger
    ::fwLog::Logger::sptr m_logger;

    /// Progress callback for jobs
    ProgressCallback m_progressCallback;

    /// Cancel information for jobs
    CancelRequestedCallback m_cancelRequestedCallback;

};

} // namespace iod
} // namespace writer
} // namespace fwGdcmIO
