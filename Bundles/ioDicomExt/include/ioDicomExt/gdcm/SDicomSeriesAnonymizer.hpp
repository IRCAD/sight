/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IODICOMEXT_GDCM_SDICOMSERIESANONYMIZER_HPP__
#define __IODICOMEXT_GDCM_SDICOMSERIESANONYMIZER_HPP__

#include <fwServices/IController.hpp>

#include "ioDicomExt/config.hpp"

namespace fwMedData
{
class SeriesDB;
}

namespace ioDicomExt
{

namespace gdcm
{

/**
 * @brief   Services to anonymize a DicomSeries
 * @class   SDicomSeriesAnonymizer
 * @date    2013.
 */
class IODICOMEXT_CLASS_API SDicomSeriesAnonymizer : public ::fwServices::IController
{

public:
    fwCoreServiceClassDefinitionsMacro ( (SDicomSeriesAnonymizer)( ::fwServices::IController) );

    /**
     * @brief   constructor
     */
    IODICOMEXT_API SDicomSeriesAnonymizer() throw();

    /**
     * @brief   destructor
     */
    IODICOMEXT_API virtual ~SDicomSeriesAnonymizer() throw();

protected:

    /**
     * @brief Configuring method. This method is used to configure the service.
     *
     * XML configuration sample:
       @verbatim
       <service uid="anonymizeSelection" impl="::ioDicomExt::gdcm::SDicomSeriesAnonymizer" autoConnect="yes">
         <config seriesDBUID="mySeriesDB" />
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

    /// Override
    IODICOMEXT_API void anonymize();

    /// SeriesDB UID
    std::string m_seriesDBUID;

    /// SeriesDB of the Series
    SPTR(::fwMedData::SeriesDB) m_seriesDB;
};

} // namespace gdcm
} // namespace ioDicomExt

#endif //__IODICOMEXT_GDCM_SDICOMSERIESANONYMIZER_HPP__
