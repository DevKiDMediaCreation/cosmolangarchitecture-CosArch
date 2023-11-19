$$
\begin{align}
    [\text{Prog}] &\to [\text{Stmt}]^* \\
    [\text{Stmt}] &\to
    \begin{cases}
        \text{exit}(\text{[Ident]}); \\
        \text{let ident} = [\text{Expr}]
    \end{cases}\\
[Expr] &\to
 \begin{cases}
        \text{int\_lit} \\
        \text{ident} \\
        \text{[BinExpr]}
    \end{cases}\\

\text{[BinExpr]} &\to
\begin{cases}
\text{[Expr]} * \text{[Expr]} & prec=1\\

\text{[Expr]} + \text{[Expr]} & prec= 0 \\
\end{cases}
\end{align}
$$
