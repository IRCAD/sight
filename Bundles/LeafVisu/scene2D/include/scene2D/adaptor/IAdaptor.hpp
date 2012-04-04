/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _SCENE2D_ADAPTOR_IADAPTOR_HPP_
#define _SCENE2D_ADAPTOR_IADAPTOR_HPP_

#include <fwServices/IService.hpp>

#include "scene2D/config.hpp"
#include "scene2D/Render.hpp"
#include "scene2D/data/Axis.hpp"
#include "scene2D/data/Event.hpp"



namespace scene2D
{
namespace adaptor
{

class SCENE2D_CLASS_API IAdaptor : public ::fwServices::IService
{

public:

    // Point2D coordinate <X, Y>
    typedef std::pair< double, double > Point2DType;

    // <width, height>
    typedef std::pair<float, float> ViewSizeRatio;

    // <width, height>
    typedef std::pair<float, float> ViewportSizeRatio;

    // <width, height>
    typedef std::pair<float, float> Scene2DRatio;

    fwCoreServiceClassDefinitionsMacro ( (IAdaptor)(::fwServices::IService) ) ;

    /// Set the zValue.
    SCENE2D_API void setZValue(float _zValue);

    /// Get the zValue.
    SCENE2D_API float getZValue();

    /// Set the render that manage the IAdaptor.
    SCENE2D_API void setScene2DRender( ::scene2D::Render::sptr _scene2DRender);

    /// Get the render that manage the IAdaptor.
    SCENE2D_API SPTR(::scene2D::Render) getScene2DRender();

    /// Get the object associated to the IAdaptor.
    SCENE2D_API ::fwData::Object::sptr getRegisteredObject(::scene2D::Render::ObjectIDType _objectId);

    /// Interact with the mouse events catched on the IAdaptor (virtual function, its behavior is only defined in the specific adaptors).
    SCENE2D_API virtual void processInteraction( ::scene2D::data::Event::sptr _event );


protected:

    /// Constructor, set the zValue to 0.
    SCENE2D_API IAdaptor() throw();

    /// Basic destructor, do nothing.
    SCENE2D_API virtual ~IAdaptor() throw();

    /// ToDo IM
    SCENE2D_API virtual void info(std::ostream &_sstream ) ;

    /// Not implemented in IAdaptor but in its subclasses
    SCENE2D_API void configuring() throw ( ::fwTools::Failed );

    /// Call doStart() function.
    SCENE2D_API void starting() throw ( ::fwTools::Failed );

    /// Call DoUpdate() function.
    SCENE2D_API void updating() throw ( ::fwTools::Failed );

    /// Call DoUpdate(_msg) function.
    SCENE2D_API void updating( ::fwServices::ObjectMsg::csptr _msg ) throw ( ::fwTools::Failed );

    /// Start and stop the IAdaptor.
    SCENE2D_API void swapping() throw ( ::fwTools::Failed );

    /// Call doStop() function and reset the axis.
    SCENE2D_API void stopping()    throw ( ::fwTools::Failed );

    /// Pure virtual -> implemented in the subclasses
    SCENE2D_API virtual void doStart() = 0;

    /// Pure virtual -> implemented in the subclasses
    SCENE2D_API virtual void doUpdate() = 0;

    /// Pure virtual -> implemented in the subclasses
    SCENE2D_API virtual void doUpdate( ::fwServices::ObjectMsg::csptr _msg ) = 0;

    /// Pure virtual -> implemented in the subclasses
    SCENE2D_API virtual void doSwap() = 0;

    /// Pure virtual -> implemented in the subclasses
    SCENE2D_API virtual void doStop() = 0;

    /// Get a pair of doubles (a point), two axis, and convert the pair of doubles values from adaptor
    //  coordinates to scene coordinates
    SCENE2D_API Point2DType mapAdaptorToScene(Point2DType _xy, ::scene2D::data::Axis::sptr _xAxis, ::scene2D::data::Axis::sptr _yAxis);

    /// Get a pair of doubles (a point), two axis, and convert the pair of doubles values from scene
    //  coordinates to adaptor coordinates
    SCENE2D_API Point2DType mapSceneToAdaptor(Point2DType _xy, ::scene2D::data::Axis::sptr _xAxis, ::scene2D::data::Axis::sptr _yAxis);

    /// Return the ratio between view's initial size and its current size
    SCENE2D_API ViewSizeRatio getViewSizeRatio();

    /// Return the ratio between viewport's initial size and its current size
    SCENE2D_API ViewportSizeRatio getViewportSizeRatio();

    /// Initialize the source values used for computing view's size ratio.
    SCENE2D_API void initializeViewSize();

    /// Initialize the source values used for computing viewport's size ratio.
    SCENE2D_API void initializeViewportSize();

    SCENE2D_API Scene2DRatio getRatio();

    /// The x Axis.
    SPTR(::scene2D::data::Axis) m_xAxis;

    /// The y Axis.
    SPTR(::scene2D::data::Axis) m_yAxis;

    /// The adaptor zValue (depth within the scene). The adaptor with the highest zValue is displayed on top of all adaptors.
    float m_zValue;

    /// Opacity of the adaptor. Default value set to 1 (opaque).
    float m_opacity;

    // Initial size of the widget (view). The goal of keeping a reference on the initial size is to
    // avoid unwanted scaling onto some objects (such as transfer function points, histogram cursor,
    // etc) when a resize event is caught.
    ViewSizeRatio m_viewInitialSize;

    // Initial size of the viewport. The goal of keeping a reference on the initial size of the
    // viewport is the same as preceding.
    ViewportSizeRatio m_viewportInitialSize;


private:

    /// The render that manage the IAdaptor.
    ::scene2D::Render::wptr m_scene2DRender;

};


} // namespace adaptor
} // namespace scene2D


#endif // _SCENE2D_ADAPTOR_IADAPTOR_HPP_

