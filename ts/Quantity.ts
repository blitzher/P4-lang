class QuantityError extends Error {}

enum QuantityType {
  Liquid,
  Weight,
  Number,
}

// The
export class Quantity {
  static LiquidTypes = ["ml", "l", "oz", "cups"];
  static WeightTypes = ["g", "kg", "lbs"];

  amount: number;
  type: QuantityType;
  localizationable: boolean;
  scaleable: boolean;

  /* Take a string, such as "1000ml"
   * and create the relevant object */
  constructor(quantity: string) {
    const match = quantity.match(/(\d+)(\w+)?/);

    if (!match) {
      throw new QuantityError(`Invalid quantity amount: ${quantity}!`);
    }

    const type_s = match.length > 2 ? match[2] : "number";

    /* Determine if the quantity can be localized differently */
    const localizationableMatch = quantity.match(/^\(\d+(\w+)?\)/);
    if (localizationableMatch) {
      this.localizationable = true;
    }

    /* If there is a unit */
    if (Quantity.LiquidTypes.includes(type_s)) {
      this.type = QuantityType.Liquid;
    } else if (Quantity.WeightTypes.includes(type_s)) {
      this.type = QuantityType.Weight;
    } else {
      this.type = QuantityType.Number;
    }

    this.amount = Number.parseFloat(match[1]);
  }

  /**
   * Scale to a different unit of the same type
   * @param targetUnit
   */
  scaleTo(targetUnit: string) {}
}
