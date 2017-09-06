/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IOVTK_SIMAGEWRITER_HPP__
#define __IOVTK_SIMAGEWRITER_HPP__

#include "ioVTK/config.hpp"

#include <io/IWriter.hpp>

#include <boost/filesystem/path.hpp>

#include <string>

namespace fwData
{
class Image;
}

namespace fwJobs
{
class IJob;
}

namespace ioVTK
{

/**
 * @brief   VTK Image Writer
 *
 * Service writing a VTK Image using the fwVtkIO lib.
 */
class IOVTK_CLASS_API SImageWriter : public ::io::IWriter
{

public:

    /**
     * @brief Constructor. Do nothing.
     */
    IOVTK_API SImageWriter() noexcept;

    ~SImageWriter() noexcept
    {
    }

    fwCoreServiceClassDefinitionsMacro( (SImageWriter)( ::io::IWriter) );

    typedef ::fwCom::Signal< void ( SPTR(::fwJobs::IJob) ) > JobCreatedSignalType;

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
     * @param[in] image std::shared_ptr< ::fwData::Image >.
     * @param[in] sigJobCreated signal emitted when the image is saved.
     * @return bool.
     *
     * This method is used to save an image using the file path.
     * Returns \b true if the image saving is a success and \b false if it fails
     */
    IOVTK_API static bool saveImage( const ::boost::filesystem::path& imgFile,
                                     const SPTR(::fwData::Image)& image,
                                     const SPTR(JobCreatedSignalType)& sigJobCreated);

protected:

    IOVTK_API virtual ::io::IOPathType getIOPathType() const;

    /**
     * @brief Starting method.
     *
     * This method is used to initialize the service.
     */
    IOVTK_API virtual void starting();

    /**
     * @brief Stopping method.
     *
     * The stopping method is empty for this service.
     */
    IOVTK_API virtual void stopping();

    /**
     * @brief Configuring method.
     *
     * The configuring method only calls the configuring method from the base class
     */
    IOVTK_API virtual void configuring();

    /**
     * @brief Updating method.
     *
     * This method is used to update the service.
     * The image is read.
     */
    IOVTK_API void updating();

    /**
     * @brief Info method.
     *
     * This method is used to give
     * informations about the service.
     */
    IOVTK_API void info(std::ostream& _sstream );

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

    SPTR(JobCreatedSignalType) m_sigJobCreated;
};

} // namespace ioVTK

#endif //__IOVTK_SIMAGEWRITER_HPP__
