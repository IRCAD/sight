/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGDCMIO_WRITER_IOD_INFORMATIONOBJECTDEFINITION_HPP__
#define __FWGDCMIO_WRITER_IOD_INFORMATIONOBJECTDEFINITION_HPP__

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

    typedef std::function< void(std::uint64_t) > ProgressCallback;
    typedef std::function< bool() > CancelRequestedCallback;

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
                                             ProgressCallback progress = nullptr,
                                             CancelRequestedCallback cancel = nullptr);

    /// Destructor
    FWGDCMIO_API virtual ~InformationObjectDefinition();

    /// Write DICOM file
    FWGDCMIO_API virtual void write(const ::fwMedData::Series::sptr& series) = 0;

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

#endif // __FWGDCMIO_WRITER_IOD_INFORMATIONOBJECTDEFINITION_HPP__
