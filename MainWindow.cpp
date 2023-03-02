/*=========================================================================

  Name:        MainWindow.cpp

  Author:      David Borland

  Copyright:   The Renaissance Computing Institute (RENCI)

  Description: The main window for the Voluminous visualization program

=========================================================================*/


#include "MainWindow.h"

#include <QApplication>
#include <QtConcurrentRun>
#include <QColorDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QProgressDialog>
#include <QResource>
#include <QTimer>

#include <vtkQImageToImageSource.h>
#include <vtkRenderWindow.h>

#include <QDoubleSlider.h>

#include "AboutDialog.h"
#include "VTKPipeline.h"

#include <vrpn_Tracker.h>


static void VRPN_CALLBACK HandlePosition(void* userData, const vrpn_TRACKERCB t) {
//    printf("%f %f %f\n", t.pos[0], t.pos[1], t.pos[2]);

    VTKPipeline* pipeline = static_cast<MainWindow*>(userData)->GetVTKPipeline();
    
    if (!pipeline) {
        return;
    }

    if (t.sensor == 0) {
        pipeline->SetHeadPosition(t.pos[0], t.pos[1], t.pos[2]);
    }
}


MainWindow::MainWindow(int argc, char** argv) : QMainWindow(NULL) {
    // Create the GUI from the Qt Designer file
    setupUi(this);


    // Make stereo options unavailable, unless a stereo window is requested
    menuStereo->setEnabled(false);

    // No tracker by default
    tracker = NULL;


    // Disable the GUI until we have data
    tabWidget->setEnabled(false);


    // Hide the advanced controls for now
    tabWidget->removeTab(2);

    
    // Set up double sliders
    isovalue1DoubleSlider = new QDoubleSlider(isovalue1Slider, isovalue1SpinBox, this);
    connect(isovalue1DoubleSlider, SIGNAL(valueChanged(double)), this, SLOT(isovalue1DoubleSlider_valueChanged(double)));
    connect(isovalue1DoubleSlider, SIGNAL(sliderReleased()), this, SLOT(isovalue1DoubleSlider_sliderReleased()));

    isovalue2DoubleSlider = new QDoubleSlider(isovalue2Slider, isovalue2SpinBox, this);
    connect(isovalue2DoubleSlider, SIGNAL(valueChanged(double)), this, SLOT(isovalue2DoubleSlider_valueChanged(double)));
    connect(isovalue2DoubleSlider, SIGNAL(sliderReleased()), this, SLOT(isovalue2DoubleSlider_sliderReleased()));

    isovalueExponentDoubleSlider = new QDoubleSlider(isovalueExponentSlider, isovalueExponentSpinBox, this);
    connect(isovalueExponentDoubleSlider, SIGNAL(valueChanged(double)), isovalue1DoubleSlider, SLOT(setExponent(double)));
    connect(isovalueExponentDoubleSlider, SIGNAL(valueChanged(double)), isovalue2DoubleSlider, SLOT(setExponent(double)));


    isovalue1DualValue->setUpdateSeparately(true);


    connect(isovalue1ExploratorySlider, SIGNAL(valueChanged(double)), testIsovalue1SpinBox, SLOT(setValue(double)));
    connect(testIsovalue1SpinBox, SIGNAL(valueChanged(double)), isovalue1ExploratorySlider, SLOT(setValue(double)));
    

    // Create the visualization pipeline
    pipeline = NULL;
    CreatePipeline();


    // Parse command-line options
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-ActiveStereo") == 0) {
            // Try to create a stereo-capable window
            vtkRenderWindow* win = qvtkWidget->GetRenderWindow();
            win->StereoCapableWindowOn();
            win->StereoRenderOn();
        }
        else if (strcmp(argv[i], "-zSpace") == 0) {
            // Try to create a stereo-capable window
            vtkRenderWindow* win = qvtkWidget->GetRenderWindow();
            win->StereoCapableWindowOn();
            win->StereoRenderOn();

            // Create the tracker
            tracker = new vrpn_Tracker_Remote("Tracker0@localhost");
            if (tracker->register_change_handler(this, HandlePosition) == -1) {
                printf("Error: Can't register tracker callback.");
            }
            else {
                // Create a timer for updating tracking
                QTimer* timer = new QTimer(this);
                connect(timer, SIGNAL(timeout()), this, SLOT(trackingTimer()));
                timer->start(0);
            }
        }
    }

    // See if stereo is available    
    if (qvtkWidget->GetRenderWindow()->GetStereoCapableWindow()) {
        menuStereo->setEnabled(true);
        actionUseStereo->setChecked(true);
    }
}

MainWindow::~MainWindow() {
    // Clean up
    if (tracker) {
        delete tracker;
        tracker = NULL;
    }

    delete pipeline;
    pipeline = NULL;
}


VTKPipeline* MainWindow::GetVTKPipeline() {
    return pipeline;
}


///////////////////////////////////////////////////////////////////////////
// Respond to menu events

void MainWindow::on_actionOpenVolume_triggered() {
    // Open a file dialog to read the VTK file
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    "Open Volume",
                                                    "",
                                                    "All Files (*);;Legacy VTK Files (*.vtk);;VTK XML ImageData Files (.vti)");

    // Check for file name
    if (fileName == "") {
        return;
    }

        
    // Disable the GUI until we have data
    tabWidget->setEnabled(false);
    

    // Create a new pipeline
    CreatePipeline();


    // Clear the screen
    pipeline->Render();
    

    // ProgressBar.   
    // Doing things this way because vtkStructuredPointsReader would only give one update at 50% and then 
    // no updates until the volume was loaded, making it look frozen.  By loading the data in a thread 
    // with a continually animated progress bar, it doesn't look like application is frozen.
    // This code was adapted from:  http://qt-project.org/wiki/Progress-bar
    QFutureWatcher<bool> futureWatcher;
    QProgressDialog progressDialog("Opening " + fileName.right(fileName.length() - fileName.lastIndexOf("/") - 1), "Cancel", 0, 0, this);

    connect(&futureWatcher, SIGNAL(finished()), this, SLOT(openVolumeFinished()));
    connect(&futureWatcher, SIGNAL(finished()), &progressDialog , SLOT(cancel()));

    future = QtConcurrent::run(pipeline, &VTKPipeline::OpenVolume, fileName.toStdString(), &errorMessage);
    futureWatcher.setFuture(future);
    
    progressDialog.setWindowModality(Qt::WindowModal);
    progressDialog.exec();
}


void MainWindow::openVolumeFinished() {
    if (!future.result()) {
        // Show error message
        QMessageBox::critical(this, "Error", errorMessage.c_str());

        return;
    }

    if (!pipeline->CreateVisualization(errorMessage)) {
        QMessageBox::critical(this, "Error", errorMessage.c_str());

        return;
    }
    
    RefreshGUI();
}




void MainWindow::on_actionSaveScreenshot_triggered() {
    // Open a file dialog to save the PNG image
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    "Save Screenshot",
                                                    "VoluminousScreenshot.png",
                                                    "PNG Files (*.png)");

    // Check for file name
    if (fileName == "") {
        return;
    }

    // Save a screenshot
    pipeline->SaveScreenshot(fileName.toStdString());
}

void MainWindow::on_actionExit_triggered() {
    qApp->exit();
}


void MainWindow::on_actionUseStereo_triggered() {
    pipeline->SetUseStereo(actionUseStereo->isChecked());
}

void MainWindow::on_actionFlipEyes_triggered() {
    pipeline->FlipEyes();
}


void MainWindow::on_actionAbout_triggered() {
    AboutDialog about(this);
    about.exec();
}

void MainWindow::on_actionControls_triggered() {
    const char* message = 
        "--Mouse--\n"
        "Left button + drag: Rotate\n"
        "Middle button + drag: Pan\n"
        "Right button + drag: Zoom\n"
        "Wheel: Zoom\n"
        "\n"
        "--IsovalueControls--\n"
        "Change the two isosurface values and visibility with the associated sliders and check boxes.\n"
        "\n"
        "The Isovalue Exponent slider changes the output of the isovalue sliders to be based on a curve with the given exponent.\n"
        "An exponent of 1 results in a standard linear slider.\n"
        "\n"
        "--Settings--\n"
        "Set the background color and turn visualization elements on and off with the associated button and check boxes.\n"
        "\n"
        "The Grayscale radio button changes the color map such that it will be more suitable for printing in black and white.\n"
        "\n"
        "The Interactive Data Resolution slider changes the amount of downsampling applied to the volume when moving the isovalue sliders.\n"
        "Sliding to the left provides faster rendering but with a less-accurate volume.\n"
        "Sliding to the right provides slower rendering but with a more-accurate volume.";

    QMessageBox::about(this, "Controls", message);
}


///////////////////////////////////////////////////////////////////////////
// Timer events

void MainWindow::trackingTimer() {
    if (tracker) {
        tracker->mainloop();
    }
}


///////////////////////////////////////////////////////////////////////////
// Respond to widget events

void MainWindow::isovalue1DoubleSlider_valueChanged(double value) {
    pipeline->SetIsovalue1(value, true);   
    pipeline->Render();
}

void MainWindow::isovalue1DoubleSlider_sliderReleased() {
    pipeline->SetIsovalue1(isovalue1DoubleSlider->value());   
    pipeline->Render();
}


void MainWindow::isovalue2DoubleSlider_valueChanged(double value) {
    pipeline->SetIsovalue2(value, true);
    pipeline->Render();
}

void MainWindow::isovalue2DoubleSlider_sliderReleased() {
    pipeline->SetIsovalue2(isovalue2DoubleSlider->value());   
    pipeline->Render();
}


void MainWindow::on_isovalue1VisibleCheckBox_toggled(bool checked) {
	pipeline->SetIsovalue1Visible(checked);
    pipeline->Render();
}

void MainWindow::on_isovalue1TranslucentCheckBox_toggled(bool checked) {
	pipeline->SetIsovalue1Translucent(checked);

    pipeline->Render();
}
	
void MainWindow::on_isovalue2VisibleCheckBox_toggled(bool checked) {
	pipeline->SetIsovalue2Visible(checked);
    pipeline->Render();
}

void MainWindow::on_isovalue2TranslucentCheckBox_toggled(bool checked) {
	pipeline->SetIsovalue2Translucent(checked);
    pipeline->Render();
}


void MainWindow::on_isovalue1DualValue_value1Changed(double value) {
    double v1 = isovalue1DualValue->getValue1();
    double v2 = isovalue1DualValue->getValue2();

    double v = v1 * pow(10.0, v2);

    pipeline->SetIsovalue1(v, true);

    pipeline->Render();
}

void MainWindow::on_isovalue1DualValue_bar1Released() {
    double v1 = isovalue1DualValue->getValue1();
    double v2 = isovalue1DualValue->getValue2();

    double v = v1 * pow(10.0, v2);

    pipeline->SetIsovalue1(v);

    pipeline->Render();   
}

void MainWindow::on_isovalue1DualValue_value2Changed(double value) {
    double v1 = isovalue1DualValue->getValue1();
    double v2 = isovalue1DualValue->getValue2();

    double v = v1 * pow(10.0, v2);

    pipeline->SetIsovalue1(v, true);

    pipeline->Render();
}

void MainWindow::on_isovalue1DualValue_bar2Released() {
    double v1 = isovalue1DualValue->getValue1();
    double v2 = isovalue1DualValue->getValue2();

    double v = v1 * pow(10.0, v2);

    pipeline->SetIsovalue1(v);

    pipeline->Render();   
}

void MainWindow::on_isovalue1DualValue_valuesChanged(QPointF values) {
    double v1 = isovalue1DualValue->getValue1();
    double v2 = isovalue1DualValue->getValue2();

    double v = v1 * pow(10.0, v2);

    pipeline->SetIsovalue1(v, true);

    pipeline->Render();

/*
    if (values.y() > values.x() * 0.8) {
        isovalue1DualValue->setValues(values.x(), values.x() * 0.8);
    }
    else {
        pipeline->SetIsovalue1(values.x());
        pipeline->SetIsovalue2(values.y());

        printf("%f %f\n", values.x(), values.y());

        pipeline->Render();
    }
*/
}

void MainWindow::on_isovalue1DualValue_handleReleased() {
    double v1 = isovalue1DualValue->getValue1();
    double v2 = isovalue1DualValue->getValue2();

    double v = v1 * pow(10.0, v2);

    pipeline->SetIsovalue1(v);

    pipeline->Render();   
}

void MainWindow::on_isovalue1ExploratorySlider_valueChanged(double value) {
    pipeline->SetIsovalue1(value, true);

    pipeline->Render();
}

void MainWindow::on_isovalue1ExploratorySlider_sliderReleased() {
    pipeline->SetIsovalue1(isovalue1ExploratorySlider->getValue());

    pipeline->Render();
}


void MainWindow::on_setBackgroundColorButton_clicked() {
    // Open a file dialog to read the VTK XML file
    double rgb[3];
    pipeline->GetBackgroundColor(rgb);
    QColor color;
    color.setRgbF(rgb[0], rgb[1], rgb[2]);
    color = QColorDialog::getColor(color, this, "Choose Background Color");

    if (color.isValid()) {
        pipeline->SetBackgroundColor(color.redF(), color.greenF(), color.blueF());
        pipeline->Render();
    }
}


void MainWindow::on_colorRadioButton_toggled(bool checked) {
    if (checked) {
        pipeline->SetColorMapType(VTKPipeline::Color);
        pipeline->Render();
    }
}

void MainWindow::on_grayscaleRadioButton_toggled(bool checked) {
    if (checked) {
        pipeline->SetColorMapType(VTKPipeline::Grayscale);
        pipeline->Render();
    }
}


void MainWindow::on_showAxesCheckBox_toggled(bool checked) {
    pipeline->SetShowAxes(checked);
    pipeline->Render();
}

void MainWindow::on_showColorLegendCheckBox_toggled(bool checked) {
    pipeline->SetShowColorLegend(checked);
    pipeline->Render();
}

void MainWindow::on_showDataLabelCheckBox_toggled(bool checked) {
    pipeline->SetShowDataLabel(checked);
    pipeline->Render();
}


void MainWindow::on_interactiveDataResolutionSlider_valueChanged(int value)
{
    pipeline->SetInteractiveDataMagnification((double)value / 10);
}


void MainWindow::CreatePipeline() {
    // Load the RENCI logos
    QImage logoImage(":/logo");
    vtkSmartPointer<vtkQImageToImageSource> logo = vtkSmartPointer<vtkQImageToImageSource>::New();
    logo->SetQImage(&logoImage);
    logo->Update();

    QImage bwLogoImage(":/logo_bw");
    vtkSmartPointer<vtkQImageToImageSource> bwLogo = vtkSmartPointer<vtkQImageToImageSource>::New();
    bwLogo->SetQImage(&bwLogoImage);
    bwLogo->Update();
    

    // Clean up the old visualization pipeline and create a new one
    if (pipeline) {
        delete pipeline;
    }

    pipeline = new VTKPipeline(qvtkWidget->GetInteractor(), 
                               logo->GetOutput(), bwLogo->GetOutput(),
                               reinterpret_cast<const char*>(QResource(":/opaqueShader").data()), 
                               reinterpret_cast<const char*>(QResource(":/translucentShader").data()));
}


void MainWindow::RefreshGUI() {
    // Find the maximum absolute value of the data
    double maxValue = pipeline->GetMaximumAbsoluteValue();


    isovalue1DualValue->blockSignals(true);
    isovalue1DualValue->setValue1Range(1.0, 10.0);
    isovalue1DualValue->setValue2Range(-10.0, 1.0);

    isovalue1DualValue->setValue1(pipeline->GetIsovalue1() * 10.0);
    isovalue1DualValue->setValue2(-2.0);
    isovalue1DualValue->blockSignals(false);



    isovalue1ExploratorySlider->blockSignals(true);
    isovalue1ExploratorySlider->setRange(0.0, maxValue);
    isovalue1ExploratorySlider->setValue(pipeline->GetIsovalue1());
    isovalue1ExploratorySlider->blockSignals(false);

/*
    isovalue1DualValue->blockSignals(true);
    isovalue1DualValue->setValue1Range(0.0, maxValue);
    isovalue1DualValue->setValue2Range(0.0, maxValue);

    isovalue1DualValue->setValue1(pipeline->GetIsovalue1());
    isovalue1DualValue->setValue2(pipeline->GetIsovalue2());
    isovalue1DualValue->blockSignals(false);
*/


    // Set the spin boxes based on the maximum absolute value.
    // Have to block signals here when changing the range because otherwise, if the maxValue is greater than the 
    // current range of the spin box, the spin box value will get set to maxValue, causing 
    // the pipeline isovalue to also be set to maxValue.
    isovalue1SpinBox->blockSignals(true);
    isovalue2SpinBox->blockSignals(true);

    isovalue1SpinBox->setRange(0.0, maxValue);
    isovalue1SpinBox->setValue(pipeline->GetIsovalue1());
    
    isovalue2SpinBox->setRange(0.0, maxValue);
    isovalue2SpinBox->setValue(pipeline->GetIsovalue2());    

    isovalue1SpinBox->blockSignals(false); 
    isovalue2SpinBox->blockSignals(false);

    
    isovalue1DoubleSlider->blockSignals(true);
    isovalue2DoubleSlider->blockSignals(true);

    isovalueExponentSpinBox->setValue(10); 

    isovalue1DoubleSlider->blockSignals(false);
    isovalue2DoubleSlider->blockSignals(false);


    // Set the isovalue visibility checkboxes
    isovalue1VisibleCheckBox->setChecked(pipeline->GetIsovalue1Visible());
    isovalue1TranslucentCheckBox->setChecked(pipeline->GetIsovalue1Translucent());
    isovalue2VisibleCheckBox->setChecked(pipeline->GetIsovalue2Visible());
    isovalue2TranslucentCheckBox->setChecked(pipeline->GetIsovalue2Translucent());


    // Set other widgets
    colorRadioButton->setChecked(pipeline->GetColorMapType() == VTKPipeline::Color);
    grayscaleRadioButton->setChecked(pipeline->GetColorMapType() == VTKPipeline::Grayscale);

    showAxesCheckBox->setChecked(pipeline->GetShowAxes());
    showColorLegendCheckBox->setChecked(pipeline->GetShowColorLegend());
    showDataLabelCheckBox->setChecked(pipeline->GetShowDataLabel());

    interactiveDataResolutionSlider->setValue(pipeline->GetInteractiveDataMagnification() * 10);

    // Enable the GUI
    tabWidget->setEnabled(true);
}