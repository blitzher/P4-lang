"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.Lexer = exports.TOKEN = void 0;
var fs = require("fs");
var TOKEN;
(function (TOKEN) {
    TOKEN[TOKEN["title:"] = 0] = "title:";
    TOKEN[TOKEN["subtitle:"] = 1] = "subtitle:";
    TOKEN[TOKEN["for:"] = 2] = "for:";
    TOKEN[TOKEN["ingredients:"] = 3] = "ingredients:";
    TOKEN[TOKEN["procedure:"] = 4] = "procedure:";
})(TOKEN = exports.TOKEN || (exports.TOKEN = {}));
var Lexer = /** @class */ (function () {
    function Lexer(filename) {
        this.current_token = "unfielded";
        this.fields = {};
        var raw_file = fs.readFileSync(filename).toString();
        for (var _i = 0, _a = raw_file.split("\n"); _i < _a.length; _i++) {
            var line = _a[_i];
            this.LexLine(line);
        }
    }
    Lexer.prototype.LexLine = function (line) {
        var line_split = line.trim().split(" ");
        if (line[0] == "" && line.length == 1) {
            return;
        }
        if (line_split[0] in TOKEN) {
            this.current_token = line_split[0];
            line_split = line_split.splice(1); /* Remove the first element */
        }
        if (this.fields[TOKEN[this.current_token]] == undefined) {
            this.fields[TOKEN[this.current_token]] = [];
        }
        var joined = line_split.join(" ");
        if (joined.length > 0) {
            this.fields[TOKEN[this.current_token]].push(joined);
        }
    };
    return Lexer;
}());
exports.Lexer = Lexer;
//# sourceMappingURL=Lexer.js.map