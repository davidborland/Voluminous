/*=========================================================================

  Name:        Voluminous.cpp

  Author:      David Borland

  Copyright:   The Renaissance Computing Institute (RENCI)

  Description: Contains the main function for the Voluminous program.  
               Just instantiates a QApplication object and the 
			   application main window using Qt.

=========================================================================*/


#include "MainWindow.h"

#include <QApplication>


int main(int argc, char** argv) {
    // Initialize Qt
    QApplication app(argc, argv);

    MainWindow mainWindow(argc, argv);
    mainWindow.setWindowIcon(QIcon(":/icon"));
    mainWindow.show();

    return app.exec();
}