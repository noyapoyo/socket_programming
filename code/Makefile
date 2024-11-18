all:
	g++ server.cpp utils/network_utils.cpp utils/message_utils.cpp utils/auth_utils.cpp utils/status_change.cpp utils/other.cpp -pthread -o server
	g++ client.cpp utils/network_utils.cpp utils/message_utils.cpp utils/auth_utils.cpp utils/status_change.cpp utils/other.cpp -pthread -o client

clean:
	rm -f client server
