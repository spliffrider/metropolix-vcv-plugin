#pragma once
#include <string>
namespace metroqa {
#if defined ARCH_LIN
// Xvfb/Mesa's default front buffer can be empty. Render the real scene to a
// Rack-owned framebuffer for deterministic Linux evidence.
void screenshot(const std::string& path);
#else
inline void screenshot(const std::string& path) { APP->window->screenshot(path); }
#endif
}
