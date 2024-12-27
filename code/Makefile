all:
	g++ server.cpp global.cpp utils/network_utils.cpp utils/message_utils.cpp utils/auth_utils.cpp utils/server_change.cpp utils/other.cpp -pthread -lssl -lcrypto -I/usr/include/SDL2 -D_REENTRANT -lSDL2 -lsndfile -o server
	g++ client.cpp utils/network_utils.cpp utils/message_utils.cpp utils/auth_utils.cpp utils/client_change.cpp utils/other.cpp -pthread -lssl -lcrypto -I/usr/include/SDL2 -D_REENTRANT -lSDL2 -lsndfile -o client

clean:
	rm -f client server
