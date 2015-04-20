require 'net/http'
require 'json'

# sends a test JSON file to the REACT docker container on the ip and port given in parameter and return the response

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
end


def display_help
	puts "ABOUT: api_call_test.rb sends <json_file> to <ip>:<dest_port> and returns the response"
	puts "USAGE: ./api_call_test.rb <json_file> <ip> <dest_port>"
	puts "EXAMPLE: ./api_call_test.rb test1/REACT_Input_Example1.json 0.0.0.0 31331"
end

json_file=ARGV[0]
ip=ARGV[1]
port=ARGV[2]
if json_file.nil? or !File.exists?(json_file) or File.directory?(json_file) then
	puts "ERROR: invalid json file provided. Please check that the file exists and is valid.".red()
	display_help()
	exit
end
if ip.nil? then
	puts "ERROR: destination IP missing.".red()
	display_help()
	exit
end
if port.nil? then
	puts "WARNING: No port specified, defaulting to port 80".yellow()
	port="80"
end
input = File.read(json_file)
json = JSON.parse(input)
uri = URI("http://"+ip+":"+port+"/do/run")
res = Net::HTTP.post_form(uri, 'input' => input)
puts res.body
