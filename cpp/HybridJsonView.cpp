#include "HybridJsonView.hpp"

#include <NitroModules/AnyMap.hpp>
#include <NitroModules/ArrayBuffer.hpp>

#include <simdjson.h>

namespace margelo::nitro::fastjson {

  using JsonCacheVariants = std::variant<nitro::NullType, std::vector<std::string>, std::string, std::shared_ptr<HybridJsonViewSpec>>;

  std::unordered_map<std::string, JsonCacheVariants> jsonViews;

  // struct JsonParseContext {
  //   simdjson::padded_string pstr;
  //   simdjson::ondemand::parser parser;
  //   simdjson::ondemand::document doc;
  //   simdjson::ondemand::object obj;

  //   JsonParseContext(std::string source) : pstr(source) {
  //     err = parser.p
  //   }
  // };

  // class JsonParseContext {
  //   public:
  //     simdjson::padded_string pstr;
  //     simdjson::ondemand::parser parser;
  //     simdjson::ondemand::document doc;
  //     simdjson::ondemand::object obj;


  //     static JsonParseContext& instance;


  //     static std::shared_ptr<JsonParseContext> create(std::string source) {
  //       auto context = std::make_shared<JsonParseContext>();
  //       context->pstr = simdjson::padded_string(source);
  //       context->parser = simdjson::ondemand::parser();
  //       // context->doc = context->parser.iterate(context->pstr);
  //       // context->obj = context->doc.get_object();
  //       return context;
  //     }
  // };

  // static JsonParseContext& instance;

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
  if (jsonViews.find(std::string(pstr)) != jsonViews.end()) {
    std::cout << "rawJson: pstr is in jsonViews" << std::endl;
    return std::get<std::string>(jsonViews[std::string(pstr)]);
  }
  doc = parser.iterate(pstr);
  std::string str = std::string(doc.raw_json().value());
  jsonViews[std::string(pstr)] = str;
  return str;
}

std::shared_ptr<ArrayBuffer> HybridJsonView::toBuffer() {
  return ArrayBuffer::allocate(0);
}

std::variant<nitro::NullType, std::shared_ptr<HybridJsonViewSpec>> HybridJsonView::getValue(const std::string& key) {
  if (jsonViews.find(key) != jsonViews.end()) {
    std::cout << "getValue: key is in jsonViews" << std::endl;
    if (std::holds_alternative<std::shared_ptr<HybridJsonViewSpec>>(jsonViews[key])) {
      return std::get<std::shared_ptr<HybridJsonViewSpec>>(jsonViews[key]);
    }
    return nitro::null;
  }
  auto start = std::chrono::high_resolution_clock::now();
  doc = parser.iterate(pstr);
  auto end = std::chrono::high_resolution_clock::now();
  std::cout << "getValue took " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " milliseconds" << std::endl;
  
  auto value = doc[key];
  if (value.error() || value.is_null()) {
    jsonViews[key] = nitro::null;
    return nitro::null;
  }
  double length = 0;
  std::string type = getJsonType(value.type());

  // if (type == "object") {
  //   length = value.count_fields();
  // }
  // if (type == "array") {
  //   length = value.count_elements();
  // }

  std::shared_ptr<HybridJsonView> view = std::make_shared<HybridJsonView>();
  view->pstr = std::move(value.raw_json().value());
  view->_length = 2;
  view->_type = type;
  std::shared_ptr<HybridJsonViewSpec> asSpec = view; 
  jsonViews[key] = asSpec;
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

bool HybridJsonView::has(const std::string& key) {
  doc = parser.iterate(pstr);
  auto obj = doc[key];
  if (obj.error() || obj.is_null()) {
    return false;
  }
  return true;
}

std::variant<nitro::NullType, std::shared_ptr<HybridJsonViewSpec>> HybridJsonView::at(double index) {
  doc = parser.iterate(pstr);
  if (doc.type() == simdjson::ondemand::json_type::array) {
    auto value = doc.at(index);
    if (value.error() || value.is_null()) {
      jsonViews[std::to_string(index)] = nitro::null;
      return nitro::null;
    }
    std::shared_ptr<HybridJsonView> view = std::make_shared<HybridJsonView>();
    view->pstr = std::move(value.raw_json().value());
    view->_length = 1;
    view->_type = "object";
    std::shared_ptr<HybridJsonViewSpec> asSpec = view; 
    jsonViews[std::to_string(index)] = asSpec;
    return std::move(asSpec);
  }
  return nitro::null;
}

std::variant<nitro::NullType, std::shared_ptr<HybridJsonViewSpec>> HybridJsonView::atPath(const std::string& path) {
  if (jsonViews.find(path) != jsonViews.end()) {
    std::cout << "path is in jsonViews" << std::endl;
    if (std::holds_alternative<std::shared_ptr<HybridJsonViewSpec>>(jsonViews[path])) {
      return std::get<std::shared_ptr<HybridJsonViewSpec>>(jsonViews[path]);
    }
    return nitro::null;
  }
  doc = parser.iterate(pstr);
  auto value = doc.at_path(path);
  if (value.error() || value.is_null()) {
    jsonViews[path] = nitro::null;
    return nitro::null;
  }
  std::shared_ptr<HybridJsonView> view = std::make_shared<HybridJsonView>();
  std::string type = getJsonType(value.type());
  // view->_length = type == "object" ? value.count_fields() : value.count_elements();
  view->_length = 2;
  view->_type = type;
  view->pstr = std::move(value.raw_json().value());
  std::shared_ptr<HybridJsonViewSpec> asSpec = view; 
  jsonViews[path] = asSpec;
  return asSpec;
};

std::variant<nitro::NullType, std::vector<std::string>> HybridJsonView::atPathWithWildcard(const std::string& path) {
  auto dom = simdjson::dom::parser();
  auto doc = dom.parse(pstr);
  auto value = doc.at_path_with_wildcard(path);
  if (value.error() || !value.has_value()) {
    jsonViews[path] = nitro::null;
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
      // for (const auto& [key, value] : key.get_object()) {
      //   // objectData[std::string(obj.key)] = simdjson::to_string(obj.value);
      //   arrayData.push_back("{" + std::string(key) + ": " + simdjson::to_string(value) + "}");
      // }
      // auto val2 = simdjson::to_json(key.get_object().value());
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
  jsonViews[path] = arrayData;
  return arrayData;
  // std::vector<std::string> keys;
  // for (auto key : value.value()) {
  //   std::string keyString = std::string(key.get_string().value());
  //   std::cout << "keyString: " << keyString << std::endl;
  //   keys.push_back(keyString);
  // }
  return nitro::null;
};

std::string HybridJsonView::asString() {
  doc = parser.iterate(pstr);
  if (doc.type() == simdjson::ondemand::json_type::string) {
    return std::string(doc.get_string().value());
  }
  if (doc.type() == simdjson::ondemand::json_type::number) {
    return std::to_string(doc.get_number().value().get_int64());
  }
  return std::string();
}

double HybridJsonView::asNumber() {
  doc = parser.iterate(pstr);
  std::cout << "type is " << doc.type() << std::endl;
  if (doc.type() == simdjson::ondemand::json_type::number) {
    double val = doc.get_number().value();
    return val;
  }
  if (doc.type() == simdjson::ondemand::json_type::string) {
    return std::stod(std::string(doc.get_string().value()));
  }
  if (doc.type() == simdjson::ondemand::json_type::boolean) {
    return doc.get_bool().value();
  }
  if (doc.type() == simdjson::ondemand::json_type::null) {
    return 0;
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

} // namespace margelo::nitro::fastjson
