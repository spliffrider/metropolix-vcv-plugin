#pragma once
#include <string>
namespace metroqa {
// Xvfb/Mesa's default front buffer can be empty. Render the real scene to a
// Rack-owned framebuffer on Linux; use Rack's front buffer on other platforms.
void screenshot(const std::string& path);
}
