/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IODICOMEXT_COMMON_ACTION_SSERIESDBMERGER_HPP__
#define __IODICOMEXT_COMMON_ACTION_SSERIESDBMERGER_HPP__

#include <boost/filesystem/path.hpp>

#include <fwGui/IActionSrv.hpp>
#include <fwMedData/SeriesDB.hpp>

#include "ioDicomExt/config.hpp"

namespace ioDicomExt
{

namespace common
{

namespace action
{

/**
 * @brief   This service is used to merge selected series with a SeriesDB.
 *          All the series in the vector are pushed in the destination SeriesDB.
 * @class   SSeriesDBMerger
 * @date    2013.
 */
class IODICOMEXT_CLASS_API SSeriesDBMerger : public ::fwGui::IActionSrv
{
public:

    fwCoreServiceClassDefinitionsMacro ( (SSeriesDBMerger)( ::fwGui::IActionSrv ) );

    /**
     * @brief Constructor
     */
    IODICOMEXT_API SSeriesDBMerger() throw();

    /**
     * @brief Destructor
     */
    IODICOMEXT_API virtual ~SSeriesDBMerger() throw();

protected:

    /**
     * @brief Configuring method. This method is used to configure the service.
     *
     * XML configuration sample:
       @verbatim
       <service uid="actionPushSeries" impl="::ioDicomExt::common::action::SSeriesDBMerger">
         <config destinationSeriesDBID="mySeriesDB" />
       </service>
       @endverbatim
     */
    IODICOMEXT_API virtual void configuring() throw(::fwTools::Failed);

    /// Override
    IODICOMEXT_API virtual void starting() throw(::fwTools::Failed);

    /// Override
    IODICOMEXT_API virtual void stopping() throw(::fwTools::Failed);

    /// Override
    IODICOMEXT_API void updating() throw(::fwTools::Failed);

    /// Override
    IODICOMEXT_API virtual void receiving( CSPTR(::fwServices::ObjectMsg) _msg ) throw(::fwTools::Failed);

    /// Override
    IODICOMEXT_API void info(std::ostream &_sstream );

protected:

    /// Destination SeriesDB ID
    std::string m_destinationSeriesDBID;

    /// Destination SeriesDB
    ::fwMedData::SeriesDB::sptr m_destinationSeriesDB;

};

} // namespace action
} // namespace common
} // namespace ioDicomExt

#endif // __IODICOMEXT_COMMON_ACTION_SSERIESDBMERGER_HPP__
