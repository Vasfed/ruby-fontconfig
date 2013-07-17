require 'mkmf'
require 'pkg-config'

PKGConfig.have_package('fontconfig')

create_makefile('fontconfig')