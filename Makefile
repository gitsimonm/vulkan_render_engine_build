include .env

CFLAGS = -std=c++17 -I. -I$(VULKAN_SDK_PATH)/include
LDFLAGS = -L$(VULKAN_SDK_PATH)/lib `pkg-config --static --libs glfw3` -lvulkan

a.out: *.cpp *.hpp
	g++ $(CFLAGS) -o a.out *.cpp $(LDFLAGS) 

# the command 'g++ -o target_name file_name' compiles and links our 'main.cpp' file into the executable target 'a.out' 

.PHONY: test clean

test: a.out
	./a.out

clean:
	rm -f a.out