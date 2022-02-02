from locale import currency


TOKEN = [
	"title:",
	"subtitle:",
	"for:",
	"ingredients:",
	"procedure:"
]

class Lexer:

	fields = {}
	current_field = "unfielded"

	def __init__(self, file) -> None:
		with open(file, "r") as handle:
			file_data = handle.readlines()
		
		for line in file_data:
			self.LexLine(line)



	def LexLine(self, line: str):
		
		line = line.strip() 
		line_split = line.split(" ")

		if (line_split[0] in TOKEN):
			self.current_field = line_split[0]
			line_split = line_split[1:]

		if (self.current_field not in self.fields):
			self.fields[self.current_field] = []


		self.fields[self.current_field].append(" ".join(line_split))
