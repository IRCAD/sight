/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKADAPTOR_NEGATOWINDOWINGINTERACTOR_HPP__
#define __VISUVTKADAPTOR_NEGATOWINDOWINGINTERACTOR_HPP__

#include "visuVTKAdaptor/config.hpp"

#include <fwData/TransferFunction.hpp>

#include <fwDataTools/helper/MedicalImageAdaptor.hpp>

#include <fwRenderVTK/IVtkAdaptorService.hpp>

class vtkCommand;

namespace visuVTKAdaptor
{



class VISUVTKADAPTOR_CLASS_API NegatoWindowingInteractor : public ::fwDataTools::helper::MedicalImageAdaptor,
                                                           public ::fwRenderVTK::IVtkAdaptorService
{

public:

    fwCoreServiceClassDefinitionsMacro ( (NegatoWindowingInteractor)(::fwRenderVTK::IVtkAdaptorService) );

    VISUVTKADAPTOR_API NegatoWindowingInteractor() noexcept;

    VISUVTKADAPTOR_API virtual ~NegatoWindowingInteractor() noexcept;

    VISUVTKADAPTOR_API void startWindowing();

    VISUVTKADAPTOR_API void stopWindowing();

    VISUVTKADAPTOR_API void updateWindowing( double, double);

    VISUVTKADAPTOR_API void resetWindowing();

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Image::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     * Connect Image::s_BUFFER_MODIFIED_SIG to this::s_UPDATE_SLOT
     */
    VISUVTKADAPTOR_API virtual KeyConnectionsType getObjSrvConnections() const;

protected:

    VISUVTKADAPTOR_API void doStart();
    VISUVTKADAPTOR_API void doStop();

    VISUVTKADAPTOR_API void doConfigure();
    VISUVTKADAPTOR_API void doSwap();
    // redraw all (stop then restart sub services)
    VISUVTKADAPTOR_API void doUpdate();

    vtkCommand* m_vtkObserver;
    double m_initialWindow;
    double m_initialLevel;
    double m_priority;
};




} //namespace visuVTKAdaptor

#endif // __VISUVTKADAPTOR_NEGATOWINDOWINGINTERACTOR_HPP__
