#pragma once

#include "HybridFastJsonSpec.hpp"
#include "HybridJsonViewSpec.hpp"

namespace margelo::nitro::fastjson
{
    using JsonPromise = Promise<std::variant<nitro::NullType, std::shared_ptr<HybridJsonViewSpec>>>;

    class HybridFastJson : public HybridFastJsonSpec
    {
    public:
        HybridFastJson();

        std::shared_ptr<JsonPromise> parse(const std::variant<std::string, JsonSourceUri>& source) override;
    };
}
