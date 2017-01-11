/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __SCENE2D_PROCESSING_SCOMPUTEHISTOGRAM_HPP__
#define __SCENE2D_PROCESSING_SCOMPUTEHISTOGRAM_HPP__

#include "scene2D/config.hpp"

#include <fwData/Histogram.hpp>
#include <fwData/Image.hpp>

#include <fwServices/IController.hpp>

namespace scene2D
{

namespace processing
{

/**
 * @brief The SComputeHistogram service computes the histogram of the image.
 */
class SCENE2D_CLASS_API SComputeHistogram : public ::fwServices::IController
{

public:

    fwCoreServiceClassDefinitionsMacro ( (SComputeHistogram)(::fwServices::IController) );

    /// Constructor. Does nothing.
    SCENE2D_API SComputeHistogram() throw();

    /// Destructor. Does nothing.
    SCENE2D_API virtual ~SComputeHistogram() throw();

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Image::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     * Connect Image::s_BUFFER_MODIFIED_SIG to this::s_UPDATE_SLOT
     */
    SCENE2D_API virtual KeyConnectionsType getObjSrvConnections() const;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Image::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     * Connect Image::s_BUFFER_MODIFIED_SIG to this::s_UPDATE_SLOT
     */
    SCENE2D_API virtual KeyConnectionsMap getAutoConnections() const;

protected:

    /**
     * @brief Configures the service.
     * @code{.xml}
       <service uid="computeHistogram" type="::fwServices::IController" impl="::scene2D::processing::SComputeHistogram" autoConnect="yes">
        <histogramId>Histogram</histogramId>
        <binsWidth>5.0</binsWidth>
       </service>
       @endcode
     * - \b histogramId: Id of the histogram object.
     * - \b binsWidth: desired bins width used to classified pixel.
     */
    SCENE2D_API void configuring() throw ( ::fwTools::Failed );

    /// Starts the service. Calls updating().
    SCENE2D_API void starting()    throw ( ::fwTools::Failed );

    /// Computes the histogram and notifies Histogram::s_MODIFIED_SIG.
    SCENE2D_API void updating()    throw ( ::fwTools::Failed );

    /// Swaps the service. Calls updating().
    SCENE2D_API void swapping()    throw ( ::fwTools::Failed );

    /// Stops the service. Does nothing.
    SCENE2D_API void stopping()    throw ( ::fwTools::Failed );

private:

    /// Returns the histogram object from the id m_histogramId.
    ::fwData::Histogram::sptr getHistogram();

    /// FwID of the histogram
    std::string m_histogramId;

    /// Desired bins width used to classified pixel.
    float m_binsWidth;

};
} // namespace processing
} // namespace scene2D


#endif // __SCENE2D_PROCESSING_SCOMPUTEHISTOGRAM_HPP__

