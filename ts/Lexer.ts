import * as fs from "fs";

export enum TOKEN {
  "title:",
  "subtitle:",
  "for:",
  "ingredients:",
  "procedure:",
}

export class Lexer {
  current_token?: TOKEN | "unfielded" = "unfielded";
  fields: { [key: string]: string[] } = {};

  constructor(filename: string) {
    const raw_file = fs.readFileSync(filename).toString();

    for (const line of raw_file.split("\n")) {
      this.LexLine(line);
    }
  }

  LexLine(line: string) {
    let line_split = line.trim().split(" ");

    if (line[0] == "" && line.length == 1) {
      return;
    }

    if (line_split[0] in TOKEN) {
      this.current_token = <any>line_split[0];
      line_split = line_split.splice(1); /* Remove the first element */
    }

    if (this.fields[TOKEN[this.current_token]] == undefined) {
      this.fields[TOKEN[this.current_token]] = [];
    }

    const joined = line_split.join(" ");

    if (joined.length > 0) {
      this.fields[TOKEN[this.current_token]].push(joined);
    }
  }
}
