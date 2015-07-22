/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDICOMIOEXT_GDCM_HELPER_DICOMSERIESANONYMIZER_HPP__
#define __FWDICOMIOEXT_GDCM_HELPER_DICOMSERIESANONYMIZER_HPP__

#include <fwDicomData/DicomSeries.hpp>

#include <fwTools/ProgressAdviser.hpp>

#include "fwDicomIOExt/gdcm/helper/DicomAnonymizer.hpp"
#include "fwDicomIOExt/gdcm/DicomSeriesDBReader.hpp"
#include "fwDicomIOExt/gdcm/DicomSeriesWriter.hpp"
#include "fwDicomIOExt/config.hpp"

namespace fwDicomIOExt
{
namespace gdcm
{
namespace helper
{

/**
 * @brief   This class contains helpers to anonymize DicomSeries
 * @class   DicomSeriesAnonymizer
 * @date    2013.
 */
class FWDICOMIOEXT_CLASS_API DicomSeriesAnonymizer
{
public:

    fwCoreClassDefinitionsWithFactoryMacro((DicomSeriesAnonymizer), (()), new DicomSeriesAnonymizer );

    /// Constructor
    FWDICOMIOEXT_API DicomSeriesAnonymizer();

    /// Destructor
    FWDICOMIOEXT_API virtual ~DicomSeriesAnonymizer();

    /// Add handler to the anonymizer
    FWDICOMIOEXT_API void addHandler(const ::fwTools::ProgressAdviser::ProgessHandler &handler);

    /// Anonymize the DicomSeries (modify the current object)
    FWDICOMIOEXT_API void anonymize(::fwDicomData::DicomSeries::sptr source);

    /// Anonymize the DicomSeries (doesn't modify the current object)
    FWDICOMIOEXT_API void anonymize(::fwDicomData::DicomSeries::sptr source,
                                    ::fwDicomData::DicomSeries::sptr destination);

protected:

    /// Dicom writer
    ::fwDicomIOExt::gdcm::DicomSeriesWriter::sptr m_writer;

    /// Dicom anonymizer
    ::fwDicomIOExt::gdcm::helper::DicomAnonymizer m_dicomAnonymizer;

    /// Dicom Reader
    ::fwDicomIOExt::gdcm::DicomSeriesDBReader::sptr m_reader;

};

} // namespace helper
} // namespace gdcm
} // namespace fwDicomIOExt

#endif /*__FWDICOMIOEXT_GDCM_HELPER_DICOMSERIESANONYMIZER_HPP__*/
