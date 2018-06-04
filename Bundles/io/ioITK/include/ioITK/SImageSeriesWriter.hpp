/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "ioITK/config.hpp"

#include <fwIO/IWriter.hpp>

#include <boost/filesystem/path.hpp>

#include <string>

namespace ioITK
{

/**
 * @brief Writer for .inr.gz image series
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::ioITK::SImageSeriesWriter">
       <inout key="data" uid="..." />
       <file>...</file>
   </service>
   @endcode
 * @subsection In-Out In-Out
 * - \b data [::fwMedData::ImageSeries]: image series  to save.
 * @subsection Configuration Configuration
 * - \b file (optional): path of the file to save, if it not defined, 'configureWithIHM()' should be called to define
 * the path.
 */
class IOITK_CLASS_API SImageSeriesWriter : public ::fwIO::IWriter
{

public:
    fwCoreServiceClassDefinitionsMacro( (SImageSeriesWriter)( ::fwIO::IWriter) );

    IOITK_API SImageSeriesWriter() noexcept;

    IOITK_API virtual ~SImageSeriesWriter() noexcept;

protected:

    /// Override
    IOITK_API virtual void starting() override;

    /// Override
    IOITK_API virtual void stopping() override;

    /// Override
    IOITK_API virtual void configuring() override;

    /// Override
    IOITK_API void updating() override;

    /// Override
    IOITK_API void info(std::ostream& _sstream ) override;

    /// Override
    IOITK_API virtual void configureWithIHM() override;

    /// Return managed file type, here FILE
    IOITK_API ::fwIO::IOPathType getIOPathType() const override;

};

} // namespace ioITK
