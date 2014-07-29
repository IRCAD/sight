/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _IOGDCM_SSERIESDBREADER_HPP_
#define _IOGDCM_SSERIESDBREADER_HPP_

#include <vector>
#include <string>
#include <boost/filesystem/path.hpp>

#include <io/IReader.hpp>

#include "ioGdcm/config.hpp"


namespace fwMedData
{
    class SeriesDB;
}

namespace ioGdcm
{

class IOGDCM_CLASS_API SSeriesDBReader : public ::io::IReader
{

public :
    fwCoreServiceClassDefinitionsMacro ( (SSeriesDBReader)( ::io::IReader) ) ;

    /**
     * @brief   constructor
     *
     */
    IOGDCM_API SSeriesDBReader() throw();

    /**
     * @brief   destructor
     */
    IOGDCM_API virtual ~SSeriesDBReader() throw();

protected:

    typedef ::fwRuntime::ConfigurationElement::sptr ConfigurationType;

    /**
     * The reader can be configured as a regular reader.
     * It is also possible to define a filter that must be applied prior to the reading process.
     * @verbatim
       <config filterType="::fwDicomIOFilter::custom::NoFilter" />
       @endverbatim
     * On the other hand, you can specify a service configuration using the FilterSelectorSrvConfig element:
     * @verbatim
        <extension implements="::fwServices::registry::ServiceConfig">
            <id>FilterSelectorConfig</id>
            <service>::ioDicomExt::dcmtk::editor::SFilterSelectorDialog</service>
            <desc>"Open" action's filter selector config</desc>
            <config>
                <selection mode="include" />
                <addSelection filter="::fwDicomIOFilter::custom::DefaultDicomFilter" />
                <addSelection filter="::fwDicomIOFilter::custom::NoFilter" />
            </config>
        </extension>
    @endverbatim
     */
    IOGDCM_API virtual void configuring() throw (fwTools::Failed);

    /// Override
    IOGDCM_API virtual void starting() throw(::fwTools::Failed);

    /// Override
    IOGDCM_API virtual void stopping() throw(::fwTools::Failed);

    /// Override
    IOGDCM_API void updating() throw(::fwTools::Failed);

    /// Override
    virtual void receiving( CSPTR(::fwServices::ObjectMsg) _msg ) throw(::fwTools::Failed){};

    /// Override
    IOGDCM_API void info(std::ostream &_sstream ) ;

    /// Override
    IOGDCM_API virtual std::string getSelectorDialogTitle();

    /// Override
    IOGDCM_API virtual void configureWithIHM();

    /// Return managed file type, here FOLDER
    IOGDCM_API ::io::IOPathType getIOPathType() const;

private :

    /**
     * @brief Create a seriesDB and fill it using the data of the DICOM files
     * @param[in] dicomDir DICOM folder
     */
    SPTR( ::fwMedData::SeriesDB ) createSeriesDB( const ::boost::filesystem::path & dicomFile );

    /// Selector config used to select a filter to apply
    std::string m_filterSelectorSrvConfig;

    /// Selected filter key
    std::string m_filterType;

};

} // namespace ioGdcm

#endif //_IOGDCM_SSERIESDBREADER_HPP_
