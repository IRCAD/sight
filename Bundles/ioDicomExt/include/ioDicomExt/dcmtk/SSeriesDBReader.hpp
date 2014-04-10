/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IODICOMEXT_DCMTK_SSERIESDBREADER_HPP__
#define __IODICOMEXT_DCMTK_SSERIESDBREADER_HPP__

#include <string>
#include <boost/filesystem/path.hpp>

#include <io/IReader.hpp>

#include "ioDicomExt/config.hpp"

namespace fwMedData
{
    class SeriesDB;
    class Patient;
}

namespace ioDicomExt
{

namespace dcmtk
{

class IODICOMEXT_CLASS_API SSeriesDBReader : public ::io::IReader
{

public :
    typedef std::string                  ExtensionType;
    typedef std::vector< ExtensionType > ExtensionsType;

    fwCoreServiceClassDefinitionsMacro ( (SSeriesDBReader)( ::io::IReader) ) ;
    /**
     * @brief   constructor
     *
     */
    IODICOMEXT_API SSeriesDBReader() throw();

    /**
     * @brief   destructor
     */
    IODICOMEXT_API virtual ~SSeriesDBReader() throw();

protected:

    /**
     * The reader can be configured as a regular reader.
     * It is also possible to define a filter that must be applied prior the reading process.
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
    IODICOMEXT_API virtual void configuring() throw (fwTools::Failed);

    /// Override
    IODICOMEXT_API virtual void starting() throw(::fwTools::Failed);

    /// Override
    IODICOMEXT_API virtual void stopping() throw(::fwTools::Failed);

    /// Override
    IODICOMEXT_API void updating() throw(::fwTools::Failed);

    /// Override
    virtual void receiving( CSPTR(::fwServices::ObjectMsg) _msg ) throw(::fwTools::Failed){};

    /// Override
    IODICOMEXT_API void info(std::ostream &_sstream ) ;

    /// Override
    IODICOMEXT_API virtual ExtensionsType getSupportedExtensions() ;

    /// Override
    IODICOMEXT_API virtual std::string getSelectorDialogTitle();

    /// Override
    IODICOMEXT_API virtual void configureWithIHM();

    /// Return path type managed by the service, here FOLDER
    IODICOMEXT_API ::io::IOPathType getIOPathType() const;

private :

    void notificationOfDBUpdate();

    SPTR(::fwMedData::SeriesDB) createSeriesDB(const ::boost::filesystem::path& dicomDir);

    /// Selector config used to select a filter to apply
    std::string m_filterSelectorSrvConfig;

    /// Selected filter key
    std::string m_filterType;

};

} // namespace dcmtk
} // namespace ioDicomExt

#endif //__IODICOMEXT_DCMTK_SSERIESDBREADER_HPP__
