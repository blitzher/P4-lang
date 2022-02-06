"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.Parser = void 0;
var Ingredient_1 = require("./Ingredient");
var Lexer_1 = require("./Lexer");
var Procedure_1 = require("./Procedure");
var Recipe_1 = require("./Recipe");
var Parser = /** @class */ (function () {
    function Parser(_lexer) {
        this.lexer = _lexer;
    }
    Parser.prototype.Parse = function () {
        var recipe = new Recipe_1.Recipe();
        var lexer_fields = this.lexer.fields;
        recipe.title = lexer_fields[Lexer_1.TOKEN["title:"]].join(" ");
        recipe.subtitle = lexer_fields[Lexer_1.TOKEN["subtitle:"]].join(" ");
        recipe.for = Number.parseInt(lexer_fields[Lexer_1.TOKEN["for:"]][0]);
        for (var _i = 0, _a = lexer_fields[Lexer_1.TOKEN["ingredients:"]]; _i < _a.length; _i++) {
            var ingredient = _a[_i];
            recipe.ingredients.push(new Ingredient_1.Ingredient(ingredient));
        }
        recipe.procedure = this.ParseProcedure(lexer_fields[Lexer_1.TOKEN["procedure:"]], recipe);
        return recipe;
    };
    /* TODO */
    Parser.prototype.ParseProcedure = function (procedure, recipe) {
        var procedureArray = [];
        var procedureIndex = 0;
        var current_line = procedure[procedureIndex];
        var get_next = function () {
            current_line = procedure[++procedureIndex];
            return current_line;
        };
        while (procedureIndex < procedure.length) {
            /* Give each procedure the ability to get the next line */
            /* It can (for the time being) be assumed that each
             * procedure is constructed on a 'with(...)' statement,
             * and takes all following lines appropriately */
            procedureArray.push(new Procedure_1.Procedure(current_line, recipe.ingredients, get_next));
        }
        return procedureArray;
    };
    return Parser;
}());
exports.Parser = Parser;
//# sourceMappingURL=Parser.js.map