#define WINDOWS_LEAN_AND_MEAN 1
#include <windows.h>
#include <stdint.h>

int SysInfo(int *ncpu,int64_t *mem,char **hostnm)
{
  SYSTEM_INFO si;
  MEMORYSTATUSEX msx;
  DWORD nSize;

  GetSystemInfo(&si);
  *ncpu = si.dwNumberOfProcessors;

  msx.dwLength = sizeof(msx);
  GlobalMemoryStatusEx(&msx);
  *mem = msx.ullTotalPhys/(1<<20);

  GetComputerNameEx(ComputerNameNetBIOS,NULL,&nSize);
  *hostnm = malloc(nSize);
  GetComputerNameEx(ComputerNameDnsFullyQualified,*hostnm,&nSize);
  return 0;
}
