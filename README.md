# Some toy map tweaks

|     Group      |   Experiment    |   Prob. Space   |     Samples     |   Iterations    |    Baseline     |  us/Iteration   | Iterations/sec  |
|:--------------:|:---------------:|:---------------:|:---------------:|:---------------:|:---------------:|:---------------:|:---------------:|
|Init            | NaiveMap        |             256 |              30 |            2048 |         1.00000 |       157.41650 |         6352.57 |
|Init            | NaiveMap        |             512 |              30 |             256 |         1.00000 |       318.73047 |         3137.45 |
|Init            | NaiveMap        |            1024 |              30 |             256 |         1.00000 |       642.86328 |         1555.54 |
|Init            | NaiveMap        |            2048 |              30 |             256 |         1.00000 |      1299.22266 |          769.69 |
|Init            | NaiveMap        |            4096 |              30 |             256 |         1.00000 |      2664.15234 |          375.35 |
|Find            | NaiveMap        |             256 |              30 |            2048 |         1.00000 |         5.42822 |       184222.36 |
|Find            | NaiveMap        |             512 |              30 |             256 |         1.00000 |        20.82422 |        48021.01 |
|Find            | NaiveMap        |            1024 |              30 |             256 |         1.00000 |        52.58594 |        19016.49 |
|Find            | NaiveMap        |            2048 |              30 |             256 |         1.00000 |       128.28906 |         7794.90 |
|Find            | NaiveMap        |            4096 |              30 |             256 |         1.00000 |       318.56250 |         3139.10 |
|Init            | Arena           |             256 |              30 |            2048 |         0.67952 |       106.96777 |         9348.61 |
|Init            | Arena           |             512 |              30 |             256 |         0.71259 |       227.12500 |         4402.86 |
|Init            | Arena           |            1024 |              30 |             256 |         0.78416 |       504.10938 |         1983.70 |
|Init            | Arena           |            2048 |              30 |             256 |         0.94246 |      1224.46094 |          816.69 |
|Init            | Arena           |            4096 |              30 |             256 |         1.30657 |      3480.90234 |          287.28 |
|Find            | Arena           |             256 |              30 |            2048 |         0.70379 |         3.82031 |       261758.69 |
|Find            | Arena           |             512 |              30 |             256 |         0.88764 |        18.48438 |        54099.75 |
|Find            | Arena           |            1024 |              30 |             256 |         0.90625 |        47.65625 |        20983.61 |
|Find            | Arena           |            2048 |              30 |             256 |         0.90835 |       116.53125 |         8581.39 |
|Find            | Arena           |            4096 |              30 |             256 |         0.82639 |       263.25781 |         3798.56 |
