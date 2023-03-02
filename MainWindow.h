/*=========================================================================

  Name:        MainWindow.h

  Author:      David Borland

  Copyright:   The Renaissance Computing Institute (RENCI)

  Description: The main window for the Voluminous visualization program

=========================================================================*/


#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QMainWindow>

#include "ui_MainWindow.h"

#include <QFuture>
#include <QFutureWatcher>

class QProgressDialog;

class QDoubleSlider;
class VTKPipeline;

class vrpn_Tracker_Remote;


class MainWindow : public QMainWindow, private Ui_MainWindow {
    Q_OBJECT

public:
    // Constructor/destructor
    MainWindow(int argc, char** argv);
    virtual ~MainWindow();

    VTKPipeline* GetVTKPipeline();

public slots:
    // For double sliders
    virtual void isovalue1DoubleSlider_valueChanged(double value);
    virtual void isovalue1DoubleSlider_sliderReleased();

    virtual void isovalue2DoubleSlider_valueChanged(double value);
    virtual void isovalue2DoubleSlider_sliderReleased();


    // Slot to receive signal when volume has been loaded
    virtual void openVolumeFinished();


    // Timer for tracking
    virtual void trackingTimer();


    // Use Qt's auto-connect magic to tie GUI widgets to slots,
    // removing the need to call connect() explicitly.
    // Names of the methods must follow the naming convention
    // on_<widget name>_<signal name>(<signal parameters>).

    // Menu events
    virtual void on_actionOpenVolume_triggered();
    virtual void on_actionSaveScreenshot_triggered();
    virtual void on_actionExit_triggered();

    virtual void on_actionUseStereo_triggered();
    virtual void on_actionFlipEyes_triggered();

    virtual void on_actionAbout_triggered();
    virtual void on_actionControls_triggered();

    // Widget events
	virtual void on_isovalue1VisibleCheckBox_toggled(bool checked);
	virtual void on_isovalue1TranslucentCheckBox_toggled(bool checked);
	virtual void on_isovalue2VisibleCheckBox_toggled(bool checked);
	virtual void on_isovalue2TranslucentCheckBox_toggled(bool checked);

    virtual void on_isovalue1DualValue_value1Changed(double value);
    virtual void on_isovalue1DualValue_bar1Released();
    virtual void on_isovalue1DualValue_value2Changed(double value);
    virtual void on_isovalue1DualValue_bar2Released();
    virtual void on_isovalue1DualValue_valuesChanged(QPointF values);
    virtual void on_isovalue1DualValue_handleReleased();

virtual void on_isovalue1ExploratorySlider_valueChanged(double value);
virtual void on_isovalue1ExploratorySlider_sliderReleased();

    virtual void on_setBackgroundColorButton_clicked();

    virtual void on_colorRadioButton_toggled(bool checked);
    virtual void on_grayscaleRadioButton_toggled(bool checked);

    virtual void on_showAxesCheckBox_toggled(bool checked);
    virtual void on_showColorLegendCheckBox_toggled(bool checked);
    virtual void on_showDataLabelCheckBox_toggled(bool checked);

    virtual void on_interactiveDataResolutionSlider_valueChanged(int value);

protected:
    // The visualization pipeline object
	VTKPipeline* pipeline;


    // Double sliders to combine sliders and spin boxes
    QDoubleSlider* isovalue1DoubleSlider;
    QDoubleSlider* isovalue2DoubleSlider;

    QDoubleSlider* isovalueExponentDoubleSlider;

    QDoubleSlider* isovalue1OpacityDoubleSlider;
    QDoubleSlider* isovalue2OpacityDoubleSlider; 


    // Progress bar objects
    QFuture<bool> future;

    std::string errorMessage;


    // Create the VTK pipeline object
    void CreatePipeline();

    // Set GUI widget values from the VTK pipeline
    void RefreshGUI();


    // VRPN
    vrpn_Tracker_Remote* tracker;
};


#endif