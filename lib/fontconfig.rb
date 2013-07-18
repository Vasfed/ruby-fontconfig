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

      def each &blk
        @pattern.each_value @key, &blk
      end

      include Enumerable
    end

    def [](key)
      return Proxy.new(self, key.to_s)
    end

    def inspect
      "#<#{self.class.name} #{self.format('%{=fcmatch}')}>"
    end

    alias :each :each_key
    include Enumerable

    def config_substitute! config=Fontconfig.current_config, kind=:font
      #kind - :pattern, :font, :scan
      unless config.substitute(self)
        raise "cannot perform substitutions on pattern"
      end
      self
    end
  end


  def self.pattern *args
    Pattern.new *args
  end

  def self.prepared_pattern *args
    pat = self.pattern *args
    pat.config_substitute!
    pat.default_substitute!
    pat
  end

  def self.match *args
    pat = if args.first.is_a? Pattern
      args.first
    else
      self.prepared_pattern *args
    end
    current_config.font_match pat
  end
end
