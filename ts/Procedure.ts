import { Ingredient } from "./Ingredient";

export class Procedure {
  body: string;

  /**
   *
   */
  constructor(
    procedure: string,
    recipeIngredients: Ingredient[],
    get_next: () => string
  ) {
    /* RegEx is readable (Copium) 
    Use 
    https://regex101.com/       and
    https://www.debuggex.com/
    for debugging
    */
    const procedureDestructure = /with\(((?:[\(\w+\)]+[,\s]*)+)\):/;
    const ingredientDestructure = /(\w+)\((\w+)\)/g; /* 'g' flag for multiple */

    const procedureInternalMatch = procedure.match(procedureDestructure);
    if (!procedureInternalMatch) {
      throw new SyntaxError(`Invalid procedure header ${procedure}`);
    }

    /* Get the first matching group, i.e. the ingredients and their quantifiers */
    const procedureInternal = procedureInternalMatch[1];
    /* Destructure the ingredients into atoms */
    const procedureIngredients = Array.from(
      procedureInternal.matchAll(ingredientDestructure)
    );

    procedureIngredients.forEach(([_, quantifier, ingredient]) => {
      console.log(quantifier, ingredient);
    });
  }
}
