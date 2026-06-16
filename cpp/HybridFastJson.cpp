#include "HybridFastJson.hpp"

#include <NitroModules/Promise.hpp>
#include "HybridJsonView.hpp"

#include <simdjson.h>

namespace margelo::nitro::fastjson
{

  std::string getJsonType(simdjson::ondemand::json_type type) {
    switch (type) {
      case simdjson::ondemand::json_type::object:  return "object";
      case simdjson::ondemand::json_type::array:   return "array";
      case simdjson::ondemand::json_type::string:   return "string";
      case simdjson::ondemand::json_type::number:   return "number";
      case simdjson::ondemand::json_type::boolean: return "boolean";
      case simdjson::ondemand::json_type::null:     return "null";
      default:                                      return "string"; // default to string
    }
  }

  std::unordered_map<std::string, std::shared_ptr<HybridJsonViewSpec>> jsonStrings;

  HybridFastJson::HybridFastJson() : HybridObject(TAG) {}

  /**
   * Parses a JSON string and returns a Promise that resolves to a JsonViewSpec.
   * Not ideal, prefer parseFile or use simple JSON.parse if possible.
   * @param str The JSON string.
   * @returns A Promise that resolves to a JsonViewSpec or null. Not cached (unlike parseFile).
   */
  std::shared_ptr<Promise<ParseResult>> HybridFastJson::parseString(const std::string &str)
  {
    std::shared_ptr<HybridJsonView> view = std::make_shared<HybridJsonView>();
    view->pstr = str;

    std::shared_ptr<HybridJsonViewSpec> asSpec = view;

    return Promise<ParseResult>::resolved(ParseResult{std::move(asSpec)});
  }


  /**
   * Parses a JSON file and returns a Promise that resolves to a JsonViewSpec.
   * No check is done if invalid path is provided. Error handling is left to the caller.
   * @param path The path to the JSON file.
   * @returns A Promise that resolves to a JsonViewSpec or null. Response is cached for future calls.
   */
  std::shared_ptr<Promise<ParseResult>> HybridFastJson::parseFile(const std::string& path)
  {
    if (jsonStrings.find(path) != jsonStrings.end())
    {
      return Promise<ParseResult>::resolved(ParseResult{jsonStrings[path]});
    }

    std::shared_ptr<HybridJsonView> view = std::make_shared<HybridJsonView>();
    view->pstr = simdjson::padded_string::load(path);

    std::shared_ptr<HybridJsonViewSpec> asSpec = view;
    view->doc = view->parser.iterate(view->pstr);
    view->setType(getJsonType(view->doc.type()));
    if (view->getType() == "array") {
      view->setLength(view->doc.count_elements());
    }
    if (view->getType() == "object") {
      view->setLength(view->doc.count_fields());
    }

    jsonStrings[path] = asSpec;

    return Promise<ParseResult>::resolved(ParseResult{asSpec});
  }

  void HybridFastJson::release(const std::string &source)
  {
    jsonStrings.erase(source);
  }

} // namespace margelo::nitro::fastjson
