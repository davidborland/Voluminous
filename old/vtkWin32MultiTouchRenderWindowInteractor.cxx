///////////////////////////////////////////////////////////////////////////////////////////////
//
// Name:        vtkWin32MultiTouchRenderWindowInteractor.cxx
//
// Author:      David Borland
//
// Description: A vtkWin32RenderWindowInteractor that processes Jason Coposky's multi-touch 
//              events in addition to regular window events
//
///////////////////////////////////////////////////////////////////////////////////////////////

#include "vtkWin32MultiTouchRenderWindowInteractor.h"

#include "vtkInteractorStyleMultiTouch.h"
#include "vtkObjectFactory.h"

#ifndef VTK_IMPLEMENT_MESA_CXX
vtkCxxRevisionMacro(vtkWin32MultiTouchRenderWindowInteractor, "$Revision: 1.1 $");
vtkStandardNewMacro(vtkWin32MultiTouchRenderWindowInteractor);
#endif

//----------------------------------------------------------------------------
vtkWin32MultiTouchRenderWindowInteractor::vtkWin32MultiTouchRenderWindowInteractor() 
{
  this->MultiTouchInterface = vtkMultiTouchInterface::New();

  this->SetInteractorStyle(vtkInteractorStyleMultiTouch::New());
  this->InteractorStyle->Delete();
}

//----------------------------------------------------------------------
vtkWin32MultiTouchRenderWindowInteractor::~vtkWin32MultiTouchRenderWindowInteractor()
{
  if (this->MultiTouchInterface != NULL) 
    {
    this->MultiTouchInterface->Delete();
    }
}

//----------------------------------------------------------------------
void vtkWin32MultiTouchRenderWindowInteractor::Initialize() 
{
  this->Superclass::Initialize();

  this->MultiTouchInterface->Initialize();
}

//----------------------------------------------------------------------
void vtkWin32MultiTouchRenderWindowInteractor::Start() 
{
  // Let the compositing handle the event loop if it wants to.
  if (this->HasObserver(vtkCommand::StartEvent) && !this->HandleEventLoop)
    {
    this->InvokeEvent(vtkCommand::StartEvent,NULL);
    return;
    }

  // No need to do anything if this is a 'mapped' interactor
  if (!this->Enabled || !this->InstallMessageProc)
    {
    return;
    }

  this->StartedMessageLoop = 1;

  MSG msg;
  while (1)
    { 
    PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
    TranslateMessage(&msg);
    DispatchMessage(&msg);

    if (this->MultiTouchInterface->ProcessSocket())
      {
      if (this->MultiTouchInterface->GetGestureName() == "one_touch")
        {
        this->InvokeEvent(vtkMultiTouchInterface::MultiTouchOneTouchEvent,NULL);
        }
      else if (this->MultiTouchInterface->GetGestureName() == "one_drag")
        {
        this->InvokeEvent(vtkMultiTouchInterface::MultiTouchOneDragEvent,NULL);
        }
      else if (this->MultiTouchInterface->GetGestureName() == "two_touch")
        {
        this->InvokeEvent(vtkMultiTouchInterface::MultiTouchTwoTouchEvent,NULL);
        }
      else if (this->MultiTouchInterface->GetGestureName() == "two_drag")
        {
        this->InvokeEvent(vtkMultiTouchInterface::MultiTouchTwoDragEvent,NULL);
        }
      else if (this->MultiTouchInterface->GetGestureName() == "three_touch")
        {
        this->InvokeEvent(vtkMultiTouchInterface::MultiTouchThreeTouchEvent,NULL);
        }
      else if (this->MultiTouchInterface->GetGestureName() == "three_drag")
        {
        this->InvokeEvent(vtkMultiTouchInterface::MultiTouchThreeDragEvent,NULL);
        }
      else if (this->MultiTouchInterface->GetGestureName() == "four_touch")
        {
        this->InvokeEvent(vtkMultiTouchInterface::MultiTouchFourTouchEvent,NULL);
        }
      else if (this->MultiTouchInterface->GetGestureName() == "four_drag")
        {
        this->InvokeEvent(vtkMultiTouchInterface::MultiTouchFourDragEvent,NULL);
        }
      else if (this->MultiTouchInterface->GetGestureName() == "five_touch")
        {
        this->InvokeEvent(vtkMultiTouchInterface::MultiTouchFiveTouchEvent,NULL);
        }
      else if (this->MultiTouchInterface->GetGestureName() == "five_drag")
        {
        this->InvokeEvent(vtkMultiTouchInterface::MultiTouchFiveDragEvent,NULL);
        }
      else if (this->MultiTouchInterface->GetGestureName() == "six_touch")
        {
        this->InvokeEvent(vtkMultiTouchInterface::MultiTouchSixTouchEvent,NULL);
        }
      else if (this->MultiTouchInterface->GetGestureName() == "six_drag")
        {
        this->InvokeEvent(vtkMultiTouchInterface::MultiTouchSixDragEvent,NULL);
        }
      else if (this->MultiTouchInterface->GetGestureName() == "zoom")
        {
        this->InvokeEvent(vtkMultiTouchInterface::MultiTouchZoomEvent,NULL);
        }
      else if (this->MultiTouchInterface->GetGestureName() == "translate_x")
        {
        this->InvokeEvent(vtkMultiTouchInterface::MultiTouchTranslateXEvent,NULL);
        }
      else if (this->MultiTouchInterface->GetGestureName() == "translate_y")
        {
        this->InvokeEvent(vtkMultiTouchInterface::MultiTouchTranslateYEvent,NULL);
        }
      else if (this->MultiTouchInterface->GetGestureName() == "translate_z")
        {
        this->InvokeEvent(vtkMultiTouchInterface::MultiTouchTranslateZEvent,NULL);
        }
      else if (this->MultiTouchInterface->GetGestureName() == "about_X_axis")
        {
        this->InvokeEvent(vtkMultiTouchInterface::MultiTouchRotateYEvent,NULL);
        }
      else if (this->MultiTouchInterface->GetGestureName() == "about_Y_axis")
        {
        this->InvokeEvent(vtkMultiTouchInterface::MultiTouchRotateYEvent,NULL);
        }
      else if (this->MultiTouchInterface->GetGestureName() == "about_Z_axis")
        {
        this->InvokeEvent(vtkMultiTouchInterface::MultiTouchRotateZEvent,NULL);
        }
      else if (this->MultiTouchInterface->GetGestureName() == "release")
        {
        this->InvokeEvent(vtkMultiTouchInterface::MultiTouchReleaseEvent,NULL);
        }
      }
    }
}


//----------------------------------------------------------------------------
void vtkWin32MultiTouchRenderWindowInteractor::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  this->MultiTouchInterface->PrintSelf(os,indent);
}