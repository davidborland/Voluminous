///////////////////////////////////////////////////////////////////////////////////////////////
//
// Name:        vtkMultiTouchInterface.h
//
// Author:      David Borland
//
// Description: Processes Jason Coposky's multi-touch events
//
///////////////////////////////////////////////////////////////////////////////////////////////


#ifndef __vtkMultiTouchInterface_h
#define __vtkMultiTouchInterface_h

#include "vtkObject.h"

#ifdef WIN32
# include "vtkWindows.h"
# include "winsock.h"
#endif

//#include "vtkClientSocket.h"
#include "vtkCommand.h"
#include "vtkStdString.h"
#include <vtkstd/vector>


// Structure defining a touch point.  Gestures can have multiple touch points.
struct TouchPoint {
    int Id;
    double Location[2];
    double Direction[2];
    int MoveLocation;
};


class VTK_RENDERING_EXPORT vtkMultiTouchInterface : public vtkObject
{
public:
  static vtkMultiTouchInterface *New();

  vtkTypeRevisionMacro(vtkMultiTouchInterface,vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Set socket information
  vtkGetStringMacro(HostName);
  vtkSetStringMacro(HostName);
  vtkGetMacro(Port,int);
  vtkSetMacro(Port,int);

  // Description:
  // Initialize the socket communication
  virtual void Initialize();

  // Description:
  // Process messages from the socket.  Returns 1 if there is a new event, 0 otherwise.
  int ProcessSocket();

  // Description:
  // Get methods for gesture data
  const vtkStdString& GetGestureName();
  const vtkstd::vector<TouchPoint>& GetTouchPoints();

  // Description:
  // Event Ids for multi-touch events
  enum MultiTouchEventIds {
      MultiTouchOneTouchEvent = vtkCommand::UserEvent,
      MultiTouchOneDragEvent,
      MultiTouchTwoTouchEvent,
      MultiTouchTwoDragEvent,
      MultiTouchThreeTouchEvent,
      MultiTouchThreeDragEvent,
      MultiTouchFourTouchEvent,
      MultiTouchFourDragEvent,
      MultiTouchFiveTouchEvent,
      MultiTouchFiveDragEvent,
      MultiTouchSixTouchEvent,
      MultiTouchSixDragEvent,
      MultiTouchZoomEvent,
      MultiTouchTranslateXEvent,
      MultiTouchTranslateYEvent,
      MultiTouchTranslateZEvent,
      MultiTouchRotateXEvent,
      MultiTouchRotateYEvent,
      MultiTouchRotateZEvent,
      MultiTouchReleaseEvent
  };

protected:
  vtkMultiTouchInterface();
  ~vtkMultiTouchInterface();

  // Description:
  // The socket being read
  char* HostName;
  int Port;
  int SocketDescriptor;
  int Connected;

  // Description:
  // Parse the buffer
  void ParseBuffer(char* buffer, int numBytes);


  // Description:
  // Hold the gesture information
  vtkStdString GestureName;
  vtkstd::vector<TouchPoint> TouchPoints;
  void ClearGesture();

  // Description:
  // Socket code.  vtkSocket currently uses TCP, so leave this code in here for now.
  int CreateSocket();
  int Receive(void* data, int length);
  int ReadInt(char** buffer);
  double ReadDouble(char** buffer);
  double Ntohd(double d);

private:  
  vtkMultiTouchInterface(const vtkMultiTouchInterface&);  // Not implemented.
  void operator=(const vtkMultiTouchInterface&);  // Not implemented.
};

#endif
