#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <curl/curl.h>
#include <string.h>

// Structure to pass status codes and size back to Go
typedef struct {
    long *status_codes;
    int size;
} StatusArray;

typedef struct {
    long *status_codes;
    int index;
    pthread_mutex_t *mutex;
    const char *url;
} ThreadData;

void *make_request(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    CURL *curl;
    CURLcode res;
    long http_code = 0;

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, data->url);
        
        // Disable verbose output from libcurl
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);

        // Optionally redirect errors to /dev/null to suppress further logs
        FILE *devnull = fopen("/dev/null", "w+");
        if (devnull) {
            curl_easy_setopt(curl, CURLOPT_STDERR, devnull);
        }

        res = curl_easy_perform(curl);
        if (res == CURLE_OK) {
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
            pthread_mutex_lock(data->mutex);
            data->status_codes[data->index] = http_code;
            pthread_mutex_unlock(data->mutex);
        } else {
            fprintf(stderr, "Request failed: %s\n", curl_easy_strerror(res));
        }

        if (devnull) {
            fclose(devnull);
        }
        
        curl_easy_cleanup(curl);
    }
    free(data);
    return NULL;
}

StatusArray make_requests_with_threads(int num_threads, const char *url) {
    pthread_t threads[num_threads];
    long *status_codes = malloc(num_threads * sizeof(long));
    pthread_mutex_t mutex;
    StatusArray result = {status_codes, num_threads};

    pthread_mutex_init(&mutex, NULL);
    for (int i = 0; i < num_threads; i++) {
        ThreadData *data = malloc(sizeof(ThreadData));
        data->status_codes = status_codes;
        data->index = i;
        data->mutex = &mutex;
        data->url = url;

        if (pthread_create(&threads[i], NULL, make_request, data) != 0) {
            perror("Failed to create thread");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&mutex);
    return result;
}

// Extern function that Go will call
StatusArray get_status_codes(int num_threads, const char *url) {
    curl_global_init(CURL_GLOBAL_ALL);
    StatusArray result = make_requests_with_threads(num_threads, url);
    curl_global_cleanup();
    return result;
}
