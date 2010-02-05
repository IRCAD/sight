/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _IODATA_CAMERAREADER_HPP_
#define _IODATA_CAMERAREADER_HPP_

#include <io/IReader.hpp>
#include <boost/filesystem/path.hpp>
#include <fwData/Camera.hpp>

#include "ioData/export.hpp"


namespace ioData
{
/**
 * @brief       Camera calibration file reader service.
 * @class       CameraReaderService.
 * @author      IRCAD (Research and Development Team).
 * @date        2009.
 *
 * Service reading camera calibration file and create a camera object.
 * @li This service has no specified start and stop method.
 * @li Use setConfiguration(cfg) and configure() methods to configure the filename.
 * @li Use update() to load the file, create the camera object and notify reading to observers.
 *
 * Service registered details : \n
 * REGISTER_SERVICE( ::io::IReader , ::ioData::CameraReaderService , ::fwData::Camera )
 */
class IODATA_CLASS_API CameraReaderService : public ::io::IReader
{
public:

        /// Super class of reader services
        typedef ::io::IReader   SuperClass;
        /**
         * @brief       Constructor : does nothing
         */
        IODATA_API CameraReaderService() ;

        /**
         * @brief       Destructor
         */
        IODATA_API ~CameraReaderService() throw() ;

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
        * @brief Starting method : default does nothing.
        *
        * This method is used to initialize the service.
        */
        IODATA_API virtual void starting() throw(::fwTools::Failed){};

        /**
         * @brief Stopping method : default does nothing.
         *
         * The stopping method is empty for this service.
         */
        IODATA_API virtual void stopping() throw(::fwTools::Failed){};

        /**
         * @brief Configuring method. This method is called by configure() from base service ( ::fwServices::IService )
         *
         * XML configuration sample:
         * @verbatim
        <service type="::ioData::CameraReaderService">
                <filename>../cam.cal</filename>
        </service>
         @endverbatim
         *
         * Configure camera calibration filename.
         */
        IODATA_API virtual void configuring( ) throw(::fwTools::Failed);

        /**
        * @brief Updating method. This method is called by update() from base service ( ::fwServices::IService )
        *
        * This method is used to update the service.
        * Call this method after configuring() to load calibration file.
        * Notify reading.
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
         * @brief       returns  (filename) extension
         */
        IODATA_API virtual std::vector< std::string > getSupportedExtensions() ;
        /// @}

private:
        /**
         * @brief Load calibration file
         *
         * This method is called by updating method to load the file and create camera object.
         *
         * @param[in] fileName calibration filename
         * @param[out] cam camera created with readed calibration file
         */
        bool loadCalibration( const std::string &fileName, ::boost::shared_ptr< ::fwData::Camera > cam );

        ::boost::filesystem::path m_fsCameraPath;
};

}

#endif /*_IODATA_CAMERAREADER_HPP_*/
