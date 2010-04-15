/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _VISUVTKADAPTOR_IMAGEMULTIDISTANCES_HPP_
#define _VISUVTKADAPTOR_IMAGEMULTIDISTANCES_HPP_


#include <fwData/PointList.hpp>
#include <fwServices/ObjectMsg.hpp>
#include <fwRenderVTK/IVtkAdaptorService.hpp>

#include "visuVTKAdaptor/config.hpp"

class vtkCommand;

namespace visuVTKAdaptor
{



class VISUVTKADAPTOR_CLASS_API ImageMultiDistances: public ::fwRenderVTK::IVtkAdaptorService
{

public:

    fwCoreServiceClassDefinitionsMacro ( (ImageMultiDistances)(::fwRenderVTK::IVtkAdaptorService) ) ;

    VISUVTKADAPTOR_API ImageMultiDistances() throw();

    VISUVTKADAPTOR_API ~ImageMultiDistances() throw();

    VISUVTKADAPTOR_API void setNeedSubservicesDeletion( bool _needSubservicesDeletion );

    VISUVTKADAPTOR_API virtual void show(bool b = true);

protected :

    VISUVTKADAPTOR_API void doStart() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void configuring() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doSwap() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doUpdate() throw(fwTools::Failed);
    VISUVTKADAPTOR_API virtual void doUpdate( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed);
    VISUVTKADAPTOR_API void doStop() throw(fwTools::Failed);

    void installSubServices( ::fwData::PointList::sptr pl );
    ::fwData::Point::sptr screenToWorld(int X,int Y);
    void removeDistance(  ::fwData::PointList::sptr plToRemove ) throw(::fwTools::Failed);
    void createNewDistance( std::string sceneId ) throw(::fwTools::Failed);

    std::list< ::fwRenderVTK::IVtkAdaptorService::sptr > m_subServices;

    vtkCommand * m_rightButtonCommand;

    bool m_needSubservicesDeletion;

};




} //namespace visuVTKAdaptor

#endif // _VISUVTKADAPTOR_IMAGEMULTIDISTANCES_HPP_
