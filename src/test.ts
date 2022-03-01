
interface stream {
    advance: () => any; peek: () => any;
}
const id = 0;

function LL2Parser(ts: stream) {
    var current = ts.advance();
    var upcoming = ts.peek();

    if (peek(2) == [a, b] == current == a && upcoming == b)

        switch (current) {
            case id:
                switch (upcoming) {
                    case ';': /* Final terminator */
                    case '(': /* Encountered 2 */
                    case ',': /* Encountered 4 */
                    case ')': /* Back into 2 */
                    default: /* Syntax error */
                }
            case '(':
                switch (upcoming) {
                    case id: /*encountered 3/4*/
                    default: /* Syntax error */
                }
            case ')':
                switch (upcoming) {
                    case ';': /* Final terminator */
                    default: /* Syntax error */
                }
            case ',':
                switch (upcoming) {
                    case id: /* Encountered 4 */
                    default: /* Syntax error */
                }
            case ';':
                switch (upcoming) {
                    default: /* Syntax error if not EOF */
                }
        }
}