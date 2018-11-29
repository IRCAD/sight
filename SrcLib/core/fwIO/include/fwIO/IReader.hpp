/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#pragma once

#include "fwIO/config.hpp"
#include "fwIO/ioTypes.hpp"

#include <fwServices/IService.hpp>

#include <boost/filesystem/path.hpp>

namespace fwIO
{

/**
 * @brief Reader service API. It manages extension points definition and extension configuration
 *
 * @section Slots Slots
 * - \b setFileFolder(const ::boost::filesystem::path&) : Sets the folder when a path is configured in FILE or
 * FILES mode
 *
 * This class represents the base interface for reader services.
 * Use the base service methods :
 * @li The service is configured with methods setConfiguration(cfg) and configure()
 * @li The method start() initialize the service
 * @li To read the object use update() method
 * @li Finally we must call stop() before deleting the service
 * @todo ACH : remove some methods : getSupportedExtensions ? getSelectorDialogTitle ?
 */
class FWIO_CLASS_API IReader : public ::fwServices::IService
{

public:
    fwCoreServiceClassDefinitionsMacro( (IReader)(::fwServices::IService) )

    /**
     * @name Slots API
     * @{
     */
    FWIO_API static const ::fwCom::Slots::SlotKeyType s_SET_FILE_FOLDER;
    /// @}

    /**
     * @name    Specific service methods for reading
     */
    //@{

    /**
     * @brief Configure the image path (by default does nothing).
     *
     * This method is used to find
     * the file path  using a file selector.
     */
    FWIO_API virtual void configureWithIHM() = 0;

    /**
     * @brief   returns  (filename) extension
     */
    FWIO_API virtual std::vector< std::string > getSupportedExtensions();

    /**
     * @brief   returns  the title of selector dialog box
     */
    FWIO_API virtual std::string getSelectorDialogTitle();

    /**
     * @brief This method must be implemented by concrete service readers
     * that use path file system to read data.
     *
     * This method returns supported path format (file, files or folder).
     * A reader can support file and folder, or files and folder, but not
     * file and files ( because files include file concept ).
     */
    FWIO_API virtual ::fwIO::IOPathType getIOPathType() const;

    /**
     * @brief Returns the file path set by the user or set during service configuration
     * @pre exception if a file path is not defined  ( m_locations.empty() )
     * @pre exception if service does not support FILE mode
     */
    FWIO_API const ::boost::filesystem::path& getFile() const;

    /**
     * @brief Sets file path
     * @pre exception if service does not support FILE mode
     */
    FWIO_API void setFile(const ::boost::filesystem::path& file);

    /**
     * @brief Returns file paths set by the user or set during service configuration
     * @pre exception if a file path is not defined ( m_locations.empty() )
     * @pre exception if service does not support FILES mode
     */
    FWIO_API const ::fwIO::LocationsType& getFiles() const;

    /**
     * @brief Sets file paths
     * @pre exception if service does not support FILES mode
     */
    FWIO_API void setFiles(const ::fwIO::LocationsType& files);

    /**
     * @brief Returns folder path set by the user or set during service configuration
     * @pre exception if a folder path is not defined ( m_locations.empty() )
     * @pre exception if service does not support FOLDER mode
     */
    FWIO_API const ::boost::filesystem::path& getFolder() const;

    /**
     * @brief Clear any location set by the setFile/setFiles/setFolder setter
     */
    FWIO_API void clearLocations();

    /**
     * @brief Returns file/files/folder paths set by the user or set during service configuration
     * @pre exception if a file path is not defined ( m_locations.empty() )
     */
    FWIO_API const ::fwIO::LocationsType& getLocations() const;

    /**
     * @brief Sets folder path
     * @pre exception if service does not support FOLDER mode
     */
    FWIO_API void setFolder(const ::boost::filesystem::path& folder);

    /**
     * @brief Slot: Sets the folder when a path is configured in FILE or FILES mode
     * This is ignored if a path is not configured
     *
     * @pre exception if service does not support FILE or FILES mode
     */
    FWIO_API void setFileFolder(::boost::filesystem::path folder);

    /// Returns if a location has been defined ( by the configuration process or directly by user )
    FWIO_API bool hasLocationDefined() const;

    //@}

protected:

    FWIO_API IReader() noexcept;

    FWIO_API virtual ~IReader() noexcept;

    /**
     * @brief This method proposes to parse xml configuration to retrieve
     * file/files/folder paths.
     *
     * You can implement your configuring method if you wan check another
     * information or information not correspond to a path on filesystem,
     * else you must use this generic approach.
     *
     * Sample configuration for a file:
     * @code{.xml}
     *  <service ... >
     *      <file>/home/user/myFile.jpg</file>
     *  </service>
     * @endcode
     * Sample configuration for many files:
     * @code{.xml}
     *  <service ... >
     *      <file>/home/user/myFile01.jpg</file>
     *      <file>/home/user/myFile02.jpg</file>
     *      <file>/home/user/myFile03.jpg</file>
     *  </service>
     * @endcode
     * Sample configuration for a folder:
     * @code{.xml}
     *  <service ... >
     *      <folder>/home/user/myFolder</folder>
     *  </service>
     * @endcode
     * Sample configuration for a resource (file stored in a bundle or a library):
     * @code{.xml}
     *  <service ... >
     *      <resource>bundle-0.1/myFile.jpg</resource>
     *  </service>
     * Sample configuration for many resources (files stored in a bundle or a library):
     * @code{.xml}
     *  <service ... >
     *      <resource>bundle-0.1/myFile01.jpg</resource>
     *      <resource>bundle-0.1/myFile02.jpg</resource>
     *      <resource>library-0.1/myFile03.jpg</resource>
     *  </service>
     * Sample configuration for a resource folder (bundle or library path):
     * @code{.xml}
     *  <service ... >
     *      <resource>bundle</resource>
     *  </service>
     *
     * You may specify the title of the modal file selection window with the `windowTitle` config attribute as such:
     * @code{.xml}
     *  <service ... >
     *      <windowTitle>Example window title</windowTitle>
     *  </service>
     * @endcode
     */
    FWIO_API virtual void configuring() override;

    /**
     * @brief Title of the window that will open when the `configureWithIHM` slot is called
     */
    std::string m_windowTitle;

private:

    /// Slot to read folder
    void readFolder(::boost::filesystem::path path);
    /// Slot to read file
    void readFile(::boost::filesystem::path path);
    /// Slot to read files
    void readFiles(::fwIO::LocationsType files);

    /// Value to stock file or folder paths
    ::fwIO::LocationsType m_locations;
};

} //namespace fwIO
