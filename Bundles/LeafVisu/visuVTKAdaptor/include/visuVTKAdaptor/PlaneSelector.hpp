/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#ifndef _VISUVTKADAPTOR_PLANESELECTOR_HPP_
#define _VISUVTKADAPTOR_PLANESELECTOR_HPP_

#include <fwData/Object.hpp>
#include <fwData/PlaneList.hpp>

#include <fwRenderVTK/IVtkAdaptorService.hpp>

#include "visuVTKAdaptor/config.hpp"

namespace visuVTKAdaptor
{

class VISUVTKADAPTOR_CLASS_API PlaneSelector: public ::fwRenderVTK::IVtkAdaptorService
{

public:

    fwCoreServiceClassDefinitionsMacro ( (PlaneSelector)(::fwRenderVTK::IVtkAdaptorService::Baseclass) ) ;

    VISUVTKADAPTOR_API PlaneSelector() throw();

    VISUVTKADAPTOR_API virtual ~PlaneSelector() throw();

    VISUVTKADAPTOR_API void selectObject( ::fwData::Object::sptr object );

protected:

    VISUVTKADAPTOR_API void doStart() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doStop() throw(fwTools::Failed);

    VISUVTKADAPTOR_API void configuring() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doSwap() throw(fwTools::Failed);
    
    VISUVTKADAPTOR_API void doUpdate() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doUpdate(::fwServices::ObjectMsg::csptr msg) throw(fwTools::Failed);

    ::fwData::Object::wptr m_currentObject;
};




} //namespace visuVTKAdaptor

#endif // _VISUVTKADAPTOR_PLANESELECTOR_HPP_
