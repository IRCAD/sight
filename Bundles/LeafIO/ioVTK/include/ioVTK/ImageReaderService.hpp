/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _IOVTK_IMAGEREADERSERVICE_HPP_
#define _IOVTK_IMAGEREADERSERVICE_HPP_

#include <boost/filesystem/path.hpp> // Used to save the file system path of loaded image

#include <io/IReader.hpp> // Definition of abstract reader class

#include "ioVTK/export.hpp"  // Declaration of class and function export

// Pre-definition of ::fwData::Image to avoid inclusion file
namespace fwData
{
        class Image;
}


namespace ioVTK
{

/**
 * @brief       Image reader service.
 * @class       ImageReaderService.
 * @author      IRCAD (Research and Development Team).
 * @date        2009.
 *
 * Service reading a VTK Image using the vtkIO lib.
 *
 * Service registered details : \n
 * REGISTER_SERVICE( ::io::IReader , ::ioVTK::ImageReaderService , ::fwData::Image )
 */
class IOVTK_CLASS_API ImageReaderService : public ::io::IReader
{

public :

        /**
         * @brief Service constructor, default parameter initialization
         * @post ImageReaderService::m_bServiceIsConfigured is equal to false : service not configured
         * @post ImageReaderService::m_fsImgPath is empty : path to read not defined
         */
        IOVTK_API ImageReaderService() throw();

        /// Destructor, do nothing.
        IOVTK_API virtual ~ImageReaderService() throw();


        /**
         * @brief Configure the image path with an wxWidget dialogBox.
         *
         * This method is used to find the file path using a file selector.
         */
        IOVTK_API virtual void configureWithIHM();


protected:

        /**
         * @brief Configure the reader service.
         *
         * This method is used to configure the service. It search in a configure element
         * the definition of image path. XML configuration sample:
         * @verbatim
         <service implementation="::ioVTK::ImageReaderService" type="::io::IReader">
                <filename id=".../imageTest.vtk"/>
         </service>
         @endverbatim
         */
        IOVTK_API virtual void configuring() throw ( ::fwTools::Failed );

        /// Method called when the service is started, does nothing.
        IOVTK_API virtual void starting() throw ( ::fwTools::Failed );

        /// Method called when the service is stopped, does nothing.
        IOVTK_API virtual void stopping() throw ( ::fwTools::Failed );

        /**
         * @brief Updating method execute the read process.
         *
         * This method is used to update the service.
         * The image is read.
         */
        IOVTK_API void updating() throw(::fwTools::Failed);

        /**
         * @brief React on modifications : here, this method does nothing.
         * @note This method is automaticaly called by update( msg ) method from base service ( ::fwServices::IService ).
         * @param[in] _msg information message for modification
         */
        IOVTK_API void updating( ::boost::shared_ptr< const ::fwServices::ObjectMsg > _msg ) throw ( ::fwTools::Failed );

        /// Info method gives some informations on service.
        IOVTK_API void info(std::ostream &_sstream ) ;

private :

        /**
        * @brief This method is used to load an vtk image using a file path.
        * @param[in] _vtkFile file system path of vtk image
        * @param[out] _pImage new empty image that will contain image loaded, if reading process is a success.
        * @return bool  \b true if the image loading is a success and \b false if it fails
        */
        bool loadImage( const ::boost::filesystem::path _vtkFile, ::boost::shared_ptr< ::fwData::Image > _pImage );

        /// This method notifies other image services that a new image has been loaded.
        void notificationOfDBUpdate();

        /// This value is \b true if the path image is known.
        bool m_bServiceIsConfigured;

        /// Image path, location of image on filesystem.
        ::boost::filesystem::path m_fsImgPath;

};

} // namespace ioVTK

#endif //_IOVTK_IMAGEREADERSERVICE_HPP_
