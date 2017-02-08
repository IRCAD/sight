/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGDCMIO_WRITER_IOD_INFORMATIONOBJECTDEFINITION_HPP__
#define __FWGDCMIO_WRITER_IOD_INFORMATIONOBJECTDEFINITION_HPP__

#include "fwGdcmIO/config.hpp"
#include "fwGdcmIO/container/DicomInstance.hpp"

#include <fwMedData/Series.hpp>

#include <boost/filesystem/path.hpp>

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

    /**
     * @brief Constructor
     * @param[in] instance DICOM instance used to share information between modules
     */
    FWGDCMIO_API InformationObjectDefinition(SPTR(::fwGdcmIO::container::DicomInstance)instance,
                                             ::boost::filesystem::path folderPath);

    /// Destructor
    FWGDCMIO_API virtual ~InformationObjectDefinition();

    /// Write DICOM file
    FWGDCMIO_API virtual void write(::fwMedData::Series::sptr series) = 0;

protected:

    /// DICOM Instance
    SPTR(::fwGdcmIO::container::DicomInstance) m_instance;

    /// Folder Path
    ::boost::filesystem::path m_folderPath;

};

} // namespace iod
} // namespace writer
} // namespace fwGdcmIO

#endif // __FWGDCMIO_WRITER_IOD_INFORMATIONOBJECTDEFINITION_HPP__
