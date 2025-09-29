#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <yivo/yivo.h>

#define PORT        8080
#define BUFFER_SIZE 1024
#define MAX_MSG_SIZE 32

typedef struct YMSG {
  int a, b;
} test_t;


int main() {
  int sockfd;
  uint8_t buffer[BUFFER_SIZE];
  // char *message = "hello";
  struct sockaddr_in server_addr, client_addr;
  socklen_t client_len = sizeof(client_addr);
  struct timeval tv;
  // int client_known = 0; // Flag to track if client address is known

  ypkt_t *ym = ypkt_create(10, sizeof(test_t));
  ypars_t *pars = ypars_create(MAX_MSG_SIZE);

  // Create UDP socket
  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0) {
    perror("Socket creation failed");
    exit(EXIT_FAILURE);
  }

  // Set 1-second timeout for receive operations
  tv.tv_sec  = 1;
  tv.tv_usec = 0;
  if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
    perror("Failed to set timeout");
    close(sockfd);
    exit(EXIT_FAILURE);
  }

  // Configure server address
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family      = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port        = htons(PORT);

  // Bind socket
  if (bind(sockfd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
    perror("Bind failed");
    close(sockfd);
    exit(EXIT_FAILURE);
  }

  printf("Server listening on port %d with 1-second timeout...\n", PORT);

  // ypkt_t *ym = ypkt_create(100, sizeof(test_t));
  // ypars_t *pars = ypars_create(16);

  while (1) {
    
    // Try to receive a message to get client address
    int n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &client_len);
    if (n >= 0) {
      buffer[n] = '\0';
      printf("Received bytes: %d\n", n);
      // client_known = 1; // Mark client address as known
    } else if (errno == EAGAIN || errno == EWOULDBLOCK) {
      printf("Waiting for client message (timeout after 1 second)\n");
      continue;
    } else {
      perror("Receive failed");
      continue;
    }

    uint32_t i = 0;
    uint8_t msgid = ypars_buffer(pars, buffer, BUFFER_SIZE, &i);

    if (msgid == ym->msg_id) {
      test_t msg;
      ypkt_unpack(ym, &msg, sizeof(test_t));
      printf("> msg: %d %d\n", msg.a, msg.b);

      msg.a += 10;
      msg.b += 50;

      ypkt_pack(ym, (uint8_t*)&msg, sizeof(test_t));

      if (sendto(sockfd, ym->data, ym->size, 0, (struct sockaddr *)&client_addr, client_len) >= 0) {
        printf("Sent: %u\n", ym->size);
      } else {
        perror("Send failed");
      }
    }
  
  }

  close(sockfd);
  return 0;
}