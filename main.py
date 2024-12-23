import ctypes
from ctypes import POINTER, c_int, c_char_p, c_long

# Load the shared library
lib = ctypes.CDLL('./status_fetcher.so')

# Define the return type and argument types for the C function
class StatusArray(ctypes.Structure):
    _fields_ = [("status_codes", POINTER(c_long)),
                ("size", c_int)]

lib.get_status_codes.restype = StatusArray
lib.get_status_codes.argtypes = [c_int, c_char_p]

def get_status_codes(num_threads, url):
    """
    Calls the C function to get status codes for the given URL using the specified number of threads.
    
    Args:
        num_threads (int): The number of threads to use.
        url (str): The URL to send requests to.
    
    Returns:
        list: A list of HTTP status codes returned from the requests.
    """
    # Convert Python strings to C-compatible strings
    url_c = c_char_p(url.encode('utf-8'))
    
    # Call the C function
    result = lib.get_status_codes(num_threads, url_c)
    
    # Extract the status codes
    status_codes = [result.status_codes[i] for i in range(result.size)]
    
    # Free the allocated memory for status codes in C
    lib.free(result.status_codes)
    
    return status_codes

if __name__ == '__main__':
    # Example usage
    num_threads = 5
    url = "https://www.google.com"
    
    print(f"Sending requests to {url} with {num_threads} threads...")
    status_codes = get_status_codes(num_threads, url)
    print("Status codes:", status_codes)
