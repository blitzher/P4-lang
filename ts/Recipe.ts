import { Ingredient } from "./Ingredient";
import { Procedure } from "./Procedure";

const fs = require("fs");

export class Recipe {
  title: string; // The title of the recipe
  subtitle: string; // The title of the recipe
  for: number; // The amount of people the recipe is for
  ingredients: Ingredient[]; // The ingredients
  procedure: Procedure[];

  constructor() {
    this.ingredients = [];
    this.procedure = [];
  }
}
