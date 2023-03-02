/*=========================================================================

  Name:        Isosurface.h

  Author:      David Borland, The Renaissance Computing Institute (RENCI)

  Copyright:   The Renaissance Computing Institute (RENCI)

  Description: Container class for an isosurface.

=========================================================================*/


#ifndef ISOSURFACE_H

#include <vtkSmartPointer.h>

class vtkActor;
class vtkAlgorithmOutput;
class vtkContourFilter;
class vtkPolyDataMapper;
class vtkProperty;
class vtkReverseSense;
class vtkXMLMaterial;


class Isosurface {
public:
    Isosurface(vtkAlgorithmOutput* volume, double value, bool translucent,
               const std::string& opaqueMaterial, const std::string& translucentMaterial);
    ~Isosurface();

    void SetInput(vtkAlgorithmOutput* volume);

    vtkActor* GetActor();
    vtkContourFilter* GetIsosurface();

	bool GetTranslucent();
	void SetTranslucent(bool translucent);

protected:
    vtkSmartPointer<vtkContourFilter> isosurface;
    vtkSmartPointer<vtkActor> actor;

    std::string opaqueMaterial;
    std::string translucentMaterial;

	bool translucent;
};


#endif