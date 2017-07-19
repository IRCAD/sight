/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IODATA_SATTACHMENTSERIESREADER_HPP__
#define __IODATA_SATTACHMENTSERIESREADER_HPP__

#include "ioData/config.hpp"

#include <io/IReader.hpp>

#include <boost/filesystem/path.hpp>

namespace ioData
{

/**
 * @brief   SAttachmentSeries reader service.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
    <service uid="..." type="::ioData::SAttachmentSeriesReader" autoConnect="no">
        <inout key="data" uid="..."/>
    </service>
   @endcode
 *
 * @subsection In-Out In-Out
 * - \b data [::fwMedData::SeriesDB]: SeriesDB updated with AttachmentSeries.
 */
class IODATA_CLASS_API SAttachmentSeriesReader : public ::io::IReader
{

public:

    fwCoreServiceClassDefinitionsMacro( (SAttachmentSeriesReader)(::io::IReader) );

    /// Super class of reader services
    typedef ::io::IReader SuperClass;

    /// Constructor : does nothing
    IODATA_API SAttachmentSeriesReader();

    /** @name Specified reader service methods ( override from ::io::IWriter )
     * @{
     */

    /// Returns  (filename) extension
    IODATA_API virtual std::vector< std::string > getSupportedExtensions();

    /**
     * @brief Configure the attachment path.
     *
     * This method is used to find the file path  using a file selector.
     */
    IODATA_API void configureWithIHM();
    /// @}

    /// Returns path type managed by the service, here FILE
    IODATA_API virtual ::io::IOPathType getIOPathType() const;

protected:

    /** @name Service methods ( override from ::fwServices::IService )
     * @{
     */

    /**
     * @brief Starting method : default does nothing.
     *
     * This method is used to initialize the service.
     */
    virtual void starting() throw(::fwTools::Failed)
    {
    }

    /**
     * @brief Stopping method : default does nothing.
     *
     * The stopping method is empty for this service.
     */
    virtual void stopping() throw(::fwTools::Failed)
    {
    }

    /**
     * @brief Configuring method : calls implementation from `io::IReader`
     */
    IODATA_API virtual void configuring() throw(::fwTools::Failed);

    /**
     * @brief Updating method. This method is called by update() from base service ( ::fwServices::IService )
     *
     * This method is used to update the service.
     */
    IODATA_API void updating() throw(::fwTools::Failed);

    /**
     * @brief Info method.
     *
     * This method is used to give
     * informations about the service.
     *
     * @param[out] _sstream output stream
     */
    IODATA_API virtual void info(std::ostream& _sstream );
    /// @}

};

} //namespace ioData

#endif /*__IODATA_SATTACHMENTSERIESREADER_HPP__*/
