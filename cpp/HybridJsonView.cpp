#include "HybridJsonView.hpp"

#include <NitroModules/ArrayBuffer.hpp>

namespace margelo::nitro::fastjson {

    HybridJsonView::HybridJsonView() : HybridObject(TAG) {};

    std::shared_ptr<AnyMap> HybridJsonView::toJson() {
        return std::make_shared<AnyMap>();
    }

    std::shared_ptr<margelo::nitro::ArrayBuffer> HybridJsonView::toBuffer() {
        return nullptr;
    }

    std::variant<nitro::NullType, std::shared_ptr<HybridJsonViewSpec>> HybridJsonView::getValue(const std::string& key) {
        return std::variant<nitro::NullType, std::shared_ptr<HybridJsonViewSpec>>();
    }
    std::vector<std::string> HybridJsonView::keys() {
        return std::vector<std::string>();
    }
    bool HybridJsonView::has(const std::string& key) {
        return false;
    }
    std::variant<nitro::NullType, std::shared_ptr<HybridJsonViewSpec>> HybridJsonView::at(double index) {
        return std::variant<nitro::NullType, std::shared_ptr<HybridJsonViewSpec>>();
    }
    std::string HybridJsonView::asString() {
        return std::string();
    }
    double HybridJsonView::asNumber() {
        return 0;
    }
    bool HybridJsonView::asBoolean() {
        return false;
    }

}