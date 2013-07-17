require 'minitest/autorun'
require 'test_helper'

describe Fontconfig::ObjectSet do
  describe "class methods" do
    subject { Fontconfig::ObjectSet }
    describe "new" do
      it "works" do
        subject.new.must_be_instance_of subject
      end

      it "init by array of strings" do
        arr = %w{str1 str2 str3}.sort
        (s = subject.new(arr)).must_be_instance_of subject
        s.to_a.sort.must_equal arr
      end
    end
  end

  subject { Fontconfig::ObjectSet.new }

  it "add" do
    subject << "str"
    subject.to_a.must_equal ["str"]
  end
end
