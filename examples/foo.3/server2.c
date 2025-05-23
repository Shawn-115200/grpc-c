#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include "foo.grpc-c.h"


static grpc_c_server_t *test_server;

int test_times = 0;

/*
 * This function gets invoked whenever say_hello RPC gets called
 */
void foo__greeter__sigle_stream_cb(grpc_c_context_t *context)
{
    int ret;
    foo__sigle_stream_req *h = NULL;

    printf("%s:%u\n", __FUNCTION__, __LINE__);
    /*
     * Read incoming message into h
     */
    if (grpc_c_read(context, (void **)&h, 0, -1)) {
        printf("Failed to read data from client\n");
    }

    if ( h ) {
        printf("%s: Received request: %s\n", __FUNCTION__, h->name1);
        if (h->user_info) {
            printf("User: %s, Score: %.2f\n", h->user_info->username, h->user_info->score);
        }
        foo__sigle_stream_req_free(h);
    }

    /*
     * Create a reply
     */
    foo__sigle_stream_rsp r;
    foo__sigle_stream_rsp__init(&r);

    char buf[1024];
    buf[0] = '\0';
    snprintf(buf, 1024, "hello, world! from server.");
    r.message1 = buf;
    
    // 设置用户响应信息
    r.user_info = malloc(sizeof(info__rspUserInfo));
    info__rsp_user_info__init(r.user_info);
    r.user_info->username = "server_user";
    r.user_info->score = 95.5;
    r.user_info->timestamp = time(NULL) * 1000; // 当前时间戳（毫秒）
    
    /*
     * Write reply back to the client
     */
    ret = grpc_c_write(context, &r, 0, -1);
    if ( ret ) {
        printf("Failed to write %d\n", ret);
    }

    grpc_c_status_t status;
    status.code = 0;
    status.message[0] = '\0';

    /*
     * Finish response for RPC
     */
    if (grpc_c_finish(context, &status, 0)) {
        printf("Failed to write status\n");
    }
}

void foo__greeter__client_stream_cb(grpc_c_context_t *context)
{
    int ret;
    foo__client_stream_req *h = NULL;
    
    char buf[1024];
    foo__client_stream_rsp r;
    foo__client_stream_rsp__init(&r);

    /*
     * Create a reply
     */
    printf("%s:%u\n", __FUNCTION__, __LINE__);
     
    buf[0] = '\0';
    snprintf(buf, 1024, "hello, world! from server.");
    r.message3 = buf;

    // 设置用户响应信息
    r.user_info = malloc(sizeof(info__rspUserInfo));
    info__rsp_user_info__init(r.user_info);
    r.user_info->username = "server_user";
    r.user_info->score = 95.5;
    r.user_info->timestamp = time(NULL) * 1000;

    for(;;)
    {
        /*
         * Read incoming message into h
         */
        ret = grpc_c_read(context, (void **)&h, 0, -1);
        if (ret)
        {
            break;
        }
        printf("%s: Received request: %s\n", __FUNCTION__, h->name3);
        if (h->user_info) {
            printf("User: %s, Score: %.2f\n", h->user_info->username, h->user_info->score);
        }
        foo__client_stream_req_free(h);
    }

    /*
     * Write reply back to the client
     */
    ret = grpc_c_write(context, &r, 0, -1);
    if ( ret ) {
        printf("Failed to write %d\n", ret);
    }
    
    grpc_c_status_t status;
    status.code = 0;
    status.message[0] = '\0';

    /*
     * Finish response for RPC
     */
    if (grpc_c_finish(context, &status, 0)) {
        printf("Failed to write status\n");
    }
}

void foo__greeter__server_stream_cb(grpc_c_context_t *context)
{
    int i;
    int ret;
    foo__server_stream_req *h = NULL;

    printf("%s:%u\n", __FUNCTION__, __LINE__);
    /*
     * Read incoming message into h
     */
    if (grpc_c_read(context, (void **)&h, 0, -1)) {
        printf("Failed to read data from client\n");
    }

    if ( h ) {
        printf("%s: Received request: %s\n", __FUNCTION__, h->name2);
        if (h->user_info) {
            printf("User: %s, Score: %.2f\n", h->user_info->username, h->user_info->score);
        }
        foo__server_stream_req_free(h);
    }

    /*
     * Create a reply
     */
    foo__server_stream_rsp r;
    foo__server_stream_rsp__init(&r);

    char buf[1024];
    buf[0] = '\0';
    snprintf(buf, 1024, "hello, world! from server.");
    r.message2 = buf;

    // 设置用户响应信息
    r.user_info = malloc(sizeof(info__rspUserInfo));
    info__rsp_user_info__init(r.user_info);
    r.user_info->username = "server_user";
    r.user_info->score = 95.5;
    r.user_info->timestamp = time(NULL) * 1000;

    for( i = 0 ; i < 10 ; i++ )
    {
        /*
         * Write reply back to the client
         */
        ret = grpc_c_write(context, &r, 0, -1);
        if ( ret ) {
            printf("Failed to write %d\n", ret);
            break;
        }
    }
    
    grpc_c_status_t status;
    status.code = 0;
    status.message[0] = '\0';

    /*
     * Finish response for RPC
     */
    if (grpc_c_finish(context, &status, 0)) {
        printf("Failed to write status\n");
    }
}

void foo__greeter__double_stream_cb(grpc_c_context_t *context)
{
    int ret;
    foo__double_stream_req *h = NULL;
    
    char buf[1024];
    foo__double_stream_rsp r;
    foo__double_stream_rsp__init(&r);

    printf("%s:%u\n", __FUNCTION__, __LINE__);
    /*
     * Create a reply
     */
    for(;;)
    {
        /*
         * Read incoming message into h
         */
        ret = grpc_c_read(context, (void **)&h, 0, -1);
        if (ret)
        {
            break;
        }

        printf("%s: Received request: %s\n", __FUNCTION__, h->name4);
        if (h->user_info) {
            printf("User: %s, Score: %.2f\n", h->user_info->username, h->user_info->score);
        }
        snprintf(buf, 1024, "hello, %s! from server.", h->name4);
        r.message4 = buf;

        // 设置用户响应信息
        r.user_info = malloc(sizeof(info__rspUserInfo));
        info__rsp_user_info__init(r.user_info);
        r.user_info->username = "server_user";
        r.user_info->score = 95.5;
        r.user_info->timestamp = time(NULL) * 1000;

        foo__double_stream_req_free(h);
        
        /*
         * Write reply back to the client
         */
        ret = grpc_c_write(context, &r, 0, -1);
        if ( ret ) {
            printf("Failed to write %d\n", ret);
            break;
        }
    }

    grpc_c_status_t status;
    status.code = 0;
    status.message[0] = '\0';

    /*
     * Finish response for RPC
     */
    if (grpc_c_finish(context, &status, 0)) {
        printf("Failed to write status\n");
    }
}

/*
 * Takes socket path as argument
 */
int foo_server() 
{
    int i = 0;

    /*
     * Initialize grpc-c library to be used with vanilla gRPC
     */
    grpc_c_init();
    printf("%s:%u\n", __FUNCTION__, __LINE__);
    
    /*
     * Create server object
     */
    test_server = grpc_c_server_create("127.0.0.1:4000", NULL, NULL);
    if (test_server == NULL) {
        printf("Failed to create server\n");
        exit(1);
    }

    /*
     * Initialize greeter service
     */
    foo__greeter__service_init(test_server);

    /*
     * Start server
     */
    grpc_c_server_start(test_server);

    /*
     * Blocks server to wait to completion
     */
    grpc_c_server_wait(test_server);

    /*
     * Destory server
     */
    grpc_c_server_destroy(test_server);

    /*
     * Destory grpc-c library.
     */
    grpc_c_shutdown();
}

int main()
{
    foo_server();
}
