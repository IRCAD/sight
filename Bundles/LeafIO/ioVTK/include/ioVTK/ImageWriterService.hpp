/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IOVTK_IMAGEWRITERSERVICE_HPP__
#define __IOVTK_IMAGEWRITERSERVICE_HPP__

#include <string>
#include <boost/filesystem/path.hpp>

#include <io/IWriter.hpp>


#include "ioVTK/config.hpp"

namespace fwData
{
class Image;
}


namespace ioVTK
{

/**
 * @brief   VTK image writer service.
 * @class   ImageWriterService
 *
 * Service writing a VTK Image using the fwVtkIO lib.
 *
 * Service registered details : \n
 * fwServicesRegisterMacro( ::io::IWriter , ::ioVTK::ImageWriterService , ::fwData::Image )
 */
class IOVTK_CLASS_API ImageWriterService : public ::io::IWriter
{

public:
    ~ImageWriterService() throw()
    {
    }

    fwCoreServiceClassDefinitionsMacro ( (ImageWriterService)( ::io::IWriter) );

    /**
     * @brief Configure the image path.
     *
     * This method is used to find
     * the file path  using a file selector.
     */
    IOVTK_API virtual void configureWithIHM();

    /**
     * @brief Save a VTK image.
     * @param[in] imgFile ::boost::filesystem::path.
     * @param[out] image ::boost::shared_ptr< ::fwData::Image >.
     * @return bool.
     *
     * This method is used to save an image using the file path.
     * Returns \b true if the image saving is a success and \b false if it fails
     */
    IOVTK_API static bool saveImage( const ::boost::filesystem::path& imgFile, const SPTR(::fwData::Image)& image );


protected:

    IOVTK_API virtual ::io::IOPathType getIOPathType() const;

    /**
     * @brief Starting method.
     *
     * This method is used to initialize the service.
     */
    IOVTK_API virtual void starting() throw(::fwTools::Failed);

    /**
     * @brief Stopping method.
     *
     * The stopping method is empty for this service.
     */
    IOVTK_API virtual void stopping() throw(::fwTools::Failed);

    /**
     * @brief Updating method.
     *
     * This method is used to update the service.
     * The image is read.
     */
    IOVTK_API void updating() throw(::fwTools::Failed);

    /**
     * @brief React on modifications : default does nothing.
     *
     * @note This method is automatically called by update( msg ) method from base service ( ::fwServices::IService ).
     *
     * @param[in] _msg information message for modification
     */
    void receiving( CSPTR(::fwServices::ObjectMsg)_msg ) throw(::fwTools::Failed)
    {
    }

    /**
     * @brief Info method.
     *
     * This method is used to give
     * informations about the service.
     */
    IOVTK_API void info(std::ostream &_sstream );

private:
    /**
     * @brief the m_bServiceIsConfigured value is \b true
     * if the image path is known.
     */
    bool m_bServiceIsConfigured;

    /**
     * @brief Image path.
     */
    ::boost::filesystem::path m_fsImgPath;

};

} // namespace ioVTK

#endif //__IOVTK_IMAGEWRITERSERVICE_HPP__
