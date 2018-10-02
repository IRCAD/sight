/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "visuVTKAdaptor/config.hpp"

#include <fwData/TransferFunction.hpp>

#include <fwDataTools/helper/MedicalImage.hpp>

#include <fwRenderVTK/IAdaptor.hpp>

class vtkCommand;

namespace visuVTKAdaptor
{

/**
 * @brief Manage windowing interaction for image
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::visuVTKAdaptor::SNegatoWindowingInteractor" autoConnect="yes">
       <inout key="image" uid="..." />
       <inout key="tf" uid="..." optional="yes" />
       <config picker="negatodefault" />
   </service>
   @endcode
 * @subsection In-Out In-Out
 * - \b image [::fwData::Image]: image to display.
 * - \b tf [::fwData::TransferFunction] (optional): the current TransferFunction. If it is not defined, we use the
 *      image's default transferFunction
 *
 * @subsection Configuration Configuration:
 * - \b config(mandatory) : contains the adaptor configuration
 *    - \b picker (mandatory): identifier of the picker
 */
class VISUVTKADAPTOR_CLASS_API SNegatoWindowingInteractor : public ::fwDataTools::helper::MedicalImage,
                                                            public ::fwRenderVTK::IAdaptor
{

public:

    fwCoreServiceClassDefinitionsMacro( (SNegatoWindowingInteractor)(::fwRenderVTK::IAdaptor) );

    VISUVTKADAPTOR_API SNegatoWindowingInteractor() noexcept;

    VISUVTKADAPTOR_API virtual ~SNegatoWindowingInteractor() noexcept;

    VISUVTKADAPTOR_API void startWindowing();

    VISUVTKADAPTOR_API void stopWindowing();

    VISUVTKADAPTOR_API void updateWindowing( double, double);

    VISUVTKADAPTOR_API void resetWindowing();

protected:

    VISUVTKADAPTOR_API void configuring() override;
    VISUVTKADAPTOR_API void starting() override;
    VISUVTKADAPTOR_API void updating() override;
    VISUVTKADAPTOR_API void stopping() override;

    /// Select the current tf
    VISUVTKADAPTOR_API void swapping(const KeyType& key) override;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Image::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     * Connect Image::s_BUFFER_MODIFIED_SIG to this::s_UPDATE_SLOT
     */
    VISUVTKADAPTOR_API virtual KeyConnectionsMap getAutoConnections() const override;

    vtkCommand* m_vtkObserver;
    double m_initialWindow;
    double m_initialLevel;
    float m_priority;
};

} //namespace visuVTKAdaptor
