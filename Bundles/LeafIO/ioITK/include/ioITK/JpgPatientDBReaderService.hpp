/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _IOITK_JPGPATIENTDBREADERSERVICE_HPP_
#define _IOITK_JPGPATIENTDBREADERSERVICE_HPP_

#include <string>
#include <boost/filesystem/path.hpp>

#include <io/IReader.hpp>

#include <fwData/PatientDB.hpp>
#include <fwData/Patient.hpp>

#include "ioITK/export.hpp"


namespace ioITK
{

class IOITK_CLASS_API JpgPatientDBReaderService : public ::io::IReader
{

public :
    fwCoreServiceClassDefinitionsMacro ( (JpgPatientDBReaderService)( ::io::IReader) ) ;

protected:

    /**
     * @brief   constructor
     *
     */
    IOITK_API JpgPatientDBReaderService() throw();

    /**
     * @brief   destructor
     */
    IOITK_API virtual ~JpgPatientDBReaderService() throw();

    /// Override
    IOITK_API virtual void configuring() throw(::fwTools::Failed) ;

    /// Override
    IOITK_API virtual void starting() throw(::fwTools::Failed);

    /// Override
    IOITK_API virtual void stopping() throw(::fwTools::Failed);

    /// Override
    IOITK_API void updating() throw(::fwTools::Failed);

    /// Override
    IOITK_API virtual void updating(::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed) {} ;

    /// Override
    IOITK_API void info(std::ostream &_sstream ) ;

    /// Override
    IOITK_API virtual std::vector< std::string > getSupportedExtensions() ;

    /// Override
    IOITK_API virtual std::string getSelectorDialogTitle();

    /// Override
    IOITK_API virtual void configureWithIHM();

private :

    void notificationOfDBUpdate();

    ::fwData::PatientDB::sptr createPatientDB( const ::boost::filesystem::path inrFileDir );

    bool m_bServiceIsConfigured;

    ::boost::filesystem::path m_fsPatientDBPath;
};

} // namespace ioITK

#endif //_IOITK_INRPATIENTDBREADERSERVICE_HPP_
