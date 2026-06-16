#pragma once

#include "HybridJsonViewSpec.hpp"
#include <simdjson.h>

namespace margelo::nitro::fastjson {

class HybridJsonView : public HybridJsonViewSpec {
public:
  HybridJsonView();
  HybridJsonView(const simdjson::ondemand::object& object);

  std::string getType() override;
  void setType(const std::string& type) override;
  double getLength() override;
  void setLength(double length) override;

  std::string rawJson() override;
  std::variant<nitro::NullType, std::shared_ptr<HybridJsonViewSpec>> getValue(const std::string& key) override;
  std::vector<std::string> keys() override;
  std::variant<nitro::NullType, std::shared_ptr<HybridJsonViewSpec>> at(double index) override;
  std::variant<nitro::NullType, std::shared_ptr<HybridJsonViewSpec>> atPath(const std::string& path) override;
  std::variant<nitro::NullType, std::vector<std::string>> atPathWithWildcard(const std::string& path) override;
  std::variant<nitro::NullType, std::shared_ptr<HybridJsonViewSpec>> next() override;

  std::string asString() override;
  double asNumber() override;
  bool asBoolean() override;
  std::shared_ptr<AnyMap> asObject() override;

  simdjson::padded_string pstr;
  simdjson::ondemand::parser parser;
  simdjson::ondemand::document doc;
  simdjson::ondemand::object obj;

  simdjson::ondemand::array_iterator arrayIterator;
  bool hasArrayIterateStarted = false;

private:
  std::string _type;
  double _length = 0;
};

} // namespace margelo::nitro::fastjson
