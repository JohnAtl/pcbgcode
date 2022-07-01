# _*_ Mode: Ruby -*-
#
# Rakefile for pcb-gcode.
#
require 'pp'
require 'rake/clean'

# this will be improved later
PCB_GCODE_VERSION = "3.6.2.4"
VERSION_KEYWORD = '$VERSION$'

RELEASE_FILE = "~/Documents/pcb-gcode-#{PCB_GCODE_VERSION}.zip"

ignore_files = ['pcb_gcode_is_setup', '*.old', 'storage.nv',
  'make/*', 'make', 'make/',
  '*.b#*', '*.s#*', '*.l#*',
  '*.DS_Store', 'optomize_me.txt',
  '*.svn*',
  'docs/pcbgcode.aux', 'docs/pcbgcode.glo', 'docs/pcbgcode.gls', 'docs/pcbgcode.idx',
  'docs/pcbgcode.ilg', 'docs/pcbgcode.ind', 'docs/pcbgcode.lof', 'docs/pcbgcode.log', 
  'docs/pcbgcode.lot', 'docs/pcbgcode.out', 'docs/pcbgcode.toc',
  'docs/figs/*',
  'build', 'build/*', 'build/'
]

desc "Build the docs/pcbgcode.pdf file."
file 'docs/pcbgcode.pdf' => 'docs/pcbgcode.tex' do |t|
  system "cd docs && pdflatex pcbgcode"
  system "cd docs && makeindex pcbgcode"
  system "cd docs && pdflatex pcbgcode"
  system "cd docs && pdflatex pcbgcode"
end

desc "Create the .zip file to be released."
task :release_file do
  zip_cmd = "rm ../pcb-gcode-#{PCB_GCODE_VERSION}.zip"
  system(zip_cmd)
  zip_cmd = "zip -r ../pcb-gcode-#{PCB_GCODE_VERSION}.zip *"
  zip_cmd += ' -x '
  zip_cmd += ignore_files.join(' -x ')
  system(zip_cmd)
  zip_cmd = "unzip ../pcb-gcode-#{PCB_GCODE_VERSION} -d ../pcb-gcode-#{PCB_GCODE_VERSION}"
  system(zip_cmd)
  zip_cmd = "rm ~/Documents/pcb-gcode-#{PCB_GCODE_VERSION}.zip"
  system(zip_cmd)
  zip_cmd = "cd .. && zip -r ~/Documents/pcb-gcode-#{PCB_GCODE_VERSION}.zip pcb-gcode-#{PCB_GCODE_VERSION}"
  system(zip_cmd)
end

desc "Copy current settings/* files to the safe_options folder."
task :safe_options do
  SAFE_OPTIONS = ['pcb-defaults', 'pcb-machine', 'pcb-gcode-options', 'user-gcode']
  SAFE_OPTIONS.each do |name|
    cp 'settings/' + name + '.h', 'safe_options/' + name + '.release.h'
  end
end

desc "Recreate data folders for viewers. Usually after re-exporting from Processing."
task :fix_viewers do
  system("cp -R viewer/data viewer/applet")
  system("cp -R viewer/data viewer/application.linux/")
  system("cp -R viewer/data viewer/application.macosx/")
  system("cp -R viewer/data viewer/application.windows/")
end

desc "Write the convert units function in pcb-gcode-setup.ulp."
task :write_convert_units do
  system("make/write_convert_units.rb")
end

def inplace_edit(filename, from_keyword, to_keyword, options={})
  File.open(filename + ".new", "w") { |output|
    File.open(filename).each_line { |line|
      output << line.gsub(from_keyword, to_keyword)
    }
  }

  if options.has_key?(:keep_backup_in)
    FileUtils.mv(filename, options[:keep_backup_in])
    File.rename(filename + ".new", filename)
  else
    if FileTest.exists?("pcb-gcode-setup.old")
      File.delete("pcb-gcode-setup.old")
    end
    File.rename(filename, filename + ".old")
    File.rename(filename + ".new", filename)
    File.delete(filename + ".old")
  end
end

desc "Update version numbers in relevant files."
task :insert_version_numbers do
  inplace_edit("docs/pcbgcode.tex", VERSION_KEYWORD, PCB_GCODE_VERSION, :keep_backup_in=>'build')
  inplace_edit("pcb-gcode.ulp", VERSION_KEYWORD, PCB_GCODE_VERSION, :keep_backup_in=>'build')
  inplace_edit("pcb-gcode-setup.ulp", VERSION_KEYWORD, PCB_GCODE_VERSION, :keep_backup_in=>'build')
end

desc "Remove version numbers from files"
task :revert_version_numbers do
  FileUtils.mv('build/pcbgcode.tex', 'docs')
  FileUtils.mv('build/pcb-gcode.ulp', '.')
  FileUtils.mv('build/pcb-gcode-setup.ulp', '.')
end

desc "Update the saved settings default.* files"
task :update_saved_defaults do
  FileUtils.cp('settings/pcb-defaults.h', 'settings/saved/default.def')
  FileUtils.cp('settings/pcb-machine.h', 'settings/saved/default.mac')
end

CLEAN.include('docs/pcbgcode.aux', 'docs/pcbgcode.glo', 'docs/pcbgcode.gls', 'docs/pcbgcode.idx',
    'docs/pcbgcode.ilg', 'docs/pcbgcode.ind', 'docs/pcbgcode.lof', 'docs/pcbgcode.log', 
    'docs/pcbgcode.lot', 'docs/pcbgcode.out', 'docs/pcbgcode.toc'
)

CLOBBER.include("~/Documents/pcb-gcode-#{PCB_GCODE_VERSION}.zip", "../pcb-gcode-#{PCB_GCODE_VERSION}",
    "../pcb-gcode-#{PCB_GCODE_VERSION}.zip"
)

task :default => [:insert_version_numbers, 
                  'docs/pcbgcode.pdf', 
                  :fix_viewers, 
                  :safe_options, 
                  :update_saved_defaults,
                  :write_convert_units, 
                  :release_file,
                  :revert_version_numbers] do
end
