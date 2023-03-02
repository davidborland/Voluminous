///////////////////////////////////////////////////////////////////////////////////////////////
//
// Name:        vtkInteractorStyleMultiTouch.cxx
//
// Author:      David Borland
//
// Description: Provides an event-driven interface to the vtkMultiTouchInterface
//
///////////////////////////////////////////////////////////////////////////////////////////////


#include "vtkInteractorStyleMultiTouch.h"

#include "vtkCamera.h"
#include "vtkCallbackCommand.h"
#include "vtkEventForwarderCommand.h"
#include "vtkMultiTouchInterface.h"
#include "vtkObjectFactory.h"
#include "vtkRenderer.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkWin32MultiTouchRenderWindowInteractor.h"

vtkCxxRevisionMacro(vtkInteractorStyleMultiTouch, "$Revision: 1.3 $");
vtkStandardNewMacro(vtkInteractorStyleMultiTouch);

//----------------------------------------------------------------------------
vtkInteractorStyleMultiTouch::vtkInteractorStyleMultiTouch()
{
    this->MultiTouchCallbackCommand = vtkCallbackCommand::New();
    this->MultiTouchCallbackCommand->SetClientData(this);
    this->MultiTouchCallbackCommand->SetCallback(vtkInteractorStyleMultiTouch::ProcessMultiTouch);
}

//----------------------------------------------------------------------------
vtkInteractorStyleMultiTouch::~vtkInteractorStyleMultiTouch()
{
}

//----------------------------------------------------------------------------
void vtkInteractorStyleMultiTouch::SetInteractor(vtkRenderWindowInteractor *i)
{
  this->Superclass::SetInteractor(i);

  if (this->Interactor)
    {
    // Touch and Drag
    this->Interactor->AddObserver(vtkMultiTouchInterface::MultiTouchOneTouchEvent, 
                                  this->MultiTouchCallbackCommand, 
                                  this->Priority);

    this->Interactor->AddObserver(vtkMultiTouchInterface::MultiTouchOneDragEvent, 
                                  this->MultiTouchCallbackCommand, 
                                  this->Priority);

    this->Interactor->AddObserver(vtkMultiTouchInterface::MultiTouchTwoTouchEvent, 
                                  this->MultiTouchCallbackCommand, 
                                  this->Priority);

    this->Interactor->AddObserver(vtkMultiTouchInterface::MultiTouchTwoDragEvent, 
                                  this->MultiTouchCallbackCommand, 
                                  this->Priority);

    this->Interactor->AddObserver(vtkMultiTouchInterface::MultiTouchThreeTouchEvent, 
                                  this->MultiTouchCallbackCommand, 
                                  this->Priority);

    this->Interactor->AddObserver(vtkMultiTouchInterface::MultiTouchThreeDragEvent, 
                                  this->MultiTouchCallbackCommand, 
                                  this->Priority);

    this->Interactor->AddObserver(vtkMultiTouchInterface::MultiTouchFourTouchEvent, 
                                  this->MultiTouchCallbackCommand, 
                                  this->Priority);

    this->Interactor->AddObserver(vtkMultiTouchInterface::MultiTouchFourDragEvent, 
                                  this->MultiTouchCallbackCommand, 
                                  this->Priority);

    this->Interactor->AddObserver(vtkMultiTouchInterface::MultiTouchFiveTouchEvent, 
                                  this->MultiTouchCallbackCommand, 
                                  this->Priority);

    this->Interactor->AddObserver(vtkMultiTouchInterface::MultiTouchFiveDragEvent, 
                                  this->MultiTouchCallbackCommand, 
                                  this->Priority);

    this->Interactor->AddObserver(vtkMultiTouchInterface::MultiTouchSixTouchEvent, 
                                  this->MultiTouchCallbackCommand, 
                                  this->Priority);

    this->Interactor->AddObserver(vtkMultiTouchInterface::MultiTouchSixDragEvent, 
                                  this->MultiTouchCallbackCommand, 
                                  this->Priority);

    // Zoom
    this->Interactor->AddObserver(vtkMultiTouchInterface::MultiTouchZoomEvent, 
                                  this->MultiTouchCallbackCommand, 
                                  this->Priority);

    // Translate
    this->Interactor->AddObserver(vtkMultiTouchInterface::MultiTouchTranslateXEvent, 
                                  this->MultiTouchCallbackCommand, 
                                  this->Priority);

    this->Interactor->AddObserver(vtkMultiTouchInterface::MultiTouchTranslateYEvent, 
                                  this->MultiTouchCallbackCommand, 
                                  this->Priority);

    this->Interactor->AddObserver(vtkMultiTouchInterface::MultiTouchTranslateZEvent, 
                                  this->MultiTouchCallbackCommand, 
                                  this->Priority);

    // Rotate
    this->Interactor->AddObserver(vtkMultiTouchInterface::MultiTouchRotateXEvent, 
                                  this->MultiTouchCallbackCommand, 
                                  this->Priority);

    this->Interactor->AddObserver(vtkMultiTouchInterface::MultiTouchRotateYEvent, 
                                  this->MultiTouchCallbackCommand, 
                                  this->Priority);

    this->Interactor->AddObserver(vtkMultiTouchInterface::MultiTouchRotateZEvent, 
                                  this->MultiTouchCallbackCommand, 
                                  this->Priority);

    // Release
    this->Interactor->AddObserver(vtkMultiTouchInterface::MultiTouchReleaseEvent, 
                                  this->MultiTouchCallbackCommand, 
                                  this->Priority);

    // Set the pointer to the multi-touch interface here so we don't need to do it every event
#ifdef WIN32
    this->MultiTouchInterface
      = reinterpret_cast<vtkWin32MultiTouchRenderWindowInteractor *>(this->Interactor)->GetMultiTouchInterface();
#else
    vtkErrorMacro(<<"Multi-touch only implemented on 32-bit Windows");
    return;
#endif
    }
}

//----------------------------------------------------------------------------
void vtkInteractorStyleMultiTouch::OnMultiTouchOneDrag()
{
  this->FindPokedRenderer(0, 0);

  if (this->CurrentRenderer == NULL)
    {
    return;
    }

  vtkstd::vector<TouchPoint> touches = this->MultiTouchInterface->GetTouchPoints();

  if (touches.size() < 1) return;

  double rotateScale = 500.0;
  double dx = touches[0].Direction[0] * rotateScale;
  double dy = touches[0].Direction[1] * rotateScale; 

  vtkCamera *camera = this->CurrentRenderer->GetActiveCamera();
  camera->Azimuth(-dx);
  camera->Elevation(dy);
  camera->OrthogonalizeViewUp();

  if (this->AutoAdjustCameraClippingRange)
    {
    this->CurrentRenderer->ResetCameraClippingRange();
    }

  if (this->Interactor->GetLightFollowCamera()) 
    {
    this->CurrentRenderer->UpdateLightsGeometryToFollowCamera();
    }

  this->Interactor->Render();
}

//----------------------------------------------------------------------------
void vtkInteractorStyleMultiTouch::OnMultiTouchZoom()
{
  this->FindPokedRenderer(0, 0);

  if (this->CurrentRenderer == NULL)
    {
    return;
    }

  vtkstd::vector<TouchPoint> touches = this->MultiTouchInterface->GetTouchPoints();

  if (touches.size() < 2) return;

  double zoomAmount = sqrt(touches[0].Direction[0] * touches[0].Direction[0] +
                           touches[0].Direction[1] * touches[0].Direction[1]);

  // Find right-most touch
  if (touches[0].Location[0] > touches[1].Location[0])
    {
    // Touch 0 is right-most
    if (touches[0].Direction[0] < 0)
      {
      // Right-most touch moving left == zoom out
      zoomAmount = -zoomAmount;
      }
    }
  else
    {
    // Touch 1 is right-most
    if (touches[1].Direction[0] < 0)
      // Right-most touch moving left == zoom out
      {
      zoomAmount = -zoomAmount;
      }
    }

  double zoomScale = 20.0;
  zoomAmount = pow(1.1, zoomAmount * zoomScale);

  // Zoom the camera
  vtkCamera *camera = this->CurrentRenderer->GetActiveCamera();
  if (camera->GetParallelProjection())
    {
    camera->SetParallelScale(camera->GetParallelScale() / zoomAmount);
    }
  else
    {
    camera->Dolly(zoomAmount);
    if (this->AutoAdjustCameraClippingRange)
      {
      this->CurrentRenderer->ResetCameraClippingRange();
      }
    }
  
  if (this->Interactor->GetLightFollowCamera()) 
    {
    this->CurrentRenderer->UpdateLightsGeometryToFollowCamera();
    }
  
  this->Interactor->Render();
}

//----------------------------------------------------------------------------
void vtkInteractorStyleMultiTouch::OnMultiTouchTranslateX()
{
  this->FindPokedRenderer(0, 0);

  if (this->CurrentRenderer == NULL)
    {
    return;
    }

  vtkstd::vector<TouchPoint> touches = this->MultiTouchInterface->GetTouchPoints();

  int i;
  for (i = 0; i < (int)touches.size(); i++) 
    {
    if (touches[i].MoveLocation != 0) break;
    }
  if (i >= (int)touches.size()) return;

  double translateScale = 1000.0;
  double x = touches[i].Location[0];
  double y = touches[i].Location[1];
  double dx = touches[i].Direction[1] * translateScale;
  double dy = 0.0; 

  double viewFocus[4], focalDepth, viewPoint[3];
  double newPickPoint[4], oldPickPoint[4], motionVector[3];

  vtkRenderWindowInteractor *rwi = this->Interactor;

  vtkCamera *camera = this->CurrentRenderer->GetActiveCamera();
  camera->GetFocalPoint(viewFocus);
  this->ComputeWorldToDisplay(viewFocus[0], viewFocus[1], viewFocus[2], 
                              viewFocus);
  focalDepth = viewFocus[2];

  this->ComputeDisplayToWorld(x, y,
                              focalDepth, 
                              newPickPoint);

  this->ComputeDisplayToWorld(x + dx, y + dy,
                              focalDepth, 
                              oldPickPoint);
  
  // Camera motion is reversed
  motionVector[0] = oldPickPoint[0] - newPickPoint[0];
  motionVector[1] = oldPickPoint[1] - newPickPoint[1];
  motionVector[2] = oldPickPoint[2] - newPickPoint[2];
  
  camera->GetFocalPoint(viewFocus);
  camera->GetPosition(viewPoint);
  camera->SetFocalPoint(motionVector[0] + viewFocus[0],
                        motionVector[1] + viewFocus[1],
                        motionVector[2] + viewFocus[2]);

  camera->SetPosition(motionVector[0] + viewPoint[0],
                      motionVector[1] + viewPoint[1],
                      motionVector[2] + viewPoint[2]);
      
  if (rwi->GetLightFollowCamera()) 
    {
    this->CurrentRenderer->UpdateLightsGeometryToFollowCamera();
    }
    
  rwi->Render();
}

//----------------------------------------------------------------------------
void vtkInteractorStyleMultiTouch::OnMultiTouchTranslateY()
{
  this->FindPokedRenderer(0, 0);

  if (this->CurrentRenderer == NULL)
    {
    return;
    }

  vtkstd::vector<TouchPoint> touches = this->MultiTouchInterface->GetTouchPoints();

  int i;
  for (i = 0; i < (int)touches.size(); i++) 
    {
    if (touches[i].MoveLocation != 0) break;
    }
  if (i >= (int)touches.size()) return;

  double translateScale = 1000.0;
  double x = touches[i].Location[0];
  double y = touches[i].Location[1];
  double dx = 0.0;
  double dy = touches[i].Direction[1] * translateScale; 

  double viewFocus[4], focalDepth, viewPoint[3];
  double newPickPoint[4], oldPickPoint[4], motionVector[3];

  vtkRenderWindowInteractor *rwi = this->Interactor;

  vtkCamera *camera = this->CurrentRenderer->GetActiveCamera();
  camera->GetFocalPoint(viewFocus);
  this->ComputeWorldToDisplay(viewFocus[0], viewFocus[1], viewFocus[2], 
                              viewFocus);
  focalDepth = viewFocus[2];

  this->ComputeDisplayToWorld(x, y,
                              focalDepth, 
                              newPickPoint);

  this->ComputeDisplayToWorld(x + dx, y + dy,
                              focalDepth, 
                              oldPickPoint);
  
  // Camera motion is reversed
  motionVector[0] = oldPickPoint[0] - newPickPoint[0];
  motionVector[1] = oldPickPoint[1] - newPickPoint[1];
  motionVector[2] = oldPickPoint[2] - newPickPoint[2];
  
  camera->GetFocalPoint(viewFocus);
  camera->GetPosition(viewPoint);
  camera->SetFocalPoint(motionVector[0] + viewFocus[0],
                        motionVector[1] + viewFocus[1],
                        motionVector[2] + viewFocus[2]);

  camera->SetPosition(motionVector[0] + viewPoint[0],
                      motionVector[1] + viewPoint[1],
                      motionVector[2] + viewPoint[2]);
      
  if (rwi->GetLightFollowCamera()) 
    {
    this->CurrentRenderer->UpdateLightsGeometryToFollowCamera();
    }
    
  rwi->Render();
}

//----------------------------------------------------------------------------
void vtkInteractorStyleMultiTouch::OnMultiTouchRotateX()
{
  this->FindPokedRenderer(0, 0);

  if (this->CurrentRenderer == NULL)
    {
    return;
    }

  vtkstd::vector<TouchPoint> touches = this->MultiTouchInterface->GetTouchPoints();

  int i;
  for (i = 0; i < (int)touches.size(); i++) 
    {
    if (touches[i].MoveLocation != 0) break;
    }
  if (i >= (int)touches.size()) return;

  double rotateScale = 500.0;
  double dy = touches[i].Direction[1] * rotateScale; 

  vtkCamera *camera = this->CurrentRenderer->GetActiveCamera();
  camera->Elevation(dy);
  camera->OrthogonalizeViewUp();

  if (this->AutoAdjustCameraClippingRange)
    {
    this->CurrentRenderer->ResetCameraClippingRange();
    }

  if (this->Interactor->GetLightFollowCamera()) 
    {
    this->CurrentRenderer->UpdateLightsGeometryToFollowCamera();
    }

  this->Interactor->Render();
}

//----------------------------------------------------------------------------
void vtkInteractorStyleMultiTouch::OnMultiTouchRotateY()
{
  this->FindPokedRenderer(0, 0);

  if (this->CurrentRenderer == NULL)
    {
    return;
    }

  vtkstd::vector<TouchPoint> touches = this->MultiTouchInterface->GetTouchPoints();

  int i;
  for (i = 0; i < (int)touches.size(); i++) 
    {
    if (touches[i].MoveLocation != 0) break;
    }
  if (i >= (int)touches.size()) return;

  double rotateScale = 500.0;
  double dy = touches[i].Direction[1] * rotateScale; 

  vtkCamera *camera = this->CurrentRenderer->GetActiveCamera();
  camera->Azimuth(-dy);
  camera->OrthogonalizeViewUp();

  if (this->AutoAdjustCameraClippingRange)
    {
    this->CurrentRenderer->ResetCameraClippingRange();
    }

  if (this->Interactor->GetLightFollowCamera()) 
    {
    this->CurrentRenderer->UpdateLightsGeometryToFollowCamera();
    }

  this->Interactor->Render();
}

//----------------------------------------------------------------------------
void vtkInteractorStyleMultiTouch::OnMultiTouchRotateZ()
{
  this->FindPokedRenderer(0, 0);

  if (this->CurrentRenderer == NULL)
    {
    return;
    }

  vtkstd::vector<TouchPoint> touches = this->MultiTouchInterface->GetTouchPoints();

  int i;
  for (i = 0; i < (int)touches.size(); i++) 
    {
    if (touches[i].MoveLocation != 0) break;
    }
  if (i >= (int)touches.size()) return;

  double rotateScale = 500.0;
  double dy = touches[i].Direction[1] * 500.0; 

  vtkCamera *camera = this->CurrentRenderer->GetActiveCamera();
  camera->Roll(-dy);
  camera->OrthogonalizeViewUp();

  if (this->AutoAdjustCameraClippingRange)
    {
    this->CurrentRenderer->ResetCameraClippingRange();
    }

  if (this->Interactor->GetLightFollowCamera()) 
    {
    this->CurrentRenderer->UpdateLightsGeometryToFollowCamera();
    }

  this->Interactor->Render();
}

//----------------------------------------------------------------------------
void vtkInteractorStyleMultiTouch::ProcessMultiTouch(vtkObject* object, unsigned long event, void* clientdata, void* calldata)
{
  vtkInteractorStyleMultiTouch* self 
    = reinterpret_cast<vtkInteractorStyleMultiTouch *>(clientdata);

  switch(event)
    {
    // One Touch 
    case vtkMultiTouchInterface::MultiTouchOneTouchEvent:
      if (self->HandleObservers && 
          self->HasObserver(vtkMultiTouchInterface::MultiTouchOneTouchEvent)) 
        {
        self->InvokeEvent(vtkMultiTouchInterface::MultiTouchOneTouchEvent,NULL);
        }
      else 
        {
        self->OnMultiTouchOneTouch();
        }
      break;

    // One Drag 
    case vtkMultiTouchInterface::MultiTouchOneDragEvent:
      if (self->HandleObservers && 
          self->HasObserver(vtkMultiTouchInterface::MultiTouchOneDragEvent)) 
        {
        self->InvokeEvent(vtkMultiTouchInterface::MultiTouchOneDragEvent,NULL);
        }
      else 
        {
        self->OnMultiTouchOneDrag();
        }
      break;

    // Two Touch 
    case vtkMultiTouchInterface::MultiTouchTwoTouchEvent:
      if (self->HandleObservers && 
          self->HasObserver(vtkMultiTouchInterface::MultiTouchTwoTouchEvent)) 
        {
        self->InvokeEvent(vtkMultiTouchInterface::MultiTouchTwoTouchEvent,NULL);
        }
      else 
        {
        self->OnMultiTouchTwoTouch();
        }
      break;

    // Two Drag 
    case vtkMultiTouchInterface::MultiTouchTwoDragEvent:
      if (self->HandleObservers && 
          self->HasObserver(vtkMultiTouchInterface::MultiTouchTwoDragEvent)) 
        {
        self->InvokeEvent(vtkMultiTouchInterface::MultiTouchTwoDragEvent,NULL);
        }
      else 
        {
        self->OnMultiTouchTwoDrag();
        }
      break;

    // Three Touch 
    case vtkMultiTouchInterface::MultiTouchThreeTouchEvent:
      if (self->HandleObservers && 
          self->HasObserver(vtkMultiTouchInterface::MultiTouchThreeTouchEvent)) 
        {
        self->InvokeEvent(vtkMultiTouchInterface::MultiTouchThreeTouchEvent,NULL);
        }
      else 
        {
        self->OnMultiTouchThreeTouch();
        }
      break;

    // Three Drag 
    case vtkMultiTouchInterface::MultiTouchThreeDragEvent:
      if (self->HandleObservers && 
          self->HasObserver(vtkMultiTouchInterface::MultiTouchThreeDragEvent)) 
        {
        self->InvokeEvent(vtkMultiTouchInterface::MultiTouchThreeDragEvent,NULL);
        }
      else 
        {
        self->OnMultiTouchThreeDrag();
        }
      break;

    // Four Touch 
    case vtkMultiTouchInterface::MultiTouchFourTouchEvent:
      if (self->HandleObservers && 
          self->HasObserver(vtkMultiTouchInterface::MultiTouchFourTouchEvent)) 
        {
        self->InvokeEvent(vtkMultiTouchInterface::MultiTouchFourTouchEvent,NULL);
        }
      else 
        {
        self->OnMultiTouchFourTouch();
        }
      break;

    // Four Drag 
    case vtkMultiTouchInterface::MultiTouchFourDragEvent:
      if (self->HandleObservers && 
          self->HasObserver(vtkMultiTouchInterface::MultiTouchFourDragEvent)) 
        {
        self->InvokeEvent(vtkMultiTouchInterface::MultiTouchFourDragEvent,NULL);
        }
      else 
        {
        self->OnMultiTouchFourDrag();
        }
      break;

    // Five Touch 
    case vtkMultiTouchInterface::MultiTouchFiveTouchEvent:
      if (self->HandleObservers && 
          self->HasObserver(vtkMultiTouchInterface::MultiTouchFiveTouchEvent)) 
        {
        self->InvokeEvent(vtkMultiTouchInterface::MultiTouchFiveTouchEvent,NULL);
        }
      else 
        {
        self->OnMultiTouchFiveTouch();
        }
      break;

    // Five Drag 
    case vtkMultiTouchInterface::MultiTouchFiveDragEvent:
      if (self->HandleObservers && 
          self->HasObserver(vtkMultiTouchInterface::MultiTouchFiveDragEvent)) 
        {
        self->InvokeEvent(vtkMultiTouchInterface::MultiTouchFiveDragEvent,NULL);
        }
      else 
        {
        self->OnMultiTouchFiveDrag();
        }
      break;

    // Six Touch 
    case vtkMultiTouchInterface::MultiTouchSixTouchEvent:
      if (self->HandleObservers && 
          self->HasObserver(vtkMultiTouchInterface::MultiTouchSixTouchEvent)) 
        {
        self->InvokeEvent(vtkMultiTouchInterface::MultiTouchSixTouchEvent,NULL);
        }
      else 
        {
        self->OnMultiTouchSixTouch();
        }
      break;

    // Six Drag 
    case vtkMultiTouchInterface::MultiTouchSixDragEvent:
      if (self->HandleObservers && 
          self->HasObserver(vtkMultiTouchInterface::MultiTouchSixDragEvent)) 
        {
        self->InvokeEvent(vtkMultiTouchInterface::MultiTouchSixDragEvent,NULL);
        }
      else 
        {
        self->OnMultiTouchSixDrag();
        }
      break;

    // Zoom
    case vtkMultiTouchInterface::MultiTouchZoomEvent:
      if (self->HandleObservers && 
          self->HasObserver(vtkMultiTouchInterface::MultiTouchZoomEvent)) 
        {
        self->InvokeEvent(vtkMultiTouchInterface::MultiTouchZoomEvent,NULL);
        }
      else 
        {
        self->OnMultiTouchZoom();
        }
      break;

    // Translate X
    case vtkMultiTouchInterface::MultiTouchTranslateXEvent:
      if (self->HandleObservers && 
          self->HasObserver(vtkMultiTouchInterface::MultiTouchTranslateXEvent)) 
        {
        self->InvokeEvent(vtkMultiTouchInterface::MultiTouchTranslateXEvent,NULL);
        }
      else 
        {
        self->OnMultiTouchTranslateX();
        }
      break;

    // Translate Y
    case vtkMultiTouchInterface::MultiTouchTranslateYEvent:
      if (self->HandleObservers && 
          self->HasObserver(vtkMultiTouchInterface::MultiTouchTranslateYEvent)) 
        {
        self->InvokeEvent(vtkMultiTouchInterface::MultiTouchTranslateYEvent,NULL);
        }
      else 
        {
        self->OnMultiTouchTranslateY();
        }
      break;

    // Translate Z
    case vtkMultiTouchInterface::MultiTouchTranslateZEvent:
      if (self->HandleObservers && 
          self->HasObserver(vtkMultiTouchInterface::MultiTouchTranslateZEvent)) 
        {
        self->InvokeEvent(vtkMultiTouchInterface::MultiTouchTranslateZEvent,NULL);
        }
      else 
        {
        self->OnMultiTouchTranslateZ();
        }
      break;

    // Rotate Y  
    case vtkMultiTouchInterface::MultiTouchRotateYEvent:
      if (self->HandleObservers && 
          self->HasObserver(vtkMultiTouchInterface::MultiTouchRotateYEvent)) 
        {
        self->InvokeEvent(vtkMultiTouchInterface::MultiTouchRotateYEvent,NULL);
        }
      else 
        {
        self->OnMultiTouchRotateY();
        }
      break;

    // Rotate Z  
    case vtkMultiTouchInterface::MultiTouchRotateZEvent:
      if (self->HandleObservers && 
          self->HasObserver(vtkMultiTouchInterface::MultiTouchRotateZEvent)) 
        {
        self->InvokeEvent(vtkMultiTouchInterface::MultiTouchRotateZEvent,NULL);
        }
      else 
        {
        self->OnMultiTouchRotateZ();
        }
      break;

    // Release 
    case vtkMultiTouchInterface::MultiTouchReleaseEvent:
      if (self->HandleObservers && 
          self->HasObserver(vtkMultiTouchInterface::MultiTouchReleaseEvent)) 
        {
        self->InvokeEvent(vtkMultiTouchInterface::MultiTouchReleaseEvent,NULL);
        }
      else 
        {
        self->OnMultiTouchRelease();
        }
      break;
    }
}

//----------------------------------------------------------------------------
void vtkInteractorStyleMultiTouch::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}