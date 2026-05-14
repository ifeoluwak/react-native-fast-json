#include "HybridFastJson.hpp"

#include <NitroModules/Promise.hpp>
#include "HybridJsonView.hpp"

#include <simdjson.h>

namespace margelo::nitro::fastjson
{

  std::unordered_map<std::string, std::shared_ptr<HybridJsonViewSpec>> jsonStrings;

  HybridFastJson::HybridFastJson() : HybridObject(TAG) {}

  /**
   * Parses a JSON string and returns a Promise that resolves to a JsonViewSpec.
   * Not ideal, prefer parseFile or use simple JSON.parse if possible.
   * @param str The JSON string.
   * @returns A Promise that resolves to a JsonViewSpec or null. Response is cached for future calls.
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
      std::cout << "jsonStrings: uri is in jsonStrings" << std::endl;
      return Promise<ParseResult>::resolved(ParseResult{jsonStrings[path]});
    }

    std::shared_ptr<HybridJsonView> view = std::make_shared<HybridJsonView>();
    view->pstr = simdjson::padded_string::load(path);

    std::shared_ptr<HybridJsonViewSpec> asSpec = view;

    jsonStrings[path] = asSpec;

    return Promise<ParseResult>::resolved(ParseResult{asSpec});
  }


  void HybridFastJson::release(const std::string &source)
  {
    jsonStrings.erase(source);
  }

} // namespace margelo::nitro::fastjson
