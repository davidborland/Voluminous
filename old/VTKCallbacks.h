///////////////////////////////////////////////////////////////////////////////////////////////
//
// Name:        VTKCallbacks.h
//
// Author:      David Borland
//
// Description: Classes derived from vtkCommand to handle interaction events in Voluminous.
//
///////////////////////////////////////////////////////////////////////////////////////////////


#ifndef VTKCALLBACKS_H
#define VTKCALLBACKS_H


#include <vtkCommand.h>


class KeyPressCallback : public vtkCommand {
public:
    KeyPressCallback();
    static KeyPressCallback* New();

    virtual void Execute(vtkObject* caller, unsigned long eventId, void* callData);
};


#endif