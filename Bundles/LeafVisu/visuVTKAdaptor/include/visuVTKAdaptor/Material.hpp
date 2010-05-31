/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _VISUVTKADAPTOR_MATERIAL_HPP_
#define _VISUVTKADAPTOR_MATERIAL_HPP_


#include <fwServices/ObjectMsg.hpp>

#include <fwRenderVTK/IVtkAdaptorService.hpp>

#include "visuVTKAdaptor/config.hpp"


class VISUVTKADAPTOR_CLASS_API vtkProperty;


namespace visuVTKAdaptor
{



class VISUVTKADAPTOR_CLASS_API Material: public ::fwRenderVTK::IVtkAdaptorService
{

public:

    fwCoreServiceClassDefinitionsMacro ( (Material)(::fwRenderVTK::IVtkAdaptorService) ) ;

    VISUVTKADAPTOR_API Material() throw();

    VISUVTKADAPTOR_API virtual ~Material() throw();

    VISUVTKADAPTOR_API void setVtkProperty(vtkProperty *property);

    VISUVTKADAPTOR_API vtkProperty *getVtkProperty();

protected:

    VISUVTKADAPTOR_API void doStart() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doStop() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void configuring() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doSwap() throw(fwTools::Failed);
   VISUVTKADAPTOR_API void doUpdate() throw(fwTools::Failed);
   VISUVTKADAPTOR_API void doUpdate( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed);

    void updateMaterial( ::fwData::Material::sptr material );


    vtkProperty *m_property;
    bool m_manageProperty;

};




} //namespace visuVTKAdaptor

#endif // _VISUVTKADAPTOR_MATERIAL_HPP_
