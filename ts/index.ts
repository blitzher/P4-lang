import { Lexer } from "./Lexer";
import { Parser } from "./Parser";

function main() {
  const lexer = new Lexer("../tests/eggs.rcp");
  const parser = new Parser(lexer);

  const recipe = parser.Parse();

  return;
}

main();
