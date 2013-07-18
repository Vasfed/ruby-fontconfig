require 'minitest/autorun'
require 'test_helper'

describe Fontconfig::Pattern do

  describe "class methods" do
    subject { Fontconfig::Pattern }

    it "create by parsing" do
      res = subject.parse("Monospace Bold 12px")
      res.must_be_instance_of subject
    end

  end

  describe "instance" do
    subject { Fontconfig::Pattern.parse("monospace-Bold-Italic 21") }

    it "to_s" do
      res = subject.to_s
      res.must_be_kind_of String
    end

    it "format" do
      res = subject.format("%{family}")
      # subject.debug_print
      # puts "format #{res}"
      res.must_be_kind_of String
    end

    it "hash" do
      subject.hash.must_be_kind_of Fixnum
    end

    it "set/det/delete" do
      subject.add("key", "value")
      subject.add("key", "value2", true)
      subject.get("key", 0).must_equal "value"
      subject.get("key", 1).must_equal "value2"
      subject.delete("key")
      subject.get("key", 0).must_be_nil
    end

    it "default_substitute!" do
      subject.default_substitute!
      subject["familylang"][0].must_equal "en"
    end

    describe "advanced" do
      let(:key_type){ String }

      it "each_key" do
        n = 0
        subject.each_key{|k|
          k.must_be_kind_of key_type
          n+=1
        }
        n.must_be :>, 0
      end

      it "each is each_key" do
        subject.each{|k|
        }
        subject.to_a.sort.must_equal subject.keys.sort
      end

      it "keys" do
        keys = subject.keys
        keys.must_be_kind_of Array
        keys.size.must_be :>,0
        keys.each{|k|
          k.must_be_kind_of key_type
        }
      end

      it "each value for key" do
        n = 0
        subject.each_value("family"){|k|
          n+=1
        }
        n.must_be :>, 0
      end
    end

  end

end