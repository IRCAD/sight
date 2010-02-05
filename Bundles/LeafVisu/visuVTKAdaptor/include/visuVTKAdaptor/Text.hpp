/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef fwRenderVTK_IVtkAdaptorService_Text
#define fwRenderVTK_IVtkAdaptorService_Text


#include <string>

#include <fwRenderVTK/IVtkAdaptorService.hpp>

#include "visuVTKAdaptor/config.hpp"

class vtkTextActor;
//class vtkActor2D;
class vtkTextMapper;

namespace visuVTKAdaptor
{

class VISUVTKADAPTOR_CLASS_API Text: public ::fwRenderVTK::IVtkAdaptorService
{

public:
    fwCoreServiceClassDefinitionsMacro ( (Text)(::fwRenderVTK::IVtkAdaptorService::Baseclass) ) ;

    VISUVTKADAPTOR_API Text() throw();
    VISUVTKADAPTOR_API virtual ~Text() throw();

    VISUVTKADAPTOR_API virtual void setText(std::string str);
    VISUVTKADAPTOR_API virtual std::string getText(){return m_text;};

protected:

    VISUVTKADAPTOR_API virtual void doStart() throw(fwTools::Failed);
    VISUVTKADAPTOR_API virtual void doStop() throw(fwTools::Failed);
    VISUVTKADAPTOR_API virtual void doSwap() throw(fwTools::Failed){};
    VISUVTKADAPTOR_API virtual void doUpdate() throw(fwTools::Failed){};
    VISUVTKADAPTOR_API virtual void doUpdate(::fwServices::ObjectMsg::csptr msg) throw(fwTools::Failed){};
    VISUVTKADAPTOR_API virtual void configuring() throw(fwTools::Failed);
 

    std::string m_text;

    vtkActor2D * m_actor;
    vtkTextMapper * m_mapper;

};




} //namespace visuVTKAdaptor

#endif //fwRenderVTK_IVtkAdaptorService_Text
