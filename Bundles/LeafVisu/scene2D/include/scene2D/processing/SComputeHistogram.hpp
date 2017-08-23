/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
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
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service uid="computeHistogram" type="::scene2D::processing::SComputeHistogram" autoConnect="yes">
    <inout key="histogram" uid="..." />
    <in key="image" uid="..." />
    <binsWidth>5.0</binsWidth>
   </service>
   @endcode
 *
 * @subsection In-Out In-Out
 * - \b histogram [::fwData::Histogram]: histogram to compute.
 *
 * @subsection In In
 * - \b image [::fwData::Image]: image used to compute histogram.
 *
 * @subsection Configuration Configuration:
 * - \b binsWidth (mandatory): desired bins width used to classified pixel.
 */
class SCENE2D_CLASS_API SComputeHistogram : public ::fwServices::IController
{

public:

    fwCoreServiceClassDefinitionsMacro( (SComputeHistogram)(::fwServices::IController) );

    /// Constructor. Does nothing.
    SCENE2D_API SComputeHistogram() noexcept;

    /// Destructor. Does nothing.
    SCENE2D_API virtual ~SComputeHistogram() noexcept;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Image::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     * Connect Image::s_BUFFER_MODIFIED_SIG to this::s_UPDATE_SLOT
     */
    SCENE2D_API virtual KeyConnectionsMap getAutoConnections() const;

protected:

    SCENE2D_API void configuring();

    /// Starts the service. Calls updating().
    SCENE2D_API void starting();

    /// Computes the histogram and notifies Histogram::s_MODIFIED_SIG.
    SCENE2D_API void updating();

    /// Swaps the service. Calls updating().
    SCENE2D_API void swapping();

    /// Stops the service. Does nothing.
    SCENE2D_API void stopping();

private:

    /// Desired bins width used to classified pixel.
    float m_binsWidth;

};
} // namespace processing
} // namespace scene2D

#endif // __SCENE2D_PROCESSING_SCOMPUTEHISTOGRAM_HPP__

