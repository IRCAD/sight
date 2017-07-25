/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKADAPTOR_SNEGATOWINDOWINGINTERACTOR_HPP__
#define __VISUVTKADAPTOR_SNEGATOWINDOWINGINTERACTOR_HPP__

#include "visuVTKAdaptor/config.hpp"

#include <fwData/TransferFunction.hpp>

#include <fwDataTools/helper/MedicalImageAdaptor.hpp>

#include <fwRenderVTK/IAdaptor.hpp>

class vtkCommand;

namespace visuVTKAdaptor
{

/**
 * @brief Manage windowing interaction for image
 *
 * @code{.xml}
   <service type="::visuVTKAdaptor::SNegatoWindowingInteractor" autoConnect="yes">
       <inout key="image" uid="..." />
       <inout key="tfSelection" uid="..." />
       <config picker="negatodefault" selectedTFKey="tkKey" />
   </service>
   @endcode
 * @subsection In-Out In-Out
 * - \b image [::fwData::Image]: image to display.
 * - \b tfSelection [::fwData::Composite] (optional): composite containing the TransferFunction.
 *
 * @subsection Configuration Configuration:
 * - \b config(mandatory) : contains the adaptor configuration
 *    - \b picker (mandatory): identifier of the picker
 *    - \b selectedTFKey (optional): key of the transfer function to use in negato
 */
class VISUVTKADAPTOR_CLASS_API SNegatoWindowingInteractor : public ::fwDataTools::helper::MedicalImageAdaptor,
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

    VISUVTKADAPTOR_API void configuring();
    VISUVTKADAPTOR_API void starting();
    VISUVTKADAPTOR_API void updating();
    VISUVTKADAPTOR_API void stopping();

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Image::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     * Connect Image::s_BUFFER_MODIFIED_SIG to this::s_UPDATE_SLOT
     */
    VISUVTKADAPTOR_API virtual KeyConnectionsMap getAutoConnections() const;

    vtkCommand* m_vtkObserver;
    double m_initialWindow;
    double m_initialLevel;
    double m_priority;
};

} //namespace visuVTKAdaptor

#endif // __VISUVTKADAPTOR_SNEGATOWINDOWINGINTERACTOR_HPP__
