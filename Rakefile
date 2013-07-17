#!/usr/bin/env rake
require "bundler/gem_tasks"

require 'rake/extensiontask'
Rake::ExtensionTask.new('fontconfig')

require 'rake/testtask'

Rake::TestTask.new(:test) do |t|
  t.pattern = 'spec/**/*_spec.rb'
  t.libs.push 'spec'
end


task :test => :compile

task :default => :test
