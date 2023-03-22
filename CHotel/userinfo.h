#ifndef USERINFO_H
#define USERINFO_H
#include <yyjson.h>
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

void free_userinfo_node(UserInfo *user);
void free_userinfo_list(UserInfo *head);
void delete_userinfo_node(UserInfo **head, int id);
void append_userinfo_node(UserInfo *head, yyjson_val *val);
void print_userinfo_list(UserInfo *head);
UserInfo *create_userinfo_node(yyjson_val *val);

#endif /* USERINFO_H */
