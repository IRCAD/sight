/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VTKGDCMIO_IMAGESERIESWRITER_HPP__
#define __VTKGDCMIO_IMAGESERIESWRITER_HPP__

#include <fwDataIO/writer/GenericObjectWriter.hpp>
#include <fwData/location/Folder.hpp>
#include <fwTools/ProgressAdviser.hpp>

#include "vtkGdcmIO/config.hpp"

namespace fwMedData
{
class ImageSeries;
}

namespace vtkGdcmIO
{

class ImageSeriesWriter : public ::fwDataIO::writer::GenericObjectWriter< ::fwMedData::ImageSeries >,
                             public ::fwData::location::enableFolder< ::fwDataIO::writer::IObjectWriter >,
                             public ::fwTools::ProgressAdviser
{

public :

    fwCoreClassDefinitionsWithFactoryMacro((ImageSeriesWriter)( ::fwDataIO::writer::GenericObjectWriter< ::fwMedData::ImageSeries>),
                                           (()),
                                           ::fwDataIO::writer::factory::New< ImageSeriesWriter >
                                          );
    fwCoreAllowSharedFromThis();

    VTKGDCMIO_API ImageSeriesWriter(::fwDataIO::writer::IObjectWriter::Key key);

    VTKGDCMIO_API void write();

    VTKGDCMIO_API std::string  extension();
};

} // namespace vtkGdcmIO

#endif // __VTKGDCMIO_IMAGESERIESWRITER_HPP__
