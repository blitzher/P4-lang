# overall:
- the language is case-insensitive
- the language ignores indentation and mostly ignores white-space

# output:
- compiling a program produces a recipe card in a text format (html/txt)
- if recipes are included as ingredients, recipe cards for these will be produced for these as well

# ingredients:
- a recipe must include exactly one "ingredients:" statement
- all specified ingredients must be used in at least one with() statement
- ingredients may have a specified amount
  - this is annotated by ()
  - an amount consists of a number followed by a measuring unit
  - if no amount is specified, this is the same as explicitly writing (1)
- ingredients are comma-seperated
- ingredients may be suffixed with zero to many ingredient operators

## ingredient operators
- ingredients can be specified as optional by suffixing them with a ?
  - optional ingredients behave just like other ingredients, except that:
    - they are clearly stated as optional on the recipe card
- ingredients can be specified as self contained recipes by suffixing them with a *
  - if an ingredient is specified as a recipe, a recipe with that title must exist in the users recipe list
  - otherwise, the program will not be able to compile
- ingredients can be specified as uncountable by suffixing them with a +
  - uncountable ingredients can never be used up

## ingredient arithmetic:
- if ingredients are specified by an amount, the following is true:
  - it is impossible to use more of the ingredient than specified
  - the entire amount must be used during the procedure
  - if no amount is specified in a with() statement, it is the same as implicity writing rest

# kitchenware:
- a recipe must include zero or one "kitchenware:" statements
- all specified kitchenware must be used in at least one using() statement
- kitchenwares are comma-seperated

# nutrients:
- a recipe must include zero or one "nutrients:" statements
- this is an optional field in which users can specify:
  - calories
  - protein
  - fat
  - carbohydrates
- nutrients are comma-seperated
- calories must be specified in either cal or kcal
- the micronutrients (protein, fat, carbohydrates) must be specified in some weight measuring unit

# procedure:
- a recipe must include exactly one "procedure:" statement
- a procedure must include at least one with()/using()
- each subprocedure of the procedure is described using a with() and/or using() statements

# with():
- takes one to many input parameters
- all input parameters must be ingredients
- the input parameters may have a specified amount
  - this is written in the form: ingredient (amount)
  - the units must match with what was specified in the ingredient list (e.g. kg to g is legal, g to ml is not)
  - the specified amount may use the keywords: all, half, quarter, rest
    - these keywords operates on the initially specified amount
- input parameters are seperated by comma
- the body should specify the subprocedure
- all ingredients included as parameters must be included in the body
  - these are annotated by []
  - these may have an alternative name (an alias), following the format [alternative name : actual name]
    - an alias may be the empty string (e.g.  [: actual name])
- may end with a yield statement or an update statement, but not both

# using():
- an optional statement serving as an extention of the with() statement or a standalone statement
- takes one to many input parameters
- all input parameters must be kitchenware
- the included parameters does not have to be included in the body of the with() statement
  - however, if they are, they should be annotated by []
- if a using() statement is used without a with(), it is not possible to yield or update anything

# yield:
- an optional statement to end a with() statement
- specifies one to many output parameters
- all specified parameters are considered new ingredients
  - these must be used in a later with() statement
  - the ingredients must not have the same name as already existing ingredients
  - multiple ingredients are comma-seperated
- yielded ingredients can be suffixed with +, but not * or ?

  # update:
- an optional statement to end a with() statement
- specifies one to many output parameters
  - only the input parameters passed to the with() can be specified as output
- updating an ingredient corresponds to not consuming it
  - therefore, updated ingredients must still be consumed later
  - multiple updated ingredients are comma-seperated
- yielded ingredients can be suffixed with +, but not * or ?

# units:
- users may use any name for measuring units, however, these do not convert
- standard measurement units are defined by the language
  - these include
    - weight: g, kg, oz
    - volume: ml, dl, l, fl oz, cup, tsp, tbsp, qt, gal
    - temperature: C, F
    - length: mm, cm, in, ft
- within a with() or using() statement, units should be annotated by {}
  - this will make the units convertable between the metric system and the US imperial system
    - the amount must be specified on the form {number unit}
- all units are named using their standard abbreviation without periods
- no warning will be given if units from different systems are used in the same recipe

# compilation flags:
- a recipe may be compiled with the flag -metric or -imperial

# tags:
- a recipe may include zero to one "tags:" statements
- optional descriptors of the recipe
- these are comma-seperated

# time:
- a recipe may include zero to one "time:" statements
- optional statement
- prep time should specify how much of the time in the recipe is used for preperation
- cook time should specify how much of the time in the recipe is used for cooking
- total time = prep time + cook time
- if only prep time, cook time or total time is specified, it will appear on the recipe card as total time
- if total time and cook time is specified, prep will be calculated automatically and vice versa
- prep time + cook time must be equal to total time
- time may be specified as a range by use of a dash (e.g. 2-3 hours)
  - when used in a calculation, the average will be used (e.g. prep time: 20-30 min, cook time: 15 min, equals total time: 40 min)

# name:
- names of kitchenware and ingredients must consist of only A-Za-z0-9%-_ and whitespace
- names of units must consist of only A-Za-z-_ and whitespace

# encapsulation:
- to differentiate between different parts of the program, the compiler identifies specific keywords
  - title:
  - description:
  - amount:
  - cook time:
  - prep time:
  - total time:
  - tags:
  - ingredients:
  - kitchenware:
  - procedure:
  - yield:
  - with()
  - using()
- subprocedure headers must be on one of these forms:
  - with():
  - using():
  - with() using():
- once a keyword has been recognized, the compiler reads until it recognizes another keyword
- depending on the keyword, the compiler will look for different syntax in between the keywords
- procedure must be specified after both ingredients and kitchenware must be updated
