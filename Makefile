RACK_DIR ?= .tools/Rack-SDK
SOURCES += $(wildcard src/*.cpp)
DISTRIBUTABLES += README.md docs examples res
DISTRIBUTABLES += reference/metropolix-panel-user-reference.png
EXTRA_LDFLAGS += -static-libgcc
include $(RACK_DIR)/plugin.mk

.PHONY: test
test:
	@mkdir -p build/tests
	$(CXX) -std=c++11 -O2 -Wall -Wextra -Werror -pedantic tests/SequencerTests.cpp -o build/tests/sequencer-tests.exe
	./build/tests/sequencer-tests.exe
	$(CXX) -std=c++11 -O2 -Wall -Wextra -Werror -pedantic tests/ManualConformanceTests.cpp -o build/tests/manual-tests.exe
	./build/tests/manual-tests.exe
	$(CXX) -std=c++11 -O2 -Wall -Wextra -Werror -pedantic tests/AccumulatorTests.cpp -o build/tests/accumulator-tests.exe
	./build/tests/accumulator-tests.exe
	$(CXX) -std=c++11 -O2 -Wall -Wextra -Werror -pedantic tests/ModulationTests.cpp -o build/tests/modulation-tests.exe
	./build/tests/modulation-tests.exe

.PHONY: rack-test-build
rack-test-build:
	@mkdir -p build/tests
	$(CXX) -std=c++11 -O2 -DARCH_WIN -D_USE_MATH_DEFINES -I$(RACK_DIR)/include -I$(RACK_DIR)/dep/include tests/RackIntegrationTests.cpp -L$(RACK_DIR) -lRack -static-libstdc++ -static-libgcc -o build/tests/rack-integration.exe

.PHONY: ui-test-build
ui-test-build:
	@mkdir -p build/ui-test
	$(CXX) -std=c++11 -O1 -g -DMETROPOLIX_QA -DARCH_WIN -D_USE_MATH_DEFINES -I$(RACK_DIR)/include -I$(RACK_DIR)/dep/include src/plugin.cpp src/Metropolix.cpp -shared -L$(RACK_DIR) -lRack -static-libstdc++ -static-libgcc -o build/ui-test/plugin.dll
