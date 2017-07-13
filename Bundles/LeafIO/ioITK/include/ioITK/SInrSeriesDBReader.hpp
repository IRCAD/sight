/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IOITK_SINRSERIESDBREADER_HPP__
#define __IOITK_SINRSERIESDBREADER_HPP__

#include "ioITK/config.hpp"

#include <fwCore/macros.hpp>

#include <io/IReader.hpp>

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
 */
class IOITK_CLASS_API SInrSeriesDBReader : public ::io::IReader
{

public:
    fwCoreServiceClassDefinitionsMacro( (SInrSeriesDBReader)( ::io::IReader) );

    IOITK_API SInrSeriesDBReader() throw();

    IOITK_API virtual ~SInrSeriesDBReader() throw();

protected:

    /// Does nothing.
    virtual void starting() throw(::fwTools::Failed)
    {
    }

    /// Does nothing.
    virtual void stopping() throw(::fwTools::Failed)
    {
    }

    /// Calls base class implementation
    virtual void configuring() throw(::fwTools::Failed);

    /// Reads inr files specified by user (configure or configureWithIHM) and pushes them into SeriesDB.
    IOITK_API virtual void updating() throw(::fwTools::Failed);

    /**
     * @brief Configure the inr files path.
     *
     * This method is used to find the inr files path using a files selector.
     */
    IOITK_API virtual void configureWithIHM();

    /// Returns managed file type, here FILES
    IOITK_API ::io::IOPathType getIOPathType() const;

private:

    /// Initializes Series with dummy values and Study with specified instanceUID.
    void initSeries(SPTR(::fwMedData::Series) series, const std::string& instanceUID);

    /// Reads specified inr file in image.
    bool createImage( const ::boost::filesystem::path inrFile, SPTR(::fwData::Image) image );
};

} // namespace ioITK

#endif //__IOITK_SINRSERIESDBREADER_HPP__
