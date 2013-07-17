require 'minitest/autorun'
require 'fontconfig'

describe Fontconfig::Config do
  describe "class methods" do
    subject { Fontconfig::Config }
    describe "new" do
      it "works" do
        subject.new.must_be_instance_of subject
      end

      it "loads config" do
        skip
        subject.new "some_config_file"
      end

      it "raises if cannot load" do
        assert_raises(RuntimeError){
          subject.new "some_config_file"
        }
      end
    end

    it "filename" do
      res = subject.filename('')
      puts "res is #{res}"
      res.must_be_instance_of String
    end
  end

  subject { Fontconfig::Config.new }


  it "config_dirs" do
    r = subject.config_dirs
    puts "config_dirs is '#{r}'"
    r.must_be_instance_of Array
  end
end