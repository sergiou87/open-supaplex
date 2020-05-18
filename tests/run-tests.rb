#!/usr/bin/env ruby

require 'thread'

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

$maxSimultaneousTests = 8

$solvesPath = "solves"

$totalTests=0
$totalPassedTests=0
$totalFailedTests=0
$globalMutex = Mutex.new

def runSuite(suite)
	suiteMutex = Mutex.new
	
	suiteTotalPassedTests = 0
	suiteTotalFailedTests = 0
	suiteFailedTests = []
	suiteTotalTests = 0

	runTest = Proc.new { |path, mutex|
		result = system("cd #{$gamePath} && ./#{$gameBinary} -u \"#{File.expand_path(path)}\"") # || exit # stop at first failure

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

	Dir.glob("#{$solvesPath}/#{suite}/*.[sS][pP]") do |testPath|
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
	end
end

threads = []

Dir.foreach($solvesPath) do |suite|
	next if suite == '.' or suite == '..' or !File.directory?("#{$solvesPath}/#{suite}")

	runSuite(suite)
end

puts "Finished all tests!"
puts " - Passed: #{$totalPassedTests}"
puts " - Failed: #{$totalFailedTests}"
puts " - Total:  #{$totalTests}"
