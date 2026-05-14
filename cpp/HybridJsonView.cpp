#include "HybridJsonView.hpp"

#include <NitroModules/AnyMap.hpp>
#include <NitroModules/ArrayBuffer.hpp>

#include <simdjson.h>

namespace margelo::nitro::fastjson {

  using JsonCacheVariants = std::variant<nitro::NullType, bool, double, std::vector<std::string>, std::string, std::shared_ptr<AnyMap>, std::shared_ptr<HybridJsonViewSpec>>;

  std::unordered_map<std::string, JsonCacheVariants> jsonViews;

HybridJsonView::HybridJsonView() : HybridObject(TAG) {
  parser = simdjson::ondemand::parser();
}

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

std::string HybridJsonView::getType() {
  return _type;
}

void HybridJsonView::setType(const std::string& type) {
  _type = type;
}

double HybridJsonView::getLength() {
  return _length;
}

void HybridJsonView::setLength(double length) {
  _length = length;
}

std::string HybridJsonView::rawJson() {
  std::string rawJsonKey = std::string(pstr).substr(0, 10) + "_rawJson";
  if (jsonViews.find(rawJsonKey) != jsonViews.end()) {
    std::cout << "rawJson: pstr is in jsonViews" << std::endl;
    return std::get<std::string>(jsonViews[rawJsonKey]);
  }
  doc = parser.iterate(pstr);
  std::string str = std::string(doc.raw_json().value());
  jsonViews[rawJsonKey] = str;
  return str;
}

std::variant<nitro::NullType, std::shared_ptr<HybridJsonViewSpec>> HybridJsonView::getValue(const std::string& key) {
  std::string getValueKey = key + "_getValue";
  if (jsonViews.find(getValueKey) != jsonViews.end()) {
    std::cout << "getValue: key is in jsonViews" << std::endl;
    if (std::holds_alternative<std::shared_ptr<HybridJsonViewSpec>>(jsonViews[getValueKey])) {
      return std::get<std::shared_ptr<HybridJsonViewSpec>>(jsonViews[getValueKey]);
    }
    return nitro::null;
  }
  auto start = std::chrono::high_resolution_clock::now();
  doc = parser.iterate(pstr);
  auto end = std::chrono::high_resolution_clock::now();
  std::cout << "getValue took " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " milliseconds" << std::endl;
  
  auto value = doc[key];
  if (value.error() || value.is_null()) {
    jsonViews[getValueKey] = nitro::null;
    return nitro::null;
  }
  double length = 0;
  std::string type = getJsonType(value.type());

  if (type == "object") {
    length = value.count_fields();
  }
  if (type == "array") {
    length = value.count_elements();
  }

  std::shared_ptr<HybridJsonView> view = std::make_shared<HybridJsonView>();
  view->pstr = std::move(value.raw_json().value());
  view->_length = length;
  view->_type = type;
  std::shared_ptr<HybridJsonViewSpec> asSpec = view; 
  jsonViews[getValueKey] = asSpec;
  return asSpec;
}

std::vector<std::string> HybridJsonView::keys() {
  doc = parser.iterate(pstr);
  if (doc.type() == simdjson::ondemand::json_type::array) {
   std::vector<std::string> keys;
   for (auto key : doc.get_array()) {
      keys.push_back(key.has_value() ? std::string(key.value()) : "");
   }

   return keys;
  }

  if (doc.type() == simdjson::ondemand::json_type::object) {
    std::vector<std::string> keys;
    for (auto key : doc.get_object()) {
      keys.push_back(key.has_value() ? std::string(key.unescaped_key().value()) : "");
    }

    return keys;
  }
  return std::vector<std::string>();
}

std::variant<nitro::NullType, std::shared_ptr<HybridJsonViewSpec>> HybridJsonView::at(double index) {
  // get pstr substring first 10 characters
  // this should give us a unique key for the at method
  std::string atKey = std::string(pstr).substr(0, 10) + std::to_string(index) + "_at";
  if (jsonViews.find(atKey) != jsonViews.end()) {
    std::cout << "at: key is in jsonViews" << std::endl;
    if (std::holds_alternative<std::shared_ptr<HybridJsonViewSpec>>(jsonViews[atKey])) {
      return std::get<std::shared_ptr<HybridJsonViewSpec>>(jsonViews[atKey]);
    }
    return nitro::null;
  }
  doc = parser.iterate(pstr);
  if (doc.type() == simdjson::ondemand::json_type::array) {
    auto value = doc.at(index);
    if (value.error() || value.is_null()) {
      jsonViews[atKey] = nitro::null;
      return nitro::null;
    }
    std::shared_ptr<HybridJsonView> view = std::make_shared<HybridJsonView>();
    view->_length = value.count_elements();
    view->pstr = std::move(value.raw_json().value());
    view->_type = "array";
    std::shared_ptr<HybridJsonViewSpec> asSpec = view; 
    jsonViews[atKey] = asSpec;
    return std::move(asSpec);
  }
  jsonViews[atKey] = nitro::null;
  return nitro::null;
}

std::variant<nitro::NullType, std::shared_ptr<HybridJsonViewSpec>> HybridJsonView::atPath(const std::string& path) {
  std::string atPathKey = path + "_atPath";
  if (jsonViews.find(atPathKey) != jsonViews.end()) {
    std::cout << "path is in jsonViews" << std::endl;
    if (std::holds_alternative<std::shared_ptr<HybridJsonViewSpec>>(jsonViews[atPathKey])) {
      return std::get<std::shared_ptr<HybridJsonViewSpec>>(jsonViews[atPathKey]);
    }
    return nitro::null;
  }
  doc = parser.iterate(pstr);
  auto value = doc.at_path(path);
  if (value.error() || value.is_null()) {
    jsonViews[atPathKey] = nitro::null;
    return nitro::null;
  }
  std::shared_ptr<HybridJsonView> view = std::make_shared<HybridJsonView>();
  std::string type = getJsonType(value.type());
  view->_length = 0;
  if (type == "object") {
    view->_length = value.count_fields();
  }
  if (type == "array") {
    view->_length = value.count_elements();
  }
  view->_type = type;
  view->pstr = std::move(value.raw_json().value());
  std::shared_ptr<HybridJsonViewSpec> asSpec = view; 
  jsonViews[atPathKey] = asSpec;
  return asSpec;
};

std::variant<nitro::NullType, std::vector<std::string>> HybridJsonView::atPathWithWildcard(const std::string& path) {
  std::string atPathWithWildcardKey = path + "_atPathWithWildcard";
  if (jsonViews.find(atPathWithWildcardKey) != jsonViews.end()) {
    std::cout << "atPathWithWildcard: path is in jsonViews" << std::endl;
    if (std::holds_alternative<std::vector<std::string>>(jsonViews[atPathWithWildcardKey])) {
      return std::get<std::vector<std::string>>(jsonViews[atPathWithWildcardKey]);
    }
    return nitro::null;
  }
  auto dom = simdjson::dom::parser();
  auto doc = dom.parse(pstr);
  auto value = doc.at_path_with_wildcard(path);
  if (value.error() || !value.has_value()) {
    jsonViews[atPathWithWildcardKey] = nitro::null;
    return nitro::null;
  }
  std::unordered_map<std::string, std::string> objectData;
  std::vector<std::string> arrayData;
  for (const auto& key : value.value()) {
    auto typex = key.type();
    std::cout << "type is " << typex << std::endl;
    switch (key.type()) {
      case simdjson::dom::element_type::OBJECT:
      std::cout << "object: " << std::endl;
      arrayData.push_back(simdjson::to_string(key.get_object().value()));

      break;
      case simdjson::dom::element_type::ARRAY:
      std::cout << "array: " << std::endl;
      for (const auto& obj : key.get_array()) {
        std::cout << "obj: " << obj << std::endl;
        arrayData.push_back(simdjson::to_string(obj));
      }
      break;
      case simdjson::dom::element_type::STRING:
      std::cout << "string: " << std::string(key.get_string().value()) << std::endl;
      arrayData.push_back(simdjson::to_string(key));
      break;
      case simdjson::dom::element_type::INT64:
      std::cout << "number: " << key.get_int64().value() << std::endl;
      arrayData.push_back(std::to_string(key.get_int64()));
      break;
      case simdjson::dom::element_type::UINT64:
      std::cout << "number: " << key.get_uint64().value() << std::endl;
      arrayData.push_back(std::to_string(key.get_uint64()));
      break;
      case simdjson::dom::element_type::DOUBLE:
      std::cout << "number: " << key.get_double().value() << std::endl;
      arrayData.push_back(std::to_string(key.get_double()));
      break;
      case simdjson::dom::element_type::BOOL:
      std::cout << "boolean: " << key.get_bool().value() << std::endl;
      arrayData.push_back(std::to_string(key.get_bool()));
      break;
      default:
      break;
    }
  }
  // return std::vector<std::string>(objectData.begin(), objectData.end());
  jsonViews[atPathWithWildcardKey] = arrayData;
  return arrayData;
};

std::string HybridJsonView::asString() {
  // get pstr substring first 10 characters
  // this should give us a unique key for the asString method
  std::string asStringKey = std::string(pstr).substr(0, 10) + "_asString";
  if (jsonViews.find(asStringKey) != jsonViews.end()) {
    std::cout << "asString: pstr is in jsonViews" << std::endl;
    if (std::holds_alternative<std::string>(jsonViews[asStringKey])) {
      return std::get<std::string>(jsonViews[asStringKey]);
    }
    return std::string();
  }
  doc = parser.iterate(pstr);
  if (doc.type() == simdjson::ondemand::json_type::string) {
    std::string str = std::string(doc.get_string().value());
    jsonViews[asStringKey] = str;
    return str;
  }
  if (doc.type() == simdjson::ondemand::json_type::number) {
    std::string str = std::to_string(doc.get_double().value());
    jsonViews[asStringKey] = str;
    return str;
  }
  return std::string();
}

double HybridJsonView::asNumber() {
  std::string asNumberKey = std::string(pstr).substr(0, 10) + "_asNumber";
  if (jsonViews.find(asNumberKey) != jsonViews.end()) {
    std::cout << "asNumber: pstr is in jsonViews" << std::endl;
    if (std::holds_alternative<double>(jsonViews[asNumberKey])) {
      return std::get<double>(jsonViews[asNumberKey]);
    }
    return 0;
  }
  doc = parser.iterate(pstr);
  std::cout << "type is " << doc.type() << std::endl;
  if (doc.type() == simdjson::ondemand::json_type::number) {
    double val = doc.get_double().value();
    jsonViews[asNumberKey] = val;
    return val;
  }
  if (doc.type() == simdjson::ondemand::json_type::string) {
    double val = std::stod(std::string(doc.get_string().value()));
    jsonViews[asNumberKey] = val;
    return val;
  }
  if (doc.type() == simdjson::ondemand::json_type::boolean) {
    double val = doc.get_bool().value() ? 1 : 0;
    jsonViews[asNumberKey] = val;
    return val;
  }
  if (doc.type() == simdjson::ondemand::json_type::null) {
    double val = 0;
    jsonViews[asNumberKey] = val;
    return val;
  }
  return 0;
}

bool HybridJsonView::asBoolean() {
  doc = parser.iterate(pstr);
  std::cout << "type is " << doc.type() << std::endl;
  if (doc.type() == simdjson::ondemand::json_type::boolean) {
    return doc.get_bool().value();
  }
  if (doc.type() == simdjson::ondemand::json_type::string) {
    return std::stoi(std::string(doc.get_string().value())) != 0;
  }
  if (doc.type() == simdjson::ondemand::json_type::number) {
    return doc.get_number().value().get_uint64() != 0ull;
  }
  return false;
}


AnyValue materializeValue(simdjson::ondemand::value& value) {
  std::unordered_map<std::string, AnyValue> objectData;
  std::string str;
  std::vector<AnyValue> arrayData;
  std::cout << value.type() << std::endl;
  switch (value.type()) {
    case simdjson::ondemand::json_type::object:
      for (auto it : value.get_object()) {
        std::string key = std::string(it.unescaped_key().value());
        auto value = it.value().value();
        AnyValue result = materializeValue(value);
        objectData[key] = result;
      }
      return AnyValue(objectData);
    case simdjson::ondemand::json_type::array:
      for (auto it : value.get_array()) {
        AnyValue result = materializeValue(it.value());
        arrayData.push_back(result);
      }
    return AnyValue(arrayData);
    case simdjson::ondemand::json_type::string:
      str = std::string(value.get_string().value());
    return AnyValue(str);
    case simdjson::ondemand::json_type::number:
    return AnyValue(value.get_double().value());
    case simdjson::ondemand::json_type::boolean:
    return AnyValue(value.get_bool().value());
    case simdjson::ondemand::json_type::null:
    return AnyValue(nitro::null);
    default:
    return AnyValue(nitro::null);
  }
}

std::shared_ptr<AnyMap> HybridJsonView::asObject() {
  std::string asObjectKey = std::string(pstr).substr(0, 10) + "_asObject";
  if (jsonViews.find(asObjectKey) != jsonViews.end()) {
    std::cout << "asObject: pstr is in jsonViews" << std::endl;
    if (std::holds_alternative<std::shared_ptr<AnyMap>>(jsonViews[asObjectKey])) {
      return std::get<std::shared_ptr<AnyMap>>(jsonViews[asObjectKey]);
    }
    return AnyMap::make();
  }
  doc = parser.iterate(pstr);
  auto map = AnyMap::make();
  if (doc.type() == simdjson::ondemand::json_type::object) {
    std::unordered_map<std::string, AnyValue> objectData;
    auto value = doc.get_object();
    if (value.error() || value.is_empty()) {
      jsonViews[asObjectKey] = nitro::null;
      return map;
    }
    for (auto it : value) {
      auto key = it.unescaped_key().value();
      auto value = it.value().value();
      AnyValue result = materializeValue(value);
      objectData[std::string(key)] = result;
    }
    map->setObject("data", objectData);
    jsonViews[asObjectKey] = map;
    return map;
  }
  if (doc.type() == simdjson::ondemand::json_type::array) {
    std::vector<AnyValue> arrayData;
    for (auto it : doc.get_array()) {
      AnyValue result = materializeValue(it.value());
      arrayData.push_back(result);
    }
    map->setArray("data", arrayData);
    jsonViews[asObjectKey] = map;
    return map;
  }
  jsonViews[asObjectKey] = map;
  return map;
}

} // namespace margelo::nitro::fastjson
