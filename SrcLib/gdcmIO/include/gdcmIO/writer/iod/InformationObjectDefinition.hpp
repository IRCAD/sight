/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GDCMIO_WRITER_IOD_INFORMATIONOBJECTDEFINITION_HPP__
#define __GDCMIO_WRITER_IOD_INFORMATIONOBJECTDEFINITION_HPP__

#include <fwMedData/Series.hpp>

#include "gdcmIO/container/DicomInstance.hpp"
#include "gdcmIO/config.hpp"

namespace gdcmIO
{
namespace writer
{
namespace iod
{

/**
 * @class InformationObjectDefinition
 * @brief InformationObjectDefinition base class used to write DICOM modules
 */
class GDCMIO_CLASS_API InformationObjectDefinition
{

public:

    /**
     * @brief Constructor
     * @param[in] instance DICOM instance used to share information between modules
     */
    GDCMIO_API InformationObjectDefinition(SPTR(::gdcmIO::container::DicomInstance)instance,
                                           ::boost::filesystem::path folderPath);

    /// Destructor
    GDCMIO_API virtual ~InformationObjectDefinition();

    /// Write DICOM file
    GDCMIO_API virtual void write(::fwMedData::Series::sptr series) = 0;

protected:

    /// DICOM Instance
    SPTR(::gdcmIO::container::DicomInstance) m_instance;

    /// Folder Path
    ::boost::filesystem::path m_folderPath;


};

} // namespace iod
} // namespace writer
} // namespace gdcmIO

#endif // __GDCMIO_WRITER_IOD_INFORMATIONOBJECTDEFINITION_HPP__
