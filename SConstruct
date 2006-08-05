# SConstruct for Nonpareil
# $Id$
# Copyright 2004, 2005 Eric L. Smith <eric@brouhaha.com>

# Nonpareil is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 2 as
# published by the Free Software Foundation.  Note that I am not
# granting permission to redistribute or modify Nonpareil under the
# terms of any later version of the General Public License.

# Nonpareil is distributed in the hope that they will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program (in the file "COPYING"); if not, write to
# the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
# Boston, MA 02111, USA.

release = '0.78'  # should get from a file, and use only if a release option
                  # is specified

conf_file = 'nonpareil.conf'

#-----------------------------------------------------------------------------
# Options
#-----------------------------------------------------------------------------

opts = Options (conf_file)

opts.AddOptions (EnumOption ('target',
			     help = 'execution target platform',
			     allowed_values = ('posix', 'win32'),
			     default = 'posix',
			     ignorecase = 1),

		 PathOption ('prefix',
			     'installation path prefix',
			     '/usr/local'),

		 # Don't use PathOption for other paths, because we don't
		 # require the directories to preexist.
		 ('bindir',
		  'path for executable files (default is $prefix/bin)',
		  ''),

		 ('libdir',
		  'path for library files (default is $prefix/lib/nonpareil)',
		  ''),

		 ('destdir',
		  'installation virtual root directory (for packaging)',
		  ''),

		 BoolOption ('debug',
			     help = 'compile for debugging',
			     default = 1),

		 # Feature switches:

		 BoolOption ('has_debugger_gui',
			     help = 'enable debugger GUI interface',
			     default = 0),

		 BoolOption ('has_debugger_cli',
			     help = 'enable debugger command-line interface',
			     default = 0),

		 BoolOption ('use_tcl',
			     help = 'use Tcl as debug command interpreter (only when debugger CLI is enabled)',
			     default = 1),  # only if has_debugger_cli

		 BoolOption ('use_readline',
			     help = 'use Readline library for command editing and history (only when debugger CLI is enabled)',
			     default = 1))  # only if has_debugger_cli

#-----------------------------------------------------------------------------
# Cache options
#-----------------------------------------------------------------------------

env = Environment (options = opts)
opts.Update (env)
opts.Save (conf_file, env)

#-----------------------------------------------------------------------------
# Generate help text from options
#-----------------------------------------------------------------------------

Help (opts.GenerateHelpText (env))

#-----------------------------------------------------------------------------
# More defaults and variable settings
#-----------------------------------------------------------------------------

# Don't scatter .sconsign files everywhere, and especially don't put them
# into install directories.
SConsignFile ()

env ['RELEASE'] = release

#-----------------------------------------------------------------------------
# Add some builders to the environment:
#   tarball
#   NSIS
#-----------------------------------------------------------------------------

Export ('env')
SConscript ('scons/ncz.py')
SConscript ('scons/tarball.py')
# SConscript ('scons/nsis.py')

#-----------------------------------------------------------------------------
# package a release source tarball
#-----------------------------------------------------------------------------

bin_dist_files = Split ("""README COPYING CREDITS""")

src_dist_files = Split ("""INSTALL DEBUGGING TODO SConstruct""")

source_release_dir = env.Distribute ('nonpareil-' + release,
				     bin_dist_files + src_dist_files)

source_release_tarball = env.Tarball ('nonpareil-' + release + '.tar.gz',
                                      source_release_dir)

env.Alias ('srcdist', source_release_tarball)

env.AddPostAction (source_release_tarball, Delete (source_release_dir))

#-----------------------------------------------------------------------------
# package a source snapshot tarball
#-----------------------------------------------------------------------------

import time

snap_date = time.strftime ("%Y.%m.%d")

snapshot_dir = env.Distribute ('nonpareil-' + snap_date, src_dist_files)

snapshot_tarball = env.Tarball ('nonpareil-' + snap_date + '.tar.gz',
                                snapshot_dir)

env.Alias ('srcsnap', snapshot_tarball)

env.AddPostAction (snapshot_tarball, Delete (snapshot_dir))

#-----------------------------------------------------------------------------
# package a Windows binary distribution ZIP file
#-----------------------------------------------------------------------------

if env ['target'] == 'win32':
    win32_bin_dist_dir = Dir ('nonpareil-' + release + '-win32')
    Export ('win32_bin_dist_dir')
    Install (win32_bin_dist_dir, bin_dist_files)
    win32_bin_dist_zip = Zip ('nonpareil-' + release + '-win32.zip', win32_bin_dist_dir)
    env.Alias ('dist', win32_bin_dist_zip)
    env.AddPostAction (win32_bin_dist_zip, Delete (win32_bin_dist_dir.path))

#-----------------------------------------------------------------------------
# package a Windows installer
#-----------------------------------------------------------------------------

#if env ['target'] == 'win32':
#    win32_nsis_installer_fn = 'nonpareil-' + release + '-setup.exe'
#    win32_nsis_installer = env.MakeNSISInstaller (win32_nsis_installer_fn,
#                                                  'src/nonpareil.nsi')
#    env.Alias ('installer', win32_nsis_installer)
#    env.AddPostAction (win32_nsis_installer, Delete (win32_bin_dist_dir.path))

#-----------------------------------------------------------------------------
# Installation paths
#-----------------------------------------------------------------------------

if not env ['bindir']:
	env ['bindir'] = env ['prefix'] + '/bin'

if not env ['libdir']:
	env ['libdir'] = env ['prefix'] + '/lib/nonpareil'

#-----------------------------------------------------------------------------
# Prepare for SConscription
#-----------------------------------------------------------------------------

Export ('source_release_dir snapshot_dir')

host_build_dir = 'build/' + env ['PLATFORM']
target_build_dir = 'build/' + env ['target']

if env ['debug']:
	host_build_dir += '-debug'
	target_build_dir += '-debug'

#-----------------------------------------------------------------------------
# sound files
#-----------------------------------------------------------------------------

SConscript ('sound/SConscript')

env.Append (GPLv2 = File ('COPYING'))

#-----------------------------------------------------------------------------
# host platform code
#-----------------------------------------------------------------------------

env.Append (KML_41CV = File ('calc/nut/41cv/41cv.kml'))

native_env = env.Copy ()
native_env ['build_target_only'] = 0
SConscript ('src/SConscript',
            build_dir = host_build_dir,
            duplicate = 0,
	    exports = {'build_env' : native_env,
		       'native_env' : env})

#-----------------------------------------------------------------------------
# ROM sources - assemble with host tools
#-----------------------------------------------------------------------------

SConscript ('scons/uasm.py')

#w25_rom = env.UASM (target = 'calc/woodstock/25/25.rom',
#		    source = 'calc/woodstock/25/25.asm')
#Default (w25_rom)

#-----------------------------------------------------------------------------
# the calculators
#-----------------------------------------------------------------------------

all_calcs = {'classic':    ['35', '45', '55', '80'],
	     'woodstock':  ['21', '25'],
	     'spice':      ['32e', '33c', '34c', '37e', '38c', '38e'],
	     'nut':        ['41cv', '41cx'],
	     'voyager':    ['11c', '12c', '15c'] }

ncz_files = []

for family in all_calcs:
	for model in all_calcs [family]:
		print family, model
		model_dir = Dir ('calc/' + family + '/' + model)
		kml = FindFile (model + '.kml', model_dir)
		# parse kml to find ROM, image files, etc.
		ncz_files += env.NCZ (target = 'build/calc/' + model + '.ncz',
				      source = 'calc/' + family + '/' + model + '/' + model + '.kml');
#		SConscript ('calc/SConscript',
#			    exports = 'model',
#			    src_dir = 'calc/' + family + '/' + model,
#			    build_dir = 'build/calc/' + family + '/' + model,
#			    duplicate = 0)

Default (ncz_files)

#-----------------------------------------------------------------------------
# target platform code if cross-compiling
#-----------------------------------------------------------------------------

if (env ['PLATFORM'] != env ['target']):
	cross_build_env = env.Copy ()
	cross_build_env ['build_target_only'] = 1
	SConscript ('src/SConscript',
		    build_dir = target_build_dir,
		    duplicate = 0,
		    exports = {'build_env': cross_build_env,
			       'native_env' : env})

#-----------------------------------------------------------------------------
# Windows DLLs
#-----------------------------------------------------------------------------

SConscript ('win32/dll/SConscript',
	    build_dir = target_build_dir + '/dll',
	    duplicate = 0)

#-----------------------------------------------------------------------------
# documentation
#-----------------------------------------------------------------------------

SConscript ('doc/SConscript')

#-----------------------------------------------------------------------------
# scons directory, which contains various scons builders we use, and the
# scons-local tarball for those that don't want to install SCons
#-----------------------------------------------------------------------------

SConscript ('scons/SConscript')
