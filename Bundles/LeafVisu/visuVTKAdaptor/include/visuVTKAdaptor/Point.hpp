/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKADAPTOR_POINT_HPP__

#ifndef __VISUVTKADAPTOR_POINT_HPP__
#define __VISUVTKADAPTOR_POINT_HPP__

#include "visuVTKAdaptor/config.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signals.hpp>

#include <fwRenderVTK/IVtkAdaptorService.hpp>

class vtkHandleWidget;
class vtkHandleRepresentation;
class vtkCommand;

namespace visuVTKAdaptor
{

class VISUVTKADAPTOR_CLASS_API Point : public ::fwRenderVTK::IVtkAdaptorService
{

public:

    fwCoreServiceClassDefinitionsMacro( (Point)(::fwRenderVTK::IVtkAdaptorService) );

    VISUVTKADAPTOR_API Point() throw();

    VISUVTKADAPTOR_API virtual ~Point() throw();

    VISUVTKADAPTOR_API void setRadius(double radius);

    VISUVTKADAPTOR_API void setColor(double red, double green, double blue, double alpha = 1.0);

    VISUVTKADAPTOR_API void setSelectedColor(double red, double green, double blue, double alpha = 1.0);

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Point::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     */
    VISUVTKADAPTOR_API virtual KeyConnectionsType getObjSrvConnections() const;

    /**
     * @Signals
     * @{
     */
    /// Type of signal when point interaction is started
    typedef ::fwCom::Signal< void () > InteractionStartedSignalType;
    VISUVTKADAPTOR_API static const ::fwCom::Signals::SignalKeyType s_INTERACTION_STARTED_SIG;
    /**
     * @}
     */

protected:

    VISUVTKADAPTOR_API void doStart() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doConfigure() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doSwap() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doUpdate() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doStop() throw(fwTools::Failed);

private:
    vtkHandleWidget* m_handle;
    vtkHandleRepresentation* m_representation;
    vtkCommand* m_pointUpdateCommand;
};

} //namespace visuVTKAdaptor

#endif // __VISUVTKADAPTOR_POINT_HPP__

#endif // __VISUVTKADAPTOR_POINT_HPP__
