/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "ioITK/config.hpp"

#include <fwCore/macros.hpp>

#include <fwIO/IReader.hpp>

#include <boost/filesystem/path.hpp>

#include <string>

namespace fwData
{
class Image;
}

namespace fwMedData
{
class Series;
}

namespace ioITK
{

/**
 * @brief Reads inr files and pushes them into SeriesDB.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::ioITK::SInrSeriesDBReader">
       <inout key="data" uid="..." />
       <file>...</file>
       <file>...</file>
       <file>...</file>
       <file>...</file>
   </service>
   @endcode
 * @subsection In-Out In-Out
 * - \b data [::fwMedData::SeriesDB]: store the loaded images.
 * @subsection Configuration Configuration
 * - \b file (optional): path of the images to load, if it not defined, 'configureWithIHM()' should be called to define
 * the path.
 */
class IOITK_CLASS_API SInrSeriesDBReader : public ::fwIO::IReader
{

public:
    fwCoreServiceClassDefinitionsMacro( (SInrSeriesDBReader)( ::fwIO::IReader) );

    IOITK_API SInrSeriesDBReader() noexcept;

    IOITK_API virtual ~SInrSeriesDBReader() noexcept;

protected:

    /// Does nothing.
    virtual void starting() override
    {
    }

    /// Does nothing.
    virtual void stopping() override
    {
    }

    /// Calls base class implementation
    virtual void configuring() override;

    /// Reads inr files specified by user (configure or configureWithIHM) and pushes them into SeriesDB.
    IOITK_API virtual void updating() override;

    /**
     * @brief Configure the inr files path.
     *
     * This method is used to find the inr files path using a files selector.
     */
    IOITK_API virtual void configureWithIHM() override;

    /// Returns managed file type, here FILES
    IOITK_API ::fwIO::IOPathType getIOPathType() const override;

private:

    /// Initializes Series with dummy values and Study with specified instanceUID.
    void initSeries(SPTR(::fwMedData::Series) series, const std::string& instanceUID);

    /// Reads specified inr file in image.
    bool createImage( const ::boost::filesystem::path inrFile, SPTR(::fwData::Image) image );
};

} // namespace ioITK
