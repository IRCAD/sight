/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _IODATA_DEFAULTCOMPOSITEOBJECTREADER_HPP_
#define _IODATA_DEFAULTCOMPOSITEOBJECTREADER_HPP_

#include <boost/filesystem/path.hpp>

#include <io/IReader.hpp>

#include "ioData/export.hpp"


namespace ioData
{
/**
 * @brief   ProcessObject reader service.
 * @class   DefaultCompositeObjectReaderService
 * @author  IRCAD (Research and Development Team).
 * @date    2009.
 *
 * Service reading a ProcessObject. Read the process object and read all its inputs.
 * @li Use setConfiguration(cfg) and configure() methods to configure the process object elements.
 * @li Use start() to initialize the service.
 * @li Use update() to load the process object and load all its elements.
 * @li Use stop() to stop service before to destroy it.
 *
 * Service registered details : \n
 * REGISTER_SERVICE( ::io::IReader , ::ioData::DefaultCompositeObjectReaderService , ::fwData::ProcessObject )
 */
class IODATA_CLASS_API DefaultCompositeObjectReaderService : public ::io::IReader
{

public:

    fwCoreServiceClassDefinitionsMacro ( (DefaultCompositeObjectReaderService)(::io::IReader) ) ;

    /// Super class of reader services
    typedef ::io::IReader   SuperClass;
    /**
     * @brief   Constructor : does nothing
     */
    IODATA_API DefaultCompositeObjectReaderService() ;

    /**
     * @brief   Destructor
     */
    IODATA_API ~DefaultCompositeObjectReaderService() throw() ;

    /**
     * @brief Configure the path : default does nothing.
     *
     * This method is used to find
     * the file path  using a file selector.
     */
    IODATA_API virtual void configureWithIHM(){};


protected:

    /** @name Service methods ( override from ::fwServices::IService )
     * @{
     */

    /**
     * @brief Configure service. This method is called by configure() from base service ( ::fwServices::IService )
     *
     * XML configuration sample:
     * @verbatim
         <service type="::io::IReader" implementation="::ioData::DefaultCompositeObjectReaderService">
            <input id="OBJ0A">
                <service implementation="::ioVTK::MeshReaderService" type="::io::IReader">
                    <filename id=".../meshTest.vtk"/>
            </service>
            </input>
            <input id="OBJ0B">
                <service implementation="::ioVTK::MeshReaderService" type="::io::IReader">
                    <filename id=".../meshTest2.vtk"/>
                </service>
            </input>
         </service>
         @endverbatim
     */
    IODATA_API virtual void configuring( ) throw(::fwTools::Failed);

    /**
     * @brief Starting method. This method is called by start() from base service ( ::fwServices::IService )
     *
     * This method is used to initialize the service.
     * For each inputs, one tries to start the io service. For configured input io service, the configuration is applied
     */
    IODATA_API virtual void starting( ) throw(::fwTools::Failed);

    /**
     * @brief Stopping method. This method is called by stop() from base service ( ::fwServices::IService )
     *
     * The stopping method is empty for this service.
     */
    IODATA_API virtual void stopping( ) throw(::fwTools::Failed);

    /**
     * @brief Updating method. This method is called by update() from base service ( ::fwServices::IService )
     *
     * This method is used to update the service.
     * Call reader method update() on all the object contained in composite if they have a reader service.
     */
    IODATA_API void updating() throw(::fwTools::Failed);

    /**
     * @brief React on modifications : default does nothing.
     *
     * @note This method is automaticaly called by update( msg ) method from base service ( ::fwServices::IService ).
     *
     * @param[in] _msg information message for modification
     */
    IODATA_API void updating( ::boost::shared_ptr< const ::fwServices::ObjectMsg > _msg ) throw(::fwTools::Failed){};

    /**
     * @brief Info method.
     *
     * This method is used to give
     * informations about the service.
     *
     * @param[out] _sstream output stream
     */
    IODATA_API virtual void info(std::ostream &_sstream ) ;
    /// @}

    /** @name Specified reader service methods ( override from ::io::IReader )
     * @{
     */

    /**
     * @brief   returns  (filename) extension
     */
    IODATA_API virtual std::vector< std::string > getSupportedExtensions() ;
    /// @}


};

} // namespace ioData

#endif // _IODATA_DEFAULTCOMPOSITEOBJECTREADER_HPP_
