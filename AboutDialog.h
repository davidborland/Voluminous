/*=========================================================================

  Name:        AboutDialog.h

  Author:      David Borland, The Renaissance Computing Institute (RENCI)

  Copyright:   The Renaissance Computing Institute (RENCI)

  Description: About dialog for the Voluminous visualization program

=========================================================================*/


#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H


#include <QDialog>

#include "ui_AboutDialog.h"


class AboutDialog : public QDialog, private Ui_AboutDialog {
    Q_OBJECT

public:
    // Constructor/destructor
    AboutDialog(QWidget* parent = NULL);
    virtual ~AboutDialog();

public slots:
    // Use Qt's auto-connect magic to tie GUI widgets to slots,
    // removing the need to call connect() explicitly.
    // Names of the methods must follow the naming convention
    // on_<widget name>_<signal name>(<signal parameters>).
    virtual void on_okButton_clicked();
};


#endif
