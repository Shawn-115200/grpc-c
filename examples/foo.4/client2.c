#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include "foo.grpc-c.h"
#include "google/protobuf/empty.grpc-c.h"

#define SOCKET_PATH "/tmp/luo.socket"
#define SOCKET_URL "unix://" SOCKET_PATH

void call_sigle_stream(grpc_c_client_t *client)
{
	int i;
	int ret;
	grpc_c_status_t status;

	printf("%s:%u\n", __FUNCTION__, __LINE__);
	for (i = 0 ; i < 1; i++) {
		google__protobuf__Empty empty_req;
		google__protobuf__empty__init(&empty_req);
		foo__sigle_stream_rsp *r = NULL;

		printf("\tThis will invoke a blocking RPC\n");
		ret = foo__greeter__sigle_stream__sync(client, NULL, 0, &empty_req, &r, &status,
											   -1);
		if (ret) {
			printf("call failed! %d\n", ret);
			break;
		}

		if (status.code == GRPC_STATUS_OK) {
			printf("RPC succeeded with message: %s\n", status.message);
			if (r) {
				printf("%s: Received response: %s\n", __FUNCTION__, r->message1);
				if (r->user_info) {
					printf("Server User: %s, Score: %.2f\n", r->user_info->username,
						   r->user_info->score);
				}
				foo__sigle_stream_rsp_free(r);
			}
		} else {
			printf("RPC failed with code %d: %s\n", status.code, status.message);
		}
	}

	printf("%s: Total Count %d\n", __FUNCTION__, i);
}

void call_sigle_stream2(grpc_c_client_t *client)
{
	int i;
	int ret;
	grpc_c_status_t status;

	printf("%s:%u\n", __FUNCTION__, __LINE__);
	for (i = 0 ; i < 1; i++) {
		foo__sigle_stream_req h;
		foo__sigle_stream_req__init(&h);
		google__protobuf__Empty *empty_rsp = NULL;

		char str[BUFSIZ] = {0};
		snprintf(str, BUFSIZ, "[client send]");
		h.name1 = str;

		// 设置用户请求信息
		h.user_info = malloc(sizeof(info__reqUserInfo));
		info__req_user_info__init(h.user_info);
		h.user_info->username = "client_user";
		h.user_info->score = 88.5;
		h.user_info->timestamp = time(NULL) * 1000;

		printf("\tThis will invoke a blocking RPC\n");
		ret = foo__greeter__sigle_stream2__sync(client, NULL, 0, &h, &empty_rsp,
												&status, -1);
		if (ret) {
			printf("call failed! %d\n", ret);
			if (h.user_info) {
				free(h.user_info);
			}
			break;
		}

		if (status.code == GRPC_STATUS_OK) {
			printf("RPC succeeded with message: %s\n", status.message);
			printf("%s: Received empty response\n", __FUNCTION__);
			if (empty_rsp) {
				google__protobuf__empty__free_unpacked(empty_rsp, NULL);
			}
		} else {
			printf("RPC failed with code %d: %s\n", status.code, status.message);
		}

		if (h.user_info) {
			free(h.user_info);
		}
	}

	printf("%s: Total Count %d\n", __FUNCTION__, i);
}

void call_client_stream(grpc_c_client_t *client)
{
	int i;
	int ret;
	grpc_c_status_t   status;
	grpc_c_context_t *context;

	printf("%s:%u\n", __FUNCTION__, __LINE__);
	ret = foo__greeter__client_stream__stream(client, NULL, 0, &context, -1);
	if (ret) {
		printf("stream connect failed! %d\n", ret);
		return;
	}

	foo__client_stream_req h;
	foo__client_stream_req__init(&h);

	char str[1024] = {0};
	snprintf(str, 1024, "[client send]");
	h.name3 = str;

	// 设置用户请求信息
	h.user_info = malloc(sizeof(info__reqUserInfo));
	info__req_user_info__init(h.user_info);
	h.user_info->username = "client_user";
	h.user_info->score = 88.5;
	h.user_info->timestamp = time(NULL) * 1000;

	for (i = 0 ; i < 10 ; i++) {
		ret = grpc_c_write(context, &h, 0, -1);
		if (ret) {
			printf("stream write failed! %d\n", ret);
			break;
		}
	}

	ret = grpc_c_write_done(context, 0, -1);
	if (ret) {
		printf("stream write done failed! %d\n", ret);
	}

	foo__client_stream_rsp *r;

	ret = grpc_c_read(context, (void **)&r, 0, -1);
	if (ret) {
		printf("stream read failed! %d\n", ret);
	} else {
		printf("%s: Received response: %s\n", __FUNCTION__, r->message3);
		if (r->user_info) {
			printf("Server User: %s, Score: %.2f\n", r->user_info->username,
				   r->user_info->score);
		}
		foo__client_stream_rsp_free(r);
	}

	ret = grpc_c_finish(context, &status, 0);
	if (ret) {
		printf("stream finish failed! %d\n", ret);
	} else {
		if (status.code == GRPC_STATUS_OK) {
			printf("Stream RPC succeeded with message: %s\n", status.message);
		} else {
			printf("Stream RPC failed with code %d: %s\n", status.code, status.message);
		}
	}

	printf("%s: Total Count %d\n", __FUNCTION__, i);
}

void call_server_stream(grpc_c_client_t *client)
{
	int i;
	int ret;
	grpc_c_status_t   status;
	grpc_c_context_t *context;

	printf("%s:%u\n", __FUNCTION__, __LINE__);
	ret = foo__greeter__server_stream__stream(client, NULL, 0, &context, -1);
	if (ret) {
		printf("stream connect failed! %d\n", ret);
		return;
	}

	foo__server_stream_req h;
	foo__server_stream_req__init(&h);

	char str[1024] = {0};
	snprintf(str, 1024, "[client send]");
	h.name2 = str;

	// 设置用户请求信息
	h.user_info = malloc(sizeof(info__reqUserInfo));
	info__req_user_info__init(h.user_info);
	h.user_info->username = "client_user";
	h.user_info->score = 88.5;
	h.user_info->timestamp = time(NULL) * 1000;

	ret = grpc_c_write(context, &h, 0, -1);
	if (ret) {
		printf("stream write failed! %d\n", ret);
	}

	ret = grpc_c_write_done(context, 0, -1);
	if (ret) {
		printf("stream write done failed! %d\n", ret);
	}

	for (i = 0 ;; i++) {
		foo__server_stream_rsp *r;

		ret = grpc_c_read(context, (void **)&r, 0, -1);
		if (ret) {
			break;
		}

		printf("%s: Received response: %s\n", __FUNCTION__, r->message2);
		if (r->user_info) {
			printf("Server User: %s, Score: %.2f\n", r->user_info->username,
				   r->user_info->score);
		}
		foo__server_stream_rsp_free(r);
	}

	ret = grpc_c_finish(context, &status, 0);
	if (ret) {
		printf("stream finish failed! %d\n", ret);
	} else {
		if (status.code == GRPC_STATUS_OK) {
			printf("Stream RPC succeeded with message: %s\n", status.message);
		} else {
			printf("Stream RPC failed with code %d: %s\n", status.code, status.message);
		}
	}

	printf("%s: Total Count %d\n", __FUNCTION__, i);
}


void call_double_stream(grpc_c_client_t *client)
{
	int i;
	int ret;
	grpc_c_status_t   status;
	grpc_c_context_t *context;

	printf("%s:%u\n", __FUNCTION__, __LINE__);
	ret = foo__greeter__double_stream__stream(client, NULL, 0, &context, -1);
	if (ret) {
		printf("stream connect failed! %d\n", ret);
		return;
	}

	foo__double_stream_req h;
	foo__double_stream_req__init(&h);

	char str[1024] = {0};
	snprintf(str, 1024, "[client send]");
	h.name4 = str;

	// 设置用户请求信息
	h.user_info = malloc(sizeof(info__reqUserInfo));
	info__req_user_info__init(h.user_info);
	h.user_info->username = "client_user";
	h.user_info->score = 88.5;
	h.user_info->timestamp = time(NULL) * 1000;

	for (i = 0 ; i < 10 ; i++) {
		ret = grpc_c_write(context, &h, 0, -1);
		if (ret) {
			printf("stream write failed! %d\n", ret);
			break;
		}

		foo__double_stream_rsp *r;

		ret = grpc_c_read(context, (void **)&r, 0, -1);
		if (ret) {
			printf("stream read failed! %d\n", ret);
			break;
		}

		printf("%s: Received response: %s\n", __FUNCTION__, r->message4);
		if (r->user_info) {
			printf("Server User: %s, Score: %.2f\n", r->user_info->username,
				   r->user_info->score);
		}
		foo__double_stream_rsp_free(r);
	}

	ret = grpc_c_write_done(context, 0, -1);
	if (ret) {
		printf("stream write done failed! %d\n", ret);
	}

	ret = grpc_c_finish(context, &status, 0);
	if (ret) {
		printf("stream finish failed! %d\n", ret);
	} else {
		if (status.code == GRPC_STATUS_OK) {
			printf("Stream RPC succeeded with message: %s\n", status.message);
		} else {
			printf("Stream RPC failed with code %d: %s\n", status.code, status.message);
		}
	}

	printf("%s: Total Count %d\n", __FUNCTION__, i);
}


/*
 * Takes as argument the socket name
 */
int foo_client()
{
	int retry_count = 0;
	const int max_retries = 5; // 最大重试次数
	const int retry_delay = 10; // 重试间隔（秒）

	/*
	 * Initialize grpc-c library to be used with vanilla grpc
	 */
	grpc_c_init();
	printf("%s:%u\n", __FUNCTION__, __LINE__);

	/*
	 * Create a client object with client name as foo client to be talking to
	 * a insecure server
	 */
	while (1) {
		grpc_c_client_t *client = grpc_c_client_init(SOCKET_URL, NULL, NULL);
		if (client != NULL) {
			// 客户端初始化成功，可以继续执行
			call_sigle_stream(client);
			call_sigle_stream2(client);
#if 0
			call_client_stream(client);
			call_server_stream(client);
			call_double_stream(client);
#endif
			grpc_c_client_stop(client);
			grpc_c_client_wait(client);
			grpc_c_client_free(client);
			printf("\tis not NULL\n");
			break; // 成功连接后退出循环
		} else {
			printf("Failed to connect to server. Retrying in %d seconds...\n", retry_delay);
			sleep(retry_delay); // 等待一段时间后重试
			retry_count++;
		}

		if (retry_count >=  max_retries)
			break;
	}

	if (retry_count == max_retries) {
		printf("Failed to connect to server after %d retries.\n", max_retries);
	}

	grpc_c_shutdown();
}

int main()
{
	foo_client();
}
