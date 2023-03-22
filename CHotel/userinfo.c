#include <stdio.h>
#include <stdlib.h>
#include "userinfo.h"
#include "log.h"

/*
   			"id": 1,
			"user": "Morad",
			"password": "1234",
			"admin": "false",
			"purse": "1000",
			"phoneNumber": "00945692356",
			"address": "Canada, Montral"

*/
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

int main (int argc, char *argv[])
{
	// Read JSON file, allowing comments and trailing commas
	yyjson_read_flag flg = YYJSON_READ_ALLOW_COMMENTS
				| YYJSON_READ_ALLOW_TRAILING_COMMAS;
	yyjson_read_err err;
	yyjson_doc *doc = yyjson_read_file("./Config/UsersInfo.json",
					   flg, NULL, &err);
	UserInfo *head = NULL, *list_iter = NULL;

	// Iterate over the root object
	yyjson_val *arr = yyjson_obj_get(obj, "users");


	yyjson_arr_iter iter;
	yyjson_arr_iter_init(arr, &iter);


	yyjson_val *val;



	while ((val = yyjson_arr_iter_next(&iter))) {
	    //printf("%s: %s\n", yyjson_get_str(val), yyjson_get_type_desc(val));
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
/*
   			"id": 1,
			"user": "Morad",
			"password": "1234",
			"admin": "false",
			"purse": "1000",
			"phoneNumber": "00945692356",
			"address": "Canada, Montral"

*/

	//struct UserInfo new_user;
	//new_user.id = 5;
	//new_user.user = "borapocan";
	//new_user.password = "p4ss";
	//new_user.admin = "false";
	//new_user->user = "borapocan";
	//printf("%d", new_user.id);
	//printf("%s", new_user.user);
	//printf("%s", new_user.password);

	//print_userinfo_list(head);
	//delete_userinfo_node(&head, 1);
	//print_userinfo_list(head);
	//append_userinfo_node()
      char *json = yyjson_write(doc, YYJSON_WRITE_PRETTY, NULL);
      puts(json);

	// Free the doc
      free_userinfo_list(head);
      //yyjson_doc_free(idoc);
      yyjson_doc_free(doc);
	return 0;
}
