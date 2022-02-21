overall:

- the language is case-insensitive
- the language kinda ignores white-space

output:

- compiling a program produces a recipe card in a text format (html/txt)
- if recipes are included as ingredients, recipe cards for these will be produced for these as well

ingredients:

- must not include , \ \* ? [] {} () :
- must be used in at least one with() statement
- may have a specified amount
  - this is annotated by ()
  - an amount consists of a number followed by a measuring unit
    - these are seperated by whitespace
  - a number may be a fraction (e.g. 2/3)
- if no amount is specified, the ingredient will be uncountable
  - uncountable ingredients can never be used up
- ingredients are comma-seperated
- ingredients can be specified as optional by suffixing them with a ?
  - optional ingredients behave just like other ingredients
  - the only difference is that they are clearly stated as optional on the recipe card
- ingredients can be specified as self contained recipes by suffixing them with a \*
  - if an ingredient is specified as a recipe, a recipe with that title must exist in the users recipe list
  - otherwise, the program will not be able to compile

ingredient arithmetic:

- if ingredients are specified by an amount, the following is true:
  - it is impossible to use more of the ingredient than specified
  - the entire amount must be used during the procedure
  - if no amount is specified in a with() statement, it is the same as implicity writing all

kitchenware:

- must not include , \ \* ? [] {} () :
- must be used in at least one using() statement
- are comma-seperated

procedure:

- each step of the procedure is described using a with() statements
- The procedure must end with a done() statement

with():

- takes zero to many input parameters
- all input parameters must be ingredients
- the input parameters may have a specified amount
  - this is written in the form: ingredient (amount)
  - the units must match with what was specified in the ingredient list (e.g. kg to g is legal, g to ml is not)
  - if no unit is specified, it is assumed that the unit is the same as was specified in the ingredient list
  - the specified amount may use the keywords: all, half, quarter, rest
    - these keywords operates on the initially specified amount
- input parameters are seperated by comma
- the body should specify the procedure
- all ingredients included as parameters must be included in the body
  - these are annotated by []
  - these may have an alternative name, following the format [alternative name : actual name]
- may end with a yield statement or an update statement, but not both

using():

- an optional statement serving as an extention of the with() statement
- takes zero to many input parameters
- all input parameters must be kitchenware
- the included parameters does not have to be included in the body of the with() statement
  - however, if they are, they should be annotated by []

update:

- an optional statement to end a with() statement
- specifies zero to many output parameters
  - only the input parameters passed to the with() can be specified as output
- updating an ingredient corresponds to not consuming it
  - therefore, updated ingredients must still be consumed later
  - multiple updated ingredients are comma-seperated
- yielded ingredients can be suffixed with +, but not \* or ?

yield:

- an optional statement to end a with() statement
- specifies zero to many output parameters
- all specified parameters are considered new ingredients
  - these must be used in a later with() statement or in the done statement
  - the ingredients must not have the same name as already existing ingredients
  - multiple ingredients are comma-seperated

done:

- the final statement of the program
- specifies zero to one output parameter
  - this ingredient must be an ingredient not yet consumed
    - it must be the exact same name as the title of the recipe
- when using a recipe as an ingredient, this statement specifies what that recipe gives you
- if no parameter is given, the title of the recipe will automatically be parsed
  - it still must have been yielded previously
- the program terminates once the last parenthesis of the done has been read

units:

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

compilation flags:

- a recipe may be compiled with the flag -metric or -imperial

tags:

- optional descriptors of the recipe

time:

- optional statement
- prep time should specify how much of the time in the recipe is used for preperation
- cook time should specify how much of the time in the recipe is used for cooking
- total time = prep time + cook time
- if only prep time, cook time or total time is specified, it will appear on the recipe card as total time
- if total time and cook time is specified, prep will be calculated automatically and vice versa
- prep time + cook time must be equal to total time
- time may be specified as a range by use of a dash (e.g. 2-3 hours)
  - when used in a calculation, the average will be used (e.g. prep time: 20-30 min, cook time: 15 min, equals total time: 40 min)

calories:

- optional statement
- user may specify total amount or amount pr. serving

encapsulation:

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
  - done:
  - (all)
  - (half)
  - (quarter)
  - (rest)
  - with()
  - using()
- subprocedure headers must be on one of these forms:
  - with():
  - using():
  - with() using():
- procedure must be the last part of the program
- once a keyword has been recognized, the compiler reads until it recognizes another keyword
  - the only exception is when it reads done, which makes it produce the recipe card and terminate the program
- depending on the keyword, the compiler will look for different syntax in between the keywords

notes:

- syntax highlighter must recognize - % and /
- can you have an empty with?
- can an alias be empty ([:some ingredient])
- why is calories a keyword, but not e.g. "tips" or "nutrional content" or "durability"

extra notes:
procedure:

- each step of the procedure is described using a with() statements /// wrong
- The procedure must end with a done() statement

with():

- takes zero to many input parameters /// one to many?

using():

- an optional statement serving as an extention of the with() statement
- takes zero to many input parameters

update & yield:

- an optional statement to end a with() statement
- specifies zero to many output parameters

encapsulation:
-subprocedure
