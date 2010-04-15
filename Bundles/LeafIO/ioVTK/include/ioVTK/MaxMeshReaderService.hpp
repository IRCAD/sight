/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _MAX_MESHREADER_SERVICE_HPP_
#define _MAX_MESHREADER_SERVICE_HPP_

#include <io/IReader.hpp>
#include <boost/filesystem/path.hpp>

#include "ioVTK/export.hpp"


namespace ioVTK
{

/**
 * @brief   3ds Max reader service.
 * @class   MaxMeshReaderService.
 * @author  IRCAD (Research and Development Team).
 * @date    2009.
 *
 * Service reading a 3ds Max file using the vtkIO lib
 * (for .3ds at this time).
 *
 * Service registered details : \n
 * REGISTER_SERVICE( ::io::IReader , ::ioVTK::MaxMeshReaderService , ::fwData::Model )
 */
class IOVTK_CLASS_API MaxMeshReaderService : public ::io::IReader
{
public:
    fwCoreServiceClassDefinitionsMacro ( (MaxMeshReaderService)( ::io::IReader) ) ;
    typedef ::io::IReader   SuperClass;
    /**
     * @brief   Constructor
     */
    IOVTK_API MaxMeshReaderService() throw() ;

    /**
     * @brief   Destructor
     */
    IOVTK_API ~MaxMeshReaderService() throw() ;

    /**
     * @brief Configure the mesh path.
     *
     * This method is used to find
     * the file path  using a file selector.
     */
    IOVTK_API virtual void configureWithIHM();

    /*!
    * @brief Get the supported extensions.
    *
    * @return std::vector< std::string >.
    *
    * Return a std::vector< std::string > that contains the supported extensions.
    */
    IOVTK_API virtual std::vector< std::string > getSupportedExtensions();


protected:

    /**
    * @brief Starting method : default does nothing.
    *
    * This method is used to initialize the service.
    */
    IOVTK_API virtual void starting() throw(::fwTools::Failed){};

    /**
     * @brief Stopping method : default does nothing.
     *
     * The stopping method is empty for this service.
     */
    IOVTK_API virtual void stopping() throw(::fwTools::Failed){};

    /**
    * @brief Configuring method.
    *
    * XML configuration sample:
    * @verbatim
    <service implementation="::ioVTK::ImageReaderService" type="::io::IReader">
        <filename id=".../imageTest.vtk"/>
    </service>
    @endverbatim
    * This method is used to configure the service.
    */
    IOVTK_API virtual void configuring( ) throw(::fwTools::Failed);
    /**
    * @brief Info method.
    *
    * This method is used to give
    * informations about the service.
    */
    IOVTK_API void updating() throw(::fwTools::Failed);

    /**
     * @brief React on modifications : default does nothing.
     *
     * @note This method is automaticaly called by update( msg ) method from base service ( ::fwServices::IService ).
     *
     * @param[in] _msg information message for modification
     */
    IOVTK_API void updating( ::boost::shared_ptr< const ::fwServices::ObjectMsg > _msg ) throw(::fwTools::Failed){};

    /**
    * @brief Info method.
    *
    * This method is used to give
    * informations about the service.
    */
    IOVTK_API virtual void info(std::ostream &_sstream );


private:

    /**
    * @brief the m_bServiceIsConfigured value is \b true
    * if the path image is known.
    */
    bool m_bServiceIsConfigured;

    ::boost::filesystem::path m_fsMeshPath;
};

}

#endif /*_MAX_MESHREADER_SERVICE_HPP_*/
