"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.Procedure = void 0;
var Procedure = /** @class */ (function () {
    /**
     *
     */
    function Procedure(procedure, recipeIngredients, get_next) {
        /* RegEx is readable (Copium)
        Use
        https://regex101.com/       and
        https://www.debuggex.com/
        for debugging
        */
        var procedureDestructure = /with\(((?:[\(\w+\)]+[,\s]*)+)\):/;
        var ingredientDestructure = /(\w+)\((\w+)\)/g; /* 'g' flag for multiple */
        var procedureInternalMatch = procedure.match(procedureDestructure);
        if (!procedureInternalMatch) {
            throw new SyntaxError("Invalid procedure header ".concat(procedure));
        }
        /* Get the first matching group, i.e. the ingredients and their quantifiers */
        var procedureInternal = procedureInternalMatch[1];
        /* Destructure the ingredients into atoms */
        var procedureIngredients = Array.from(procedureInternal.matchAll(ingredientDestructure));
        procedureIngredients.forEach(function (_a) {
            var _ = _a[0], quantifier = _a[1], ingredient = _a[2];
            console.log(quantifier, ingredient);
        });
    }
    return Procedure;
}());
exports.Procedure = Procedure;
//# sourceMappingURL=Procedure.js.map