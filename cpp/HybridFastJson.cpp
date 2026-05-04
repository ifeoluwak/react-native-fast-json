#include "HybridFastJson.hpp"


namespace margelo::nitro::fastjson {
    HybridFastJson::HybridFastJson() : HybridObject(TAG) {};

    std::shared_ptr<JsonPromise> HybridFastJson::parse(const std::variant<std::string, JsonSourceUri>& source) {
        return std::make_shared<JsonPromise>([]() {
            return std::variant<nitro::NullType, std::shared_ptr<HybridJsonViewSpec>>();
        });
    };
 }
