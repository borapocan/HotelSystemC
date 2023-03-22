#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#define MAX_DATA_SIZE 1024

typedef struct {
    char* hostName;
    int commandChannelPort;
} ServerConfig;

typedef struct {
    int id;
    char* user;
    char* password;
    bool admin;
    char* purse;
    char* phoneNumber;
    char* address;
} UserInfo;

typedef struct {
    UserInfo** users;
    int count;
} UsersInfo;

// Global variables
ServerConfig* config;
UsersInfo* users_info;

// Function prototypes
ServerConfig* loadServerConfig();
void freeServerConfig(ServerConfig* config);
UsersInfo* loadUsersInfo();
void freeUsersInfo(UsersInfo* info);

char* strndup(const char* s, size_t n)
{
    char* p = strchr(s, '\0');
    size_t len = p - s;
    if (len > n)
        len = n;
    p = malloc(len + 1);
    if (p) {
        memcpy(p, s, len);
        p[len] = '\0';
    }
    return p;
}

char* file_read(const char* filepath)
{
    FILE* file = fopen(filepath, "rb");
    if (file == NULL) {
        printf("Could not open file \"%s\".", filepath);
        exit(74);
    }

    fseek(file, 0L, SEEK_END);
    size_t fileSize = ftell(file);
    rewind(file);

    char* buffer = (char*)malloc(fileSize + 1);
    if (buffer == NULL) {
        printf("Not enough memory to read \"%s\".", filepath);
        exit(74);
    }

    size_t bytesRead = fread(buffer, sizeof(char), fileSize, file);
    if (bytesRead < fileSize) {
        printf("Could not read the \"%s\".", filepath);
        exit(74);
    }

    buffer[bytesRead] = '\0';

    fclose(file);
    return buffer;
}

int jsoneq(const char* json, const jsmntok_t* tok, const char* str)
{
    if (tok->type == JSMN_STRING && (int)strlen(str) == tok->end - tok->start && strncmp(json + tok->start, str, tok->end - tok->start) == 0) {
        return 0;
    } else {
        return -1;
    }
}

ServerConfig* loadServerConfig()
{
    const char* config_filename = "Config/base.json";
    char* config_data = file_read(config_filename);

    ServerConfig* serverconfig = malloc(sizeof(ServerConfig));

    jsmn_parser parser;
    jsmn_init(&parser);

    jsmntok_t tokens[MAX_DATA_SIZE];
    int num_tokens = jsmn_parse(&parser, config_data, strlen(config_data), tokens, MAX_DATA_SIZE);

    if (num_tokens < 1 || tokens[0].type != JSMN_OBJECT) {
        fprintf(stderr, "Invalid JSON data.\n");
        return NULL;
    }

    int i;
    for (i = 1; i < num_tokens; i++) {
        if (jsoneq(config_data, &tokens[i], "hostName") == 0) {
            serverconfig->hostName = strndup(config_data + tokens[i + 1].start, tokens[i + 1].end - tokens[i + 1].start);
            i++;
        } else if (jsoneq(config_data, &tokens[i], "commandChannelPort") == 0) {
            serverconfig->commandChannelPort = atoi(config_data + tokens[i + 1].start);
            i++;
        }
    }

    free(config_data);
    return serverconfig;
}

void freeServerConfig(ServerConfig* serverconfig)
{
    free(serverconfig->hostName);
    free(serverconfig);
}

UsersInfo* loadUsersInfo()
{
    const char* users_filename = "Config/UsersInfo.json";
    char* users_data = file_read(users_filename);

    UsersInfo* usersinfo = malloc(sizeof(UsersInfo));

    jsmn_parser parser;
    jsmn_init(&parser);

    jsmntok_t tokens[MAX_DATA_SIZE];
    int num_tokens = jsmn_parse(&parser, users_data, strlen(users_data), tokens, MAX_DATA_SIZE);

    if (num_tokens < 1 || tokens[0].type != JSMN_OBJECT) {
        fprintf(stderr, "Invalid JSON data.\n");
        return NULL;
    }

    int users_count = 0;
    int i;
    for (i = 1; i < num_tokens; i++) {
        for (int j = 0; j < tokens[i].size; j++) {
            printf("token[%d][%d] type: %d, value: %.*s\n", i, j, tokens[i + j + 1].type, tokens[i + j + 2].end - tokens[i + j + 2].start, users_data + tokens[i + j + 2].start);
        }

        if (jsoneq(users_data, &tokens[i], "users") == 0 && tokens[i + 1].type == JSMN_ARRAY) {
            users_count = tokens[i + 1].size;
            usersinfo->users = malloc(users_count * sizeof(UserInfo*));
            int k = 0;
            for (int j = 0; j < tokens[i + 1].size; j++) {
                jsmntok_t* user_token = &tokens[i + j + 2];
                if (user_token->type == JSMN_OBJECT) {
                    UserInfo* user = malloc(sizeof(UserInfo));
                    for (int l = 0; l < user_token->size; l++) {
                        jsmntok_t* user_info_token = &tokens[i + j + l + 3];
                        if (jsoneq(users_data, user_info_token, "id") == 0) {
                            user->id = atoi(
                                strndup(users_data + user_info_token[1].start, user_info_token[1].end - user_info_token[1].start));
                        } else if (jsoneq(users_data, user_info_token, "user") == 0) {
                            user->user = strndup(users_data + user_info_token[1].start, user_info_token[1].end - user_info_token[1].start);
                        }
                        // Add additional else if statements for other user info fields
                    }
                    usersinfo->users[k] = user;
                    k++;
                }
            }
        }
    }

    usersinfo->count = users_count;
    free(users_data);
    return usersinfo;
}

void freeUsersInfo(UsersInfo* usersinfo)
{
    int i;
    for (i = 0; i < usersinfo->count; i++) {
        free(usersinfo->users[i]->user);
        free(usersinfo->users[i]->password);
        free(usersinfo->users[i]->purse);
        free(usersinfo->users[i]->phoneNumber);
        free(usersinfo->users[i]->address);
    }

    free(usersinfo->users);
    free(usersinfo);
}

int main(int argc, char** argv)
{
    config = loadServerConfig();
    printf("Host: %s\n", config->hostName);
    printf("Port: %d\n", config->commandChannelPort);

    users_info = loadUsersInfo();
    printf("Number of users: %d\n", users_info->count);
    for (int i = 0; i < users_info->count; i++) {
        UserInfo* user = users_info->users[i];
        printf("User: %d -> %s\n", user->id, user->user);
    }

    // freeServerConfig(config);
    // freeUsersInfo(users_info);

    return 0;
}
