require "fontconfig/version"

require 'rbconfig'
require "fontconfig.#{RbConfig::CONFIG['DLEXT']}"

module Fontconfig
  class Pattern

    class Proxy
      def initialize pattern, key
        @pattern = pattern
        @key = key
      end

      def [](num)
        @pattern.get(@key, num)
      end

      def <<(obj)
        @pattern.add(@key, obj, true)
      end

      def delete!
        @pattern.delete(@key)
      end
    end

    def [](key)
      return Proxy.new(self, key)
    end
  end
end
