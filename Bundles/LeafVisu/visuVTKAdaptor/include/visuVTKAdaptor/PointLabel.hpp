/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _VISUVTKADAPTOR_POINTLABEL_HPP_
#define _VISUVTKADAPTOR_POINTLABEL_HPP_

#include <string>

#include <fwRenderVTK/IVtkAdaptorService.hpp>

#include "visuVTKAdaptor/config.hpp"
#include "visuVTKAdaptor/Text.hpp"

class VISUVTKADAPTOR_CLASS_API vtkTextActor;

namespace visuVTKAdaptor
{



class VISUVTKADAPTOR_CLASS_API PointLabel: public Text
{

public:
    fwCoreServiceClassDefinitionsMacro ( (PointLabel)(::fwRenderVTK::IVtkAdaptorService) ) ;

    VISUVTKADAPTOR_API PointLabel();

protected:

    VISUVTKADAPTOR_API void doUpdate() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doUpdate( ::fwServices::ObjectMsg::csptr msg ) throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doSwap() throw(fwTools::Failed);

    void starting() throw(::fwTools::Failed);
    void stopping() throw(::fwTools::Failed);




};



} //namespace visuVTKAdaptor

#endif //  _VISUVTKADAPTOR_POINTLABEL_HPP_

