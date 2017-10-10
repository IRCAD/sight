/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGDCMIO_HELPER_DICOMSERIESANONYMIZER_HPP__
#define __FWGDCMIO_HELPER_DICOMSERIESANONYMIZER_HPP__

#include "fwGdcmIO/config.hpp"
#include "fwGdcmIO/helper/DicomAnonymizer.hpp"
#include "fwGdcmIO/helper/DicomSeriesWriter.hpp"
#include "fwGdcmIO/reader/SeriesDB.hpp"

#include <fwMedData/DicomSeries.hpp>

#include <fwTools/ProgressAdviser.hpp>

namespace fwJobs
{
class IJob;
class Aggregator;
}

namespace fwGdcmIO
{
namespace helper
{

/**
 * @brief This class contains helpers to anonymize DicomSeries
 */
class FWGDCMIO_CLASS_API DicomSeriesAnonymizer
{
public:

    fwCoreClassFactoryMacro((DicomSeriesAnonymizer), (()), new DicomSeriesAnonymizer );

    /// Constructor
    FWGDCMIO_API DicomSeriesAnonymizer();

    /// Destructor
    FWGDCMIO_API virtual ~DicomSeriesAnonymizer();

    /// Anonymize the DicomSeries (modify the current object)
    FWGDCMIO_API void anonymize(const ::fwMedData::DicomSeries::sptr& source);

    /// Anonymize the DicomSeries (doesn't modify the current object)
    FWGDCMIO_API void anonymize(const ::fwMedData::DicomSeries::sptr& source,
                                const ::fwMedData::DicomSeries::sptr& destination);

    /// Get job observer
    FWGDCMIO_API SPTR(::fwJobs::Aggregator) getJob() const;

protected:

    /// Dicom anonymizer
    ::fwGdcmIO::helper::DicomAnonymizer m_anonymizer;

    /// Job observer
    SPTR(::fwJobs::Aggregator) m_job;

};

} // namespace helper
} // namespace fwGdcmIO

#endif /*__FWGDCMIO_HELPER_DICOMSERIESANONYMIZER_HPP__*/
