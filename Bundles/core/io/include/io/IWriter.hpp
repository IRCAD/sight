/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _IO_IWRITER_HPP_
#define _IO_IWRITER_HPP_

#include <boost/filesystem/path.hpp>

#include <fwServices/IService.hpp>

#include "io/config.hpp"
#include "io/ioTypes.hpp"

namespace io
{

/**
 * @brief Writer service API. It manages extension points definition and extension configuration
 * @class   IWriter.
 * @author  IRCAD (Research and Development Team).
 * @date    2007-2009.
 *
 * This class represents the base interface for writer services.
 * Use the base service methods :
 * @li The service is configured with methods setConfiguration(cfg) and configure()
 * @li The method start() initialize the service
 * @li To write the object use update() method
 * @li Finally we must call stop() before deleting the service
 */
class IO_CLASS_API IWriter : public fwServices::IService
{

public :
    fwCoreServiceClassDefinitionsMacro ( (IWriter)(::fwServices::IService) ) ;

    /**
     * @brief Configure the image path (by default does nothing).
     *
     * This method is used to find
     * the file path  using a file selector.
     */
    IO_API virtual void configureWithIHM() = 0 ;

    /**
     * @brief This method must be implemented by concrete service writers
     * that use path file system to write data.
     *
     * This method returns supported path format (file, files or folder).
     * A reader can support file and folder, or files and folder, but not
     * file and files ( because files include file concept ).
     */
    IO_API virtual ::io::IOPathType getIOPathType() const;

    /**
     * @brief Returns the file path setted by user or setted during service configuration
     * @pre exception if a file path is not defined  ( m_locations.empty() )
     * @pre exception if service not supported FILE management
     */
    IO_API ::boost::filesystem::path getFile() const;

    /**
     * @brief Sets file path
     * @pre exception if service not supported FILE management
     */
    IO_API void setFile(const ::boost::filesystem::path &file);

    /**
     * @brief Returns file paths setted by user or setted during service configuration
     * @pre exception if a file path is not defined ( m_locations.empty() )
     * @pre exception if service not supported FILES management
     */
    IO_API ::io::LocationsType getFiles() const;

    /**
     * @brief Sets file paths
     * @pre exception if service not supported FILES management
     */
    IO_API void setFiles(const ::io::LocationsType &files);

    /**
     * @brief Returns folder path setted by user or setted during service configuration
     * @pre exception if a folder path is not defined ( m_locations.empty() )
     * @pre exception if service not supported FOLDER management
     */
    IO_API ::boost::filesystem::path getFolder() const;

    /**
     * @brief Clear any location set by the setFile/setFiles/setFolder setter
     */
    IO_API void clearLocations() const;

    /**
     * @brief Sets folder path
     * @pre exception if service not supported FOLDER management
     */
    IO_API void setFolder(const ::boost::filesystem::path &folder);

    /// Returns if a location has been defined ( by the configuration process or directly by user )
    IO_API bool hasLocationDefined() const;

protected:

    /// Constructor (by default does nothing)
    IO_API IWriter() throw() ;

    /// Destructor
    IO_API virtual ~IWriter() throw() ;

    /**
     * @brief This method proposes to parse xml configuration to retrieve
     * file/files/folder paths.
     *
     * You can implement your configuring method if you wan check another
     * information or information not correspond to a path on filesystem,
     * else you must use this generic approach.
     *
     * Sample configuration for a file:
     * @verbatim
     <service ... >
        <file>/home/user/myFile.jpg<file/>
     </service>
     @endverbatim
     * Sample configuration for many files:
     * @verbatim
     <service ... >
        <file>/home/user/myFile01.jpg<file/>
        <file>/home/user/myFile02.jpg<file/>
        <file>/home/user/myFile03.jpg<file/>
     </service>
     @endverbatim
     * Sample configuration for a folder:
     * @verbatim
     <service ... >
        <folder>/home/user/myFolder<folder/>
     </service>
     @endverbatim
     */
    IO_API virtual void configuring() throw (fwTools::Failed);

private:

    /// Value to stock file or folder paths
    ::io::LocationsType m_locations;
};

}


#endif // _IO_IWRITER_HPP_
