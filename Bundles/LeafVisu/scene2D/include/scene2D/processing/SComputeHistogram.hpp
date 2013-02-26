/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __SCENE2D_PROCESSING_SCOMPUTEHISTOGRAM_HPP__
#define __SCENE2D_PROCESSING_SCOMPUTEHISTOGRAM_HPP__

#include <fwData/Histogram.hpp>
#include <fwData/Image.hpp>

#include <fwServices/IController.hpp>

#include "scene2D/config.hpp"

namespace scene2D
{

namespace processing
{

/**
 * @brief The SComputeHistogram service computes the histogram of the image.
 * @class SComputeHistogram
 * @date 2013
 */
class SCENE2D_CLASS_API SComputeHistogram : public ::fwServices::IController
{

public:

    fwCoreServiceClassDefinitionsMacro ( (SComputeHistogram)(::fwServices::IController) ) ;

    /// Constructor. Does nothing.
    SCENE2D_API SComputeHistogram() throw();

    /// Destructor. Does nothing.
    SCENE2D_API virtual ~SComputeHistogram() throw();

protected:

    /**
    * @brief Configures the service.
    * @verbatim
    <service uid="computeHistogram" type="::fwServices::IController" impl="::scene2D::processing::SComputeHistogram" autoConnect="yes">
        <histogramId>Histogram</histogramId>
        <binsWidth>5.0</binsWidth>
    </service>
    @endverbatim
    * - \b histogramId: Id of the histogram object.
    * - \b binsWidth: desired bins width used to classified pixel.
    */
    SCENE2D_API void configuring() throw ( ::fwTools::Failed );

    /// Starts the service. Calls updating().
    SCENE2D_API void starting()    throw ( ::fwTools::Failed );

    /// Computes the histogram and notifies VALUE_IS_MODIFIED.
    SCENE2D_API void updating()    throw ( ::fwTools::Failed );

    /// Updates the histogram object when it receives image message (BUFFER, MODIFIED, NEW_IMAGE).
    SCENE2D_API void receiving( fwServices::ObjectMsg::csptr _msg ) throw ( ::fwTools::Failed );

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

