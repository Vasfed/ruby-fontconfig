require 'bundler/setup'

require 'minitest/autorun'
require 'fontconfig'

describe Fontconfig do
  subject { Fontconfig }

  it "should be" do
    subject.wont_be_nil
  end


end
