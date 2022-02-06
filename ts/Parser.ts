import { Ingredient } from "./Ingredient";
import { Lexer, TOKEN } from "./Lexer";
import { Procedure } from "./Procedure";
import { Recipe } from "./Recipe";

export class Parser {
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
      lexer_fields[TOKEN["procedure:"]],
      recipe
    );

    return recipe;
  }

  /* TODO */
  ParseProcedure(procedure: string[], recipe: Recipe) {
    const procedureArray = [];

    let procedureIndex = 0;
    let current_line = procedure[procedureIndex];
    let get_next = () => {
      current_line = procedure[++procedureIndex];
      return current_line;
    };

    while (procedureIndex < procedure.length) {
      /* Give each procedure the ability to get the next line */

      /* It can (for the time being) be assumed that each
       * procedure is constructed on a 'with(...)' statement,
       * and takes all following lines appropriately */
      procedureArray.push(
        new Procedure(current_line, recipe.ingredients, get_next)
      );
    }

    return procedureArray;
  }
}
