#pragma once
#include <array>
#include <cstdint>
namespace metro {
struct MxMessage {
    static constexpr std::uint32_t Signature=0x4d585631;
    std::uint32_t signature=Signature;
    std::int64_t sourceId=-1;
    std::array<float,8> output{};
    float delay=0,level=1;
};
}
