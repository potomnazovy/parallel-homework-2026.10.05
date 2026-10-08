#include <windows.h>
#include <iostream>
#include <string>
#include <cstring>

const std::string SHMN = "Local\\ThSHM";

int main(int argc, char** argv)
{
  if (argc != 2)
  {
    std::cerr << "Invalid input" << '\n';
    return 1;
  }

  std::string line;
  if (!std::getline(std::cin, line))
  {
    std::cerr << "getline error: failed to read line" << '\n';
    return 1;
  }

  SIZE_T total = line.size() + 1;

  HANDLE shm = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, static_cast< DWORD >(total), SHNM.c_str());
  if (!shm)
  {
    std::cerr << "CreateFileMapping failed, error = " << GetLastError() << '\n';
    return 1;
  }

  char* ptr = static_cast< char* >(MapViewOfFile(shm, FILE_MAP_ALL_ACCESS, 0, 0, total));
  if (!ptr)
  {
    std::cerr << "MapViewOfFile failed, error = " << GetLastError() << '\n';

    CloseHandle(shm);

    return 1;
  }

  std::memcpy(ptr, line.c_str(), total);

  if (!UnmapViewOfFile(ptr))
  {
    std::cerr << "UnmapViewOfFile failed, error = " << GetLastError() << '\n';

    CloseHandle(shm);

    return 1;
  }

  PROCESS_INFORMATION pi = {};
  STARTUPINFOA si  = {.cb = sizeof(si)};

  if (!CreateProcessA(argv[1], argv[1], NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi))
  {
    std::cerr << "CreateProcess failed, error = " << GetLastError() << "\n";

    CloseHandle(shm);

    return 1;
  }

  DWORD w = WaitForSingleObject(pi.hProcess, INFINITE);
  if (w == WAIT_FAILED)
  {
    std::cerr << "WaitForSingleObject failed, error = " << GetLastError() << '\n';

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    CloseHandle(shm);

    return 1;
  }

  DWORD exitCode = 0;
  if (!GetExitCodeProcess(pi.hProcess, &exitCode))
  {
    std::cerr << "GetExitCodeProcess failed, error = " << GetLastError() << '\n';
  }
  else if (exitCode != 0)
  {
    std::cerr << "Child exited with code " << exitCode << '\n';
  }
  else
  {
    std::cout << "Child finished successfully\n";
  }

  CloseHandle(pi.hProcess);
  CloseHandle(pi.hThread);
  CloseHandle(shm);

  return 0;
}
