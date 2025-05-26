#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <sys/stat.h>
#include "foo.grpc-c.h"
#include "google/protobuf/empty.grpc-c.h"

#define SOCKET_PATH "/tmp/luo.socket"
#define SOCKET_URL "unix://" SOCKET_PATH

static grpc_c_server_t *test_server;
static volatile sig_atomic_t g_running = 1;

// 清理函数
static void cleanup(void)
{
	if (test_server) {
		// 先停止服务器
		grpc_c_server_stop(test_server);
		// 等待服务器完全停止
		grpc_c_server_wait(test_server);
		// 然后销毁服务器
		grpc_c_server_destroy(test_server);
		test_server = NULL;
	}
	// 删除socket文件
	unlink(SOCKET_PATH);
	printf("Server cleanup completed\n");
}

// 信号处理函数
static void signal_handler(int signo)
{
	printf("\nReceived signal %d, cleaning up...\n", signo);
	g_running = 0;
}

/*
 * This function gets invoked whenever say_hello RPC gets called
 */
void foo__greeter__sigle_stream_cb(grpc_c_context_t *context)
{
	int ret;
	Google__Protobuf__Empty *empty_req = NULL;
	grpc_c_status_t status;

	printf("%s:%u\n", __FUNCTION__, __LINE__);
	/*
	 * Read incoming message
	 */
	if (grpc_c_read(context, (void **)&empty_req, 0, -1)) {
		printf("Failed to read data from client\n");
		status.code = GRPC_STATUS_INTERNAL;
		snprintf(status.message, sizeof(status.message), "Failed to read request data");
		grpc_c_finish(context, &status, 0);
		goto cleanup;
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
	if (!r.user_info) {
		status.code = GRPC_STATUS_INTERNAL;
		snprintf(status.message, sizeof(status.message), "Failed to allocate memory");
		grpc_c_finish(context, &status, 0);
		goto cleanup;
	}
	info__rsp_user_info__init(r.user_info);
	r.user_info->username = "server_user";
	r.user_info->score = 95.5;
	r.user_info->timestamp = time(NULL) * 1000;

	/*
	 * Write reply back to the client
	 */
	ret = grpc_c_write(context, &r, 0, -1);
	if (ret) {
		printf("Failed to write %d\n", ret);
		status.code = GRPC_STATUS_INTERNAL;
		snprintf(status.message, sizeof(status.message), "Failed to write response");
		grpc_c_finish(context, &status, 0);
		goto cleanup;
	}

	status.code = GRPC_STATUS_OK;
	snprintf(status.message, sizeof(status.message), "Success");

	/*
	 * Finish response for RPC
	 */
	if (grpc_c_finish(context, &status, 0)) {
		printf("Failed to write status\n");
	}

cleanup:
	if (empty_req) {
		google__protobuf__empty__free_unpacked(empty_req, NULL);
	}
}

void foo__greeter__client_stream_cb(grpc_c_context_t *context)
{
	int ret;
	foo__client_stream_req *h = NULL;
	grpc_c_status_t status;

	char buf[1024];
	foo__client_stream_rsp r;
	foo__client_stream_rsp__init(&r);

	printf("%s:%u\n", __FUNCTION__, __LINE__);

	buf[0] = '\0';
	snprintf(buf, 1024, "hello, world! from server.");
	r.message3 = buf;

	// 设置用户响应信息
	r.user_info = malloc(sizeof(info__rspUserInfo));
	if (!r.user_info) {
		status.code = GRPC_STATUS_INTERNAL;
		snprintf(status.message, sizeof(status.message), "Failed to allocate memory");
		grpc_c_finish(context, &status, 0);
		return;
	}
	info__rsp_user_info__init(r.user_info);
	r.user_info->username = "server_user";
	r.user_info->score = 95.5;
	r.user_info->timestamp = time(NULL) * 1000;

	for (;;) {
		ret = grpc_c_read(context, (void **)&h, 0, -1);
		if (ret) {
			break;
		}
		printf("%s: Received request: %s\n", __FUNCTION__, h->name3);
		if (h->user_info) {
			printf("User: %s, Score: %.2f\n", h->user_info->username, h->user_info->score);
		}
		foo__client_stream_req_free(h);
	}

	ret = grpc_c_write(context, &r, 0, -1);
	if (ret) {
		printf("Failed to write %d\n", ret);
		status.code = GRPC_STATUS_INTERNAL;
		snprintf(status.message, sizeof(status.message), "Failed to write response");
		grpc_c_finish(context, &status, 0);
		return;
	}

	status.code = GRPC_STATUS_OK;
	snprintf(status.message, sizeof(status.message), "Success");

	if (grpc_c_finish(context, &status, 0)) {
		printf("Failed to write status\n");
	}
}

void foo__greeter__server_stream_cb(grpc_c_context_t *context)
{
	int i;
	int ret;
	foo__server_stream_req *h = NULL;
	grpc_c_status_t status;

	printf("%s:%u\n", __FUNCTION__, __LINE__);
	/*
	 * Read incoming message into h
	 */
	if (grpc_c_read(context, (void **)&h, 0, -1)) {
		printf("Failed to read data from client\n");
		status.code = GRPC_STATUS_INTERNAL;
		snprintf(status.message, sizeof(status.message), "Failed to read request data");
		grpc_c_finish(context, &status, 0);
		return;
	}

	if (h) {
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
	if (!r.user_info) {
		status.code = GRPC_STATUS_INTERNAL;
		snprintf(status.message, sizeof(status.message), "Failed to allocate memory");
		grpc_c_finish(context, &status, 0);
		return;
	}
	info__rsp_user_info__init(r.user_info);
	r.user_info->username = "server_user";
	r.user_info->score = 95.5;
	r.user_info->timestamp = time(NULL) * 1000;

	for (i = 0; i < 10; i++) {
		ret = grpc_c_write(context, &r, 0, -1);
		if (ret) {
			printf("Failed to write %d\n", ret);
			status.code = GRPC_STATUS_INTERNAL;
			snprintf(status.message, sizeof(status.message), "Failed to write response");
			grpc_c_finish(context, &status, 0);
			return;
		}
	}

	status.code = GRPC_STATUS_OK;
	snprintf(status.message, sizeof(status.message), "Success");

	if (grpc_c_finish(context, &status, 0)) {
		printf("Failed to write status\n");
	}
}

void foo__greeter__double_stream_cb(grpc_c_context_t *context)
{
	int ret;
	foo__double_stream_req *h = NULL;
	grpc_c_status_t status;

	char buf[1024];
	foo__double_stream_rsp r;
	foo__double_stream_rsp__init(&r);

	printf("%s:%u\n", __FUNCTION__, __LINE__);

	for (;;) {
		ret = grpc_c_read(context, (void **)&h, 0, -1);
		if (ret) {
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
		if (!r.user_info) {
			status.code = GRPC_STATUS_INTERNAL;
			snprintf(status.message, sizeof(status.message), "Failed to allocate memory");
			grpc_c_finish(context, &status, 0);
			foo__double_stream_req_free(h);
			return;
		}
		info__rsp_user_info__init(r.user_info);
		r.user_info->username = "server_user";
		r.user_info->score = 95.5;
		r.user_info->timestamp = time(NULL) * 1000;

		foo__double_stream_req_free(h);

		ret = grpc_c_write(context, &r, 0, -1);
		if (ret) {
			printf("Failed to write %d\n", ret);
			status.code = GRPC_STATUS_INTERNAL;
			snprintf(status.message, sizeof(status.message), "Failed to write response");
			grpc_c_finish(context, &status, 0);
			return;
		}
	}

	status.code = GRPC_STATUS_OK;
	snprintf(status.message, sizeof(status.message), "Success");

	if (grpc_c_finish(context, &status, 0)) {
		printf("Failed to write status\n");
	}
}

void foo__greeter__sigle_stream2_cb(grpc_c_context_t *context)
{
	int ret;
	foo__sigle_stream_req *h = NULL;
	grpc_c_status_t status;

	printf("%s:%u\n", __FUNCTION__, __LINE__);
	/*
	 * Read incoming message into h
	 */
	if (grpc_c_read(context, (void **)&h, 0, -1)) {
		printf("Failed to read data from client\n");
		status.code = GRPC_STATUS_INTERNAL;
		snprintf(status.message, sizeof(status.message), "Failed to read request data");
		grpc_c_finish(context, &status, 0);
		goto cleanup;
	}

	if (h) {
		printf("%s: Received request: %s\n", __FUNCTION__, h->name1);
		if (h->user_info) {
			printf("User: %s, Score: %.2f\n", h->user_info->username, h->user_info->score);
		}
	}

	/*
	 * Create an empty response
	 */
	google__protobuf__Empty empty_rsp;
	google__protobuf__empty__init(&empty_rsp);

	/*
	 * Write empty response back to the client
	 */
	ret = grpc_c_write(context, &empty_rsp, 0, -1);
	if (ret) {
		printf("Failed to write %d\n", ret);
		status.code = GRPC_STATUS_INTERNAL;
		snprintf(status.message, sizeof(status.message), "Failed to write response");
		grpc_c_finish(context, &status, 0);
		goto cleanup;
	}

	status.code = GRPC_STATUS_OK;
	snprintf(status.message, sizeof(status.message), "Success");

	if (grpc_c_finish(context, &status, 0)) {
		printf("Failed to write status\n");
	}

cleanup:
	if (h) {
		foo__sigle_stream_req_free(h);
	}
}

/*
 * Takes socket path as argument
 */
int foo_server()
{
	int i = 0;

	// 注册信号处理
	signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);
	signal(SIGQUIT, signal_handler);

	/*
	 * Remove existing socket file if it exists
	 */
	struct stat st;
	if (stat(SOCKET_PATH, &st) == 0) {
		if (unlink(SOCKET_PATH) != 0) {
			printf("Failed to remove existing socket file\n");
			exit(1);
		}
	}

	/*
	 * Initialize grpc-c library to be used with vanilla gRPC
	 */
	grpc_c_init();
	printf("%s:%u\n", __FUNCTION__, __LINE__);

	/*
	 * Create server object
	 */
	test_server = grpc_c_server_create(SOCKET_URL, NULL, NULL);
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
	 * Main server loop
	 */
	while (g_running) {
		sleep(1);  // 每秒检查一次是否需要退出
	}

	/*
	 * Cleanup before exit
	 */
	cleanup();

	/*
	 * Destory grpc-c library.
	 */
	grpc_c_shutdown();
	return 0;
}

int main()
{
	return foo_server();
}
