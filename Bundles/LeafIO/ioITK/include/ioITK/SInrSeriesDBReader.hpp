/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IOITK_SINRSERIESDBREADER_HPP__
#define __IOITK_SINRSERIESDBREADER_HPP__

#include <string>
#include <boost/filesystem/path.hpp>

#include <fwCore/macros.hpp>

#include <io/IReader.hpp>

#include "ioITK/export.hpp"

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

class IOITK_CLASS_API SInrSeriesDBReader : public ::io::IReader
{

public :
    fwCoreServiceClassDefinitionsMacro ( (SInrSeriesDBReader)( ::io::IReader) ) ;

    IOITK_API SInrSeriesDBReader() throw();

    IOITK_API virtual ~SInrSeriesDBReader() throw();

protected:

    /// Override
    virtual void starting() throw(::fwTools::Failed){};

    /// Override
    virtual void stopping() throw(::fwTools::Failed){};

    IOITK_API virtual void updating() throw(::fwTools::Failed) ;

    /// Override
    IOITK_API void info(std::ostream &_sstream ) ;

    /// Override
    IOITK_API virtual void configureWithIHM();

    /// Return managed file type, here FILES
    IOITK_API ::io::IOPathType getIOPathType() const;

private :

    void  initSeries(SPTR(::fwMedData::Series) series, const std::string& instanceUID);

    bool createImage( const ::boost::filesystem::path inrFileDir, SPTR(::fwData::Image) image );
};

} // namespace ioITK

#endif //__IOITK_SINRSERIESDBREADER_HPP__
