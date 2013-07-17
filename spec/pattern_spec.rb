require 'minitest/autorun'
require 'fontconfig'

describe Fontconfig::Pattern do

  describe "class methods" do
    subject { Fontconfig::Pattern }

    it "create by parsing" do
      res = subject.parse("Monospace Bold 12px")
      res.must_be_instance_of subject
    end

    it "to_s" do
      res = subject.parse("monospace bold 12px").to_s
      res.must_be_kind_of String
      puts "Tos: #{res}"
    end
  end
end