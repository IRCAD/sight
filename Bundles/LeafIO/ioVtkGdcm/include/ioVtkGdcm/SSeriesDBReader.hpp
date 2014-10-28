/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IOVTKGDCM_SSERIESDBREADER_HPP__
#define __IOVTKGDCM_SSERIESDBREADER_HPP__

#include <string>
#include <boost/filesystem/path.hpp>

#include <io/IReader.hpp>

#include "ioVtkGdcm/config.hpp"

namespace fwMedData
{
    class SeriesDB;
    class Patient;
}


namespace ioVtkGdcm
{

class IOVTKGDCM_CLASS_API SSeriesDBReader : public ::io::IReader
{

public :
    typedef std::string                  ExtensionType;
    typedef std::vector< ExtensionType > ExtensionsType;

    fwCoreServiceClassDefinitionsMacro ( (SSeriesDBReader)( ::io::IReader) ) ;
    /**
     * @brief   constructor
     *
     */
    IOVTKGDCM_API SSeriesDBReader() throw();

    /**
     * @brief   destructor
     */
    IOVTKGDCM_API virtual ~SSeriesDBReader() throw();

protected:

    /// Override
    IOVTKGDCM_API virtual void starting() throw(::fwTools::Failed);

    /// Override
    IOVTKGDCM_API virtual void stopping() throw(::fwTools::Failed);

    /// Override
    IOVTKGDCM_API void updating() throw(::fwTools::Failed);

    /// Override
    virtual void receiving( CSPTR(::fwServices::ObjectMsg) _msg ) throw(::fwTools::Failed){};

    /// Override
    IOVTKGDCM_API void info(std::ostream &_sstream ) ;

    /// Override
    IOVTKGDCM_API virtual ExtensionsType getSupportedExtensions() ;

    /// Override
    IOVTKGDCM_API virtual std::string getSelectorDialogTitle();

    /// Override
    IOVTKGDCM_API virtual void configureWithIHM();

    /// Return path type managed by the service, here FOLDER
    IOVTKGDCM_API ::io::IOPathType getIOPathType() const;

private :

    void notificationOfDBUpdate();

    SPTR(::fwMedData::SeriesDB) createSeriesDB(const ::boost::filesystem::path& dicomDir);

};

} // namespace ioVtkGdcm

#endif //__IOVTKGDCM_SSERIESDBREADER_HPP__
