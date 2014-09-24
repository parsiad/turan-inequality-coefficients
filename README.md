turan-inequality-coefficients
=============================

It is nontrivial to show that the power-series

$${}_2F_1(2,-m+1;S-m+2;-x){}^2 - {}_2F_1(1,-m;S-m+1;-x) {}_2F_1(3,-m+2;S-m+3;-x) = \sum_{n=0}^{\infty} \varphi_n x^n$$

for integers $m$ and $S$ with $1 \leq m \leq S$ has nonnegative coefficients.

Instead, we do this algebraically for a range of $S$ values.
The power series coefficients are computed precisely using GNU MP.
Pochhammer symbols appearing in ${}_2F_1$ are cached to speed up computation, along with the use of multi-threading.

usage: coefficients S_MIN S_MAX
