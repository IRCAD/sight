/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IODICOM_SSERIESDBMERGER_HPP__
#define __IODICOM_SSERIESDBMERGER_HPP__

#include "ioDicom/config.hpp"

#include <fwGui/IActionSrv.hpp>
#include <fwMedData/SeriesDB.hpp>

#include <boost/filesystem/path.hpp>

namespace ioDicom
{

/**
 * @brief   This service is used to merge selected series with a SeriesDB.
 *          All the series in the vector are pushed in the destination SeriesDB.
 * @class   SSeriesDBMerger
 * @date    2013.
 */
class IODICOM_CLASS_API SSeriesDBMerger : public ::fwGui::IActionSrv
{
public:

    fwCoreServiceClassDefinitionsMacro ( (SSeriesDBMerger)( ::fwGui::IActionSrv ) );

    /**
     * @brief Constructor
     */
    IODICOM_API SSeriesDBMerger() throw();

    /**
     * @brief Destructor
     */
    IODICOM_API virtual ~SSeriesDBMerger() throw();

protected:

    /**
     * @brief Configuring method. This method is used to configure the service.
     *
     * XML configuration sample:
       @code{.xml}
       <service uid="actionPushSeries" impl="::ioDicom::common::action::SSeriesDBMerger">
         <config destinationSeriesDBID="mySeriesDB" />
       </service>
       @endcode
     */
    IODICOM_API virtual void configuring() throw(::fwTools::Failed);

    /// Override
    IODICOM_API virtual void starting() throw(::fwTools::Failed);

    /// Override
    IODICOM_API virtual void stopping() throw(::fwTools::Failed);

    /// Override
    IODICOM_API void updating() throw(::fwTools::Failed);

    /// Override
    IODICOM_API void info(std::ostream &_sstream );

protected:

    /// Destination SeriesDB ID
    std::string m_destinationSeriesDBID;

    /// Destination SeriesDB
    ::fwMedData::SeriesDB::sptr m_destinationSeriesDB;

};

} // namespace ioDicom

#endif // __IODICOM_SSERIESDBMERGER_HPP__
