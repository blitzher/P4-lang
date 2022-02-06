import { Quantity } from "./Quantity";

export class Ingredient {
  amount: Quantity; // The amount of the ingredient
  type: string; // The type of the ingredient, by its name

  // Take a string, such as "2 eggs"
  // and create the relevant object
  constructor(ingredient_s: string) {
    const [amount_s, type] = ingredient_s.trim().split(" ");

    this.type = type;
    this.amount = new Quantity(amount_s);
  }
}
