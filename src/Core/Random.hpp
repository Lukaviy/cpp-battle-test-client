#pragma once

#include <cstdint>
#include <random>

namespace sw::core
{
    // Random source as a service. Lets tests inject a deterministic engine.
    class IRandom
    {
    public:
        virtual ~IRandom() = default;

        // Inclusive uniform integer in [min, max].
        virtual std::uint32_t uniform(std::uint32_t min, std::uint32_t max) = 0;

        // Convenience: returns true with probability `chance / scale`.
        bool roll(std::uint32_t chance, std::uint32_t scale)
        {
            if (chance == 0)
            {
                return false;
            }
            if (chance >= scale)
            {
                return true;
            }
            return uniform(1, scale) <= chance;
        }
    };

    class DefaultRandom final : public IRandom
    {
    public:
        DefaultRandom() :
                _engine(std::random_device{}())
        {}

        explicit DefaultRandom(std::uint32_t seed) :
                _engine(seed)
        {}

        std::uint32_t uniform(std::uint32_t min, std::uint32_t max) override
        {
            std::uniform_int_distribution<std::uint32_t> dist(min, max);
            return dist(_engine);
        }

    private:
        std::mt19937 _engine;
    };
}
