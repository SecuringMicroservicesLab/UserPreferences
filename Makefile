# Use pkg-config to dynamically fetch all necessary flags
LDFLAGS = $(shell pkg-config --libs grpc++ protobuf)
CXXFLAGS = -std=c++17 $(shell pkg-config --cflags grpc++)

# The rest of your Makefile
SRCS = src/main.cpp services.pb.cc services.grpc.pb.cc
OBJS = $(SRCS:.cpp=.o)

server: $(SRCS)
	g++ $(CXXFLAGS) $(SRCS) $(LDFLAGS) -o server -I include/laserpants/dotenv

clean:
	rm -f server
