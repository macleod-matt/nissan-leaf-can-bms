BOARD?=longan-rp2040:longan-rp2040:canbed2040
PORT?=/dev/ttyACM0
BUILD=build

.PHONY: default all flash clean

default:  all flash clean 

all: 
	arduino-cli compile --fqbn $(BOARD) --output-dir $(BUILD) CANBusNissanLeaf/CANBusNissanLeaf.ino

flash: 
	arduino-cli upload --fqbn $(BOARD) --port $(PORT) --input-dir $(BUILD)

clean:
	rm -r build
