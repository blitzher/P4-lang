from lexer import Lexer

def main():
	lexer = Lexer("../tests/eggs.rcp")

	print(lexer.fields)
	
if __name__ == "__main__":
	main()