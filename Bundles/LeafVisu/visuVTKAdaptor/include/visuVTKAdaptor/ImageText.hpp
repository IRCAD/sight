/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include <string>

#include <fwRenderVTK/IVtkAdaptorService.hpp>
#include <fwComEd/helper/MedicalImageAdaptor.hpp>

#include "visuVTKAdaptor/Text.hpp"
#include "visuVTKAdaptor/config.hpp"

class vtkTextActor;

namespace visuVTKAdaptor
{

class VISUVTKADAPTOR_CLASS_API ImageText: public Text, public ::fwComEd::helper::MedicalImageAdaptor
{

public:

    fwCoreServiceClassDefinitionsMacro ( (ImageText)(::fwRenderVTK::IVtkAdaptorService) ) ;

    VISUVTKADAPTOR_API ImageText() throw();

    VISUVTKADAPTOR_API virtual ~ImageText() throw();

protected:

    VISUVTKADAPTOR_API void doStart() throw(fwTools::Failed);

    VISUVTKADAPTOR_API void doUpdate() throw(fwTools::Failed);

    VISUVTKADAPTOR_API void doUpdate(::fwServices::ObjectMsg::csptr msg) throw(fwTools::Failed);

    VISUVTKADAPTOR_API void doSwap() throw(fwTools::Failed);




};




} //namespace visuVTKAdaptor
