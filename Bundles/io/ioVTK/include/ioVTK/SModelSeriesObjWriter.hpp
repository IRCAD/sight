/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#ifndef ANDROID

#include "ioVTK/config.hpp"

#include <fwData/Mesh.hpp>

#include <fwIO/IWriter.hpp>

#include <boost/filesystem/path.hpp>

#include <string>

namespace fwData
{
class Mesh;
}

namespace fwJobs
{
class IJob;
}

namespace ioVTK
{

/**
 * @brief   Wavefront OBJ files.
 *
 * Service writing a model series as .obj files using the fwVtkIO lib.
 *
 * @section Signals Signals
 * - \b jobCreated(SPTR(::fwJobs::IJob)): emitted when the image is writing to display a progress bar.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::ioVTK::SModelSeriesObjWriter">
       <inout key="data" uid="..." />
       <folder>...</folder>
   </service>
   @endcode
 * @subsection In-Out In-Out
 * - \b data [::fwMedData::ModelSeries]: model to save.
 * @subsection Configuration Configuration
 * - \b folder (optional): path of the folder, if it is not defined, 'configureWithIHM()' should be called to define
 * the path.
 */
class IOVTK_CLASS_API SModelSeriesObjWriter : public ::fwIO::IWriter
{

public:

    typedef ::fwCom::Signal< void ( SPTR(::fwJobs::IJob) ) > JobCreatedSignalType;

    /**
     * @brief Constructor. Do nothing.
     */
    IOVTK_API SModelSeriesObjWriter() noexcept;

    ~SModelSeriesObjWriter() noexcept
    {
    }

    fwCoreServiceClassDefinitionsMacro( (SModelSeriesObjWriter)( ::fwIO::IWriter) );

    /**
     * @brief Configure the mesh path.
     *
     * This method is used to find the file path  using a file selector.
     */
    IOVTK_API virtual void configureWithIHM() override;

protected:

    IOVTK_API virtual ::fwIO::IOPathType getIOPathType() const override;

    /**
     * @brief Starting method.
     *
     * This method is used to initialize the service.
     */
    IOVTK_API virtual void starting() override;

    /**
     * @brief Stopping method.
     *
     * The stopping method is empty for this service.
     */
    IOVTK_API virtual void stopping() override;

    /**
     * @brief Configuring method.
     *
     * The configuring method only calls the configuring method from the base class
     */
    IOVTK_API virtual void configuring() override;

    /**
     * @brief Updating method.
     *
     * This method is used to update the service.
     * The mesh is read.
     */
    IOVTK_API void updating() override;

    /**
     * @brief Info method.
     *
     * This method is used to give
     * informations about the service.
     */
    IOVTK_API void info(std::ostream& _sstream ) override;

private:

    /**
     * @brief the m_bServiceIsConfigured value is \b true
     * if the image path is known.
     */
    bool m_bServiceIsConfigured;

    /**
     * @brief Mesh path.
     */
    ::boost::filesystem::path m_fsMeshPath;

    SPTR(JobCreatedSignalType) m_sigJobCreated;

};

} // namespace ioVTK

#endif //ANDROID
