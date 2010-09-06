/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _VISUVTKADAPTOR_PLANELIST_HPP_
#define _VISUVTKADAPTOR_PLANELIST_HPP_

#include <fwRenderVTK/IVtkAdaptorService.hpp>

#include "visuVTKAdaptor/config.hpp"
#include "visuVTKAdaptor/MeshFactory.hpp"


class vtkCommand;

namespace visuVTKAdaptor
{


class VISUVTKADAPTOR_CLASS_API PlaneList: public ::fwRenderVTK::IVtkAdaptorService
{

public:

    fwCoreServiceClassDefinitionsMacro ( (PlaneList)(::fwRenderVTK::IVtkAdaptorService) ) ;

    VISUVTKADAPTOR_API PlaneList() throw();

    VISUVTKADAPTOR_API virtual ~PlaneList() throw();

    VISUVTKADAPTOR_API void setPlaneCollectionId(::fwRenderVTK::VtkRenderService::VtkObjectIdType id) { m_planeCollectionId = id; }

    VISUVTKADAPTOR_API ::fwRenderVTK::VtkRenderService::VtkObjectIdType getPlaneCollectionId() { return m_planeCollectionId; }

protected:

    VISUVTKADAPTOR_API void doStart() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doStop() throw(fwTools::Failed);

    VISUVTKADAPTOR_API void configuring() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doSwap() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doUpdate() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doUpdate(::fwServices::ObjectMsg::csptr msg) throw(fwTools::Failed);



    vtkCommand * m_rightButtonCommand;
    ::fwRenderVTK::VtkRenderService::VtkObjectIdType m_planeCollectionId;
};




} //namespace visuVTKAdaptor

#endif // _VISUVTKADAPTOR_PLANELIST_HPP_
