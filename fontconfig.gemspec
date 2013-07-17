# -*- encoding: utf-8 -*-
require File.expand_path('../lib/fontconfig/version', __FILE__)

Gem::Specification.new do |gem|
  gem.authors       = ["Vasily Fedoseyev"]
  gem.email         = ["vasilyfedoseyev@gmail.com"]
  gem.description   = %q{}
  gem.summary       = %q{Ruby bindings for fontconfig library}
  gem.homepage      = ""

  gem.files         = `git ls-files`.split($\)
  gem.executables   = gem.files.grep(%r{^bin/}).map{ |f| File.basename(f) }
  gem.test_files    = gem.files.grep(%r{^(test|spec|features)/})
  gem.name          = "fontconfig"
  gem.require_paths = ["lib"]
  gem.version       = Fontconfig::VERSION

  gem.extensions = "ext/fontconfig/extconf.rb"

  gem.add_dependency "pkg-config"
  gem.add_development_dependency "rake-compiler"
  gem.add_development_dependency "minitest"
end
