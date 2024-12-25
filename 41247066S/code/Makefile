all:
	g++ server.cpp global.cpp utils/network_utils.cpp utils/message_utils.cpp utils/auth_utils.cpp utils/server_change.cpp utils/other.cpp -pthread -lssl -lcrypto -o server
	g++ client.cpp utils/network_utils.cpp utils/message_utils.cpp utils/auth_utils.cpp utils/client_change.cpp utils/other.cpp -pthread -lssl -lcrypto -o client

clean:
	rm -f client server