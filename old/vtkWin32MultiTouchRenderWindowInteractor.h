///////////////////////////////////////////////////////////////////////////////////////////////
//
// Name:        vtkWin32MultiTouchRenderWindowInteractor.h
//
// Author:      David Borland
//
// Description: A vtkWin32RenderWindowInteractor that processes Jason Coposky's multi-touch 
//              events in addition to regular window events
//
///////////////////////////////////////////////////////////////////////////////////////////////


#ifndef __vtkWin32MultiTouchRenderWindowInteractor_h
#define __vtkWin32MultiTouchRenderWindowInteractor_h

#include "vtkWin32RenderWindowInteractor.h"

#include "vtkMultiTouchInterface.h"

class VTK_RENDERING_EXPORT vtkWin32MultiTouchRenderWindowInteractor : public vtkWin32RenderWindowInteractor 
{
public:
  // Description:
  // Construct object.
  static vtkWin32MultiTouchRenderWindowInteractor *New();

  vtkTypeRevisionMacro(vtkWin32MultiTouchRenderWindowInteractor,vtkWin32RenderWindowInteractor);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Initialize the event handler
  virtual void Initialize();

  // Description:
  // This will start up the event loop and never return. If you
  // call this method it will loop processing events until the
  // application is exited.
  virtual void Start();

  // Description:
  // Set/Get the multi-touch interactor
  vtkGetObjectMacro(MultiTouchInterface,vtkMultiTouchInterface);

protected:
  vtkWin32MultiTouchRenderWindowInteractor();
  ~vtkWin32MultiTouchRenderWindowInteractor();

  vtkMultiTouchInterface* MultiTouchInterface;

private:
  vtkWin32MultiTouchRenderWindowInteractor(const vtkWin32MultiTouchRenderWindowInteractor&);  // Not implemented.
  void operator=(const vtkWin32MultiTouchRenderWindowInteractor&);  // Not implemented.

};

#endif