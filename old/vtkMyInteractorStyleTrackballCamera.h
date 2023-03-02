///////////////////////////////////////////////////////////////////////////////////////////////
//
// Name:        vtkMyInteractorStyleTrackballCamera.h
//
// Author:      David Borland
//
// Description: Override keypresses.
//
///////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __vtkMyInteractorStyleTrackballCamera_h
#define __vtkMyInteractorStyleTrackballCamera_h

#include "vtkInteractorStyleTrackballCamera.h"

class VTK_RENDERING_EXPORT vtkMyInteractorStyleTrackballCamera : public vtkInteractorStyleTrackballCamera 
{
public:
  static vtkMyInteractorStyleTrackballCamera *New();
  vtkTypeRevisionMacro(vtkMyInteractorStyleTrackballCamera,vtkInteractorStyleTrackballCamera);

  virtual void OnChar();

protected:
  vtkMyInteractorStyleTrackballCamera(){}
  ~vtkMyInteractorStyleTrackballCamera(){}

private:
  vtkMyInteractorStyleTrackballCamera(const vtkMyInteractorStyleTrackballCamera&);  // Not implemented.
  void operator=(const vtkMyInteractorStyleTrackballCamera&);  // Not implemented.
};

#endif


