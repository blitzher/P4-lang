# Features

## Must have

- Add a title and optionally subtitle for each recipe
- Portion sizes, either to number of people or amount of
- Options to scale recipes to a number of people,
  with recursive ingredients scaling appropriately
- Language constructs for different kitchen containers and
  and procedures which interact with these as environments
- Generate a complete list of ingredients,
- Generate a set of instructions
- Disallow unused and surplus of ingredients throughout the
  recipe procedure

- Allow importing of other recipes as ingredients, which add their final yield to the ingredient list

### Procedure specifics

- Procedures take some input ingredients, with an environment and yield.
- Procedures produce a yield, which gets added as an intermediate ingredient
- The states of the environment are explicitly changed, and do not have a yield
- When the ingredients and intermediate ingredients are complete, they are consumed with the 'done' statement

#TODO:
consider utensils

## Should have

- Generate warnings for container sizes, when
- Conversions between different meassurement systems
  (metric, imperial)

## Could have

- Fractions for output

## Won't have

-
