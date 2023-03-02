/*=========================================================================

  Name:        Isosurface.cpp

  Author:      David Borland, The Renaissance Computing Institute (RENCI)

  Copyright:   The Renaissance Computing Institute (RENCI)

  Description: Container class for an isosurface.

=========================================================================*/


#include "Isosurface.h"


#include <vtkActor.h>
#include <vtkAlgorithmOutput.h>
#include <vtkContourFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkReverseSense.h>
#include <vtkXMLMaterial.h>


Isosurface::Isosurface(vtkAlgorithmOutput* volume, double value, bool translucent,
                       const std::string& opaqueMaterial, const std::string& translucentMaterial) 
	: translucent(translucent), opaqueMaterial(opaqueMaterial), translucentMaterial(translucentMaterial) {
    // Create the isosurface
    isosurface = vtkSmartPointer<vtkContourFilter>::New();
    isosurface->SetInputConnection(volume);
    isosurface->SetNumberOfContours(1);
    isosurface->SetValue(0, value);
    isosurface->ComputeScalarsOff();
    isosurface->ComputeNormalsOn();
    isosurface->ComputeGradientsOff();


    // Mapper for the surface
    vtkSmartPointer<vtkPolyDataMapper> mapper =  vtkSmartPointer<vtkPolyDataMapper>::New();

    if (value < 0.0) {
        // Negative value, so flip the normals
        vtkSmartPointer<vtkReverseSense> reverse =  vtkSmartPointer<vtkReverseSense>::New();
        reverse->SetInputConnection(isosurface->GetOutputPort());
        reverse->ReverseCellsOff();
        reverse->ReverseNormalsOn();

        mapper->SetInputConnection(reverse->GetOutputPort());
    }
    else {
        // Positive value, don't flip the normals 
        mapper->SetInputConnection(isosurface->GetOutputPort());
    }


    // Actor for the surface
    actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);


    // Appearance
    vtkProperty* p = actor->GetProperty();

    p->SetAmbient(0.1);

    p->SetDiffuse(1.0);

    p->SetSpecular(1.0);
    p->SetSpecularColor(1.0, 1.0, 1.0);
    p->SetSpecularPower(50.0);

	SetTranslucent(translucent);
    
    p->ShadingOn();
}

Isosurface::~Isosurface() {
}


void Isosurface::SetInput(vtkAlgorithmOutput* volume) {
    isosurface->SetInputConnection(volume);
}


vtkActor* Isosurface::GetActor() {
    return actor;
}

vtkContourFilter* Isosurface::GetIsosurface() {
    return isosurface;
}


bool Isosurface::GetTranslucent() {
    return translucent;
}

void Isosurface::SetTranslucent(bool translucent) {
    this->translucent = translucent;

	vtkProperty* p = actor->GetProperty();

	if (translucent) {
        // Use shader for translucent surface
        p->LoadMaterialFromString(translucentMaterial.c_str());
		
		// XXX: This needs to be < 1.0 (and > 0.0) to work without depth sorting for silhouetteFalloff.xml.
		// Exactly why, I'm not sure.
        p->SetOpacity(0.1);
    }
    else {
        // Per-pixel lighting
        p->LoadMaterialFromString(opaqueMaterial.c_str());

		p->SetOpacity(1.0);
    }
}