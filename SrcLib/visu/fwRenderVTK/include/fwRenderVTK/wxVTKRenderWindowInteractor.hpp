/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __LIBFWRENDERVTK_VTKRENDERWINDOWINTERACTOR_H
#define __LIBFWRENDERVTK_VTKRENDERWINDOWINTERACTOR_H

#include <boost/function.hpp>
#include <map>

#include <vtkinria3d/wxVTKRenderWindowInteractor.h>

#include "fwCore/HiResTimer.hpp"
#include "fwWX/Timer.hpp"

#include "fwRenderVTK/config.hpp"


namespace fwRenderVTK {

class  FWRENDERVTK_CLASS_API inriaVTKRenderWindowInteractor : public ::wxVTKRenderWindowInteractor
{

public :

//#ifdef __MACOSX__
#ifndef _WIN32
      FWRENDERVTK_API virtual void PrintSelf(ostream& os, vtkIndent indent) { this->::wxVTKRenderWindowInteractor::PrintSelf(os, indent); }
      FWRENDERVTK_API virtual void Render(){ this->::wxVTKRenderWindowInteractor::Render(); }

      FWRENDERVTK_API virtual int InternalCreateTimer(int a, int b, unsigned long c) { return this->::wxVTKRenderWindowInteractor::InternalCreateTimer(a,b,c); }
      FWRENDERVTK_API virtual int InternalDestroyTimer(int a) { return this->::wxVTKRenderWindowInteractor::InternalDestroyTimer(a); }
      FWRENDERVTK_API virtual int DestroyTimer() { return this->::wxVTKRenderWindowInteractor::DestroyTimer(); }
      FWRENDERVTK_API virtual void UpdateSize(int a, int b);// { this->::wxVTKRenderWindowInteractor::UpdateSize(a,b); }
      FWRENDERVTK_API virtual void Initialize() { this->::wxVTKRenderWindowInteractor::Initialize(); }
      FWRENDERVTK_API virtual void Enable() { this->::wxVTKRenderWindowInteractor::Enable(); }
      FWRENDERVTK_API virtual void Disable() { this->::wxVTKRenderWindowInteractor::Disable(); }
      FWRENDERVTK_API virtual void Start() { this->::wxVTKRenderWindowInteractor::Start(); }
      FWRENDERVTK_API virtual void CollectRevisions(std::basic_ostream<char, std::char_traits<char> >& a){ this->::wxVTKRenderWindowInteractor::CollectRevisions(a); }
      FWRENDERVTK_API virtual int CreateTimer(int a) { return this->::wxVTKRenderWindowInteractor::CreateTimer(a); }
#endif

      FWRENDERVTK_API inriaVTKRenderWindowInteractor();
      FWRENDERVTK_API inriaVTKRenderWindowInteractor(wxWindow *parent,
              wxWindowID id,
              const wxPoint &pos = wxDefaultPosition,
              const wxSize &size = wxDefaultSize,
              long style = wxWANTS_CHARS | wxNO_FULL_REPAINT_ON_RESIZE,
              const wxString &name = wxPanelNameStr);
};


/*
 * @brief composant wx-vtk avanc�
 *
 *  la classe fwWxVTKRenderWindowInteractor de fwRenderVTK enrobe celle de vtkinria3d et permet :
 *   * un comportement normal (utilisation transparente de vtkinria3d::wxVTKRenderWindowInteractor)
 *   * un nombre de rendu par seconde limit� (un appel toutes les "delta" ms, mode Framerated)
 *   * un mode framerated avec un "rappel" : si depuis le dernier rendu demand� explicitement (c�d par un appel ext�rieur sur la classe � Render()) aucune autre demande n'est enregistr�e dans un temps delta, un rappel � Render() est automatiquement effectu� pour avoir l'assurance de la validit� de l'affichage (mode OneShot?)
 *   * un mode automatique, effectuant un rendu inconditionnellement toutes les "delta" ms (Mode Timed).
*/

class  FWRENDERVTK_CLASS_API fwWxVTKRenderWindowInteractor : public inriaVTKRenderWindowInteractor
{
public:
  // Description:
  // Instantiate the object.

  typedef enum
  {
  MODE_DIRECT = 0,
  MODE_FRAMERATED,
  MODE_ONESHOT,
  MODE_TIMED,
  MODE_MEANTIME,
  MODE_NONE,

  _MODE_LAST, //Not used
  } mode_t;

  FWRENDERVTK_API mode_t GetRenderMode();
  FWRENDERVTK_API void SetRenderMode(mode_t mode);
  FWRENDERVTK_API void SetRenderModeToDirect();
  FWRENDERVTK_API void SetRenderModeToFrameRated();
  FWRENDERVTK_API void SetRenderModeToOneShot();
  FWRENDERVTK_API void SetRenderModeToTimed();
  FWRENDERVTK_API void SetRenderModeToMeanTime();
  FWRENDERVTK_API void SetRenderModeToNone();

  FWRENDERVTK_API fwWxVTKRenderWindowInteractor();

  FWRENDERVTK_API fwWxVTKRenderWindowInteractor(wxWindow *parent,
          wxWindowID id,
          const wxPoint &pos = wxDefaultPosition,
          const wxSize &size = wxDefaultSize,
          long style = wxWANTS_CHARS | wxNO_FULL_REPAINT_ON_RESIZE,
          const wxString &name = wxPanelNameStr);

  FWRENDERVTK_API static fwWxVTKRenderWindowInteractor *New();
#ifdef __MACOSX__
  FWRENDERVTK_API virtual void PrintSelf(ostream& os, vtkIndent indent);
#else
  FWRENDERVTK_API virtual void PrintSelf(ostream& os, vtkIndent indent);
#endif

  FWRENDERVTK_API virtual void Render();

  FWRENDERVTK_API void setFramerate(double framerate);
  FWRENDERVTK_API void setDelta(double delta);
  FWRENDERVTK_API double getDelta();


  FWRENDERVTK_API void RenderDirect();

  FWRENDERVTK_API void ResetHandle();

  FWRENDERVTK_API static bool isDepthPeelingSupported(vtkRenderWindow *renwin);

protected:

  ~fwWxVTKRenderWindowInteractor();

  void InitializeRenderModes();

  void RenderFirst();
  void RenderFrameRated();
  void RenderOneShot();
  void RenderPeriodic();
  void RenderMeanTime();
  void RenderNone();

  ::fwCore::HiResTimer timer;
  double m_delta;

  double m_meanTime;

  ::fwWX::Timer<> m_triggerTimer;

  ::boost::function0<void> m_renderFunction;
  ::boost::function0<void> m_renderModes[_MODE_LAST] ;
  mode_t m_currentMode;

private:
  fwWxVTKRenderWindowInteractor(const fwWxVTKRenderWindowInteractor&);  //Not implemented
  void operator=(const fwWxVTKRenderWindowInteractor&);  //Not implemented
};

} // fwRenderVTK namespace

#endif //__LIBFWRENDERVTK_VTKRENDERWINDOWINTERACTOR_H
