#include "HybridFastJson.hpp"

#include <NitroModules/Promise.hpp>
#include "HybridJsonView.hpp"

#include <simdjson.h>

namespace margelo::nitro::fastjson {

  std::unordered_map<std::string, std::shared_ptr<HybridJsonViewSpec>> jsonStrings;

HybridFastJson::HybridFastJson() : HybridObject(TAG) {}

    std::shared_ptr<Promise<ParseResult>> HybridFastJson::parse(const std::variant<std::string, JsonSourceUri>& source) {

    if (std::holds_alternative<std::string>(source)) {
        // for when the source is a string
        std::cout << "source is a string" << std::endl;
        // padded input
        simdjson::padded_string pstr = std::get<std::string>(source);

        std::shared_ptr<HybridJsonView> view = std::make_shared<HybridJsonView>();
        view->pstr = std::move(pstr);

        std::shared_ptr<HybridJsonViewSpec> asSpec = view;
        
        return Promise<ParseResult>::resolved(ParseResult{std::move(asSpec)});
    } else {
        // for when the source is a JsonSourceUri
        // std::cout << "source is a JsonSourceUri" << std::endl;

        auto uri = std::get<JsonSourceUri>(source);

        if (jsonStrings.find(uri.uri) != jsonStrings.end()) {
            std::cout << "jsonStrings: uri is in jsonStrings" << std::endl;
          return Promise<ParseResult>::resolved(ParseResult{jsonStrings[uri.uri]});
        }

        // std::cout << "uri: " << uri.uri << std::endl;

        auto json = simdjson::padded_string::load(uri.uri);
        
        std::shared_ptr<HybridJsonView> view = std::make_shared<HybridJsonView>();
        view->pstr = std::move(json);

        std::shared_ptr<HybridJsonViewSpec> asSpec = view;

        jsonStrings[uri.uri] = asSpec;
        
        return Promise<ParseResult>::resolved(ParseResult{asSpec});
    }
}

} // namespace margelo::nitro::fastjson
