$$
\begin{align}
    [\text{Prog}] &\to [\text{Stmt}]^* \\
    [\text{Stmt}] &\to
    \begin{cases}
        \text{exit}([\text{Expr}]); \\
        \text{let}\space\text{ident} = [\text{Expr}]; \\
        \text{if([Expr])[Scope][IfPred]}\\
        \text{[Scope]}
    \end{cases} \\
    [\text{Scope}] &\to
    \begin{cases}
        \text{\{[Stmt]\}*}
    \end{cases} \\
    [\text{IfPred}] &\to
    \begin{cases}
        \text{elif}(\text{[Expr]})\text{[Scope][IfPred]} \\
        \text{[Else]}\\
        \epsilon
    \end{cases} \\
    [\text{Else}] &\to
    \begin{cases}
        \text{else}(\text{[Expr]})\text{[Scope]} \\
        \epsilon
    \end{cases}\\
    [\text{Expr}] &\to
    \begin{cases}
        [\text{Term}] \\
        [\text{NodeBinExpr}]
    \end{cases} \\
    [\text{BinExpr}] &\to
    \begin{cases}
        [\text{Expr}] * [\text{Expr}] & \text{prec} = 1 \\
        [\text{Expr}] + [\text{Expr}] & \text{prec} = 0 \\
    \end{cases} \\
    [\text{Term}] &\to
    \begin{cases}
        \text{int\_lit} \\
        \text{ident} \\
        \text{([Expr])}
    \end{cases}
\end{align}
$$
