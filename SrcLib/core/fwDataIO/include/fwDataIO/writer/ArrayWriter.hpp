/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDATAIO_WRITER_ARRAYWRITER_HPP__
#define __FWDATAIO_WRITER_ARRAYWRITER_HPP__

#include "fwDataIO/config.hpp"
#include "fwDataIO/writer/GenericObjectWriter.hpp"

#include <fwData/Array.hpp>
#include <fwData/location/SingleFile.hpp>

namespace fwDataIO
{
namespace writer
{

/**
 * @brief   Array Writer. Write file format .raw
 *
 *
 * Ircad writer to write a ::fwData::Array on filesystem in a raw format
 */
class FWDATAIO_CLASS_API ArrayWriter :  public GenericObjectWriter< ::fwData::Array >,
                                        public ::fwData::location::enableSingleFile< ::fwDataIO::writer::IObjectWriter >
{

public:

    fwCoreClassDefinitionsWithFactoryMacro((ArrayWriter)(GenericObjectWriter< ::fwData::Array>),
                                           (()),
                                           ::fwDataIO::writer::factory::New< ArrayWriter >
                                           )

    /// Constructor. Do nothing.
    FWDATAIO_API ArrayWriter(::fwDataIO::writer::IObjectWriter::Key key);

    /// Destructor. Do nothing.
    FWDATAIO_API virtual ~ArrayWriter();

    /// Read the file with zlib API.
    FWDATAIO_API virtual void write() override;

    /// Defines extension supported by this writer ".raw"
    FWDATAIO_API virtual std::string extension() override;

};

} // namespace writer
} // namespace fwDataIO

#endif // __FWDATAIO_WRITER_ARRAYWRITER_HPP__
