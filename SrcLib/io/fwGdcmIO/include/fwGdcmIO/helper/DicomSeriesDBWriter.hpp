/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#ifndef __FWGDCMIO_HELPER_DICOMSERIESDBWRITER_HPP__
#define __FWGDCMIO_HELPER_DICOMSERIESDBWRITER_HPP__

#include "fwGdcmIO/config.hpp"

#include <fwData/location/Folder.hpp>
#include <fwData/location/SingleFile.hpp>

#include <fwDataIO/writer/GenericObjectWriter.hpp>

#include <string>

namespace fwMedData
{
class SeriesDB;
}

namespace fwJobs
{
class Aggregator;
}

namespace fwGdcmIO
{

namespace helper
{

class DicomAnonymizer;

class FWGDCMIO_CLASS_API DicomSeriesDBWriter :
    public ::fwDataIO::writer::GenericObjectWriter< ::fwMedData::SeriesDB >,
    public ::fwData::location::enableFolder< ::fwDataIO::writer::IObjectWriter >,
    public ::fwData::location::enableSingleFile< ::fwDataIO::writer::IObjectWriter >

{
public:

    fwCoreClassDefinitionsWithFactoryMacro((DicomSeriesDBWriter)
                                           (::fwDataIO::writer::GenericObjectWriter< ::fwMedData::SeriesDB>),
                                           (()), ::fwDataIO::writer::factory::New< DicomSeriesDBWriter >);
    fwCoreAllowSharedFromThis();

    /**
     * @brief Construtor/Destructor
     * @{ */
    FWGDCMIO_API DicomSeriesDBWriter(::fwDataIO::writer::IObjectWriter::Key key);
    FWGDCMIO_API virtual ~DicomSeriesDBWriter();
    /**  @} */

    /// Return an empty string
    FWGDCMIO_API std::string extension() override;

    /// Get job Aggregator
    FWGDCMIO_API SPTR(::fwJobs::Aggregator) getAggregator();

    /// Set anonymizer (if anonymization required)
    FWGDCMIO_API void setAnonymizer(const SPTR(helper::DicomAnonymizer)& anonymizer);

    /// Write the image series in DICOM format.
    FWGDCMIO_API void write() override;

    /// Enable Zip output format
    FWGDCMIO_API void enableZippedArchive(bool enable);

    /// Set Producer
    FWGDCMIO_API void setProducer(std::string producer);

private:

    /// Job observer
    SPTR(::fwJobs::Aggregator) m_aggregator;

    /// Optionnal anonymiser
    SPTR(DicomAnonymizer) m_anonymizer;

    /// Enable zip archive
    bool m_enableZippedArchive;

    /// Producer
    std::string m_producer;

};

} // namespace helper
} // namespace fwGdcmIO

#endif // __FWGDCMIO_HELPER_DICOMSERIESDBWRITER_HPP__
