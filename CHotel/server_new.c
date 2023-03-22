// Server side C/C++ program to demonstrate Socket
// programming
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h> // inet_addr()
#include <netdb.h>
#include <strings.h> // bzero()
#include <unistd.h> // read(), write(), close()
#include "yyjson.h"
#include "yyjson.c"
//#include "userinfo.h"
#include "log.h"
#include "log.c"
//#include "userinfo.c"

#define MAX 80
#define PORT 8080
#define CHECK_JSON_STR(s) s ? yyjson_get_str(s) : "\0"


typedef struct UserInfo UserInfo;
struct UserInfo {
	int id;
	char *user;
	char *password;
	char *admin;
	char *purse;
	char *phone_number;
	char *address;
	UserInfo *next;
};

UserInfo *create_userinfo_node(yyjson_val *val);
void free_userinfo_node(UserInfo *user);
void free_userinfo_list(UserInfo *head);
void delete_userinfo_node(UserInfo **head, int id);
void append_userinfo_node(UserInfo *head, yyjson_val *val);
void print_userinfo_list(UserInfo *head);
int check_userinfo_exist(UserInfo *head, char *user);
char* get_userinfo_password(UserInfo *head, char *user);
int last_userinfo_id(UserInfo *head);




int main(int argc, char const* argv[])
{
	//Read JSON file, allowing comments and trailing commas
	yyjson_read_flag flg = YYJSON_READ_ALLOW_COMMENTS
				| YYJSON_READ_ALLOW_TRAILING_COMMAS;
	yyjson_read_err err;
	yyjson_doc *doc = yyjson_read_file("./Config/UsersInfo.json",
					   flg, NULL, &err);
	yyjson_val *obj = yyjson_doc_get_root(doc);
	UserInfo *head = NULL, *list_iter = NULL;
	// Iterate over the root object
	yyjson_val *arr = yyjson_obj_get(obj, "users");
	yyjson_arr_iter iter;
	yyjson_arr_iter_init(arr, &iter);
	yyjson_val *val;
	if (doc) {
		while ((val = yyjson_arr_iter_next(&iter))) {
			if (!head) {
				head = create_userinfo_node(val);
				list_iter = head;
			} else {
				head->next = create_userinfo_node(val);
				head = head->next;
			}
		}
		head = list_iter;
	} else {
	    printf("read error (%u): %s at position: %ld\n", err.code, err.msg, err.pos);
	}
	print_userinfo_list(head);
	printf("Last ID: %d\n", last_userinfo_id(head));
	printf("Morad Password: %s\n", get_userinfo_password(head, "Morad"));
	int server_fd, new_socket, addrlen, client_input, opt = 1;
	struct sockaddr_in server_address, client_address;
	struct UserInfo new_user;
	addrlen = sizeof(server_address);
	char client_buffer[1024] = { 0 };
	// Creating socket file descriptor
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket failed");
		exit(EXIT_FAILURE);
	}
	// Forcefully attaching socket to the port 8080
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
		       &opt, sizeof(opt))) {
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = inet_addr("0.0.0.0");
	server_address.sin_port = htons(8080);
	if (bind(server_fd, (struct sockaddr*)&server_address,
		 sizeof(server_address)) < 0) {
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	if (listen(server_fd, 3) < 0) {
		perror("listen");
		exit(EXIT_FAILURE);
	}
	while (1) {
		if ((new_socket = accept(server_fd, (struct sockaddr*)&server_address, (socklen_t*)&addrlen)) < 0) {
			perror("accept");
			exit(EXIT_FAILURE);
		}
		printf("Server: got connection from %s port %d\n",
		       inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));
		client_input = read(new_socket, client_buffer, 1024);
		printf("Client chose: %s\n", client_buffer);

		if (strcmp(client_buffer, "1") == 0) {
			bzero(client_buffer, sizeof(client_buffer));
			printf("Username: ");
			if ((client_input = read(new_socket, client_buffer, 1024)) < 0){
				perror("Sign In Error: 430\n");
				//send()
			}

			if (check_userinfo_exist(head, client_buffer)) {
				printf("User %s exists.", client_buffer);
				//perror("Sign Up Error: 451\n");
				//send()
				char *sign_in_password = get_userinfo_password(head, client_buffer);
				bzero(client_buffer, sizeof(client_buffer));
				//printf("Password: %s\n", sign_in_password);
				if ((client_input = read(new_socket, client_buffer, 1024)) < 0){
					perror("Sign In Error: 430\n");
					//send()
				}
				//printf("Password2: %s\n", client_buffer);
				if (strcmp(sign_in_password, client_buffer) == 0) {
					printf("Login Confirmed");
				} else {
					perror("Sign In Error: 431");
				}
			} else {
				perror("Sign In Error: 431");
			}
		} else if (strcmp(client_buffer, "2") == 0) {
			bzero(client_buffer, sizeof(client_buffer));
			if ((client_input = read(new_socket, client_buffer, 1024)) < 0) {
				perror("Sign Up Error: 503\n");
				//send()
			}
			if (check_userinfo_exist(head, client_buffer)) {
				printf("User %s exists.", client_buffer);
				perror("Sign Up Error: 451\n");
				printf("User %s exists.", client_buffer);
				//send()
			} else {
				printf("Sign Up Message: 311\n");
				printf("Username: %s\n", client_buffer);
				//send()
				bzero(client_buffer, sizeof(client_buffer));
				if ((client_input = read(new_socket, client_buffer, 1024)) < 0) {
					perror("Sign Up Error: 503\n");
					//send()
				}
				printf("Password: %s\n", client_buffer);
				bzero(client_buffer, sizeof(client_buffer));
				if ((client_input = read(new_socket, client_buffer, 1024)) < 0) {
					perror("Sign Up Error: 503\n");
					//send()
				}
				printf("Purse: %s\n", client_buffer);
				bzero(client_buffer, sizeof(client_buffer));
				if ((client_input = read(new_socket, client_buffer, 1024)) < 0) {
					perror("Sign Up Error: 503\n");
					//send()
				}
				printf("Phone Number: %s\n", client_buffer);
				bzero(client_buffer, sizeof(client_buffer));
				if ((client_input = read(new_socket, client_buffer, 1024)) < 0) {
					perror("Sign Up Error: 503\n");
					//send()
				}
				printf("Address: %s\n", client_buffer);
				printf("Sign Up Message: 231\n");
				//send()
			}
		} else {
			/* default: */
			printf("invalid");
		}
	}
	//Free the doc
	free_userinfo_list(head);
	yyjson_doc_free(doc);
	//Close the socket
	close(new_socket);
	shutdown(server_fd, SHUT_RDWR);
	return 0;
}

UserInfo *create_userinfo_node(yyjson_val *val)
{
	UserInfo *node = malloc(sizeof(UserInfo));
	if (!node) {
		log_warn("Can't allocate memory for userinfo node");
		return NULL;
	}
	yyjson_val *tmp = yyjson_obj_get(val, "id");
	node->id = yyjson_get_int(tmp);
	tmp = yyjson_obj_get(val, "user");
	node->user = strdup(CHECK_JSON_STR(tmp));
	tmp = yyjson_obj_get(val, "password");
	node->password = strdup(CHECK_JSON_STR(tmp));
	tmp = yyjson_obj_get(val, "admin");
	node->admin = strdup(CHECK_JSON_STR(tmp));
	tmp = yyjson_obj_get(val, "purse");
	node->purse = strdup(CHECK_JSON_STR(tmp));
	tmp = yyjson_obj_get(val, "phoneNumber");
	node->phone_number = strdup(CHECK_JSON_STR(tmp));
	tmp = yyjson_obj_get(val, "address");
	node->address = strdup(CHECK_JSON_STR(tmp));
	return node;
}

void free_userinfo_node(UserInfo *user)
{
	free(user->user);
	free(user->password);
	free(user->admin);
	free(user->purse);
	free(user->phone_number);
	free(user->address);
	free(user);
}

void free_userinfo_list(UserInfo *head)
{
	if (!head)
		return;
	free_userinfo_list(head->next);
	free_userinfo_node(head);
}

void append_userinfo_node(UserInfo *head, yyjson_val *val)
{
	while (head->next) {
		head = head->next;
	}
	head->next = create_userinfo_node(val);
}

void delete_userinfo_node(UserInfo **head, int id)
{
	UserInfo **indirect = head, *tmp = NULL;
	while ((*indirect)->id != id) {
		indirect = &(*indirect)->next;
	}

	tmp = *indirect;
	if ((*head)->id != id)
		*indirect = (*indirect)->next;
	else
		*head = (*indirect)->next;
	free_userinfo_node(tmp);
}

void print_userinfo_list(UserInfo *head)
{
	if (!head)
		return;
	do {
		printf("%d ", head->id);
	} while((head = head->next));
	puts("\n");
}

int check_userinfo_exist(UserInfo *head, char *user)
{
	if (!head)
		return 0;
	while (head != NULL) {
		if (strcmp(head->user, user) == 0)
			return 1;
		head = head->next;
	}
	return 0;
}

char* get_userinfo_password(UserInfo *head, char *user)
{
	if (!head)
		return 0;
	while (head != NULL) {
		if (strcmp(head->user, user) == 0)
			return head->password;
		head = head->next;
	}
}

int last_userinfo_id(UserInfo *head)
{
	int id;
	if (!head)
		return -1;
	while (head != NULL) {
		id = head->id;
		head = head->next;
	}
	return id;
}

//With OS Commands
int check_user_exist(char *username)
{
	int size, idx = 0;
	char *usernames[1024], buffer[1024];
	FILE *process = popen("grep '\"user\":' ./Config/UsersInfo.json | awk '{print $2}' | cut -d '\"' -f2", "r");
	while (fgets(buffer, sizeof(buffer), process) != NULL) {
		char *p = strchr(buffer, '\n');
		if (p) *p = '\0';
		usernames[idx] = strdup(buffer);
		if (strcmp(usernames[idx], username) == 0) {
			return 1;
		}
		++idx;
	}
	pclose(process);
	return 0;
}
