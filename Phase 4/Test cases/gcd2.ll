Program
..Id(gcd2)
..VarPart
....VarDecl
......Id(x)
......Id(y)
......Id(integer)
..FuncPart
....FuncDef
......Id(gcd)
......FuncParams
........Params
..........Id(A)
..........Id(B)
..........Id(integer)
......Id(integer)
......VarPart
......IfElse
........Eq
..........Id(A)
..........IntLit(0)
........Assign
..........Id(gcd)
..........Id(B)
........StatList
..........While
............Gt
..............Id(B)
..............IntLit(0)
............IfElse
..............Gt
................Id(A)
................Id(B)
..............Assign
................Id(A)
................Sub
..................Id(A)
..................Id(B)
..............Assign
................Id(B)
................Sub
..................Id(B)
..................Id(A)
..........Assign
............Id(gcd)
............Id(A)
..IfElse
....Geq
......Id(paramcount)
......IntLit(2)
....StatList
......ValParam
........IntLit(1)
........Id(x)
......ValParam
........IntLit(2)
........Id(y)
......WriteLn
........Call
..........Id(gcd)
..........Id(x)
..........Id(y)
....WriteLn
......String('Error: two parameters required.')
