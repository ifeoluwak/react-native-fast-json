#pragma once

#include "HybridJsonViewSpec.hpp"
#include <NitroModules/ArrayBuffer.hpp>

namespace margelo::nitro::fastjson
{
    using JsonViewToJson = std::function<std::shared_ptr<Promise<std::shared_ptr<AnyMap>>>()>;
    using JsonViewToBuffer = std::function<std::shared_ptr<Promise<std::shared_ptr<margelo::nitro::ArrayBuffer>>>()>;
    using JsonViewGet = std::function<std::shared_ptr<Promise<std::variant<nitro::NullType, std::shared_ptr<HybridJsonViewSpec>>>>(const std::string& /* key */)>;
    using JsonViewKeys = std::function<std::shared_ptr<Promise<std::vector<std::string>>>()>;
    using JsonViewHas = std::function<std::shared_ptr<Promise<bool>>(const std::string& /* key */)>;
    using JsonViewAt = std::function<std::shared_ptr<Promise<std::variant<nitro::NullType, std::shared_ptr<HybridJsonViewSpec>>>>(double /* index */)>;
    using JsonViewType = std::shared_ptr<AnyMap>;
    using JsonViewAsString = std::function<std::shared_ptr<Promise<std::string>>()>;
    using JsonViewAsNumber = std::function<std::shared_ptr<Promise<double>>()>;
    using JsonViewAsBoolean = std::function<std::shared_ptr<Promise<bool>>()>;

    class HybridJsonView : public HybridJsonViewSpec
    {
    public:
        HybridJsonView();

        std::shared_ptr<AnyMap> toJson() override;
        std::shared_ptr<margelo::nitro::ArrayBuffer> toBuffer() override;
        std::variant<nitro::NullType, std::shared_ptr<HybridJsonViewSpec>> getValue(const std::string& key) override;
        std::vector<std::string> keys() override;
        bool has(const std::string& key) override;
        std::variant<nitro::NullType, std::shared_ptr<HybridJsonViewSpec>> at(double index) override;
        std::string asString() override;
        double asNumber() override;
        bool asBoolean() override;
    };
}
