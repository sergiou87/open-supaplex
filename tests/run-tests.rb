#!/usr/bin/env ruby

require 'thread'
require 'etc'

if ARGV.length < 1 then
	puts "Missing executable path. Usage: run_tests.rb <path_to_OpenSupaplex>"
	exit 1
end

$gameBinaryPath = ARGV[0]

if not File.file?($gameBinaryPath) then
	puts "#{$gameBinaryPath} is not a valid file"
	exit 1
end

$gamePath = File.dirname($gameBinaryPath)
$gameBinary = File.basename($gameBinaryPath)

$maxSimultaneousTests = Etc.nprocessors * 8

puts "Running #{$maxSimultaneousTests} tests simultaneously"

$solvesPath = "solves"

$totalTests=0
$totalPassedTests=0
$totalFailedTests=0
$failedTests = []
$globalMutex = Mutex.new

def runSuite(suite, expectedExitCode = 0, binary = $gameBinary, pattern = "*.[sS][pP]")
	suiteMutex = Mutex.new
	
	suiteTotalPassedTests = 0
	suiteTotalFailedTests = 0
	suiteFailedTests = []
	suiteTotalTests = 0

	runTest = Proc.new { |path, mutex|
		system("./#{binary}", "-u", File.expand_path(path), chdir: $gamePath)
		status = $?
		result = status && status.exited? && status.exitstatus == expectedExitCode

		testName = File.basename(path)

		mutex.synchronize do
			if result then
				suiteTotalPassedTests += 1
			else
				suiteTotalFailedTests += 1
				suiteFailedTests += [testName]
			end
			suiteTotalTests += 1
		end
	}
	
	puts "Starting with suite '#{suite}'…"

	threads = []

	Dir.glob("#{suite}/#{pattern}") do |testPath|
		thread = Thread.new do
			runTest.call(testPath, suiteMutex)
		end

		threads += [thread]

		while threads.length() >= $maxSimultaneousTests
			oldestThread = threads.shift
			threads.first.join()
		end
	end

	for t in threads do
		t.join()
	end	
	
	puts "Finished test suite '#{suite}':"
	puts " - Passed: #{suiteTotalPassedTests}"
	puts " - Failed: #{suiteTotalFailedTests}"
	puts " - Total:  #{suiteTotalTests}"
	puts ""
	puts "Failed tests: #{suiteFailedTests}"
	puts ""

	$globalMutex.synchronize do
		$totalTests += suiteTotalTests
		$totalPassedTests += suiteTotalPassedTests
		$totalFailedTests += suiteTotalFailedTests

		for failedTest in suiteFailedTests
			$failedTests += [ "#{suite}/#{failedTest}" ]
		end
	end
end

threads = []

Dir.foreach($solvesPath) do |suite|
	next if suite == '.' or suite == '..' or !File.directory?("#{$solvesPath}/#{suite}")

	runSuite("#{$solvesPath}/#{suite}")
end

runSuite("regressions/solves")
runSuite("regressions/failures", 1)
runSuite("regressions/debug-solves", 0, "debug-base-tests")
runSuite("regressions/debug-failures", 1, "debug-base-tests")
runSuite("regressions/menu-solves", 0, "menu-throttle-tests", "*.csv")

puts "Finished all tests!"
puts " - Passed: #{$totalPassedTests}"
puts " - Failed: #{$totalFailedTests}"
puts " - Total:  #{$totalTests}"
puts ""
puts "Failed tests: #{$failedTests}"

if $totalFailedTests > 0 then
	exit 1
else
	exit 0
end
