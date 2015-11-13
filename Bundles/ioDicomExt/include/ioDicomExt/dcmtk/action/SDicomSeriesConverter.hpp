/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IODICOMEXT_DCMTK_ACTION_SDICOMSERIESCONVERTER_HPP__
#define __IODICOMEXT_DCMTK_ACTION_SDICOMSERIESCONVERTER_HPP__

#include <fwGui/IActionSrv.hpp>
#include <fwMedData/SeriesDB.hpp>

#include "ioDicomExt/config.hpp"

namespace ioDicomExt
{

namespace dcmtk
{

namespace action
{

/**
 * @brief   This service is used to read and push DicomSeries to a SeriesDB
 * @class   SDicomSeriesConverter
 * @date    2014.
 */
class IODICOMEXT_CLASS_API SDicomSeriesConverter : public ::fwGui::IActionSrv
{
public:

    fwCoreServiceClassDefinitionsMacro ( (SDicomSeriesConverter)( ::fwGui::IActionSrv ) );

    /**
     * @brief Constructor
     */
    IODICOMEXT_API SDicomSeriesConverter() throw();

    /**
     * @brief Destructor
     */
    IODICOMEXT_API virtual ~SDicomSeriesConverter() throw();

protected:

    /**
     * @brief Configuring method. This method is used to configure the service.
     *
     * XML configuration sample:
       @verbatim
       <service uid="actionConvertSeries" impl="::ioDicomExt::dcmtk::action::SDicomSeriesConverter">
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
    IODICOMEXT_API void info(std::ostream &_sstream );

protected:

    /// Destination SeriesDB ID
    std::string m_destinationSeriesDBID;

    /// Destination SeriesDB
    ::fwMedData::SeriesDB::sptr m_destinationSeriesDB;

};

} // namespace action
} // namespace dcmtk
} // namespace ioDicomExt

#endif // __IODICOMEXT_DCMTK_ACTION_SDICOMSERIESCONVERTER_HPP__
