/*=========================================================================

  Name:        AboutDialog.cpp

  Author:      David Borland, The Renaissance Computing Institute (RENCI)

  Copyright:   The Renaissance Computing Institute (RENCI)

  Description: About dialog for the Voluminous visualization program

=========================================================================*/


#include "AboutDialog.h"

#include <QResource>
#include <QScrollBar>

AboutDialog::AboutDialog(QWidget* parent) : QDialog(parent) {
    // Create the GUI from the Qt Designer file
    setupUi(this);

    // Don't allow resizing
    layout()->setSizeConstraint(QLayout::SetFixedSize);


    // Create image
    QIcon icon = windowIcon();
    QSize size = icon.actualSize(QSize(64, 64));
    iconLabel->setPixmap(icon.pixmap(size));


    // Header text
    const char* header = 
        "Voluminous v1.0<br>"
        "<br>"
        "Volume visualization tool built with VTK and Qt";

    headerLabel->setTextFormat(Qt::RichText);
    headerLabel->setText(header);


    // Information
    const char* info = 
        "<table><tbody>"
        "<tr><td>Author:</td><td>David Borland</td></tr>"
        "<tr><td>Organization:<td>The Renaissance Computing Institute (RENCI)</td></tr>"
        "<tr><td></td><td>The University of North Carolina at Chapel Hill</td></tr>"
        "<tr><td>Contact:</td><td><a href=\"mailto:borland@renci.org\?subject=Voluminous inquiry\">borland@renci.org</a><td></tr>"
        "</tbody></table>";

    infoLabel->setTextFormat(Qt::RichText);
    infoLabel->setText(info);


    // License
    licenseLabel->setText(reinterpret_cast<const char*>(QResource(":/license").data()));

    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setMinimumWidth(licenseLabel->minimumSizeHint().width() + 
                                scrollArea->verticalScrollBar()->width());

    // RENCI url
    urlLabel->setTextFormat(Qt::RichText);
    urlLabel->setText("<a href='http://www.renci.org'>www.renci.org</a>");
}

AboutDialog::~AboutDialog() {
}

void AboutDialog::on_okButton_clicked() {
    close();
}