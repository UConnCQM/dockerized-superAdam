#!/usr/bin/ruby
require_relative './algorun/Task.rb'
require_relative './algorun/RandomSampling.rb'
require_relative './algorun/Server.rb'

if ENV["RS_HOME"].nil? then
	ENV["RS_HOME"]=ENV["PWD"]
end
HOME=ENV["RS_HOME"]

class String
	# colorization
	def colorize(color_code)
		"\e[#{color_code}m#{self}\e[0m"
	end

	def red
		colorize(31)
	end

	def green
		colorize(32)
	end

	def yellow
		colorize(33)
	end

	def pink
		colorize(35)
	end
end

def run_test(num)
	return "test not implemented"
end

param=ARGV[0]
if param.nil? then param="help" end
case param
	when "help"
		puts "RandomSampling computes the basin of attractors for large systems using random sampling."
		puts ""
		puts "Usage: ./run.rb [help|check|<input_file.json>]"
		puts ""
		puts "* check: run all json tests file against RandomSampling and compares them to reference output files"
		puts "* test <test_number>: run test #<test_number>"
		puts "* <input_file.json>: run RandomSampling with the input JSON file"
		puts "* clean: remove temporary and compiled files"
		puts "* help: shows this help message"
	when "test"
		num=ARGV[1]
		if not ["1","2","3"].include? num then
			puts "Test number does not exist. Please choose a number between 1 and 3."
			exit
	       	end
		begin
			puts run_test(num)
		rescue StandardError=>e
			STDERR.puts "ERROR: "+e.to_s
		end
	when "check"
		puts "tests not implemented"
		#tests=[]
		#begin
		#	["1","2","3"].each do |i|
		#		tests.push(run_test(i))
		#		puts tests.last
		#	end
		#	tests.each { |p| puts p }
		#rescue StandardError=>e
		#	STDERR.puts "ERROR: "+e.to_s
		#end
	when "clean"
		Dir.chdir HOME
		system("rm -f output.json input.json")
	when "start"
		server = WEBrick::HTTPServer.new(:Port => 8765, "RequestTimeout" => 300, :DocumentRoot => HOME+"/algorun/web/")
		server.mount("/do", Algorun)
		trap("INT") {
			server.shutdown
		}
		server.start()
	else
		if !File.exists?(param) or File.directory?(param) then
			puts ("ERROR: No valid JSON input file given")
		else
			input = File.read(param)
			json = JSON.parse(input)
			begin
				task=RandomSampling.new(json,HOME+'/src/RandomSampling.pl')
				task.run(HOME+"/output.json")
				puts JSON.dump(task.render_output(HOME+"/output.json"))
			rescue StandardError=>e
				STDERR.puts "ERROR: "+e.to_s
			end
		end
end
