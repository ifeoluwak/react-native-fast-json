package com.margelo.nitro.fastjson
  
import com.facebook.proguard.annotations.DoNotStrip

@DoNotStrip
class FastJson : HybridFastJsonSpec() {
  override fun multiply(a: Double, b: Double): Double {
    return a * b
  }
}
