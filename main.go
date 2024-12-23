package main

/*
#cgo LDFLAGS: -L. -lstatus_fetcher
#include <stdlib.h>

// Define the structure from the C code
typedef struct {
    long *status_codes;
    int size;
} StatusArray;

// Declare the function from the C code
extern StatusArray get_status_codes(int num_threads, const char *url);
*/
import "C"
import (
    "fmt"
    "unsafe"
)

func main() {
    numThreads := 5
    url := "https://www.google.com"

    // Convert the Go string to a C string
    cURL := C.CString(url)
    defer C.free(unsafe.Pointer(cURL))

    // Call the C function with the number of threads and the URL
    result := C.get_status_codes(C.int(numThreads), cURL)

    // Convert C array to Go slice
    statusCodes := (*[1 << 30]C.long)(unsafe.Pointer(result.status_codes))[:result.size:result.size]

    // Print the status codes
    for i, code := range statusCodes {
        fmt.Printf("Thread %d: HTTP Status Code: %d\n", i, code)
    }

    // Free the memory allocated for the status codes array in C
    C.free(unsafe.Pointer(result.status_codes))
}
