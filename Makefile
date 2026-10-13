RACK_DIR ?= .tools/Rack-SDK
SOURCES += $(wildcard src/*.cpp)
DISTRIBUTABLES += README.md examples res
# Explicit public documentation list: local continuation notes stay local.
DISTRIBUTABLES += docs/HARDWARE-UI.md docs/MOD-TEST.md docs/UI-TARGET.md
DISTRIBUTABLES += docs/VALIDATION.md docs/DEVELOPMENT.md docs/TESTING.md docs/FEATURE-STATUS.md
DISTRIBUTABLES += docs/AUX-INPUTS.md docs/CTRL-KNOBS.md docs/PRESETS.md
DISTRIBUTABLES += docs/PRESET-CHAINS.md
DISTRIBUTABLES += docs/SWING.md
DISTRIBUTABLES += docs/STAGE-OVERRIDES.md docs/LOOPY.md
DISTRIBUTABLES += docs/SCALES.md docs/SLIDES.md docs/STAGE-CV.md docs/CLOCK-OUTPUT.md
DISTRIBUTABLES += docs/OUTPUT-ROUTING.md docs/TRACK-OPTIONS.md
DISTRIBUTABLES += docs/ACCUMULATOR-ROUTING.md
DISTRIBUTABLES += docs/COMPLETE-ROUTING.md
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
	$(CXX) -std=c++11 -O2 -Wall -Wextra -Werror -pedantic tests/AuxTests.cpp -o build/tests/aux-tests.exe
	./build/tests/aux-tests.exe
	$(CXX) -std=c++11 -O2 -Wall -Wextra -Werror -pedantic tests/ControlTests.cpp -o build/tests/control-tests.exe
	./build/tests/control-tests.exe
	$(CXX) -std=c++11 -O2 -Wall -Wextra -Werror -pedantic -pthread tests/PresetTests.cpp -o build/tests/preset-tests.exe
	./build/tests/preset-tests.exe
	$(CXX) -std=c++11 -O2 -Wall -Wextra -Werror -pedantic -pthread tests/ChainTests.cpp -o build/tests/chain-tests.exe
	./build/tests/chain-tests.exe
	$(CXX) -std=c++11 -O2 -Wall -Wextra -Werror -pedantic tests/SwingTests.cpp -o build/tests/swing-tests.exe
	./build/tests/swing-tests.exe
	$(CXX) -std=c++11 -O2 -Wall -Wextra -Werror -pedantic tests/OverrideTests.cpp -o build/tests/override-tests.exe
	./build/tests/override-tests.exe

	$(CXX) -std=c++11 -O2 -Wall -Wextra -Werror -pedantic tests/LoopyTests.cpp -o build/tests/loopy-tests.exe
	./build/tests/loopy-tests.exe
	$(CXX) -std=c++11 -O2 -Wall -Wextra -Werror -pedantic tests/ScaleTests.cpp -o build/tests/scale-tests.exe
	./build/tests/scale-tests.exe
	$(CXX) -std=c++11 -O2 -Wall -Wextra -Werror -pedantic tests/SlideTests.cpp -o build/tests/slide-tests.exe
	./build/tests/slide-tests.exe
	$(CXX) -std=c++11 -O2 -Wall -Wextra -Werror -pedantic tests/StageCvTests.cpp -o build/tests/stage-cv-tests.exe
	./build/tests/stage-cv-tests.exe
	$(CXX) -std=c++11 -O2 -Wall -Wextra -Werror -pedantic tests/ClockOutputTests.cpp -o build/tests/clock-output-tests.exe
	./build/tests/clock-output-tests.exe
	$(CXX) -std=c++11 -O2 -Wall -Wextra -Werror -pedantic tests/OutputRoutingTests.cpp -o build/tests/output-routing-tests.exe
	./build/tests/output-routing-tests.exe
	$(CXX) -std=c++11 -O2 -Wall -Wextra -Werror -pedantic tests/TrackOptionsTests.cpp -o build/tests/track-options-tests.exe
	./build/tests/track-options-tests.exe
	$(CXX) -std=c++11 -O2 -Wall -Wextra -Werror -pedantic tests/AccumulatorRoutingTests.cpp -o build/tests/accumulator-routing-tests.exe
	./build/tests/accumulator-routing-tests.exe
	$(CXX) -std=c++11 -O2 -Wall -Wextra -Werror -pedantic tests/ExtendedRoutingTests.cpp -o build/tests/extended-routing-tests.exe
	./build/tests/extended-routing-tests.exe

.PHONY: rack-test-build
rack-test-build:
	@mkdir -p build/tests
	$(CXX) -std=c++11 -O2 -DARCH_WIN -D_USE_MATH_DEFINES -I$(RACK_DIR)/include -I$(RACK_DIR)/dep/include tests/RackIntegrationTests.cpp -L$(RACK_DIR) -lRack -static-libstdc++ -static-libgcc -o build/tests/rack-integration.exe

.PHONY: ui-test-build
ui-test-build: build/ui-test/plugin.dll

build/ui-test/plugin.dll: $(SOURCES) $(wildcard src/*.hpp tests/*.hpp) Makefile $(RACK_DIR)/include/rack.hpp
	@mkdir -p build/ui-test
	$(CXX) -std=c++11 -O1 -g -DMETROPOLIX_QA -DARCH_WIN -D_USE_MATH_DEFINES -I$(RACK_DIR)/include -I$(RACK_DIR)/dep/include src/plugin.cpp src/Metropolix.cpp -shared -L$(RACK_DIR) -lRack -static-libstdc++ -static-libgcc -o build/ui-test/plugin.dll
