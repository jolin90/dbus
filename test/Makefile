CFLAGS = `pkg-config dbus-1 --cflags` \
		 `pkg-config dbus-1 --libs`

all: client server

client:
	g++ -o $@ client.cpp  $(CFLAGS)

server:
	g++ -o $@ server.cpp  $(CFLAGS)

clean:
	@ rm client server -rvf
