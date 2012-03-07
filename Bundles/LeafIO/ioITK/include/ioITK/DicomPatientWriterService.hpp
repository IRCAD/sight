/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _IOITK_DICOMPATIENTWRITERSERVICE_HPP_
#define _IOITK_DICOMPATIENTWRITERSERVICE_HPP_

#include <string>
#include <boost/filesystem/path.hpp>

#include <fwData/Patient.hpp>

#include <io/IWriter.hpp>

#include "ioITK/export.hpp"

namespace ioITK
{

class IOITK_CLASS_API DicomPatientWriterService : public ::io::IWriter
{

public :
    fwCoreServiceClassDefinitionsMacro ( (DicomPatientWriterService)( ::io::IWriter) ) ;

protected:

    /**
     * @brief   constructor
     *
     */
    IOITK_API DicomPatientWriterService() throw();

    /**
     * @brief   destructor
     */
    IOITK_API virtual ~DicomPatientWriterService() throw();

    /// Override
    IOITK_API virtual void starting() throw(::fwTools::Failed);

    /// Override
    IOITK_API virtual void stopping() throw(::fwTools::Failed);

    /// Override
    IOITK_API void updating() throw(::fwTools::Failed);

    /// Override
    IOITK_API virtual void updating( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed) {} ;

    /// Override
    IOITK_API void info(std::ostream &_sstream ) ;

    /// Override
    IOITK_API virtual std::vector< std::string > getSupportedExtensions() ;

    /// Override
    IOITK_API virtual std::string getSelectorDialogTitle();

    /// Override
    IOITK_API virtual void configureWithIHM();

    /// Return managed file type, here FOLDER
    IOITK_API ::io::IOPathType getIOPathType() const;

private :

    void savePatient(   const ::boost::filesystem::path patientDBPath, ::fwData::Patient::sptr _pPatient );

};

} // namespace ioITK

#endif //_IOITK_DICOMPATIENTWRITERSERVICE_HPP_

