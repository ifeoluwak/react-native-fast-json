#pragma once

#include "HybridFastJsonSpec.hpp"
#include "HybridJsonViewSpec.hpp"

namespace margelo::nitro::fastjson {

using ParseResult = std::variant<nitro::NullType, std::shared_ptr<HybridJsonViewSpec>>;

class HybridFastJson : public HybridFastJsonSpec {
public:
  HybridFastJson();

  std::shared_ptr<Promise<std::variant<nitro::NullType, std::shared_ptr<HybridJsonViewSpec>>>> parseString(const std::string& str) override;
  std::shared_ptr<Promise<std::variant<nitro::NullType, std::shared_ptr<HybridJsonViewSpec>>>> parseFile(const std::string& path) override;
  void release(const std::string& source) override;
};

} // namespace margelo::nitro::fastjson
