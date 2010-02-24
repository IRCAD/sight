/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include <fwCore/base.hpp>

#include <vtkAssemblyPath.h>
#include <vtkCallbackCommand.h>
#include <vtkCamera.h>
#include <vtkCellPicker.h>
#include <vtkDoubleArray.h>
#include <vtkMath.h>
#include <vtkObjectFactory.h>
#include <vtkPlane.h>
#include <vtkPlanes.h>
#include <vtkPoints.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>

#include <vtkOpenGL.h>
#include <vtkOpenGLProperty.h>
#include <vtkOpenGLExtensionManager.h>

#include <boost/bind.hpp>

#include "fwRenderVTK/wxVTKRenderWindowInteractor.hpp"

namespace fwRenderVTK {

inriaVTKRenderWindowInteractor::inriaVTKRenderWindowInteractor()
: ::wxVTKRenderWindowInteractor()
{}

inriaVTKRenderWindowInteractor::inriaVTKRenderWindowInteractor(wxWindow *parent,
        wxWindowID id,
        const wxPoint &pos,
        const wxSize &size,
        long style,
        const wxString &name)
    : ::wxVTKRenderWindowInteractor(parent, id, pos, size, style, name) // vtkinria3d's one
{ }

#ifndef _WIN32
void inriaVTKRenderWindowInteractor::UpdateSize(int x, int y)
{
  if( RenderWindow )
  {
    // if the size changed tell render window
    if ( x != Size[0] || y != Size[1] )
    {
      // adjust our (vtkRenderWindowInteractor size)
      Size[0] = x;
      Size[1] = y;
      // and our RenderWindow's size
      // WARNING !! Crash on linux64 if x or y == 0 !! (wxWidget2.9, vtk5.4) - X Error
      if(x > 0 && y > 0)
      {
          RenderWindow->SetSize(x, y);
      }
    }
  }
}
#endif
// ======================================================================

fwWxVTKRenderWindowInteractor * fwWxVTKRenderWindowInteractor::New()
{
  // we don't make use of the objectfactory, because we're not registered
  return new fwWxVTKRenderWindowInteractor;
}


fwWxVTKRenderWindowInteractor::fwWxVTKRenderWindowInteractor()
: inriaVTKRenderWindowInteractor()
{
    this->InitializeRenderModes();
}
//---------------------------------------------------------------------------
fwWxVTKRenderWindowInteractor::fwWxVTKRenderWindowInteractor(wxWindow *parent,
                                                         wxWindowID id,
                                                         const wxPoint &pos,
                                                         const wxSize &size,
                                                         long style,
                                                         const wxString &name)
  : inriaVTKRenderWindowInteractor(parent, id, pos, size, style, name)
{
#if ( SPYLOG_LEVEL <= 2 )
#ifndef _DEBUG
    vtkObject::GlobalWarningDisplayOff();
#endif
#endif
    this->InitializeRenderModes();
}

fwWxVTKRenderWindowInteractor::~fwWxVTKRenderWindowInteractor()
{
}

void fwWxVTKRenderWindowInteractor::InitializeRenderModes()
{
    m_delta = 1./50.;
    this->timer.reset(m_delta);

    m_renderModes[MODE_DIRECT]     = ::boost::bind(&fwWxVTKRenderWindowInteractor::RenderDirect     , this);
    m_renderModes[MODE_FRAMERATED] = ::boost::bind(&fwWxVTKRenderWindowInteractor::RenderFrameRated , this);
    m_renderModes[MODE_ONESHOT]    = ::boost::bind(&fwWxVTKRenderWindowInteractor::RenderOneShot    , this);
    m_renderModes[MODE_TIMED]      = ::boost::bind(&fwWxVTKRenderWindowInteractor::RenderPeriodic   , this);
    m_renderModes[MODE_MEANTIME]   = ::boost::bind(&fwWxVTKRenderWindowInteractor::RenderMeanTime   , this);
    m_renderModes[MODE_NONE]       = ::boost::bind(&fwWxVTKRenderWindowInteractor::RenderNone       , this);

    m_triggerTimer.setFunction(m_renderModes[MODE_DIRECT]);

//    this->SetRenderModeToDirect();
//    this->SetRenderModeToFrameRated();
    this->SetRenderModeToOneShot();
//    this->SetRenderModeToTimed();

}


  fwWxVTKRenderWindowInteractor::mode_t fwWxVTKRenderWindowInteractor::GetRenderMode()
  {
    return m_currentMode;
  }

  void fwWxVTKRenderWindowInteractor::SetRenderMode(fwWxVTKRenderWindowInteractor::mode_t mode)
  {
      SLM_TRACE_FUNC();
      assert(0 <= mode && mode < _MODE_LAST);
      m_currentMode = mode;
      m_renderFunction = ::boost::bind(&fwWxVTKRenderWindowInteractor::RenderFirst, this);
  }

  void fwWxVTKRenderWindowInteractor::SetRenderModeToDirect()
  {
      SLM_TRACE_FUNC();
      this->SetRenderMode(MODE_DIRECT);
  }

  void fwWxVTKRenderWindowInteractor::SetRenderModeToFrameRated()
  {
      SLM_TRACE_FUNC();
      this->SetRenderMode(MODE_FRAMERATED);
  }

  void fwWxVTKRenderWindowInteractor::SetRenderModeToOneShot()
  {
      SLM_TRACE_FUNC();
      this->SetRenderMode(MODE_ONESHOT);
  }

  void fwWxVTKRenderWindowInteractor::SetRenderModeToTimed()
  {
      SLM_TRACE_FUNC();
      this->SetRenderMode(MODE_TIMED);
  }

  void fwWxVTKRenderWindowInteractor::SetRenderModeToMeanTime()
    {
        SLM_TRACE_FUNC();
        this->SetRenderMode(MODE_MEANTIME);
    }

  void fwWxVTKRenderWindowInteractor::SetRenderModeToNone()
    {
        SLM_TRACE_FUNC();
        this->SetRenderMode(MODE_NONE);
    }

void fwWxVTKRenderWindowInteractor::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

void fwWxVTKRenderWindowInteractor::setFramerate(double framerate)
{
  this->setDelta(1./framerate);
}

void fwWxVTKRenderWindowInteractor::setDelta(double delta){
    m_delta = delta;

    m_triggerTimer.stop();
}

double fwWxVTKRenderWindowInteractor::getDelta(){
    return m_delta;
}

void fwWxVTKRenderWindowInteractor::Render()
{
//    SLM_TRACE_FUNC();
    m_renderFunction();
}

void fwWxVTKRenderWindowInteractor::RenderFirst()
{
    //SLM_TRACE_FUNC();
    if (this->GetParent()->IsShownOnScreen())
    {
        if (this->IsShownOnScreen())
        {
            this->RenderDirect();
            this->timer.start();
            this->m_meanTime = 0;
            m_renderFunction = m_renderModes[m_currentMode];
            if (m_currentMode == MODE_ONESHOT){
                m_triggerTimer.setRate(m_delta*1000);
                m_triggerTimer.setOneShot(true);
                m_triggerTimer.start();
            }
            else if (m_currentMode == MODE_TIMED){
                m_triggerTimer.setRate(m_delta*1000);
                m_triggerTimer.setOneShot(false);
                m_triggerTimer.start();
            }
            else {
                m_triggerTimer.stop();
            }
            Layout();
            Update();
        }
        this->GetParent()->Layout();
        this->GetParent()->Update();
    }
}

void fwWxVTKRenderWindowInteractor::RenderDirect()
{
//    SLM_TRACE_FUNC();
    this-> inriaVTKRenderWindowInteractor::Render();
}


void fwWxVTKRenderWindowInteractor::RenderFrameRated()
{
//    SLM_TRACE_FUNC();
    if ( this->timer.getElapsedTimeInSec() >= m_delta )
    {
//        SLM_TRACE("Render !");
#ifdef _WIN32
        // fix "refresh" bug in WIN32 with modal windows
        this->GetRenderWindow()->Render();
#else
        this->inriaVTKRenderWindowInteractor::Render();
#endif
        this->timer.reset();
    }
}

void fwWxVTKRenderWindowInteractor::RenderMeanTime()
{
//    SLM_TRACE_FUNC();
    double time = this->timer.getElapsedTimeInSec();

    if ( time >= m_meanTime)
    {
//        SLM_TRACE("Render !");
        this->inriaVTKRenderWindowInteractor::Render();
        this->timer.reset();
    }
    m_meanTime = (m_meanTime + time)/2;
}

void fwWxVTKRenderWindowInteractor::RenderOneShot()
{
//    SLM_TRACE_FUNC();
    if ( this->timer.getElapsedTimeInSec() > m_delta )
    {
//        SLM_TRACE("Render !");
        this-> inriaVTKRenderWindowInteractor::Render();
        this->timer.reset();
        m_triggerTimer.start();
    }
}

void fwWxVTKRenderWindowInteractor::RenderPeriodic()
{
//    SLM_TRACE_FUNC();
    SLM_FATAL("Not implemented");

}

void fwWxVTKRenderWindowInteractor::RenderNone()
{
    SLM_TRACE_FUNC();
}

void fwWxVTKRenderWindowInteractor::ResetHandle()
{
#ifdef _WIN32
    SLM_TRACE_FUNC();
  //must always be here
//  wxPaintDC pDC(this);
//  Handle
  //do it here rather than in the cstor: this is safer.
//  if(!Handle)
//  {
    long Handle = GetHandleHack();
    RenderWindow->SetWindowId(reinterpret_cast<void *>(Handle));
    RenderWindow->SetParentId(reinterpret_cast<void *>(this->GetParent()->GetHWND()));
//  }
  // get vtk to render to the wxWindows
  Render();
#endif
}

bool fwWxVTKRenderWindowInteractor::isDepthPeelingSupported( vtkRenderWindow *renwin)
{

//    http://vtk.org/Wiki/VTK/Depth_Peeling
//    Required OpenGL extensions
//
//      * GL_ARB_depth_texture or OpenGL>=1.4
//      * GL_ARB_shadow or OpenGL>=1.4
//      * GL_EXT_shadow_funcs or OpenGL>=1.5
//      * GL_ARB_vertex_shader or OpenGL>=2.0
//      * GL_ARB_fragment_shader or OpenGL>=2.0
//      * GL_ARB_shader_objects or OpenGL>=2.0
//      * GL_ARB_occlusion_query or OpenGL>=1.5
//      * GL_ARB_multitexture or OpenGL>=1.3
//      * GL_ARB_texture_rectangle
//      * GL_SGIS_texture_edge_clamp or GL_EXT_texture_edge_clamp or OpenGL>=1.2

    vtkOpenGLExtensionManager *extensions = vtkOpenGLExtensionManager::New();
    extensions->SetRenderWindow(renwin);


    int supports_GL_1_3=extensions->ExtensionSupported("GL_VERSION_1_3");
    int supports_GL_1_4=extensions->ExtensionSupported("GL_VERSION_1_4");
    int supports_GL_1_5=extensions->ExtensionSupported("GL_VERSION_1_5");
    int supports_GL_2_0=extensions->ExtensionSupported("GL_VERSION_2_0");

    int supports_vertex_shader;
    int supports_fragment_shader;
    int supports_shader_objects;
    if(supports_GL_2_0)
    {
        supports_vertex_shader=1;
        supports_fragment_shader=1;
        supports_shader_objects=1;
    }
    else
    {
        supports_vertex_shader=extensions->ExtensionSupported("GL_ARB_vertex_shader");
        supports_fragment_shader=extensions->ExtensionSupported("GL_ARB_fragment_shader");
        supports_shader_objects=extensions->ExtensionSupported("GL_ARB_shader_objects");
    }
    int supports_multitexture=supports_GL_1_3 || extensions->ExtensionSupported("GL_ARB_multitexture");
    int supports_occlusion_query;
    int supports_shadow_funcs;
    if(supports_GL_1_5)
    {
        supports_occlusion_query=1;
        supports_shadow_funcs=1;
    }
    else
    {
        supports_occlusion_query=extensions->ExtensionSupported("GL_ARB_occlusion_query");
        supports_shadow_funcs=extensions->ExtensionSupported("GL_EXT_shadow_funcs");
    }

    int supports_depth_texture;
    int supports_shadow;
    int supports_blend_func_separate;
    if(supports_GL_1_4)
    {
        supports_depth_texture=1;
        supports_blend_func_separate=1;
        supports_shadow=1;
    }
    else
    {
        supports_depth_texture=extensions->ExtensionSupported("GL_ARB_depth_texture");
        supports_shadow=extensions->ExtensionSupported("GL_ARB_shadow");
        supports_blend_func_separate=extensions->ExtensionSupported("GL_EXT_blend_func_separate");
    }

    int supports_GL_ARB_texture_rectangle=extensions->ExtensionSupported("GL_ARB_texture_rectangle");

    // spec claims it is GL_SGIS_texture_edge_clamp, reality shows it is
    // GL_EXT_texture_edge_clamp on Nvidia.
    // part of OpenGL 1.2 core
    // there is no new function with this extension, we don't need to load
    // it.
    int supports_edge_clamp=extensions->ExtensionSupported("GL_VERSION_1_2");
    if(!supports_edge_clamp)
    {
        supports_edge_clamp=extensions->ExtensionSupported("GL_SGIS_texture_edge_clamp");
        if(!supports_edge_clamp)
        {
            // nvidia cards.
            supports_edge_clamp=extensions->ExtensionSupported("GL_EXT_texture_edge_clamp");
        }
    }

    extensions->Delete();

    //TODO FIXME bad test
    //GLint alphaBits;
    //glGetIntegerv(GL_ALPHA_BITS, &alphaBits);
    //int supportsAtLeast8AlphaBits=alphaBits>=8;


    bool depthPeelingIsSupported  = supports_depth_texture &&
        supports_shadow &&
        supports_blend_func_separate &&
        supports_shadow_funcs &&
        supports_vertex_shader &&
        supports_fragment_shader &&
        supports_shader_objects &&
        supports_occlusion_query &&
        supports_multitexture &&
        supports_GL_ARB_texture_rectangle &&
        supports_edge_clamp ;//&&   supportsAtLeast8AlphaBits;

    SLM_TRACE_IF("Depth peeling is supported",depthPeelingIsSupported);
    SLM_TRACE_IF("Depth peeling isn't supported",!depthPeelingIsSupported);

    return depthPeelingIsSupported;
}

} // fwRenderVTK namespace

