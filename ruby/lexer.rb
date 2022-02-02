
module Lexer

	TOKENS = [
		'title:',
		'subtitle:',
		'for:',
		'ingredients:',
		'procedure:'
	]

	class Lexer

		@current_field = "unfielded"
		@fields = {}

		def initialize(filename = "")

			@fields = Hash.new
			if filename == ""
				raise "No filename for lexer!"
			end
			
			file = File.open("../tests/eggs.rcp")
			file_data = file.read
			file.close

			for line in file_data.split "\n" do
				lex_line line
			end
		end

		def lex_line(line)
			line_split = line.split " "

			if TOKENS.include? line_split[0]
				@current_field = line_split[0]
				line_split = line_split.drop 1
			end
			
			
			if @fields.key @current_field == nil
				@fields[@current_field] = Array.new
			end

			@fields[@current_field].push line_split

		end


	end
end