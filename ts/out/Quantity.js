"use strict";
var __extends = (this && this.__extends) || (function () {
    var extendStatics = function (d, b) {
        extendStatics = Object.setPrototypeOf ||
            ({ __proto__: [] } instanceof Array && function (d, b) { d.__proto__ = b; }) ||
            function (d, b) { for (var p in b) if (Object.prototype.hasOwnProperty.call(b, p)) d[p] = b[p]; };
        return extendStatics(d, b);
    };
    return function (d, b) {
        if (typeof b !== "function" && b !== null)
            throw new TypeError("Class extends value " + String(b) + " is not a constructor or null");
        extendStatics(d, b);
        function __() { this.constructor = d; }
        d.prototype = b === null ? Object.create(b) : (__.prototype = b.prototype, new __());
    };
})();
Object.defineProperty(exports, "__esModule", { value: true });
exports.Quantity = void 0;
var QuantityError = /** @class */ (function (_super) {
    __extends(QuantityError, _super);
    function QuantityError() {
        return _super !== null && _super.apply(this, arguments) || this;
    }
    return QuantityError;
}(Error));
var QuantityType;
(function (QuantityType) {
    QuantityType[QuantityType["Liquid"] = 0] = "Liquid";
    QuantityType[QuantityType["Weight"] = 1] = "Weight";
    QuantityType[QuantityType["Number"] = 2] = "Number";
})(QuantityType || (QuantityType = {}));
// The
var Quantity = /** @class */ (function () {
    /* Take a string, such as "1000ml"
     * and create the relevant object */
    function Quantity(quantity) {
        var match = quantity.match(/(\d+)(\w+)?/);
        if (!match) {
            throw new QuantityError("Invalid quantity amount: ".concat(quantity, "!"));
        }
        var type_s = match.length > 2 ? match[2] : "number";
        /* Determine if the quantity can be localized differently */
        var localizationableMatch = quantity.match(/^\(\d+(\w+)?\)/);
        if (localizationableMatch) {
            this.localizationable = true;
        }
        /* If there is a unit */
        if (Quantity.LiquidTypes.includes(type_s)) {
            this.type = QuantityType.Liquid;
        }
        else if (Quantity.WeightTypes.includes(type_s)) {
            this.type = QuantityType.Weight;
        }
        else {
            this.type = QuantityType.Number;
        }
        this.amount = Number.parseFloat(match[1]);
    }
    /**
     * Scale to a different unit of the same type
     * @param targetUnit
     */
    Quantity.prototype.scaleTo = function (targetUnit) { };
    Quantity.LiquidTypes = ["ml", "l", "oz", "cups"];
    Quantity.WeightTypes = ["g", "kg", "lbs"];
    return Quantity;
}());
exports.Quantity = Quantity;
//# sourceMappingURL=Quantity.js.map