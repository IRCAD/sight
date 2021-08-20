/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "modules/io/vtk/config.hpp" // Declaration of class and function export

#include <io/base/service/IReader.hpp> // Definition of abstract reader class

#include <filesystem> // Used to save the file system path of loaded image

// Pre-definition of data::Image to avoid inclusion file
namespace sight::data
{

class Image;

}

namespace sight::core::jobs
{

class IJob;

}

namespace sight::module::io::vtk
{

/**
 * @brief Service reading a VTK Image using the fwVtkIO lib.
 *
 * @section Signals Signals
 * - \b jobCreated(SPTR(core::jobs::IJob)): emitted to display a progress bar while the image is loading (it should be
 * connected to a SJobBar).
 *
 * @section Slots Slots
 * - \b readFile(std::filesystem::path) : read the given file
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="sight::module::io::vtk::SImageReader">
       <inout key="data" uid="..." />
       <file>...</file>
   </service>
   @endcode
 * @subsection In-Out In-Out
 * - \b data [sight::data::Image]: loaded image.
 * @subsection Configuration Configuration
 * - \b file (optional): path of the image to load, if it is not defined, 'openLocationDialog()' should be called to
 * define the path.
 */
class MODULE_IO_VTK_CLASS_API SImageReader : public sight::io::base::service::IReader
{
public:

    ~SImageReader() noexcept
    {
    }

    SIGHT_DECLARE_SERVICE(SImageReader, sight::io::base::service::IReader);

    typedef core::com::Signal<void (SPTR(core::jobs::IJob))> JobCreatedSignalType;

    /**
     * @brief Configure the image path with a dialogBox.
     *
     * This method is used to find the file path using a file selector.
     */
    MODULE_IO_VTK_API void openLocationDialog() override;

    /**
     * @brief Constructor. Do nothing.
     */
    MODULE_IO_VTK_API SImageReader() noexcept;

    /**
     * @brief This method is used to load an vtk image using a file path.
     * @param[in] vtkFile file system path of vtk image
     * @param[out] image new empty image that will contain image loaded, if reading process is a success.
     * @return bool  \b true if the image loading is a success and \b false if it fails
     */
    MODULE_IO_VTK_API static bool loadImage(
        const std::filesystem::path& vtkFile,
        std::shared_ptr<data::Image> img,
        const SPTR(JobCreatedSignalType)& sigJobCreated
    );

protected:

    MODULE_IO_VTK_API sight::io::base::service::IOPathType getIOPathType() const override;

    /// Method called when the service is started, does nothing.
    MODULE_IO_VTK_API void starting() override;

    /// Method called when the service is stopped, does nothing.
    MODULE_IO_VTK_API void stopping() override;

    /// Method called when the service is stopped, does nothing.
    MODULE_IO_VTK_API void configuring() override;

    /**
     * @brief Updating method execute the read process.
     *
     * This method is used to update the service.
     * The image is read.
     */
    MODULE_IO_VTK_API void updating() override;

    /// Info method gives some informations on service.
    MODULE_IO_VTK_API void info(std::ostream& _sstream) override;

private:

    /// Image path, location of image on filesystem.
    std::filesystem::path m_fsImgPath;

    SPTR(JobCreatedSignalType) m_sigJobCreated;
};

} // namespace sight::module::io::vtk
