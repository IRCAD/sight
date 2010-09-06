/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#ifndef _VISUVTKADAPTOR_PLANESELECTIONNOTIFIER_HPP_
#define _VISUVTKADAPTOR_PLANESELECTIONNOTIFIER_HPP_

#include <fwData/Object.hpp>
#include <fwData/PlaneList.hpp>

#include <fwRenderVTK/IVtkAdaptorService.hpp>

#include "visuVTKAdaptor/config.hpp"

namespace visuVTKAdaptor
{

class VISUVTKADAPTOR_CLASS_API PlaneSelectionNotifier: public ::fwRenderVTK::IVtkAdaptorService
{

public:

    fwCoreServiceClassDefinitionsMacro ( (PlaneSelectionNotifier)(::fwRenderVTK::IVtkAdaptorService) ) ;

    VISUVTKADAPTOR_API PlaneSelectionNotifier() throw();

    VISUVTKADAPTOR_API virtual ~PlaneSelectionNotifier() throw();

    VISUVTKADAPTOR_API void setPlaneListId(std::string id)
    { m_planeListId = id; }

    VISUVTKADAPTOR_API void setPlaneSelectionId(std::string id)
    { m_planeSelectionId = id;}

    VISUVTKADAPTOR_API void selectPlane( ::fwData::Object::sptr plane );

    VISUVTKADAPTOR_API void deselectPlane();

protected:

    VISUVTKADAPTOR_API void doStart() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doStop() throw(fwTools::Failed);

    VISUVTKADAPTOR_API void configuring() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doSwap() throw(fwTools::Failed);
    
    VISUVTKADAPTOR_API void doUpdate() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doUpdate(::fwServices::ObjectMsg::csptr msg) throw(fwTools::Failed);

    std::string m_planeListId;
    std::string m_planeSelectionId;

    ::fwData::PlaneList::wptr m_currentPlaneList;
};




} //namespace visuVTKAdaptor

#endif // _VISUVTKADAPTOR_PLANESELECTIONNOTIFIER_HPP_
