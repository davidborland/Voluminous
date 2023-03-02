/*=========================================================================

  Name:        Slice.cpp

  Author:      David Borland, The Renaissance Computing Institute (RENCI)

  Copyright:   The Renaissance Computing Institute (RENCI)

  Description: Container class for a slice of the volume.

=========================================================================*/


#include "Slice.h"


#include <vtkActor.h>
#include <vtkActorCollection.h>
#include <vtkAlgorithmOutput.h>
#include <vtkClipPolyData.h>
#include <vtkColorTransferFunction.h>
#include <vtkCutter.h>
#include <vtkPlane.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>


#include <vtkReverseSense.h>


Slice::Slice(vtkAlgorithmOutput* volume, vtkColorTransferFunction* colorMap,
             int direction, double clipValue, const double center[3], const double size[3]) {
    // Create the plane used for cutting
    plane = vtkSmartPointer<vtkPlane>::New();

    if (direction == 0) plane->SetNormal(0, 0, 1);         // XY
    else if (direction == 1) plane->SetNormal(0, 1, 0);    // XZ
    else if (direction == 2) plane->SetNormal(1, 0, 0);    // YZ

    plane->SetOrigin(center[0], center[1], center[2]);


    // Create the cutter
    cutter = vtkSmartPointer<vtkCutter>::New();
    cutter->SetInputConnection(volume);
    cutter->SetCutFunction(plane);


    // Create the clipper
    clipperPos = vtkSmartPointer<vtkClipPolyData>::New();
    clipperPos->SetInputConnection(cutter->GetOutputPort());
    clipperPos->SetValue(clipValue);

    clipperNeg = vtkSmartPointer<vtkClipPolyData>::New();
    clipperNeg->SetInputConnection(cutter->GetOutputPort());
    clipperNeg->SetValue(-clipValue);
    clipperNeg->InsideOutOn();


    // Create the normal flippers
    vtkSmartPointer<vtkReverseSense> reversePos = vtkSmartPointer<vtkReverseSense>::New();
    reversePos->SetInputConnection(clipperPos->GetOutputPort());
    reversePos->ReverseNormalsOn();
    reversePos->ReverseCellsOn();
    
    vtkSmartPointer<vtkReverseSense> reverseNeg = vtkSmartPointer<vtkReverseSense>::New();
    reverseNeg->SetInputConnection(clipperNeg->GetOutputPort());
    reverseNeg->ReverseNormalsOn();
    reverseNeg->ReverseCellsOn();

    
    // Mappers for the slices
    vtkSmartPointer<vtkPolyDataMapper> mapperPos1 = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapperPos1->SetInputConnection(clipperPos->GetOutputPort());
    mapperPos1->SetLookupTable(colorMap);
    mapperPos1->InterpolateScalarsBeforeMappingOn();
    mapperPos1->UseLookupTableScalarRangeOn();

    vtkSmartPointer<vtkPolyDataMapper> mapperPos2 = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapperPos2->SetInputConnection(reversePos->GetOutputPort());
    mapperPos2->SetLookupTable(colorMap);        
    mapperPos2->InterpolateScalarsBeforeMappingOn();
    mapperPos2->UseLookupTableScalarRangeOn();

    vtkSmartPointer<vtkPolyDataMapper> mapperNeg1 = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapperNeg1->SetInputConnection(clipperNeg->GetOutputPort());
    mapperNeg1->SetLookupTable(colorMap);
    mapperNeg1->InterpolateScalarsBeforeMappingOn();
    mapperNeg1->UseLookupTableScalarRangeOn();

    vtkSmartPointer<vtkPolyDataMapper> mapperNeg2 =vtkSmartPointer< vtkPolyDataMapper>::New();
    mapperNeg2->SetInputConnection(reverseNeg->GetOutputPort());
    mapperNeg2->SetLookupTable(colorMap);        
    mapperNeg2->InterpolateScalarsBeforeMappingOn();
    mapperNeg2->UseLookupTableScalarRangeOn();


    // Actors for the slices
    vtkSmartPointer<vtkActor> actorPos1 = vtkSmartPointer<vtkActor>::New();
    actorPos1->SetMapper(mapperPos1);

    vtkSmartPointer<vtkActor> actorPos2 = vtkSmartPointer<vtkActor>::New();
    actorPos2->SetMapper(mapperPos2);

    vtkSmartPointer<vtkActor> actorNeg1 = vtkSmartPointer<vtkActor>::New();
    actorNeg1->SetMapper(mapperNeg1);

    vtkSmartPointer<vtkActor> actorNeg2 = vtkSmartPointer<vtkActor>::New();
    actorNeg2->SetMapper(mapperNeg2);


    // Move the slices to the appropriate positions
    if (direction == 0) {
        // XY
        actorPos1->SetPosition(0.0, 0.0, size[2] * 0.5);
        actorPos2->SetPosition(0.0, 0.0, -size[2] * 0.5);

        actorNeg1->SetPosition(0.0, 0.0, size[2] * 0.5);
        actorNeg2->SetPosition(0.0, 0.0, -size[2] * 0.5);
    }
    else if (direction == 1) {
        // XZ
        actorPos1->SetPosition(0.0, size[1] * 0.5, 0.0);
        actorPos2->SetPosition(0.0, -size[1] * 0.5, 0.0);

        actorNeg1->SetPosition(0.0, size[1] * 0.5, 0.0);
        actorNeg2->SetPosition(0.0, -size[1] * 0.5, 0.0);
    }
    else {
        // ZY
        actorPos1->SetPosition(size[0] * 0.5, 0.0, 0.0);
        actorPos2->SetPosition(-size[0] * 0.5, 0.0, 0.0);

        actorNeg1->SetPosition(size[0] * 0.5, 0.0, 0.0);
        actorNeg2->SetPosition(-size[0] * 0.5, 0.0, 0.0);
    }

    // Turn on backface culling and turn off lighting
    actorPos1->GetProperty()->BackfaceCullingOn();
    actorPos2->GetProperty()->BackfaceCullingOn();
    actorPos1->GetProperty()->SetAmbient(1.0);
    actorPos2->GetProperty()->SetAmbient(1.0);
    actorPos1->GetProperty()->SetDiffuse(0.0);
    actorPos2->GetProperty()->SetDiffuse(0.0);
    actorPos1->GetProperty()->SetSpecular(0.0);
    actorPos2->GetProperty()->SetSpecular(0.0);

    actorNeg1->GetProperty()->BackfaceCullingOn();
    actorNeg2->GetProperty()->BackfaceCullingOn();
    actorNeg1->GetProperty()->SetAmbient(1.0);
    actorNeg2->GetProperty()->SetAmbient(1.0);
    actorNeg1->GetProperty()->SetDiffuse(0.0);
    actorNeg2->GetProperty()->SetDiffuse(0.0);
    actorNeg1->GetProperty()->SetSpecular(0.0);
    actorNeg2->GetProperty()->SetSpecular(0.0);


    // Aggregate the actors
    actors = vtkSmartPointer<vtkActorCollection>::New();
    actors->AddItem(actorPos1);
    actors->AddItem(actorPos2);
    actors->AddItem(actorNeg1);
    actors->AddItem(actorNeg2);
}

Slice::~Slice() {
}


void Slice::SetInput(vtkAlgorithmOutput* volume) {
    cutter->SetInputConnection(volume);
}


vtkActorCollection* Slice::GetActors() {
    return actors;
}


void Slice::SetClipValue(double value) {
    clipperPos->SetValue(value);
    clipperNeg->SetValue(-value);

    clipperPos->Update();
    clipperNeg->Update();
}