#pragma once
#include <atomic>

// Test-build-only clock shared by the audio sink and Rack's rendered UI.
// Frame timestamps follow recorded samples, even when rendering is slower
// than real time. These symbols do not exist in production builds.
namespace metrovideo {
extern std::atomic<int> samples;
extern std::atomic<bool> armed;
}
