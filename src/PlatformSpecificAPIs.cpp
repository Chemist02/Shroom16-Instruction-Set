// For interrupting std::cin
#if defined(_WIN32)
    #include <windows.h>
#elif defined(__unix__) || defined(__APPLE__) || defined(_POSIX_VERSION)
    #include <unistd.h>
#else
    #error "Unknown or unsupported operating system"
#endif

void closeStdIn() {
#if defined(_WIN32)
	CancelIoEx(GetStdHandle(STD_INPUT_HANDLE), NULL);
#elif defined(__unix__) || defined(__APPLE__) || defined(_POSIX_VERSION)
	close(0);
#endif
}
