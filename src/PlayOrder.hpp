#pragma once
#include <array>
#include <cstdint>

namespace metro {
// Numeric IDs 0..20 follow the manual's ORDER list, pp. 71-74.
enum Order { Forward, PingPong, Pendulum, OddEven, Jump2, Jump3, Jump4, Jump5,
    Hopscotch, Arp3Shift1, Arp4Shift2, Arp4Shift1, Converge, Diverge,
    Growing, Shrinking, PedalPoint, Brownian, Shuffle, QuasiRandom, RandomOrder };

struct RandomSource {
    uint32_t state = 1;
    uint32_t next() {
        state ^= state << 13;
        state ^= state >> 17;
        state ^= state << 5;
        return state;
    }
    float percent() { return static_cast<float>(next() >> 8) * (100.f / 16777216.f); }
    int index(int count) { return static_cast<int>((static_cast<uint64_t>(next()) * count) >> 32); }
};

class PlayOrder {
    std::array<int, 36> pattern{};
    int size = 0;
    int cursor = -1;
    int cachedLength = 0;
    Order cachedOrder = Forward;
    int walk = 0;
    int lastRandom = -1;

    void append(int value) { pattern[size++] = value; }
    void configure(int length, Order order, RandomSource& random) {
        cachedLength = length;
        cachedOrder = order;
        size = 0;
        cursor = -1;
        lastRandom = -1;
        walk = 0;
        switch (order) {
        case PingPong:
            for (int i = 0; i < length; ++i) append(i);
            for (int i = length - 1; i >= 0; --i) append(i);
            break;
        case Pendulum:
            for (int i = 0; i < length; ++i) append(i);
            for (int i = length - 2; i > 0; --i) append(i);
            break;
        case OddEven:
            for (int i = 0; i < length; i += 2) append(i);
            for (int i = 1; i < length; i += 2) append(i);
            break;
        case Jump2: case Jump3: case Jump4: case Jump5: {
            int step = 2 + static_cast<int>(order) - static_cast<int>(Jump2);
            int i = 0;
            do { append(i); i = (i + step) % length; } while (i != 0);
        } break;
        case Hopscotch:
            for (int i = 0; i < length; ++i) { append(i); append((i + 2) % length); }
            break;
        case Arp3Shift1: case Arp4Shift2: case Arp4Shift1: {
            // Follow the printed sequences, not an inferred meaning of the
            // shift labels: the two Arp-4 labels appear counterintuitive.
            int width = order == Arp3Shift1 ? 3 : 4;
            int stride = order == Arp4Shift1 ? 2 : 1;
            int start = 0;
            do {
                for (int i = 0; i < width; ++i) append((start + i) % length);
                start = (start + stride) % length;
            } while (start != 0);
        } break;
        case Converge:
            for (int i = 0; i < length; ++i) append(i % 2 == 0 ? i / 2 : length - 1 - i / 2);
            break;
        case Diverge: {
            int left = (length - 1) / 2;
            int right = left + 1;
            while (left >= 0 || right < length) {
                if (left >= 0) append(left--);
                if (right < length) append(right++);
            }
        } break;
        case Growing:
            for (int n = 1; n <= length; ++n) for (int i = 0; i < n; ++i) append(i);
            break;
        case Shrinking:
            for (int n = length; n >= 1; --n) for (int i = 0; i < n; ++i) append(i);
            break;
        case PedalPoint:
            if (length == 1) append(0);
            for (int i = 1; i < length; ++i) { append(0); append(i); }
            break;
        default:
            for (int i = 0; i < length; ++i) append(i);
            if (order == Shuffle) {
                for (int i = length - 1; i > 0; --i) {
                    int j = random.index(i + 1);
                    int value = pattern[i]; pattern[i] = pattern[j]; pattern[j] = value;
                }
            }
        }
    }
public:
    void reset() { cachedLength = 0; }

    int next(int length, Order order, bool reverse, const std::array<bool, 8>& enabled, RandomSource& random) {
        if (length != cachedLength || order != cachedOrder) configure(length, order, random);
        int pool[8];
        int count = 0;
        for (int i = 0; i < length; ++i) if (enabled[i]) pool[count++] = i;
        if (count == 0) return -1;
        if (order == RandomOrder || order == QuasiRandom) {
            if (order == QuasiRandom && length > 2 && count > 1) {
                int filtered = 0;
                for (int i = 0; i < count; ++i) if (pool[i] != lastRandom) pool[filtered++] = pool[i];
                count = filtered;
            }
            lastRandom = pool[random.index(count)];
            return lastRandom;
        }
        if (order == Brownian) {
            for (int attempt = 0; attempt < 32; ++attempt) {
                if (cursor >= 0) {
                    float choice = random.percent();
                    int step = choice < 50.f ? 1 : choice < 75.f ? 0 : -1;
                    walk = (walk + step + length) % length;
                }
                cursor = 0;
                int p = reverse ? length - 1 - walk : walk;
                if (enabled[p]) return p;
            }
            // Bounded recovery for a heavily skipped random walk. The manual
            // does not specify how skipped stages affect this random process.
            return pool[random.index(count)];
        }
        for (int attempt = 0; attempt < size; ++attempt) {
            cursor = (cursor + 1) % size;
            int p = reverse ? length - 1 - pattern[cursor] : pattern[cursor];
            if (enabled[p]) return p;
        }
        return -1; // For example, every reachable Jump-by-2 stage is skipped.
    }
};
} // namespace metro
