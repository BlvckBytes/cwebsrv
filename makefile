CC=gcc

cws:
	$(CC) cwebsrv.c cws_client_handler.c cws_client.c cws_util.c -o cws
