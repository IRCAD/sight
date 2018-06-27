/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "ioData/config.hpp"

#include <fwIO/IWriter.hpp>

#include <boost/filesystem/path.hpp>

namespace ioData
{
/**
 * @brief This service write a ::fwData::TransformationMatrix3D into a .trf file.
 *
 * This format is basic: an ascii file which contains matrix 4x4 values.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::ioData::TransformationMatrix3DWriterService">
       <in key="data" uid="..." />
   </service>
   @endcode
 * @subsection Input Input
 * - \b data [::fwData::TransformationMatrix3D]: matrix to save.
 */
class IODATA_CLASS_API TransformationMatrix3DWriterService : public ::fwIO::IWriter
{

public:

    fwCoreServiceClassDefinitionsMacro( (TransformationMatrix3DWriterService)(::fwIO::IWriter) );

    /// Super class of writer services
    typedef ::fwIO::IWriter SuperClass;

    /** @name Specified writer service methods ( override from ::fwIO::IWriter )
     * @{
     */

    /**
     * @brief Configure the matrix path.
     *
     * This method is used to find
     * the file path  using a file selector.
     */
    IODATA_API virtual void configureWithIHM() override;

    /// Return path type managed by the service, here FILE
    IODATA_API virtual ::fwIO::IOPathType getIOPathType() const override;

    ///@}

protected:

    /** @name Service methods ( override from ::fwServices::IService )
     * @{
     */

    /**
     * @brief Starting method. This method is called by start() from base service ( ::fwServices::IService )
     *
     * This method is used to initialize the service. The starting method is empty for this service.
     */
    IODATA_API virtual void starting() override;

    /**
     * @brief Stopping method. This method is called by stop() from base service ( ::fwServices::IService )
     *
     * The stopping method is empty for this service.
     */
    IODATA_API virtual void stopping() override;

    /**
     * @brief Configuring method : calls implementation from `io::IWriter`
     */
    IODATA_API virtual void configuring() override;

    /**
     * @brief Updating method. This method is called by update() from base service ( ::fwServices::IService )
     *
     * This method is used to update the service.
     * The transformation matrix is written with the writer ::fwDataIO::writer::TransformationMatrix3DWriter.
     * Notify writing.
     */
    IODATA_API void updating() override;

    /**
     * @brief Info method.
     *
     * This method is used to give
     * informations about the service.
     *
     * @param[out] _sstream output stream
     */
    IODATA_API virtual void info(std::ostream& _sstream ) override;
    /// @}

};

} // namespace ioData
