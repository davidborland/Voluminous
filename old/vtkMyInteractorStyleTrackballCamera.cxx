///////////////////////////////////////////////////////////////////////////////////////////////
//
// Name:        vtkMyInteractorStyleTrackballCamera.cxx
//
// Author:      David Borland
//
// Description: Override keypresses.
//
///////////////////////////////////////////////////////////////////////////////////////////////

#include "vtkMyInteractorStyleTrackballCamera.h"

#include <vtkAbstractPicker.h>
#include <vtkAbstractPropPicker.h>
#include <vtkAssemblyPath.h>
#include <vtkObjectFactory.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

vtkCxxRevisionMacro(vtkMyInteractorStyleTrackballCamera, "$Revision: 1.1 $");
vtkStandardNewMacro(vtkMyInteractorStyleTrackballCamera);

//----------------------------------------------------------------------------
void vtkMyInteractorStyleTrackballCamera::OnChar()
{    
    // Don't want default vtk behavior for keypresses
    vtkRenderWindowInteractor *rwi = this->Interactor;

    switch (rwi->GetKeyCode()) 
    {
    case 'Q' :
    case 'q' :
      rwi->ExitCallback();
      break;

      break;
    }
}