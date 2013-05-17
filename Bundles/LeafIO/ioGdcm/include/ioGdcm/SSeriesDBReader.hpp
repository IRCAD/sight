/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _IOGDCM_SSERIESDBREADER_HPP_
#define _IOGDCM_SSERIESDBREADER_HPP_

#include <vector>
#include <string>
#include <boost/filesystem/path.hpp>

#include <io/IReader.hpp>

#include "ioGdcm/config.hpp"


namespace fwMedData
{
    class SeriesDB;
}

namespace ioGdcm
{

class IOGDCM_CLASS_API SSeriesDBReader : public ::io::IReader
{

public :
    fwCoreServiceClassDefinitionsMacro ( (SSeriesDBReader)( ::io::IReader) ) ;

    /**
     * @brief   constructor
     *
     */
    IOGDCM_API SSeriesDBReader() throw();

    /**
     * @brief   destructor
     */
    IOGDCM_API virtual ~SSeriesDBReader() throw();

protected:

    typedef ::fwRuntime::ConfigurationElement::sptr ConfigurationType;

    /// Override
    IOGDCM_API virtual void starting() throw(::fwTools::Failed);

    /// Override
    IOGDCM_API virtual void stopping() throw(::fwTools::Failed);

    /// Override
    IOGDCM_API void updating() throw(::fwTools::Failed);

    /// Override
    virtual void receiving( CSPTR(::fwServices::ObjectMsg) _msg ) throw(::fwTools::Failed){};

    /// Override
    IOGDCM_API void info(std::ostream &_sstream ) ;

    /// Override
    IOGDCM_API virtual std::vector< std::string > getSupportedExtensions() ;

    /// Override
    IOGDCM_API virtual std::string getSelectorDialogTitle();

    /// Override
    IOGDCM_API virtual void configureWithIHM();

    /// Return managed file type, here FOLDER
    IOGDCM_API ::io::IOPathType getIOPathType() const;

private :

    SPTR( ::fwMedData::SeriesDB ) createSeriesDB( const ::boost::filesystem::path & dicomFile );

};

} // namespace ioGdcm

#endif //_IOGDCM_SSERIESDBREADER_HPP_
