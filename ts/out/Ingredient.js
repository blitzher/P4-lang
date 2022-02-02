var Ingredient = /** @class */ (function () {
    // Take a string, such as "2 eggs"
    // and create the relevant object
    function Ingredient(ingredient_s) {
        var _a = ingredient_s.trim().split(" "), amount_s = _a[0], type = _a[1];
        this.type = type;
        this.amount = new Quantity(amount_s);
    }
    return Ingredient;
}());
//# sourceMappingURL=Ingredient.js.map