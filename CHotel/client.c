#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <yyjson.h>
#include <sys/socket.h>
#include <unistd.h>
#define PORT 8080


// Send the data to the server and set the timeout of 20 seconds
int SocketSend(int hSocket,char* Rqst,short lenRqst)
{
    int shortRetval = -1;
    struct timeval tv;
    tv.tv_sec = 20;  /* 20 Secs Timeout */
    tv.tv_usec = 0;
    if(setsockopt(hSocket,SOL_SOCKET,SO_SNDTIMEO,(char *)&tv,sizeof(tv)) < 0) {
        printf("Time Out\n");
        return -1;
    }
    shortRetval = send(hSocket, Rqst, lenRqst, 0);
    return shortRetval;
}

int main(int argc, char const* argv[])
{
	int status, client_fd;
	struct sockaddr_in serv_addr;
	char client_buffer[1024] = { 0 };
	if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("\n Socket creation error \n");
		return -1;
	}
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);
	// Convert IPv4 and IPv6 addresses from text to binary form
	if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)
		<= 0) {
		printf(
		"\nInvalid address/ Address not supported \n");
		return -1;
	}
	if ((status = connect(client_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr))) < 0) {
		printf("\nConnection Failed \n");
		return -1;
	}
	printf("Sucessfully conected with server\n");
	printf("CLIENT MENU:\n1 - Sign In\n2 - Sign Up\n");
	printf("Select an option: ");
	gets(client_buffer);
	send(client_fd, client_buffer, strlen(client_buffer), 0);

	if (strcmp(client_buffer, "1") == 0) {
		printf("Option 1 selected\n");
		bzero(client_buffer, sizeof(client_buffer));
		printf("Username: ");
		gets(client_buffer);
		send(client_fd, client_buffer, strlen(client_buffer), 0);
		bzero(client_buffer, sizeof(client_buffer));
		printf("Password: ");
		gets(client_buffer);
		send(client_fd, client_buffer, strlen(client_buffer), 0);
	} else if (strcmp(client_buffer, "2") == 0) {
		printf("Option 2 selected\n");
		bzero(client_buffer, sizeof(client_buffer));
		printf("Username: ");
		gets(client_buffer);
		send(client_fd, client_buffer, strlen(client_buffer), 0);
		bzero(client_buffer, sizeof(client_buffer));
		printf("Password: ");
		gets(client_buffer);
		send(client_fd, client_buffer, strlen(client_buffer), 0);
		//bzero(client_buffer, sizeof(client_buffer));
		//printf("Admin: ");
		//gets(client_buffer);
		//send(client_fd, client_buffer, strlen(client_buffer), 0);
		bzero(client_buffer, sizeof(client_buffer));
		printf("Purse: ");
		gets(client_buffer);
		send(client_fd, client_buffer, strlen(client_buffer), 0);
		bzero(client_buffer, sizeof(client_buffer));
		printf("Phone Number: ");
		gets(client_buffer);
		send(client_fd, client_buffer, strlen(client_buffer), 0);
		bzero(client_buffer, sizeof(client_buffer));
		printf("Address: ");
		gets(client_buffer);
		send(client_fd, client_buffer, strlen(client_buffer), 0);

	} else {
		printf("Invalid option\n");
	}
    // closing the connected socket
    close(client_fd);
    return 0;
}
