///////////////////////////////////////////////////////////////////////////////////////////////
//
// Name:        VTKCallbacks.cpp
//
// Author:      David Borland
//
// Description: Classes derived from vtkCommand to handle interaction events in Voluminous.
//
///////////////////////////////////////////////////////////////////////////////////////////////


#include "VTKCallbacks.h"

#include <vtkRendererCollection.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderLargeImage.h>
#include <vtkTIFFWriter.h>


KeyPressCallback::KeyPressCallback() {
}


KeyPressCallback* KeyPressCallback::New() {
    return new KeyPressCallback;
}

void KeyPressCallback::Execute(vtkObject* caller, unsigned long eventId, void* callData) {
    vtkRenderWindowInteractor* interactor = vtkRenderWindowInteractor::SafeDownCast(caller);

    if (eventId == KeyPressEvent) {
        char c = interactor->GetKeyCode();

        if (c == 's') {
            vtkRenderWindow* window = interactor->GetRenderWindow();

            vtkRenderLargeImage* large = vtkRenderLargeImage::New();
            large->SetInput(window->GetRenderers()->GetFirstRenderer());
            large->SetMagnification(3);

            vtkTIFFWriter* writer = vtkTIFFWriter::New();
            writer->SetCompressionToNoCompression();
            writer->SetInputConnection(large->GetOutputPort());

            window->Render();
            window->Modified();

            writer->SetFileName("Data/VoluminousImage.tif");
            writer->Update();
        }
        else if (c == 'q') {
            exit(0);
        }
    }
}