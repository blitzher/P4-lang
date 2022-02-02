var fs = require("fs");
var Recipe = /** @class */ (function () {
    function Recipe(filename) {
        fs.readFileSync(filename);
    }
    return Recipe;
}());
//# sourceMappingURL=Recipe.js.map