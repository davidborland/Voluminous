///////////////////////////////////////////////////////////////////////////////////////////////
//
// Name:        vtkInteractorStyleMultiTouch.h
//
// Author:      David Borland
//
// Description: Provides an event-driven interface to the vtkMultiTouchInteractor
//
///////////////////////////////////////////////////////////////////////////////////////////////


#ifndef __vtkInteractorStyleMultiTouch_h
#define __vtkInteractorStyleMultiTouch_h

#include "vtkInteractorStyle.h"

#include "vtkMultiTouchInterface.h"

class VTK_RENDERING_EXPORT vtkInteractorStyleMultiTouch : public vtkInteractorStyle
{
public:
  static vtkInteractorStyleMultiTouch *New();  
  vtkTypeRevisionMacro(vtkInteractorStyleMultiTouch,vtkInteractorStyle);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Set the Interactor wrapper being controlled by this object.
  virtual void SetInteractor(vtkRenderWindowInteractor *interactor);

  // Description:
  // Handle multi-touch events
  virtual void OnMultiTouchOneTouch() {}
  virtual void OnMultiTouchOneDrag();
  virtual void OnMultiTouchTwoTouch() {}
  virtual void OnMultiTouchTwoDrag() {}
  virtual void OnMultiTouchThreeTouch() {}
  virtual void OnMultiTouchThreeDrag() {}
  virtual void OnMultiTouchFourTouch() {}
  virtual void OnMultiTouchFourDrag() {}
  virtual void OnMultiTouchFiveTouch() {}
  virtual void OnMultiTouchFiveDrag() {}
  virtual void OnMultiTouchSixTouch() {}
  virtual void OnMultiTouchSixDrag() {}
  virtual void OnMultiTouchZoom();
  virtual void OnMultiTouchTranslateX();
  virtual void OnMultiTouchTranslateY();
  virtual void OnMultiTouchTranslateZ() {}
  virtual void OnMultiTouchRotateX();
  virtual void OnMultiTouchRotateY();
  virtual void OnMultiTouchRotateZ();
  virtual void OnMultiTouchRelease() {}

protected:    
  vtkInteractorStyleMultiTouch();
  ~vtkInteractorStyleMultiTouch();

  // Description:
  // Multi-touch event method
  static void ProcessMultiTouch(vtkObject* object, 
                                unsigned long event,
                                void* clientdata, 
                                void* calldata);

  // Description:
  // Callback for multi-touch events
  vtkCallbackCommand* MultiTouchCallbackCommand;

  // The multi-touch interface
  vtkMultiTouchInterface* MultiTouchInterface;

private:
  vtkInteractorStyleMultiTouch(const vtkInteractorStyleMultiTouch&);  // Not implemented.
  void operator=(const vtkInteractorStyleMultiTouch&);  // Not implemented.
};

#endif
