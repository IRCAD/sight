/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GDCMIO_WRITER_SERIES_HPP__
#define __GDCMIO_WRITER_SERIES_HPP__

#include <fwTools/ProgressAdviser.hpp>

#include <fwDataIO/writer/GenericObjectWriter.hpp>

#include <fwMedData/Series.hpp>
#include <fwMedData/ImageSeries.hpp>

#include <fwData/location/Folder.hpp>

#include "gdcmIO/exception/Failed.hpp"
#include "gdcmIO/config.hpp"

namespace gdcmIO
{
namespace writer
{

/**
 * @brief   This class handles DICOM files writing.
 *
 * It defines needs of data storage and delegates writing to appropriate tools.
 * It manages all related data of one patient.
 *
 * @class   Series
 */
class GDCMIO_CLASS_API Series : public ::fwDataIO::writer::GenericObjectWriter< ::fwMedData::Series >,
                                                  public ::fwData::location::enableFolder< ::fwDataIO::writer::IObjectWriter >,
                                                  public ::fwTools::ProgressAdviser
{

public :

    fwCoreClassDefinitionsWithFactoryMacro( (Series)( ::fwDataIO::writer::GenericObjectWriter< ::fwMedData::Series >),
                                            (()),
                                            ::fwDataIO::writer::factory::New< Series >
                                           );


    GDCMIO_API Series(::fwDataIO::writer::IObjectWriter::Key key);

    GDCMIO_API ~Series();

    /**
     * @brief   Load and start appropriate writing tools.
     * In addition, it creates an adapted folder tree where DICOM files will be stored.
     * It manages writing of the following general DICOM modules : patient, study and series
     *
     * @note    Currently, it handles image, SR and RT document writing.
     */
    GDCMIO_API void write() throw(::gdcmIO::exception::Failed);

    /**
     * Overriding
     * @brief   Do nothing
     */
    GDCMIO_API std::string extension();

private :

    /**
     * @brief   Check if there is comment on acquisition.
     *
     * @param   a_series    ImageSeries to check.
     */
    bool hasDocumentSR( ::fwMedData::ImageSeries::csptr a_series ) const;

};

}   // namespace writer
}   // namespace gdcmIO

#endif // __GDCMIO_WRITER_SERIES_HPP__
