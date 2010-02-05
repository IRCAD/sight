/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWRENDER_IRENDER_HPP_
#define _FWRENDER_IRENDER_HPP_

#include <fwTools/Failed.hpp>
#include <fwServices/IService.hpp>
#include <fwData/Integer.hpp>

#include <fwWX/Timer.hpp>

#include "fwRender/config.hpp"

class wxWindow;

/**
 * @brief   The namespace fwRender contains the base interface for rendering services.
 * @namespace   fwRender
 * @author  IRCAD (Research and Development Team).
 * @date    2007-2009.
 *
 */
namespace fwRender
{

/**
 * @brief   Defines the service interface managing the rendering service for object.
 * @class   IRender.
 * @author  IRCAD (Research and Development Team).
 * @date    2007-2009.
 *
 * Use the base service methods :
 * @li The service is configured with methods setConfiguration(cfg) and configure()
 * @li The method start() initialize the service
 * @li Use update() method to update data
 * @li To react on modifications use update(msg)
 * @li Finally we must call stop() before deleting the service
 *
 */
class FWRENDER_CLASS_API IRender : public fwServices::IService
{

public :
    fwCoreServiceClassDefinitionsMacro ( (IRender)(::fwServices::IService::Baseclass) ) ;

    /// Clock rate identifiant (for persistance)
    static const std::string ClockRateId ;

    /**
     * @brief   Constructor. Initialize default values.
     */
    FWRENDER_API IRender() throw() ;

    /**
     * @brief   Destructor. Do nothing.
     */
    FWRENDER_API virtual ~IRender() throw() ;



    /**
     * @name    Helper service methods
     */
    //@{

    /**
     * @brief Configure the service (use it in configuring, before start).
     *
     * Configure clock rate if it's specified in the configuration.
     */
    FWRENDER_API void initClockRate();

    /**
     * @brief Configure the service (use it in configuring, before start).
     *
     * Configure gui container if it's specified in the configuration.
     */
    FWRENDER_API void initGuiContainerId();

    /**
     * @brief Initialize the Render.
     *
     * This method initializes the gui container ( create a new frame if the container is not configured).
     * It starts timer if clock is configured and register communication channel.
     *
     * @note This method should be called in starting() method by concrete services.
     */
    FWRENDER_API void initRender();

    /**
     * @brief Stop the timer and destroy the container if they where configured.
     *
     * @note This method should be called in stopping() method by concrete services.
     */
    FWRENDER_API void stopRender();
    //@}



    /**
     * @name    Standard service methods
     */
    //@{

    /**
     * @brief Info method.
     *
     * This method is used to give
     * informations about the service clock rate.
     *
     * @param[out] _sstream output stream
     */
    FWRENDER_API virtual void info(std::ostream &_sstream ) ;

    //@}

    /**
     * @name    Specific service methods for visualization management
     */
    //@{
    /**
     * @brief   Specify container within which visualization will appear
     */
    FWRENDER_API virtual void setWxContainer( wxWindow * container ) ;

    /**
     * @brief Returns visualization wxContainer
     */
    FWRENDER_API virtual wxWindow * getWxContainer() ;

    /**
     * @brief   defines the clock rate
     *
     * @param[in] _rate clock rate
     */
    FWRENDER_API virtual void setClockRate(int _rate) ;

    /**
     * @brief   retrieve the clock rate
     *
     * @li m_clockRate.first == true if clock rate is configured
     * @li m_clockRate.second = clock rate value
     */
    FWRENDER_API  virtual std::pair< bool , int > getClockRate() ;

    //@}

protected :

    /// container
    wxWindow * m_container;

    /**
     * @brief gui container indentifier
     *
     * @li m_guiContainerId.first == true if gui container is configured
     * @li m_guiContainerId.second = container wx ID
     */
    std::pair< bool , int > m_guiContainerId ;

    /**
     * @brief gui container indentifier
     *
     * @li m_clockRate.first == true if clock rate is configured
     * @li m_clockRate.second = clock rate value
     **/
    std::pair< bool , int > m_clockRate ;

    /// Clock used to refresh view
    ::fwWX::Timer<>::ptr m_timer ;

    /// flag if the container is locally created (ie the container was not configured)
    bool m_isContainerLocallyCreated ;

};

} // namespace fwRenderVTK

#endif /*_FWRENDER_IRENDER_HPP_*/


