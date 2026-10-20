#!/usr/bin/env ruby

require 'fileutils'
require 'open3'
require 'rbconfig'
require 'tmpdir'

runner = File.expand_path('run-tests.rb', __dir__)

Dir.mktmpdir('opensupaplex-runner-test-') do |directory|
	suite = File.join(directory, 'solves', 'scheduler')
	FileUtils.mkdir_p(suite)
	8.times do |index|
		File.write(File.join(suite, "%02d.sp" % index), '')
	end

	game = File.join(directory, 'fake-game')
	File.write(game, <<~RUBY)
		#!/usr/bin/env ruby
		if File.basename(ARGV.fetch(1)) == '00.sp'
			exit 1
		end
		exit 0
	RUBY
	File.chmod(0755, game)

	# Pending workers finish only when joined, so process timing cannot mask a skipped join.
	loader = <<~RUBY
		def Etc.nprocessors; 1; end
		class PendingRunnerWorker
			def initialize(&work)
				@work = work
				@joined = false
			end
			def join
				unless @joined
					@joined = true
					@work.call
				end
				self
			end
		end
		class << Thread
			def new(&work)
				PendingRunnerWorker.new(&work)
			end
		end
		load ARGV.shift
	RUBY
	output, status = Open3.capture2e(
		RbConfig.ruby, '-retc', '-e', loader, runner, game, chdir: directory
	)
	expected = "Finished all tests!\n - Passed: 7\n - Failed: 1\n - Total:  8\n"
	unless status.exitstatus == 1 && output.include?(expected)
		abort "Runner skipped the delayed oldest worker or its failure:\n#{output}"
	end
end

puts 'Runner waited for all eight workers and reported the delayed failure.'
