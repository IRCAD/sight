/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDICOMIOEXT_GDCM_DICOMSERIESWRITER_HPP__
#define __FWDICOMIOEXT_GDCM_DICOMSERIESWRITER_HPP__

#include <fwDataIO/writer/GenericObjectWriter.hpp>
#include <fwData/location/Folder.hpp>
#include <fwTools/ProgressAdviser.hpp>

#include "fwDicomIOExt/config.hpp"

namespace fwDicomData
{
class DicomSeries;
}

namespace fwDicomIOExt
{
namespace gdcm
{

/**
 * @brief Write an DicomSeries in DICOM format.
 * @class DicomSeriesWriter
 * @date  2013.
 */
class DicomSeriesWriter : public ::fwDataIO::writer::GenericObjectWriter< ::fwDicomData::DicomSeries >,
                          public ::fwData::location::enableFolder< ::fwDataIO::writer::IObjectWriter >,
                          public ::fwTools::ProgressAdviser
{

public:

    fwCoreClassDefinitionsWithFactoryMacro((DicomSeriesWriter)(
                                               ::fwDataIO::writer::GenericObjectWriter< ::fwDicomData::DicomSeries>),
                                           (()), ::fwDataIO::writer::factory::New< DicomSeriesWriter >);
    fwCoreAllowSharedFromThis();

    /// Constructor. Does nothing
    FWDICOMIOEXT_API DicomSeriesWriter(::fwDataIO::writer::IObjectWriter::Key key);

    /// Write the image series in DICOM format.
    FWDICOMIOEXT_API void write();

    /// Return an empty string
    FWDICOMIOEXT_API std::string  extension();

protected:
    unsigned int m_writeCount;
};

} // namespace gdcm
} // namespace fwDicomIOExt

#endif // __FWDICOMIOEXT_GDCM_DICOMSERIESWRITER_HPP__
