"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
var Lexer_1 = require("./Lexer");
var Parser_1 = require("./Parser");
function main() {
    var lexer = new Lexer_1.Lexer("../tests/eggs.rcp");
    var parser = new Parser_1.Parser(lexer);
    var recipe = parser.Parse();
    return;
}
main();
//# sourceMappingURL=index.js.map