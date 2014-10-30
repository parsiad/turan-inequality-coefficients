turan-inequality-coefficients
=============================

It is nontrivial to show that for integers ``m`` and ``S`` with ``1 <= m <= S``, the power-series

![turan-inequality-coefficients power_series](https://raw.githubusercontent.com/parsiad/turan-inequality-coefficients/gh-pages/power_series.png "Power Series")

has nonnegative coefficients.

Instead, we do this algebraically for a range of ``S`` values.
The power series coefficients are computed precisely using GNU MP.
Pochhammer symbols appearing in ``${}_2F_1$`` are cached to speed up computation, along with the use of multi-threading.

Usage
-----

Run ``make`` to compile. Then, run

``./coefficients S_MIN S_MAX > log_file.txt``

where ``S_MIN`` and ``S_MAX`` are the lower and upper bounds for ``S``, respectively.
