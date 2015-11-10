/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKADAPTOR_POINT_HPP__
#define __VISUVTKADAPTOR_POINT_HPP__

#ifndef ANDROID

#include <fwServices/ObjectMsg.hpp>
#include <fwRenderVTK/IVtkAdaptorService.hpp>

#include "visuVTKAdaptor/config.hpp"

class vtkHandleWidget;
class vtkHandleRepresentation;
class vtkCommand;



namespace visuVTKAdaptor
{


class VISUVTKADAPTOR_CLASS_API Point : public ::fwRenderVTK::IVtkAdaptorService
{

public:

    fwCoreServiceClassDefinitionsMacro ( (Point)(::fwRenderVTK::IVtkAdaptorService) );

    VISUVTKADAPTOR_API Point() throw();

    VISUVTKADAPTOR_API virtual ~Point() throw();

    VISUVTKADAPTOR_API void setColor(double red, double green, double blue, double alpha = 1.0);

    VISUVTKADAPTOR_API void setSelectedColor(double red, double green, double blue, double alpha = 1.0);

protected:

    VISUVTKADAPTOR_API void doStart() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void configuring() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doSwap() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doUpdate() throw(fwTools::Failed);
    VISUVTKADAPTOR_API virtual void doReceive( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed);
    VISUVTKADAPTOR_API void doStop() throw(fwTools::Failed);



protected:
    vtkHandleWidget         * m_handle;
    vtkHandleRepresentation * m_representation;
    vtkCommand              * m_pointUpdateCommand;

};
} //namespace visuVTKAdaptor

#endif // ANDROID

#endif // __VISUVTKADAPTOR_POINT_HPP__
