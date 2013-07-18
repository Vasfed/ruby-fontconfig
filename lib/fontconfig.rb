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
      key = key.to_s
      return nil unless has_key?(key)
      return Proxy.new(self, key)
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

    def prepare! config=Fontconfig.current_config, kind=:font
      @prepared = true
      config_substitute! config, kind
      default_substitute!
      self
    end

    def match config=Fontconfig.current_config
      raise "unprepared pattern match" unless @prepared
      config.font_match self
    end

    def filename
      self[:filename].first
    end
  end


  def self.pattern *args
    Pattern.new *args
  end

  def self.prepared_pattern *args
    self.pattern(*args).prepare!
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
