BUILD_DIR := build

all: clean
	@ mkdir -p $(BUILD_DIR)
	@ cd $(BUILD_DIR) && cmake .. && make

clean:
	@ rm $(BUILD_DIR) tags -rf
