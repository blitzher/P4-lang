class QuantityError extends Error {}

enum QuantityType {
  Liquid,
  Weight,
  Number,
}

// The
class Quantity {
  static LiquidTypes = ["ml", "l", "oz", "cups"];
  static WeightTypes = ["g", "kg", "lbs"];

  amount: number;
  type: QuantityType;

  /* Take a string, such as "1000ml"
   * and create the relevant object */
  constructor(quantity: string) {
    const match = quantity.match(/\d+(\w+)?/);

    if (!match) {
      throw new QuantityError(`Invalid quantity amount: ${quantity}!`);
    }

    /* If there is a unit */
    if (match.length > 1) {
      if (match[1] in Quantity.LiquidTypes) {
        this.type = QuantityType.Liquid;
      } else if (match[1] in Quantity.WeightTypes) {
        this.type = QuantityType.Weight;
      }
    } else {
      this.type = QuantityType.Number;
    }
    this.amount = Number.parseFloat(match[0]);
  }

  /**
   * Scale to a different unit of the same type
   * @param targetUnit
   */
  scaleTo(targetUnit: string) {}
}
