## Grammar

- `program` ::= `statement*`
- `statement` ::= `declaration` | `assignment` | `conditional` | `for_loop` | `print` | `function` | `return`
- `declaration` ::= "`let`" `identifier` "`;`" | "`let`" `identifier`  "=" `expression` "`;`"
- `assignment` ::= `identifier` "`=`" (`expression` | `string`) "`;`"
- `expression` ::= `term` `(("+" | "-") term)*`
- `term` ::= `factor` `(("*" | "/") factor)*`
- `factor` ::= `identifier` | `number` | `string` | "`(`" `expression` "`)`"
- `conditional` ::= `if_statement` | `if_else_statement`
- `for_loop` ::= "`for`" "`(`" `assignment` `expression` "`;`" `assignment` "`)`" "`{`" `statement*` "`}`"
- `if_statement` ::= "`if`" "`(`" `comparison` "`)`" "`{`" `statement*` "`}`"
- `if_else_statement` ::= "`if`" "`(`" `comparison` "`)`" "`{`" `statement*` "`}`" "`else`" "`{`" `statement*` "`}`"
- `print` ::= "`print`" "`(`" `expression` "`)`"
- `function` ::= "`fn`" `identifier` "`(`" `parameters?` "`)`" "`{`" `statement*` "`}`" "`;`"?
- `parameters` ::= `identifier` ("`,`" `identifier`)*
- `return` ::= "`return`" `expression` "`;`"
- `call` ::= `identifier` "`(`" `arguments?` "`)`"
- `arguments` ::= `expression` ("`,`" `expression`)*
- `comparison` ::= `expression` `comp_op` `expression`
- `comp_op` ::= "`==`" | "`!=`" | "`<`" | "`<=`" | "`>`" | "`>=`"
- `identifier` ::= `letter` `(letter | digit)*`
- `number` ::= `digit+`
- `string` ::= `"` (any character) `"`
- `boolean` ::= "`true`" | "`false`"
- `letter` ::= "`a`" | "`b`" | ... | "`z`" | "`A`" | ... | "`Z`"
- `digit` ::= "`0`" | "`1`" | ... | "`9`"


