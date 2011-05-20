/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _VISUVTKADAPTOR_POINTLIST_HPP_
#define _VISUVTKADAPTOR_POINTLIST_HPP_

#include <set>
#include <vector>

#include <fwRenderVTK/IVtkAdaptorService.hpp>

#include "visuVTKAdaptor/config.hpp"
#include "visuVTKAdaptor/MeshFactory.hpp"

namespace visuVTKAdaptor
{

class VISUVTKADAPTOR_CLASS_API PointList: public ::fwRenderVTK::IVtkAdaptorService
{

public:
    typedef std::vector< WPTR(::fwData::Point) > WeakPointListType;
    typedef std::set< WPTR(::fwData::Point) > WeakPointSetType;

    fwCoreServiceClassDefinitionsMacro ( (PointList)(::fwRenderVTK::IVtkAdaptorService) ) ;

    VISUVTKADAPTOR_API PointList() throw();

    VISUVTKADAPTOR_API virtual ~PointList() throw();

protected:

    VISUVTKADAPTOR_API void doStart() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doStop() throw(fwTools::Failed);

    VISUVTKADAPTOR_API void configuring() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doSwap() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doUpdate() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doUpdate(::fwServices::ObjectMsg::csptr msg) throw(fwTools::Failed);

    VISUVTKADAPTOR_API void createServices(WeakPointListType &wPtList);
    VISUVTKADAPTOR_API WeakPointListType getWeakPointList();
    VISUVTKADAPTOR_API WeakPointListType getNewPoints();

     WeakPointListType m_oldWeakPointList;
     WeakPointListType m_weakPointList;

};




} //namespace visuVTKAdaptor

#endif // _VISUVTKADAPTOR_POINTLIST_HPP_
