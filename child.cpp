#include <windows.h>
#include <iostream>
#include <string>

const std::string SHMN = "Local\\ThSHM";

int main()
{
  HANDLE shm = OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE, SHMN.c_str());

  if (!shm)
  {
    std::cerr << "OpenFileMappingA failed, error = " << GetLastError() << '\n';
    return 1;
  }

  char* ptr = static_cast< char* >(MapViewOfFile(shm, FILE_MAP_ALL_ACCESS, 0, 0, 0));
  if (!ptr)
  {
    std::cerr << "MapViewOfFile failed, error = " << GetLastError() << '\n';

    CloseHandle(shm);

    return 1;
  }

  if (ptr[0] == '\0')
  {
    std::cout << "Child received: <empty string>" << std::endl;
  }
  else
  {
    std::cout << "Child received: " << ptr << std::endl;
  }

  if (!UnmapViewOfFile(ptr))
  {
    std::cerr << "UnmapViewOfFile failed, error = " << GetLastError() << '\n';

    CloseHandle(shm);

    return 1;
  }

  CloseHandle(shm);
  return 0;
}
