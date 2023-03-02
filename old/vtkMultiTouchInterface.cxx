///////////////////////////////////////////////////////////////////////////////////////////////
//
// Name:        vtkMultiTouchInterface.cxx
//
// Author:      David Borland
//
// Description: Processes Jason Coposky's multi-touch events
//
///////////////////////////////////////////////////////////////////////////////////////////////


#include "vtkMultiTouchInterface.h"

#include "vtkObjectFactory.h"

vtkCxxRevisionMacro(vtkMultiTouchInterface, "$Revision: 1.1 $");
vtkStandardNewMacro(vtkMultiTouchInterface);


//----------------------------------------------------------------------------
vtkMultiTouchInterface::vtkMultiTouchInterface()
{
  this->HostName = NULL;
  this->Port = -1;
  this->Connected = 0;
}

//----------------------------------------------------------------------------
vtkMultiTouchInterface::~vtkMultiTouchInterface()
{
  this->SetHostName(NULL);
  closesocket(this->SocketDescriptor);
}

//----------------------------------------------------------------------------
void vtkMultiTouchInterface::Initialize() 
{
  // Initialize the socket library
#ifdef WIN32
  WORD versionRequested;
  WSADATA wsaData;

  // Use the MAKEWORD(lowbyte, highbyte) macro declared in Windef.h
  versionRequested = MAKEWORD(2, 2);

  if (WSAStartup(versionRequested, &wsaData) != 0) 
    {
    // Tell the user that we could not find a usable Winsock DLL
    vtkErrorMacro(<<"WSAStartup failed!");
    return;
    }
#else
  vtkErrorMacro(<<"Multi-touch currently only implemented on 32-bit Windows");
#endif

  // Check that we have a server to connect to
  if (HostName == NULL || Port == -1)
    {
    vtkErrorMacro(<<"Host name and/or port not set!");
    return;
    } 
  if (this->CreateSocket() != 0)
    {
    vtkErrorMacro(<<"Could not create socket!");
    return;
    }
  this->Connected = 1;
}

//----------------------------------------------------------------------------  
const vtkStdString& vtkMultiTouchInterface::GetGestureName()
{
  return this->GestureName;
}

//----------------------------------------------------------------------------  
const vtkstd::vector<TouchPoint>& vtkMultiTouchInterface::GetTouchPoints()
{
  return this->TouchPoints;
}

//----------------------------------------------------------------------------
int vtkMultiTouchInterface::ProcessSocket()
{
    // Read from the socket
    const int bufferSize = 16384;   // Magic number, taken from OSC MAX_UDP_PACKET_SIZE
    char buffer[bufferSize];
    memset(buffer, '\0', bufferSize);

    int numBytes = this->Receive(buffer, bufferSize);

    if (numBytes <= 0 || numBytes > bufferSize)
      {
      return 0;
      }

    // Tokenize
    this->ClearGesture();
    this->ParseBuffer(buffer, numBytes);

    return 1;
}

//----------------------------------------------------------------------------
void vtkMultiTouchInterface::ParseBuffer(char* buffer, int numBytes)
{
  // Create a vtkStdString to make parsing certain things easier
  vtkStdString s(buffer, numBytes);

  // Flag for the end of osc header junk
  const vtkStdString start_flag = "set";

  // Find our start position  
  unsigned int si = 0;
  while (si < s.size() - start_flag.size() && (s.substr(si, start_flag.size())).compare(start_flag) != 0)
    {
    si++;
    }

  // Skip past start_flag
  si += (int)start_flag.size() + 1;

  vtkStdString str = s.substr(si);

  // Find the gesture type
  this->GestureName = str.substr(0, str.find_first_of('\0'));

  if (this->GestureName == "release")
    {
    // No more info.  Return.
    return;
    }

  // Skip to next 4 byte chunk after the gesture name (including '\0')
  int chunkDist = 4 - ((int)this->GestureName.size() + 1) % 4;
  if (chunkDist == 4) chunkDist = 0;
  char* bufPtr = buffer + si + this->GestureName.size() + 1 + chunkDist;

  // Get the number of touches
  int numTouches = this->ReadInt(&bufPtr);

  for (int i = 0; i < numTouches; i++) 
    {
    TouchPoint tp;

    // Fill in the touch point
    tp.Id = this->ReadInt(&bufPtr);
    tp.Location[0] = this->ReadDouble(&bufPtr);
    tp.Location[1] = this->ReadDouble(&bufPtr);
    tp.Direction[0] = this->ReadDouble(&bufPtr);
    tp.Direction[1] = this->ReadDouble(&bufPtr);
    tp.MoveLocation = this->ReadInt(&bufPtr);

    this->TouchPoints.push_back(tp);
    }
}

//----------------------------------------------------------------------------
int vtkMultiTouchInterface::ReadInt(char** buffer)
{
  char intBuffer[4];
  memcpy(intBuffer, *buffer, 4);

  *buffer += 4;

  return ntohl(*((int*)intBuffer));
}

//----------------------------------------------------------------------------
double vtkMultiTouchInterface::ReadDouble(char** buffer)
{
  char doubleBuffer[8];
  memcpy(doubleBuffer, *buffer, 8);

  *buffer += 8;

  return this->Ntohd(*((double*)doubleBuffer));
}

//----------------------------------------------------------------------------
double vtkMultiTouchInterface::Ntohd(double d) 
{
  double swapped;
  char* chSwapped = (char*)&swapped;
  char* chD = (char*)&d;

  for (int i = 0; i < sizeof(double); i++) 
    {
    chSwapped[i] = chD[sizeof(double) - i - 1];
    }

  return swapped;
}

//----------------------------------------------------------------------------
void vtkMultiTouchInterface::ClearGesture() 
{
  this->GestureName = "";
  this->TouchPoints.clear();
}

//----------------------------------------------------------------------------
int vtkMultiTouchInterface::CreateSocket()
{
  // Create a UDP socket
  this->SocketDescriptor = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (this->SocketDescriptor == INVALID_SOCKET)
    {
    vtkErrorMacro(<<"Could not create socket!");
    return -1;
    }

  // Make non-blocking
  u_long blocking = 1;
  if (ioctlsocket(this->SocketDescriptor, FIONBIO, &blocking) == SOCKET_ERROR)
    {
    vtkErrorMacro(<<"Could not set non-blocking mode!");
    closesocket(this->SocketDescriptor);
    return -1;
    }

  // Set up the server information
  struct sockaddr_in server = {0};
  server.sin_family = AF_INET;
  server.sin_port = htons(this->Port);
  server.sin_addr.s_addr = INADDR_ANY;

  // Bind the address to the socket
  if (bind(this->SocketDescriptor, reinterpret_cast<struct sockaddr*>(&server), sizeof(sockaddr)) == SOCKET_ERROR)
    {
    vtkErrorMacro(<<"Could not bind name to socket!");
    closesocket(this->SocketDescriptor);
    return -1;
    }

  return 0;
}

//----------------------------------------------------------------------------
int vtkMultiTouchInterface::Receive(void* data, int length)
{
  return recvfrom(this->SocketDescriptor, (char*)data, length, 0, 0, 0);
}

//----------------------------------------------------------------------------
void vtkMultiTouchInterface::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  os << indent << "HostName: " << this->HostName << endl;
  os << indent << "Port: " << this->Port << endl;
}