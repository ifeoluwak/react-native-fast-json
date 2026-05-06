require "json"

package = JSON.parse(File.read(File.join(__dir__, "package.json")))

Pod::Spec.new do |s|
  s.name         = "FastJson"
  s.version      = package["version"]
  s.summary      = package["description"]
  s.homepage     = package["homepage"]
  s.license      = package["license"]
  s.authors      = package["author"]

  # Align with Swift CxxStdlib / Nitro toolchains (iOS 16+).
  s.platforms    = { :ios => '16.0' }
  s.source       = { :git => "https://github.com/ifeoluwak/react-native-fast-json.git", :tag => "#{s.version}" }

  s.source_files = [
    "ios/**/*.{swift}",
    "ios/**/*.{m,mm}",
    # .h: simdjson amalgamation header in cpp/third_party
    "cpp/**/*.{h,hpp,cpp}",
  ]


  # Force O3 optimization specifically for C++ files
  s.compiler_flags = '-O3 -DNDEBUG'

  s.pod_target_xcconfig = {
    'GCC_OPTIMIZATION_LEVEL' => '3',
    'OTHER_CPLUSPLUSFLAGS' => '-O3 -DNDEBUG',
    "HEADER_SEARCH_PATHS" => "\"$(PODS_TARGET_SRCROOT)/cpp\" \"$(PODS_TARGET_SRCROOT)/cpp/third_party\" $(inherited)",
  }

  s.dependency 'React-jsi'
  s.dependency 'React-callinvoker'

  load 'nitrogen/generated/ios/FastJson+autolinking.rb'
  add_nitrogen_files(s)

  install_modules_dependencies(s)
end
