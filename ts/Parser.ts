import { Lexer, TOKEN } from "./Lexer";

class Parser {
  /**
   * Parse the lexed recipe to its proper objects
   */

  lexer: Lexer;

  constructor(_lexer: Lexer) {
    this.lexer = _lexer;
  }

  Parse(): Recipe {
    const recipe = new Recipe();

    const lexer_fields = this.lexer.fields;

    recipe.title = lexer_fields[TOKEN["title:"]].join(" ");
    recipe.subtitle = lexer_fields[TOKEN["subtitle:"]].join(" ");
    recipe.for = Number.parseInt(lexer_fields[TOKEN["for:"]][0]);

    for (const ingredient of lexer_fields[TOKEN["ingredients:"]]) {
      recipe.ingredients.push(new Ingredient(ingredient));
    }

    recipe.procedure = this.ParseProcedure(
      lexer_fields[TOKEN["procedure:"]].join(" ")
    );

    return recipe;
  }

  /* TODO */
  ParseProcedure(procedure: string) {
    const procedureArray = [];

    return procedureArray;
  }
}
