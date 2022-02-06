"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.Ingredient = void 0;
var Quantity_1 = require("./Quantity");
var Ingredient = /** @class */ (function () {
    // Take a string, such as "2 eggs"
    // and create the relevant object
    function Ingredient(ingredient_s) {
        var _a = ingredient_s.trim().split(" "), amount_s = _a[0], type = _a[1];
        this.type = type;
        this.amount = new Quantity_1.Quantity(amount_s);
    }
    return Ingredient;
}());
exports.Ingredient = Ingredient;
//# sourceMappingURL=Ingredient.js.map