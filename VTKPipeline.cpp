/*=========================================================================

  Name:        VTKPipeline.cpp

  Author:      David Borland, The Renaissance Computing Institute (RENCI)

  Copyright:   The Renaissance Computing Institute (RENCI)

  Description: Container class for all Voluminous VTK code.

=========================================================================*/


#include "VTKPipeline.h"

#include "Isosurface.h"
#include "Slice.h"

#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkColorTransferFunction.h>
#include <vtkContourFilter.h>
#include <vtkCubeAxesActor.h>
#include <vtkImageActor.h>
#include <vtkImageData.h>
#include <vtkImageMapper.h>
#include <vtkImageResize.h>
#include <vtkOutlineFilter.h>
#include <vtkPNGReader.h>
#include <vtkPNGWriter.h>
#include <vtkPointData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkRendererCollection.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkScalarBarActor.h>
#include <vtkStructuredPointsReader.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <vtkTubeFilter.h>
#include <vtkWindowToImageFilter.h>
#include <vtkXMLImageDataReader.h>

#include <algorithm>


#include <vtkInteractorStyleTrackballActor.h>


VTKPipeline::VTKPipeline(vtkRenderWindowInteractor* rwi,
                         vtkImageData* logoImage, vtkImageData* bwLogoImage,
                         const std::string& opaqueMaterial, const std::string& translucentMaterial)
: interactor(rwi), opaqueMaterial(opaqueMaterial), translucentMaterial(translucentMaterial) {
    // Renderer
    renderer = vtkSmartPointer<vtkRenderer>::New();

    renderer->SetBackground(0.95, 0.95, 0.9);
//    renderer->SetBackground(1.0, 1.0, 1.0);

    interactor->GetRenderWindow()->AddRenderer(renderer);


    // No reader or volume yet
    reader = NULL;
    volume = NULL;


    // No slices yet
    slices[0] = slices[1] = slices[2] = NULL;

    // Create all member visualization objects
    shrinker = vtkSmartPointer<vtkImageResize>::New();

    axes = vtkSmartPointer<vtkCubeAxesActor>::New();
    colorLegend = vtkSmartPointer<vtkScalarBarActor>::New();
    dataLabel = vtkSmartPointer<vtkTextActor>::New();


    // Create the logos
    logo = vtkSmartPointer<vtkImageActor>::New();
    logo->SetInput(logoImage); 
    logo->InterpolateOn();
    logo->SetOrigin((double)logoImage->GetDimensions()[0] / 2.0, (double)(double)logoImage->GetDimensions()[0] / 2.0, 0.0);
    logo->SetOpacity(0.5);

    bwLogo = vtkSmartPointer<vtkImageActor>::New();
    bwLogo->SetInput(bwLogoImage); 
    bwLogo->InterpolateOn();
    bwLogo->SetOrigin((double)bwLogoImage->GetDimensions()[0] / 2.0, (double)(double)bwLogoImage->GetDimensions()[0] / 2.0, 0.0);
    bwLogo->SetOpacity(0.5);
    bwLogo->VisibilityOff();


    // Use a separate renderer for the logo, to make placement easier
    logoRenderer = vtkSmartPointer<vtkRenderer>::New();
    double scale = 0.1;
    logoRenderer->SetViewport(0.0, 0.0, scale, scale);
    logoRenderer->InteractiveOff();
    logoRenderer->AddViewProp(logo);
    logoRenderer->AddViewProp(bwLogo);

    interactor->GetRenderWindow()->AddRenderer(logoRenderer);
    interactor->GetRenderWindow()->SetNumberOfLayers(2);

    renderer->SetLayer(0);
    logoRenderer->SetLayer(1);


    // No data range yet
    dataRange[0] = dataRange[1] = 0.0;


    // Color map type
    colorMapType = Color;
}

VTKPipeline::~VTKPipeline() {
    interactor->GetRenderWindow()->RemoveRenderer(renderer);
    interactor->GetRenderWindow()->RemoveRenderer(logoRenderer);

    // Clean up
    for (int i = 0; i < (int)isosurfaces.size(); i++) {
        delete isosurfaces[i];
    }

    for (int i = 0; i < 3; i++) {
        if (slices[i]) {
            delete slices[i];
        }
    }
}


bool VTKPipeline::OpenVolume(const std::string& fileName, std::string* errorMessage) {
    // Should only call this once per pipeline
    if (reader) {
//        std::cout << "VTKPipeline::OpenVolume() : Volume already loaded." << std::endl;
        *errorMessage = "Volume already loaded";

        return false;
    }


    // File information string
    size_t p = fileName.find_last_of("/\\") + 1;
    std::string fileInfo = fileName.substr(p, fileName.find_last_of(".") - p);

    if (fileName.rfind(".vtk") == fileName.length() - 4) {
        // Load legacy VTK structured point data
        vtkSmartPointer<vtkStructuredPointsReader> spReader = vtkSmartPointer<vtkStructuredPointsReader>::New();
        spReader->SetFileName(fileName.c_str());
        
        if (spReader->IsFileStructuredPoints()) {
            reader = spReader;

            reader->Update();
            
            std::string header = spReader->GetHeader();
            if (header.size() > 0) {
                fileInfo = header;
            }
        }
    }
    else if (fileName.rfind(".vti") == fileName.length() - 4) {
        // Load VTK XML image data file
        vtkSmartPointer<vtkXMLImageDataReader> iReader = vtkSmartPointer<vtkXMLImageDataReader>::New();
        iReader->SetFileName(fileName.c_str());
        
        reader = iReader;

        reader->Update();
    }  

    
    if (reader == NULL) {
//        std::cout << "VTKPipeline::OpenVolume() : Volume must be in .vtk structured points or .vti format." << std::endl;
        *errorMessage = "Volume must be in .vtk structured points or .vti format";

        return false;
    }
            

    if (reader->GetOutputDataObject(0) == NULL) {
//       std::cout << "VTKPipeline::OpenVolume() : Could not open " << fileName << std::endl;     
        *errorMessage = "Could not open " + fileName;

       return false;
    }


    // Go ahead and set the data label string
    dataLabel->SetInput(fileInfo.c_str());


    return true;
}


bool VTKPipeline::CreateVisualization(std::string& errorMessage) {
    // Should only call this once per pipeline
    if (volume) {
//        std::cout << "VTKPipeline::CreateVisualization() : Visualization already created." << std::endl;
        errorMessage = "Visualization already created";

        return false;
    }

       
    // Get the volume information
    volume = (vtkImageData*) reader->GetOutputDataObject(0);

    volume->GetScalarRange(dataRange);

    double bounds[6];
    volume->GetBounds(bounds);

    double origin[3];
    volume->GetOrigin(origin);

    double size[3];
    for (int i = 0; i < 3; i++) {
        size[i] = (bounds[2 * i + 1] - bounds[2 * i]);
    }

    double center[3];
    for (int i = 0; i < 3; i++) {
        center[i] = origin[i] + size[i] * 0.5;
    }


    // Set up the volume shrinker
    shrinker->SetInputConnection(reader->GetOutputPort());
    shrinker->SetMagnificationFactors(0.5, 0.5, 0.5);
    shrinker->SetResizeMethodToMagnificationFactors();
    shrinker->InterpolateOn();


    // Set up axes
    double axesBounds[6];
    for (int i = 0; i < 3; i++) {
        // Do some rounding if close enough
        double fraction = 0.01;
        double d;

        int i1 = 2 * i;
        int i2 = 2 * i + 1;
        
        d = bounds[i1] - floor(bounds[i1]);
        if (d < size[i] * fraction) axesBounds[i1] = floor(bounds[i1]);
        else axesBounds[i1] = bounds[i1];

        d = ceil(bounds[i2]) - bounds[i2];
        if (d < size[i] * fraction) axesBounds[i2] = ceil(bounds[i2]);
        else axesBounds[i2] = bounds[i2];
    }

    axes->SetBounds(axesBounds);

    axes->GetXAxesLinesProperty()->SetColor(0.75, 0.75, 0.75);
    axes->GetXAxesLinesProperty()->SetAmbient(1.0);
    axes->GetXAxesLinesProperty()->SetDiffuse(0.25);
    axes->GetXAxesLinesProperty()->SetSpecular(0.0);

    axes->GetYAxesLinesProperty()->SetColor(0.75, 0.75, 0.75);
    axes->GetYAxesLinesProperty()->SetAmbient(1.0);
    axes->GetYAxesLinesProperty()->SetDiffuse(0.25);
    axes->GetYAxesLinesProperty()->SetSpecular(0.0);

    axes->GetZAxesLinesProperty()->SetColor(0.75, 0.75, 0.75);
    axes->GetZAxesLinesProperty()->SetAmbient(1.0);
    axes->GetZAxesLinesProperty()->SetDiffuse(0.25);
    axes->GetZAxesLinesProperty()->SetSpecular(0.0);

    axes->GetLabelTextProperty(0)->SetColor(0.75, 0.75, 0.75);
    axes->GetLabelTextProperty(1)->SetColor(0.75, 0.75, 0.75);
    axes->GetLabelTextProperty(2)->SetColor(0.75, 0.75, 0.75);

    axes->GetTitleTextProperty(0)->SetColor(0.75, 0.75, 0.75);
    axes->GetTitleTextProperty(1)->SetColor(0.75, 0.75, 0.75);
    axes->GetTitleTextProperty(2)->SetColor(0.75, 0.75, 0.75);

    axes->SetFlyModeToOuterEdges();

    axes->SetCamera(renderer->GetActiveCamera());

    renderer->AddViewProp(axes);


    // Create the outline    
    vtkSmartPointer<vtkOutlineFilter> outline = vtkSmartPointer<vtkOutlineFilter>::New();
    outline->SetInputConnection(reader->GetOutputPort());

    vtkSmartPointer<vtkTubeFilter> tubeOutline = vtkSmartPointer<vtkTubeFilter>::New();
    tubeOutline->SetInputConnection(outline->GetOutputPort());
    tubeOutline->SetRadius(size[0] * 0.002);
    
    vtkSmartPointer<vtkPolyDataMapper> outlineMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    outlineMapper->SetInputConnection(tubeOutline->GetOutputPort());

    vtkSmartPointer<vtkActor> outlineActor = vtkSmartPointer<vtkActor>::New();
    outlineActor->SetMapper(outlineMapper);
//    outlineActor->GetProperty()->SetColor(background[0] * 0.75, background[1] * 0.75, background[2] * 0.75);
    outlineActor->GetProperty()->SetColor(0.75, 0.75, 0.75);
    outlineActor->GetProperty()->SetAmbient(1.0);
    outlineActor->GetProperty()->SetDiffuse(0.25);
    outlineActor->GetProperty()->SetSpecular(0.0);

    renderer->AddViewProp(outlineActor);


    // Create the data label
//    dataLabel->SetInput(fileInfo.c_str());
    dataLabel->GetTextProperty()->SetColor(0.75, 0.75, 0.75);
    dataLabel->GetTextProperty()->SetJustificationToRight();
    dataLabel->SetTextScaleModeToProp();
    dataLabel->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();
    dataLabel->SetPosition(1.0, 0.0);
    dataLabel->SetPosition2(1.0, 0.025);

    renderer->AddViewProp(dataLabel);


    // Create the isosurfaces
    double maxValue = GetMaximumAbsoluteValue();
    double val1 = maxValue * 0.1;
    double val2 = maxValue * 0.01;

    isosurfaces.push_back(new Isosurface(reader->GetOutputPort(), -val1, false, opaqueMaterial, translucentMaterial));
    isosurfaces.push_back(new Isosurface(reader->GetOutputPort(), val1, false, opaqueMaterial, translucentMaterial));
    isosurfaces.push_back(new Isosurface(reader->GetOutputPort(), -val2, true, opaqueMaterial, translucentMaterial));
    isosurfaces.push_back(new Isosurface(reader->GetOutputPort(), val2, true, opaqueMaterial, translucentMaterial));

    for (int i = 0; i < (int)isosurfaces.size(); i++) {
        renderer->AddViewProp(isosurfaces[i]->GetActor());
    }


    // Create the slices
	colorMap = vtkSmartPointer<vtkColorTransferFunction>::New();
    SetColorMap();
   
    for (int i = 0; i < 3; i++) {
        slices[i] = new Slice(reader->GetOutputPort(), colorMap, i, val2, center, size);

        vtkActorCollection* a = slices[i]->GetActors();
        a->InitTraversal();
        for (vtkIdType i = 0; i < a->GetNumberOfItems(); i++) {
            renderer->AddViewProp(a->GetNextActor());
        }
    }


    // Create a color legend
    double width = 0.5;
    double height = 0.1;
    colorLegend->SetLookupTable(colorMap);
    colorLegend->SetOrientationToHorizontal();
    colorLegend->SetPosition((1.0 - width) * 0.5, 0.9);
    colorLegend->SetWidth(width);
    colorLegend->SetHeight(height);
    colorLegend->SetMaximumNumberOfColors(1024);
    colorLegend->SetLabelTextProperty(axes->GetLabelTextProperty(0));

    renderer->AddViewProp(colorLegend);


    // Show the data
    renderer->ResetCamera();
    Render();


    return true;
}


void VTKPipeline::SaveScreenshot(const std::string& fileName) {
    Render();
    interactor->GetRenderWindow()->Modified();

    vtkSmartPointer<vtkWindowToImageFilter> image = vtkSmartPointer<vtkWindowToImageFilter>::New();
    image->SetInput(interactor->GetRenderWindow());

    vtkSmartPointer<vtkPNGWriter> writer = vtkSmartPointer<vtkPNGWriter>::New();
    writer->SetInputConnection(image->GetOutputPort());
    writer->SetFileName(fileName.c_str());
    writer->Write();
}


double VTKPipeline::GetIsovalue1() {
    return isosurfaces[1]->GetIsosurface()->GetValue(0);
}

double VTKPipeline::GetIsovalue2() {
    return isosurfaces[3]->GetIsosurface()->GetValue(0);
}


void VTKPipeline::SetIsovalue1(double value, bool doFast) {
    SetIsovalues(0, 1, value, doFast);
}

void VTKPipeline::SetIsovalue2(double value, bool doFast) {
    SetIsovalues(2, 3, value, doFast);
}

void VTKPipeline::SetIsovalues(int index1, int index2, double value, bool doFast) {
    if (doFast) {
        isosurfaces[index1]->SetInput(shrinker->GetOutputPort());
        isosurfaces[index2]->SetInput(shrinker->GetOutputPort());

/*
        for (int i = 0; i < 3; i++) {
            slices[i]->SetInput(shrinker->GetOutputPort());
        }
*/
    }
    else {       
        isosurfaces[index1]->SetInput(reader->GetOutputPort());
        isosurfaces[index2]->SetInput(reader->GetOutputPort());

/*
        for (int i = 0; i < 3; i++) {
            slices[i]->SetInput(reader->GetOutputPort());
        }
*/
    }

    isosurfaces[index1]->GetIsosurface()->SetValue(0, -value);
    isosurfaces[index2]->GetIsosurface()->SetValue(0, value);    
    
    if (!doFast) {
        // Only update clipping of slices when interaction is finished
        double v1 = GetIsovalue1();
        double v2 = GetIsovalue2();
        double clipValue = std::min(v1, v2);

        for (int i = 0; i < 3; i++) {
            slices[i]->SetClipValue(clipValue);
        }
    }
}


bool VTKPipeline::GetIsovalue1Visible() {
	return isosurfaces[0]->GetActor()->GetVisibility() == 1;
}

bool VTKPipeline::GetIsovalue1Translucent() {
	return (bool)isosurfaces[0]->GetTranslucent();
}

bool VTKPipeline::GetIsovalue2Visible() {
	return isosurfaces[2]->GetActor()->GetVisibility() == 1;
}

bool VTKPipeline::GetIsovalue2Translucent() {
	return isosurfaces[2]->GetTranslucent();
}



void VTKPipeline::SetIsovalue1Visible(bool visible) {
	isosurfaces[0]->GetActor()->SetVisibility(visible);
	isosurfaces[1]->GetActor()->SetVisibility(visible);
}

void VTKPipeline::SetIsovalue1Translucent(bool translucent) {
	isosurfaces[0]->SetTranslucent(translucent);
	isosurfaces[1]->SetTranslucent(translucent);
}

void VTKPipeline::SetIsovalue2Visible(bool visible) {
	isosurfaces[2]->GetActor()->SetVisibility(visible);
	isosurfaces[3]->GetActor()->SetVisibility(visible);
}

void VTKPipeline::SetIsovalue2Translucent(bool translucent) {
	isosurfaces[2]->SetTranslucent(translucent);
	isosurfaces[3]->SetTranslucent(translucent);
}


void VTKPipeline::GetBackgroundColor(double rgb[3]) {
    renderer->GetBackground(rgb);
}

void VTKPipeline::SetBackgroundColor(double red, double green, double blue) {
    renderer->SetBackground(red, green, blue);
}


VTKPipeline::ColorMapType VTKPipeline::GetColorMapType() {
    return colorMapType;
}

void VTKPipeline::SetColorMapType(ColorMapType type) {
    colorMapType = type;

    SetColorMap();
}


bool VTKPipeline::GetShowAxes() {
    return axes->GetVisibility() == 1;
}

void VTKPipeline::SetShowAxes(bool show) {
    axes->SetVisibility(show);
}


bool VTKPipeline::GetShowColorLegend() {
    return colorLegend->GetVisibility() == 1;
}

void VTKPipeline::SetShowColorLegend(bool show) {
    colorLegend->SetVisibility(show);
}


bool VTKPipeline::GetShowDataLabel() {
    return dataLabel->GetVisibility() == 1;
}

void VTKPipeline::SetShowDataLabel(bool show) {
    dataLabel->SetVisibility(show);
}


double VTKPipeline::GetInteractiveDataMagnification() {
    return shrinker->GetMagnificationFactors()[0];
}

void VTKPipeline::SetInteractiveDataMagnification(double magnification) {
    shrinker->SetMagnificationFactors(magnification, magnification, magnification);
}


void VTKPipeline::GetDataRange(double range[2]) {
    range[0] = dataRange[0];
    range[1] = dataRange[1];
}

double VTKPipeline::GetMaximumAbsoluteValue() {
    return std::max(fabs(dataRange[0]), dataRange[1]);
}


void VTKPipeline::Render() {
    interactor->Render();
}


void VTKPipeline::SetUseStereo(bool useStereo) {
    interactor->GetRenderWindow()->SetStereoRender(useStereo);
}


void VTKPipeline::SetHeadPosition(double x, double y, double z) {
    renderer->GetActiveCamera()->GetPosition(cameraPosition);

    double scale = 50;

    renderer->GetActiveCamera()->SetPosition(cameraPosition[0] + x * scale,
                                             cameraPosition[1] + y * scale,
                                             cameraPosition[2] + z * scale);

    renderer->ResetCameraClippingRange();
    
    Render();
    
    renderer->GetActiveCamera()->SetPosition(cameraPosition);
}

void VTKPipeline::FlipEyes() {
    renderer->GetActiveCamera()->SetEyeAngle(-renderer->GetActiveCamera()->GetEyeAngle());
}


void VTKPipeline::SetColorMap() {
    switch (colorMapType) {
        case Color:
            SetColorMapToColor();
            SetIsosurfacesToColor();
    
            // Set logo
            logo->VisibilityOn();
            bwLogo->VisibilityOff();

            break;

        case Grayscale:
            SetColorMapToGrayscale();
            SetIsosurfacesToGrayscale();

            // Set logo
            logo->VisibilityOff();
            bwLogo->VisibilityOn();

            break;

        default:
            std::cout << "VTKPipeline::SetColorMap() : Unknown color map type, defaulting to Color" << std::endl;
            colorMapType = Color;

            SetColorMap();
    }
}

void VTKPipeline::SetColorMapToColor() {
    colorMap->RemoveAllPoints();

    if (dataRange[0] < 0.0 && dataRange[1] > 0.0) {
        // Positive and negative values
        double small = std::min(fabs(dataRange[0]), dataRange[1]);
        double big = std::max(fabs(dataRange[0]), dataRange[1]);

        if (fabs(dataRange[0]) > fabs(dataRange[1])) {
            // Larger negative values
            colorMap->AddRGBPoint(-big, 1.0, 1.0, 1.0);
            colorMap->AddRGBPoint(-small - (big - small) * 0.5, 0.0, 1.0, 1.0);
            colorMap->AddRGBPoint(-small, 0.0, 0.0, 1.0);
            colorMap->AddRGBPoint(0.0, 0.75, 0.75, 0.75);
            colorMap->AddRGBPoint(small, 1.0, 0.0, 0.0);
        }
        else {
            // Larger positive values
            colorMap->AddRGBPoint(-small, 0.0, 0.0, 1.0);
            colorMap->AddRGBPoint(0.0, 0.75, 0.75, 0.75);
            colorMap->AddRGBPoint(small, 1.0, 0.0, 0.0);
            colorMap->AddRGBPoint(small + (big - small) * 0.5, 1.0, 1.0, 0.0);
            colorMap->AddRGBPoint(big, 1.0, 1.0, 1.0);
        }
    }
    else {
        if (dataRange[1] < 0.0) {
            // Only negative values
            double big = dataRange[0];
            colorMap->AddRGBPoint(0.0, 0.75, 0.75, 0.75);
            colorMap->AddRGBPoint(big / 3.0, 0.0, 0.0, 1.0);
            colorMap->AddRGBPoint(big * 2.0 / 3.0, 0.0, 1.0, 1.0);
            colorMap->AddRGBPoint(big, 1.0, 1.0, 1.0);
        }
        else {
            // Only positive values
            double big = dataRange[1];           
            colorMap->AddRGBPoint(0.0, 0.75, 0.75, 0.75);
            colorMap->AddRGBPoint(big / 3.0, 1.0, 0.0, 0.0);
            colorMap->AddRGBPoint(big * 2.0 / 3.0, 1.0, 1.0, 0.0);
            colorMap->AddRGBPoint(big, 1.0, 1.0, 1.0);
        }
    }
}

void VTKPipeline::SetColorMapToGrayscale() {
    colorMap->RemoveAllPoints();

    if (dataRange[0] < 0.0 && dataRange[1] > 0.0) {
        // Positive and negative values
        double small = std::min(fabs(dataRange[0]), dataRange[1]);
        double big = std::max(fabs(dataRange[0]), dataRange[1]);

        if (fabs(dataRange[0]) > fabs(dataRange[1])) {
            // Larger negative values
            double smallValue = 0.5 + 0.5 * small / big;
            colorMap->AddRGBPoint(-big, 0.0, 0.0, 0.0);
            colorMap->AddRGBPoint(0.0, 0.5, 0.5, 0.5);
            colorMap->AddRGBPoint(small, smallValue, smallValue, smallValue);
        }
        else {
            // Larger positive values
            double smallValue = 0.5 - 0.5 * small / big;
            colorMap->AddRGBPoint(-small, smallValue, smallValue, smallValue);
            colorMap->AddRGBPoint(0.0, 0.5, 0.5, 0.5);
            colorMap->AddRGBPoint(big, 1.0, 1.0, 1.0);
        }
    }
    else {
        if (dataRange[1] < 0.0) {
            // Only negative values
            double big = dataRange[0];
            colorMap->AddRGBPoint(0.0, 0.5, 0.5, 0.5);
            colorMap->AddRGBPoint(big, 0.0, 0.0, 0.0);
        }
        else {
            // Only positive values
            double big = dataRange[1];           
            colorMap->AddRGBPoint(0.0, 0.5, 0.5, 0.5);
            colorMap->AddRGBPoint(big, 1.0, 1.0, 1.0);
        }
    }
}


void VTKPipeline::SetIsosurfacesToColor() {
    for (int i = 0; i < (int)isosurfaces.size(); i++) {
        vtkProperty* p = isosurfaces[i]->GetActor()->GetProperty();
        if (isosurfaces[i]->GetIsosurface()->GetValue(0) < 0.0) { 
            p->SetAmbientColor(0.0, 0.0, 1.0);
            p->SetDiffuseColor(0.0, 0.0, 1.0);
        }
        else {            
            p->SetAmbientColor(1.0, 0.0, 0.0);
            p->SetDiffuseColor(1.0, 0.0, 0.0);
        }
    }
}

void VTKPipeline::SetIsosurfacesToGrayscale() {
    for (int i = 0; i < (int)isosurfaces.size(); i++) {
        vtkProperty* p = isosurfaces[i]->GetActor()->GetProperty();
        if (isosurfaces[i]->GetIsosurface()->GetValue(0) < 0.0) { 
            p->SetAmbientColor(0.25, 0.25, 0.25);
            p->SetDiffuseColor(0.25, 0.25, 0.25);
        }
        else {            
            p->SetAmbientColor(0.75, 0.75, 0.75);
            p->SetDiffuseColor(0.75, 0.75, 0.75);
        }
    }
}